#include <Arduino.h>
#include "signal_generator.h"

void initPulseGeneratorPin(){
    pinMode(SIGNAL_OUT_PIN, OUTPUT);
}
void generatePulses(uint32_t count){
    Serial.print("[BILL_MACHINE]\tGenerating: ");
    Serial.println(count);

    for(int i = 0; i < count; i++){
        digitalWrite(SIGNAL_OUT_PIN, HIGH);
        delay(LOW_DURATION);
        digitalWrite(SIGNAL_OUT_PIN, LOW);
        delay(HIGH_DURATION);
    }
}