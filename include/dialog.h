#ifndef DIALOG_H
#define DIALOG_H

#include <QMainWindow>
#include <iostream>
#include<vector>
#include <QDialog>
#include <QtCore>
#include<QtGui>
#include<QTreeWidget>
#include "newupload2.h"
#include <QListWidgetItem>
#include <QVector>
#include <QString>
#include <QMap>
#include <cstdio>
#include "image.h"
#include "peer.h"
#include "update.h"
#include "key.h"
#include "Views.h"
#include<QVector>
#include<QPair>
#include<QString>

struct UserPanelEntry
{
    QString PublicKey;
    QString name;
    int ViewCount;
};


namespace Ui {
class Dialog;
}

class Dialog : public QDialog{
    Q_OBJECT

    void AddRoot(int, QString);
    void AddChild(QTreeWidgetItem *, QString, int);

    QVector<QString> TreeKey;
    QVector<QVector<int> > ImageID;
    QVector<QString> ComboBoxKeys;

    void UpdateTree();

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void UpdateGUI();

private slots:
    void on_treeWidget_doubleClicked(const QModelIndex &index);
    void on_New_btn_clicked();
    void on_Edit_btn_clicked();
    void on_Update_image_data_clicked();
    void on_DeleteUser_btn_clicked();
    void on_Refresh_btn_clicked();
    void on_logout_btn_clicked();
    void on_AddUser_btn_clicked();

private:
    Ui::Dialog *ui;
    NewUpload2  *NewUploadWindow2;
    QStandardItemModel *model;

    //--------------------------------------------------

    QVector<QString> getSharedPeerList(); // return names of people shared with me images
    QVector<Image> getPeerImages(QString peername ); // return all images sent before from a certain peer.
    QVector<Image> getMyImages(); // return all images of ME.
    QVector<QPair<QString ,QString >> getOnlinePeers(); //QVector<QPair<Publickey,onlinePeerName>>
    QVector<UserPanelEntry> UserPanelInfo();//QVector<UserPanelEntry>
    //void AddUser(QString, int,int); // void AddUser(Publickey,count,image ID);
    //void DeleteUser(QString, int,int); // void DeleteUser(Publickey,count,image ID);
    void UpdateUsersFromPanel(const QVector<QPair<QString, int>>&, const int&); // void UpdateUsers(Publickey,count,image ID);
    int count =10;// dumy counter
};

#endif // DIALOG_H
