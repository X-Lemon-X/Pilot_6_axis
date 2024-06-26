#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <esp_wifi.h>

#include <list>
#include <string>
#include "esp32-hal-cpu.h"

#include "IO_Control.h"
#include "PassGenerator.h"
#include "SetingsPage.h"
#include "Setings.h"
#include "OledPrintLib.h"



//#include <EEPROM.h>   // https://randomnerdtutorials.com/esp32-flash-memory/
//#include <Preferences.h>  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/


//for use of dual cores refere to https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

//----------------------IMPORTANT----BUILDING

// #define SAVE_TO_FLASH  // only use once to save new setings to flash -> Flush ESP32 with it uncomented then Flush again without it //newer work with it uncommented or it will break EPPROM 
// #define DEBUG

//-----------------------------INFO
#define VERSION 2.5



//-----------------------------JOYSTICKS PINOUT
#define PIN_X_ROT_JOYSTIK_1 33
#define PIN_Y_ROT_JOYSTIK_1 32
#define PIN_Z_ROT_JOYSTIK_1 35

#define PIN_X_ROT_JOYSTIK_2 34
#define PIN_Y_ROT_JOYSTIK_2 39
#define PIN_Z_ROT_JOYSTIK_2 36

#define PIN_BTN_JOYSTIK_1 19
#define PIN_BTN_JOYSTIK_2 23


//-----------------------------BTNs PINOUT
#define PIN_MS_BTN_1 15
#define PIN_MS_BTN_2 2
#define PIN_MS_BTN_3 0
#define PIN_MS_BTN_4 4
#define PIN_MS_BTN_5 16
#define PIN_MS_BTN_6 17
#define PIN_MS_BTN_7 5
#define PIN_MS_BTN_8 18


//-----------------------------ADDITIONAL PINOUT
#define PIN_BATTERY_VOLTAGE

//--------------WIFIpasssword.c_str()---------------OLED DISPLAY
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define WIRE Wire
#define OLED_RESET -1 //4

#define DISPLAY_MAX_LINE_COUNT 7
#define DISPLAY_MAX_CHARACTER_COUNT 22
String displayLines[DISPLAY_MAX_LINE_COUNT];
int lineCount=0;


//-----------------------------WIFI_SETUP
#define WIFI_SETINGS_BEG_NAME "str_WIFI_"
#define WIFI_CONNECTION_TRY_MAX_COUNT 20
#define WIFI_PASSWORD_LENGTH 15
#define WIFI_NETWORKS_MAX 6
const char* WIFI_NAME = "remocon";
uint8_t newMACAddress[] = {0xA0, 0x46, 0x2A, 0x7A, 0x68, 0x36}; //A0-46-2A-7A-68-36
IPAddress access_poin_ip(192, 168, 50, 1);
IPAddress access_poin_gateway(192, 168, 50, 1);
IPAddress access_poin_subnet(255, 255, 255, 0);
const char* ssid     = "RC_6D";
std::string WIFIpasssword="123456789";

std::pair<String,String> wifi_settings[WIFI_NETWORKS_MAX];

//-----------------------------UDP
#define UDP_BEGIN "$RC:"
#define UDP_END "#\r"
#define UDP_FIXED_SIZE 5

Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//WebServer server(80);
AsyncWebServer server(80);

//-----------------------------SETINGS
#define RC_MODE_NORMAL 0
#define RC_MODE_SETUP 1


