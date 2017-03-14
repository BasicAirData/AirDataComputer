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
#include <SPI.h>
#include <SD.h> //For Micro SD support
#include <i2c_t3.h> //Library for second I2C
#include <SSC.h>  //Library for SSC series sensors
//#include <SSC1.h>  //Library for sensor on second bus

#define INPUT_SIZE 1024
#define DELIMITER '\n'      // Message delimiter. It must match with Android class one;

const int chipSelect = 4; //HW pin for micro SD adaptor CS

int counter = 0;
int value = 0;

char input[INPUT_SIZE + 1];
char *ch;
bool endmsg = false;

//  create an SSC sensor with I2C address 0x28. Differential pressure on first I2C BUS
SSC ssc(0x28, 8);
//  create an SSC sensor with I2C address 0x28 on second I2C bus
SSC ssc1(0x28, 8);

void setup()
{
  Serial1.begin(9600);// Begin the serial monitor at 9600bps
  ch = &input[0]; //Var init
  Wire.begin(); // First I2C Bus
  Wire1.begin(); //Second I2C Bus
}


void loop()
{
  delay(10);

  if (Serial1.available()) // If the bluetooth has received any character
  {
    while (Serial1.available() && (!endmsg)) { // until (end of buffer) or (newline)
      *ch = Serial1.read();                    // read char from serial
      if (*ch == DELIMITER) {
        endmsg = true;                        // found DELIMITER
        *ch = 0;
      }
      else ++ch;                              // increment index
    }

    if ((endmsg) && (ch != &input[0]))        // end of (non empty) message !!!
    {
      char *command = strtok(input, ",");

      if (!strcmp(command, "$STR"))           // Received a command, for example "$STR,1" 
      {
        command = strtok (NULL, ",");
        value = atoi(command);                // Read the value after the comma, for example "1". Integer conversion

        // If we receive "$STR,1" the ADC switch to sensor test mode. Starts tests and reports.
        if (value == 1) testme();

        counter = counter + value;
        if (counter > 1000) counter = 0;      // A reset, for the serial gamers :)
        //--------------------------------------------------------------------------

        // Answer with a string
        Serial1.print("$ANS,");                
        Serial1.print(counter);
        Serial1.write(DELIMITER);
      } 
    }
    if (endmsg) {
      endmsg = false;
      *ch = 0;
      ch = &input[0];                         // Return to first index, ready for the new message;
    }
  }
}
void testme()
{
  //Is SDCard Present and working?
  Serial1.print("Checking for SD Card: ");
  if (!SD.begin(chipSelect)) {
    Serial1.println("Card failed, or not present");
    return;
  }
  Serial1.println("card initialized.");
  //Is Differential Pressure sensor present and working? (First I2C bus)
  //Setup of the sensor parameters
  Serial1.println("Differential pressure measurement");
  ssc.setMinRaw(0);
  ssc.setMaxRaw(16383);
  ssc.setMinPressure(0.0);
  ssc.setMaxPressure(1.6);
  //  update pressure / temperature
  Serial1.print("update()\t");
  Serial1.println(ssc.update());
  // print pressure
  Serial1.print("pressure()\t");
  Serial1.println(ssc.pressure());  
  // print temperature
  Serial1.print("temperature()\t");
  Serial1.println(ssc.temperature());
  delay(100);
  
  //Is Absolute Pressure sensor present and working?(Second I2C bus)
  //Setup of the sensor parameters
  Serial1.println("Absolute pressure measurement");
  ssc1.setMinRaw(0);
  ssc1.setMaxRaw(16383);
  ssc1.setMinPressure(0.0);
  ssc1.setMaxPressure(0.0689476);
  //  update pressure / temperature
  Serial1.print("update()\t");
  Serial1.println(ssc1.update());
  // print pressure
  Serial1.print("pressure()\t");
  Serial1.println(ssc1.pressure());
  // print temperature
  Serial1.print("temperature()\t");
  Serial1.println(ssc1.temperature());
  delay(100);
}


