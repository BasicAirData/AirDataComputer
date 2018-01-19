/*JLJ Adding Msg 19 e 20
   Linear calibration only
 * */
/* Work in progress Asgard ADC Firmware Relase 0.4.0 16/01/2018
   This is a preliminary release, work in progress. Misbehaviour is plausible.
   AsgardADC.ino - Air Data Computer Firmware
   Conform to ADC Common Mesage Set 0.4
   Firmware for Teensy 3.6 MCU
   Created by JLJ and GC, BasicAirData Team www.basicairdata.eu

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

#include <TimeLib.h>
#include <Metro.h>
#include <AirDC.h>
#include <SD.h>
#include <SPI.h>
#include <SD_t3.h>
#include <i2c_t3.h>                       // Library for second I2C 
#include <SSC.h>                          // Library for SSC series sensors, support two bus I2C

#define ADC_NAME "ASGARD"                 // The name of the ADC device
#define PROTOCOL_VERSION "0.4"            // The version of the communication protocol

#define BUFFERLENGTH 512                  // The length of string buffers
#define DELIMITER '\n'
#define SEPARATOR ","
#define LEAVE_AS_IS "="
#define DEFAULT_LOGFILE "DATALOG.CSV"     // The default log file name
#define DEFAULT_CONFIGFILE "CONFIG.CFG"   // The configuration file name

const int ledPin      = 13;               // The led is ON when the application is logging on SD
const int TsensorPin  = A0;               // The input pin for the analog Temperature sensor
const int chipSelect  = BUILTIN_SDCARD;   // HW pin for micro SD adaptor CS

File dataFile;                            // The file on SD
File configFile;                          // The setup configuration file on SD
bool isSDCardPresent;                     // The presence of a (formatted) card into the slot of SD
bool loadConfiguration;                   // The flag is true if the config file is present into SD (and must be loaded on start)
Sd2Card card;                             // The SD Card
SdVolume volume;                          // The volume (partition) on SD
double iTAS, ip1TAS, res, iof;            // Accessory variables for Air Data calculations

AirDC AirDataComputer(1);     // The Air Data Computer

SSC diffp(0x28, 0);           // Create an SSC sensor with I2C address 0x28 on I2C bus 0
SSC absp(0x28, 1);            // Create an SSC sensor with I2C address 0x28 on I2C bus 1

String message_BT;            // string that stores the incoming BT message
String message_COM;           // string that stores the incoming COM message
String message_CFG;           // string that stores the configuration messages

float acquisition_freq     =   1;         // The minimum frequency of acquisition = 1 Hz (1 s)
float sendtoserial_freq    =   0;         // The frequency of sendtoserial        = NOT ACTIVE
float sendtobluetooth_freq =   0;         // The frequency of sendtobluetooth     = NOT ACTIVE
float sendtosd_freq        =   0;         // The frequency of sendtosd,           = NOT ACTIVE

long acquisition_interval  = (long)(1.0f / acquisition_freq * 1000.0f);
long sendtoserial_interval;
long sendtobluetooth_interval;
long sendtosd_interval;

int delaymicroseconds_interval = 5000;    // The delay at the end of main loop. It may vary in the range 100-5000 micros.

Metro acquisitionTimer     = Metro(acquisition_interval);             // The timer for acquisition
Metro sendtoserialTimer    = Metro(sendtoserial_interval);            // The timer for sendtoserialTimer
Metro sendtobluetoothTimer = Metro(sendtobluetooth_interval);         // The timer for sendtobluetoothTimer
Metro sendtosdTimer        = Metro(sendtosd_interval); //NOT ACTIVE   // The timer for sendtosdTimer

bool sendtoserial_needs_acquisition    = true;    // The check that the aquisition has been updated since the last $DTA
bool sendtobluetooth_needs_acquisition = true;    // The check that the aquisition has been updated since the last $DTA
bool sendtosd_needs_acquisition        = true;    // The check that the aquisition has been updated since the last $DTA

char Data[2][BUFFERLENGTH];  // The strings containing the data message
char *p_Data;                // The pointer to the current valid data string





void setup() {
  // Deafult configuration for ADC Hardware. 1 present; 0 not installed
  AirDataComputer._status[AIRDC_STATUS_SD]        = '0'; // SD Card
  AirDataComputer._status[AIRDC_STATUS_DELTAP]    = '1'; // Deltap pressure sensor
  AirDataComputer._status[AIRDC_STATUS_P]         = '1'; // Absolute pressure sensor
  AirDataComputer._status[AIRDC_STATUS_TAT]       = '1'; // External temperature sensor
  AirDataComputer._status[AIRDC_STATUS_TDELTAP]   = '1'; // Deltap sensor temperature
  AirDataComputer._status[AIRDC_STATUS_TABSP]     = '1'; // Absolute pressure sensor temperature
  AirDataComputer._status[AIRDC_STATUS_RTCBATT]   = '0'; // Real time clock temperature temperature
  AirDataComputer._status[AIRDC_STATUS_ERRWARN]   = '0'; // Error/Warning
  AirDataComputer._status[AIRDC_STATUS_BLUETOOTH] = '0'; // BT Module present on serial1

  strcpy(AirDataComputer._logfile, DEFAULT_LOGFILE);

  pinMode(TsensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin (57600);                             // Set up the Serial Connection using high speed, for precise log timings
  //Serial.begin (9600);                            // Set up the Serial Connection
  Serial1.begin (9600);                             // Set up the BT Connection
  delay(500);
  if (Serial1) AirDataComputer._status[AIRDC_STATUS_BLUETOOTH] = '1';    // BT Module present on serial1

  SDCardCheck();

  Wire.begin();                     // I2C Bus 0
  Wire1.begin();                    // I2C Bus 1

  // Setup sensors parameters
  diffp.setMinRaw (1638.3);
  diffp.setMaxRaw (14744.7);
  diffp.setMinPressure (-6984.760);
  diffp.setMaxPressure (6984.760);
  absp.setMinRaw (1638.3);
  absp.setMaxRaw (14744.7);
  absp.setMinPressure (0.0);
  absp.setMaxPressure (160000.0);

  strcpy(Data[0], "\0");     // Initialize the Data
  strcpy(Data[1], "\0");     // Initialize the Data
  p_Data = &Data[1][0];      // The current valid status index is 1

  if (isSDCardPresent) {     // Searches the configuration file
    configFile = SD.open(DEFAULT_CONFIGFILE);
    if (configFile) loadConfiguration = true;
  }
}



void getNextFilename(char* namefile)
{
  if (strlen(namefile) < 13) {
    char namefile1[13], newname[13], number[13], nformat[5], *before, *after;
    int numbersize = 13 - strlen(namefile);
    numbersize = min (numbersize, 4);
    sprintf(nformat, "%%0%id", numbersize);
    strcpy(namefile1, namefile);
    before = strtok (namefile1, "?");
    after = strtok (NULL, "?");
    bool fileexists = true;
    for (int i = 0; i < (pow(10, numbersize)) && fileexists; i++) {
      sprintf(number, nformat, i);
      strcpy(newname, before);
      strcat(newname, number);
      strcat(newname, after);
      if (!SD.exists(newname)) {
        strcpy(namefile, newname);
        fileexists = false;
      }
    }
  }
}


void dateTime(uint16_t* date, uint16_t* time)                 // Callback for file timestamps
{
  *date = FAT_DATE(year(), month(), day());
  *time = FAT_TIME(hour(), minute(), second());
}



void SDCardCheck()
{
  if (!isSDCardPresent) {
    isSDCardPresent = card.init(SPI_FULL_SPEED, chipSelect);
    if (isSDCardPresent) SD.begin(chipSelect);
  }
  isSDCardPresent = volume.init(card);
  AirDataComputer._status[AIRDC_STATUS_SD] = (isSDCardPresent ? '1' : '0');
}



void acquisition()
{
  // 1) Sensors reading

  // Outside Temperature Sensor
  if (AirDataComputer._status[AIRDC_STATUS_TAT] == '1') {   // External Temperature sensor present
    AirDataComputer._TRaw = analogRead(TsensorPin);
    AirDataComputer._TAT = ((((AirDataComputer._TRaw * (3300.0 / 1024)) - 750) / 10) + 25) + 273.15; // Total Air Temperature
  }

  // Differential Pressure sensor
  if (AirDataComputer._status[AIRDC_STATUS_DELTAP] == '1') {     // Differential pressure sensor present
    diffp.update();
    AirDataComputer._qc = diffp.pressure();
    AirDataComputer._qcRaw = diffp.pressure_Raw();
    if (AirDataComputer._status[AIRDC_STATUS_TDELTAP] == '1') {   // Temperature differential pressure sensor present
      AirDataComputer._Tdeltap = diffp.temperature();
      AirDataComputer._TdeltapRaw = diffp.temperature_Raw();
    }
  }

  // Absolute Pressure sensor
  if (AirDataComputer._status[AIRDC_STATUS_P] == '1') {           // Absolute pressure sensor present
    absp.update();
    AirDataComputer._pRaw = absp.pressure_Raw();
    AirDataComputer._p = absp.pressure();
    if (AirDataComputer._status[AIRDC_STATUS_TABSP] == '1') {     // Temperature absolute pressure sensor present
      AirDataComputer._Tabsp = absp.temperature();
      AirDataComputer._TabspRaw = absp.temperature_Raw();
    }
  }

  // No sensor for RH, we are selecting dry air but the library will handle moist air if required
  AirDataComputer._RH = 0;

  // For low speeds this is a good approximation
  AirDataComputer._T = AirDataComputer._TAT;


  // 2) Calculations

  // Init
  AirDataComputer.RhoAir(1);                    // Calculates the air density
  AirDataComputer.Viscosity(1);                 // Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
  AirDataComputer.CalibrationFactor(1);         // Calibration factor set to 1
  AirDataComputer.IAS(1);                       // Calculates IAS method 1
  AirDataComputer._TAS = AirDataComputer._IAS;
  AirDataComputer.Mach(1);                      // Calculates Mach No
  AirDataComputer.OAT(1);                       // Outside Air Temperature
  // Wild iteration
  iof = 1;
  while ((res > 0.05) || (iof < 10)) {
    AirDataComputer.RhoAir(1);                  // Calculates the air density
    AirDataComputer.Viscosity(1);               // Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
    AirDataComputer.CalibrationFactor(2);       // Update calibration fator vat at TAS
    AirDataComputer.IAS(1);                     // IAS
    AirDataComputer.CAS(1);                     // CAS
    AirDataComputer.TAS(1);                     // True Air Speed
    AirDataComputer.Mach(1);                    // Update Mach No
    iTAS = AirDataComputer._TAS;                // Store TAS value
    AirDataComputer.OAT(1);                     // Update outside Air Temperature
    AirDataComputer.RhoAir(1);                  // Calculates the air density
    AirDataComputer.Viscosity(1);               // Calculates the dynamic viscosity, Algorithm 2 (UOM Pas1e-6)
    AirDataComputer.CalibrationFactor(2);       // Update calibration fator vat at TAS
    AirDataComputer.TAS(1);                     // Update TAS
    AirDataComputer.Mach(1);                    // Update Mach No
    AirDataComputer.OAT(1);                     // Update outside Air Temperature
    ip1TAS = AirDataComputer._TAS;
    res = abs(ip1TAS - iTAS) / iTAS;
    iof++;
  }
  // Uncorrected ISA Altitude _h
  AirDataComputer.ISAAltitude(1);
  AirDataComputer._d = 8e-3;
  AirDataComputer.Red(1);


  // 3) $DTA message update

  char *p_newData;                                                  // The pointer to unused index;
  p_newData = (p_Data == &Data[0][0] ? &Data[1][0] : &Data[0][0]);  // The new string is composed into the unused Data index
  strcpy (p_newData, "$DTA");

  char workbuff[BUFFERLENGTH];                                      // General purpose buffer, used for itoa conversion
  AirDataComputer.PrepareData();

  for (int i = 0; i < AIRDC_DATA_VECTOR_SIZE; i++) {                // Prepare to send #10 - DTA - DATA_ASSERT message
    strcat (p_newData, SEPARATOR); // Add the separator
    if (AirDataComputer._datasel[i] == '1') {
      switch (i) {
        case AIRDC_DATA_TIME:
        case AIRDC_DATA_QCRAW:
        case AIRDC_DATA_PRAW:
        case AIRDC_DATA_TRAW:
        case AIRDC_DATA_TDELTAPRAW:
        case AIRDC_DATA_TABSPRAW:
        case AIRDC_DATA_MILLIS:
          sprintf(workbuff, "%.0f", (AirDataComputer._dataout[i])); break;

        case AIRDC_DATA_P:
        case AIRDC_DATA_TAT:
        case AIRDC_DATA_TDELTAP:
        case AIRDC_DATA_TABSP:
        case AIRDC_DATA_UIAS:
        case AIRDC_DATA_UTAS:
        case AIRDC_DATA_UH:
        case AIRDC_DATA_UT:
        case AIRDC_DATA_T:
        case AIRDC_DATA_RE:
          sprintf(workbuff, "%.1f", (AirDataComputer._dataout[i])); break;

        case AIRDC_DATA_IAS:
        case AIRDC_DATA_TAS:
        case AIRDC_DATA_H:
        case AIRDC_DATA_QC:
          sprintf(workbuff, "%.2f", (AirDataComputer._dataout[i])); break;

        case AIRDC_DATA_C:
          sprintf(workbuff, "%.4f", (AirDataComputer._dataout[i])); break;

        case AIRDC_DATA_RHO:
          sprintf(workbuff, "%.6f", (AirDataComputer._dataout[i])); break;

        case AIRDC_DATA_MU:
          sprintf(workbuff, "%.8f", (AirDataComputer._dataout[i])); break;

        default:
          sprintf(workbuff, "%f", (AirDataComputer._dataout[i])); break;   // UNUSED DEFAULT FORMAT
      }
      strcat (p_newData, workbuff);
    } else {
    }
  }
  strcat (p_newData, "\0");

  p_Data = (p_Data == &Data[0][0] ? &Data[1][0] : &Data[0][0]);   // Switch the pointer to the current valid string to the new one

  sendtoserial_needs_acquisition      = false;    // The aquisition has been updated since the last $DTA
  sendtobluetooth_needs_acquisition   = false;    // The aquisition has been updated since the last $DTA
  sendtosd_needs_acquisition          = false;    // The aquisition has been updated since the last $DTA
}



void sendtoserial(void)
{
  char printdata[BUFFERLENGTH];
  strcpy (printdata, p_Data);
  sendtoserial_needs_acquisition = true;
  Serial.println(printdata);
}



void sendtobluetooth(void)
{
  char printdata[BUFFERLENGTH];
  strcpy (printdata, p_Data);
  sendtobluetooth_needs_acquisition = true;
  Serial1.println(printdata);
}



void sendtosd(void)
{
  char printdata[BUFFERLENGTH];
  strcpy (printdata, p_Data);
  sendtosd_needs_acquisition = true;
  if (dataFile) dataFile.println(printdata);
}



void loop() {
  bool endmsg_BT                = false;  // it becomes true when a message is received from BLUETOOTH
  bool endmsg_COM               = false;  // it becomes true when a message is received from SERIAL
  bool endmsg_CFG               = false;  // it becomes true when a message is received from CONFIGURATION FILE
  char workbuff[BUFFERLENGTH]   = "";     // General purpose buffer, used for itoa conversion
  char Message[BUFFERLENGTH]    = "";     // it contains the message to be processed
  char Answer[BUFFERLENGTH]     = "";     // it contains the answer to the message


  // 0) Check Metro Timers:

  if ((acquisition_freq > 0) && (acquisitionTimer.check() == 1)) acquisition();

  if ((sendtoserial_freq > 0) && (sendtoserialTimer.check() == 1)) {
    if (sendtoserial_needs_acquisition) {
      acquisition();
      acquisitionTimer.reset();
    }
    sendtoserial();
  }

  if ((sendtobluetooth_freq > 0) && (sendtobluetoothTimer.check() == 1)) {
    if (sendtobluetooth_needs_acquisition) {
      acquisition();
      acquisitionTimer.reset();
    }
    sendtobluetooth();
  }

  if ((sendtosd_freq > 0) && (sendtosdTimer.check() == 1)) {
    if (sendtosd_needs_acquisition) {
      acquisition();
      acquisitionTimer.reset();
    }
    sendtosd();
  }


  // 1) Read input from Configuration file, Serial, and Bluetooth:

  if (loadConfiguration) {
    while (configFile.available()) {
      char ch = configFile.read();
      if (ch == DELIMITER) {
        endmsg_CFG = true;
        message_CFG.toCharArray (Message, BUFFERLENGTH - 1);
        message_CFG = "";
        goto endread;
      } else if (ch != '\r') message_CFG += char(ch);   // Store string from CONFIG file
    }
    loadConfiguration = false;
    configFile.close();
    endmsg_CFG = true;
    message_CFG.toCharArray (Message, BUFFERLENGTH - 1);
    message_CFG = "";
    goto endread;
  }

  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == DELIMITER) {
      endmsg_COM = true;
      message_COM.toCharArray (Message, BUFFERLENGTH - 1);
      message_COM = "";
      goto endread;
    } else if (ch != '\r') message_COM += char(ch);     // Store string from serial command
  }

  while (Serial1.available()) {
    char ch = Serial1.read();
    if (ch == DELIMITER) {
      endmsg_BT = true;
      message_BT.toCharArray (Message, BUFFERLENGTH - 1);
      message_BT = "";
      goto endread;
    } else if (ch != '\r') message_BT += char(ch);      // Store string from bluetooth command
  }

endread:


  // 2) Process messages

  if (endmsg_COM || endmsg_BT || endmsg_CFG) {
    char *command = strtok(Message, SEPARATOR);

    // --------------------------------------------------
    // #0 – HBQ – HEARTBEAT_REQ                          --> Reply HBA - HEARTBEAT_ASSERT
    // --------------------------------------------------
    // $HBQ,Devicename,ProtocolVersion
    // $HBQ,REMOTEPROGRAM,0.3

    if (!strcmp(command, "$HBQ")) {
      strcpy (Answer, "$HBA,");
      strcat (Answer, ADC_NAME);
      strcat (Answer, SEPARATOR);
      strcat (Answer, PROTOCOL_VERSION);
      goto endeval;
    }

    // --------------------------------------------------
    // #1 – HBA – HEARTBEAT_ASSERT
    // --------------------------------------------------
    // $HBA,Devicename,ProtocolVersion
    // $HBA,DEVICE,0.3

    if (!strcmp(command, "$HBA")) {
      goto endeval;
    }

    // --------------------------------------------------
    // #2 – TMS – TIME_SET                               --> Reply TMA - TIME_ASSERT
    // --------------------------------------------------
    // $TMS,SecondsFrom1_1_1970
    // $TMS,1495396317          Set time to 21 May 2017, 21.52...

    if (!strcmp(command, "$TMS")) {
      command = strtok (NULL, SEPARATOR);
      if (command != NULL) {
        long newTime = atol(command);
        if (newTime > 0) setTime(newTime);
      }
      strcpy (Answer, "$TMA,");
      long int currenttime = static_cast<long int> (now());
      char s[20];
      ltoa(currenttime, s, 10);
      strcat (Answer, s);
      goto endeval;
    }

    // --------------------------------------------------
    // #3 – TMQ – TIME_REQ                               --> Reply TMA - TIME_ASSERT
    // --------------------------------------------------
    // $TMQ

    if (!strcmp(command, "$TMQ")) {
      strcpy (Answer, "$TMA,");
      long int currenttime = static_cast<long int> (now());
      char s[20];
      ltoa(currenttime, s, 10);
      strcat (Answer, s);
      goto endeval;
    }

    // --------------------------------------------------
    // #4 – TMA – TIME_ASSERT
    // --------------------------------------------------
    // $TMA,SecondsFrom1_1_1970

    if (!strcmp(command, "$TMA")) {
      goto endeval;
    }

    // --------------------------------------------------
    // #5 – STS – STATUS_SET                             --> Reply STA - STATUS_ASSERT
    // --------------------------------------------------
    // $STS,0,1,1,1,1,1,0,0,0
    // $STS,=,=,=,0,=,=,=,=,=     Deactivate the external temperature sensor. Don't touch other fields

    if (!strcmp(command, "$STS")) {
      int i;
      i = 0;
      for (i = 0; i < AIRDC_STATUS_VECTOR_SIZE; i++) {
        // Receive the status vector
        command = strtok (NULL, SEPARATOR);
        if (strlen(command) < 1) goto endeval;
        if (!strcmp(command, "0")) {
          AirDataComputer._status[i] = '0';
          workbuff[2 * i] = '0';
        }
        else if (!strcmp(command, "1")) {
          AirDataComputer._status[i] = '1';
          workbuff[2 * i] = '1';
        } else {
          workbuff[2 * i] = AirDataComputer._status[i];
        }
        workbuff[2 * i + 1] = ',';
      }
      AirDataComputer._status[AIRDC_STATUS_VECTOR_SIZE + 1] = '\0';
      workbuff[2 * AIRDC_STATUS_VECTOR_SIZE - 1] = '\0';

      strcpy (Answer, "$STA,");
      strcat (Answer, workbuff);           // TODO = Send the real status buffer, and NOT the received workbuff
      goto endeval;
    }

    // --------------------------------------------------
    // #6 – STQ – STATUS_REQ                             --> Reply STA - STATUS_ASSERT
    // --------------------------------------------------
    // $STQ

    if (!strcmp(command, "$STQ")) {
      int i;
      i = 0;
      for (i = 0; i < AIRDC_STATUS_VECTOR_SIZE; i++) {
        // Returns the status vector
        workbuff[2 * i] = AirDataComputer._status[i];
        workbuff[2 * i + 1] = ',';
      }
      workbuff[2 * i - 1] = '\0';

      strcpy (Answer, "$STA,");
      strcat (Answer, workbuff);
      goto endeval;
    }

    // --------------------------------------------------
    // #7 – STA – STATUS_ASSERT
    // --------------------------------------------------

    if (!strcmp(command, "$STA")) {
      goto endeval;
    }

    // --------------------------------------------------
    // #8 – DTS – DATA_SET
    // --------------------------------------------------
    // $DTS,=,=,=,=,=,=,=,=,300   Sets the external temperature to 300 [K] (if the sensor is not present)

    if (!strcmp(command, "$DTS")) {
      int i;
      i = 0;
      for (i = 0; i < AIRDC_DATA_VECTOR_SIZE; i++) { // Check the fields
        command = strtok (NULL, SEPARATOR);
        if (strlen(command) < 1) goto endeval;
        if (strcmp(command, LEAVE_AS_IS)) {
          switch (i) {
            case AIRDC_DATA_TAT:                                            // External Temperature
              if (AirDataComputer._status[AIRDC_STATUS_TAT] == '0') {
                AirDataComputer._TAT = atol(command);
                AirDataComputer._TRaw = 0l;
              }
              break;
            case AIRDC_DATA_QC:                                             // Differential pressure
              if (AirDataComputer._status[AIRDC_STATUS_DELTAP] == '0') {
                AirDataComputer._qc = atol(command);
                AirDataComputer._qcRaw = 0l;
              }
              break;
            case AIRDC_DATA_TDELTAP:                                        // Temperature differential pressure sensor
              if (AirDataComputer._status[AIRDC_STATUS_TDELTAP] == '0') {
                AirDataComputer._Tdeltap = atol(command);
                AirDataComputer._TdeltapRaw = 0l;
              }
              break;
            case AIRDC_DATA_P:                                              // Absolute pressure
              if (AirDataComputer._status[AIRDC_STATUS_P] == '0') {
                AirDataComputer._p = atol(command);
                AirDataComputer._pRaw = 0l;
              }
              break;
            case AIRDC_DATA_TABSP:                                          // Temperature absolute pressure sensor
              if (AirDataComputer._status[AIRDC_STATUS_TABSP] == '0') {
                AirDataComputer._Tabsp = atol(command);
                AirDataComputer._TabspRaw = 0l;
              }
              break;
            default:  // IT DOES NOTHING
              break;
          }
        }
      }
      goto endeval;
    }

    // --------------------------------------------------
    // #9 – DTQ – DATA_REQ
    // --------------------------------------------------
    // Request all the data fields of #10
    // $DTQ,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    // Request all the data fields except Deltap and Ext Temperature
    // $DTQ,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    // Request time, deltap, p, ext temperature
    // $DTQ,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    // Toggle ON the ext temperature field
    // $DTQ,=,=,=,=,=,=,=,=,1
    // Request a $DTA string
    // $DTQ

    if (!strcmp(command, "$DTQ")) {
      if (!strcmp(command, "$DTQ,")) {
        strcpy (Answer, p_Data);      // Simply return the data
        goto endeval;
      }
      strcpy (Answer, "");
      int i;
      i = 0;
      for (i = 0; i < AIRDC_DATA_VECTOR_SIZE; i++) {
        command = strtok (NULL, SEPARATOR);
        if (strlen(command) < 1) goto endeval;
        if (strcmp(command, LEAVE_AS_IS)) AirDataComputer._datasel[i] = command[0];
      }

      // Save to the SD the configuration data
      //SDCardCheck();
      //if (isSDCardPresent) {
      //  SD.remove(DEFAULT_CONFIGFILE);
      //  File dataFile = SD.open(DEFAULT_CONFIGFILE, FILE_WRITE);
      //  if (dataFile) { // if the file is available, write to it:
      //    AirDataComputer._status[AIRDC_STATUS_SD] = '1';  // SD Card present
      //    dataFile.print("$DTQ,");
      //    dataFile.println(workbuff);
      //    dataFile.close();
      //  } else {        // if the file isn't open, pop up an error:
      //    AirDataComputer._status[AIRDC_STATUS_SD] = '0';  // SD Card not present
      //    goto endeval;
      //  }
      //}
      // Modified #10 reply message. No data will be transmitted. It is a plain acknowledge.
      //strcpy (outstr,"$DTA,");
      //strcat (Answer, workbuff);
      goto endeval;
    }

    // --------------------------------------------------
    // #10 – DTA – DATA_ASSERT
    // --------------------------------------------------

    if (!strcmp(command, "$DTA")) {
      goto endeval;
    }

    // --------------------------------------------------
    // #11 - LCS - LOG_CURRENTFILE_SET                   --> Reply LCA - LOG_CURRENTFILE_ASSERT
    // --------------------------------------------------
    // Set the current log file to the LOG.CSV
    // $LCS,LOG1.CSV

    if (!strcmp(command, "$LCS"))
    {
      command = strtok (NULL, SEPARATOR);
      if (strlen(command) < 1) {
        goto endeval;
      }
      SDCardCheck();
      if (isSDCardPresent) {
        if (strstr(command, "?") != NULL) getNextFilename(command);
        if (!SD.exists(command)) {                    // The file doesn't exists.
          SdFile::dateTimeCallback(dateTime);
          File file = SD.open(command, FILE_WRITE);  // Creates the file with the right timestamp
          file.close();
          SdFile::dateTimeCallbackCancel();
        }
        dataFile.close();
        strcpy(AirDataComputer._logfile, command);
        //dataFile = SD.open(AirDataComputer._logfile, O_WRITE | O_CREAT | O_APPEND);
        dataFile = SD.open(AirDataComputer._logfile, FILE_WRITE);
        if (dataFile) { // if the file is available, write to it:
          AirDataComputer._status[AIRDC_STATUS_SD] = '1';  // SD Card present
        } else {        // if the file isn't open, pop up an error:
          AirDataComputer._status[AIRDC_STATUS_SD] = '0';  // SD Card not present
          goto endeval;
        }
        AirDataComputer._status[AIRDC_STATUS_SD] = '1';  // SD Card present
      }
      strcpy (Answer, "$LCA");
      strcat (Answer, SEPARATOR);
      strcat (Answer, AirDataComputer._logfile);
    }

    // --------------------------------------------------
    // #12 – LCQ – LOG_CURRENTFILE_REQ                   --> Reply LCA - LOG_CURRENTFILE_ASSERT
    // --------------------------------------------------

    if (!strcmp(command, "$LCQ")) {
      strcpy (Answer, "$LCA");
      strcat (Answer, SEPARATOR);
      strcat (Answer, AirDataComputer._logfile);
      goto endeval;
    }

    // --------------------------------------------------
    // #13 – LCA – LOG_CURRENTFILE_ASSERT
    // --------------------------------------------------

    if (!strcmp(command, "$LCA")) {
      goto endeval;
    }

    // --------------------------------------------------
    // #14 – DFS – DATA_FREQ_SET                         --> Reply DFA - DATA_FREQ_ASSERT
    // --------------------------------------------------
    // $DFS,Serial_freq,Bluetooth_freq,SD_freq                                    <-- SPECS CHANGED
    // $DFS,5,0.5,0    Serial = 1 Hz           Bluetooth = 0.5 Hz   No SD logging
    // $DFS,=,0.5,5    Serial = leave as is    Bluetooth = 0.5 Hz   SD = 5 Hz

    if (!strcmp(command, "$DFS")) {  // $DFS,Serial,Bluetooth,SD
      command = strtok (NULL, SEPARATOR);
      if (command != NULL) {
        float DataFrequency_COM = (strcmp(command, LEAVE_AS_IS) ? atof(command) : -1);    // Read the freq value
        command = strtok (NULL, SEPARATOR);
        if (command != NULL) {
          float DataFrequency_BT = (strcmp(command, LEAVE_AS_IS) ? atof(command) : -1);   // Read the freq value
          command = strtok (NULL, SEPARATOR);
          if (command != NULL) {
            float DataFrequency_SD = (strcmp(command, LEAVE_AS_IS) ? atof(command) : -1); // Read the freq value
            // Change frequencies
            if ((DataFrequency_COM >= 0) && (DataFrequency_COM != sendtoserial_freq)) {
              sendtoserial_freq = DataFrequency_COM;
              if (sendtoserial_freq > 0) {
                sendtoserial_interval = (long)(1.0f / sendtoserial_freq * 1000.0f);
                sendtoserialTimer.interval(sendtoserial_interval);
                sendtoserialTimer.reset();
                sendtoserial_needs_acquisition = true;
              }
            }
            if ((DataFrequency_BT >= 0) && (DataFrequency_BT != sendtobluetooth_freq)) {
              sendtobluetooth_freq = DataFrequency_BT;
              if (sendtobluetooth_freq > 0) {
                sendtobluetooth_interval = (long)(1.0f / sendtobluetooth_freq * 1000.0f);
                sendtobluetoothTimer.interval(sendtobluetooth_interval);
                sendtobluetoothTimer.reset();
                sendtobluetooth_needs_acquisition = true;
              }
            }
            if ((DataFrequency_SD >= 0) && (DataFrequency_SD != sendtosd_freq)) {
              digitalWrite(ledPin, LOW); // set the LED off
              if (dataFile) dataFile.close();
              sendtosd_freq = DataFrequency_SD;
              if (sendtosd_freq > 0) {
                SDCardCheck();
                //dataFile = SD.open(AirDataComputer._logfile, O_WRITE | O_CREAT | O_APPEND);
                dataFile = SD.open(AirDataComputer._logfile, FILE_WRITE);
                if (dataFile) {
                  AirDataComputer._status[AIRDC_STATUS_SD] = '1';  // SD Card present
                  sendtosd_interval = (long)(1.0f / sendtosd_freq * 1000.0f);
                  sendtosdTimer.interval(sendtosd_interval);
                  sendtosdTimer.reset();
                  sendtosd_needs_acquisition = true;
                  digitalWrite(ledPin, HIGH); // set the LED on
                } else {
                  AirDataComputer._status[AIRDC_STATUS_SD] = '0';  // SD Card not present
                  sendtosd_freq = 0;
                }
              }
            }
          }
        }
      }
      strcpy (Answer, "$DFA,");
      char f1[20], f2[20], f3[20];
      sprintf(f1, "%.3f", sendtoserial_freq);
      strcat (Answer, f1);
      strcat (Answer, SEPARATOR);
      sprintf(f2, "%.3f", sendtobluetooth_freq);
      strcat (Answer, f2);
      strcat (Answer, SEPARATOR);
      sprintf(f3, "%.3f", sendtosd_freq);
      strcat (Answer, f3);

      // Set the new delaymicroseconds_interval = 0.5% of the minimum of the log delays
      long delay_interval = 1000l;    // 1 second min
      if (sendtoserial_freq > 0)    delay_interval = fminl(sendtoserial_interval, delay_interval);
      if (sendtobluetooth_freq > 0) delay_interval = fminl(sendtobluetooth_interval, delay_interval);
      if (sendtosd_freq > 0)        delay_interval = fminl(sendtosd_interval, delay_interval);
      if (delay_interval < 20l) delay_interval = 20l;
      delaymicroseconds_interval = (int)(delay_interval * 5);
      //Serial.println(delaymicroseconds_interval);           // Left for debug purpose

      goto endeval;
    }

    // --------------------------------------------------
    // # 15 – DFQ – DATA_FREQ_REQ                        --> Reply DFA - DATA_FREQ_ASSERT
    // --------------------------------------------------
    // $DFQ

    if (!strcmp(command, "$DFQ")) {
      strcpy (Answer, "$DFA,");
      char f1[20], f2[20], f3[20];
      sprintf(f1, "%.3f", sendtoserial_freq);
      strcat (Answer, f1);
      strcat (Answer, SEPARATOR);
      sprintf(f2, "%.3f", sendtobluetooth_freq);
      strcat (Answer, f2);
      strcat (Answer, SEPARATOR);
      sprintf(f3, "%.3f", sendtosd_freq);
      strcat (Answer, f3);
      goto endeval;
    }

    // --------------------------------------------------
    // #16 – DFA – DATA_FREQ_ASSERT
    // --------------------------------------------------

    if (!strcmp(command, "$DFA")) {
      goto endeval;
    }

    // --------------------------------------------------
    // #17 - FMQ - FILE_MANAGER_REQ
    // --------------------------------------------------

    if (!strcmp(command, "$FMQ"))
    {
      SDCardCheck();
      File dir;
      int n = 0;
      if (strlen(command) < 1) goto endeval;

      command = strtok (NULL, SEPARATOR);   // Required command
      char *param = nullptr;
      param = strtok (NULL, SEPARATOR);     // Parameter

      if (!strcmp(command, "LST")) {          // List files on the SD
        strcpy (Answer, "$FMA,LST");
        if (!isSDCardPresent) goto endeval;
        dir = SD.open("/");            // open root

        // Send a string with the total number of files to be sent
        n = 0;
        while (true) {
          File entry =  dir.openNextFile();
          if (!entry) break;
          if (!entry.isDirectory()) n++;
          entry.close();
        }
        dir.close();
        dir = SD.open("/");                 // open root
        strcat (Answer, SEPARATOR);
        itoa(n, workbuff, 10);              // Append number of files
        strcat (Answer, workbuff);
        while (true) {
          File entry2 =  dir.openNextFile();
          if (!entry2) break;
          if (!entry2.isDirectory()) {
            strcat (Answer, SEPARATOR);
            strcat (Answer, entry2.name()); // Send out filename
            strcat (Answer, SEPARATOR);
            ltoa(entry2.size(), workbuff, 10);
            strcat (Answer, workbuff);      // Send out file size
          }
          entry2.close();
        }
        dir.close();
        goto endeval;
      }

      if (!strcmp(command, "NEW"))            // Requires creation of a file
      {
        strcpy (Answer, "$FMA,NEW");
        if (!isSDCardPresent) goto endeval;
        if (strstr(param, "?") != NULL) getNextFilename(param);
        if (!SD.exists(param)) {                    // The file doesn't exists.
          SdFile::dateTimeCallback(dateTime);
          File file = SD.open(param, FILE_WRITE);  // Creates the file with the right timestamp
          file.close();
          SdFile::dateTimeCallbackCancel();
        }
        dir = SD.open(param, FILE_WRITE);
        if (SD.exists(param))
        {
          strcat (Answer, SEPARATOR);
          //strcpy(airdata->_logfile,param);// Now you can set the file as current using #LCS command
          strcat (Answer, param);
        }
        dir.close();
        goto endeval;
      }

      if (!strcmp(command, "DEL"))            // Requires deletion of a file
      {
        strcpy (Answer, "$FMA,DEL");
        if (!isSDCardPresent) goto endeval;
        if (strcmp(param, AirDataComputer._logfile)) {  // The file must not be the default log file
          SD.remove(param);
          if (!(SD.exists(param)))
          {
            strcat (Answer, SEPARATOR);
            strcat (Answer, param);
          }
        }
        goto endeval;
      }

      if (!strcmp(command, "PRP"))            // Requires information about one file
      {
        strcpy (Answer, "$FMA,PRP");
        if (!isSDCardPresent) goto endeval;
        if (SD.exists(param)) {               // if file exists
          dir = SD.open(param);
          strcat (Answer, SEPARATOR);
          strcat (Answer, dir.name());        // Send out filename
          strcat (Answer, SEPARATOR);
          ltoa(dir.size(), workbuff, 10);
          strcat (Answer, workbuff);          // Send out file size
          dir.close();
        }
        goto endeval;
      }

      if (!strcmp(command, "DMP"))        //Requires dump of one file
      {
        strcpy (Answer, "$FMA,DMP");
        if (!isSDCardPresent) goto endeval;
        dir = SD.open(param);
        // if the file is available
        if (dir) {
          if (endmsg_COM) {
            strcat (Answer, SEPARATOR);
            strcat (Answer, dir.name());
            Serial.println(Answer);
            strcpy (Answer, "");

            while (dir.available() && (!Serial.available())) {
              for (int i = 0; (i < 512) && (dir.available()); i++) {
                Serial.write(dir.read());
              }
              Serial.flush();
              if (Serial.available()) Serial.println();
            }
            /*if (dir.available()) Serial.println("$EOD");
              else*/ Serial.println("$EOF");
          }
          if (endmsg_BT) {
            strcat (Answer, SEPARATOR);
            strcat (Answer, dir.name());
            Serial1.println(Answer);
            strcpy (Answer, "");

            while (dir.available() && (!Serial1.available())) {
              for (int i = 0; (i < 512) && (dir.available()); i++) {
                Serial1.write(dir.read());
              }
              Serial1.flush();
              if (Serial1.available()) Serial1.println();
            }
            /*if (dir.available()) Serial.println("$EOD");
              else*/ Serial1.println("$EOF");
          }
          dir.close();
        }

        goto endeval;
      }
    }

    // --------------------------------------------------
    // #19 - CCS–CALIBRATION_CON_SET
    // --------------------------------------------------
    if (!strcmp(command, "$CCS"))
    {
      float SensorID, CalibMode;
      if (strlen(command) < 1) goto endeval;
      command = strtok (NULL, SEPARATOR);   // Required command
      if (!strcmp(command, "EXE")) {        //Execute calibration command
        command = strtok (NULL, SEPARATOR);
        if (command != NULL) {
          SensorID = (atof(command));    // Read the Sensor ID value
          command = strtok (NULL, SEPARATOR);
          if (command != NULL) {
            CalibMode = (atof(command));    // Read required calibration mode
          }
        }
      }
      strcpy (Answer, "$CCA,");
      char f1[20], f2[20], f3[20];
      sprintf(f1, "%.3f", SensorID);
      strcat (Answer, f1);
      strcat (Answer, SEPARATOR);
      sprintf(f2, "%.3f", CalibMode);
      strcat (Answer, f2);
      goto endeval;
    }

endeval:


    // 3) In case answers to the right applicant

    if (strcmp(Answer, "")) {
      if (endmsg_COM) Serial.println(Answer);
      if (endmsg_BT)  Serial1.println(Answer);
    }
  }

  delayMicroseconds(delaymicroseconds_interval); //delay
}