struct InputsData {
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

using namespace IO_Control;

FourAxisJoystick joystick1;
FourAxisJoystick joystick2;
InputsData inputs_main;
OledPrintLib *oledPrint = NULL;
Setings::Setings setings_data;
uint8_t RC_Mode = RC_MODE_NORMAL;
bool Wifi_Connected = false;

//https://microcontrollerslab.com/esp32-esp8266-web-server-input-data-html-forms/

void Task_ReadingInputs(void *param);
void Task_Conenct_to_Wifi(void *param);
void Task_SendUDP_data(void *param);

#pragma region Display

void Print(String string)
{
  #ifdef DEBUG
    Serial.print(string);
  #endif
  oledPrint->Print(string);
}

void Println(String string)
{
  #ifdef DEBUG
    Serial.println(string);
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

#pragma endregion

void PrintSetings(Setings::Setings setings)
{
  Println("Setings:");
  Println("ver:");
  Println("");
  Println("");
  for(auto element=setings._setings.begin(); element != setings._setings.end(); element++ )
  {
    UpdateLine(String(element->first.c_str()),5);
    UpdateLine(PrintSeting(element->second));
  }
}

void PrintInfo()
{
  Println("Remote Controler 6D");
  Println("ver: "+ String(VERSION));
}

void LoadSetings()
{

  #pragma region Joysticks
  setings_data.AddSeting("int_Joystick_left_MMin",-2048);
  setings_data.AddSeting("int_Joystick_left_MMax",2048);

  setings_data.AddSeting("int_Joystick_right_MMin",-2048);
  setings_data.AddSeting("int_Joystick_right_MMax",2048);

  setings_data.AddSeting("flo_Joystick_right_filer",0.95f);
  setings_data.AddSeting("flo_Joystick_left_filer",0.95f);

  #pragma endregion

  #pragma region Buttons

  //setings_data.AddSeting("bool_Btns_Mstable",string("0,0,0,0,0,0,0,0"));

  #pragma endregion

  #pragma region WIFI

  setings_data.AddSeting("str_host_wifi",std::string("192.168.1.210"));
  setings_data.AddSeting("int_host_port",25000);

  setings_data.AddSeting("str_passwd",std::string("qwerty1234"));

  setings_data.AddSeting("int_upd_freq",40);

  setings_data.AddSeting("str_WIFI_1_S",std::string("ForeverWIFIv2_2.4"));
  setings_data.AddSeting("str_WIFI_1_P",std::string("jmnseroj0ity43"));

  setings_data.AddSeting("str_WIFI_2_S",std::string("400%mocy"));
  setings_data.AddSeting("str_WIFI_2_P",std::string("qwerty987654321"));

  setings_data.AddSeting("str_WIFI_3_S",std::string(""));
  setings_data.AddSeting("str_WIFI_3_P",std::string(""));
  
  setings_data.AddSeting("str_WIFI_4_S",std::string(""));
  setings_data.AddSeting("str_WIFI_4_P",std::string(""));

  setings_data.AddSeting("str_WIFI_5_S",std::string(""));
  setings_data.AddSeting("str_WIFI_5_P",std::string(""));

  setings_data.AddSeting("str_WIFI_6_S",std::string(""));
  setings_data.AddSeting("str_WIFI_6_P",std::string(""));

  //setings_data.AddSeting("str_WIFI_7_S",string(""));
 // setings_data.AddSeting("str_WIFI_7_P",string(""));

  //setings_data.AddSeting("str_WIFI_8_S",string(""));
  //setings_data.AddSeting("str_WIFI_8_P",string(""));
  //setings_data.AddSeting("str_WIFI_9_S",string(""));
  //setings_data.AddSeting("str_WIFI_9_P",string(""));
  #pragma endregion

  setings_data.InitEPPROM();
  #ifdef SAVE_TO_FLASH
    setings_data.SaveSetingsToFlash();
  #endif
  setings_data.LoadSetingsFromFlash();


  wifi_settings[0] = std::pair<String,String>(setings_data.GetSeting("str_WIFI_1_S").data._string,setings_data.GetSeting("str_WIFI_1_P").data._string);
  wifi_settings[1] = std::pair<String,String>(setings_data.GetSeting("str_WIFI_2_S").data._string,setings_data.GetSeting("str_WIFI_2_P").data._string);
  wifi_settings[2] = std::pair<String,String>(setings_data.GetSeting("str_WIFI_3_S").data._string,setings_data.GetSeting("str_WIFI_3_P").data._string);
  wifi_settings[3] = std::pair<String,String>(setings_data.GetSeting("str_WIFI_4_S").data._string,setings_data.GetSeting("str_WIFI_4_P").data._string);
  wifi_settings[4] = std::pair<String,String>(setings_data.GetSeting("str_WIFI_5_S").data._string,setings_data.GetSeting("str_WIFI_5_P").data._string);
  wifi_settings[5] = std::pair<String,String>(setings_data.GetSeting("str_WIFI_6_S").data._string,setings_data.GetSeting("str_WIFI_6_P").data._string);

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
  
  joystick1.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_1, PIN_Y_ROT_JOYSTIK_1, PIN_Z_ROT_JOYSTIK_1, PIN_BTN_JOYSTIK_1,
    setings_data.GetSeting("flo_Joystick_left_filer").data._float,
    setings_data.GetSeting("int_Joystick_left_MMin").data._int,
    setings_data.GetSeting("int_Joystick_left_MMax").data._int);

  joystick2.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_2, PIN_Y_ROT_JOYSTIK_2, PIN_Z_ROT_JOYSTIK_2, PIN_BTN_JOYSTIK_2,
    setings_data.GetSeting("flo_Joystick_right_filer").data._float,
    setings_data.GetSeting("int_Joystick_right_MMin").data._int,
    setings_data.GetSeting("int_Joystick_right_MMax").data._int);
  
  joystick1.AutoZero();
  joystick2.AutoZero();
}

