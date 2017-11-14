 /**
 * AirDC - Library for Basic Air Data calculations
 * Created by J.L.J., December 3, 2015.\n Refer to http:\\www.basicairdata.eu
 * Status: The big updates are coming 04/2017 JLJ
*/

#ifndef AirDC_h
#define AirDC_h
#include <Arduino.h>
#include <TimeLib.h>
#define DEFAULT_LOG_FILE "datalog.csv"
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
   String OutputSerial(int mode);
//Correction and Auxiliary
    void PitotCorrection(int mode);
    void Viscosity(int mode);
    void Red(int mode);
//Calibration factor
    void CalibrationFactor(int mode);
//Prepare data for output
    void PrepareData(void);
//General use
    int _pid;/**< Class ID */
//Hardware configuration
    char _status[11];/**< Hardware Status vector see communication protocol msg #5*/
//Active log file
    char _logfile[15]=DEFAULT_LOG_FILE;/**< Defines the active log file name */
//Message handling configuration
    char _datasel[25]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','\0'};/**< Data selection vector see communication protocol msg #9*/
    double _dataout[25]; //Double array that contains data ready to be send out
//Geometric
    double _d;/**< Reference length for Re number calculation*/
    double _PitotXcog;/**< Distance along x body axes of the Pitot tip from center of gravity*/
    double _PitotYcog;/**< Distance along y body axes of the Pitot tip from center of gravity*/
    double _PitotZcog;/**< Distance along z body axes of the Pitot tip from center of gravity*/
//Measurements
    double _p;/**< Static pressure Pa*/
    double _pRaw;/**< Static pressure raw sensor value*/
    double _T;/**< Temperature K*/
    double _TRaw;/**< Temperature sensor counts*/
    double _RH;/**< Relative Humidity*/
    double _qc;/**< Differential pressure at Pitot, sensor counts*/
    double _qcRaw;/**< Raw sensor value of differential pressure at Pitot, Impact pressure minus static pressure Pa*/
    double _AOA;/**< Angle of Attack, rads*/
    double _AOS;/**< Angle of Sideslip, rads*/
    double _pSeaLevel; /**< Value of pressure at sea level Pa*/
    double _Tdeltap;/**<Temperature at deltap sensor*/
    double _Tabsp;/**<Temperature at absolute p sensor*/
    double _TdeltapRaw;/**<Raw temperature at deltap sensor*/
    double _TabspRaw;/**<Raw temperature at absolute p sensor*/
//AirData
    double _Rho;/**< Air Density kg/m^3*/
    double _IAS;/**< Indicated Air speed m/s*/
    double _CAS;/**< Calibrated Air Speed m/s*/
    double _TAS;/**< True Air Speed m/s*/
    double _TASPCorrected;/**< True Air Speed, corrected m/s*/
    double _M;/**< Mach number*/
    double _TAT;/**< Total Air Temperature K*/
    double _h;/**< Altitude m*/
    double _mu;/**< Dynamic Viscosity Pas*/
    double _Re;/**< Reynolds Number*/
    double _AOAdot;/**< Time derivate of AOA rad/s*/
    double _AOSdot;/**< Time derivate of AOS rad/s*/
//Measurements Uncertainty
    double _up;/**< Pressure uncertainty Pa*/
    double _uT;/**< Temperature uncertainty Pa*/
    double _uRH;/**< Relative Humidity uncertainty*/
    double _uqc;/**< Differential pressure uncertainty Pa*/
//AirData Uncertainty
    double _uRho;/**< Air density uncertainty kg/^3*/
    double _uIAS;/**< IAS uncertainty*/
    double _uCAS;/**< CAS uncertainty*/
    double _uTAS;/**< TAS uncertainty*/
    double _uTAT;/**< TAT uncertainty*/
    double _uh;/**< Altitude uncertainty*/
//Inertial Unit
    double _Ip;/**< Pitch rate*/
    double _Iq;/**< Roll rate*/
    double _Ir;/**< yaw rate*/
    double _c; /**Probe calibration factor*/
};
#endif

