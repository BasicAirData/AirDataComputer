/*
  AirDC.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include "AirDC.h"
#include "MatrixMath.h"
#include <math.h>

AirDC::AirDC(int pid)
{
    //Default parameters values
    _pid = pid;
    //Geometric
    _d=0.008;
    //Parameter values
    _Rho=1.225;
    _p=101325;
    _T=288.15;
    _RH=0.0;
    _qc=0.0;
    _AOA=0.17;
    _AOS=0.01;
    _IAS=0.0;
    _TASPCorrected=0.0;
    //Uncertainty of measurements
    _uRho=0.0; //To be calculated, 0 default value
    _up=5.0;
    _uT=0; //To be calculated
    _uRH=0.05;
    _uqc=5.0;
    _uIAS=0.0;//To be calculated, 0 default value
    _uTAT=0.0;//To be calculated, 0 default value
//Inertial Unit
    _Ip=0;
    _Iq=2.6;
    _Ir=0;
    _Ipdot=0.00;
    _Iqdot=0.00;
    _Irdot=0.00;
}
//RhoAir(Pressure,Temperature,Relative Humidity,mode)
//Mode 1 is the default BasicAirData routine
//http://www.basicairdata.eu/calculation-routines.html
void AirDC::RhoAir(int mode)
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
        double p,T,RH,psv,t,f,xv,Z;
        double Sp,ST,SRH; //sensibility factors
        p=_p;
        T=_T;
        RH=_RH;
        for (int i=1; i<5; i++)
        {
            switch (i)
            {
            case 1:
            {
                p=p+10;
                break;
            }
            case 2:
            {
                p=p+10;
                break;
            }
            case 3:
            {
                p=p-10;
                T=T+10;
                break;
            }
            case 4:
            {
                T=T-10;
                RH=RH+0.1;
                break;
            }
            }
            psv=1*exp(A*pow(T,2)+B*T+C+D/T);
            t=T-273.15;
            f=alfa+bet*p+gama*pow(t,2);
            xv=RH*f*psv/p;
            Z=1-p/T*(a0+a1*t+a2*pow(t,2)+(b0+b1*t)*xv+(c0+c1*t)*pow(xv,2))+pow(p,2)/pow(T,2)*(d+e*pow(xv,2));

            switch (i)
            {
            case 1:
            {
                //Calculates Sensibility factor for p
                _Rho=p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001;
                break;
            }
            case 2:
            {
                //Calculates Sensibility factor for p
                Sp=((p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001)-_Rho)/10;
                break;
            }
            case 3:
            {
                //Calculates Sensibility factor for T
                ST=((p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001)-_Rho)/10;
                break;
            }
            case 4:
            {
                //Calculates Sensibility factor for RH
                SRH=((p*Ma/(Z*R*T)*(1-xv*(1-Mv/Ma))*0.001)-_Rho)*10;
                _uRho=sqrt(Sp*Sp*_up*_up+ST*ST*_uT*_uT+SRH*SRH*_uRH*_uRH);
                break;
            }
            }
        }
        break;
    }

}
void AirDC::IAS(int mode)
{
//Indicated Airspeed
//IAS=ASI=EAS
    //http://www.basicairdata.eu/pitot-tube.html
    //https://en.wikipedia.org/wiki/Equivalent_airspeed
    switch (mode)
    {
    case 1:
        if (_qc<0)
        {
            _qc=0;
        }
        _IAS=1.27775310604201*sqrt(_qc);
        if (_qc>0)
        {
            _uIAS=0.638876553021004/(sqrt(_qc))*_uqc;
        }
        else
        {
            _uIAS=0;
        }
        break;
    }
}
void AirDC::CAS(int mode)
{
//Calibrated Airspeed
    //http://www.basicairdata.eu/pitot-tube.html
    //https://en.wikipedia.org/wiki/Equivalent_airspeed
    switch (mode)
    {
    case 1:
        //Here the calibration table should be lookup.
        _CAS=_IAS;
        _uCAS=_uIAS;
        break;
    }
}
void AirDC::TAS(int mode)
{
//True Airspeed
//http://www.basicairdata.eu/pitot-tube.html
//TAS=IAS*(rhostandard/rhoair)^0.5
    _TAS=_CAS*sqrt(1.225/_Rho);
    _uTAS= sqrt((1.225/_Rho)*pow(_uCAS,2)+ pow(0.5*_CAS*1.225/(pow(_Rho,1.5)),2)*pow(_uRho,2));
}
void AirDC::Mach(int mode)
{
    switch (mode)
    {
    case 1:
    {
        //  http://www.basicairdata.eu/air-properties.html
        //  https://en.wikipedia.org/wiki/Julius_von_Mayer#Mayer.27s_relation
        double gamma=1.4; // cp/cv;
        double R,Ma,Rair;
        R=8314.459848; // J/K/mol
        Ma=0.0289645; //Kg/mol  Molecular mass of dry air
        Rair=R/Ma;
        _M=_TAS/(sqrt(gamma*Rair*_T));
        break;
    }

    }

}

void AirDC::OAT(int mode)
{
//Outside Air Temperature
//http://www.basicairdata.blogspot.it/2013/05/resistance-temperature-detectors-for.html
//https://en.wikipedia.org/wiki/Total_air_temperature
    switch (mode)
    {
    case 1:
    {
        double gamma=1.4; //cp/cv
        //https://en.wikipedia.org/wiki/Julius_von_Mayer#Mayer.27s_relation

        _T=_TAT/(1+(gamma-1)/2*pow(_M,2));
        _uT=1/(1+(gamma-1)/2*2*pow(_M,2))*_uTAT;
    }
    }

}
void AirDC::ISAAltitude(int mode)
{
    switch (mode)
    {
    case 1:
    {
        double Ps,h;
        Ps=_p*0.000295299875080277;//Pa to inHg Conversion
        //Using Goodrich 4081 Air data handbook formula

        _h=(pow(29.92126,0.190255)-pow(Ps,0.190255))*76189.2339431570; //US atmosphere 1962
        //Back to SI
        _h=_h*0.3048;
        //76189.2339431570*(_p*0.000295299875080277)^0.190255
        _uh=4418.19264813511*pow(Ps,-0.809745)*_up*0.000295299875080277;
    }
    }

}
String AirDC::OutputSerial(int mode)
{
    String StreamOut;
    switch(mode)
    {
    case 1: //Measurements output
    {
//_p,_T,_RH,_qc,AOA,AOS
        String s1(_p, 6);
        String s2(_T, 6);
        String s3(_RH, 6);
        String s4(_qc, 6);
        String s5(_AOA, 6);
        String s6(_AOS, 6);
        StreamOut='$TMO,'+s1+','+s2+','+s3+','+s4+','+s5+','+s6;
//To read string on the other side
        /*
          if (Serial.find("$TMO,")) {
            _p = Serial.parseFloat(); //
            _T = Serial.parseFloat();//
            _RH = Serial.parseFloat();//
            _qc = Serial.parseFloat();//
        */
        break;
    }
    case 2: //Air data output
        //_Rho,_IAS,_CAS,_TAS,_TASPCorrected,_M,_TAT,_h,_mu,_Re
    {
        String s1(_Rho, 6);
        String s2(_IAS, 6);
        String s3(_CAS, 6);
        String s4(_TAS, 6);
        String s5(_TASPCorrected, 6);
        String s6(_M, 6);
        String s7(_TAT, 6);
        String s8(_h, 6);
        String s9(_mu, 6);
        String s10(_Re, 6);
        StreamOut='$TAD,'+s1+','+s2+','+s3+','+s4+','+s5+','+s6+','+s7+','+s8+','+s9+','+s10;
        break;
    }
    case 3: //Measurements uncertainty output
        //_up,_uT,_uRH,_uqc
    {
        String s1(_up, 6);
        String s2(_uT, 6);
        String s3(_uRH, 6);
        String s4(_uqc, 6);
        StreamOut='$TMU,'+s1+','+s2+','+s3+','+s4;
        break;
    }
    case 4: //Air data uncertainty output
        //_uRho,_uIAS,_uCAS,_uTAS,_uTAT,_uh;
    {
        String s1(_uRho, 6);
        String s2(_uIAS, 6);
        String s3(_uCAS, 6);
        String s4(_uTAS, 6);
        String s5(_uTAT, 6);
        String s6(_uh, 6);
        StreamOut='$TAU,'+s1+','+s2+','+s3+','+s4+','+s5+','+s6;
        break;
    }
    return StreamOut;
    }
}
void AirDC::PitotCorrection(int mode)
{
//Based on
//http://basicairdata.blogspot.it/2014/07/pitot-correction-for-position-and.html
    switch (mode)
    {
    case 1: //No_compensation
    {
    _TASPCorrected=_TAS;
    break;
    }
    case 2:  //Steady state(no angular acceleration) assumed for this method
    {
        float R[3][3];
        float PB[3][1]; //Position of probe tip in body coordinates
        float WB[3][1]; //Angular rates in body coordinates [p q r]'
        float WW[3][1];//Angular rates in wind coordinates [p q r]'
        float PW[3][1]; //Position of probe tip in wind ref. frame
        float PWDOT[3][1]; //Velocity of tip in wind ref. frame
        float VCorrected[3][1];  //Measured Airspeed
        PB[1][1]=0.5; //Installation position respect c.o.g.
        PB[2][1]=0;
        PB[3][1]=0;
        WB[1][1]=_Ip;   //Angular rates . P, q, r from sensors
        WB[2][1]=_Iq;
        WB[3][1]=_Ir;

        R[1][1]=cos(_AOA)*cos(_AOS);
        R[1][2]=sin(_AOS);
        R[1][3]=sin(_AOA)*sin(_AOS);
        R[2][1]=-1*cos(_AOA)*sin(_AOS);
        R[2][2]=cos(_AOS);
        R[2][3]=-1*sin(_AOA)*sin(_AOS);
        R[3][1]=-1*sin(_AOA);
        R[3][2]=0;
        R[3][3]=cos(_AOA);
        Serial.Println("Debug")
        Matrix.Print((float*)R,3,3,"R");

//Calculation of Position vector in wind axes
        Matrix.Multiply((float*)R,(float*)PB,3,3,1,(float*)PW);
//Calculation of angular rates at tip in wind frame. Attention, assumed low angular acceleration. High rates in another method.
        Matrix.Multiply((float*)R,(float*)WB,3,3,1,(float*)WW);
        Serial.Println("Debug")
        Matrix.Print((float*)PW,3,1,"PW");
        Matrix.Print((float*)WW,3,1,"WW");
//Calculation of velocity vector at tip in wind coordinates
//Cross product WWxPW
        PWDOT[1][1]=WW[2][1]*PW[3][1]-WW[3][1]*PW[2][1];
        PWDOT[2][1]=WW[3][1]*PW[1][1]-WW[1][1]*PW[3][1];
        PWDOT[3][1]=WW[1][1]*PW[2][1]-WW[2][1]*PW[1][1];
//Airspeed vector
        VCorrected[1][1]=_TAS-PWDOT[1][1];
        VCorrected[2][1]= -PWDOT[2][1];
        VCorrected[3][1]= -PWDOT[3][1];
        Serial.Println("Debug")
        Matrix.Print((float*)VCorrected,3,1,"VCorrected");
        // _TASPCorrected=sqrt(pow(VCorrected[1][1],2)+pow(VCorrected[2][1],2)+pow(VCorrected[3][1],2));
        _TASPCorrected=0;
        break;
    }
    }
}
void AirDC::Viscosity(int mode)
{
    switch(mode)
    {
    case 1:
    {
        //Calculate viscosity. Sutherland's formula, note that unit number multiplied 10e6
        _mu= 18.27*(291.15+120)/(_T+120)*pow((_T/291.15),(3/2))*1e-6;
    }
    }
}
void AirDC::Red(int mode)
{
    switch(mode)
    {
    case 1:
    {
        _Re=_Rho*_TAS*_d/_mu;
    }
    }
}
