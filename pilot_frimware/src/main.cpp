#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// #include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <esp_wifi.h>

#include "esp32-hal-cpu.h"
#include <list>
#include <string>

#include "EPPROMSetings.h"
#include "IO_Control.h"
#include "OledPrintLib.h"
#include "PassGenerator.h"
#include "Setings.h"
#include "SetingsPage.h"
#include "PageGenerator.hpp"
#include "main.hpp"


// Display
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OledPrintLib *oledPrint  = NULL;
String displayLines[DISPLAY_MAX_LINE_COUNT];
int lineCount = 0;


uint8_t newMACAddress[] = { 0xA0, 0x46, 0x2A, 0x7A, 0x68, 0x36 }; // A0-46-2A-7A-68-36
IPAddress access_poin_ip(192, 168, 50, 1);
IPAddress access_poin_gateway(192, 168, 50, 1);
IPAddress access_poin_subnet(255, 255, 255, 0);

// HTTP server
AsyncWebServer server(80);


// IO
FourAxisJoystick joystick1;
FourAxisJoystick joystick2;
InputsData inputs_main;
uint8_t RC_Mode = RC_MODE_NORMAL;

RcSettings rc_settings = {};
EPPROM_Settings epprom_settings;
bool Wifi_Connected = false;
bool restart_flag   = false;

// https://microcontrollerslab.com/esp32-esp8266-web-server-input-data-html-forms/

void Task_ReadingInputs(void *param);
void Task_Conenct_to_Wifi(void *param);
void Task_SendUDP_data(void *param);

#pragma region Display

void Print(String string) {
#ifdef DEBUG
  Serial.print(string);
#endif
  oledPrint->Print(string);
}

void Println(String string) {
#ifdef DEBUG
  Serial.println(string);
#endif
  oledPrint->Println(string);
}

void UpdateLine(String string) {
  oledPrint->UpdateLine(string);
}

void UpdateLine(String string, int line) {
  oledPrint->UpdateLine(string, line);
}

#pragma endregion

void PrintSetings(RcSettings setings) {
  Serial.println("Setings:");
  Serial.println("ver:");
  Serial.println("");
  Serial.println("");

  Serial.println("Left Joystick Min:");
  Serial.println(String(setings.int_Joystick_left_MMin));
  Serial.println("Left Joystick Max:");
  Serial.println(String(setings.int_Joystick_left_MMax));
  Serial.println("Right Joystick Min:");
  Serial.println(String(setings.int_Joystick_right_MMin));
  Serial.println("Right Joystick Max:");
  Serial.println(String(setings.int_Joystick_right_MMax));
  Serial.println("Left Joystick Filter:");
  Serial.println(String(setings.flo_Joystick_left_filer));
  Serial.println("Right Joystick Filter:");
  Serial.println(String(setings.flo_Joystick_right_filer));
  Serial.println("Update Freq:");
  Serial.println(String(setings.int_upd_freq));
  Serial.println("Host Port:");
  Serial.println(String(setings.int_host_port));
  Serial.println("SSID:");
  Serial.println(String(setings.str_ssid));
  Serial.println("Password:");
  Serial.println(String(setings.str_passwd));
  Serial.println("Target IP:");
  Serial.println(String(setings.str_target_ip));
  for(int i = 0; i < WIFI_NETWORKS_MAX; ++i) {
    Serial.println("WiFi Seed[" + String(i) + "]:");
    Serial.println(String(setings.str_wifi_ssids[i]));
    Serial.println("WiFi Password[" + String(i) + "]:");
    Serial.println(String(setings.str_wifi_passwords[i]));
  }
}

void PrintInfo() {
  Println("Remote Controler 6D");
  Println("ver: " + String(VERSION));
}

void LoadSetings() {

#pragma endregion

  // we load some default values to the settings
  rc_settings.int_Joystick_left_MMin   = -2048;
  rc_settings.int_Joystick_left_MMax   = 2048;
  rc_settings.int_Joystick_right_MMin  = -2048;
  rc_settings.int_Joystick_right_MMax  = 2048;
  rc_settings.flo_Joystick_left_filer  = 0.95f;
  rc_settings.flo_Joystick_right_filer = 0.95f;
  rc_settings.int_upd_freq             = 20;
  rc_settings.int_host_port            = 25000;
  strcpy(rc_settings.str_ssid, "RC_6D");
  strcpy(rc_settings.str_passwd, "qwerty1234");
  strcpy(rc_settings.str_target_ip, "192.168.20.2");
  // str_target_ip
  // str_host_wifi
  strcpy(rc_settings.str_wifi_ssids[0], "NomadT");
  strcpy(rc_settings.str_wifi_passwords[0], "NomadToliwka");

  epprom_settings.init(sizeof(rc_settings));
#ifdef SAVE_TO_FLASH
  epprom_settings.save_settings(rc_settings, SETTINGS_ADRES_BEG);
#endif
  epprom_settings.load_settings(rc_settings, SETTINGS_ADRES_BEG);
}

