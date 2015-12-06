#include <AirDC.h>
#include <AirSensor.h>

#define PSENSOR 1
#if PSENSOR==1
#include <Wire.h>
#endif

int psensor;
double p; //Static Pressure
AirSensor AirDataSensor(1);
AirDC AirDataComputer(1);

void setup() {
Serial.begin(9600);
#if PSENSOR==1
  psensor = 1;
  Wire.begin();
#endif
#if PSENSOR==2
  psensor = 2;
#endif

}

void loop() {
  
  AirDataSensor.Pressure(psensor);
  AirDataComputer._p=AirDataSensor._p;
  delay(1000);
  Serial.println(psensor);  //Selected sensor
  Serial.println(AirDataComputer._p); //Pressure reading
}
