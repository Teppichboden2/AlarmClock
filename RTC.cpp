#include "RTC.h"
#include "Global_defines.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

static void printBin(uint8_t b) {
  for(int i=7; i>=0; --i) {
    Serial.printf( (b>>i) & 1 ? "1" : "0" );
  }
  Serial.println("");
}

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

void RTC::setAlarm(uint8_t alarm, uint8_t h, uint8_t m, uint8_t d, uint8_t mode) {
  byte reg = (alarm) ? 0x07 : 0x0b;

  uint8_t hh = bin2bcd(h);
  uint8_t mm = bin2bcd(m);
  uint8_t dd = bin2bcd(d);

  switch(mode) {
    case DAILY:
      dd |= (1 << 7); 
      break;
    case WEEKLY:
      dd |= (1 << 6);
      break;
    case ONCE:
      break;
    default:
      break;
  }

  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(reg);
  if(alarm) Wire.write( 0 );
  Wire.write( mm );
  Wire.write( hh );
  Wire.write( dd );
  Wire.endTransmission();
}

DateTime RTC::now() {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write((byte)0);  
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ADDRESS, 0x12);
  uint8_t ss = bcd2bin(Wire.read());            // Seconds
  uint8_t mm = bcd2bin(Wire.read());            // Minutes
  uint8_t hh = bcd2bin(Wire.read());            // Hours
  Wire.read();                                  // Day of the week
  uint8_t d = bcd2bin(Wire.read());             // Day / date
  uint8_t m = bcd2bin(Wire.read());             // Month
  uint16_t y = bcd2bin(Wire.read()) + 2000;     // Year
  uint8_t a1s = bcd2bin(Wire.read());           // Alarm1 seconds
  uint8_t a1m = bcd2bin(Wire.read());           // Alarm1 minutes
  uint8_t a1h = bcd2bin(Wire.read());           // Alarm1 hours
  uint8_t a1d = bcd2bin(Wire.read() & ~0xC0 );  // Alarm1 day / date
  uint8_t a2m = bcd2bin(Wire.read());           // Alarm2 minutes
  uint8_t a2h = bcd2bin(Wire.read());           // Alarm2 hours
  uint8_t a2d = bcd2bin(Wire.read() & ~0xC0 );  // Alarm2 day / date
  Wire.read();                                  // Control register
  Wire.read();                                  // Status register
  Wire.read();                                  // Aging offset
  uint8_t th = Wire.read();                     // MSB of temp
  uint8_t tl = ((uint8_t)Wire.read()>>6) * 25;  // LSF of temp

  alarm_1 = DateTime(y,m,a1d,a1h,a1m,a1s);
  alarm_2 = DateTime(y,m,a2d,a2h,a2m,0);
  tempHigh = th;
  tempLow = tl;
  
  return DateTime (y, m, d, hh, mm, ss);
}
