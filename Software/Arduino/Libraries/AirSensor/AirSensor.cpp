/*
  AirSensor.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include "AirSensor.h"
#include <math.h>
#include <Wire.h>

AirSensor::AirSensor(int pid)
{
    //Default parameters values
    _pid = pid;
}
void AirSensor::Pressure(int sensor)
{
    switch (sensor)
    {
    case 1: //Sensor one is I2C HLCA12X5
        //http://www.first-sensor.com/cms/upload/datasheets/DS_Standard-HCLA_E_11629.pdf
        //Basic software for HLCA12X5 from Sensortechnics/First
        // Pressure  - Sensor output hex/dec
        //12.5  mBar 1250 Pa - 6CCCx /27852d
        //-12.5 mBar 1250 Pa - 0666x /1638d
        // 0    mBar    0 Pa - 3999x/14745
        //(27852-1638)/2500=10.4856 per Pascal
        int   reading ;//The reading from pressure sensor[Pa]
        float rawdata; //Reading with offset
        Wire.beginTransmission(120); //Initialize correct I2C device number
        Wire.requestFrom(120, 2);
        if(2 <= Wire.available())   //When the two requested bytes are available proceed to data handling
        {
            reading = Wire.read();  //16 Bit reading, 1 byte per time
            reading = reading << 8;
            reading |= Wire.read();
            rawdata=(reading-1638)/10.4856-1250;
        }
        _p=rawdata; //pa
        _up=10;//pa
        break;
    case 2:
        break; //Sensor two is SPI
    }
}

