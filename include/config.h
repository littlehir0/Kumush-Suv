#pragma once
#include <Arduino.h>

#define SIGNAL_IN_PIN 13
#define BILL_EN_PIN 32
#define SIGNAL_OUT_PIN 21
#define MAINBOARD_EN_PIN 19

/*
#define MODEM_RST      5
#define MODEM_PWKEY    4
#define MODEM_POWER_ON 23
#define MODEM_TX       27
#define MODEM_RX       26
*/

#define SIGNAL_WEIGHT 500
#define CASH_REPORT_DELAY 1000
#define HEART_BEAT_DELAY 5000
#define STATE_REPORT_DELAY 1000

#define DEVICE_UID "water001"
#define LOGS 1

#define MQTT_SERVER "6.tcp.eu.ngrok.io"
#define MQTT_TOPIC "vending_machine_topic"
#define MQTT_PORT 20409