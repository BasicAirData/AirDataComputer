/**
 * CapCom.cpp - Library for Basic Air Data Communications
 * Under renew to fit the new requirements 18-04-2017
 * Created by J. Larragueta, December 3, 2015.
 * https://github.com/BasicAirData/AirDataComputer/wiki/Communication
 * http://www.basicairdata.eu/
*/
#include "CapCom.h"
#include <stdio.h>

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
char del[2]=","; //delimiter
//first field is the command
tmp = strtok(msg,del);
// bla bla
for(tmp=strtok(NULL,del); tmp!=NULL; tmp=strtok(NULL,del))
{
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


