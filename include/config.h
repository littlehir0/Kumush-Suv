#pragma once

#include <Arduino.h>

#define SIGNAL_IN_PIN 33
#define BILL_EN_PIN 32
#define SIGNAL_OUT_PIN 18

#define DEVICE_UID "water002"
#define LOGS 1

#define MQTT_SERVER "4.tcp.eu.ngrok.io"
#define MQTT_TOPIC "vending_machine_topic"
#define MQTT_PORT 18914