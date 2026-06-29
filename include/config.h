#pragma once
#include <Arduino.h>

#define SIGNAL_IN_PIN 13
#define BILL_EN_PIN 32
#define SIGNAL_OUT_PIN 21

#define SIGNAL_WEIGHT 500
#define CASH_REPORT_DELAY 1000
#define HEART_BEAT_DELAY 5000

#define DEVICE_UID "water001"
#define LOGS 1

#define MQTT_SERVER "6.tcp.eu.ngrok.io"
#define MQTT_TOPIC "vending_machine_topic"
#define MQTT_PORT 20409