/**
 * CapCom.cpp - Library for Basic Air Data Communications
 * Under renew to fit the new requirements 18-04-2017
 * Created by J. Larragueta, December 3, 2015.
 * https://github.com/BasicAirData/AirDataComputer/wiki/Communication
 * http://www.basicairdata.eu/
*/
#include "CapCom.h"
#include <stdio.h>
#include <string.h>
#include <SD.h>

CapCom::CapCom(int pid)
{
    /** CapCom Default
    constructor*/
    //Default parameters values
    _pid = pid;
}
/** Handles the message to the ADC
* @param  *airdata Pointer  to the AirDataComputer to use to gather data
* @param  *inmsg Pointer to message to process
* @param  *outstr Pointer to reply message
* @return Void
 */
void CapCom::HandleMessage(AirDC *airdata,char *inmsg, char*outstr)
{
    int counter = 0;
    int value = 0;
    char uDELIMITER[2];  //Delimiter for output string
    uDELIMITER[0]=DELIMITER;
    uDELIMITER[1]='\0';
    char workbuff[200]; //General purpose buffer, used for itoa conversion
//Here we trap each message and react
//This section should be reordered with the most used message first
    char *command = strtok(inmsg,SEPARATOR);
//#0 - HBQ - HEARTBEAT_REQ
    if (!strcmp(command, "$HBQ"))
    {
//Receive the first field(description), null char added manually
        command = strtok (NULL, SEPARATOR);
        if (strlen(command)<1)
        {
            goto furout;
        }
        strncpy(_PeerDevice,command, PEERDEVICE_SIZE);
        _PeerDevice[PEERDEVICE_SIZE - 1] = '\0';
//Receive the second field(firmware_version), null char added manually
        command = strtok (NULL, SEPARATOR);
        if (strlen(command)<1)
        {
            goto furout;
        }
        strncpy(_PeerDeviceVer,command, PEERDEVICE_SIZE_VER);
        _PeerDeviceVer[PEERDEVICE_SIZE_VER - 1] = '\0';
        //Reply #1 -HBA - HEARTBEAT_ASSERT
        strcpy (outstr,"$HBA,");
        strcat (outstr,ADC_NAME);
        strcat (outstr,SEPARATOR);
        strcat (outstr,FIRMWARE_V);
        strcat (outstr,uDELIMITER);
            }
//#2 - TMS - TIMESET
    if (!strcmp(command, "$TMS"))
    {
//Receive the first field(Time as seconds since Jan 1, 1970)
        command = strtok (NULL, SEPARATOR);
        if (strlen(command)<1)
        {
            goto furout;
        }

        setTime(atol(command));
        long int tempo = static_cast<long int> (now());
        ltoa(tempo,workbuff,10);
        //Reply #4 - TMA - TIME_ASSERT
        strcpy (outstr,"$TMA,");
        strcat (outstr,workbuff);
        strcat (outstr,uDELIMITER);
    }
//#3 - TMQ - TIME_REQ
    if (!strcmp(command, "$TMQ"))
    {
        if (strlen(command)<1)
        {
            goto furout;
        }
        long int tempo = static_cast<long int> (now());
        ltoa(tempo,workbuff,10);
        //Reply #4 - TMA - TIME_ASSERT
        strcpy (outstr,"$TMA,");
        strcat (outstr,workbuff);
        strcat (outstr,uDELIMITER);
    }

//#5 - STS - STATUS_SET
    if (!strcmp(command, "$STS"))
    {
        int giro;
        for (giro=0; giro<9; giro++)
        {

            /*Receive the fields from 1 to 9
            1	SD card present
            2	Deltap sensor
            3	Absolute pressure sensor
            4	External Temperaure sensor
            5	Deltap sensor temperature
            6	Absolute pressure sensor temperature
            7	Real time clock battery
            8	Error/Warning
            9   BT interface
            */
            command = strtok (NULL, SEPARATOR);
            if (strlen(command)<1)
            {
                goto furout;
            }
            airdata->_status[giro]=command[0];
            workbuff[2*giro]=command[0];
            workbuff[2*giro+1]=',';
        }
        airdata->_status[giro+1]='\0';
        workbuff[2*giro-1]='\0';
        //Reply #7 - STA - STATUS_ASSERT
        strcpy (outstr,"$STA,");
        strcat (outstr,workbuff);
        strcat (outstr,uDELIMITER);
    }
//#6 - STQ - STATUS_REQ
    if (!strcmp(command, "$STQ"))
    {
        int giro;
        for (giro=0; giro<9; giro++)
        {

            /*Receive the fields from 1 to 9
            1	SD card present
            2	Deltap sensor
            3	Absolute pressure sensor
            4	External Temperaure sensor
            5	Deltap sensor temperature
            6	Absolute pressure sensor temperature
            7	Real time clock battery
            8	Error/Warning
            9   BT interface
            */
            workbuff[2*giro]=airdata->_status[giro];
            workbuff[2*giro+1]=',';
        }
        workbuff[2*giro-1]='\0';
        //Reply #7 - STA - STATUS_ASSERT
        strcpy (outstr,"$STA,");
        strcat (outstr,workbuff);
        strcat (outstr,uDELIMITER);
    }

//#8 - DTS - DATA_SET

    if (!strcmp(command, "$DTS"))
    {
        int giro;
        for (giro=0; giro<7; giro++)
        {
            command = strtok (NULL, SEPARATOR);
            if (strlen(command)<1)
            {
                goto furout;
            }
            airdata->_status[giro]=command[0];
            workbuff[2*giro]=command[0];
            workbuff[2*giro+1]=',';
        }
        airdata->_status[giro+1]='\0';
        workbuff[2*giro-1]='\0';
        //Save to the SD the configuration data
        if(airdata->_status[0]=='1')   //If SD is installed
        {
            File dataFile = SD.open("config.csv", FILE_WRITE);
            // if the file is available, write to it:
            if (dataFile)
            {
                dataFile.println("Config File, Basic Air Data Team, JLJ@BasicAirData 2017");  //Write Status to SD
                dataFile.print("$STA,");
                dataFile.println(workbuff);
                dataFile.close();
            }
            // if the file isn't open, pop up an error:
            else
            {
                goto furout;
            }
        }
        //Modified #10 reply message. No data will be transmitted. It is a plain acknowledge.
        strcpy (outstr,"$DTA,");
        strcat (outstr,uDELIMITER);
    }
    //#9 - DTQ - DATA_REQ -> WIP
    if (!strcmp(command, "$DTQ"))
    {
        int giro;
        for (giro=0; giro<24; giro++)
        {
            //Receive the fields from 1 to 24
            command = strtok (NULL, SEPARATOR);
            if (strlen(command)<1)
            {
                goto furout;
            }
            airdata->_datasel[giro]=command[0];
        }
       // airdata->_datasel[giro+1]='\0';
//Prepare to send #10 - DTA - DATA_ASSERT message
        strcpy (outstr,"$DTA");
        for (giro=0; giro<24; giro++)
        {
//Check the fields from 1 to 24
            strcat (outstr,SEPARATOR); //Add the separator
            if (airdata->_datasel[giro]=='1')
            {
                airdata->PrepareData();
                sprintf(workbuff, "%f", (airdata->_dataout[giro]));
                strcat (outstr,workbuff);

            }
            else
            {
            }

        }
        strcat (outstr,uDELIMITER);
    }

    //#17 - LGD - LOG_FILE_DELETE
    if (!strcmp(command, "$LGD"))
    {
        SD.remove("datalog.csv");
    }
//#18 - LGQ - LOG_FILE_REQ
    if (!strcmp(command, "$LGQ"))
    {
        File dataFile = SD.open("datalog.csv");

        // if the file is available
        if (dataFile)
        {
            while (dataFile.available()) //To be modified?
            {
                if (airdata->_status[7]==0){ //Serial port only
                Serial.write(dataFile.read()); //<- Incomplete : To change that part to handle serial + BT
                }
                if (airdata->_status[7]==1){ //BT module installed on Serial1
                Serial1.write(dataFile.read()); //<- Incomplete : To change that part to handle serial + BT
                }
            }
            dataFile.close();
            //Serial.println("Dump done!");
        }
        // if the file isn't open, pop up an error:
        else
        {
            //Serial.println("error opening datalog.csv");
            goto furout;
        }
    }
//Service message
    if (!strcmp(command, "$STR"))           // Received a command, for example "$STR,1"
    {
        if (strlen(command)<1)
        {
            goto furout;
        }
        command = strtok (NULL, SEPARATOR);
        value = atoi(command);                // Read the value after the comma, for example "1". Integer conversion
        counter = counter + value;
        if (counter > 1000) counter = 0;      // A reset, for the serial gamers :)
        //--------------------------------------------------------------------------
        strcpy (outstr,"$ANS,");
        itoa(counter,workbuff,10);
        strcat (outstr,workbuff);
        strcat (outstr,uDELIMITER);
    }
furout:
    ;
}



