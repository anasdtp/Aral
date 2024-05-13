#include "TIMEMS.h"

static bool START;
static unsigned long timeMs;
static unsigned long start_time;

TIMEMS::TIMEMS(/* args */)
{
    START = false; start_time = 0; timeMs = 0;
}

void TIMEMS::start(){
    START = true;
    start_time = millis();
}

void TIMEMS::stop(){
    START = false;
    start_time = millis();
}

void TIMEMS::reset(){
    start_time = millis();
}

unsigned long TIMEMS::read(){
    return timeMs;
}

void TIMEMS::begin(){
    // Créez une tâche pour incrémenter uscount sur le deuxième cœur (CORE_1)
  xTaskCreatePinnedToCore(timer_task, "timer_task", 2048, NULL, 1, NULL, 1);
} 

void TIMEMS::timer_task(void *pvParameter){
    while(1){
        if(START){
            // Serial.println(timeMs);
            timeMs = (millis() - start_time);
        }
    }
}

