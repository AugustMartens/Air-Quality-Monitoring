#include <SoftwareSerial.h>
#include <SPI.h>

//#include <rn2xx3.h>
//#include <mySensor.h>
#include "myDHT.h"
#include <DHT_U.h>
#include <DHT.h>
#include "O3.h"
#include "CO2.h"
#include "UARTPic.h"
#include "pm.h"
//#include "loraModule.h"
#include "datalogger.h"





//Defines
#define AMOUNT 15                 // TODO send this value to PIC
#define DHTTYPE DHT11
#define BAUDRATEUART0 B9600
#define BAUDRATEUART1 B9600
#define BAUDRATEUART2 B9600
#define BAUDRATEUART3 B9600
#define SDAVAILABLE 0
#define SDUNAVAILABLE 1



//Define pins
#define DHTPIN 5
#define chipSelectSD 3
#define UARTPORTPM fourth
#define UARTPORTCO2 second
#define UARTPORTGPS first
#define UARTPORT4 third
#define CHIPSELECTUARTPIC A5
#define chipSelectO3 4
#define RX 7
#define TX 8
#define so2config 0x00
#define cardDetection 9

//potentiostat data
#define setupPotentiostat 0b00111010
#define resistorsPotentiostat 0b11111000

enum Names{
  UARTPortPm = UARTPORTPM, UARTPortCO2 = UARTPORTCO2, UARTPortGPS = UARTPORTGPS, UARTPort4 = UARTPORT4
};
//initialisation of the sensors
SoftwareSerial loraSerial(RX, TX);
//create an instance of the rn2xx3 library,
//giving the software serial as port to use

//LoraModule myLoraModule(TX, RX, loraSerial);
UARTPic * uartPic = new UARTPic(BAUDRATEUART0,BAUDRATEUART1,BAUDRATEUART2,BAUDRATEUART3,CHIPSELECTUARTPIC);
CO2 co2(UARTPORTCO2,1,uartPic);
DHT  dht(DHTPIN, DHTTYPE);
O3 o3(chipSelectO3);
pm pm(UARTPORTPM,uartPic);
Datalogger * sd = new Datalogger();

//definition of variables
bool dhtAvailable = false;
bool O3Available = false;
bool SO2 = false;
bool CO2Available = false;
bool GpsAvailable = false;
bool PmAvailable = false;
bool LORAWan = false;
bool LORALocal = false;
bool SDcard = false;
bool UART4Available = false;

//init sensor variables

int O3Value;
int CO2Value;
int pm1;
int pm2;
int pm3;
byte sensorAvailablePic;
int amount;
int uartportPM;
int uartportCO2;
int uartportGPS;
int uartport4;
long longitude;
long latitude;
byte day;
byte month;
int year;
byte hours;
byte minutes;
byte seconds;
const char *appEuiDingNet = "80BACEC94C07D533";
const char *appKeyDingNet = "6C36EF7447D64AE27B34C40E11BC02A6";
const char *appEuiLocal = "80BACEC94C07D533";
const char *appKeyLocal = "6C36EF7447D64AE27B34C40E11BC02A6";




//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void setup() {
  Serial.begin(9600);
   //Serial.println(SD.begin(chipSelectSD));
  //SD.end();
  pinMode(A4,OUTPUT);
  digitalWrite(A4,HIGH);
    
 
  pinMode(CHIPSELECTUARTPIC,OUTPUT);
  pinMode(cardDetection,INPUT_PULLUP);
  
  String configFile;
if(digitalRead(cardDetection)==1){
  
  amount = AMOUNT;
  uartportPM = UARTPORTPM;
  uartportCO2 = UARTPORTCO2;
  uartportGPS = UARTPORTGPS;
  uartport4 = UARTPORT4;
  SDcard = SDUNAVAILABLE;
  Serial.println("SD card not availalble");
}
else{
  
  SDcard = SDAVAILABLE;  
  configFile = sd->readSD();
  
//Serial.println(configFile);   // to test in which format the file is read
if(configFile.equals("fileUnreachable")){
  amount = AMOUNT;
  uartportPM = UARTPORTPM;
  uartportCO2 = UARTPORTCO2;
  uartportGPS = UARTPORTGPS;
  uartport4 = UARTPORT4;  
}
else{
  getConfigFromFile(&configFile);
}
}




byte sendBuffer[]={amount, UARTPORTPM, UARTPORTCO2, UARTPORTGPS,UARTPORT4, setupPotentiostat,resistorsPotentiostat};
int amountSend = sizeof(sendBuffer);
  
