#include <Arduino.h>
#include "IO_Control.h"
#include "OledMenu.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <esp_wifi.h>

#include <list>
#include "esp32-hal-cpu.h"
#include "SetingsPage.h"
#include "Setings.h"



//#include <EEPROM.h>   // https://randomnerdtutorials.com/esp32-flash-memory/
//#include <Preferences.h>  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/


//for use of dual cores refere to https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/


//-----------------------------INFO
#define VERSION 1.2



//-----------------------------JOYSTICKS PINOUT
#define PIN_X_ROT_JOYSTIK_1 13
#define PIN_Y_ROT_JOYSTIK_1 14
#define PIN_Z_ROT_JOYSTIK_1 27

#define PIN_X_ROT_JOYSTIK_2 26
#define PIN_Y_ROT_JOYSTIK_2 25
#define PIN_Z_ROT_JOYSTIK_2 33

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

Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//WebServer server(80);
AsyncWebServer server(80);

using namespace OledMenu;
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

IO_Control::FourAxisJoystick joystick1;
IO_Control::FourAxisJoystick joystick2;
InputsData inputs_main;

const char* input_parameter1 = "input_string";
const char* input_parameter2 = "input_integer";
const char* input_parameter3 = "input_float";

Setings::Setings setings_data;


TaskHandle_t SecondLoop;

 //https://microcontrollerslab.com/esp32-esp8266-web-server-input-data-html-forms/

void ReadingInputs();
void loop2(void * pvParameters);

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

  joystick1.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_1, PIN_Y_ROT_JOYSTIK_1, PIN_Z_ROT_JOYSTIK_1, PIN_BTN_JOYSTIK_1, 0.80f);
  joystick2.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_2, PIN_Y_ROT_JOYSTIK_2, PIN_Z_ROT_JOYSTIK_2, PIN_BTN_JOYSTIK_2, 0.95f);

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
  
  String str = "RC v";
  str += String(VERSION);

  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,0);
  display.println(str);
  display.setTextSize(1);    
}

void PageNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void InitAccessPoint()
{
  const char* ssid     = "Remote_Controler_4D";
  const char* password = "123456789";
 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    int errors_count=0; 
    for (size_t i = 0; i < request->params(); i++)
    {
      String val = request->getParam(i)->value;
      errors_count += setings_data.UpdateSeting(request->getParam(i)->name, val) == Setings::ERRORS_OK ? 0 : 1;
    }
   request->send(200, "text/html", "Setings send to be updated <br><a href=\"/\">Return to Home Page</a>");
  });


  server.onNotFound(PageNotFound);
  server.begin();
}

void InitSetupMode()
{
  InitAccessPoint();

}

void InitNormalMode()
{

    WiFi.mode(WIFI_STA);
  uint8_t newMACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  WiFi.setHostname("Pyszne ciacteczka");
  WiFi.begin("ForeverWIFI", "6TTZQWQ67NR9");


 int core = xPortGetCoreID(); 
  if(core == 1)
    core = 0;
  else
    core = 1;

  display.setCursor(0,20);
  display.println("Connecting to WiFi ..");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  //xTaskCreatePinnedToCore(loop2, "SecondLoop", 10000, NULL, 1, &SecondLoop, core);        
  //xTaskCreatePinnedToCore(loop1, "FirstLoop", 10000, NULL, 0, &SecondLoop, xPortGetCoreID());          
}

void setup() {

  Serial.begin(115200);
  
  GPIOinit();
  DisplayInit();

  InitSetupMode();

  if(InOut::ReadInput(PIN_MS_BTN_4) || InOut::ReadInput(PIN_MS_BTN_5)) InitSetupMode();
  else InitNormalMode();
}

// Serialhandliing and wireless conections
void loop() {
  if(WiFi.status() == WL_CONNECTED){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(WiFi.localIP());
  display.setCursor(0,9);
  display.print("RRSI: ");
  display.println(WiFi.RSSI());
  display.setCursor(0,20);
  display.println(WiFi.macAddress());
  display.display();
  }
  else{
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("disconnected");
    display.display();
  }

  delay(10);
}


// reding io and comunication hadler
void loop2(void * pvParameters)
{
 ReadingInputs();
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


