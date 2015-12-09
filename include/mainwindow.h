#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_SignUP_btn_clicked();
    void on_login_btn_clicked();

private:
    Ui::MainWindow *ui;
    Dialog secDialog;
};

#endif // MAINWINDOW_H
