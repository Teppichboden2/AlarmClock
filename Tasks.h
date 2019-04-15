#ifndef TASKS_H
#define TASKS_H

extern TaskHandle_t isr_task_handle;
extern TaskHandle_t draw_task_handle;

void drawTaskFnc(void *);
void measureTaskFnc(void *);
void interruptTaskFnc(void *);

void util_ISR();
void scrollup_ISR();
void scrolldown_ISR();

#endif