void GPIOinit() {
  pinMode(PIN_MS_BTN_1, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_2, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_3, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_4, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_5, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_6, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_7, INPUT_PULLUP);
  pinMode(PIN_MS_BTN_8, INPUT_PULLUP);

  pinMode(PIN_BTN_JOYSTIK_1, INPUT_PULLUP);
  pinMode(PIN_BTN_JOYSTIK_2, INPUT_PULLUP);

  pinMode(PIN_X_ROT_JOYSTIK_1, INPUT);
  pinMode(PIN_Y_ROT_JOYSTIK_1, INPUT);
  pinMode(PIN_Z_ROT_JOYSTIK_1, INPUT);

  pinMode(PIN_X_ROT_JOYSTIK_2, INPUT);
  pinMode(PIN_Y_ROT_JOYSTIK_2, INPUT);
  pinMode(PIN_Z_ROT_JOYSTIK_2, INPUT);

  joystick1.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_1, PIN_Y_ROT_JOYSTIK_1, PIN_Z_ROT_JOYSTIK_1,
                 PIN_BTN_JOYSTIK_1, rc_settings.flo_Joystick_left_filer, rc_settings.int_Joystick_left_MMin,
                 rc_settings.int_Joystick_left_MMax);

  joystick2.init(RESOLUTION_12_BIT, PIN_X_ROT_JOYSTIK_2, PIN_Y_ROT_JOYSTIK_2, PIN_Z_ROT_JOYSTIK_2,
                 PIN_BTN_JOYSTIK_2, rc_settings.flo_Joystick_right_filer, rc_settings.int_Joystick_right_MMin,
                 rc_settings.int_Joystick_right_MMax);

  joystick1.AutoZero();
  joystick2.AutoZero();
}

void DisplayInit() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;)
      ;
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  Serial.println("SSD1306 allocation success");
  oledPrint = new OledPrintLib(&display, DISPLAY_MAX_LINE_COUNT, DISPLAY_MAX_CHARACTER_COUNT);
}

#pragma region WEB PAGES

void PageNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void InitAccessPoint() {

#ifdef DEBUG
  Serial.println("wifi setup sd:");
  Serial.println(rc_settings.str_ssid);
  Serial.println("wifi setup pw:");
  Serial.println(rc_settings.str_passwd);
#endif

  WiFi.mode(WIFI_AP);
  // WiFi.softAPConfig(apIP, gateway, subnet);
  WiFi.softAPConfig(access_poin_ip, access_poin_gateway, access_poin_subnet);
  WiFi.softAP(rc_settings.str_ssid, rc_settings.str_passwd);
}

void InitAllWebEvents() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    auto page = PageGenerator::generate_page(rc_settings);
    request->send_P(200, "text/html", page.c_str());
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    RcSettings temp_settings = rc_settings;
    auto info                = PageGenerator::get_settings_from_page(request, temp_settings);
    auto page                = PageGenerator::get_saved_settings_page(info);
    request->send(200, "text/html", page.c_str());
    if(info) {
      epprom_settings.save_settings(temp_settings, SETTINGS_ADRES_BEG);
      restart_flag = true;
      Println("Settings saved!");
    }
  });

  server.onNotFound(PageNotFound);
  server.begin();
}

#pragma endregion

int look_for_saved_seed(String ssid) {
  for(int i = 0; i < WIFI_NETWORKS_MAX; i++) {
    if(strcmp(rc_settings.str_wifi_ssids[i], ssid.c_str()) == 0)
      return i;
  }
  return -1;
}

bool ConnectWithWifiNetwork() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  WiFi.disconnect();
  bool not_connected = true;
  std::vector<int> ssids;
  WiFi.scanNetworks();
  vTaskDelay(1000);

  while(not_connected) {
    Println("Scanning for WiFi...");
    int i = WiFi.scanNetworks();
    // if there are no networks we wait and try again
    if(i <= 0) {
      vTaskDelay(500);
      continue;
    }

    ssids.clear();
    for(int count = 0; count < i; count++) {
      String ssid = WiFi.SSID(count);
      Println("?:" + ssid);
      int maybe_seed = look_for_saved_seed(ssid);
      if(maybe_seed >= 0) {
        ssids.push_back(maybe_seed);
      }
    }

    // we sort the ssids to have the highest priority first
    std::sort(ssids.begin(), ssids.end());

    // we try to connect to the highest priority ssid
    for(auto &&seed : ssids) {
      WiFi.begin(rc_settings.str_wifi_ssids[seed], rc_settings.str_wifi_passwords[seed]);
      int connection_try_count = 0;
      Println("Connecting to:");
      Println("");
      delay(500);
      while(WiFi.status() != WL_CONNECTED && WIFI_CONNECTION_TRY_MAX_COUNT != connection_try_count++) {
        Print(".");
        delay(500);
      }

      if(WiFi.status() == WL_CONNECTED) {
        Println(rc_settings.str_wifi_ssids[seed]);
        Println("TIP:");
        Print(rc_settings.str_target_ip);
        Println("Port:");
        Print(String(rc_settings.int_host_port));
        Println("IP:");
        Print(WiFi.localIP().toString());
        not_connected = false;
        break;
      }
    }
  }
  return !not_connected;
}

