#include "config.h"
#include "bill_machine.h"
#include "signal_generator.h"
#include "internet_handler.h"

BillMachine bm(true);

void setup() {
  Serial.begin(115200);

  initAT();
  bm.init();
  initPulseGeneratorPin();
}

void loop() {
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
