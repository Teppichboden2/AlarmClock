#include "Tasks.h"
#include "LCD.h"
#include "RTC.h"
#include "Tasks.h"
#include "PeriodicTask.h"
#include "Global_defines.h"

const int UP_BUTTON = 34;
const int DOWN_BUTTON = 35;
const int UTIL_BUTTON = 32;

extern TaskHandle_t isr_task_handle = NULL;
extern TaskHandle_t draw_task_handle = NULL;

void setup() {
  Serial.begin(MONITOR_SPEED);
  delay(500);

  LCD::init(14, 19, 15, 5, 17, 16, 4, 16, 2);
  RTC::init();

  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UTIL_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(UP_BUTTON),scrollup_ISR,RISING);
  attachInterrupt(digitalPinToInterrupt(DOWN_BUTTON),scrolldown_ISR,RISING);
  attachInterrupt(digitalPinToInterrupt(UTIL_BUTTON),util_ISR,RISING);

  PeriodicTask *measure = new PeriodicTask(MEASURE_TASK,measureTaskFnc,3000,prioNORMAL,1000,0);
  PeriodicTask *draw = new PeriodicTask(DRAW_TASK,drawTaskFnc,3000,prioNORMAL,500,1);
  PeriodicTask *interrupt = new PeriodicTask(ISR_TASK,interruptTaskFnc,3000,prioREALTIME,0,0);
  isr_task_handle = interrupt->getHandle();
  draw_task_handle = draw->getHandle();
  draw->start();
  measure->start();
  interrupt->start();

  Serial.printf("Code is running on core %d\n",xPortGetCoreID());
}

void loop() { 
  // Aufgabe für IDLE-Task?
  delay(5000);
}
