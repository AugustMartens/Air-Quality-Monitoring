#include "UARTPic.h"




UARTPic::UARTPic(Baudrate baudrateUart0,Baudrate baudrateUart1,Baudrate baudrateUart2,Baudrate baudrateUart3, int csPin)
{
  
  //TODO eigen functie maken  
  ssPin = csPin;
  SPI.begin();//TODO Als dat in de main gebeurt moet dat niet
  digitalWrite(csPin, LOW);
  SPI.transfer(0x81);
  SPI.transfer(baudrateUart0); 
  SPI.transfer(0x82); 
  SPI.transfer(baudrateUart1); 
  SPI.transfer(0x83); 
  SPI.transfer(baudrateUart2);
  SPI.transfer(0x84); 
  SPI.transfer(baudrateUart3);   
  digitalWrite(csPin, HIGH);
  
  
}

byte UARTPic::sendConfig( int amountBytes, byte sendBuffer[])
{
  SPI.setDataMode(SPI_MODE0);
  
digitalWrite(ssPin, LOW);
int transfercode = 0x50;

SPI.transfer(transfercode);
delay(15);

SPI.transfer(amountBytes);
int i = 0;
for(i; i< amountBytes;i++)
{
  delay(15);
 SPI.transfer(sendBuffer[i]);
  
}
   //digitalWrite(ssPin,HIGH);
    int amountReceive = 1;
  byte receiveBuffer[amountReceive];
delay(20);
SPI.transfer(amountReceive);

  
   i = 0;
    for(i; i< amountReceive;i++)
  {
  delay(30);
     receiveBuffer[i] = SPI.transfer(0xFF);
     Serial.print("receiveBuffer in sendconfig:");
     Serial.println(receiveBuffer[i],BIN);
  }
  
  digitalWrite(ssPin, HIGH);
return receiveBuffer[0];

}
/*
byte UARTPic::receiveConfig(){
  SPI.setDataMode(SPI_MODE2);
  digitalWrite(ssPin, LOW);
  int amountReceive = 1;
  byte receiveBuffer[amountReceive];
delay(20);
SPI.transfer(amountReceive);

  
int   i = 0;
    for(i; i< amountReceive;i++)
  {
  delay(15);
     receiveBuffer[i] = SPI.transfer(0xFF);
     Serial.println(receiveBuffer[i],BIN);
  }
digitalWrite(ssPin,HIGH);
return receiveBuffer[0];
}

*/
int UARTPic::sendData(UartPort uartPort, int amountBytes, byte sendBuffer[])
{
digitalWrite(ssPin, LOW);
int transfercode = 0x41+uartPort;
SPI.transfer(transfercode);
SPI.transfer(amountBytes);
int i = 0;
for(i; i< amountBytes;i++)
{
  SPI.transfer(sendBuffer[i]);
}

digitalWrite(ssPin, HIGH);
  return i;
}




int UARTPic::receiveData(UartPort uartPort, int amountBytes, byte receiveBuffer[])
{
  
 // Serial.print("_cs ");
  //Serial.println(ssPin);
   digitalWrite(ssPin, LOW);
   delay(15);
   int transfercode = 0x20+uartPort;
   delay(15);
   SPI.transfer(transfercode);
   delay(10);
   SPI.transfer(amountBytes);
   delay(30);
   int i =0;
   for( i; i< amountBytes;i++)
   {

       
      receiveBuffer[i] = SPI.transfer(0xFF);
      //Serial.println(receiveBuffer[i]);
      delay(30);      
   }
   
   digitalWrite(ssPin, HIGH);
  return i;
}


int UARTPic::amountReceiveBytes(UartPort uartPort)
{
   digitalWrite(ssPin, LOW);
   int transfercode = 0x11+uartPort;
   SPI.transfer(transfercode);
   int theBytes = SPI.transfer(0xFF);   
   digitalWrite(ssPin, HIGH);
  return theBytes;
}



int UARTPic::amountTransmitBytes(UartPort uartPort)
{
  digitalWrite(ssPin, LOW);
   int transfercode = 0x31+uartPort;
   SPI.transfer(transfercode);
   int theBytes = SPI.transfer(0xFF);   
   digitalWrite(ssPin, HIGH);
  return theBytes;
}
