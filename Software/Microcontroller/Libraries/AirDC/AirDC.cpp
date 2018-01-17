/**
* AirDC - Library for Basic Air Data calculations
* J.L.J (C)2015, Basic Air Data Team.\n Refer to http:\\www.basicairdata.eu
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implie  d warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "AirDC.h"
#include "MatrixMath.h"
#include <math.h>
#include <Arduino.h>
#include <TimeLib.h>

AirDC::AirDC(int pid)
{
    /** AirDC Default
    constructor*/
    //Default parameters values
    _pid = pid;
    //Geometric
    _d=0.008;
    _PitotXcog=0.5;
    _PitotYcog=0;
    _PitotZcog=0;
    //Parameter values
    _Rho=1.225;
    _p=90000;
    _T=288.15;
    _RH=0.0;
    _qc=350;
    _pSeaLevel=101325; //Value of pressure at sea level
    _AOA=0.17;
    _AOS=0.00;
    _IAS=0.0;
    _TAT=288.15;
    _TASPCorrected=0.0;
    _AOAdot=1;
    _AOSdot=0;
    //Uncertainty of measurements
    _uRho=0.0; //To be calculated, 0 default value
    _up=5.0;
    _uT=0; //To be calculated
    _uRH=0.05;
    _uqc=5.0;
    _uIAS=0.0;//0 default value
    _uTAT=0.25;//From sensor specs 0 default value
