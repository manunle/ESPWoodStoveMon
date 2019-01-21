#include "MAX31850Temp.h"

MAX31850::MAX31850(uint8_t owPin)
{
  ds = new OneWire(owPin);
}

int MAX31850::init()
{
//  if ( !ds->search(addr)) {
//    Serial.println("No more addresses.");
//    Serial.println();
//    ds->reset_search();
//    delay(250);
//    return 1;
//  }
  return 0;
}

float MAX31850::getTemp()
{
  int i;
  byte data[12];
  float celsius, fahrenheit;
  byte present = 0;
  
  if ( !ds->search(addr)) {
//    Serial.println("No more addresses.");
//    Serial.println();
    ds->reset_search();
    delay(250);
//    return 1;
  }
//  Serial.print("ROM =");
//  for( i = 0; i < 8; i++) {
//    Serial.write(' ');
//    Serial.print(addr[i], HEX);
//  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return 0.0;
  }
//  Serial.println();
 
  ds->reset();
  ds->select(addr);
  ds->write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);

  present = ds->reset();
  ds->select(addr);    
  ds->write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds->read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
  int16_t raw = (data[1] << 8) | data[0];
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  return fahrenheit;
}

