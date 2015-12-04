/*
  AirDC.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include "AirDC.h"
#include <math.h>

AirDC::AirDC(int pid)
{
    //pinMode(pin, OUTPUT);
    _pid = pid;
}
//Rho(Pressure,Temperature,Relative Humidity)
//http://www.basicairdata.eu/calculation-routines.html
double AirDC::Rho(double p, double T,double RH)
{
//Some definition
    const double R= 8.314510;//J/(mol°K)
    const double xco2=0.0004;//Co2 fraction
    const double A=1.2378847e-5;
    const double B=-1.9121316e-2;
    const double C=33.93711047;
    const double D=-6.3431645e3;
    const double alfa=1.00062;
    const double bet=3.14e-8;
    const double gama=5.6e-7;
    const double a0=1.58123e-6;
    const double a1=-2.9331e-8;
    const double a2=1.1043e-10;
    const double b0=5.707e-6;
    const double b1=-2.051e-8;
    const double c0=1.9898e-4;
    const double c1=-2.376e-6;
    const double d=1.83e-11;
    const double e=- 0.765e-8;
    const double Ma=28.9635 + 12.011*(xco2- 0.0004);
    const double Mv=18.01528;
    double psv,t,f,xv,Z,rho,h;
    h=RH;
    psv=1*exp(A*pow(T,2)+B*T+C+D/T);
    t=T-273.15;
    f=alfa+bet*p+gama*pow(t,2);
    xv=h*f*psv/p;
    Z=1-p/T*(a0+a1*t+a2*pow(t,2)+(b0+b1*t)*xv+(c0+c1*t)*pow(xv,2))+pow(p,2)/pow(T,2)*(d+e*pow(xv,2));
    rho=p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001;
    //Class Variables undate and Output
    return rho;
}
