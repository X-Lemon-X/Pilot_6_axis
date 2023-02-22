#include <Arduino.h>
#include "IO_Control.h"
#include "PassGenerator.h"
#include "OledPrintLib.h"
#include "SendData.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <esp_wifi.h>
#include "esp32-hal-cpu.h"
#include "SetingsPage.h"
#include "Setings.h"

//#include <EEPROM.h>   // https://randomnerdtutorials.com/esp32-flash-memory/
//#include <Preferences.h>  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/

//for use of dual cores refere to https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

//https://microcontrollerslab.com/esp32-esp8266-web-server-input-data-html-forms/



//------------DEBUG

//#define DEBUG

#define print(x) Serial.println(x);

//-----------------------------INFO
#define VERSION 1.4

//-----------------------------JOYSTICKS PINOUT
#define PIN_X_ROT_JOYSTIK_1 36   // 13 -> adc1:36  VP
#define PIN_Y_ROT_JOYSTIK_1 39   // 14 -> adc1:39  VN
#define PIN_Z_ROT_JOYSTIK_1 34   // 27 -> adc1:34

#define PIN_X_ROT_JOYSTIK_2 35   // 26 -> adc1:35
#define PIN_Y_ROT_JOYSTIK_2 32   // 25 -> adc1:32
#define PIN_Z_ROT_JOYSTIK_2 33   // --  -> adc1:33

#define PIN_BTN_JOYSTIK_1 4
#define PIN_BTN_JOYSTIK_2 19


//-----------------------------BTNs PINOUT
#define PIN_MS_BTN_1 2
#define PIN_MS_BTN_2 0
#define PIN_MS_BTN_3 16
#define PIN_MS_BTN_4 17
#define PIN_MS_BTN_5 5
#define PIN_MS_BTN_6 18
#define PIN_MS_BTN_7 23
#define PIN_MS_BTN_8 15

//-----------------------------ADDITIONAL PINOUT
#define PIN_BATTERY_VOLTAGE

//-----------------------------OLED DISPLAY
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define WIRE Wire
#define OLED_RESET 4

#define DISPLAY_MAX_LINE_COUNT 7
#define DISPLAY_MAX_CHARACTER_COUNT 21

//-----------------------------WIFI_SETUP
#define WIFI_SETINGS_BEG_NAME "str_WIFI_"
#define WIFI_CONNECTION_TRY_MAX_COUNT 10
#define WIFI_PASSWORD_LENGTH 8
#define TIME_BASE_DEFAULT 1000000 // us

const char* WIFI_NAME = "remocon";
uint8_t newMACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
const char* ssid     = "Remote_Controler_4D";
std::string WIFIpasssword="123456789";

using namespace IO_Control;

struct InputsData
{
  int joystick_1_x=0;
  int joystick_1_y=0;
  int joystick_1_z=0;
  int joystick_1_btn=0;

  int joystick_2_x=0;
  int joystick_2_y=0;
  int joystick_2_z=0;
  int joystick_2_btn=0;

  int btn_1 = 0;
  int btn_2 = 0;
  int btn_3 = 0;
  int btn_4 = 0;
  int btn_5 = 0;
  int btn_6 = 0;
  int btn_7 = 0;
  int btn_8 = 0;
};

#pragma region //Global Veriables | objects

Adafruit_SSD1306 display;
//WebServer server(80);
AsyncWebServer *server;
IO_Control::FourAxisJoystick joystick1;
IO_Control::FourAxisJoystick joystick2;
InputsData inputs_main;
OledPrintLib *oledPrint = NULL;
SendDataProtocol::Send_WIFI_UDP *sendUdp = NULL;
Setings::Setings setings_data;

TaskHandle_t FirstLoop;
TaskHandle_t SecondLoop;


#pragma endregion

#pragma region // Function Declarations

