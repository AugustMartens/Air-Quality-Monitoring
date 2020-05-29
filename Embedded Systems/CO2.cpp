#include "CO2.h"


unsigned long lastRequest = 0;

CO2::CO2(UartPort uartPort,  uint8_t type, UARTPic * uartPic) {


  _type = type;
  _uartPort = uartPort;


}


/**
   Enables or disables the debug mode (more logging).
*/
void CO2::setCO2Debug(boolean enable) {
  debug = enable;
  if (debug) {
    Serial.println(F("MHZ: debug mode ENABLED"));
  } else {
    Serial.println(F("MHZ: debug mode DISABLED"));
  }
}


boolean CO2::isReady() {
  if (preHeating) return false;
  if (_type == MHZ14A)
    return lastRequest < millis() - MHZ14A_RESPONSE_TIME;
  else {
    Serial.print(F("MHZ::isReady() => UNKNOWN SENSOR \""));
    Serial.print(_type);
    Serial.println(F("\""));
    return true;
  }
}


char CO2::getLastTemperature() {
  if (preHeating) return STATUS_NOT_READY;
  return temperature;
}



//TODO rewrite
int CO2::readCO2UARTBySPI(int * CO2)
{
    byte myBuffer[3];
    _uartPic->receiveData(_uartPort,3,myBuffer);
       
    if(myBuffer[0] == 0 && myBuffer[1] == 0 && myBuffer[2] == 0)
    {
        preHeating =  true;
        return STATUS_NOT_READY;
    }
    temperature = myBuffer[2];    
    *CO2 = myBuffer[0] | myBuffer[1] << 8;
    //Serial.print("CO2: ");Serial.println(*CO2);
  return 0;
}



int CO2::calibrateUARTBySPI()
{
  //TODO send a new comando
}



boolean CO2::isConnectedBySPI() {

  //TODO NEW commando
  if (true)
  {
    return false;
  }
  else
  {
    return true;



  }
}
