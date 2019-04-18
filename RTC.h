#ifndef RTC_H
#define RTC_H

#include <RTClib.h>
#include <Wire.h>

class RTC : public RTC_DS3231 {
public:
  
  uint8_t getTemperatureHigh() const;
  uint8_t getTemperatureLow() const;
  DateTime now();   
  DateTime& alarm1() {return alarm_1;}
  DateTime& alarm2() {return alarm_2;}

  static void setAlarm1(uint8_t, uint8_t, uint8_t, uint8_t);
  static void setAlarm2(uint8_t, uint8_t, uint8_t);

  static void setAlarm(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

  static RTC* getInstance();
  static void init();

private:
  uint8_t tempHigh;
  uint8_t tempLow;

  DateTime alarm_1;
  DateTime alarm_2;

  static RTC *instance;
  RTC() : RTC_DS3231() {}
  ~RTC() {}

  RTC(RTC&) = delete;
};

#endif
