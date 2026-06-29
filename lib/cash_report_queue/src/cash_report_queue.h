#ifndef CASH_REPORT_QUEUE_H
#define CASH_REPORT_QUEUE_H
#include <Arduino.h>

#define QUEUE_CAPASITY 10

class RepoQueue{
    private:
    String transaction[QUEUE_CAPASITY];
    uint32_t cash[QUEUE_CAPASITY];
    uint16_t front;
    uint16_t tail;
    uint16_t total;

    public:
    RepoQueue();

    bool isEmpty();
    bool isFull();

    bool enqueue(int amount, String trans);
    bool dequeue();

    String getTransaction();
    uint32_t getCash();
};

#endif