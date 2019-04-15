#ifndef PERIODICTASK_H
#define PERIODICTASK_H

#include "Arduino.h"
#include <string>

typedef void (*fctPtr)(void *);

typedef enum {
    prioREALTIME = configMAX_PRIORITIES-1,
    prioVERY_HIGH = configMAX_PRIORITIES-2,
    prioHIGH = configMAX_PRIORITIES-3,
    prioNORMAL = configMAX_PRIORITIES-4,
    prioLOW = configMAX_PRIORITIES-5,
    prioVERY_LOW = configMAX_PRIORITIES-6,
    prioIDLE = tskIDLE_PRIORITY
} taskPriority;

class PeriodicTask {
    public:
        PeriodicTask( std::string name, void(*taskFct)(void*), uint32_t stackSize, UBaseType_t priority, uint32_t frequencyInMs, uint8_t core);
        void start();
        void delayWithFrequency();

        fctPtr getFunctionPtr();
        TickType_t getFrequency() { return freq; }
        TaskHandle_t getHandle() { return handle; }
        std::string getName() { return t_name; }
        PeriodicTask() = delete;
    private:
        TickType_t freq;
        fctPtr functionPtr;
        TaskHandle_t handle;
        std::string t_name;
};

#endif