void WaitForConnection() {
  wl_status_t statusPrev = WL_IDLE_STATUS;
  while(true) {
    wl_status_t status = WiFi.status();
    vTaskDelay(100);
    if(status != WL_CONNECTED) {
      Println("WIFI disconnected!");
      Wifi_Connected = ConnectWithWifiNetwork();
    }
    if(status == statusPrev)
      continue;
    if(Wifi_Connected)
      InitAllWebEvents();
    statusPrev = WiFi.status();
  }
}

void InitSetupMode() {
  InitAccessPoint();
  InitAllWebEvents();
  Println("--Setup mode--");
  Println("http://" + WiFi.softAPIP().toString());
  Println("sd:");
  Println(String(rc_settings.str_ssid));
  Println("pw:");
  Println(String(rc_settings.str_passwd));
}

void InitNormalMode() {
  xTaskCreate(Task_ReadingInputs, "Tread", 2000, NULL, 3, NULL);
  xTaskCreate(Task_Conenct_to_Wifi, "Twifi", 3000, NULL, 3, NULL);
  xTaskCreate(Task_SendUDP_data, "Tudp", 3000, NULL, 3, NULL);
}

void setup() {
  Serial.begin(115200);
  DisplayInit();
  PrintInfo();
  LoadSetings();
  PrintSetings(rc_settings);
  GPIOinit();

  if(!InOut::ReadInput(PIN_MS_BTN_4) || !InOut::ReadInput(PIN_MS_BTN_5)) {
    RC_Mode = RC_MODE_SETUP;
    InitSetupMode();
  } else {
    RC_Mode = RC_MODE_NORMAL;
    InitNormalMode();
  }
}

void loop() {

  if(restart_flag) {
    restart_flag = false;
    vTaskDelay(WAIT_TIME_BETWEEN_RESTART);
    ESP.restart();
  }
  vTaskDelay(100);
}

String fixString(int value, int size) {
  String str = String(value);
  if(str.length() >= size)
    return str;
  std::string adons = "";
  adons.resize(size - str.length(), ' ');
  return String(adons.c_str()) + str;
}

void Task_SendUDP_data(void *param) {
  WiFiUDP udp;
  IPAddress remoteIP;
  remoteIP.fromString(rc_settings.str_target_ip);

  unsigned int remotePort = rc_settings.int_host_port;
  float send_delay        = 1000.0 / (float)rc_settings.int_upd_freq;

  while(1) {
    String data = UDP_BEGIN + fixString(inputs_main.joystick_1_x, 5) + ":" + fixString(inputs_main.joystick_1_y, 5) +
                  ":" + fixString(inputs_main.joystick_1_z, 5) + ":" + fixString(inputs_main.joystick_1_btn, 1) +
                  ":" + fixString(inputs_main.joystick_2_x, 5) + ":" + fixString(inputs_main.joystick_2_y, 5) +
                  ":" + fixString(inputs_main.joystick_2_z, 5) + ":" + fixString(inputs_main.joystick_2_btn, 1) +
                  ":" + fixString(inputs_main.btn_1, 1) + ":" + fixString(inputs_main.btn_2, 1) + ":" +
                  fixString(inputs_main.btn_3, 1) + ":" + fixString(inputs_main.btn_4, 1) + ":" +
                  fixString(inputs_main.btn_5, 1) + ":" + fixString(inputs_main.btn_6, 1) + ":" +
                  fixString(inputs_main.btn_7, 1) + ":" + fixString(inputs_main.btn_8, 1) + ":" + UDP_END;

    if(Wifi_Connected) {
      udp.beginPacket(remoteIP, remotePort);
      udp.print(data);
      udp.endPacket();
    }
    vTaskDelay(pdMS_TO_TICKS(send_delay));
  }
}

void Task_Conenct_to_Wifi(void *param) {
  WaitForConnection();
}

void Task_ReadingInputs(void *param) {
  InputsData inputs;

  while(1) {
    inputs.joystick_1_x   = joystick1.readX();
    inputs.joystick_1_y   = joystick1.readY();
    inputs.joystick_1_z   = joystick1.readZ();
    inputs.joystick_1_btn = !joystick1.readBtn();

    inputs.joystick_2_x   = joystick2.readX();
    inputs.joystick_2_y   = joystick2.readY();
    inputs.joystick_2_z   = joystick2.readZ();
    inputs.joystick_2_btn = !joystick2.readBtn();

    inputs.btn_1 = !InOut::ReadInput(PIN_MS_BTN_1);
    inputs.btn_2 = !InOut::ReadInput(PIN_MS_BTN_2);
    inputs.btn_3 = !InOut::ReadInput(PIN_MS_BTN_3);
    inputs.btn_4 = !InOut::ReadInput(PIN_MS_BTN_4);
    inputs.btn_5 = !InOut::ReadInput(PIN_MS_BTN_5);
    inputs.btn_6 = !InOut::ReadInput(PIN_MS_BTN_6);
    inputs.btn_7 = !InOut::ReadInput(PIN_MS_BTN_7);
    inputs.btn_8 = !InOut::ReadInput(PIN_MS_BTN_8);
    inputs_main  = inputs;
    vTaskDelay(5);
  }
}
