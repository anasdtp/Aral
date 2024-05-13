#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class TIMEMS
{
public:
    TIMEMS(/* args */);

    static void begin(); 

    static void start();
    static void stop();
    static void reset();
    static unsigned long read();
private:

    static void timer_task(void *parameter);
};

