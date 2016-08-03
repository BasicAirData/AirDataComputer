/*
  DifferentialPressureSensor.ino - AirDc Library Example File
  Reads a differential pressure sensor and printout air density. Measurements are given with uncertainty.
  Interrupt Based Measurement cycle
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include <AirDC.h>
#include <AirSensor.h>

#define DPSENSOR 3 //Selects the differential pressure Hardware sensor see AirSensor.cpp for details on Hookup

#if DPSENSOR==
#include <Wire.h>
#endif
#if DPSENSOR==2
#include <SPI.h>
#endif
const int ledPin=13;
volatile const int basescaledivider=5;// Divides the main interrupt frequency. Main interrupt is triggere 5 times per second, divided freq is 1 per second
volatile int countwme=0; //Cycle counter
int dpsensor;
double p; //Static Pressure
AirSensor AirDataSensor(1);
AirDC AirDataComputer(1);

void setup() {
  //Common Init
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
//Sensor Specific
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
//Interrupt Specific
 // Time 1 by compare setup
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 12500;            // Desired Frequency 5 Hz   OCR1A = 16000000/256/5     
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}
//Interrupt Handler
ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  countwme ++; //Frequency divider variable
  AirDC *ptrAirDC;
  ptrAirDC = &AirDataComputer;
  noInterrupts(); 
  AirDataSensor.ReadDifferentialPressure(ptrAirDC, dpsensor);
  interrupts(); 
  if (countwme==basescaledivider){ 
  noInterrupts(); 
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
  AirDataComputer.IAS(1);// Calculates the IAS, Algorithm 1
  AirDataComputer.RhoAir(1);// Calculates the air density, Algorithm 1
  countwme=0;
  interrupts(); 
  }

}

void loop() {
  delay(1500); //loop delay
  Serial.println(dpsensor);  //Sends the Selected sensor
  Serial.println(AirDataComputer._qc); //Sends the differential pressure reading
  Serial.println(AirDataComputer._uqc, 10); //Sends the uncertainty of differential pressure measurement
  Serial.println(AirDataComputer._IAS);//Sends the indicated Airspeed
  Serial.println(AirDataComputer._uIAS, 10); //Sends the uncertainty of IAS measurement
  Serial.println(AirDataComputer._Rho);//Sends the density of Air
  Serial.println(AirDataComputer._uRho, 10); //Sends the uncertainty of the density of air
}
