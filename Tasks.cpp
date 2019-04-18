#include <Arduino.h>
#include "Tasks.h"
#include "LCD.h"
#include "RTC.h"
#include "Global_defines.h"
#include <string>

extern TaskHandle_t isr_task_handle;
extern TaskHandle_t draw_task_handle;

const char* WEEKDAYS[] {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

static struct {
  DateTime now;
  DateTime alarm_1;
  DateTime alarm_2;

  uint8_t alarm1_mode;
  uint8_t alarm2_mode;

  bool alarm1;
  bool alarm2;
} Data;

static int indexTop = 0;
static int indexBottom = 1;

char clk[32];
char date[32];
char temperature[32];
char weekday[32];
char a1[32];
char a2[32];

char *order[] = {
    clk,
    date,
    weekday,
    a1,
    a2,
    temperature
};

static void renderRows() {
  // Data
  RTC *rtc = RTC::getInstance();
  sprintf(clk, "%02d:%02d:%02d", Data.now.hour(), Data.now.minute(), Data.now.second());
  sprintf(date, "%02d/%02d/%04d", Data.now.day(), Data.now.month(), Data.now.year());
  sprintf(weekday,WEEKDAYS[Data.now.dayOfTheWeek()]); 
  sprintf(temperature,"%d,%d%s%c",rtc->getTemperatureHigh(),rtc->getTemperatureLow(),"\x05",'C');
  sprintf(a1,"A1:%02d:%02d %02d.",rtc->alarm1().hour(), rtc->alarm1().minute(), rtc->alarm1().day());
  sprintf(a2,"A2:%02d:%02d %02d.",rtc->alarm2().hour(), rtc->alarm2().minute(), rtc->alarm2().day());
}

static void printUI(LCD *lcd) {
  lcd->setCursor(13,0);
  lcd->print(Data.alarm1 ? "\x06 " : "  ");
  lcd->write(indexTop != 0 ? byte(ARRU) : byte(32));
  lcd->setCursor(13,1);
  lcd->print(Data.alarm2 ? "\x06 " : "  ");
  lcd->write(indexBottom != (sizeof(order)/4-1) ? byte(ARRD) : byte(32) );
}

static void printRows() {
  LCD *lcd = LCD::getInstance();
  lcd->clear();  
  lcd->setCursor(0, 0);
  lcd->print(order[indexTop]);
  lcd->setCursor(0, 1);
  lcd->print(order[indexBottom]);
  printUI(lcd);
}

void drawTaskFnc(void *) {
  for(;;) {
    xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, portMAX_DELAY);
    renderRows(); // 0ms
    printRows();  // 10ms 
  }
}

void measureTaskFnc(void *) {
  static uint8_t secondOld = 0;
  Data.now = RTC::getInstance()->now();

  if(Data.now.second() != secondOld) {
    secondOld = Data.now.second();
    //Serial.printf("Alarm 1: %02d:%02d Uhr am %02d.\n",RTC::getInstance()->alarm1().hour(),RTC::getInstance()->alarm1().minute(),RTC::getInstance()->alarm1().day());
    //Serial.printf("Alarm 2: %02d:%02d Uhr am %02d.\n\n",RTC::getInstance()->alarm2().hour(),RTC::getInstance()->alarm2().minute(),RTC::getInstance()->alarm2().day());
    // Notify drawing task on core 1
    xTaskNotify(draw_task_handle,0,eNoAction);
  }
}

void interruptTaskFnc(void *) {
  for(;;) {
    uint32_t notifyBits;
    xTaskNotifyWait(pdFALSE, ULONG_MAX, &notifyBits, portMAX_DELAY);

    // Util Button
    if(notifyBits & UTIL_EVENT) {
      if      (Data.alarm1 && Data.alarm2)    Data.alarm1 = false;
      else if (!Data.alarm1 && Data.alarm2)   Data.alarm2 = false; 
      else if (!Data.alarm1 && !Data.alarm2)  Data.alarm1 = true;
      else                                    Data.alarm2 = true;
    }

    // Upscroll Button
    if(notifyBits & UPSCROLL_EVENT) {
      if(indexTop != 0) {
        --indexTop;
        --indexBottom;
      }
    }

    // Downscroll Button
    if(notifyBits & DOWNSCROLL_EVENT) {
      if(indexBottom != sizeof(order)/4 -1) {
        ++indexTop;
        ++indexBottom; 
      }
    }

    // Notify drawing task on core 1
    xTaskNotify(draw_task_handle,0,eNoAction);  
  }
}

void util_ISR() {
  static unsigned long last = 0;
  unsigned long timer = millis();

  if(timer - last > DEBOUNCE) {
    xTaskNotifyFromISR(isr_task_handle,UTIL_EVENT,eSetBits,NULL);
  }
  
  last = timer;
}

void scrollup_ISR() {
  static unsigned long last = 0;
  unsigned long timer = millis();

  if(timer - last > DEBOUNCE) {
    xTaskNotifyFromISR(isr_task_handle,UPSCROLL_EVENT,eSetBits,NULL);
  }
  
  last = timer;
}

void scrolldown_ISR() {
  static unsigned long last = 0;
  unsigned long timer = millis();

  if(timer - last > DEBOUNCE) {
    xTaskNotifyFromISR(isr_task_handle,DOWNSCROLL_EVENT,eSetBits,NULL);
  }
  
  last = timer;
}
