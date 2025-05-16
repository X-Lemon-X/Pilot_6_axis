#pragma once

// #include <EEPROM.h>   // https://randomnerdtutorials.com/esp32-flash-memory/
// #include <Preferences.h>  //
// https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/

// for use of dual cores refere to
// https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

//----------------------IMPORTANT----BUILDING
// only use once to save new setings to flash -> Flush
// #define SAVE_TO_FLASH
// ESP32 with it uncomented then Flush again without it //newer work with it
// uncommented or it will break EPPROM
#define DEBUG

//-----------------------------INFO
#define VERSION 3.1

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
#define OLED_RESET -1 // 4

#define DISPLAY_MAX_LINE_COUNT 7
#define DISPLAY_MAX_CHARACTER_COUNT 22


//-----------------------------WIFI_SETUP
// #define WIFI_SETINGS_BEG_NAME "str_WIFI_"
#define WIFI_CONNECTION_TRY_MAX_COUNT 20


//-----------------------------UDP
#define UDP_BEGIN "$RC:"
#define UDP_END "#\r"
#define UDP_FIXED_SIZE 5


//-----------------------------SETINGS
#define RC_MODE_NORMAL 0
#define RC_MODE_SETUP 1


#define SETTINGS_ADRES_BEG 0

#define WAIT_TIME_BETWEEN_RESTART 2000

#define STRING_SIZE 35
#define WIFI_NETWORKS_MAX 10


struct InputsData {
  int joystick_1_x   = 0;
  int joystick_1_y   = 0;
  int joystick_1_z   = 0;
  int joystick_1_btn = 0;

  int joystick_2_x   = 0;
  int joystick_2_y   = 0;
  int joystick_2_z   = 0;
  int joystick_2_btn = 0;

  int btn_1 = 0;
  int btn_2 = 0;
  int btn_3 = 0;
  int btn_4 = 0;
  int btn_5 = 0;
  int btn_6 = 0;
  int btn_7 = 0;
  int btn_8 = 0;
};


struct RcSettings {
  int int_Joystick_left_MMin;
  int int_Joystick_left_MMax;
  int int_Joystick_right_MMin;
  int int_Joystick_right_MMax;

  float flo_Joystick_left_filer;
  float flo_Joystick_right_filer;

  int int_host_port;
  int int_upd_freq;
  char str_ssid[STRING_SIZE];
  char str_target_ip[STRING_SIZE];
  char str_passwd[STRING_SIZE];

  char str_wifi_ssids[WIFI_NETWORKS_MAX][STRING_SIZE];
  char str_wifi_passwords[WIFI_NETWORKS_MAX][STRING_SIZE];
};