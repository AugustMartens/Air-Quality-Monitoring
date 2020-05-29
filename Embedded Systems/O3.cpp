#include "O3.h"


/*
   select slave select pin where the ozone sensor is listening on
*/
O3::O3(uint8_t slaveSelect) {
  _slaveselect = slaveSelect;
  settingsO3 = SPISettings(500000, MSBFIRST, SPI_MODE3);
}

/*
   set values for the right type of sensor (LOW_CONCENTRATION /HIGH_CONCENTRATION)
   See if ozone sensor is connected.
   Returns true if connected
*/
bool O3::setupO3() {
  //Serial.begin(9600);
  

  digitalWrite(_slaveselect, HIGH);
  pinMode(_slaveselect, OUTPUT);  // CS (out from Arduino)
  pinMode(O3_MOSI, OUTPUT);  //MOSI (data out from Arduino)
  pinMode(O3_MISO, INPUT);  // MISO (data in to Arduino)
  pinMode(O3_SCK, OUTPUT); // SCK  (serial clock)

  // choose right reference value R0 for the right model LOW_CONCENTRATION or HIGH_CONCENTRATION
  switch (O3_model) {
    case LOW_CONCENTRATION :
      O3_valueR0 = 110470.60; //default
      O3_valueR0 = 65564.00; // calculated
      O3_valueR0 = 5289.70; // calculated
      O3_valueR0 = 5233.19; // calibrate
      break;
    case HIGH_CONCENTRATION :
      O3_valueR0 = 385.40;
      break;
  }


  unsigned long int begina = millis();   //time measurement

  /*
    Specific sequence to activate sensor and start measurement
  */
  digitalWrite(_slaveselect, HIGH);
  delayMicroseconds(100);
  digitalWrite(_slaveselect, LOW);  // start next conversion
  delay(50);            // delay in milliseconds (nominal MCP3550-60 rate: 66.7 msec => 15 Hz)
  int ii = 0;              // use i as loop counter
  do {
    ii++;
    delayMicroseconds(50);                            // loop keeps trying for up to 1 second
  } while ((digitalRead(O3_MISO) == HIGH) && (ii < 20000)); // wait for bit to drop low (ready)

  /*
    get message (6 bytes) from the sensor
    If more then 3 bytes are the same the sensor is not connected
  */
  int recievethesame = 0; // count the times the byte is 255
  byte previous = 25;
  SPI.beginTransaction(settingsO3);
  for (int amount = 0; amount < 6; amount++) {
    byte bita = SPI.transfer(0x00); // get byte from sensor
    //Serial.print("SPI: ");
    //Serial.println(bita);
    if (bita == previous) {
      recievethesame++;
    }
    previous = bita;
  }
  SPI.endTransaction();
 /* Serial.print("time from begin conversion ");
  Serial.println(millis() - begina); // time measurement
  Serial.print("recievethesame: ");*/
  
  //Serial.println(recievethesame); // time measurement
  if (recievethesame >= 3) {
    return false;
  }
  
  return true;
}

/*
   This method asks the sensor for a measurement and convert it to an onzone concentration
   i is for the place where the concentration ozone can be set and where the temperature and humidity can be taken from
   Returns status code
*/
unsigned int O3::measureO3(int i, int ozone[], int temperature[], int humidity[]) {


  if ( i < 0) {
    return -2; // i does not contain an valid value
  }

  int temp = temperature[i];
  int hum = humidity[i];


  if (temp == 0 && hum == 0) {
    return -3; // elements in array are null
  }

  double temperaturenow = ((double)temp) / 100;
  double humiditynow = ((double)hum) / 100;
  /*Serial.print("temp ");
  Serial.print(temperaturenow);
  Serial.print(", hum ");
  Serial.print(humiditynow);*/


  readWordStruct worda;

  worda = readword();    // data in:  32-bit word gets 24 bits via SPI port


  if ((worda.error == 0)) {
    /*Serial.print(", VRL digital ");
    Serial.print(worda.worda);*/
    ozone[i] = (int)(getO3(worda.worda, UG_M3, temperaturenow, humiditynow) * 100);
    /*Serial.println();
      Serial.print("return ");
      Serial.println(0);*/
      /*Serial.print("value ");
      double o3 = ((double) ozone[i]) / 100;
      Serial.print(o3);
      Serial.println(" UG_M3");*/
    return 0;
  } else {
   /* Serial.print("return ");
      Serial.println(worda.error);
      Serial.print("value ");
      double o3 = ((double) ozone[i]) / 100;
      Serial.print(o3);
      Serial.println(" UG_M3");*/
    return worda.error;
  }

}

