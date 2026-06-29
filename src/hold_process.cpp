#include <Arduino.h>
#include "hold_process.h"

void holdInit(){
    pinMode(MAINBOARD_EN_PIN, INPUT_PULLUP);
}

bool isHoldTriggered(){
    return digitalRead(MAINBOARD_EN_PIN) == HIGH;
}