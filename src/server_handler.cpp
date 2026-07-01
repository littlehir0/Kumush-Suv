#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
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
uint32_t lastServerConnect = 0;

bool IsNewPayment = false;
uint32_t newPaymentAmount = 0;
uint32_t lastHeartBeat = 0;
String cashAckedTransaction = "";
bool isCashRepoAcked = false;

bool busyAcked = false;
bool idleAcked = false;

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
  Serial.println("[MODEM]\tStarting modem...");
  modem.restart();

  Serial.print("\t|__ Modem Info: ");
  Serial.println(modem.getModemInfo());

  Serial.print("\t|__ SIM Status: ");
  Serial.println(modem.getSimStatus());


  Serial.println("[MODEM]\tConnecting to network...");
  if (!modem.waitForNetwork()) {
    Serial.println("[MODEM]\tNetwork failed!");
    //while (1);
  }

  Serial.println("[MODEM]\tNetwork OK");
  Serial.print("\t|__ Operator: ");
  Serial.println(modem.getOperator());

  Serial.print("\t|__ Signal: ");
  Serial.println(modem.getSignalQuality());

  Serial.print("\t|__ Time: ");
  Serial.println(modem.getGSMDateTime(DATE_FULL));

  modem.sendAT("+CBC");
  if (modem.waitResponse(5000L) == 1)
    Serial.println("\t|__ Battery query OK");

  modem.sendAT("+CCID");
  if (modem.waitResponse(5000L) == 1)
    Serial.println("\t|__ ICCID query OK");

  modem.sendAT("+CIMI");
  if (modem.waitResponse(5000L) == 1)
    Serial.println("\t|__ IMSI query OK");

  SimOperator op = getOperator();

  Serial.print("\t|__ Detected APN: ");
  Serial.println(apn[op]);

  Serial.print("\t|__ Internet: ");

  if(gsmClient.connect("google.com", 80)) {
    Serial.println("OK");
    gsmClient.stop();
  } else {
    Serial.println("CANNOT REACH");
  }

  Serial.println("[MODEM]\tConnecting to GPRS...");
  if (!modem.gprsConnect(apn[op].c_str(), GPRS_USER, GPRS_PASS)) {
    Serial.println("[MODEM]\tGPRS failed!");
    while (1);
  }
  Serial.println("[MODEM]\tGPRS connected!");

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(onMessage);

  Serial.println("[MODEM]\tConnecting to MQTT...");

  mqttReconnect();
}

void publishStr(const char* text, String purpose){
  if (mqtt.connected()) {

    mqtt.publish(MQTT_TOPIC, text);
    Serial.print("[MQTT]\t");
    Serial.println(purpose);

  } else {
    Serial.print("[MQTT]\tMQTT failed, rc=");
    Serial.println(mqtt.state());
  }
}

SimOperator getOperator() {
  String imsi = modem.getIMSI();
  imsi.trim();
  Serial.print("\t|__ IMSI: ");
  Serial.println(imsi);

  String code = imsi.substring(0, 5);

  if (code == "43408") return UZMOBILE;
  if (code == "43404") return BEELINE;
  if (code == "43405") return UCELL;
  if (code == "43407") return MOBIUZ;

  return UNKNOWN;
}

