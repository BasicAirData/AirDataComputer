/*
  DifferentialPressureSensor.ino - AirDc Library Example File
  Reads a differential pressure sensor and printout air density. Measurements are given with uncertainty.
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include <AirDC.h>
#include <AirSensor.h>

#define DPSENSOR 3 //Selects the differential pressure Hardware sensor see AirSensor.cpp for details on Hookup

#if DPSENSOR==1
#include <Wire.h>
#endif
#if DPSENSOR==2
#include <SPI.h>
#endif
int dpsensor;
double p; //Static Pressure
AirSensor AirDataSensor(1);
AirDC AirDataComputer(1);

void setup() {
  Serial.begin(9600);
#if DPSENSOR==1
  dpsensor = 1;
  Wire.begin();
#endif
#if DPSENSOR==2
  dpsensor = 2;
  int cs = 10;
  pinMode (cs, OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
#endif
#if DPSENSOR==3
  dpsensor = 3;
#endif

}

void loop() {
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;
  AirDataSensor.ReadDifferentialPressure(ptrAirDC, dpsensor);
  AirDataComputer.IAS(1);// Calculates the IAS, Algorithm 1
  AirDataComputer.RhoAir(1);// Calculates the air density, Algorithm 1
  delay(1000); //loop delay
  Serial.println(dpsensor);  //Sends the Selected sensor
  Serial.println(AirDataComputer._qc); //Sends the differential pressure reading
  Serial.println(AirDataComputer._uqc, 10); //Sends the uncertainty of differential pressure measurement
  Serial.println(AirDataComputer._IAS);//Sends the indicated Airspeed
  Serial.println(AirDataComputer._uIAS, 10); //Sends the uncertainty of IAS measurement
  Serial.println(AirDataComputer._Rho);//Sends the density of Air
  Serial.println(AirDataComputer._uRho, 10); //Sends the uncertainty of the density of air
}
