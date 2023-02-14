
//#include <list>
#include <string>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OledPrintLib.h"


  OledPrintLib::OledPrintLib(Adafruit_SSD1306 *screen, int lineCount)
  {
    this->screen = screen;
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

    screen->clearDisplay();
    screen->setCursor(0,0); 

    for(int i=1; i < this->lineCount; i++)
    { 
      screen->setCursor(0,9*(i-1));
      std::string str = displayLines[i].c_str();
      str.resize(22);
      screen->print(String(str.c_str()));
    }
    screen->display();
  }

  void OledPrintLib::DislpayPrint(String line[10])
  {
    screen->clearDisplay();
    screen->setCursor(0,0); 

    for(int i=0; i < this->lineCount; i++)
    { 
      screen->setCursor(0,9*i);
      std::string str = displayLines[i].c_str();
      str.resize(22);
      screen->print(String(str.c_str()));
    }
      screen->display();
  }

  void OledPrintLib::UpdateLine(String string, int line)
  {
    if(line >= this->lineCount) line = this->endLineIndex;
    else if (line < 0) line =0;

    displayLines[line] = string;
    DislpayPrint(displayLines);


    #ifdef DEBUG
      Serial.println(string);
    #endif
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

    #ifdef DEBUG
    Serial.print(string);
    #endif
  }

  void OledPrintLib::Println(String string)
  {
    for(int i=1; i < this->lineCount; i++) displayLines[i-1] = displayLines[i];
    displayLines[this->lineCount-1] = string;
    DislpayPrint(displayLines);

    #ifdef DEBUG
    Serial.println(string);
    #endif
  }
