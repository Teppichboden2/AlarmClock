#include "PeriodicTask.h"
#include "Global_defines.h"

extern "C" {
    static void taskFnc(void *para)
    {
        // para is "this" pointer of task's object!
        PeriodicTask *obj = reinterpret_cast<PeriodicTask *>(para);
        uint32_t notifyBits;
        if( xTaskNotifyWait(pdFALSE, ULONG_MAX, &notifyBits, portMAX_DELAY) == pdPASS ) {
            if( notifyBits & START_EVENT ) {
                TickType_t previousWake = xTaskGetTickCount();
                while(true) {
                    ( *(obj->getFunctionPtr()) )(nullptr);
                    vTaskDelayUntil(&previousWake, obj->getFrequency());
                }
            }
        }
    }
}

PeriodicTask::PeriodicTask( std::string name, void(*taskFct)(void*), uint32_t stackSize, UBaseType_t priority, uint32_t frequencyInMs, uint8_t core) 
    : t_name(name)
{
    functionPtr = taskFct;
    freq = frequencyInMs/portTICK_RATE_MS;
    xTaskCreatePinnedToCore(taskFnc, name.c_str(), stackSize, this, priority, &handle, core);
}

void PeriodicTask::start() {
    xTaskNotify(handle, START_EVENT, eSetBits);
}

fctPtr PeriodicTask::getFunctionPtr() {
    return functionPtr;
}
