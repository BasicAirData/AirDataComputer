/*
  AirDC.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/

#ifndef AirDC_h
#define AirDC_h
#include "Arduino.h"
class AirDC
{
  public:
    AirDC(int pid);
    double Rho(double T, double p,double RH);
  private:
    int _pid;
    double _p;
    double _t;
    double _RH;
};
#endif

