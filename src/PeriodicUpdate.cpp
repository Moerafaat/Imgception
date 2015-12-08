#include "PeriodicUpdate.h"
#include "application.h"
#include <QDebug>

PeriodicUpdate::PeriodicUpdate(Application *P, const int sec) : ptr(P), EXIT_FLAG(false), seconds(sec){

}

void PeriodicUpdate::Exit(){
    EXIT_FLAG = true;
    QThread::terminate();
}

void PeriodicUpdate::run(){
    while(!EXIT_FLAG){
        QThread::setTerminationEnabled(false);
        if(mutex.tryLock()){
            ptr->updatePeers();
            mutex.unlock();
        }
        QThread::setTerminationEnabled(true);
        QThread::sleep(seconds);
    }
}

bool PeriodicUpdate::Lock(){
    return mutex.tryLock();
}

void PeriodicUpdate::Unlock(){
    mutex.unlock();
}
