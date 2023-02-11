
#include <string>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#ifndef OLEDPRINTLIB_H
#define OLEDPRINTLIB_H
#endif



#ifdef OLEDPRINTLIB_H
class OledPrintLib
{
  private:

  Adafruit_SSD1306 *display; 
  String *displayLines;
  int lineCount=0;
  int endLineIndex=0;

  void DislpayPrint(String line, bool pushLine, bool updateLine);
  void DislpayPrint(String line[10]);

  public:

  OledPrintLib(Adafruit_SSD1306 *display, int lineCount);

  void UpdateLine(String string, int line);
  void UpdateLine(String string);
  void Print(String string);
  void Println(String string);
};

#endif