/*
   This method measures the resistance of the sensor continually and if there are 15 consecutive measurements the same,
   it will store the last value as O3_valueR0
   Use for greater sensitivity of data
   Can take a while
*/
void O3::calibrateO3() {
  // Take care of the last Rs value read on the sensor
  // (forget the decimals)
  float lastRsValue = 0;
  // Count how many time we keep the same Rs value in a row
  int countReadInRow = 0;
  // Count how long we have to wait to have consistent value
  int count = 0;


  int timeToReadConsistency = -1;
  switch (O3_model) {
    case LOW_CONCENTRATION :
      timeToReadConsistency = 15;
      break;
    case HIGH_CONCENTRATION :
      timeToReadConsistency = 20;
      break;
  }

  byte OVL, OVH;      // overload condition HIGH and LOW, respectively
  unsigned int loop_counter;              // loop counter
  unsigned long w;
  long x;
  readWordStruct worda;
  int errors = 0;
  while (countReadInRow <= timeToReadConsistency) {

    worda = readword();    // data in:  32-bit word gets 24 bits via SPI port

    if ((worda.error == 0)) {
      errors = 0;
      float value = readRs(worda.worda);
      Serial.print((int)(value / 1));
      Serial.print(" , lastRsValue: ");
      Serial.println((int)(lastRsValue / 1));
      if ((int)(lastRsValue / 1) != (int)(value / 1)) {
        lastRsValue = value;
        countReadInRow = 0;
      } else {
        Serial.println("goed");
        countReadInRow++;
      }
      count++;
      delay(1000);
    } else {
      errors++;
      if (errors == 10) {
        return worda.error;
      }
    }
  }
  if (lastRsValue < 0) {
    return -8;
  }
  O3_valueR0 = lastRsValue;
  return 0;

}

/*
   This method measures the resistance of the sensor continually and will calculate the average of all the data points
   it will store the average as O3_valueR0
   for best accuracy take multiples of 24 hours
   small variance on runtime is possible
*/
//void O3::calibrateTimeO3(int hours);


/*
   This method measures the resistance of the sensor continually and will calculate the average of all the data points
   it will store the average as O3_valueR0
   for best accuracy take multiples of 24 hours
   small variance on runtime is possible
*/
//void O3::calibrateSamplesO3(uint32_t amount_samples);









struct readWordStruct O3::readword() {
  union {
    unsigned long svar;
    byte c[4];
  } w;        // allow access to 4-byte word, or each byte separately

  byte OVL, OVH;      // overload condition HIGH and LOW, respectively
  unsigned long wo;
  long x;
  readWordStruct returna = {0, 0};
  //SPIsetup();
  
  digitalWrite(_slaveselect, HIGH);
  delayMicroseconds(100);
  digitalWrite(_slaveselect, LOW);  // start next conversion
  delay(50);            // delay in milliseconds (nominal MCP3550-60 rate: 66.7 msec => 15 Hz)
  int loop_counter = 0;              // use i as loop counter
  do {
    loop_counter++;
    delayMicroseconds(50);                            // loop keeps trying for up to 1 second
  } while ((digitalRead(O3_MISO) == HIGH) && (loop_counter < 20000)); // wait for bit to drop low (ready)
  SPI.beginTransaction(settingsO3);
  w.c[3] = SPI.transfer(0x00);  // fill 3 bytes with data: 22 bit signed int + 2 overflow bits
  w.c[2] = SPI.transfer(0x00);
  w.c[1] = SPI.transfer(0x00);
  w.c[0] = 0x00;                // low-order byte set to zero
  wo = w.svar;
  SPI.endTransaction();
  OVL = ((wo & 0x80000000) != 0x00000000);  // ADC negative overflow bit (input > +Vref)
  OVH = ((wo & 0x40000000) != 0x00000000);  // ADC positive overflow bit (input < -Vref)
  if (OVL == 0 && OVH == 0) {
    if ((loop_counter < 10000)) {

      x = wo << 2; // to use the sign bit
      x = x / 1024; // to move the LSB to bit 0 position
    } else {
      returna.error = -5;
    }
  } else {
    returna.error = -6;
  }
  returna.worda = x;

