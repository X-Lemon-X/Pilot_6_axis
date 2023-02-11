
//#include <list>
#include <string>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OledPrintLib.h"

#ifndef DEBB
#define DEBB #ifdef DEBUG
#endif

#ifndef DEBE
#define DEBE #endif
#endif

  OledPrintLib::OledPrintLib(Adafruit_SSD1306 *display, int lineCount);
  {
    if(display==NULL) return;
    this->display = display;
    this->lineCount=lineCount;
    String lines[lineCount];
    this->displayLines = &lines[0];
    this->endLineIndex = lineCount-1;
  }

  void OledPrintLib::DislpayPrint(String line, bool pushLine, bool updateLine)
  {
    if(pushLine) line = displayLines[endLineIndex] + line;
    else if(updateLine) displayLines[endLineIndex] = line;
    else for(int i=1; i < this->lineCount; i++) displayLines[i-1] = displayLines[i];
    
    displayLines[endLineIndex] = line;

    display.clearDisplay();
    display.setCursor(0,0); 

    for(int i=1; i < this->lineCount; i++)
    { 
      display.setCursor(0,9*(i-1));
      std::string str = displayLines[i].c_str();
      str.resize(22);
      display.print(String(str.c_str()));
    }
    display.display();
  }

  void OledPrintLib::DislpayPrint(String line[10])
  {
    display.clearDisplay();
    display.setCursor(0,0); 

    for(int i=0; i < this->lineCount; i++)
    { 
      display.setCursor(0,9*i);
      std::string str = displayLines[i].c_str();
      str.resize(22);
      display.print(String(str.c_str()));
    }
      display.display();
  }

  void OledPrintLib::UpdateLine(String string, int line)
  {
    if(line >= this->lineCount) line = this->endLineIndex;
    else if (line < 0) line =0;

    displayLines[line] = string;
    DislpayPrint(displayLines);


    DEBB
      Serial.println(string);
    DEBE
  }

  void OledPrintLib::UpdateLine(String string)
  {
    UpdateLine(string,endLineIndex);
  }

  void OledPrintLib::Print(String string)
  {
    string = displayLines[endLineIndex] + string;
    displayLines[this->lineCount-1] = string;
    DislpayPrint(displayLines);

    DEBB
    Serial.print(string);
    DEBE
  }

  void OledPrintLib::Println(String string)
  {
    for(int i=1; i < this->lineCount; i++) displayLines[i-1] = displayLines[i];
    displayLines[this->lineCount-1] = string;
    DislpayPrint(displayLines);

    DEBB
    Serial.println(string);
    DEBE
  }
