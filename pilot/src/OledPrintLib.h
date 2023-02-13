
//#include <string>

#ifndef OLEDPRINTLIB_H
#define OLEDPRINTLIB_H
#endif


#ifdef OLEDPRINTLIB_H
class OledPrintLib
{
  private:

  Adafruit_SSD1306 *screen = NULL; 
  String *displayLines;
  int lineCount=0;
  int endLineIndex=0;

  void DislpayPrint(String line, bool pushLine, bool updateLine);
  void DislpayPrint(String *line);

  public:

  OledPrintLib(Adafruit_SSD1306 *screen, int lineCount)
  {
    this->screen = screen;
    this->lineCount=lineCount;
    String lines[lineCount];
    this->displayLines = &lines[0];
    this->endLineIndex = lineCount-1;
  }

  void UpdateLine(String string, int line);
  void UpdateLine(String string);
  void Print(String string);
  void Println(String string);
};

#endif