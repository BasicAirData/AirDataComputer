/*
  AirDC.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/

#ifndef AirSensor_h
#define AirSensor_h
#include "Arduino.h"
#include "AirDc.h"
class AirSensor
{
  public:
    AirSensor(int pid);
    void ReadDifferentialPressure(AirDC *out,int mode);
  //private:
    int _pid;
 /*   double _p;
    double _T;
    double _RH;
    double _qc;
    double _up;
    double _uT;
    double _uRH;
    double _uqc;*/
};
#endif
