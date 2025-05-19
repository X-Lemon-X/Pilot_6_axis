#pragma once
// #include <string>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include <deque>


class OledPrintLib {
private:
  Adafruit_SSD1306 *screen = NULL;
  std::vector<String> buffer_screen;
  std::deque<String> buffer_screen_deque;

  int lineCount;
  int endLineIndex;
  int charactersMaxContWidth;
  int battery_precent;
  int wifi_precent;
  bool status_connected;
  String wifi_name;

  void display_print();

public:
  OledPrintLib(Adafruit_SSD1306 *screen, int lineCount, int charactersMaxContWidth);
  ~OledPrintLib() {
  }
  void update_line(String string, int line);
  void update_line(String string);
  void print(String string);
  void println(String string);
  void set_status(int battery_precent, int wifi_precent, bool status_connected, String wifi_name);
};
