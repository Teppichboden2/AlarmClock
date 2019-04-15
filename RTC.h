#ifndef RTC_H
#define RTC_H

#include <RTClib.h>
#include <Wire.h>

class RTC : public RTC_DS3231 {
public:
  
  uint8_t getTemperatureHigh() const;
  uint8_t getTemperatureLow() const;
  DateTime now();   

  static RTC* getInstance();
  static void init();

private:
  uint8_t tempHigh;
  uint8_t tempLow;

  static RTC *instance;
  RTC() : RTC_DS3231() {}
  ~RTC() {}

  RTC(RTC&) = delete;
  RTC(RTC&&) = delete;
};

#endif
