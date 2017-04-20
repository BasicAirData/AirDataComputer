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

CapCom::CapCom(int pid)
{
    /** CapCom Default
constructor*/
    //Default parameters values
    _pid = pid;
}
/** Handles the message to the ADC
* @param  *airdata Pointer  to the AirDataComputer to use to gather data
* @param  *msg Pointer to message to process
* @return Void
 */
void CapCom::HandleMessage(AirDC *airdata,char *msg)
{
//char tmp[20];
char *tmp;
//char cmd[4];
char del[2]=","; //delimiter
//first field is the command
tmp = strtok(msg,del);
//It is safe code even if we have a wrong tmp string
//What if the first field is numeric?
//It will be possible to make one atoi and the a switch

if (strcmp(tmp,"$HBQ")){ //ID 0 message
for(tmp=strtok(NULL,del); tmp!=NULL; tmp=strtok(NULL,del))
{
}
}
if (strcmp(tmp,"$HBA")){ //ID 1 message
for(tmp=strtok(NULL,del); tmp!=NULL; tmp=strtok(NULL,del))
{
}
}
//bla bla all the others IDs
if (strcmp(tmp,"$LGA")){ //ID 19 message
for(tmp=strtok(NULL,del); tmp!=NULL; tmp=strtok(NULL,del))
{
}
}




/*for(tmp=strtok(msg,del); tmp!=NULL; tmp=strtok(NULL,del))
{
}*/




//To read string on the other side
        /*
          if (Serial.find("$TMO,")) {
            _p = Serial.parseFloat(); //
            _T = Serial.parseFloat();//
            _RH = Serial.parseFloat();//
            _qc = Serial.parseFloat();//
        */
}


