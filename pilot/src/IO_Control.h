// IO_Control.h

#include <Arduino.h>

#ifndef IO_CONTROL_H
#define IO_CONTROL_H


namespace IO_Control
{

  class FourAxisJoystick
  {
    private:

      int RESOLUTION = 4095;
      int pinX;
      int pinY;
      int pinZ;
      int pinBtn;
      int map_min= -2048;
      int map_max= 2048;
      int map_middle= 0;

      bool btn_prev;

      float x_prev;
      float y_prev;
      float z_prev;

      int offset_X=0;
      int offset_Y=0;
      int offset_Z=0;

      float multi_prev;
      float multi_cur;

      int ReadAna(int pin,int offset, float *prev);
      void resolutionSet(int resolution);

    public:
      #define RESOLUTION_10_BIT 1026
      #define RESOLUTION_12_BIT 4096

      FourAxisJoystick();
      ~FourAxisJoystick();

      void init(int resolution, int pinX, int pinY, int pinZ, int pinBtn, float softness);
      void init(int resolution, int pinX, int pinY, int pinZ, int pinBtn, float softness, int maping_min, int maping_max);
      
      void AutoZero();

      int readX();
      int readY();
      int readZ();
      int readBtn();

      int readX_Raw();
      int readY_Raw();
      int readZ_Raw();

  };

  class InOut
  {
    private:
    
    public:
      InOut(/* args */);
      ~InOut();
      
      static int ReadInput(int pin);
      static int GetAvarageAnalog(int pin, int count);

  };

  class InputGPIO: public InOut
  {
    private:
      bool prevState=false;
      bool lachRL=false;
      bool inverse=false;
      float prevValue=0;
      float dumping=1;
      float dumpingRevers=0;
      int GPIOnumber=0;
      int type;
    public:
      #define SET_AS_INPUT 1
      #define INVERSE_INPUT true
      #define DONT_INVERSE_INPUT false

      InputGPIO(int pin, int type = SET_AS_INPUT, bool inverseInput = DONT_INVERSE_INPUT )
      {
        this->GPIOnumber = pin;
        this->type = type;
        this->inverse = inverseInput;
      }

      void SetDumping(float dumping)
      {
        this->dumping = dumping;
        this->dumpingRevers = 1.0f - dumping;
      }

      bool ReadWithLach()
      {
        bool state = ReadDigital();
        if(state != prevState && state == false)
        { 
          lachRL = !lachRL;
        }
        prevState = state;
        return lachRL;
      }

      bool ReadDigital()
      {
        bool state = (bool)ReadInput(this->GPIOnumber);
        return inverse? !state : state;
      }

      int ReadAnalogWithDumping()
      {
        float value = (float)analogRead(this->GPIOnumber);
        value = (value * this->dumping) + (prevValue * dumpingRevers);
        prevValue = value;
        return (int)value;
      }

  };
}

#endif