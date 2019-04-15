#include "RTC.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }

RTC* RTC::instance = nullptr;

RTC* RTC::getInstance() {

  
  return instance;
}

void RTC::init() {
  instance = new RTC();
  if(!instance->begin()) {
    delete instance;
    instance = nullptr;
  }
}

uint8_t RTC::getTemperatureHigh() const {
  return tempHigh;
}

uint8_t RTC::getTemperatureLow() const {
  return tempLow;
}

DateTime RTC::now() {
  Wire.beginTransmission(DS3231_ADDRESS);
    Wire.write((byte)0);  
    Wire.endTransmission();
  
    Wire.requestFrom(DS3231_ADDRESS, 0x12);
    uint8_t ss = bcd2bin(Wire.read() & 0x7F);
    uint8_t mm = bcd2bin(Wire.read());
    uint8_t hh = bcd2bin(Wire.read());
    Wire.read();  // Day of the week
    uint8_t d = bcd2bin(Wire.read());
    uint8_t m = bcd2bin(Wire.read());
    uint16_t y = bcd2bin(Wire.read()) + 2000;
    
    Wire.read();  // Alarm1 seconds
    Wire.read();  // Alarm1 minutes
    Wire.read();  // Alarm1 hours
    Wire.read();  // Alarm1 day / date
    Wire.read();  // Alarm2 minutes
    Wire.read();  // Alarm2 hours
    Wire.read();  // Alarm2 day / date
    Wire.read();  // Control register
    Wire.read();  // Status register
    Wire.read();  // Aging offset
    
    tempHigh = Wire.read();
    tempLow = ( (uint8_t) Wire.read()>>6) * 25;
    
    return DateTime (y, m, d, hh, mm, ss);
}
