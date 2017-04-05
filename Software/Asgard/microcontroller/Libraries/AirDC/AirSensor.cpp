/**
 * AirSensor.cpp - Library for sensor interfacing
 * Created by J.L.J., December 3, 2015.\n Refer to http:\\www.basicairdata.eu
*/

#include "AirSensor.h"
#include <math.h>
#include <Wire.h>
#include <OneWire.h>
#include <SPI.h>
#define DEBUG 1
AirSensor::AirSensor(int pid)
{
/** AirDC Default
constructor*/
    _pid = pid;
}
/** Read the current differential pressure sensor value and copy the value to AirDC
* @param  *out, assign an AirDC class for data output
* @param  sensor 1, sensor HLCA12X5
* @param  sensor 2, sensor LDES205U
* @param  sensor 3, sensor MPXV7002
* @return Void
 */
void AirSensor::ReadDifferentialPressure(AirDC *out,int sensor)
{
    switch (sensor)
    {
    case 1 :
    {
        //Sensor one is I2C HLCA12X5
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
        //Update measurement values
        out->_qc=rawdata; //pa
        out->_uqc=10.0;//pa
    }
    break;
    case 2 :
    {
        /*
        Differential Pressure sensor LDES205U by FirstSensor/SensorThecnics
        This sensor uses SPI

        Circuit:
        LDES250U   <-->  ArduinoUno
        MOSI: pin 7      MOSI: pin 11                    GREEN
        MISO: pin 8      MISO: pin 12   //MSB first      RED
        SCLK: pin 6      SCK : pin 13                    YELLOW
        /CS:  pin 9      SS  : pin 10   //Asserted low   BROWN
        Power connections to LDES250U

        pin 2 +5V   RED
        pin 3 GND  BLACK

        DATASHEET
        http://www.first-sensor.com/cms/upload/datasheets/DS_Standard-LDE_E_11815.pdf
        */
        float sensorgain=0.00833333; // Pa/Count
        const byte POLL=0x2D;
        const byte REGISTER=0x14;
        const byte GETMEASURE=0x98;
        int value,result;
        int cs = 10;
        float rawpressure;
//Get the reading
        digitalWrite(cs, LOW);
        SPI.transfer(POLL);
        digitalWrite(cs, HIGH);
        digitalWrite(cs, LOW);
        SPI.transfer(REGISTER);
        digitalWrite(cs, HIGH);
        digitalWrite(cs, LOW);
        SPI.transfer(GETMEASURE);
        digitalWrite(cs, HIGH);
        digitalWrite(cs, LOW);
        byte hb = SPI.transfer(0x00);
        byte lb = SPI.transfer(0x00);
        digitalWrite(cs, HIGH);
        result=word(hb,lb);
        rawpressure=result*sensorgain;
        out->_qc=rawpressure; //pa
        out->_uqc=5.0;//pa
    }
    break; //Sensor two is SPI
    case 3 :
    {
        //MPXV7002 Sensor
        //Wire the Arduino Uno and MPXV7002 (That sensor is mounted on HobbyKing Pitot Board) in this way
        //MPXV7002    <->   Arduino Uno
        //GND                     GND
        //VDD                      5V
        //SIGNAL                   A0
        float offsetv=0;
        int analogPin=0; //Wired on input A0
        int raw;
        float Vread,Pread;
        raw = analogRead(analogPin);
        Vread=5.0/1023.0*(raw);
        Pread=(Vread-2.5-offsetv)*1000;
        #if (DEBUG==1)
        Pread=469.9651;
        #endif
        out->_qc=Pread; //pa
        out->_uqc=50.0;//pa
    }
    break;
    }
}
/** Read the current static pressure value and copy the value to AirDC
* implementation is not completed
* @param  *out, assign an AirDC class for data output
* @param  sensor 99, debug mode _p=90000 Pa and _up=100 Pa
* @return Void
 */
void AirSensor::ReadStaticPressure(AirDC *out,int sensor)
{
    switch (sensor)
    {
    case 99 :
    {
        out->_p=90000;
        out->_up=100;
        break;
    }
    }
}
/** Read the current total air temperature and copy the value to AirDC
* @param  *out, assign an AirDC class for data output
* @param  sensor 1, sensor DS18X20
* @return Void
 */
void AirSensor::ReadTAT(AirDC *out,int sensor)
{
    switch (sensor)
    {
    case 1 :
    {
//Total Air Temperature Sensor
//http://www.basicairdata.blogspot.it/2013/05/resistance-temperature-detectors-for.html
//DS18x20
//Based on http://playground.arduino.cc/Learning/OneWire
//Wiring
//Data to Arduino pin 16
//Vcc to Arduino +5
//Gnd to Arduino Gnd
//on pin 16 (a 4.7K resistor is necessary between data pin and +5V)
//Fixed Rom Address
//  byte addr[8]={0x28,0x87,0x80,0x50,0x5, 0x0, 0x0, 0x57};

        OneWire  ds(16);  // on pin 16 (a 4.7K resistor is necessary between data pin and +5V)
        int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
        double TheMeasure;
        byte i;
        byte present = 0;
        byte data[12];
        byte addr[8]= {0x28,0x87,0x80,0x50,0x5, 0x0, 0x0, 0x57};

        ds.reset();
        ds.select(addr);
        ds.write(0x44,1);         // start conversion, with parasite power on at the end

        delay(1000);     // maybe 750ms is enough, maybe not
        // we might do a ds.depower() here, but the reset will take care of it.

        present = ds.reset();
        ds.select(addr);
        ds.write(0xBE);         // Read Scratchpad
        for ( i = 0; i < 9; i++)             // we need 9 bytes
        {
            data[i] = ds.read();
        }
        LowByte = data[0];
        HighByte = data[1];
        TReading = (HighByte << 8) + LowByte;
        SignBit = TReading & 0x8000;  // test most sig bit
        if (SignBit) // negative
        {
            TReading = (TReading ^ 0xffff) + 1; // 2's comp
        }
        Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

        Whole = Tc_100 / 100;  // separate off the whole and fractional portions
        Fract = Tc_100 % 100;
        TheMeasure=double(Fract)/100+Whole;
        if (SignBit) // If its negative
        {
            TheMeasure=-1*TheMeasure;
        }
        TheMeasure=TheMeasure+273.15;
	out->_TAT=TheMeasure;
        out->_uTAT=0.5;
        break;
    }
    }
}
/** Acquire relative humidity and copy the value to AirDC
* @param  *out, assign an AirDC class for data output
* @param  sensor 99, debug mode RH=0.5 _uRH=0.05
* @return Void
 */
void AirSensor::ReadRH(AirDC *out,int sensor){
    switch (sensor)
    {
    case 99 :
    {
        out->_RH=0.5;
        out->_uRH=0.05;
        break;
    }
    }

}
