#include "pm.h"






pm::pm(UartPort uartPort, UARTPic *uartPic)
{
    _uartPort = uartPort;
    _uartPic = uartPic;
}


char pm::receivePm(int * pm1, int * pm2,int * pm3)
{
    byte mybuffer[3];
    _uartPic->receiveData(_uartPort,3,mybuffer);
    
    if(mybuffer[0] == 0 && mybuffer[1] == 0 && mybuffer[2] == 0)
    {
        return -1;
    }
    *pm3 = mybuffer[2];
    *pm2 = mybuffer[1];
    *pm1 = mybuffer[0] ;
  return 0;
}
