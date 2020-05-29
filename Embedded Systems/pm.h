#ifndef PM_H
#define PM_H
#include "UARTPic.h"





class pm
{
public:
    pm(UartPort uartPort, UARTPic * uartPic);
    char receivePm(int * pm1, int * pm2,int * pm3);

private:
    UartPort _uartPort;
    UARTPic * _uartPic;
};

#endif // PM_H
