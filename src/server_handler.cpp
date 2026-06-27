#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include "server_handler.h"

TinyGsm modem(SerialAT);
TinyGsmClient gsmClient(modem);
PubSubClient mqtt(gsmClient);

String apn[4] = {
    "internet.uzmobile.uz",
    "internet.beeline.uz",
    "internet",
    "net.mobiuz.uz"
};

String balanceCode[4] = {
    "*100*1#",
    "*102#",
    "*100#",
    "*100#"
};

void serverInit(){
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_POWER_ON, HIGH);

    pinMode(MODEM_PWKEY, OUTPUT);
    digitalWrite(MODEM_PWKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWKEY, HIGH);

    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);

}
void serverConnect(){
    Serial.println("\n[MODEM]\tStarting modem...");
    modem.restart();

    Serial.println("[MODEM]\tConnecting to network...");
    if (!modem.waitForNetwork()) {
      Serial.println("[MODEM]\tNetwork failed!");
      while (1);
    }

    Serial.println("[MODEM]\tConnecting to GPRS...");
    if (!modem.gprsConnect(APN, GPRS_USER, GPRS_PASS)) {
      Serial.println("[MODEM]\tGPRS failed!");
      while (1);
    }
    Serial.println("[MODEM]\tGPRS connected!");

    mqtt.setServer(MQTT_SERVER, MQTT_PORT);

    Serial.println("[MODEM]\tConnecting to MQTT...");

    if (mqtt.connect(DEVICE_UID)) {
      Serial.println("[MODEM]\tMQTT connected!");
    } else {
      Serial.print("[MODEM]\tMQTT failed, rc=");
      Serial.println(mqtt.state());
    }

}
void publishStr(const char* text){
    if (mqtt.connect(DEVICE_UID)) {

      mqtt.publish(MQTT_TOPIC, text);
      Serial.println("[MODEM]\tMessage sent.");

    } else {
      Serial.print("[MODEM]\tMQTT failed, rc=");
      Serial.println(mqtt.state());
    }

}

SimOperator getOperator() {
    String imsi = modem.getIMSI();
    imsi.trim();
    Serial.print("[MODEM]\tIMSI: ");
    Serial.println(imsi);

    String code = imsi.substring(0, 5);

    if (code == "43408") return UZMOBILE;
    if (code == "43404") return BEELINE;
    if (code == "43405") return UCELL;
    if (code == "43407") return MOBIUZ;

    return UNKNOWN;
}