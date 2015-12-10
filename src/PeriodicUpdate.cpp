#include "PeriodicUpdate.h"
#include "peerprogram.h"
#include <QDebug>

PeriodicUpdate::PeriodicUpdate(const int sec) : EXIT_FLAG(false), seconds(sec){

}

void PeriodicUpdate::Exit(){
    EXIT_FLAG = true;
    QThread::terminate();
}

void PeriodicUpdate::run(){
    QThread::sleep(5);
    while(!EXIT_FLAG){
        QThread::setTerminationEnabled(false);
        if(mutex.tryLock()){
            PeerProgram::updatePeers();
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
 int x;
