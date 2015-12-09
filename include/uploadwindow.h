#ifndef UPLOADWINDOW_H
#define UPLOADWINDOW_H

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include "Steganifier.h"

namespace Ui {
class uploadWindow;
}

class uploadWindow : public QDialog
{
    Q_OBJECT

public:
    explicit uploadWindow(QWidget *parent = 0);
    ~uploadWindow();

private slots:
    void on_BrowseButton_fake_clicked();

    void on_UploadButton_fake_clicked();

    void on_saveButton_clicked();

    void on_BrowseButton_Original_clicked();

    void on_UploadButton_Original_clicked();

private:
    Ui::uploadWindow *ui;
    Steganifier *stgna;
};

#endif // UPLOADWINDOW_H
