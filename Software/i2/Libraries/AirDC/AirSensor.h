/**
 * AirSensor.h - Library for sensor interfacing
 * Created by J.L.J., December 3, 2015.\n Refer to http:\\www.basicairdata.eu
*/

#ifndef AirSensor_h
#define AirSensor_h
//#include "Arduino.h"
#include <AirDC.h>
/** AirSensor class
*handles hardware specific stuff for some sensors
*sends the output to an AirDC object
*/
class AirSensor
{
  public:
    AirSensor(int pid);
    void ReadDifferentialPressure(AirDC *out,int sensor); /**< Acquire differeintial pressure*/
    void ReadStaticPressure(AirDC *out,int sensor);/**< Acquire static pressure*/
    void ReadTAT(AirDC *out,int sensor);/**< Acquire total air temperature*/
    void ReadRH(AirDC *out,int sensor);/**< Acquire relative humidity*/
    int _pid;
};
#endif
