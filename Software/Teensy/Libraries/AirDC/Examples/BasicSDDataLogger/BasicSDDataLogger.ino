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
#define SDSAVE 1 //If 1 then the data is saved to Secure Digital Card
#define TSENSOR 1 //Selects the Temperature sensor DS18B20 see AirSensor.cpp for details
const int mainperiod=900000; //Main sample period in ms (15 minutes,900000=1000*60*15). Set to your sample period (grater than 1000 ms)

const int chipSelect = 4;
unsigned long int t1; //Store the current run time in ms
int TATSensor;
int InitTime=0; //Flag

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
  InitTime=1;
#if TSENSOR==1
  TATSensor = 1;
#endif
  Serial.begin(9600);
  while (!Serial && (millis()<4500)) {
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
  //Init the Time, here a fixed value you can use the time library to synchronize with different sources
  //hour and date setup
  //Time 23:59:55
  //Date 31/12/2016
  setTime(13, 17, 11, 8, 1, 2016);
}

void loop(void) {
  t1=millis();
#if TSENSOR==1  
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8]={0x28,0x87,0x80,0x50,0x5, 0x0, 0x0, 0x57};
#endif
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;
//Measurements
AirDataComputer._p=101325;
//Temperature sensor DS18B20, suspensive sensor reading
  AirDataSensor.ReadTAT(ptrAirDC, TATSensor);
//Computation
  AirDataComputer.OAT(51); //Save the TAT as OAT
  AirDataComputer.RhoAir(1);// Calculates the air density, Algorithm 1
  AirDataComputer.Viscosity(1);// Calculates the dynamic viscosity, Algorithm 1
while((millis()-t1)<mainperiod) {
//Idle until the next sample time
  }
  //Header Output
  Serial.println("");
  Serial.println("$TEX,Rho[kg/m^3],_TAT[K],_TAT[C],_uTAT[K/C],_p[Pa],Viscosity[Pas1e-6],hour,minute,second,month,day,year,millis");
  Serial.println(AirDataComputer.OutputSerial(51)); // Output for Temperature Logger Example
#if SDSAVE==1
  //Saves to SD Card
  if (InitTime==1){
    InitTime=0;
  //Write Header
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("Logger Example For AirData Library, http://www.basicairdata.eu, JLJ@BasicAirData 2016");
    dataFile.println("$TEX,Rho[kg/m^3],_TAT[K],_TAT[C],_uTAT[K;C],_p[Pa],Viscosity[Pas1e-6],_hour,minute,second,month,day,year,millis");
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.csv");
  }
  }
  else {
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
  }
  #endif

}
