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
    QVector<QString> UserPanelKey;

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
    void on_TableViewItemEdit();

private:
    Ui::Dialog *ui;
    NewUpload2  *NewUploadWindow2;
    QStandardItemModel *model;
};

#endif // DIALOG_H
