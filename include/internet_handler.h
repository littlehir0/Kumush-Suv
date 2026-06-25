#pragma once
#include "config.h"
#include <Arduino.h>

#define SerialAT Serial2

#define MODEM_PWKEY 4
#define MODEM_RST 5
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26

#define CMD_TIMEOUT 3000



enum SimOperator {
    UZMOBILE = 0,
    BEELINE,
    UCELL,
    MOBIUZ,
    UNKNOWN
};

extern String apn[4];
extern String balanceCode[4];

void sendAT(String command, String comment, uint32_t timeout = CMD_TIMEOUT);
void initAT();
SimOperator getOperator();
