
/*
   Setup of SPI is not included in this code.
   SPI.setClockDivider(SPI_CLOCK_DIV32);  // SPI clock rate < 5 MHz per MCP3550 spec
*/



#ifndef O3_H
#define O3_H



#if ARDUINO >= 100
#include "Arduino.h"
#else

#endif

/*
   NOG WEG WERKEN? willem
*/
#include <SPI.h> // use Arduino SPI library
#define O3_MOSI /*ICSP-4*/ 11  // MOSI is not used for this device
#define O3_MISO /*ICSP-1*/ 12  // status and data bits from ADC // with pullup to 3.3V
#define O3_SCK  /*ICSP-3*/ 13   // SPI clock from Arduino to ADC


struct readWordStruct {
  unsigned long worda;
  int error;
};

enum MQ131Model {LOW_CONCENTRATION, HIGH_CONCENTRATION}; // different types of ozon sensors // we use the low concentration setting because our sensor works till 20 ppm
enum MQ131Unit {PPM, PPB, MG_M3, UG_M3};

#define O3_model LOW_CONCENTRATION
#define O3_VALUE_RL 5100 // value potentiometer
#define O3_INDOOR_OUTDOOR_MULTIPLICATION 1 // set to two measure outdoor ozon concentration while indoor ( not that accurate)

// status codes
#define  STATUS_GOOD 0
#define  STATUS_NO_RESPONSE -2
#define STATUS_READ_ERROR -6
#define STATUS_OVERFLOW -7

class O3 {

  public:
    /*
       select slave select pin where the ozone sensor is listening on
    */
    O3(uint8_t slaveSelect);

    /*
       set values for the right type of sensor (LOW_CONCENTRATION /HIGH_CONCENTRATION)
       See if ozone sensor is connected.
       Returns true if connected
    */
    bool setupO3();

    /*
       This method asks the sensor for a measurement and convert it to an onzone concentration
       i is for the place where the concentration ozone can be set and where the temperature and humidity can be taken from
       Returns status code
    */
    unsigned int measureO3(int i, int ozone[], int temperature[], int humidity[]);

    /*
       This method measures the resistance of the sensor continually and if there are 15 consecutive measurements the same,
       it will store the last value as O3_valueR0
       Use for greater sensitivity of data
       Can take a while
    */
    void calibrateO3();

    /*
       This method measures the resistance of the sensor continually and will calculate the average of all the data points
       it will store the average as O3_valueR0
       for best accuracy take multiples of 24 hours
       small variance on runtime is possible
    */
    void calibrateTimeO3(int hours);


    /*
       This method measures the resistance of the sensor continually and will calculate the average of all the data points
       it will store the average as O3_valueR0    
       for best accuracy take multiples of 24 hours
       small variance on runtime is possible
    */
    void calibrateSamplesO3(uint32_t amount_samples);

  private:
    uint8_t _slaveselect;
    float O3_valueR0 = -1; // value of sensorresistance with 200 ppm (R0) is changed in setup to the right value
    SPISettings settingsO3; 
    
    struct readWordStruct readword();
    float getO3(double measurement, MQ131Unit unit, double temperaturenow, double humiditynow);
    float readRs(double measurement);
    float getEnvCorrectRatio(double temperaturenow, double humiditynow);
    float convert(float input, MQ131Unit unitIn, MQ131Unit unitOut);
 
};

#endif
