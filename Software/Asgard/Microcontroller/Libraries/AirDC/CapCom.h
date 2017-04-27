/**
 * CapCom.h - Library for Basic Air Data Communications
 * Created by J.L.J., December 3, 2015.
 * Under renew to fit the new requirements 18-04-2017
 * Created by J.L.J., December 3, 2015.
 * https://github.com/BasicAirData/AirDataComputer/wiki/Communication
 * http://www.basicairdata.eu/
*/

#ifndef CapCom_h
#define CapCom_h
#include <AirDC.h>
#define PEERDEVICE_SIZE 20
#define PEERDEVICE_SIZE_VER 10
#define DELIMITER '\n'
#define SEPARATOR ","
#define INPUT_SIZE 512
#define OUTPUT_SIZE 512
#define ADC_NAME "Asgard"
#define FIRMWARE_V "1.0b"

class CapCom
{
  public:
    CapCom(int pid);
    void HandleMessage(AirDC *airdata, char *inmsg, char*outstr);
    void DTA(AirDC *airdata, char*outstr);
    //General use
    int _pid;/**< Class ID */
    int _DataFrequency;/**< Current sample rate for the communications */
    int _ReqPeriod; /**< Required sample rate for the communications */
    //ADC Software Related
    char _PeerDevice[PEERDEVICE_SIZE];/**< Device actually connected for communication with ADC */
    char _PeerDeviceVer[PEERDEVICE_SIZE_VER];/**< Device actually connected for communication with ADC */
};
#endif
