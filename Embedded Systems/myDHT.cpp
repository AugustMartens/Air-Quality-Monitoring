// First install "DHT sensor library" via the Library Manager
#include "myDHT.h"





int get_temp_hum(int temperature[], int humidity[], int i, DHT dht)
{
  dht.begin();  
  int amount = sizeof(temperature)/sizeof(typeof(temperature));
  if( i <0 /*|| i> amount*/){
    return -1;
  }
 amount = sizeof(humidity)/sizeof(typeof(humidity));
  if( i <0 /*|| i> amount*/){
    return -1;
  }

  bool gelukt = false;
  
 while(gelukt == false){
    temperature[i] = dht.readTemperature(false)  ;
    humidity[i] = dht.readHumidity(false);
  
   if(temperature[i] !=0 && humidity[i] !=0  ){
     gelukt = true;
      //Serial.println(temperature[i]);
      //Serial.println(humidity[i]);
  
  }
 }
  

  return 0;
  
}
 
  



bool testDHT(uint8_t dhtPin){  
pinMode(dhtPin,OUTPUT);
 digitalWrite(dhtPin,LOW);
 delay(20);
 digitalWrite(dhtPin,HIGH);
 pinMode(dhtPin,INPUT_PULLUP);
 for(int i = 0;i<wait;i++){
    if(digitalRead(dhtPin)==0){
      return true;
      
    }
    
  
  }

return false;

  
}
 