void onMessage(char* topic, byte* payload, unsigned int length){
  if(mqtt.connected()){
    String msg;
    for(uint32_t i=0; i < length; i++)
      msg += (char)payload[i];


    StaticJsonDocument<256> in;
    DeserializationError err = deserializeJson(in, msg);

    if(err){
      Serial.print("[JSON]\tParse failed: ");
      Serial.println(err.c_str());
      return;
    }

    String deviceUID = in["device_id"] | "";
    String transactionUID = in["transaction_id"] | "";
    String timestamp = in["timestamp"] | "";
    String status = in["status"] | "";
    uint32_t online_amount = in["online_amount"] | 0;
    bool busy = in["busy"] | false;

    // When ONLINE PAYMENT preformed
    if(online_amount > 0){
      Serial.println("[PYMNT]\tOnline payment received");
      Serial.print("\t|__ TransactionUID: ");
      Serial.println(transactionUID);
      Serial.print("\t|__ Timestamp: ");
      Serial.println(timestamp);
      Serial.print("\t|__ Amount: ");
      Serial.println(online_amount);

      //Triggers for billmachine
      IsNewPayment = true;
      newPaymentAmount = online_amount;

      String receiveTime = modem.getGSMDateTime(DATE_FULL);

      StaticJsonDocument<256> res;
      res["device_id"] = DEVICE_UID;
      res["transaction_id"] = transactionUID;
      res["timestamp"] = receiveTime;
      res["status"] = "online_accepted";

      char buf[256];
      serializeJson(res, buf);
      publishStr(buf, "Confirmation sent");

    // Confimation of cash insertion
    } else if(status == "cash_accepted"){
      Serial.print("[MQTT]\tCash confirmed: ");
      Serial.println(transactionUID);

      isCashRepoAcked = true;
      cashAckedTransaction = transactionUID;
    } else if(status == "busy_accepted"){
      if(busy){
        busyAcked = true;
        Serial.println("[MQTT]\tBusy accepted");
      } else {
        idleAcked = true;
        Serial.println("[MQTT]\tIdle accepted");
      }

    }
  }
  
}

bool isOnlinePaymentArrived(){
  return IsNewPayment;
}
uint32_t getNewOnlinePayment(){
  return newPaymentAmount;
}

void resetNewPayment(){
  IsNewPayment = false;
  newPaymentAmount = 0;
}

bool mqttReconnect(){
  if(!mqtt.connected() && millis() - lastServerConnect >= SERVER_RECONNECT_DELAY){

    Serial.println("[MQTT]\tReconnecting...");  
    lastServerConnect = millis();

    if (mqtt.connect(DEVICE_UID)) {
      Serial.println("[MQTT]\tConnected!");
      mqtt.subscribe(MQTT_TOPIC);
      Serial.print("[MQTT]\tSubscribed to: ");
      Serial.println(MQTT_TOPIC);
      return true;
    }

    Serial.print("[MQTT]\tFailed, rc=");
    Serial.println(mqtt.state());
    Serial.print("[MQTT]\tWill try to reconnect in ");
    Serial.print(SERVER_RECONNECT_DELAY/1000);
    Serial.println("s");

  }
  return false;
}

void mqttLoop() {
  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }

  mqttReconnect();
}

void reportCashInsertion(uint32_t cash, String trasID){
  if(mqtt.connected()){
    String receiveTime = modem.getGSMDateTime(DATE_FULL);
    StaticJsonDocument<256> out;

    out["device_id"] = DEVICE_UID;
    out["transaction_id"] = trasID;
    out["timestamp"] = receiveTime;
    out["cash_amount"] = cash;

    char buf[256];
    serializeJson(out, buf);
    publishStr(buf, "Cash Report sent!");

  }
  
}

void sendHeartBeat(){
  if(mqtt.connected()){
    uint32_t now = millis();
    if(now - lastHeartBeat >= HEART_BEAT_DELAY){
      lastHeartBeat = now;

      String tstmp = modem.getGSMDateTime(DATE_FULL);

      StaticJsonDocument<256> beat;
      beat["device_id"] = DEVICE_UID;
      beat["timestamp"] = tstmp;
      beat["heart_beat"] = "active";

      char buf[256];
      serializeJson(beat, buf);
      publishStr(buf, "Heart beat sent");
    }
  }
  
}

bool getCashAckedStatus(){  return isCashRepoAcked; }
String getCashAckedTransaction(){ return cashAckedTransaction; }
void resetCashAckedStatus(){
  isCashRepoAcked = false;
  cashAckedTransaction = "";
}

void sendBusyMsg(bool busy){
  if(mqtt.connected()){
    String tstmp = modem.getGSMDateTime(DATE_FULL);

    StaticJsonDocument<256> hold;
    hold["device_id"] = DEVICE_UID;
    hold["timestamp"] = tstmp;
    hold["busy"] = busy;

    char buf[256];
    serializeJson(hold, buf);
    String msg = "Busy message sent: ";

    if(busy){
      msg += "busy";
    } else {
      msg += "active";
    }

    publishStr(buf, msg);
  }
  
}

bool isBusyAcked(){
  return busyAcked;
}
bool isIdleAcked(){
  return idleAcked;
}