#include "../../../include/config.h"
#include "bill_machine.h"

bool BillMachine::gotNewSignal = false;
bool BillMachine::isEn = true;
uint32_t BillMachine::signalCount = 0;
uint32_t BillMachine::lastSignalTime = 0;

// Constructors

BillMachine::BillMachine(bool en){
    isEn = en;
}

// Initializatiors and Handlers
void BillMachine::init(){
    pinMode(SIGNAL_IN_PIN, INPUT);
    pinMode(BILL_EN_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(SIGNAL_IN_PIN), BillMachine::signalISR, FALLING);

    if(LOGS){
        Serial.println("\n[BILL_MACHINE] Bill Machine starting ...");
    }
}

void BillMachine::updateSignalState(){
    if(millis() - lastSignalTime >= NEW_SIGNAL_THRESHOLD && !gotNewSignal && signalCount != 0){
        gotNewSignal = true;
    }
}

void IRAM_ATTR BillMachine::signalISR(){
    
    uint32_t now = millis();
    if(now - lastSignalTime >= PULSE_DEBOUNCE_TIME){
        signalCount++;
        lastSignalTime = now;
    }
}

// Control methods
void BillMachine::enable(){
    if(!isEn){
        digitalWrite(BILL_EN_PIN, LOW);
        isEn = true;
        if(LOGS){
            Serial.println("[BILL_MACHINE] Bill Machine ENABLED");
        }
    }
}
void BillMachine::disable(){
    if(isEn){
        digitalWrite(BILL_EN_PIN, HIGH);
        isEn = false;
        if(LOGS){
            Serial.println("[BILL_MACHINE] Bill Machine DISABLED");
        }
    }
}

bool BillMachine::isActive(){
    return isEn;
}
bool BillMachine::isNewSignalArrived(){
    return gotNewSignal;
}
uint32_t BillMachine::getSignalCount(){
    return signalCount;
}
void BillMachine::resetCount(){
    signalCount = 0;
    gotNewSignal = false;
}