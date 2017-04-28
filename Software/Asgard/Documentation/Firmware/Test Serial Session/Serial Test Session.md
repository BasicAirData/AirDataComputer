## Example Test Communication Session Between a Remote unit, R,  and an Asgard ADC. All the communications strings are terminated with a carriage return char '\n'

This sequence of commands can be used, for example, to configure and use a benchtop ADC unit. To be noted that the unit should periodically send to R the data required by the default configuration. To attain an effective communication these messages are removed.


Remote R unit hails ADC and send his identification "R" and its protocol version is number "0.2"

$HBO,R,0.2

ADC

$HBA,Asgard,0.2

R sends time for syncronization (1493384219 -> 04/28/2017 @ 12:56pm (UTC))

$TMS,1493384219

ADC sync the time and acknolege with his current time

$TMA,1493384219

R set the status of ADC unit, decides whitch hardware to use. SD card not present, BT module present.

$STS,0,1,1,1,1,1,1,1,N,1 

ADC ack

$STA,0,1,1,1,1,1,1,1,N,1 

R sets the data rate of ADC, 6 Hertz/Samples per second

$DFS,6

ADC ack

$DFA,6

R select whicth datafields wants to receive from ADC. In this case all 24 fields

$DTQ,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1

ADC ack

$DTA,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1

Time passes. R requires than that the log file is transmitted to it.

R changes the configuration to include the micro SD card

$STS,1,1,1,1,1,1,1,1,N,1 

Time passes. A series of $DTA,* lines is transmitted to R and saved to SD.


R requires the saved data

$LGQ

ADC sends all the lines of the datalog file; A series of $DTA,* lines is transmitted to R.

After receiving the data R requires deleting the log file from ADC

$LGD



