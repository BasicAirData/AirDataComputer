#include <AirDC.h>
#include <stdio.h>

/*
  Minimal -Basic Air Data calculations with AirDC library
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
AirDC BasicAirData(1);

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  BasicAirData.RhoAir(101325,288.15,0,1);
  BasicAirData.IAS(100,1);
  delay(500);
  // put your main code here, to run repeatedly:
Serial.println(BasicAirData._Rho,10);
Serial.println(BasicAirData._uRho,10);
Serial.println(BasicAirData._IAS,10);
Serial.println(BasicAirData._uIAS,10);
}
