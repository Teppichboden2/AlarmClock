#ifndef H_GLOBAL_DEFINES
#define H_GLOBAL_DEFINES

#define MONITOR_SPEED 115200

#define DEBOUNCE 150

#define MEASURE_TASK "MEASURE"
#define DRAW_TASK "DRAW"
#define ISR_TASK "ISR"

#define START_EVENT 1<<0
#define UPSCROLL_EVENT 1<<1
#define DOWNSCROLL_EVENT 1<<2
#define UTIL_EVENT 1<<3
#define ALARM_EVENT 1<<4

#define DAILY 1
#define WEEKLY 2
#define ONCE 3

#define ALARM_1 1
#define ALARM_2 0

#endif
