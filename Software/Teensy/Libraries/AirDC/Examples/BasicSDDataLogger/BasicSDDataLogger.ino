/*
  Basic.ino - AirDc Library Example File
  Reads a Temperature Sensor DS18B20, printout to serial and to SDCard.
  Created by J. Larragueta, January 2, 2016.
  Refer to http:\\www.basicairdata.eu
*/
#include <AirDC.h>
#include <SD.h> //For SDLogger
#include <Time.h> //For RTC Time 
#include <AirSensor.h>
#define SDSAVE 0 //If 1 then the data is save to Secure Digital Card
#define TSENSOR 1 //Selects the Temperature sensor DS18B20 see AirSensor.cpp for details

const int chipSelect = 4;
int TATSensor;
#if TSENSOR==1
#include <OneWire.h>
OneWire  ds(16);  // on pin 16 (a 4.7K resistor is necessary between data pin and +5V)
#endif
#if SDSAVE==0
#include <SD.h>
#endif

AirDC AirDataComputer(1);
AirSensor AirDataSensor(1);
void setup() {
#if TSENSOR==1
  TATSensor = 1;
#endif
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
#if SDSAVE==1
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
#endif
  //BasicTimeValue
  //hour and date setup
  //ora 23:59:55
  //data 31/12/2016
  setTime(23, 59, 55, 31, 12, 2016);
}

void loop(void) {
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;
  //Measurements
  AirDataSensor.ReadTAT(ptrAirDC, TATSensor);
  //Computation
  AirDataComputer.RhoAir(1);// Calculates the air density, Algorithm 1
  AirDataComputer.Viscosity(1);// Calculates the dynamic viscosity, Algorithm 1
  delay(2000);
  //Data Output
  Serial.println("$TEX,Rho,_TAT,_uTAT,hour,minute,second,month,day,year,millis");
  Serial.println(AirDataComputer.OutputSerial(51)); // Output for Temperature Logger Example
#if SDSAVE==1
  //Saves to SD Card
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(AirDataComputer.OutputSerial(51));
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.csv");
  }
#endif

}
