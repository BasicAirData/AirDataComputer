/* Work in progress Asgard ADC Firmware 23/04/2017
   This is not a release. Probably it does not compile. Working on the comm library.
   AsgardADC0.1.ino - Air Data Computer Firmware
   Firmware for Teensy 3.6 MCU.
   Please specify if the BT module is present. If BT is present uncomment the line "#define BT_PRESENT true".
   #Define SDSAVE  regulates the use of the SD card
   #define SENDOUTTOSERIAL regulates the use of the serial report

   Created by JLJ and G.C.
   BasicAirData Team.
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implie  d warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

//#define BT_PRESENT true;
#define SDSAVE 1 //If 1 then the data is saved to Secure Digital Card
#define SENDOUTTOSERIAL 1 //If 1 then the data is sent to serial port /USB

#include <AirDC.h>
#include <CapCom.h>
#if SDSAVE==1
#include <SD.h>
#include <SPI.h>
#endif
#include <SD_t3.h>
#include <i2c_t3.h> //Library for second I2C 
#include <SSC.h>  //Library for SSC series sensors, support two bus I2C
//#define DELIMITER '\n'      // Message delimiter. It must match with Android class one;
#if SDSAVE==1
const int chipSelect = BUILTIN_SDCARD; //HW pin for micro SD adaptor CS
#endif
AirDC AirDataComputer(1);
AirDC *ptrAirDC = &AirDataComputer;
CapCom CC(1);
int InitTime = 0; //To handle first time opened SD card

int TsensorPin = A0;       // select the input pin for the Temperature sensor
double temperature = 0.0;
double dp = 0.0;
double pstatic = 0.0;
SSC diffp(0x28, 0);
//  create an SSC sensor with I2C address 0x28 on I2C bus 1
SSC absp(0x28, 1);
boolean sd_present = false;
//Accessory variables for Air Data calculations
double iTAS, ip1TAS, res, iof;
//Communication related vars
bool endmsg = false;
char input[INPUT_SIZE + 1];
char outputb[OUTPUT_SIZE + 1];
char *ch;

void setup()
{
  InitTime = 1; //First run
  pinMode(TsensorPin, INPUT);                       // and set pins to input.

#ifdef BT_PRESENT
  Serial1.begin(9600);// Begin the serial monitor at 9600 bps over BT module
#endif
#ifndef BT_PRESENT
  Serial.begin(115200);// Begin the serial monitor at 57600 bps over the USB
#endif
  Wire.begin(); // I2C Bus 0
  Wire1.begin(); //I2C Bus 1
  //Setup sensors parameters
  diffp.setMinRaw(1638.3);
  diffp.setMaxRaw(14744.7);
  diffp.setMinPressure(-6984.760);
  diffp.setMaxPressure(6984.760);
  absp.setMinRaw(1638.3);
  absp.setMaxRaw(14744.7);
  absp.setMinPressure(0.0);
  absp.setMaxPressure(160000.0);
  //Init SDCard
#if SDSAVE==1
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
#endif
//Setup the installed HW ADC. 1 present; 0 not installed
AirDataComputer._status[0]=0; //SD Card
AirDataComputer._status[1]=1; //Deltap pressure sensor
AirDataComputer._status[2]=1; //Absolute pressure sensor
AirDataComputer._status[3]=1; //External temperature sensor
AirDataComputer._status[4]=1; //Deltap sensor temperature
AirDataComputer._status[5]=1; //Absolute pressure sensor temperature
AirDataComputer._status[6]=0; //Real time clock temperature temperature
AirDataComputer._status[7]=0; //BT Module present on serial1
}


void loop()
{
  delay(10);
  //#ifdef BT_PRESENT
  comm();
  //#endif
#ifndef BT_PRESENT
  //Serial.println("mark"); //Send out formatted data
  //Serial.println(millis()); //Send out formatted data
  acquisition();
  //Serial.println(millis()); //Send out formatted data
  computation();
  //Serial.println(millis()); //Send out formatted data
  //sendout();
  //Serial.println(millis()); //Send out formatted data
#endif
}
void sendout() {
  int reportno = 50; //Selects the required data
#if SENDOUTTOSERIAL==1
  Serial.println(AirDataComputer.OutputSerial(reportno)); //Send out formatted data
#endif
#if SDSAVE==1
  //Saves to SD Card
  if (InitTime == 1) {
    InitTime = 0;
    //Write Header
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print("Logger File, Basic Air Data Team, JLJ@BasicAirData 2017");
      dataFile.println(String(reportno));
      dataFile.println("$TEX,Rho[kg/m^3],_TAT[K],_TAT[C],_p[Pa],Viscosity[Pas1e-6],_qc[Pa],_T[°K],_IAS[m/s],_TAS[m/s],_c,_m[m MSL],Re,hour,minute,second,month,day,year,millis");
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
      dataFile.println(AirDataComputer.OutputSerial(reportno));
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.csv");
    }
  }
#endif

}
void computation() {
  // put your main code here, to run repeatedly:
  AirDataComputer._p = pstatic;
  AirDataComputer._qc = dp;
  AirDataComputer._RH = 0; //No sensor for RH, we are selecting dry air but the library will handle moist air if required
  AirDataComputer._TAT = temperature + 273.15; //Total Air Temperature
  //Computation
  //Init
  AirDataComputer._T = temperature + 273.15;
  AirDataComputer.RhoAir(1);// Calculates the air density
  AirDataComputer.Viscosity(2);// Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
  AirDataComputer.CalibrationFactor(1); //Calibration factor set to 1
  AirDataComputer.IAS(1); //Calculates IAS method 1
  AirDataComputer._TAS = AirDataComputer._IAS;
  AirDataComputer.Mach(1); //Calculates Mach No
  AirDataComputer.OAT(1); //Outside Air Temperature
  //Wild iteration
  iof = 1;
  while ((res > 0.05) || (iof < 10)) {
    AirDataComputer.RhoAir(1);// Calculates the air density
    AirDataComputer.Viscosity(2);// Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
    AirDataComputer.CalibrationFactor(2); //Update calibration fator vat at TAS
    AirDataComputer.IAS(1); //IAS
    AirDataComputer.CAS(1); //CAS
    AirDataComputer.TAS(1); //True Air Speed
    AirDataComputer.Mach(1); //Update Mach No
    iTAS = AirDataComputer._TAS; //Store TAS value
    AirDataComputer.OAT(1); //Update outside Air Temperature
    AirDataComputer.RhoAir(1);// Calculates the air density
    AirDataComputer.Viscosity(2);// Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
    AirDataComputer.CalibrationFactor(2); //Update calibration fator vat at TAS
    AirDataComputer.TAS(1); //Update TAS
    AirDataComputer.Mach(1); //Update Mach No
    AirDataComputer.OAT(1); //Update outside Air Temperature
    ip1TAS = AirDataComputer._TAS;
    res = abs(ip1TAS - iTAS) / iTAS;
    iof++;
  }
  //Uncorrected ISA Altitude _h
  AirDataComputer.ISAAltitude(1);
  AirDataComputer._d = 8e-3;
  AirDataComputer.Red(1);
}
void comm()
{
noInterrupts();
ch = &input[0]; //Var init
#ifndef BT_PRESENT  //Data is routed to USB serial
  if (Serial.available()) //
  {
    while (Serial.available() && (!endmsg))   // until (end of buffer) or (newline)
    {
      *ch = Serial.read();                    // read char from serial
      if (*ch == DELIMITER)
      {
        endmsg = true;                        // found DELIMITER
        *ch = 0;
      }
      else {
        ++ch;                              // increment index
      }
    }
  }
#endif
#ifdef BT_PRESENT  //Data is routed to Bluetooth module
  if (Serial1.available()) // 
  {
    while (Serial1.available() && (!endmsg))   // until (end of buffer) or (newline)
    {
      *ch = Serial1.read();                    // read char from serial
      if (*ch == DELIMITER)
      {
        endmsg = true;                        // found DELIMITER
        *ch = 0;
      }
      else {
        ++ch;                              // increment index
      }
    }
  }
#endif

  if (!((endmsg) && (ch != &input[0]))) {
    goto fine;
  }
  CC.HandleMessage(ptrAirDC, input, outputb);
  if (endmsg) {
    endmsg = false;
    *ch = 0;
    ch = &input[0];// Return to first index, ready for the new message;
  }
#ifdef BT_PRESENT
  Serial1.println(outputb);
#endif
#ifndef BT_PRESENT
  Serial.println(outputb);
#endif
fine:;
interrupts();
}

void acquisition()
{
  //Outside Temperature Sensor
  temperature = TMP36GT_AI_value_to_Celsius(analogRead(TsensorPin)); // read temperature
  //Differential Pressure sensor
  diffp.update();
  dp = diffp.pressure();
  AirDataComputer._qcRaw =diffp.pressure_Raw();
  AirDataComputer._Tdeltap =diffp.temperature();
  AirDataComputer._TdeltapRaw =diffp.temperature_Raw();
  // delay(10);
  //Absolute Pressure
  absp.update();
  AirDataComputer._pRaw =absp.pressure_Raw();
  AirDataComputer._Tabsp =absp.temperature();
  AirDataComputer._TabspRaw =absp.temperature_Raw();
  pstatic = absp.pressure();
}
double TMP36GT_AI_value_to_Celsius(int AI_value)
{ // Convert Analog-input value to temperature
  float Voltage;
  Voltage = AI_value * (3300.0 / 1024);         // Sensor value in mV:
  return ((Voltage - 750) / 10) + 25;           // Temperature according to datasheet: 750 mV @ 25 °C
  // 10 mV / °C
}

