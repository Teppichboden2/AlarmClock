#ifndef TASKS_H
#define TASKS_H

void drawTaskFnc(void *);
void measureTaskFnc(void *);
void interruptTaskFnc(void *);

void util_ISR();
void scrollup_ISR();
void scrolldown_ISR();

#endif
