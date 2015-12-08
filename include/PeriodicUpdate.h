#ifndef PERIODICUPDATE_H
#define PERIODICUPDATE_H

#include <QThread>
#include <QMutex>
class Application;
class PeriodicUpdate : public QThread{
public:
    PeriodicUpdate(Application *, const int = 15*60);
    void Exit();
    bool Lock();
    void Unlock();
private:
    Application *ptr;
    QMutex mutex;
    bool EXIT_FLAG;
    int seconds;
    void run();
};

#endif
