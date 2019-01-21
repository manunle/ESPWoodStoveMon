#ifndef MAX31850_H
#define MAX31850_H
#include "31850OneWire.h"

class MAX31850 {
private:
  OneWire *ds;
  byte addr[8];

public:
	MAX31850(uint8_t);
  int init();
	float getTemp();
};

#endif