void ReadingInputs();
void MainLoopFunction(void * pvParameters);
void SecondLoopFunction(void * pvParameters);
string ConvertInputDataToMessage(InputsData inputs);

#pragma endregion

#pragma region //OledPrintLib shorted functions

void Print(String string)
{
  #ifdef DEBUG
    print(string);
  #endif
  oledPrint->Print(string);
}

void Println(String string)
{
  #ifdef DEBUG
    print(string);
  #endif
  oledPrint->Println(string);
}

void UpdateLine(String string)
{
  oledPrint->UpdateLine(string);
}

void UpdateLine(String string, int line)
{
  oledPrint->UpdateLine(string,line);
}

void PrintSetings(Setings::Setings setings)
{
  Println("Setings:");
  Println("ver:");
  Println("");
  Println("");
  for(auto element=setings._setings.begin(); element != setings._setings.end(); element++ )
  {
    UpdateLine(String(element->first.c_str()),1);
    UpdateLine(PrintSeting(element->second));
  }
}

#pragma endregion

#pragma region //WebSiteSetup site itp don't look  

void PageNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void InitAllWebEvents()
{
  #ifndef DEBUG
  WIFIpasssword = PassGenerator_GeneratePassword(WIFI_PASSWORD_LENGTH);
  #endif

  server = new AsyncWebServer(80);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, WIFIpasssword.c_str());

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    std::string val_table_mid = "";
    for (auto element = setings_data._setings.begin(); element!= setings_data._setings.end(); ++element)
    {
      val_table_mid = val_table_mid + "\n <tr><td>" + element->first + "</td> <td><input type=\"text\" name=\"" 
      + element->first+ "\" value=\""+ Setings::GetStringFromSeting(element->second)+"\"></td> </tr>"; 
    }
    std::string val_page = string(MAIN_page_beg) + val_table_mid.c_str() + string(MAIN_page_end);  

    request->send_P(200, "text/html", val_page.c_str());
  });


  server->on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    int errors_count=0; 
    for (size_t i = 0; i < request->params(); i++)
    {
      int err ;
      Println(request->getParam(i)->name().c_str());
      Println(request->getParam(i)->value().c_str());
      err = setings_data.UpdateSeting(request->getParam(i)->name().c_str(), request->getParam(i)->value().c_str());
      Println("code:" + String(err));
      errors_count +=err;
    }
   std::string val_page = "Setings [" +to_string(request->params())+ "] send to be updated, not updated [" + 
   to_string(errors_count) + "] <br><a href=\"/\">Return to Setings Page</a>";
   setings_data.SaveSetingsToFlash();
   request->send(200, "text/html", val_page.c_str());
  });


  server->onNotFound(PageNotFound);
  server->begin();
}

bool FindWifiNetwork(String ssid, String *password, int *num)
{
  for(auto elemnt=setings_data._setings.begin(); elemnt!=setings_data._setings.end(); elemnt++ )
  {
    std::string beg = elemnt->first ;
    beg.resize(9);
    if(beg ==  WIFI_SETINGS_BEG_NAME)
    {
      //Println("Name:" + String(elemnt->first.c_str()) + " val:" + String(elemnt->second.data._string));
      *num = std::stoi(elemnt->first.substr(9,1));
      std::string type = elemnt->first.substr(elemnt->first.size()-1,1);
      if(type == "S" && string(elemnt->second.data._string) == string(ssid.c_str()))
      {
        std::string name = elemnt->first.substr(0,11) + "P";
        auto ss = setings_data._setings[name];
        *password = String(ss.data._string);
        return true;
      }
    }
  }
  return false;
}

