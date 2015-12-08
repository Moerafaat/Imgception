#include <QDebug>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "application.h"
#include "ui_application.h"
#include "key.h"
#include "globals.h"

const QString Application::application_root = "/Imgception";

Application::Application(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Application){
    ui->setupUi(this);

    // Creating core application folders.
    QDir tDir(Globals::ApplicationRoot);
    if(!tDir.exists()) tDir.mkpath(".");
    tDir.setPath(Globals::MeFolderPath);
    if(!tDir.exists()) tDir.mkpath(".");
    tDir.setPath(Globals::TempFolderPath);
    if(!tDir.exists()) tDir.mkpath(".");
}

Application::~Application(){
    delete ui;
}

// ------------------------------GUI Functions------------------------------------- //
// GUI.
void Application::logMessage(QString log){
    ui->lbl_log->setText("Log: " + log);
}
// -------------------------------------------------------------------------------- //

// ------------------------------Peer Program Functions---------------------------- //
// Stub entry.
bool Application::login(bool offline_mode){
    QString info_path = Globals::MeFolderPath + "info.txt";
    QString pub_key_path = Globals::MeFolderPath + "pubkey.txt";
    QString pri_key_path = Globals::MeFolderPath + "prikey.txt";
    QString owner_images_path = Globals::MeFolderPath + "images.txt";

    QFileInfo check_info(info_path);
    QFileInfo check_pub_key_path(pub_key_path);
    QFileInfo check_pri_key_path(pri_key_path);
    QFileInfo check_owner_images_path(owner_images_path);
    if(!check_info.exists() || !check_info.isFile() ||
            !check_pub_key_path.exists() || !check_pub_key_path.isFile() ||
            !check_pri_key_path.exists() || !check_pri_key_path.isFile() ||
            !check_owner_images_path.exists() || !check_owner_images_path.isFile()){
        logMessage("Unable to log in. Missing files.");
        return false;
    }

    QFile info_file(info_path);
    QFile owner_images_file(owner_images_path);
    if(info_file.open(QIODevice::ReadOnly) && owner_images_file.open(QIODevice::ReadOnly)){ // Successful open file.
        QTextStream info_stream(&info_file);
        info_stream >> my_name;
        info_stream >> next_image_ID;

        if(!my_public_key.readFromFile(pub_key_path, true) || !my_private_key.readFromFile(pri_key_path, true)){
            logMessage("Unable to log in. Error reading keys.");
        }

       /*QTextStream owner_images_stream(&owner_images_file);
        int i = 0;
        while(!owner_images_stream.atEnd()){
            QString line = owner_images_stream.readLine();
            QStringList image_info = line.split(" ");
            int image_ID = image_info.at(0).toInt();
            int owner_key =image_info.at(1).toInt();
            QString path = image_info.at(2);
            QString image_name = image_info.at(3);
            int up_count = image_info.at(4).toInt();
            int view_limit = image_info.at(5).toInt();
            Image image(image_ID, owner_key, path, image_name, up_count, view_limit); // TODO: owner_key.
            my_images.push_back(image); // Insert image into vector of owner images.
            image_key_to_index[image_ID] = i;
            i++;
        }

        if(offline_mode){ // Fetch images from the local database.

        }
        else{ // Fetch images remotely + local database.
            updatePeers();
        }*/

        logMessage("Sign in successful.");
        return true;
    }
    else{
        logMessage("Error during sign in.");
        return false;
    }
}

// Local invocation.
bool Application::signUp(QString Username){

    QString info_path = Globals::MeFolderPath + "info.txt";
    QFileInfo check_file(info_path);
    if(check_file.exists()){ // Already signed up.
        logMessage("You already signed up.");
        return false;
    }

    // First time to sign up.
    QString pub_key_path = Globals::MeFolderPath + "pubkey.txt";
    QString pri_key_path = Globals::MeFolderPath + "prikey.txt";
    QString owner_images_path = Globals::MeFolderPath + "images.txt";
    QFile info_file(info_path);
    QFile owner_images_file(owner_images_path);

    if(!info_file.open(QIODevice::ReadWrite)){
        logMessage("Error during sign up");
        return false;
    }
    if(!owner_images_file.open(QIODevice::ReadWrite)){
        info_file.close();
        logMessage("Error during sign up");
        return false;
    }

    QTextStream out_stream(&info_file);
    out_stream << Username + " 0" << endl;
    info_file.close();

    owner_images_file.close(); // Empty owner images file.

    //Generate keys.
    if(!Key::generate(my_public_key, my_private_key) || !my_public_key.writeToFile(pub_key_path) || !my_private_key.writeToFile(pri_key_path))
    {
        logMessage("Unable to generate keys");
        //Should delete fils on failure
        return false;
    }

    logMessage("Sign up successful.");
    return true;
}

// Stub entry.
void Application::logout(){

}

// Stub entry.
void Application::updatePeers(){

}

// Stub entry.
pair< vector<Image>, vector<Update> > Application::getNotifications(){

}

// Local invocation.
QVector<Image> Application::getAllImages(){

}

// Local invocation.
QVector<Image> Application::getImagesByOwner(int OwnerKey){

}

// Local invocation.
Image Application::getImageByID(int OwnerKey, int ImageId){

}

// Local invocation.
QString Application::createImage(QString RealPath, QString FakePath){

}

// Local invocation.
void Application::deleteImage(int OwnerKey, int ImageID){

}

// Stub entry.
void Application::notifyPeer(int OwnerKey, int ImageId, const Update &ImageUpdate){

}

// Getter.
int Application::getOwnKey(){

}

// Getter.
QString Application::getOwnName(){
    return my_name;
}
// -------------------------------------------------------------------------------- //

// ------------------------------Slots--------------------------------------------- //
void Application::on_btn_sign_in_clicked(){
    login(true);
}

void Application::on_btn_sign_up_clicked(){
    QString username = ui->le_username->text();
    if(username.size() == 0){
        logMessage("Please enter username.");
        return;
    }
    if(signUp(username)) // Successful signup.
        login(true);
}

void Application::on_btn_sign_out_clicked(){

}

void Application::on_btn_browse_real_clicked(){

}

void Application::on_btn_browse_fake_clicked(){

}

void Application::on_btn_new_image_clicked(){

}

void Application::on_btn_refresh_clicked(){

}

void Application::on_btn_edit_clicked(){

}

void Application::on_btn_delete_clicked()
{

}

void Application::on_btn_add_user_clicked()
{

}

void Application::on_lst_limit_itemChanged(QListWidgetItem *item)
{

}
