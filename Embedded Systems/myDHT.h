#ifndef myDHT_H
#define myDHT_H

#include <DHT.h>


#define wait 10000  // amount of loops to wait for a response of the DHT




int get_temp_hum(int temperature[], int humidity[], int i, DHT dht);
bool testDHT(uint8_t dhtPin);










#endif
