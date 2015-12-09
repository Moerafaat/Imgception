#include "newupload2.h"
#include "ui_newupload2.h"

NewUpload2::NewUpload2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewUpload2)
{
    ui->setupUi(this);
    stgna= new Steganifier("/home/yehia/Git-Hub/Imgception-GUI-integration/","Steganify");
}

NewUpload2::~NewUpload2()
{
    delete ui;
}

void NewUpload2::on_BrowseButton_fake_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("All Image Files (*.png *jpg *jpeg *ppm);;PNG Images (*.png);;JPEG Images (*.jpeg *.jpg);;PPM Images (*.ppm)"));
    ui->fakeImage_lineEdit->setText(fileName);
}

void NewUpload2::on_BrowseButton_Original_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "", tr("All Image Files (*.png *jpg *jpeg *ppm);;PNG Images (*.png);;JPEG Images (*.jpeg *.jpg);;PPM Images (*.ppm)"));
    ui->OriginalImage_lineEdit->setText(fileName);
}

void NewUpload2::on_UploadButton_Original_clicked()
{
    QString fileName = ui->OriginalImage_lineEdit->text();
    QPixmap pix;
    bool loaded = pix.load(fileName);
    if(loaded == false)
    {
            ui->Orignial_viewer->setText("Failed to load image" /*+ QDir::currentPath()*/ );

    }
    else
    {
        pix = pix.scaled(ui->Orignial_viewer->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->Orignial_viewer->setPixmap(pix);
    }
}

void NewUpload2::on_UploadButton_fake_clicked()
{
    QString fileName = ui->fakeImage_lineEdit->text();
    QPixmap pix;
    bool loaded = pix.load(fileName);
    if(loaded == false )
    {
            ui->Fake_viewer->setText("Failed to load image" /*+ QDir::currentPath()*/ );
    }
    else
    {
        pix = pix.scaled(ui->Orignial_viewer->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->Fake_viewer->setPixmap(pix);
    }

}

void NewUpload2::on_saveButton_clicked()
{
        //script path, the full path of the file Steganify.py
        //script name, Steganify (with no .py)

        QPixmap pix = QPixmap::fromImage(stgna->Steganify(ui->fakeImage_lineEdit->text(),ui->OriginalImage_lineEdit->text()));
        pix = pix.scaled(ui->Orignial_viewer->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->Orignial_viewer->setPixmap(pix);
}
