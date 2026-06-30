#include "config.h"
#include "bill_machine.h"
#include "signal_generator.h"
#include "server_handler.h"
#include "cash_report_queue.h"
#include "hold_process.h"
#include <TinyGsmClient.h>
#include <PubSubClient.h>

BillMachine bm(true);
RepoQueue queue;

uint32_t lastCashReportAttempt = 0;
uint32_t lastStateReportAttempt = 0;

void setup() {
  Serial.begin(115200);
  
  Serial.print("Reset reason:");
  Serial.println(esp_reset_reason());

  bm.init();
  bm.disable();

  initPulseGeneratorPin();

  holdInit();

  serverInit();
  serverConnect();

  bm.enable();
}

void loop() {
  int num = 0;
  uint32_t now = millis();

  mqttLoop();
  bm.updateSignalState();
  sendHeartBeat();

  if(!isHoldTriggered()){
    // Signal came from BILL MACHINE
    if (bm.isNewSignalArrived()) {
      num = bm.getSignalCount();
      String temp = String(now);
      queue.enqueue(num * SIGNAL_WEIGHT, temp);
    }

    // Message came from SERVER
    if(isOnlinePaymentArrived()){
      num = getNewOnlinePayment() / SIGNAL_WEIGHT;
    }

    // Pulse Generation
    if(num){
      bm.disable();
      generatePulses(num);
      bm.enable();

      bm.resetCount();
      resetNewPayment();
    }

    // Retry report cash insertion, get data from queue
    if(now - lastCashReportAttempt >= CASH_REPORT_DELAY && !queue.isEmpty()){
      lastCashReportAttempt = now;
      reportCashInsertion(queue.getCash(), queue.getTransaction());
    }

    // Updade queue
    if(!queue.isEmpty() && (getCashAckedStatus() || getCashAckedTransaction() == queue.getTransaction())){
      queue.dequeue();
      resetCashAckedStatus();
    }

  }
  
  if(isHoldTriggered()){
    bm.disable();
    if(!isBusyAcked() && now - lastStateReportAttempt >= STATE_REPORT_DELAY){
      lastStateReportAttempt = now;
      sendBusyMsg(true);
      idleAcked = false;
    }      

  } else {
    if(!isIdleAcked() && now - lastStateReportAttempt >= STATE_REPORT_DELAY){
      lastStateReportAttempt = now;
      sendBusyMsg(false);
      busyAcked = false;
    }

    bm.enable();
  }
}
