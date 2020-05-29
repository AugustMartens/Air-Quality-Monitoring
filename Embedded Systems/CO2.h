/* 
 *  
 *  based on the MHZ library By Tobias Schürg
*/

#ifndef CO2_H
#define CO2_H



#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SoftwareSerial.h>
#include "UARTPic.h"

// types of sensors.
#define MHZ14A  14


// status codes
#define  STATUS_NO_RESPONSE -2
#define MHZ14A_RESPONSE_TIME  60
#define STATUS_CHECKSUM_MISMATCH -3
#define STATUS_INCOMPLETE -4
#define STATUS_NOT_READY -5

class CO2{
  
public:

  CO2(UartPort uartPort,  uint8_t type, UARTPic * uartPic);


  void setCO2Debug(boolean enable);  
  boolean isReady();  
  int readCO2UARTBySPI(int *CO2); 
  char getLastTemperature();
  int getCO2BySPI( int CO2[], int i);
  int calibrateUARTBySPI();// calibrate by uart, at outdoor air for some time, puts of the automatic recalibration of for the timespan of being powered
  boolean isConnectedBySPI();// test if the device is connected to the Uart SPI bridge by requesting a measurement

private: 
 uint8_t _type;
 char temperature;
 UartPort _uartPort;
 boolean debug = false;   
 UARTPic * _uartPic;
 boolean preHeating = true;
 
};

#endif