void ConnectWithAvailableWIfiNetwork()
{
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  WiFi.disconnect();
  bool notConnected=true;

  String wifi[10][2];
  bool available[10];

  for (size_t i = 0; i < 10; i++)
  {
    available[i]=false;
    wifi[i][0]="";
    wifi[i][1]="";
  }
  
  while (notConnected)
  {
    int i = WiFi.scanNetworks();
    if(i>0)
    {
      for (int count = 0; count < i; count++)
      {        
        String passwd;
        String ssid= WiFi.SSID(count);
        Println("CH?:"+ssid);
        int num=0;
        if(FindWifiNetwork(ssid,&passwd,&num))
        {  
          Println("Available");
          available[num]= true;
          wifi[num][0] = ssid;
          wifi[num][1] = passwd;
        }
      } 

      for (size_t i = 0; i < 10; i++)
      {
        //Println("CH? num"  + String(i));
        if(available[i])
        {
          Println("sd?:"+wifi[i][0]);
          //Println("pass:"+wifi[i][1]);
          WiFi.begin(wifi[i][0].c_str(), wifi[i][1].c_str());
          int connectionTryCount=0;
          
          Println("");
          while (WiFi.status() != WL_CONNECTED) {
            Print(".");
            delay(100);
            if(WIFI_CONNECTION_TRY_MAX_COUNT == connectionTryCount++) break;
          }
          
          if(WiFi.status() == WL_CONNECTED){
            Println("Connected to:");
            Println(wifi[i][0]);
            Println(WiFi.localIP().toString());
            notConnected = false;
            break;
          }
        }
      }
    }
    delay(50);
  }
}

wl_status_t ReconnectWithWifiIfConenctionLost(wl_status_t prevStatus)
{
  wl_status_t status =  WiFi.status();
  if(status!= prevStatus )
  {
    if(status != WL_CONNECTED)
    { 
      Println("disconnected");
      ConnectWithAvailableWIfiNetwork();
    }
  }
  return status;
}

#pragma endregion

#pragma region //Initiating Microcontroler 

void LoadSetings()
{

  #pragma region Joysticks
  setings_data.AddSeting("int_Joystick_left_MMin",-2048);
  setings_data.AddSeting("int_Joystick_left_MMax",2048);

  setings_data.AddSeting("int_Joystick_right_MMin",-2048);
  setings_data.AddSeting("int_Joystick_right_MMax",2048);

  setings_data.AddSeting("int_Joystick_right_filer",0.95f);
  setings_data.AddSeting("int_Joystick_left_filer",0.95f);

  #pragma endregion

  #pragma region Buttons

  setings_data.AddSeting("bool_Btns_Mstable",string("0,0,0,0,0,0,0,0"));

  #pragma endregion

  #pragma region WIFI

  setings_data.AddSeting("str_host_wifi",string("192.168.1.17"));
  setings_data.AddSeting("int_host_port",25000);
  setings_data.AddSeting("int_host_RefRate",1000);
  setings_data.AddSeting("str_host_password",string("qwerty"));

  setings_data.AddSeting("str_WIFI_1_S",string("ForeverWIFI"));
  setings_data.AddSeting("str_WIFI_1_P",string("6TTZQWQ67NR9"));

  setings_data.AddSeting("str_WIFI_2_S",string("400%mocy"));
  setings_data.AddSeting("str_WIFI_2_P",string("qwerty987654321"));

  setings_data.AddSeting("str_WIFI_3_S",string(""));
  setings_data.AddSeting("str_WIFI_3_P",string(""));
  
  setings_data.AddSeting("str_WIFI_4_S",string(""));
  setings_data.AddSeting("str_WIFI_4_P",string(""));

  setings_data.AddSeting("str_WIFI_5_S",string(""));
  setings_data.AddSeting("str_WIFI_5_P",string(""));

  setings_data.AddSeting("str_WIFI_6_S",string(""));
  setings_data.AddSeting("str_WIFI_6_P",string(""));

  #pragma endregion

  setings_data.InitEPPROM();
  //setings_data.SaveSetingsToFlash();
  setings_data.LoadSetingsFromFlash();
}

