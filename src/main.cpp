#include <iostream>
#include <QDir>
#include <QApplication>
#include "application.h"
#include "globals.h"

using namespace std;

int main(int argc, char *argv[]){
    try{
        QApplication a(argc, argv);
        Application w;
        w.show();
        return a.exec();
    }catch(const char *str){
        cout << str << endl;
    }
    return 0;
}
