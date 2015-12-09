#include <QGraphicsScene>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QStringRef>
#include <QFileDialog>
#include <QtCore>
#include <QtGui>
#include <QDebug>
#include <QList>
#include "peerprogram.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "mainwindow.h"
#include "key.h"
#include "onlinepeers.h"

Dialog::Dialog(QWidget *parent) :
    ui(new Ui::Dialog), QDialog(parent){
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(1);

    model = new QStandardItemModel(0,2,this);
    model->setHorizontalHeaderItem(0,new QStandardItem(QString("Peers")));
    model->setHorizontalHeaderItem(1,new QStandardItem(QString("View Limit")));
    ui->tableView->setModel(model);

    ui->tableView->setVisible(false);
    ui->AddUser_btn->setVisible(false);
    ui->Update_image_data->setVisible(false);
    ui->DeleteUser_btn->setVisible(false);
    ui->comboBox->setVisible(false);

    //ui->Edit_btn->setEnabled(false);
}

Dialog::~Dialog(){
    PeerProgram::Exit();
    delete model;
    delete ui;
}

void Dialog::AddRoot(QString name){
    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget);
    itm->setText(0,name);
    ui->treeWidget->addTopLevelItem(itm);
}

void Dialog::AddChild(QTreeWidgetItem *parent,QString name){
    QTreeWidgetItem *itm = new QTreeWidgetItem ();
    itm->setText(0,name);
    parent->addChild(itm);
}

void Dialog::on_treeWidget_doubleClicked(const QModelIndex &index){
    //Constraining the edit button to work for the "ME" images only.
    if((index.row()==0 && index.parent().row()==-1 )  || (index.parent().row()==0)  )
        ui->Edit_btn->setEnabled(true);
    else
    {
        ui->Edit_btn->setEnabled(false);
        ui->tableView->setVisible(false);
        ui->AddUser_btn->setVisible(false);
        ui->Update_image_data->setVisible(false);
        ui->DeleteUser_btn->setVisible(false);
        ui->comboBox->setVisible(false);
    }

    if(index.parent().row() != -1){
        QPixmap pix;
        //PeerProgram::Peers[Parent.row][row].getImage().   *************************************
        bool loaded = pix.load(":/APPImages/Images/"+index.data().toString());
        if(loaded == false)
            QMessageBox::critical(this, "Loading Failed","Unable to load image");
        else{
            pix = pix.scaled(ui->ImageViewer_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->ImageViewer_label->setPixmap(pix);
            ui->CounterViewer_label->setText("Image view count: " + QString::number(count));    //Count Getter
        }
    }
}

void Dialog::on_New_btn_clicked(){
        NewUploadWindow2 = new NewUpload2(this);
        NewUploadWindow2->setModal(true);
        NewUploadWindow2->exec();
        delete NewUploadWindow2;
}

void Dialog::on_Edit_btn_clicked(){
    ui->comboBox->clear();
    ui->comboBox->addItems(PeerProgram::GetPeerNames());
    model->clear();
    //Populate Model
    ui->tableView->setVisible(true);
    ui->AddUser_btn->setVisible(true);
    ui->Update_image_data->setVisible(true);
    ui->DeleteUser_btn->setVisible(true);
    ui->comboBox->setVisible(true);
}

void Dialog::on_Update_image_data_clicked(){
    //update other Stuff**************************************************
    ui->tableView->setVisible(false);
    ui->AddUser_btn->setVisible(false);
    ui->Update_image_data->setVisible(false);
    ui->DeleteUser_btn->setVisible(false);
    ui->comboBox->setVisible(false);
}

void Dialog::on_DeleteUser_btn_clicked(){
    //QItemSelection selection = ui->tableView->selectionModel()->selection().indexes();
    for(const QModelIndex& idx : ui->tableView->selectionModel()->selection().indexes())
        model->removeRow(idx.row());
    return;
}

void Dialog::on_Refresh_btn_clicked(){
    PeerProgram::updatePeers();
    //GetNotifications  ***************************************************************
    //Update GUI
}

void Dialog::on_logout_btn_clicked(){
    this->close();
}

void Dialog::on_AddUser_btn_clicked(){
    //NEED TO VALIDATE OR LIMIT PEER NAMES***************************************************************************
    for(int i = 0; i < model->rowCount(); i++)
        if(model->index(i, 0).data() == ui->comboBox->currentText())
            return;
    QList<QStandardItem *> lst;
    lst.push_back(new QStandardItem(ui->comboBox->currentText()));
    lst.push_back(new QStandardItem("0"));
    lst[0]->setEditable(false);
    model->appendRow(lst);
}
