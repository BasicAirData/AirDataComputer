#include <Wire.h>
#include <SSC.h>

//  create an SSC sensor with I2C address 0x78 and power pin 8.
SSC ssc(0x78, 8);

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
 
  //  set min / max reading and pressure, see datasheet for the values for your 
  //  sensor
  ssc.setMinRaw(0);
  ssc.setMaxRaw(16383);
  ssc.setMinPressure(0.0);
  ssc.setMaxPressure(1.6);
 
  //  start the sensor
  Serial.print("start()\t\t");
  Serial.println(ssc.start());
} 

void loop() 
{
  //  update pressure / temperature
  Serial.print("update()\t");
  Serial.println(ssc.update());
  
  // print pressure
  Serial.print("pressure()\t");
  Serial.println(ssc.pressure());
  
  // print temperature
  Serial.print("temperature()\t");
  Serial.println(ssc.temperature());

  while(1) {
    // use Serial as a command stream
    while(Serial.available()) {
      ssc.commandRequest(Serial);
    } 
    delay(100);
  }
}
