#ifndef UARTPIC_H
#define UARTPIC_H

#include <SPI.h>
enum Baudrate{
  B1200 = 0, B2400 = 1, B4800 = 2, B9600 = 3, B19200 = 4, B38400 = 5, B57600 = 6, B115200 = 7
  
};
enum UartPort{
  first = 0, second = 1, third = 2, fourth = 3
};

class UARTPic{
  public:
  UARTPic(Baudrate baudrateUart0,Baudrate baudrateUart1,Baudrate baudrateUart2,Baudrate baudrateUart3,int csPin);
  byte sendConfig( int amountBytes, byte sendBuffer[]);
  byte receiveConfig();
  int sendData(UartPort uartPort, int amountBytes, byte sendBuffer[]);
  int receiveData(UartPort uartPort, int amountBytes, byte receiveBuffer[]);
  int amountReceiveBytes(UartPort uartPort);
  int amountTransmitBytes(UartPort uartPort);

  private:
  int  _csPin;
  int ssPin;

  
};




#endif
