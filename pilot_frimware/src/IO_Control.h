#pragma once
#include <Arduino.h>


#define RESOLUTION_8_BIT 256
#define RESOLUTION_10_BIT 1026
#define RESOLUTION_12_BIT 4096
#define RESOLUTION_14_BIT 16384

class FourAxisJoystick {
private:
  int RESOLUTION = 4095;
  int pinX;
  int pinY;
  int pinZ;
  int pinBtn;
  int map_min    = -2048;
  int map_max    = 2048;
  int map_middle = 0;

  bool btn_prev;

  float x_prev;
  float y_prev;
  float z_prev;

  int offset_X = 0;
  int offset_Y = 0;
  int offset_Z = 0;

  float multi_prev;
  float multi_cur;

  int ReadAna(int pin, int offset, float *prev);
  void resolutionSet(int resolution);

public:
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

class InOut {
private:
public:
  InOut(/* args */);
  ~InOut();

  static int ReadInput(int pin);
  static int GetAvarageAnalog(int pin, int count);
};
