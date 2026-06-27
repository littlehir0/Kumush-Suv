#include "config.h"
#include "bill_machine.h"
#include "signal_generator.h"
#include "server_handler.h"
#include <TinyGsmClient.h>
#include <PubSubClient.h>

BillMachine bm(true);

void setup() {
  Serial.begin(115200);

  serverInit();
  serverConnect();
  bm.init();
  initPulseGeneratorPin();
}

void loop() {
  delay(5000);

  uint32_t time = millis();
  publishStr(String(time).c_str());

  bm.updateSignalState();

  if (bm.isNewSignalArrived()) {
    int num = bm.getSignalCount();
    Serial.print("Generating: ");
    Serial.println(num);

    bm.disable();
    generatePulses(num);
    bm.enable();

    bm.resetCount();
  }
}
