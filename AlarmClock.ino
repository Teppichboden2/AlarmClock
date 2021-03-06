#include "Tasks.h"
#include "LCD.h"
#include "RTC.h"
#include "Tasks.h"
#include "PeriodicTask.h"
#include "Global_defines.h"

extern TaskHandle_t isr_task_handle = NULL;
extern TaskHandle_t draw_task_handle = NULL;
extern TaskHandle_t measure_task_handle = NULL;

const int UP_BUTTON = 34;
const int DOWN_BUTTON = 35;
const int UTIL_BUTTON = 25;
const int BACK_BUTTON = 32;

void setup() {
  Serial.begin(MONITOR_SPEED);
  delay(3);

  LCD::init(14, 19, 15, 5, 17, 16, 4, 16, 2);
  RTC::init();
  delay(3); // Stop tasks from running for a short period of time to prevent glitches with the LCD

  //RTC::getInstance()->setAlarm(ALARM_1,9,30,20,WEEKLY);
  //RTC::getInstance()->setAlarm(ALARM_2,10,30,19,DAILY);

  // Interrupts
  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UTIL_BUTTON, INPUT);
  pinMode(BACK_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(UP_BUTTON),scrollup_ISR,RISING);
  attachInterrupt(digitalPinToInterrupt(DOWN_BUTTON),scrolldown_ISR,RISING);
  attachInterrupt(digitalPinToInterrupt(UTIL_BUTTON),util_ISR,RISING);
  attachInterrupt(digitalPinToInterrupt(BACK_BUTTON),scrollback_ISR,RISING);

  // Tasks
  PeriodicTask *measure = new PeriodicTask(MEASURE_TASK,measureTaskFnc,3000,prioNORMAL,0,0);
  PeriodicTask *draw = new PeriodicTask(DRAW_TASK,drawTaskFnc,3000,prioNORMAL,0,0);
  PeriodicTask *interrupt = new PeriodicTask(ISR_TASK,interruptTaskFnc,3000,prioREALTIME,0,0);
  PeriodicTask *notify = new PeriodicTask(NOTIFY_TASK,notifyTaskFnc,3000,prioNORMAL,1000,0);

  
  isr_task_handle = interrupt->getHandle();
  draw_task_handle = draw->getHandle();
  measure_task_handle = measure->getHandle();
  
  draw->start();
  notify->start();
  measure->start();
  interrupt->start();

  Serial.printf("Code is running on core %d\n",xPortGetCoreID());
}

void loop() { 
  // Aufgabe für IDLE-Task?
  delay(5000);
}
