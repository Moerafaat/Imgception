#ifndef NEWUPLOAD2_H
#define NEWUPLOAD2_H

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QFileInfo>
#include "Steganifier.h"

namespace Ui {
class NewUpload2;
}

class NewUpload2 : public QDialog
{
    Q_OBJECT

public:
    explicit NewUpload2(QWidget *parent = 0);
    ~NewUpload2();

private slots:
    void on_BrowseButton_fake_clicked();

    void on_BrowseButton_Original_clicked();

    void on_UploadButton_Original_clicked();

    void on_UploadButton_fake_clicked();

    void on_saveButton_clicked();

private:
    Ui::NewUpload2 *ui;
    Steganifier *stgna;
};

#endif // NEWUPLOAD2_H
