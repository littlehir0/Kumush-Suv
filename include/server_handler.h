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

extern bool IsNewPayment;
extern uint32_t newPaymentAmount;

bool isOnlinePaymentArrived();
uint32_t getNewOnlinePayment();
void resetNewPayment();

void serverInit();
void serverConnect();
void publishStr(const char* text, String purpose = "");
SimOperator getOperator();

bool mqttReconnect();
void onMessage(char* topic, byte* payload, unsigned int length);
void mqttLoop();

extern bool isRepoAcked;
extern String ackedTransaction;

bool getAckedStatus();
String getAckedTransaction();
void resetAckedStatus();

void reportCashInsertion(uint32_t cash, String trasID);

extern uint32_t lastHeartBeat;
void sendHeartBeat();

