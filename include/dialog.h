#ifndef DIALOG_H
#define DIALOG_H

#include <QMainWindow>
#include <iostream>
#include<vector>
#include <QDialog>
#include <QtCore>
#include<QtGui>
#include<QTreeWidget>
#include"uploadwindow.h"
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

namespace Ui {
class Dialog;
}

class Dialog : public QDialog{
    Q_OBJECT

    void AddRoot(QString name );
    void AddChild(QTreeWidgetItem *parent,QString name);

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

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
    uploadWindow *NewUploadWindow;
    QStandardItemModel *model;

    int count =10;// dumy counter
};

#endif // DIALOG_H