  return (returna);   // return unsigned long word
} // end readword()


float O3::getO3(double measurement, MQ131Unit unit, double temperaturenow, double humiditynow) {
  // calculate resistance value of sensor
  float lastValueRs = readRs(measurement);
  // If no value Rs read, return 0.0
  if (lastValueRs < 0) {
    return 0.0;
  }

  float ratio = 0.0;

  switch (O3_model) {
    case LOW_CONCENTRATION :
      // Use the equation to compute the O3 concentration in ppm
      // R^2 = 0.9987
      // Compute the ratio Rs/R0 and apply the environmental correction
      ratio = lastValueRs / O3_valueR0 * getEnvCorrectRatio(temperaturenow, humiditynow);
      return O3_INDOOR_OUTDOOR_MULTIPLICATION * convert(9.4783 * pow(ratio, 2.3348), PPB, unit);
    case HIGH_CONCENTRATION :
      // Use the equation to compute the O3 concentration in ppm
      // R^2 = 0.99
      // Compute the ratio Rs/R0 and apply the environmental correction
      ratio = lastValueRs / O3_valueR0 * getEnvCorrectRatio(temperaturenow, humiditynow);
      return O3_INDOOR_OUTDOOR_MULTIPLICATION * convert(8.1399 * pow(ratio, 2.3297), PPM, unit);
    default :
      return 0.0;
  }
}

float O3::readRs(double measurement) {

  // Compute the voltage on load resistance (for 5V Arduino)
  float vRL = (((float)measurement) / 4194303) * 5.0;

  // Compute the resistance of the sensor (for 5V Arduino)
  float rS = (5 - vRL) / ((vRL / O3_VALUE_RL));
  return rS;
}

float O3::getEnvCorrectRatio(double temperaturenow, double humiditynow) {
  // Select the right equation based on humidity
  // If default value, ignore correction ratio
  if (humiditynow == 60 && temperaturenow == 20) {
    return 1.0;
  }
  // For humidity > 75%, use the 85% curve
  if (humiditynow > 75) {
    // R^2 = 0.9986
    return -0.0141 * temperaturenow + 1.5623;
  }
  // For humidity > 50%, use the 60% curve
  if (humiditynow > 50) {
    // R^2 = 0.9976
    return -0.0119 * temperaturenow + 1.3261;
  }

  // Humidity < 50%, use the 30% curve
  // R^2 = 0.996
  return -0.0103 * temperaturenow + 1.1507;
}

float O3::convert(float input, MQ131Unit unitIn, MQ131Unit unitOut) {
  if (unitIn == unitOut) {
    return input;
  }

  float concentration = 0;
  // We assume that the unit IN is PPB or PPM as the sensor provide only in PPB and PPM

  // general equation for ozon: µg/m3 = (ppb)*(12.187)*(M molecular weight) / (273.15 + °C)
  // general equation: µg/m3 = (ppb)*(M molecular weight) / [ 22.41 * (T/273) * (1013/p)]
  //temperature is not taken in to account
  switch (unitOut) {
    case PPM :
      // Convert PPB to PPM
      return input / 1000.0;
    case PPB :
      // Convert PPM to PPB
      return input * 1000.0;
    case MG_M3 :
      if (unitIn == PPM) {
        concentration = input;
      } else {
        // Convert PPB to PPM
        concentration = input / 1000.0;
      }
      // UG_M3 = PPM * M * 12.187 / (273.15 + °C)
      return concentration * 48.0 / 22.71108;
    case UG_M3 :
      if (unitIn == PPB) {
        concentration = input;
      } else {
        // Convert PPM to PPB
        concentration = input * 1000.0;
      }
      // UG_M3 = PPB * M * 12.187 / (273.15 + °C)
      return concentration * 48.0 / 22.71108;
    default :
      return input;
  }
}
