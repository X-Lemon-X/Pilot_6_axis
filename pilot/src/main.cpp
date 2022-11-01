#include <Arduino.h>
#include "IO_Control.h"
#include "OledMenu.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <list>
#include "esp32-hal-cpu.h"



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

using namespace OledMenu;

/*

MenuItem item_1 = MenuItem("item1",); 
  MenuItem set1 = MenuItem("set1", "text"); 
  MenuItem set2 = MenuItem("set2", true); 
MenuItem item_2 = MenuItem("item2",); 
  MenuItem item_2_1 = MenuItem("item2_item1",); 
    MenuItem set3 = MenuItem("set3", 15);  
  MenuItem item_2_2 = MenuItem("item2_item2",); 
    MenuItem set4 = MenuItem("set4", "text"); 
     

Menu menu_1 = Menu(list<MenuItem>{item_1, item_2});
  Menu menu_1_1 = Menu(list<MenuItem>{set1, set2});
  Menu menu_1_2 = Menu(list<MenuItem>{item_2_1, item_2_2});
    Menu menu_1_2_1 = Menu(list<MenuItem>{set3});
    Menu menu_1_2_2 =Menu(list<MenuItem>{set4});

*/
 /*    
    MenuItem set1 = MenuItem("set1", "text"); 
    MenuItem set2 = MenuItem("set2", true); 
   Menu menu_1_1 = Menu(list<MenuItem>{set1, set2});
  MenuItem item_1 = MenuItem("item1", menu_1_1);


      MenuItem set4 = MenuItem("set4", "text"); 
      Menu menu_s4 = Menu(list<MenuItem>{set4});
    MenuItem item_2_2 = MenuItem("item2_item2",menu_s4); 
      MenuItem set3 = MenuItem("set3", 15);  
      Menu menu_s3 = Menu(list<MenuItem>{set3});
    MenuItem item_2_1 = MenuItem("item2_item1",menu_s3); 
  Menu menu_1_2 = Menu(list<MenuItem>{item_2_1, item_2_2});
  MenuItem item_2 = MenuItem("item2",menu_1_2);

Menu menu_1 = Menu(list<MenuItem>{item_1, item_2});

*/

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


TaskHandle_t SecondLoop;


void ReadingInputs();
void loop2(void * pvParameters);



void setup() {


  Serial.begin(115200);
  joystick1.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_1, PIN_Y_ROT_JOYSTIK_1, PIN_Z_ROT_JOYSTIK_1, PIN_BTN_JOYSTIK_1, 0.80f);
  joystick2.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_2, PIN_Y_ROT_JOYSTIK_2, PIN_Z_ROT_JOYSTIK_2, PIN_BTN_JOYSTIK_2, 0.95f);

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

  joystick1.AutoZero();
  joystick2.AutoZero();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  display.clearDisplay();
  
  String str = "RC v";
  str += String(VERSION);

  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,0);
  display.println(str);
  display.display();


  int core = xPortGetCoreID(); 
  if(core == 1)
    core = 0;
  else
    core = 1;

  xTaskCreatePinnedToCore(loop2, "SecondLoop", 10000, NULL, 1, &SecondLoop, core);          

  




}

// Serialhandliing and wireless conections
void loop() {

  int i =5;
 
}


// reding io and comunication hadler
void loop2(void * pvParameters)
{
  for(;;)
  {
      ReadingInputs();
      //display.clearDisplay();
      //display.setCursor(0,0);
      Serial.print(inputs_main.joystick_1_x);
      Serial.print(",");
      Serial.print(inputs_main.joystick_1_y);
      Serial.print(",");
      Serial.print(inputs_main.joystick_1_z);
      Serial.print(",");
      Serial.print(inputs_main.joystick_2_x);
      Serial.print(",");
      Serial.print(inputs_main.joystick_2_y);
      Serial.print(",");
      Serial.print(inputs_main.joystick_2_z);
      Serial.print("  ,  ");
      Serial.print(inputs_main.joystick_1_btn);
      Serial.print(",");
      Serial.print(inputs_main.joystick_2_btn);
      Serial.print("  ,  ");
      Serial.println();
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

  inputs_main.btn_1 = IO_Control::InOut::ReadInput(PIN_MS_BTN_1);
  inputs_main.btn_2 = IO_Control::InOut::ReadInput(PIN_MS_BTN_2);
  inputs_main.btn_3 = IO_Control::InOut::ReadInput(PIN_MS_BTN_3);
  inputs_main.btn_4 = IO_Control::InOut::ReadInput(PIN_MS_BTN_4);
  inputs_main.btn_5 = IO_Control::InOut::ReadInput(PIN_MS_BTN_5);
  inputs_main.btn_6 = IO_Control::InOut::ReadInput(PIN_MS_BTN_6);
  inputs_main.btn_7 = IO_Control::InOut::ReadInput(PIN_MS_BTN_7);
  inputs_main.btn_8 = IO_Control::InOut::ReadInput(PIN_MS_BTN_8);
}