//while(1){
sensorAvailablePic = uartPic->sendConfig(amountSend,sendBuffer); // amount, UARTPORTPM, UARTPORTCO2, UARTPORTGPS,UARTPORT4, I2C SO2config //after this do something else because the pic needs time to ask its sensors if they are available
//sensorAvailablePic = uartPic->receiveConfig();
//delay(5000);
//}

dhtAvailable = testDHT(DHTPIN);
//Serial.println(dhtAvailable);
O3Available = o3.setupO3();
//sensorAvailablePic = uartPic->receiveConfig();
if((sensorAvailablePic & 0b00000001) == 00000001){
   GpsAvailable= true;
}
if((sensorAvailablePic & 0b00000010) == 00000010){
 CO2Available = true;
}
if((sensorAvailablePic & 0b00000100) == 00000100){
 UART4Available = true;
}
if((sensorAvailablePic & 0b00001000) == 00001000){
 PmAvailable = true;
}
if((sensorAvailablePic & 0b00010000) == 00010000){
  SO2 = true;
}

    /*loraSerial.begin(57600); //serial port to computer
    loraSerial.begin(9600); //serial port to radio
    // setup inputs with pull ups
    pinMode(CHIPSELECTUARTPIC, OUTPUT);


    //check if sensors are available
    dhtAvailable = testDHT(DHTPIN);
    O3Available = o3.setupO3();
    SO2 = false;
    CO2Available = co2.isConnectedBySPI();
    Gps = false;
    Pm = false;
    if(LORAWan = myLoraModule.initialize_radio(appEuiDingNet,appKeyDingNet))
    {
        LORALocal = false;
    }
    else
    {
        LORALocal = myLoraModule.initialize_radio(appEuiLocal,appKeyLocal);
    }
    SDcard = !SD.begin(chipSelectSD); //TODO beter via CD Switch*/




}




int calculateAverage(int toCalculateAvFrom[])
{
    int average = 0;
    for(unsigned int i = 0; i<amount;i++)
    {
        average += toCalculateAvFrom[i];
    }
    average = average/amount;
    return average;

}


bool getConfigFromFile(String * message)
{
  
  int i = 0;
  int j = 0;
  
  while(i <(message->length()-1))
  {
  
  if(message->substring(i,i+1).equals("="))
  {   
    int k = i+2;
    String mySub = message->substring(j,i-1);
    String number;

//Serial.print(" i = ");
//Serial.println(i);
    while(k < message->length()-1 )
    {
      //Serial.print(" k = ");
      //Serial.println(k);
      if(message->substring(k,k+1).equals(";"))
      {
        number = message->substring(i+2,k);
        break;
        
      }
      k++;
    }
    if(mySub.equals("amount"))
    {
      
       amount = number.toInt();
      
    }
    else if(mySub.equals("UARTPORTPM"))
    {
      uartportPM = number.toInt();
    }
    else if(mySub.equals("UARTPORTCO2")){
      uartportCO2 = number.toInt();
    }
    else if(mySub.equals("UARTPORTGPS")){
      uartportGPS = number.toInt();
    }
    else if(mySub.equals("UARTPORT4")){
      uartport4 = number.toInt();
    }
    i+=5+number.length(); 
    j = i;
       
  }
  
  i++;
}
}

