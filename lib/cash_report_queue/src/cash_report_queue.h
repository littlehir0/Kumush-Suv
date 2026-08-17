#ifndef CASH_REPORT_QUEUE_H
#define CASH_REPORT_QUEUE_H
#include <Arduino.h>

#define QUEUE_CAPASITY 15

class RepoQueue{  
    private:
    String transaction[QUEUE_CAPASITY];
    uint32_t cash[QUEUE_CAPASITY];
    int16_t front;
    int16_t tail;
    int16_t total;

    public:
    RepoQueue();

    bool isEmpty();
    bool isFull();

    bool enqueue(uint32_t amount, String trans);
    bool dequeue();

    String getTransaction();
    uint32_t getCash();
};

#endif