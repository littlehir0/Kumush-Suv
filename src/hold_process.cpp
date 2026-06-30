#include <Arduino.h>
#include "hold_process.h"

void holdInit(){
    pinMode(MAINBOARD_EN_PIN, INPUT);
}

bool isHoldTriggered(){
    return digitalRead(MAINBOARD_EN_PIN) == 0;
}