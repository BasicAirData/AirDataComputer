
/*  Work in progress 18-04-2017
 * Teensy3.6ADCAsgard.ino - Arduino Sketch for Air Data Computer first bootstrap and test
 * Firmware for Teensy 3.6
 *
 * Created by JLJ and G.C.
 * BasicAirData Team.
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
//#include <SPI.h>

#include <i2c_t3.h> //Library for second I2C 
#include <SSC.h>  //Library for SSC series sensors, support two bus I2C

//  create an SSC sensor with I2C address 0x28 on I2C bus 0
SSC diffp(0x28, 0);
//  create an SSC sensor with I2C address 0x28 on I2C bus 1
SSC absp(0x28, 1);
int TsensorPin = A0;       // select the input pin for the Temperature sensor
double temperature = 0.0;


void setup()
{
  pinMode(TsensorPin, INPUT);                       // and set pins to input.
  Serial1.begin(9600);// Begin the serial monitor at 9600bps
}


void loop()
{
 delay(100);
 testme();
}
void testme()
{
  //  create an SSC sensor with I2C address 0x28. Differential pressure on first I2C BUS

  //Outside Temperature Sensor
  temperature = TMP36GT_AI_value_to_Celsius(analogRead(TsensorPin)); // read temperature
  Serial.print("Temperature = ");
  Serial.print(temperature, 1);             // write temperature to Serial
  Serial.println(" °C");
  //Is Differential Pressure sensor present and working? (First I2C bus)
  //Setup of the sensor parameters
  Serial.println("Differential Pressure measurement");
  diffp.setMinRaw(1638);
  diffp.setMaxRaw(14745);
  diffp.setMinPressure(-0.0689476);
  diffp.setMaxPressure(0.0689476);
  //  update pressure / temperature
  Serial.print("update()\t");
  Serial.println(diffp.update());
  // print pressure
  Serial.print("pressure()\t");
  Serial.println(diffp.pressure());
    // print raw pressure
  Serial.print("pressure_Raw()\t");
  Serial.println(diffp.pressure());
  // print temperature
  Serial.print("temperature()\t");
  Serial.println(diffp.temperature());
  delay(500);

  //Is Absolute Pressure sensor present and working?(Second I2C bus)
  Serial.println("Absolute pressure measurement");
 // absp.setMinRaw(1638);
 // absp.setMaxRaw(14745);
  absp.setMinRaw(0);
  absp.setMaxRaw(16383);
  absp.setMinPressure(0.0);
  absp.setMaxPressure(1.6);
  //  update pressure / temperature
  Serial.print("update()\t");
  Serial.println(absp.update());
  // print pressure
  Serial.print("pressure()\t");
  Serial.println(absp.pressure());
   // print raw pressure
  Serial.print("pressure_Raw()\t");
  Serial.println(absp.pressure());
  // print temperature
  Serial.print("temperature()\t");
  Serial.println(absp.temperature());
  delay(500);
}
double TMP36GT_AI_value_to_Celsius(int AI_value)
{ // Convert Analog-input value to temperature
  float Voltage;
  Voltage = AI_value * (3300.0 / 1024);         // Sensor value in mV:
  return ((Voltage - 750) / 10) + 25;           // Temperature according to datasheet: 750 mV @ 25 °C
  // 10 mV / °C
}


