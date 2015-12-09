#include "uploadwindow.h"
#include "ui_uploadwindow.h"
#include<QLabel>
#include<QScrollArea>
#include<QImage>
//#include"Steganifier.h"

uploadWindow::uploadWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uploadWindow)
{
    //ui->setupUi(this);
    stgna= new Steganifier("/home/yehia/Git-Hub/Imgception-GUI-integration/","Steganify");
}

uploadWindow::~uploadWindow()
{
    delete ui;
}

void uploadWindow::on_BrowseButton_Original_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("All Image Files (*.png *jpg *jpeg *ppm);;PNG Images (*.png);;JPEG Images (*.jpeg *.jpg);;PPM Images (*.ppm)"));
    ui->OriginalImage_lineEdit->setText(fileName);
}

void uploadWindow::on_BrowseButton_fake_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("All Image Files (*.png *jpg *jpeg *ppm);;PNG Images (*.png);;JPEG Images (*.jpeg *.jpg);;PPM Images (*.ppm)"));
    ui->fakeImage_lineEdit->setText(fileName);
}

void uploadWindow::on_UploadButton_Original_clicked()
{
    QString fileName = ui->OriginalImage_lineEdit->text();
    QPixmap pix;
    bool loaded = pix.load(fileName);
    if(loaded == false)
    {
            ui->label->setText("Failed to load image" /*+ QDir::currentPath()*/ );
    }
    else
    {
        pix = pix.scaled(ui->Orignial_viewer->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->Orignial_viewer->setPixmap(pix);
    }
}

void uploadWindow::on_UploadButton_fake_clicked()
{
    QString fileName = ui->fakeImage_lineEdit->text();
    QPixmap pix;
    bool loaded = pix.load(fileName);
    if(loaded == false )
    {
            ui->label->setText("Failed to load image" /*+ QDir::currentPath()*/ );
    }
    else
    {
        pix = pix.scaled(ui->Orignial_viewer->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->Fake_viewer->setPixmap(pix);
    }

}

void uploadWindow::on_saveButton_clicked()
{
    //script path, the full path of the file Steganify.py
    //script name, Steganify (with no .py)

    QPixmap pix = QPixmap::fromImage(stgna->Steganify(ui->fakeImage_lineEdit->text(),ui->OriginalImage_lineEdit->text()));
    pix = pix.scaled(ui->Orignial_viewer->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->Orignial_viewer->setPixmap(pix);
}