void GPIOinit()
{

  pinMode(PIN_MS_BTN_1, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_2, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_3, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_4, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_5, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_6, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_7, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_8, INPUT_PULLUP);

  pinMode(PIN_BTN_JOYSTIK_1,INPUT_PULLUP);
  pinMode(PIN_BTN_JOYSTIK_2,INPUT_PULLUP);

  pinMode(PIN_X_ROT_JOYSTIK_1, INPUT);
  pinMode(PIN_Y_ROT_JOYSTIK_1, INPUT);
  pinMode(PIN_Z_ROT_JOYSTIK_1, INPUT);

  pinMode(PIN_X_ROT_JOYSTIK_2, INPUT);
  pinMode(PIN_Y_ROT_JOYSTIK_2, INPUT);
  pinMode(PIN_Z_ROT_JOYSTIK_2, INPUT);

  //joystick1.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_1, PIN_Y_ROT_JOYSTIK_1, PIN_Z_ROT_JOYSTIK_1, PIN_BTN_JOYSTIK_1, 0.80f);
  //joystick2.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_2, PIN_Y_ROT_JOYSTIK_2, PIN_Z_ROT_JOYSTIK_2, PIN_BTN_JOYSTIK_2, 0.95f);
  
  joystick1.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_1, PIN_Y_ROT_JOYSTIK_1, PIN_Z_ROT_JOYSTIK_1, PIN_BTN_JOYSTIK_1,
    setings_data.GetSeting("int_Joystick_left_filer").data._float,
    setings_data.GetSeting("int_Joystick_left_MMin").data._int,
    setings_data.GetSeting("int_Joystick_left_MMax").data._int);

  joystick2.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_2, PIN_Y_ROT_JOYSTIK_2, PIN_Z_ROT_JOYSTIK_2, PIN_BTN_JOYSTIK_2,
    setings_data.GetSeting("int_Joystick_right_filer").data._float,
    setings_data.GetSeting("int_Joystick_right_MMin").data._int,
    setings_data.GetSeting("int_Joystick_right_MMax").data._int);
  
  joystick1.AutoZero();
  joystick2.AutoZero();

  #ifdef DEBUG
    print("GPIO initiated");
  #endif
}

void DisplayInit()
{
  #ifdef DEBUG
     print("Display ... new TerminalLike oled ...")
  #endif
  //Wire.setClock(100000);

  display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  { 
    #ifdef DEBUG
    Serial.println("SSD1306 allocation failed");
    #endif
    exit(1);
  }

  display.display();
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.display();

  oledPrint = new OledPrintLib(&display,DISPLAY_MAX_LINE_COUNT,DISPLAY_MAX_CHARACTER_COUNT);

  #ifdef DEBUG
    print("Display Initiated new TerminalLike oled initiate");
  #endif

}

void InitSetupMode()
{
  InitAllWebEvents();
  Println("--Setup mode--");
  Println("sd:");
  Print(String(ssid));
  Println("pw:");
  Print(String(WIFIpasssword.c_str()));
  Println("http://192.168.4.1");
  
}

void InitNormalMode()
{
  //ConnectWithAvailableWIfiNetwork();

  sendUdp = new SendDataProtocol::Send_WIFI_UDP(setings_data.GetSeting("str_host_wifi").data._string,setings_data.GetSeting("int_host_port").data._int);

  #ifdef DEBUG
    Println(setings_data.GetSeting("str_host_wifi").data._string);
    Println(to_string(setings_data.GetSeting("int_host_port").data._int).c_str());
    Println(sendUdp->GetIp().c_str());
  #endif

  int core = xPortGetCoreID(); 
  if(core == 1) core = 0;
  else core = 1;

  MainLoopFunction(NULL);
 // xTaskCreatePinnedToCore(SecondLoopFunction, "second", 10000, NULL, 1, &SecondLoop, core);        
 // xTaskCreatePinnedToCore(MainLoopFunction, "first", 10000, NULL, 0, &FirstLoop, xPortGetCoreID());          
}

#pragma endregion

