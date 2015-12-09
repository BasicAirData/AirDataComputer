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
    void RhoAir(int mode);
    void IAS(int mode);
    void CAS(int mode);
    void TAS(int mode);
    void Mach(int mode);
    void OAT(int mode);
    void ISAAltitude(int mode);
    void OutputSerial(int mode);
//General use
    int _pid;
    String _StreamOut;
//Measurements
    double _p;
    double _T;
    double _RH;
    double _qc;
//AirData
    double _Rho;
    double _IAS;
    double _CAS;
    double _TAS;
    double _M;
    double _TAT;
    double _h;
//Measurements Uncertainty
    double _up;
    double _uT;
    double _uRH;
    double _uqc;
//AirData Uncertainty
    double _uRho;
    double _uIAS;
    double _uCAS;
    double _uTAS;
    double _uTAT;
    double _uh;
};
#endif

