
//#include <string>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>

#ifndef OLEDPRINTLIB_H
#define OLEDPRINTLIB_H
#endif


//#define DEBUG



#ifdef OLEDPRINTLIB_H

class OledPrintLib
{
  private:

  Adafruit_SSD1306 *screen = NULL; 
  std::vector<String> bufforStrings; 

  int lineCount=0;
  int endLineIndex=0;
  int charactersMaxContWidth=0;

  void DislpayPrint();

  public:

  OledPrintLib(Adafruit_SSD1306 *screen, int lineCount, int charactersMaxContWidth);
  ~OledPrintLib(){}
  void UpdateLine(String string, int line);
  void UpdateLine(String string);
  void Print(String string);
  void Println(String string);
};


#endif