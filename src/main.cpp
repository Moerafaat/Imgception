#include <QDir>
#include <QApplication>
#include "application.h"
#include "globals.h"

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    Application w;
    w.show();

    return a.exec();
}
