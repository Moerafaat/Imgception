#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "peerprogram.h"

using namespace std;

int main(int argc, char *argv[]){
    PeerProgram::InitFolders();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
