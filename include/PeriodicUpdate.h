#ifndef PERIODICUPDATE_H
#define PERIODICUPDATE_H

#include <QThread>
#include <QMutex>

class PeriodicUpdate : public QThread{
public:
    PeriodicUpdate(const int = 15*60);
    void Exit();
    bool Lock();
    void Unlock();
private:
    QMutex mutex;
    bool EXIT_FLAG;
    int seconds;
    void run();
};

#endif
