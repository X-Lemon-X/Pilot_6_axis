// JOYSTICKcontrol.cpp

#include "IO_Control.h" 
#include <Arduino.h>

using namespace IO_Control;

  FourAxisJoystick::FourAxisJoystick(/* args */)
  {
  }
  
  FourAxisJoystick::~FourAxisJoystick()
  {
  }

  void FourAxisJoystick::init(int resolution, int pinX, int pinY, int pinZ, int pinBtn, float softness)
  {
    this->multi_cur = 1.0 - softness;
    this->multi_prev = softness;
    this->pinX= pinX;
    this->pinY = pinY;
    this->pinZ = pinZ;
    this->pinBtn = pinBtn;

    resolutionSet(resolution);
  }

  void FourAxisJoystick::init(int resolution, int pinX, int pinY, int pinZ, int pinBtn, float softness, int maping_min, int maping_max)
  {
    this->multi_cur = 1.0 - softness;
    this->multi_prev = softness;
    this->pinX= pinX;
    this->pinY = pinY;
    this->pinZ = pinZ;
    this->pinBtn = pinBtn;
    this->map_max = maping_max;
    this->map_min = maping_min;
    
    resolutionSet(resolution);
  }

  void FourAxisJoystick::AutoZero()
  {
    offset_X = GetAvarageAnalog(pinX,30) - RESOLUTION/2;
    offset_Y = GetAvarageAnalog(pinY,30) - RESOLUTION/2;
    offset_Z = GetAvarageAnalog(pinZ,30) - RESOLUTION/2;
  }

  int FourAxisJoystick::GetAvarageAnalog(int pin, int count)
  {
    int ava=0;
    for (size_t i = 0; i < count; i++)
    {
      ava += analogRead(pin);
      delay(1);
    }

    ava = ava / count;

    return ava;
  }

  void FourAxisJoystick::resolutionSet(int resolution)
  {

    switch (resolution)
    {
      case RESOLUTION_10_BIT:
          RESOLUTION = RESOLUTION_10_BIT;
        break;
      
      case RESOLUTION_12_BIT:
          RESOLUTION = RESOLUTION_12_BIT;
        break;
      
      default:
          RESOLUTION = RESOLUTION_12_BIT;
        break;
    }

  } 

  int FourAxisJoystick::readX()
  {
    return ReadAna(pinX, offset_X, &x_prev);
  }

  int FourAxisJoystick::readY()
  {
    return ReadAna(pinY, offset_Y, &y_prev);
  }

  int FourAxisJoystick::readZ()
  {
    return ReadAna(pinZ, offset_Z, &z_prev);
  }

  int FourAxisJoystick::readX_Raw()
  {
    return analogRead(pinX);
  }

  int FourAxisJoystick::readY_Raw()
  {
    return analogRead(pinY);
  }
  
  int FourAxisJoystick::readZ_Raw()
  {
    return analogRead(pinZ);
  }

  int FourAxisJoystick::ReadAna(int pin, int offset, float *prev)
  {
    float value = (float)analogRead(pin) - (float)offset;
    
    value = (value * multi_cur) + (*prev * multi_prev);
    *prev = value;

    if(value >= RESOLUTION/ 2)
      return map((int)value, RESOLUTION/2, RESOLUTION - offset, 0, map_max);
    else
      return map((int)value, - offset, RESOLUTION/2, map_min, 0);
    
    //return map((int)value, 0, RESOLUTION , map_min, map_max);
  }
  
  int FourAxisJoystick::readBtn()
  { 
    return InOut::ReadInput(pinBtn);
  }