void checkPicConfig(){
  
  byte sendBuffer[]={amount, UARTPORTPM, UARTPORTCO2, UARTPORTGPS,UARTPORT4, setupPotentiostat,resistorsPotentiostat};
  int amountSend = sizeof(sendBuffer);
  sensorAvailablePic = uartPic->sendConfig(amountSend,sendBuffer); // amount, UARTPORTPM, UARTPORTCO2, UARTPORTGPS, I2C SO2config //after this do something else because the pic needs time to ask its sensors if they are available


if((sensorAvailablePic & 0b00000001) == 0b00000001){
  GpsAvailable = true;
}
if((sensorAvailablePic & 0b00000010) == 0b00000010){
 CO2Available = true;

}
if((sensorAvailablePic & 0b00000100) == 0b00000100){
   UART4Available = true;
}
if((sensorAvailablePic & 0b00001000) == 0b00001000){
  PmAvailable = true;
}
if((sensorAvailablePic & 0b00010000) == 0b00010000){
  SO2 = true;
}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {  
Serial.println("test");
digitalWrite(A4,HIGH);

  
 int counter = 0;

checkPicConfig(); // check if the sensors are still attatched to the PIC

if(digitalRead(cardDetection)==1){
   SDcard = SDUNAVAILABLE;
}
else{
  SDcard =SDAVAILABLE;  
}

  
   int temperature[amount] = {0};
  int humidity[amount] ={0};
  int O3Values[amount] ={0};
    for(int i = 0;i<amount;i++)
  {
 
    //Serial.print("i ");
    //Serial.println(i);
    //get measurements from the DHT
    if(dhtAvailable)
    {
      int error = get_temp_hum(temperature, humidity, i,dht);
      if( error == -1)
      {
        //TODO Error handling
        Serial.print("error temp ");
        Serial.println(error);
      }
      
      }
      
    
   /* if(O3Available)
    {
      
      
      O3Values[i] = o3.measureO3(i,O3Value,temperature,humidity); // is this the right manner to address the function
      Serial.print("O3Value: ");
      Serial.println(O3Values[i]);
     
      if(O3Value < 0)
      {
        O3Available = false;
      }
      
    }*/
  }
Serial.println("test2");

  
    if(CO2Available){
      //ask CO2 data to PIC
    byte receiveBuffer[2]; // because CO2 value is an unsigned int
 
      uartPic->receiveData(uartportCO2,2,receiveBuffer);
      
      CO2Value =  receiveBuffer[1] | receiveBuffer[0] << 8;
     
   }
      
    

    if(GpsAvailable){
      byte receiveBuffer[15];
      uartPic->receiveData(uartportGPS,15,receiveBuffer);
      longitude = receiveBuffer[0]|(long) receiveBuffer[1]<<8 | (long) receiveBuffer[2]<<16 | (long) receiveBuffer[3]<<24;
      latitude = receiveBuffer[4]| (long)receiveBuffer[5]<<8 | (long)receiveBuffer[6]<<16 | (long)receiveBuffer[7]<<24;
      day = receiveBuffer[8];
      month = receiveBuffer[9];
      year = receiveBuffer[10]| receiveBuffer[11]<<8;
      hours = receiveBuffer[12];
      minutes = receiveBuffer[13];
      seconds = receiveBuffer[14];
    }
    delay(30);

    if(PmAvailable){
      byte receiveBuffer[6]; 
      uartPic->receiveData(uartportPM,6,receiveBuffer);
      pm1= receiveBuffer[1] | receiveBuffer[0] << 8;
      pm2= receiveBuffer[3] | receiveBuffer[2] << 8;
      pm3= receiveBuffer[5] | receiveBuffer[4] << 8;
      Serial.print("ReceiveBuffer: ");
      for(int i =0;i<6;i++){
      Serial.print("  ");
      Serial.print(receiveBuffer[i]);
      }
      Serial.println("");
    }
  



delay(1000);


        
        
    

 
    if(LORALocal)
    {

        //TODO set false if out of range en try join otaa
    }
    else if(LORAWan)
    {

        //TODO Set false if out range
    }
    if(SDcard == SDAVAILABLE) // always save to SD if there is one available
    {
        //TODO in datalogger
        String dataString = "";

        if(dhtAvailable)
        {
            dataString += ",temperature:";
            dataString += String(calculateAverage(temperature));
            dataString += ",humidity:";
            dataString += String(calculateAverage(humidity));
        }
        if(CO2Available)
        {
            dataString +=",CO2:";
            dataString += String(CO2Value);     
        }
        if(O3Available){
          dataString +=",O3:";
            dataString += String(calculateAverage(O3Values));   
        }
        if(PmAvailable){
          dataString +=",PM1.0:";
          dataString +=String(pm1);
          dataString +=",PM2.5:";
          dataString +=String(pm2);
          dataString +=",PM10:";
          dataString +=String(pm3);
        }
      // Serial.print("dataString @ the end : ");
       //Serial.println(dataString);
       int test = sd->writeSD(dataString);
       
        if(test != 1){
          Serial.println("file not found, data not saved on SD");
        }
      

    }
    else
    {

    }

    
    delay(15);
    /*Serial.print("CO2: ");
    Serial.println(CO2Value);
    Serial.print("O3: ");
    Serial.print(O3Values[0]);
    Serial.println(" UG_M3");
    Serial.print("CO2: ");
    Serial.println(CO2Value);
    Serial.print("Pm1.0: ");
    Serial.println(pm1);
    Serial.print("Pm2.5: ");
    Serial.println(pm2);
    Serial.print("Pm10.0: ");
    Serial.println(pm3);
    Serial.print("DHT temp: ");
    Serial.println(temperature[0]);
    Serial.print("DHT Hum: ");
    Serial.println(humidity[0]);
    Serial.print("longitude: ");
    Serial.println(longitude);
    Serial.print("card detection: ");
    Serial.println(SDcard);
    Serial.println("");

    
    Serial.println("");*/
    
  digitalWrite(A4,LOW);
  delay(100);

}
