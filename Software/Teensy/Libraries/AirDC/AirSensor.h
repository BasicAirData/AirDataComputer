/*
  AirDC.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/

#ifndef AirSensor_h
#define AirSensor_h
//#include "Arduino.h"
#include <AirDC.h>
class AirSensor
{
  public:
    AirSensor(int pid);
    void ReadDifferentialPressure(AirDC *out,int sensor);
    void ReadStaticPressure(AirDC *out,int sensor);
    void ReadTAT(AirDC *out,int sensor);
    void ReadRH(AirDC *out,int sensor);
    int _pid;
};
#endif
