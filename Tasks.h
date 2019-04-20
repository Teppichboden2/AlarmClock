#ifndef TASKS_H
#define TASKS_H

#include "RTC.h"

extern TaskHandle_t isr_task_handle;
extern TaskHandle_t draw_task_handle;
extern TaskHandle_t measure_task_handle;

const static char* WEEKDAYS[] {
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

void drawTaskFnc(void *);
void measureTaskFnc(void *);
void interruptTaskFnc(void *);
void notifyTaskFnc(void *);

void util_ISR();
void scrollup_ISR();
void scrolldown_ISR();
void scrollback_ISR();

#endif
