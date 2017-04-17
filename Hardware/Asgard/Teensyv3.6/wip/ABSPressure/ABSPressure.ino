/*  Attention please : It's a beta. Probably we will change the sensor function.
  Teensy3.2ADCi2t.ino - Arduino Sketch for testing
 Bluetooth communication with Android devices using BluetoothHelper java class
 with the example android app and with the Air Data Computer Amaranth i2.
 Firmware for Teensy 3.1/3.2
 
 Created by G.Capelli and JLJ. 
 BasicAirData Team. 
 
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

#include <i2c_t3.h> //Library for second I2C
//#include <SSC.h>  //Library for SSC series sensors
#include <SSC1.h>  //Library for sensor on second bus

//  create an SSC sensor with I2C address 0x28. Differential pressure on first I2C BUS
//SSC ssc(0x28, 8);
//  create an SSC sensor with I2C address 0x28 on second I2C bus
SSC1 ssc(0x28, 8);

void setup()
{
  Serial.begin(57600);
 // Wire.begin(); // First I2C Bus
  Wire1.begin(); //Second I2C Bus
}


void loop()
{
  //Is Absolute Pressure sensor present and working?(Second I2C bus)
  //Setup of the sensor parameters
  Serial.println("Absolute pressure measurement");
  ssc.setMinRaw(1638);
//  ssc.setMaxRaw(16383);
  ssc.setMaxRaw(14745);
  ssc.setMinPressure(0.0);
  ssc.setMaxPressure(1.6);
  //  update pressure / temperature
  Serial.print("update()\t");
  Serial.println(ssc.update());
  // print pressure
  Serial.print("pressure()\t");
  Serial.println(ssc.pressure());
  // print temperature
  Serial.print("temperature()\t");
  Serial.println(ssc.temperature());
  delay(500);
}