void DisplayInit()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.display();
  Serial.println("SSD1306 allocation success");
  oledPrint = new OledPrintLib(&display,DISPLAY_MAX_LINE_COUNT,DISPLAY_MAX_CHARACTER_COUNT);
}

#pragma region WEB PAGES

void PageNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void InitAllWebEvents()
{
  //WIFIpasssword = PassGenerator_GeneratePassword(WIFI_PASSWORD_LENGTH);
  char *wifipw = setings_data.GetSeting("str_passwd").data._string;
  #ifdef DEBUG
    Serial.println("wifi setup sd:");
    Serial.println(ssid);
    Serial.println("wifi setup pw:");
    Serial.println(wifipw);
  #endif

  WiFi.mode(WIFI_AP);
  // WiFi.softAPConfig(apIP, gateway, subnet);
  WiFi.softAPConfig(access_poin_ip, access_poin_gateway, access_poin_subnet);
  WiFi.softAP(ssid, wifipw);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    
    std::string val_table_mid = "";
    for (auto element = setings_data._setings.begin(); element!= setings_data._setings.end(); ++element)
    {
      val_table_mid = val_table_mid + "\n <tr><td>" + element->first + "</td> <td><input type=\"text\" name=\"" 
      + element->first+ "\"  , style=\"background-color: #1c1c1c; color: white;\" value=\""+ Setings::GetStringFromSeting(element->second)+"\"></td> </tr>"; 
    }
    std::string val_page = std::string(MAIN_page_beg) + val_table_mid.c_str() + std::string(MAIN_page_end);  



    request->send_P(200, "text/html", val_page.c_str());
  });


  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
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
   std::string val_page = 
    "<!DOCTYPE HTML><html><head> <title>RC 6D</title> <body style=\"background-color:#1c1c1c; color: white;\"></body> <body> Settings [" +
    std::to_string(request->params())+ 
    "] send to be updated, errosrs: [" + std::to_string(errors_count) +
    "] (settings not updated) <br><a href=\"/\">Return to Settings Page</a>";
   setings_data.SaveSetingsToFlash();
   request->send(200, "text/html", val_page.c_str());
  });


  server.onNotFound(PageNotFound);
  server.begin();
}

#pragma endregion

int FindWifiNetwork(String ssid)
{
  for (int i = 0; i < WIFI_NETWORKS_MAX; i++){
    if(wifi_settings[i].first == ssid) return i;
  }
  return -1;
}

bool ConnectWithAvailableWIfiNetwork()
{
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  WiFi.disconnect();
  bool notConnected=true;
  bool available[WIFI_NETWORKS_MAX];
  for (size_t i = 0; i < WIFI_NETWORKS_MAX; i++) available[i]=false;
  WiFi.scanNetworks();

  while (notConnected)
  {
    int i = WiFi.scanNetworks();
    if(i<=0){
      delay(500);
      continue;
    }
    
    for (int count = 0; count < i; count++){        
      String ssid= WiFi.SSID(count);
      Println("?:"+ssid);
      int num = FindWifiNetwork(ssid);
      if(num != -1) available[num]= true;
    } 

    for (size_t i = 0; i < WIFI_NETWORKS_MAX; i++){
      if(!available[i]) continue;
      Println("Connecting to:");
      Println(wifi_settings[i].first);
      WiFi.begin(wifi_settings[i].first.c_str(), wifi_settings[i].second.c_str());
      int connectionTryCount=0;
      Println("");
      delay(500);
      while (WiFi.status() != WL_CONNECTED && WIFI_CONNECTION_TRY_MAX_COUNT != connectionTryCount++) {
        Print(".");
        delay(500);
      }

      if(WiFi.status() == WL_CONNECTED){
        Println("Connected to:");
        Println(wifi_settings[i].first);
        Println("Sending to:");
        Println(setings_data.GetSeting("str_host_wifi").data._string);
        Println("Port:");
        Print(String(setings_data.GetSeting("int_host_port").data._int));
        notConnected = false;
        break;
      }
    
    }
  }
  return !notConnected;
}

void WaitForConnection(){
  wl_status_t statusPrev=WL_IDLE_STATUS;
  while(true){
    wl_status_t status = WiFi.status();
    vTaskDelay(100);
    if(status == statusPrev ) continue;
    if(status != WL_CONNECTED){ 
      Println("disconnected");
      Wifi_Connected = ConnectWithAvailableWIfiNetwork();
    }
    statusPrev = WiFi.status();
  }
}

void InitSetupMode()
{
  InitAllWebEvents();
  Println("--Setup mode--");
  Println("http://" + WiFi.softAPIP().toString());
  Println("sd:");
  Println(String(ssid));
  Println("pw:");
  Println(String(setings_data.GetSeting("str_passwd").data._string));
}