//Inertial Unit
    _Ip=0;
    _Iq=3;
    _Ir=0;
    _c=1; //Probe calibration factor

}
/** Calculates the Air Density
* @param  Mode Indicates the calculation method. 1 is Basic Air Data default http://www.basicairdata.eu/calculation-routines.html
* @return Void
 */
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
                ;                //Calculates Sensibility factor for p
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
/**Calcualtes Indicated Airspeed
* IAS=ASI=EAS
* @param  Mode Indicates the calculation method. 1 is Basic Air Data default http://www.basicairdata.eu/pitot-tube.html\n https://en.wikipedia.org/wiki/Equivalent_airspeed
* @return Void
*/
void AirDC::IAS(int mode)
{
    switch (mode)
    {
    case 1:
        if (_qc<0)
        {
            _qc=-1*_qc;
        }
        _IAS=1.27775310604201*sqrt(_qc);
        _uIAS=0.638876553021004/(sqrt(abs(_qc))*_uqc;
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
        _CAS=_c*_IAS;
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
/** Calculates Outside Air Temperature
* @param  Mode Indicates the calculation method. 1 is Basic Air Data default https://en.wikipedia.org/wiki/Total_air_temperature
* @return Void
 */
void AirDC::OAT(int mode)//Outside Air Temperature.http://www.basicairdata.blogspot.it/2013/05/resistance-temperature-detectors-for.html
{
    switch (mode)
    {
    case 1:
    {
        double gamma=1.4; //https://en.wikipedia.org/wiki/Julius_von_Mayer#Mayer.27s_relation

        _T=_TAT/(1+(gamma-1)/2*pow(_M,2));
        _uT=1/(1+(gamma-1)/2*2*pow(_M,2))*_uTAT;
        break;
    }
    }
}
/** Calculates barometric altitude with ISA atmosphere
* @param  Mode 1 Uncorrected altitude above mean sea level http://www.basicairdata.eu/altimeter.html
* @param  Mode 2 Corrected above mean sea level altitude, pressure at sea level should be available, https://en.wikipedia.org/wiki/QNH
* @return Void
 */
void AirDC::ISAAltitude(int mode)
{
    switch (mode)
    {
    case 1: //Uncorrected above mean sea level altitude http://www.basicairdata.eu/altimeter.html
    {
        double Ps,h;
        Ps=_p*0.000295299875080277;//Pa to inHg Conversion
        _h=(pow(29.92126,0.190255)-pow(Ps,0.190255))*76189.2339431570; //Conform to Goodrich 4081 Air data handbook formula, US atmosphere 1962
        _h=_h*0.3048;//Back to SI
        _uh=4418.19264813511*pow(Ps,-0.809745)*_up*0.000295299875080277;
        break;
    }
    case 2:
//Corrected above mean sea level altitude. Pressure at sea level value  should be available.
//Sea level pressure should be put into _pSeaLevel. Mimics https://en.wikipedia.org/wiki/QNH
    {
        int i;
        double f,fdot,t0,t1,t,erralt;
        i=0;
        t0=1000; //Newton's initial value
        erralt=0; //Newton's initial value
        while ((abs(erralt)>(0.01)) || (i==0))
        {
            t=t0;
            f=_p-_pSeaLevel*pow((1-0.000022557695644629534*t),(5.255786239252914));
            fdot=_pSeaLevel*0.00011855842635829929*pow((1-0.000022557695644629534*t),(4.255786239252914));
            t1=t0-f/fdot;
            erralt=t1-t0;
            t0=t1;
            i=i+1;
        }
        _h=t1;
        _uh=erralt+4418.19264813511*pow(Ps,-0.809745)*_up*0.000295299875080277;
        break;
    }
    }
}
/** Correct TAS based on pitot placement
* @param  Mode 1 No compensation
* @param  Mode 2 Steady state(no angular acceleration) assumed for this method \n http://basicairdata.blogspot.it/2014/07/pitot-correction-for-position-and.html
* @return Void
 */
void AirDC::PitotCorrection(int mode)//Based on http://basicairdata.blogspot.it/2014/07/pitot-correction-for-position-and.html
{
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
        PB[0][0]=_PitotXcog; //Installation position respect c.o.g.
        PB[1][0]=_PitotYcog;
        PB[2][0]=_PitotZcog;
        WB[0][0]=_Ip-_AOSdot*sin(_AOA);   //Angular rates . P, q, r from sensors and
        //WB[1][0]=_Iq-_AOAdot;
        WB[1][0]=0;
        WB[2][0]=_Ir+_AOSdot*cos(_AOA);
//Matrix.Print((float*)PB,3,1,"PB");
        R[0][0]=cos(_AOA)*cos(_AOS);
        R[0][1]=sin(_AOS);
        R[0][2]=sin(_AOA)*cos(_AOS);
        R[1][0]=-1*cos(_AOA)*sin(_AOS);
        R[1][1]=cos(_AOS);
        R[1][2]=-1*sin(_AOA)*sin(_AOS);
        R[2][0]=-1*sin(_AOA);
        R[2][1]=0;
        R[2][2]=cos(_AOA);
        Matrix.Multiply((float*)R,(float*)PB,3,3,1,(float*)PW);//Calculation of Position vector in wind axes
        Matrix.Multiply((float*)R,(float*)WB,3,3,1,(float*)WW);//Calculation of angular rates at tip in wind frame.
        PWDOT[0][0]=WW[1][0]*PW[2][0]-WW[2][0]*PW[1][0];//Calculation of velocity vector at tip in wind coordinates. Cross product WWxPW.
        PWDOT[1][0]=WW[2][0]*PW[0][0]-WW[0][0]*PW[2][0];
        PWDOT[2][0]=WW[0][0]*PW[1][0]-WW[1][0]*PW[0][0];
        VCorrected[0][0]=_TAS-PWDOT[0][0];//Airspeed vector
        VCorrected[1][0]= -PWDOT[1][0];
        VCorrected[2][0]= -PWDOT[2][0];
        _TASPCorrected=sqrt(pow(VCorrected[0][0],2)+pow(VCorrected[1][0],2)+pow(VCorrected[2][0],2));
        break;
    }
    }
}
/** Calculates Air Viscosity
* @param  Mode 1 Calculate viscosity in Pas with Sutherland's formula
* @param  Mode 2 Calculate viscosity 10-6*Pas with Sutherland's formula
* @return Void
 */
void AirDC::Viscosity(int mode)
{
    switch(mode)
    {
    case 1://Calculate viscosity in Pas with Sutherland's formula.
    {
        _mu= 18.27*(291.15+120)/(_T+120)*pow((_T/291.15),(3/2))*1e-6;
        break;
    }
    case 2: //Calculate viscosity in 10-6*Pas with Sutherland's formula.
    {
        _mu= 18.27*(291.15+120)/(_T+120)*pow((_T/291.15),(3/2));
    }
    }
}
/** Calculates Re number
* @param  Mode 1 Uses _d as reference dimension
* @return Void
 */
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
/** Returns the calibration factor for the probe in function of operative conditions
* @param  Mode 1 fixed value c=1
* @param  Mode 2 Basic Air Data 8 mm Probe
* @return Void
 */
void AirDC::CalibrationFactor(int mode)
{
    switch(mode)
    {
    case 1:
    {
        _c=1;
        break;
    }
    case 2:
    {
        _c=-7.961198906081004E-5*_TAS+1.0007625874125878;
        break;
    }
    }
}
/** Order all the data within an array
* @return Void
 */
void AirDC::PrepareData(void) //Refresh and reorder data following #10 message order
{
    /*
    1  Timestamp see msg #4
    2  Deltap [Sensor units, counts]
    3  Absolute Pressure [Sensor units, counts]
    4  Ext Temperature [Sensor units, counts]
    5  Temp deltap [Sensor units, counts]
    6  Temp absolute [Sensor units, counts]
    7  Deltap [Pa]
    8  Absolute Pressure [Pa]
    9  Ext Temperature [K]
    10 Temp deltap [K]
    11 Temp absolute [K]
    12 IAS [m/s]
    13 TAS [m/s]
    14 Altitude [m]
    15 OAT [K]
    16 Relative time, milliseconds
    17 Uncertainty IAS [m/s]
    18 Uncertainty TAS [m/s]
    19 Uncertainty Altitude [m]
    20 Uncertainty OAT [K]
    21 Air Density [kg/m^3]
    22 Air Viscosity[Pas*10^-6]
    23 Re
    24 c factor
    */
    _dataout[AIRDC_DATA_TIME]       = (double)(now()); // Time
    _dataout[AIRDC_DATA_QCRAW]      = _qcRaw;          // Differential pressure [count]
    _dataout[AIRDC_DATA_PRAW]       = _pRaw;           // Absolute pressure [count]
    _dataout[AIRDC_DATA_TRAW]       = _TRaw;           // External Temperature sensor [count]
    _dataout[AIRDC_DATA_TDELTAPRAW] = _TdeltapRaw;     // Temperature differential pressure sensor [count]
    _dataout[AIRDC_DATA_TABSPRAW]   = _TabspRaw;       // Temperature absolute pressure sensor [count]
    _dataout[AIRDC_DATA_QC]         = _qc;             // Differential pressure [Pa]
    _dataout[AIRDC_DATA_P]          = _p;              // Static pressure [Pa]
    _dataout[AIRDC_DATA_TAT]        = _TAT;            // TAT External Temperature [K]
    _dataout[AIRDC_DATA_TDELTAP]    = _Tdeltap;        // Temperature differential pressure sensor [K]
    _dataout[AIRDC_DATA_TABSP]      = _Tabsp;          // Temperature absolute pressure sensor [K]
    _dataout[AIRDC_DATA_IAS]        = _IAS;            // Indicated Air Speed [m/s]
    _dataout[AIRDC_DATA_TAS]        = _TAS;            // True Air Speed [m/s]
    _dataout[AIRDC_DATA_H]          = _h;              // Barometric altitude [m/s]
    _dataout[AIRDC_DATA_T]          = _T;              // OAT [K]
    _dataout[AIRDC_DATA_MILLIS]     = millis();        // Internal time [milliseconds]
    _dataout[AIRDC_DATA_UIAS]       = _uIAS;           // Uncertainty IAS [m/s]
    _dataout[AIRDC_DATA_UTAS]       = _uTAS;           // Uncertainty TAS [m/s]
    _dataout[AIRDC_DATA_UH]         = _uh;             // Uncertainty Altitude [m]
    _dataout[AIRDC_DATA_UT]         = _uT;             // Uncertainty OAT [K]
    _dataout[AIRDC_DATA_RHO]        = _Rho;            // Air Density [kg/m^3]
    _dataout[AIRDC_DATA_MU]         = _mu ;            // Dynamic Air Viscosity [Pa*s]]
    _dataout[AIRDC_DATA_RE]         = _Re ;            // Reynolds number
    _dataout[AIRDC_DATA_C]          = _c ;             // c factor
}
