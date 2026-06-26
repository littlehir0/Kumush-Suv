#include "config.h"
#include "bill_machine.h"
#include "signal_generator.h"
#include "internet_handler.h"
#include "mqtt_handler.h"

BillMachine bm(true);
MQTTHandler mqtt;

void setup() {
  Serial.begin(115200);

  initAT();
  mqtt.init();
  mqtt.connectMQTT();
  
  bm.init();
  initPulseGeneratorPin();
}

void loop() {

  mqtt.publishMQTT("HELLO");
  delay(5000);

  bm.updateSignalState();

  if(bm.isNewSignalArrived()){

    int num = bm.getSignalCount();
    Serial.print("Generating: ");
    Serial.println(num);

    bm.disable();
    generatePulses(num);
    bm.enable();
    
    bm.resetCount();
  }
}