void setup() {

  Serial.begin(115200);
  DisplayInit(); //have to be first
  LoadSetings(); //load setings
  GPIOinit(); //init GPIO
  PrintSetings(setings_data);

  if(!InOut::ReadInput(PIN_MS_BTN_4) || !InOut::ReadInput(PIN_MS_BTN_5)) InitSetupMode();
  else InitNormalMode();
}

void loop(){}

//connection handler reconnectin handler reading input
void MainLoopFunction(void * pvParameters)
{
  wl_status_t statusPrev=WL_IDLE_STATUS;
  unsigned long timerMicrosSendData=0;
  unsigned long dalayMicros = (unsigned long)setings_data.GetSeting("int_host_RefRate").data._int;
  dalayMicros = TIME_BASE_DEFAULT/dalayMicros;  

  while(true)
  {
    statusPrev = ReconnectWithWifiIfConenctionLost(statusPrev);
    ReadingInputs();

    if(micros() - timerMicrosSendData > dalayMicros)
    {
      timerMicrosSendData = micros();
      sendUdp->SendData(ConvertInputDataToMessage(inputs_main));
    } 

    #ifdef DEBUG
      delay(10);
    #endif
  }
}

// reding io and updating output data
void SecondLoopFunction(void * pvParameters)
{
  while(true)
  {
    ReadingInputs();
  }
}

void ReadingInputs()
{
  inputs_main.joystick_1_x = joystick1.readX();
  inputs_main.joystick_1_y = joystick1.readY();
  inputs_main.joystick_1_z = joystick1.readZ();
  inputs_main.joystick_1_btn = joystick1.readBtn();

  inputs_main.joystick_2_x = joystick2.readX();
  inputs_main.joystick_2_y = joystick2.readY();
  inputs_main.joystick_2_z = joystick2.readZ();
  inputs_main.joystick_2_btn = joystick2.readBtn();

  inputs_main.btn_1 = InOut::ReadInput(PIN_MS_BTN_1);
  inputs_main.btn_2 = InOut::ReadInput(PIN_MS_BTN_2);
  inputs_main.btn_3 = InOut::ReadInput(PIN_MS_BTN_3);
  inputs_main.btn_4 = InOut::ReadInput(PIN_MS_BTN_4);
  inputs_main.btn_5 = InOut::ReadInput(PIN_MS_BTN_5);
  inputs_main.btn_6 = InOut::ReadInput(PIN_MS_BTN_6);
  inputs_main.btn_7 = InOut::ReadInput(PIN_MS_BTN_7);
  inputs_main.btn_8 = InOut::ReadInput(PIN_MS_BTN_8);
}

string ConvertInputDataToMessage(InputsData inputs)
{
  string buffor="#";
  buffor += ":X1 " + to_string(inputs.joystick_1_x);
  buffor += ":Y1 " + to_string(inputs.joystick_1_y);
  buffor += ":Z1 " + to_string(inputs.joystick_1_z);
  buffor += ":B1 " + to_string(inputs.joystick_1_btn);

  buffor += ":X2 " + to_string(inputs.joystick_2_x);
  buffor += ":Y2 " + to_string(inputs.joystick_2_y);
  buffor += ":Z2 " + to_string(inputs.joystick_2_z);
  buffor += ":B2 " + to_string(inputs.joystick_2_btn);

  buffor += ":H1 " + to_string(inputs.btn_1);
  buffor += ":H2 " + to_string(inputs.btn_2);
  buffor += ":H3 " + to_string(inputs.btn_3);
  buffor += ":H4 " + to_string(inputs.btn_4);
  buffor += ":H5 " + to_string(inputs.btn_5);
  buffor += ":H6 " + to_string(inputs.btn_6);
  buffor += ":H7 " + to_string(inputs.btn_7);
  buffor += ":H8 " + to_string(inputs.btn_8);
  buffor += ":@";
  return buffor;
}

