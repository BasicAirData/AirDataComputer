/**
 * CapCom.h - Library for Basic Air Data Communications
 * Created by J. Larragueta, December 3, 2015.
 * Under renew to fit the new requirements 18-04-2017
 * Created by J. Larragueta, December 3, 2015.
 * https://github.com/BasicAirData/AirDataComputer/wiki/Communication
 * http://www.basicairdata.eu/
*/

#ifndef CapCom_h
#define CapCom_h
#include <AirDC.h>
class CapCom
{
  public:
    CapCom(int pid);
    void HandleMessage(AirDC *airdata);
    int _pid;
};
#endif
