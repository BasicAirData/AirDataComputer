/*
  Basic.ino - AirDc Library Example File
  Reads a differential pressure sensor and printout basic air data. Measurements are given with uncertainty.
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include <AirDC.h>
#include <SD.h> //For SDLogger
#include <Time.h> //For RTC Time 
const int chipSelect = 4;

AirDC AirDataComputer(1);

void setup() {
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
//BasicTimeValue
  //hour and date setup
  //ora 23:59:55
  //data 31/12/2015
 setTime(23,59,55,31,12,2016);
}

void loop() {
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;
//Measurements

//Computation
  AirDataComputer.RhoAir(1);// Calculates the air density, Algorithm 1
  AirDataComputer.IAS(1);// Calculates the IAS, Algorithm 1
  AirDataComputer.CAS(1);// Calculates the CAS, Algorithm 1
  AirDataComputer.TAS(1);// Calculates the IAS, Algorithm 1
  AirDataComputer.PitotCorrection(2);// Corrected Airspeed, Algorithm 1
  AirDataComputer.Mach(1);// Calculates the Mach number, Algorithm 1
  AirDataComputer.OAT(1);// Calculates the Outside Air Temperature, Algorithm 1
  AirDataComputer.ISAAltitude(2);// Calculates the  ISA Altitude, Mehtod 2
//Auxiliary data
  AirDataComputer.Viscosity(1);// Calculates the dynamic viscosity, Algorithm 1
  AirDataComputer.Red(1);// Calculates the Re number

  delay(2000);  
//Data Output
 /* Serial.println("$TMO,_p,_T,_RH,_qc,AOA,AOS");
  Serial.println(AirDataComputer.OutputSerial(1)); // Measurements
  Serial.println("$TAD,_Rho,_IAS,_CAS,_TAS,_TASPCorrected,_M,_TAT,_h,_mu,_Re");
  Serial.println(AirDataComputer.OutputSerial(2)); // Air Data
  Serial.println("$TMU,_up,_uT,_uRH,_uqc");  
  Serial.println(AirDataComputer.OutputSerial(3)); // Measurements uncertainty
  Serial.println("$TAU,_uRho,_uIAS,_uCAS,_uTAS,_uTAT,_uh");  
  Serial.println(AirDataComputer.OutputSerial(4)); // Measurements uncertainty  
*/
  Serial.println("$TEX,Rho,_T,_p,hour,minute,second,month,day,year,millis");  
  Serial.println(AirDataComputer.OutputSerial(51)); // Output for Temperature Logger Example  

//Saves to SD Card  
/*  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(AirDataComputer.OutputSerial(1));
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
  */
}
