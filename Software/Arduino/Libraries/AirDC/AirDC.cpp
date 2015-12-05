/*
  AirDC.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include "AirDC.h"
#include <math.h>

AirDC::AirDC(int pid)
{
    //Default parameters values
    _pid = pid;
    //Parameter values
    _Rho=1.225;
    _p=101325;
    _T=288.15;
    _RH=0.0;
    _qc=0.0;
    _IAS=0.0;
    //Uncertainty of measurements
    _uRho=0.0; //To be calculated, 0 default value
    _up=5.0;
    _uT=0.8;
    _uRH=0.05;
    _uqc=5.0;
    _uIAS=0.0;//To be calculated, 0 default value
}
//RhoAir(Pressure,Temperature,Relative Humidity,mode)
//Mode 1 is the default BasicAirData routine
//http://www.basicairdata.eu/calculation-routines.html
void AirDC::RhoAir(double p, double T,double RH, int mode)
{
    switch (mode)
    {
    case 1:
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
        double psv,t,f,xv,Z;
        double Sp,ST,SRH; //sensibility factors
        _p=p;
        _T=T;
        _RH=RH;
//Sequential computation
        psv=1*exp(A*pow(T,2)+B*T+C+D/T);
        t=T-273.15;
        f=alfa+bet*p+gama*pow(t,2);
        xv=RH*f*psv/p;
        Z=1-p/T*(a0+a1*t+a2*pow(t,2)+(b0+b1*t)*xv+(c0+c1*t)*pow(xv,2))+pow(p,2)/pow(T,2)*(d+e*pow(xv,2));
        //Assign calculated values to actual status
        _Rho=p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001;
        //Calculates Sensibility factor for p
        p=p+10;
        psv=1*exp(A*pow(T,2)+B*T+C+D/T);
        t=T-273.15;
        f=alfa+bet*p+gama*pow(t,2);
        xv=RH*f*psv/p;
        Z=1-p/T*(a0+a1*t+a2*pow(t,2)+(b0+b1*t)*xv+(c0+c1*t)*pow(xv,2))+pow(p,2)/pow(T,2)*(d+e*pow(xv,2));
        Sp=((p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001)-_Rho)/10;
        p=p-10;
        //Calculates Sensibility factor for T
        T=T+10;
        psv=1*exp(A*pow(T,2)+B*T+C+D/T);
        t=T-273.15;
        f=alfa+bet*p+gama*pow(t,2);
        xv=RH*f*psv/p;
        Z=1-p/T*(a0+a1*t+a2*pow(t,2)+(b0+b1*t)*xv+(c0+c1*t)*pow(xv,2))+pow(p,2)/pow(T,2)*(d+e*pow(xv,2));
        ST=((p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001)-_Rho)/10;
        T=T-10;
//Calculates Sensibility factor for RH
        RH=RH+0.1;
        psv=1*exp(A*pow(T,2)+B*T+C+D/T);
        t=T-273.15;
        f=alfa+bet*p+gama*pow(t,2);
        xv=RH*f*psv/p;
        Z=1-p/T*(a0+a1*t+a2*pow(t,2)+(b0+b1*t)*xv+(c0+c1*t)*pow(xv,2))+pow(p,2)/pow(T,2)*(d+e*pow(xv,2));
        SRH=((p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001)-_Rho)*10;
        _uRho=sqrt(Sp*Sp*_up*_up+ST*ST*_uT*_uT+SRH*SRH*_uRH*_uRH);
        break;
    }
}
void AirDC::IAS(double qc,int mode)
{
    switch (mode){
        case 1:
    _IAS=1.27775310604201*sqrt(qc);
    _uIAS=0.638876553021004/(sqrt(qc))*_uqc;
    break;
    }
}
