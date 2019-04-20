#include <Arduino.h>
#include "Tasks.h"
#include "LCD.h"
#include "RTC.h"
#include "Global_defines.h"

// Indexes for Dennis
static int indexTop = 0;
static int indexBottom = 1;
static int currentArea = 0;
static int currentSize = 0;
static int state = IDLE;

// Buffers for DateTime Menu
char timeBuf[32];
char dateBuf[32];
char temperatureBuf[32];
char weekdayBuf[32];

// Buffers for alarm1 Menu
char alarm1Time[32];
char alarm1Mode[32] = "Daily???";

// Buffers for alarm2 Menu
char alarm2Time[32];
char alarm2Mode[32];

char *area0[] = {
    timeBuf,
    dateBuf,
    weekdayBuf,
    temperatureBuf
};

char *area1[] = {
    alarm1Time,
    alarm1Mode
};

char *area2[] = {
    alarm2Time,
    alarm2Mode
};

char *area3[] = {
  "Date/Time",
  "Alarm 1",
  "Alarm 2",
  "To the top",
  ""
};

static uint8_t a0size = sizeof(area0) / 4;
static uint8_t a1size = sizeof(area1) / 4;
static uint8_t a2size = sizeof(area2) / 4;
static uint8_t a3size = sizeof(area3) / 4;

char **areas[] = {
  area0,
  area1,
  area2,
  area3
};

uint8_t sizes[] = {
  sizeof(area0) / 4,
  sizeof(area1) / 4,
  sizeof(area2) / 4,
  sizeof(area3) / 4
};

static void switchArea(uint8_t area) {
  if(state == IDLE) {
    indexTop = 0;
    indexBottom = 1;
    
    currentArea = area;
    currentSize = sizes[area];
  }
}

static void renderRows() {
  // Data
  RTC *rtc = RTC::getInstance();
  sprintf(timeBuf, "%02d:%02d:%02d", Data.now.hour(), Data.now.minute(), Data.now.second());
  sprintf(dateBuf, "%02d/%02d/%04d", Data.now.day(), Data.now.month(), Data.now.year());
  sprintf(weekdayBuf,WEEKDAYS[Data.now.dayOfTheWeek()]); 
  sprintf(temperatureBuf,"%d,%d%s%c",rtc->getTemperatureHigh(),rtc->getTemperatureLow(),"\x05",'C');
  sprintf(alarm1Time,"A1:%02d:%02d %02d.",rtc->alarm1().hour(), rtc->alarm1().minute(), rtc->alarm1().day());
  sprintf(alarm2Time,"A2:%02d:%02d %02d.",rtc->alarm2().hour(), rtc->alarm2().minute(), rtc->alarm2().day());
}

static void printUI(LCD *lcd) {
  lcd->setCursor(13,0);
  lcd->write(Data.alarm1 ? "\x06" : " ");
  lcd->write( (currentArea == AREA_3) ? byte(ARRR) : byte(ARRL) );
  lcd->write(indexTop != 0 ? byte(ARRU) : byte(32));
  lcd->setCursor(13,1);
  lcd->write(Data.alarm2 ? "\x06 " : "  ");
  lcd->write(indexBottom != (currentSize-1) ? byte(ARRD) : byte(32) );
}

static void printRows() {
  LCD *lcd = LCD::getInstance();
  lcd->clear();  
  lcd->setCursor(0, 0);
  lcd->print(areas[currentArea][indexTop]);
  lcd->setCursor(0, 1);
  lcd->print(areas[currentArea][indexBottom]);
  printUI(lcd);
}

void drawTaskFnc(void *) {

  // Begin in area 0
  switchArea(AREA_0);
  Serial.println(sizeof(sizes));
  
  for(;;) {
    xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, portMAX_DELAY);
    renderRows(); // 0ms
    printRows();  // 10ms 
  }
}

void measureTaskFnc(void *) {
  uint8_t secondOld = 0;
  for(;;) {
    uint32_t notifyBits;
    xTaskNotifyWait(pdFALSE, ULONG_MAX, &notifyBits, portMAX_DELAY);

    if(notifyBits & MEASURE_EVENT) {
      Data.now = RTC::getInstance()->now();
      if(Data.now.second() != secondOld) {
        secondOld = Data.now.second();
        xTaskNotify(draw_task_handle,0,eNoAction);
      }
    }
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
      if(indexBottom != (currentSize-1)) {
        ++indexTop;
        ++indexBottom; 
      }
    }

    // Backscroll Button
    if(notifyBits & BACKSCROLL_EVENT) {
      switch(currentArea) {
        case AREA_0:
        case AREA_1:
        case AREA_2:
          switchArea(AREA_3);
          break;
        case AREA_3:
          switchArea(indexTop < sizeof(sizes) ? indexTop : sizeof(sizes));
          break;
        default:  ;
      }
    }

    xTaskNotify(draw_task_handle,0,eNoAction);  
  }
}

void notifyTaskFnc(void *) {
  xTaskNotify(measure_task_handle,MEASURE_EVENT,eSetBits);
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

void scrollback_ISR() {
  static unsigned long last = 0;
  unsigned long timer = millis();

  if(timer - last > DEBOUNCE) {
    xTaskNotifyFromISR(isr_task_handle,BACKSCROLL_EVENT,eSetBits,NULL);   
  }
  
  last = timer;
}
