#pragma once
#include "config.h"
#include <Arduino.h>

#define TINY_GSM_MODEM_SIM800

#define SerialAT Serial1

#define MODEM_RST      5
#define MODEM_PWKEY    4
#define MODEM_POWER_ON 23
#define MODEM_TX       27
#define MODEM_RX       26

#define APN "internet"
#define GPRS_USER ""
#define GPRS_PASS ""

enum SimOperator {
    UZMOBILE = 0,
    BEELINE,
    UCELL,
    MOBIUZ,
    UNKNOWN
};

extern String apn[4];
extern String balanceCode[4];

void serverInit();
void serverConnect();
void publishStr(const char* text);
SimOperator getOperator();

