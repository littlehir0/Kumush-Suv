#include "../../../include/config.h"
#include "cash_report_queue.h"

RepoQueue::RepoQueue() : front(0), tail(-1), total(0){}

bool RepoQueue::isFull(){
    return total == QUEUE_CAPASITY;
}

bool RepoQueue::isEmpty(){
    return total == 0;
}

bool RepoQueue::enqueue(uint32_t amount, String trans){
    if(isFull()){
        Serial.println("[QUEUE]\tQueue is full");
        return false;
    }

    tail = (tail + 1) % QUEUE_CAPASITY;
    transaction[tail] = trans;
    cash[tail] = amount;
    total++;

    return true;
}
bool RepoQueue::dequeue(){
    if(isEmpty()){
        Serial.println("[QUEUE]\tQueue is emptry");
        return false;
    }
    
    front = (front + 1) % QUEUE_CAPASITY;
    total--;
    
    if (total == 0) {
        front = 0;
        tail = -1;
    }

    return true;
}

String RepoQueue::getTransaction(){
    if(isEmpty()){
        return "";
    }
    return transaction[front];
}

uint32_t RepoQueue::getCash(){
    if(isEmpty()){
        return 0;
    }
    return cash[front];
}