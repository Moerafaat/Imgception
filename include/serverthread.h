#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H
#include <QThread>
class Application;

class ServerThread : public QThread
{
public:
    ServerThread(Application *);
private:
    Application *ptr;
    void run();
};

#endif // SERVERTHREAD_H
