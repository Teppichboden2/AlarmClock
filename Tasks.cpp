#include <Arduino.h>
#include "Tasks.h"
#include "LCD.h"
#include "RTC.h"
#include "Global_defines.h"

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
  bool alarm1;
  bool alarm2;

  DateTime now;
  
} Data;

static int indexTop = 0;
static int indexBottom = 1;

char clk[17];
char date[17];
char temperature[17];
char weekday[17];

char *order[] = {
    clk,
    date,
    weekday,
    temperature
};

static void renderRows() {
  // Data
  sprintf(clk, "%02d:%02d:%02d", Data.now.hour(), Data.now.minute(), Data.now.second());
  sprintf(date, "%02d/%02d/%04d", Data.now.day(), Data.now.month(), Data.now.year());
  sprintf(weekday,WEEKDAYS[Data.now.dayOfTheWeek()]); 
  sprintf(temperature,"%d,%d%s%c",RTC::getInstance()->getTemperatureHigh(),RTC::getInstance()->getTemperatureLow(),"\x03",'C');

  // UI
  sprintf(order[indexTop],"%-13s%1s %1s",order[indexTop],Data.alarm1?"\x04":" ");
  sprintf(order[indexBottom],"%-13s%1s %1s",order[indexBottom],Data.alarm2?"\x04":" ");
  order[indexTop][15] = indexTop!=0 ? ARRU : 0;
  order[indexBottom][15] = indexBottom!=(sizeof(order)/4-1) ? ARRD : 0;
}

static void printRows() {
  LCD *lcd = LCD::getInstance();
  lcd->clear();  
  lcd->setCursor(0, 0);
  lcd->print(order[indexTop]);
  lcd->setCursor(0, 1);
  lcd->print(order[indexBottom]);
}

void measureTaskFnc(void *) {
  static uint8_t secondOld = 0;
  Data.now = RTC::getInstance()->now();

  if(Data.now.second() != secondOld) {
    secondOld = Data.now.second();
    // Notify drawing task on core 1
    xTaskNotify(draw_task_handle,0,eNoAction);
  }
}

void drawTaskFnc(void *) {
  for(;;) {
    xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, portMAX_DELAY);
    Serial.println("Drawing");
    renderRows(); // 0ms
    printRows();  // 10ms 
  }
}

void interruptTaskFnc(void *) {
  Serial.println("Dies ist ein Test");
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
