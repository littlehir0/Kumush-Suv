#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include "../../../include/config.h"

extern TinyGsm modem;

class MQTTHandler{
    private:
    TinyGsmClient gsmClient;
    PubSubClient mqtt;

    public:
    MQTTHandler();
    void init();
    bool connectMQTT();
    bool publishMQTT(String msg);
};

#endif