void InitNormalMode()
{
  xTaskCreate(Task_ReadingInputs,"Tread",2000, NULL, 3,NULL);
  xTaskCreate(Task_Conenct_to_Wifi,"Twifi",3000, NULL, 3,NULL);
  xTaskCreate(Task_SendUDP_data,"Tudp",3000, NULL, 3,NULL);
}

void setup() {

  Serial.begin(115200);
  DisplayInit();
  PrintInfo();
  LoadSetings();
  // PrintSetings(setings_data);
  GPIOinit();

  if(!InOut::ReadInput(PIN_MS_BTN_4) || !InOut::ReadInput(PIN_MS_BTN_5)){
    RC_Mode = RC_MODE_SETUP;
    InitSetupMode();
  }
  else{
    RC_Mode = RC_MODE_NORMAL;
    InitNormalMode();
  }
}

void loop() {
vTaskDelay(1000);
}

String fixString(int value, int size)
{
  String str = String(value);
  if(str.length() >= size) return str;
  std::string adons="";
  adons.resize(size-str.length(),' ');
  return String(adons.c_str()) + str;
}

void Task_SendUDP_data(void *param)
{
  WiFiUDP udp;
  IPAddress remoteIP;
  remoteIP.fromString(setings_data.GetSeting("str_host_wifi").data._string);

  unsigned int remotePort = setings_data.GetSeting("int_host_port").data._int;;
  float send_delay =  1000.0 / (float)setings_data.GetSeting("int_upd_freq").data._int;
  
//   size_t count=0;
// size_t ti = micros();
  while(1){
    String data = UDP_BEGIN +
      fixString(inputs_main.joystick_1_x,5)+ ":"+
      fixString(inputs_main.joystick_1_y,5)+ ":"+
      fixString(inputs_main.joystick_1_z,5)+ ":"+
      fixString(inputs_main.joystick_1_btn,1)+":"+
      fixString(inputs_main.joystick_2_x,5)+":"+
      fixString(inputs_main.joystick_2_y,5)+":"+
      fixString(inputs_main.joystick_2_z,5)+":"+
      fixString(inputs_main.joystick_2_btn,1)+":"+
      fixString(inputs_main.btn_1,1)+":"+
      fixString(inputs_main.btn_2,1)+":"+
      fixString(inputs_main.btn_3,1)+":"+
      fixString(inputs_main.btn_4,1)+":"+
      fixString(inputs_main.btn_5,1)+":"+
      fixString(inputs_main.btn_6,1)+":"+
      fixString(inputs_main.btn_7,1)+":"+
      fixString(inputs_main.btn_8,1)+":"+ 
      UDP_END;
    
    // Serial.println(data);
    if(Wifi_Connected){
      udp.beginPacket(remoteIP, remotePort);
      udp.print(data);
      udp.endPacket();
    }
    // count++;
    // if(count>1000){
    //   size_t tf = micros();
    //   Serial.println("time:" + String((double)(tf-ti)/1000000.0));
    //   ti = micros();
    // }
    vTaskDelay(pdMS_TO_TICKS(send_delay));
  }
}

void Task_Conenct_to_Wifi(void *param)
{
  WaitForConnection(); 
  // while (1)
  // {
  //   vTaskDelay(50);
  // }
  
}

void Task_ReadingInputs(void *param)
{
  InputsData inputs;

  while (1)
  {
    inputs.joystick_1_x = joystick1.readX();
    inputs.joystick_1_y = joystick1.readY();
    inputs.joystick_1_z = joystick1.readZ();
    inputs.joystick_1_btn = !joystick1.readBtn();

    inputs.joystick_2_x = joystick2.readX();
    inputs.joystick_2_y = joystick2.readY();
    inputs.joystick_2_z = joystick2.readZ();
    inputs.joystick_2_btn = !joystick2.readBtn();

    inputs.btn_1 = !InOut::ReadInput(PIN_MS_BTN_1);
    inputs.btn_2 = !InOut::ReadInput(PIN_MS_BTN_2);
    inputs.btn_3 = !InOut::ReadInput(PIN_MS_BTN_3);
    inputs.btn_4 = !InOut::ReadInput(PIN_MS_BTN_4);
    inputs.btn_5 = !InOut::ReadInput(PIN_MS_BTN_5);
    inputs.btn_6 = !InOut::ReadInput(PIN_MS_BTN_6);
    inputs.btn_7 = !InOut::ReadInput(PIN_MS_BTN_7);
    inputs.btn_8 = !InOut::ReadInput(PIN_MS_BTN_8);
    inputs_main = inputs;
    vTaskDelay(5); 
  }
}


