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
#include "image.h"

Dialog::Dialog(QWidget *parent) :
    ui(new Ui::Dialog), QDialog(parent){

    // Setting up GUI
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
    ui->Edit_btn->setEnabled(false);
    QObject::connect(model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(on_TableViewItemEdit()));
}

Dialog::~Dialog(){
    try{
    PeerProgram::signOut();
    }catch(const char *err){
        qDebug() << "ERROR::" << err;
    }
    delete model;
    delete ui;
}

void Dialog::UpdateGUI(){
    UpdateTree();
}

void Dialog::UpdateTree(){
    ui->treeWidget->clear();
    TreeKey = PeerProgram::getAllKeys();
    ImageID.resize(TreeKey.size());
    for(int i = 0; i < TreeKey.size(); i++)
        AddRoot(i, TreeKey[i]);
}

void Dialog::AddRoot(int idx, QString ownerKey){
    QTreeWidgetItem *parent = new QTreeWidgetItem(ui->treeWidget);
    parent->setText(0, PeerProgram::getNameByKey(ownerKey));
    ui->treeWidget->addTopLevelItem(parent);

    ImageID[idx] = PeerProgram::getAllImageIDByOwner(ownerKey);
    for(int i=0;i<ImageID[idx].size();i++)
        AddChild(parent, ownerKey, ImageID[idx][i]);
}

void Dialog::AddChild(QTreeWidgetItem *parent, QString ownerKey, int imgID){
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, PeerProgram::getImgByID(ownerKey, imgID).getName());
    parent->addChild(itm);
}

void Dialog::on_treeWidget_doubleClicked(const QModelIndex &index){
    //Constraining the edit button to work for the "ME" images only.
    if((index.row()==0 && index.parent().row()==-1 )  || (index.parent().row()==0)  )
        ui->Edit_btn->setEnabled(true);
    else
        ui->Edit_btn->setEnabled(false);
    ui->tableView->setVisible(false);
    ui->AddUser_btn->setVisible(false);
    ui->Update_image_data->setVisible(false);
    ui->DeleteUser_btn->setVisible(false);
    ui->comboBox->setVisible(false);

    if(index.parent().row() != -1) ui->ImageViewer_label->setPixmap(QPixmap::fromImage(PeerProgram::getImgByID(TreeKey[index.parent().row()], ImageID[index.parent().row()][index.row()]).getImage()).scaled(ui->ImageViewer_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Dialog::on_New_btn_clicked(){
        NewUploadWindow2 = new NewUpload2(this);
        NewUploadWindow2->setModal(true);
        NewUploadWindow2->exec();
        delete NewUploadWindow2;
}

void Dialog::on_Edit_btn_clicked(){
    QModelIndex idx = ui->treeWidget->selectionModel()->selectedIndexes()[0];
    if(idx.parent().row() != 0) return;

    //Setting Up GUI
    ui->comboBox->clear();

    ComboBoxKeys = PeerProgram::getPeerKeys();
    ui->comboBox->addItems(QList<QString>::fromVector(PeerProgram::getVectorNameByKey(ComboBoxKeys)));
    model->clear();

    //Populate Model
    ui->tableView->setVisible(true);
    ui->AddUser_btn->setVisible(true);
    ui->Update_image_data->setVisible(true);
    ui->DeleteUser_btn->setVisible(true);
    ui->comboBox->setVisible(true);

    //Setting Up Table
    UserPanelKey = PeerProgram::getAuthorizedListByImageID(ImageID[0][idx.row()]);
    for(int i = 0; i < UserPanelKey.size(); i++)
    {
        QList<QStandardItem *> lst;
        lst.push_back(new QStandardItem(PeerProgram::getNameByKey(UserPanelKey[i])));
        lst.push_back(new QStandardItem(QString::number(PeerProgram::getAuthorizedLimit(UserPanelKey[i], ImageID[0][idx.row()]))));
        lst[0]->setEditable(false);
        model->appendRow(lst);
    }
}

void Dialog::on_Update_image_data_clicked()
{
    //GUI Stuff**************************************************
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
    for(int i = 0; i < model->rowCount(); i++)
        if(model->index(i, 0).data() == ui->comboBox->currentText())
            return;
    QList<QStandardItem *> lst;
    lst.push_back(new QStandardItem(ui->comboBox->currentText()));
    lst.push_back(new QStandardItem("0"));
    lst[0]->setEditable(false);
    model->appendRow(lst);
}

void Dialog::on_TableViewItemEdit()
{
 qDebug() << "item changed!!!";
}
