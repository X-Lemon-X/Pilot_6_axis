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

int InOut::GetAvarageAnalog(int pin, int count)
{
  int ava=0;
  for (size_t i = 0; i < count; i++) ava += analogRead(pin);
  ava = ava / count;
  return ava;
}

int InOut::ReadInput(int pin)
{
  int in;
  do in = digitalRead(pin);
  while (in != digitalRead(pin));
  return in;
} 