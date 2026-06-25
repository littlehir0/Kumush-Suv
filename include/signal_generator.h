#pragma once
#include "config.h"

#define HIGH_DURATION 100
#define LOW_DURATION 50

void initPulseGeneratorPin();
void generatePulses(uint32_t count);