/*
  Sketch_01.ino - Arduino Sketch for testing
  Bluetooth communication with Android devices using BluetoothHelper java class
  with the example android app.
  Firmware tested on Teensy 3.1

  Created by G.Capelli (BasicAirData) on 11/06/16.

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

#define INPUT_SIZE 1024
#define DELIMITER '\n'      // Message delimiter. It must match with Android class one;

int counter = 0;
int value = 0;

char input[INPUT_SIZE + 1];
char *ch;
bool endmsg = false;


void setup()
{
  Serial1.begin(9600);                         // Begin the serial monitor at 9600bps
  ch = &input[0];
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
        *ch == 0;
      }
      else ++ch;                              // increment index
    }

    if ((endmsg) && (ch != &input[0]))        // end of (non empty) message !!!
    {
      char *command = strtok(input, ",");

      if (!strcmp(command, "$STR"))           // Received a command, for example "$STR,2" 
      {
        command = strtok (NULL, ",");
        value = atoi(command);                // Read the value after the comma, for example "2". Integer conversion

        // TODO --------------------------------------------------------------------
        // Do stuff with the received data, and prepare the answer string!
        // in this example it simply adds the input number to the incremental counter and returns the result.
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

