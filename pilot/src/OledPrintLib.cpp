
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

  void OledPrintLib::DislpayPrint(String line, bool pushLine, bool updateLine)
  {
    if(pushLine) line = displayLines[endLineIndex] + line;
    else if(updateLine)
    {
      displayLines[endLineIndex] = line;
      this->bufforStrings.insert(this->bufforStrings.end(),line);
    }
    else {
      for(int i=1; i < this->lineCount; i++) displayLines[i-1] = displayLines[i];

      this->bufforStrings.push_back(line);
      if(this->bufforStrings.size() >= this->lineCount) this->bufforStrings.erase(this->bufforStrings.begin());
      
    }
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

  void OledPrintLib::DislpayPrint()
  {
    screen->clearDisplay();
    screen->setCursor(0,0); 

    int i =0;
    for (auto element = this->bufforStrings.begin(); element != this->bufforStrings.end(); element++;)
    {
      screen->setCursor(0,9*(i++));
      std::string str =  element->c_str();
      str.resize(this->charactersMaxContWidth);
      screen->print(String(str.c_str()));
    }
    

    for(int i=0; i < this->lineCount; i++)
    { 
      screen->setCursor(0,9*i);
      std::string str = displayLines[i].c_str();
      str.resize(this->charactersMaxContWidth);
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
  }
