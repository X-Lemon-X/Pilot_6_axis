
// #include <list>
#include <string>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OledPrintLib.h"
#include "main.hpp"


OledPrintLib::OledPrintLib(Adafruit_SSD1306 *screen, int lineCount = 6, int charactersMaxContWidth = 22)
: buffer_screen(std::vector<String>(lineCount)), buffer_screen_deque(std::deque<String>(lineCount)),
  lineCount(lineCount), endLineIndex(lineCount - 1), charactersMaxContWidth(charactersMaxContWidth),
  battery_precent(0), wifi_precent(0), status_connected(false),
  wifi_name("") // Initialize wifi_name to an empty string
{
}

void OledPrintLib::display_print() {
  screen->clearDisplay();

  // we print the status bar
  screen->setCursor(0, 0);
  // Draw battery icon
  int icon_x         = 0;
  int icon_y         = 0;
  int battery_width  = 12;
  int battery_height = 7;
  int tip_width      = 2;
  int tip_height     = 3;

  // Draw main battery body
  screen->drawRect(icon_x, icon_y, battery_width, battery_height, SSD1306_WHITE);
  // Draw battery tip
  screen->fillRect(icon_x + battery_width, icon_y + (battery_height - tip_height) / 2, tip_width, tip_height, SSD1306_WHITE);

  // Optionally, fill battery level
  int fill_width = (battery_width - 2) * battery_precent / 100;
  screen->fillRect(icon_x + 1, icon_y + 1, fill_width, battery_height - 2, SSD1306_WHITE);

  // Move cursor after battery icon
  screen->setCursor(icon_x + battery_width + tip_width + 2, 0);
  screen->print(battery_precent);
  // Draw WiFi signal bars (max 4 bars)
  int bars = 0;
  if(wifi_precent > 75)
    bars = 4;
  else if(wifi_precent > 50)
    bars = 3;
  else if(wifi_precent > 25)
    bars = 2;
  else if(wifi_precent > 0)
    bars = 1;
  else
    bars = 0;

  int bar_x          = screen->width() - 18;
  int bar_y          = 1;
  int bar_w          = 2;
  int bar_spacing    = 2;
  int bar_heights[4] = { 3, 5, 7, 9 };

  for(int b = 0; b < 4; ++b) {
    if(b < bars) {
      screen->fillRect(bar_x + b * (bar_w + bar_spacing), bar_y + (9 - bar_heights[b]), bar_w, bar_heights[b], SSD1306_WHITE);
    } else {
      screen->drawRect(bar_x + b * (bar_w + bar_spacing), bar_y + (9 - bar_heights[b]), bar_w, bar_heights[b], SSD1306_WHITE);
    }
  }
  // Move cursor after WiFi signal
  screen->setCursor(bar_x + 4 * (bar_w + bar_spacing) + 2, 0);
  screen->print(wifi_name);
  screen->print(" ");


  // Print lines
  int i = 1;
  for(auto &&line : buffer_screen_deque) {
    screen->setCursor(0, 9 * (i++));
    std::string str = line.c_str();
    str.resize(this->charactersMaxContWidth);
    screen->print(String(str.c_str()));
  }

  // for(auto &&line = ;) {
  //   screen->setCursor(0, 9 * (i++));
  //   std::string str = line;
  //   str.resize(this->charactersMaxContWidth);
  //   screen->print(String(str.c_str()));
  // }

  // for(auto element = this->buffer_screen.begin(); element != this->buffer_screen.end(); element++) {
  //   screen->setCursor(0, 9 * (i++));
  //   std::string str = element->c_str();
  //   str.resize(this->charactersMaxContWidth);
  //   screen->print(String(str.c_str()));
  // }
  screen->display();
}

void OledPrintLib::update_line(String string, int line) {
  if(line >= lineCount)
    line = endLineIndex;
  else if(line < 0)
    line = 0;

  // buffer_screen[lineCount - line] = string;
  buffer_screen_deque[lineCount - line] = string;
  display_print();
}

void OledPrintLib::update_line(String string) {
  update_line(string, 0);
}

void OledPrintLib::print(String line) {
  // this->buffer_screen[endLineIndex]       = this->buffer_screen[endLineIndex] + line;
  buffer_screen_deque[endLineIndex] = buffer_screen_deque[endLineIndex] + line;
  display_print();
}

void OledPrintLib::println(String line) {
  buffer_screen_deque.push_back(line);
  if(buffer_screen_deque.size() >= lineCount)
    buffer_screen_deque.pop_front();

  // this->buffer_screen.push_back(line);
  // if(this->buffer_screen.size() >= this->lineCount)
  //   this->buffer_screen.erase(this->buffer_screen.begin());
  display_print();
}

void OledPrintLib::set_status(int battery_precent, int wifi_precent, bool status_connected, String wifi_name) {
  this->battery_precent  = battery_precent;
  this->wifi_precent     = wifi_precent;
  this->status_connected = status_connected;
  this->wifi_name        = wifi_name;
  // display_print();
}