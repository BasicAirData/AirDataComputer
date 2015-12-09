/*
  DifferentialPressureSensor.ino - AirDc Library Example File
  Reads a differential pressure sensor and printout air density. Measurements are given with uncertainty.
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include <AirDC.h>
#include <AirSensor.h>

#define DPSENSOR 3 //Selects the differential pressure Hardware sensor see AirSensor.cpp for details on Hookup
#define PSENSOR 1 //Selects the static pressure Hardware sensor see AirSensor.cpp for details on Hookup
#define DEBUG 1 //

#if DPSENSOR==1
#include <Wire.h>
#endif
#if DPSENSOR==2
#include <SPI.h>
#endif
int dpsensor,psensor;
double p; //Static Pressure
AirSensor AirDataSensor(1);
AirDC AirDataComputer(1);

void setup() {
  Serial.begin(9600);
//Impact pressure, qc, differential pressure sensor related setup
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
//Static pressure sensor related setup
#if PSENSOR==1
  psensor = 1;
#endif
}

void loop() {
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;
//Measurements
  AirDataSensor.ReadDifferentialPressure(ptrAirDC, dpsensor);
  AirDataSensor.ReadStaticPressure(ptrAirDC, psensor);
  AirDataSensor.ReadTAT(ptrAirDC, psensor);
  AirDataSensor.ReadRH(ptrAirDC, psensor);
//Computation
  AirDataComputer.RhoAir(1);// Calculates the air density, Algorithm 1
  AirDataComputer.IAS(1);// Calculates the IAS, Algorithm 1
  AirDataComputer.CAS(1);// Calculates the CAS, Algorithm 1
  AirDataComputer.TAS(1);// Calculates the IAS, Algorithm 1
  AirDataComputer.Mach(1);// Calculates the Mach number, Algorithm 1
  AirDataComputer.OAT(1);// Calculates the Outside Air Temperature, Algorithm 1
  AirDataComputer.ISAAltitude(1);// Calculates the ISA altitude from static pressure, Algorithm 1
  delay(1000); //loop delay
//Visualitation  
  AirDataComputer.OutputSerial(1);
  Serial.println(AirDataComputer._StreamOut); // Prints measurements
  Serial.println(dpsensor);  //Prints the Selected sensor
  Serial.println(psensor);  //Prints the Selected sensor
  Serial.println(AirDataComputer._qc); //Differential pressure reading
  Serial.println(AirDataComputer._uqc, 10); //Uncertainty of differential pressure measurement
  Serial.println(AirDataComputer._p); //Static Pressure
  Serial.println(AirDataComputer._up, 10); //Uncertainty of static pressure
  Serial.println(AirDataComputer._IAS);//Sends the indicated Airspeed
  Serial.println(AirDataComputer._uIAS, 10); //Sends the uncertainty of IAS measurement
  Serial.println(AirDataComputer._Rho,4);//Sends the density of Air
  Serial.println(AirDataComputer._uRho, 10); //Sends the uncertainty of the density of air
  Serial.println(AirDataComputer._CAS); //Calibrated AirSpeed
  Serial.println(AirDataComputer._uCAS); //Calibrated AirSpeed uncertainty
  Serial.println(AirDataComputer._TAS); //True Airspeed
  Serial.println(AirDataComputer._uTAS); //True Airspeed uncertainty
  Serial.println(AirDataComputer._M,10); //Mach number
  Serial.println(AirDataComputer._TAT,4); //Total Air Temperature
  Serial.println(AirDataComputer._uTAT); //Total Air Temperature uncertainty
  Serial.println(AirDataComputer._T,4); //Outside Temperature, Static Temperature
  Serial.println(AirDataComputer._uT); //Outside Temperature, Static Temperature uncertainty
  Serial.println(AirDataComputer._RH); //Relative Humidity
  Serial.println(AirDataComputer._uRH); //Relative Humidity Uncertainty
  Serial.println(AirDataComputer._h); //Altitude
  Serial.println(AirDataComputer._uh); //Altitude Uncertainty
}
