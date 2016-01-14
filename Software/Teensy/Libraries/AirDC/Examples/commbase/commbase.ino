/*
  CommBase.ino - Base communication skeleton
  Attention! Work in progress
  Created by J. Larragueta, January 10, 2016.
  Refer to http:\\www.basicairdata.eu
  Code released into the public domain
*/
#include <string>
#include <SD.h>
#include <SPI.h>
#include <Time.h> //For RTC Time 
#include <TimeLib.h>
const int chipSelect = 4;  //SD card reader

void setup() {
  setSyncProvider(getTeensy3Time);
  //Wait for serial is ready
  while (!Serial && (millis() < 4500)) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  //Init SD Card
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  // setTime(16, 55, 53, 10, 1, 2016);
}
byte iB;// incomingByte
char cmd[20];
int queueCommand = 0;
int CmdLenght = 13;
int idxC = 0; //Index to the command string
void loop()
{
  unsigned char bytecount = 0;
  String sout;
  while (Serial.available() && bytecount < 20) {
    iB = Serial.read();
    switch (iB) {
      //Here the command is trapped, if requires immediate dispact it is completed at once.
      case 'C': { ////C Dump the status of ADC configuration
          Serial.println("Dump complete. ADC not online"); //For the moment :-)
          break;
        }
      case 'D': { //Dump the logfile over the serial connections
          File dataFile = SD.open("datalog.csv");

          // if the file is available, write to it:
          if (dataFile) {
            while (dataFile.available()) {
              Serial.write(dataFile.read());
            }
            dataFile.close();
            Serial.println("Dump done!");
          }
          // if the file isn't open, pop up an error:
          else {
            Serial.println("error opening datalog.csv");
          }
          break;
        }
      case 'I': {
          Serial.println("BasicAirData ADC Firmware 0.0.1 Beta Amaranth");
          break;
        }
      case 'R': { //R remove datalog.csv
          // delete the file:
          Serial.println("Removing datalog.csv...");
          SD.remove("datalog.csv");

          if (SD.exists("datalog.csv")) {
            Serial.println("datalog.csv not removed!");
          }
          else {
            Serial.println("Datalog.csv removed or not present!");
          }
          break;
        }

      case 'S': { //See time command
          ClockOut();
          break;
        }
      case 'T': { //Set time command
          unsigned long RxTime = 0L;
          Serial.println("Setting Time");
          RxTime = Serial.parseInt();
          time_t t = RxTime;
          Teensy3Clock.set(t);
          setSyncProvider(getTeensy3Time);
          //Test string T1357041600
          //Sets to 1/1/2013 at twelve o'clock
          ClockOut();
          queueCommand = 1;
          break;
        }


    }
    bytecount++;
  }
  delay(500);
}

void ClockOut() {
  // digital clock display of the time
  //  Serial.print(hour());
  padZero(hour());
  padZero(minute());
  padZero(second());
  padZero(day());
  padZero(month());
  padZero(year());
  Serial.println();
}
void padZero(int digits) {
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
