
//#include <list>
#include <string>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OledPrintLib.h"


  OledPrintLib::OledPrintLib(Adafruit_SSD1306 *screen, int lineCount=7, int charactersMaxContWidth=22)
  {
    this->screen = screen;
    this->lineCount=lineCount;  
    this->endLineIndex = lineCount-1;
    this->charactersMaxContWidth = charactersMaxContWidth;
    this->bufforStrings = std::vector<String>(lineCount);

    #ifdef DEBUG
      Serial.println("DEBUG: OledPrintLib");
      Serial.println((unsigned long)screen);
      Serial.println(lineCount);
      Serial.println((unsigned long)displayLines);
    #endif
  }

  void OledPrintLib::DislpayPrint()
  {
    screen->clearDisplay();
    int i =0;
    for (auto element = this->bufforStrings.begin(); element != this->bufforStrings.end(); element++)
    {
      screen->setCursor(0,9*(i++));
      std::string str =  element->c_str();
      str.resize(this->charactersMaxContWidth);
      screen->print(String(str.c_str()));
    }
    screen->display();
  }

  void OledPrintLib::UpdateLine(String string, int line)
  {
    if(line >= this->lineCount) line = this->endLineIndex;
    else if (line < 0) line =0;
    this->bufforStrings[this->lineCount-line] = string;
    DislpayPrint();
  }

  void OledPrintLib::UpdateLine(String string)
  {
    UpdateLine(string,0);
  }

  void OledPrintLib::Print(String line)
  {
    line = this->bufforStrings[endLineIndex] + line;   
    this->bufforStrings[endLineIndex] = line;
    DislpayPrint();
  }

  void OledPrintLib::Println(String line)
  { 
    this->bufforStrings.push_back(line);
    if(this->bufforStrings.size() >= this->lineCount) this->bufforStrings.erase(this->bufforStrings.begin());
    DislpayPrint();
  }
