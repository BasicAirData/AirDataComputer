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
#ifndef AirDC_h
#define AirDC_h
#include <Arduino.h>
#include <TimeLib.h>
#define DEFAULT_LOG_FILE "DATALOG.CSV"

#define AIRDC_DATA_TIME          0   // Timestamp
#define AIRDC_DATA_QCRAW         1   // Differential pressure [count]
#define AIRDC_DATA_PRAW          2   // Absolute pressure [count]
#define AIRDC_DATA_TRAW          3   // External Temperature sensor [count]
#define AIRDC_DATA_TDELTAPRAW    4   // Temperature differential pressure sensor [count]
#define AIRDC_DATA_TABSPRAW      5   // Temperature absolute pressure sensor [count]
#define AIRDC_DATA_QC            6   // Differential pressure [Pa]
#define AIRDC_DATA_P             7   // Static pressure [Pa]
#define AIRDC_DATA_TAT           8   // TAT External Temperature [K]
#define AIRDC_DATA_TDELTAP       9   // Temperature differential pressure sensor [K]
#define AIRDC_DATA_TABSP         10  // Temperature absolute pressure sensor [K]
#define AIRDC_DATA_IAS           11  // Indicated Air Speed [m/s]
#define AIRDC_DATA_TAS           12  // True Air Speed [m/s]
#define AIRDC_DATA_H             13  // Barometric altitude [m]
#define AIRDC_DATA_T             14  // OAT [K]
#define AIRDC_DATA_MILLIS        15  // Internal time Milliseconds
#define AIRDC_DATA_UIAS          16  // Uncertainty IAS [m/s]
#define AIRDC_DATA_UTAS          17  // Uncertainty TAS [m/s]
#define AIRDC_DATA_UH            18  // Uncertainty Altitude [m]
#define AIRDC_DATA_UT            19  // Uncertainty OAT [K]
#define AIRDC_DATA_RHO           20  // Air Density [kg/m^3]
#define AIRDC_DATA_MU            21  // Dynamic Air Viscosity [Pa*s]]
#define AIRDC_DATA_RE            22  // Reynolds number
#define AIRDC_DATA_C             23  // c factor

#define AIRDC_DATA_VECTOR_SIZE   24  // The size of this data vector


#define AIRDC_STATUS_SD          0   // SD Card
#define AIRDC_STATUS_DELTAP      1   // Differential pressure sensor
#define AIRDC_STATUS_P           2   // Absolute pressure sensor
#define AIRDC_STATUS_TAT         3   // External Temperature sensor
#define AIRDC_STATUS_TDELTAP     4   // Temperature differential pressure sensor
#define AIRDC_STATUS_TABSP       5   // Temperature absolute pressure sensor
#define AIRDC_STATUS_RTCBATT     6   // Real time clock battery
#define AIRDC_STATUS_ERRWARN     7   // Error/Warning
#define AIRDC_STATUS_BLUETOOTH   8   // Bluetooth

#define AIRDC_STATUS_VECTOR_SIZE 9   // The size of this status vector


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
    double _qc;/**< Differential pressure at Pitot, Pascal*/
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

