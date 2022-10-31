// InOut.cpp

#include "IO_Control.h"
#include <Arduino.h>


using namespace IO_Control;


InOut::InOut()
{

}
InOut::~InOut()
{

}

int InOut::ReadInput(int pin)
{
    int in;
    do
    {
      in = digitalRead(pin);

    } while (in != digitalRead(pin));
    
    return in;
} 