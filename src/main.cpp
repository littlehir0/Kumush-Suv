#include "config.h"
#include "bill_machine.h"
#include "signal_generator.h"
#include "server_handler.h"
#include "cash_report_queue.h"
#include <TinyGsmClient.h>
#include <PubSubClient.h>

BillMachine bm(true);
RepoQueue queue;

uint32_t lastReportAttempt = 0;

void setup() {
  Serial.begin(115200);

  serverInit();
  serverConnect();
  bm.init();
  initPulseGeneratorPin();
}

void loop() {
  int num = 0;
  int now = millis();
  mqttLoop();
  bm.updateSignalState();
  sendHeartBeat();

  if (bm.isNewSignalArrived()) {
    num = bm.getSignalCount();

    String temp = String(now);
    //queue.enqueue(num * SIGNAL_WEIGHT, temp);
  }
  if(isOnlinePaymentArrived()){
    num = getNewOnlinePayment() / SIGNAL_WEIGHT;
  }
  if(num){
    bm.disable();
    generatePulses(num);
    bm.enable();

    bm.resetCount();
    resetNewPayment();
  }

  /*if(now - lastReportAttempt >= CASH_REPORT_DELAY && !queue.isEmpty()){
    reportCashInsertion(queue.getCash(), queue.getTransaction());
  }
  if(getAckedStatus() || getAckedTransaction() == queue.getTransaction()){
    queue.dequeue();
    resetAckedStatus();
  }*/
}
