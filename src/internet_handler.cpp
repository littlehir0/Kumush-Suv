#define TINY_GSM_MODEM_SIM800

#include <Arduino.h>
#include <TinyGsmClient.h>
#include "internet_handler.h"

String apn[4] = {
    "internet.uzmobile.uz",
    "internet.beeline.uz",
    "internet",
    "net.mobiuz.uz"
};

String balanceCode[4]={
    "*100*1#",
    "*102#",
    "*100#",
    "*100#"
};

void sendAT(String command, String comment, uint32_t timeout){
    if(LOGS){
        Serial.print("----------| AT Command: ");
        Serial.print(command);
        Serial.print(" | " + comment);
        Serial.println(" |----------");
        SerialAT.println(command);
    }
    

    uint32_t start = millis();
    while(millis() - start <= timeout){
        while(SerialAT.available()){
            if(LOGS)
                Serial.write(SerialAT.read());
            start = millis();
        }
        if(LOGS)
            Serial.print("\t");
    }
    if(LOGS)
        Serial.println();
}

void initAT(){
    Serial.println("\n[MODEM]\tModem diagnostics starting ...");
    //Powering SIM800L
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_POWER_ON, HIGH);

    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);

    pinMode(MODEM_PWKEY, OUTPUT);
    digitalWrite(MODEM_PWKEY, LOW);
    delay(1000);

    digitalWrite(MODEM_PWKEY, HIGH);
    delay(3000);
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);

    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);

    // Is modem alive?
    sendAT("AT", "Is alive");

    // Firmware version
    sendAT("ATI", "Firmware version");

    // Is SIM inserted?
    sendAT("AT+CPIN?", "Is sim inserted?");

    // Signal strength
    sendAT("AT+CSQ", "Signal strength");

    // Registered on network?
    sendAT("AT+CREG?", "Registred on network?");

    // Which operator?
    //sendAT("AT+COPS?");
    int op = getOperator();
    Serial.print("|-----------> APN:");
    Serial.println(apn[op]);
    
    // Attached to packet data?
    sendAT("AT+CGATT?", "Attached to packet data?");

    // Set APN
    sendAT("AT+CSTT=\"" + apn[op] + "\",\"\",\"\"", "Set APN");

    // Check Balance
    sendAT("AT+CUSD=1,\"" + balanceCode[op] + "\"","Check Balance", 10000);

    // Start GPRS
    sendAT("AT+CIICR","Start GPRS", 10000);

    // Get IP address
    sendAT("AT+CIFSR", "Get IP address");

    // Check DNS
    sendAT("AT+CDNSGIP=\"google.com\"", "Check DNS", 10000);

    Serial.println();
    Serial.println("[MODEM]\tSIM800L diagnostics finished ... ");
}

SimOperator getOperator()
{
    SerialAT.println("AT+COPS?");
    //sendAT("AT+COPS?");
    delay(1000);

    String op = SerialAT.readString();

    Serial.println(op);

    op.toLowerCase();

    if (op.indexOf("uzmobile") >= 0)
        return UZMOBILE;

    if (op.indexOf("beeline") >= 0)
        return BEELINE;

    if (op.indexOf("ucell") >= 0 || op.indexOf("coscom") >= 0)
        return UCELL;

    if (op.indexOf("mobi") >= 0 || op.indexOf("uzdunrobita") >= 0)
        return MOBIUZ;

    return UNKNOWN;
}