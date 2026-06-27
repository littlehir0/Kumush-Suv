#ifndef BILL_MACHINE_H
#define BILL_MACHINE_H

#define PULSE_WEIGHT            500
#define PULSE_DEBOUNCE_TIME     70
#define NEW_SIGNAL_THRESHOLD    500

class BillMachine{
    private:
    static bool isEn;
    static bool gotNewSignal;

    static uint32_t signalCount;
    static uint32_t lastSignalTime;

    public:
    BillMachine(bool en);

    void init();

    void updateSignalState();
    static void signalISR();

    void enable();
    void disable();

    bool isActive();

    bool isNewSignalArrived();
    uint32_t getSignalCount();
    void resetCount();
};

#endif