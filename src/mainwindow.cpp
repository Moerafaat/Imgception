#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QRegExpValidator>
#include "key.h"
#include "peerprogram.h"
#include "onlinepeers.h"

MainWindow::MainWindow(QWidget *parent) :
    ui(new Ui::MainWindow), QMainWindow(parent){
    ui->setupUi(this);
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::Text,QColor::fromRgb(144,170,255,255));
    ui->Username->setPalette(*palette);
    ui->Username->setValidator(new QRegExpValidator(QRegExp("(?:[a-z]|[A-Z]|_)*"),this));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_SignUP_btn_clicked(){
    if(ui->Username->text()=="")
        QMessageBox::critical(this, "Invalid Username","Please Enter a Username.");
    else if(!PeerProgram::signUp(ui->Username->text()))
        QMessageBox::critical(this, "Unable to sign up","Unable to sign up.");
}

void MainWindow::on_login_btn_clicked(){
    if(PeerProgram::login(true)){
        PeerProgram::PU.start();
        secDialog.setModal(true);
        this->close();
        secDialog.exec();
    }
    else
        QMessageBox::critical(this, "Unable to sign in","Make sure you are signed up first.");
}
