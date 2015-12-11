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
  AirDataComputer.PitotCorrection(2);// Corrected Airspeed, Algorithm 1
  AirDataComputer.Mach(1);// Calculates the Mach number, Algorithm 1
  AirDataComputer.OAT(1);// Calculates the Outside Air Temperature, Algorithm 1
  AirDataComputer.ISAAltitude(1);// Calculates the  ISA Altitude
//Auxiliary data
  AirDataComputer.Viscosity(1);// Calculates the dynamic viscosity, Algorithm 1
  AirDataComputer.Red(1);// Calculates the ISA altitude from static pressure, Algorithm 1
  delay(2000);
//Data Output
  Serial.println("$TMO,_p,_T,_RH,_qc,AOA,AOS");
  Serial.println(AirDataComputer.OutputSerial(1)); // Measurements
  Serial.println("$TAD,_Rho,_IAS,_CAS,_TAS,_TASPCorrected,_M,_TAT,_h,_mu,_Re");
  Serial.println(AirDataComputer.OutputSerial(2)); // Air Data
  Serial.println("$TMU,_up,_uT,_uRH,_uqc");  
  Serial.println(AirDataComputer.OutputSerial(3)); // Measurements uncertainty
  Serial.println("$TAU,_uRho,_uIAS,_uCAS,_uTAS,_uTAT,_uh");  
  Serial.println(AirDataComputer.OutputSerial(4)); // Measurements uncertainty  
}
