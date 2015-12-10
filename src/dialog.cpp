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


    //Seeting Up the Tree


    AddRoot("Me");
    QVector<QString>TreeMembers=getSharedPeerList();
    for(int i=0;i<TreeMembers.size();i++)
            AddRoot(TreeMembers[i]);


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


    if(name!="Me")
       {
            QVector<Image>peerImages= getPeerImages(name);
            for(int i=0;i<peerImages.size();i++)
                 AddChild(itm,peerImages[i].image_name);
       }
    else
       {
         QVector<Image>MyImages=getMyImages();
         for(int i=0;i<MyImages.size();i++)
                          AddChild(itm,MyImages[i].image_name);

       }

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

void Dialog::on_Edit_btn_clicked()
{

    //Setting Up GUI
    ui->comboBox->clear();

    //does this has the online peer list ?
    ui->comboBox->addItems(PeerProgram::GetPeerNames());
    model->clear();

    //Populate Model
    ui->tableView->setVisible(true);
    ui->AddUser_btn->setVisible(true);
    ui->Update_image_data->setVisible(true);
    ui->DeleteUser_btn->setVisible(true);
    ui->comboBox->setVisible(true);

    //Setting Up Table
    QVector<UserPanelEntry> Entries = UserPanelInfo();//QVector<UserPanelEntry>
    for(int i = 0; i < Entries.size(); i++)
    {
        QList<QStandardItem *> lst;
        lst.push_back(new QStandardItem(Entries[i].name));
        lst.push_back(new QStandardItem(QString::number(Entries[i].ViewCount)));
        lst[0]->setEditable(false);
        model->appendRow(lst);
    }

}

void Dialog::on_Update_image_data_clicked()
{
    //STUB stuff

    int ImageSelected=3; //automate this ?
    QVector<QPair<QString, int>> ToStub;
    for(int i = 0; i < model->rowCount(); i++)
       {
            QPair<QString, int> x;
            x.first=model->index(i, 0).data().toString();
            x.second=model->index(i, 1).data().toInt();
            ToStub.push_back(x);
       }
    UpdateUsersFromPanel(ToStub,ImageSelected);

    //GUI Stuff**************************************************
    ui->tableView->setVisible(false);
    ui->AddUser_btn->setVisible(false);
    ui->Update_image_data->setVisible(false);
    ui->DeleteUser_btn->setVisible(false);
    ui->comboBox->setVisible(false);



}

void Dialog::on_DeleteUser_btn_clicked()
{
    //QItemSelection selection = ui->tableView->selectionModel()->selection().indexes();
    for(const QModelIndex& idx : ui->tableView->selectionModel()->selection().indexes())
        model->removeRow(idx.row());
    return;
}

void Dialog::on_Refresh_btn_clicked()
{
    PeerProgram::updatePeers();
    //GetNotifications  ***************************************************************
    //Update GUI
}

void Dialog::on_logout_btn_clicked(){
    this->close();
}

void Dialog::on_AddUser_btn_clicked()
{
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

QVector<QString> Dialog::getSharedPeerList()
{
    // return names of people shared with me images
    //peerprogram.h
    QVector<QString> sharedpeers;
    sharedpeers.push_back("Rafaat");
    sharedpeers.push_back("Don Omar");
    sharedpeers.push_back("Eman");
    sharedpeers.push_back("Raouf");
    sharedpeers.push_back("Omar Nasr");
    sharedpeers.push_back("Yehia");

    return sharedpeers;

}

QVector<Image> Dialog::getPeerImages(QString peername )
{
    // return all images sent before from a certain peer.

     //peerprogram.h
    QVector<Image> dummy;
    //dummy


        // Id, Key OwnerKey,Path,Name,UpCount,ViewLimit
        Image dummy_DATA1(3, Key(),":/APPImages/Images/11126921_10153134401805295_408836533708166894_o.jpg",
                    "11126921_10153134401805295_408836533708166894_o.jpg", 0, -1);
        dummy.push_back(dummy_DATA1);

        Image dummy_DATA2(4, Key(), ":/APPImages/Images/amazing_sky_2-wallpaper-2880x1620.jpg",
                    "amazing_sky_2-wallpaper-2880x1620.jpg", 0, -1);
        dummy.push_back(dummy_DATA2);

        return  dummy;

}

QVector<Image> Dialog::getMyImages()
{// return all images of ME.

    //peerprogram.h
    QVector<Image> dummy;
    //dummy


        // Id, Key OwnerKey,Path,Name,UpCount,ViewLimit
        Image dummy_DATA1(1, Key(),":/APPImages/Images/Iron-Man-The-Avengers-2012.jpg",
                    "Iron-Man-The-Avengers-2012", 0, -1);
        dummy.push_back(dummy_DATA1);

        Image dummy_DATA2(2, Key(), ":/APPImages/Images/captain_america_civil_war-wallpaper-1920x1080.jpg",
                    "captain_america_civil_war-wallpaper-1920x1080.jpg", 0, -1);
        dummy.push_back(dummy_DATA2);

        return  dummy;

}

QVector<QPair<QString ,QString >> Dialog::getOnlinePeers()
{ //QVector<QPair<Publickey,onlinePeerName>>

    //peerprogram.h

    QVector<QPair<QString ,QString >> dummy;
    QPair<QString ,QString > dummy_data;

    //dummy
    dummy_data.first=1;
    dummy_data.second="Rafaat";
    dummy.push_back(dummy_data);
    dummy_data.first=2;
    dummy_data.second="Don Omar";
    dummy.push_back(dummy_data);
    dummy_data.first=3;
    dummy_data.second="Eman";
    dummy.push_back(dummy_data);
    return  dummy;

}
QVector<UserPanelEntry> Dialog:: UserPanelInfo()
{
    //pearProgram.h
    //dummy
    QVector<UserPanelEntry> dummy;
    UserPanelEntry dummy_data;
    for(int i=0;i<3;i++)
    {
        dummy_data.name="Peer"+QString::number(i);
        dummy_data.PublicKey=QString::number(i*i);
        dummy_data.ViewCount=i*20;
        dummy.push_back(dummy_data);
    }

    return dummy;
}

void Dialog::UpdateUsersFromPanel(const QVector<QPair<QString, int>> &PeerInfo,const int& imageID)
{// void UpdateUsers(QVector<QPair<QString, int>> ,image ID);

    //peerprogram.h
    //to update internally the structure
    //showing dummy data
    qDebug()<<imageID;
    for(int i = 0; i < PeerInfo.size(); i++)
       {
            qDebug()<<PeerInfo[i].first;
            qDebug()<<PeerInfo[i].second;
       }
}




