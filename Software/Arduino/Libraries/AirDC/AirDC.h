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
    void RhoAir(double p, double T,double RH,int mode);
    void IAS(double qc,int mode);
  //private:
    int _pid;
    double _Rho;
    double _p;
    double _T;
    double _RH;
    double _qc;
    double _IAS;
    double _up;
    double _uT;
    double _uRH;
    double _uRho;
    double _uqc;
    double _uIAS;
};
#endif

