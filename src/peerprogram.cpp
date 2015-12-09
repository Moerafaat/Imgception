#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "peerprogram.h"
#include "onlinepeers.h"

const QString PeerProgram::ApplicationRoot = QDir::homePath() + "/Imgception/";
const QString PeerProgram::MeFolderPath = PeerProgram::ApplicationRoot + "me/";
const QString PeerProgram::TempFolderPath = PeerProgram::ApplicationRoot + ".temp/";

void PeerProgram::InitFolders(){
    QDir tDir(PeerProgram::ApplicationRoot);
    if(!tDir.exists()) tDir.mkpath(".");
    tDir.setPath(PeerProgram::MeFolderPath);
    if(!tDir.exists()) tDir.mkpath(".");
    tDir.setPath(PeerProgram::TempFolderPath);
    if(!tDir.exists()) tDir.mkpath(".");
}

// Stub entry.
bool PeerProgram::login(bool offline_mode){
    QString info_path = MeFolderPath + "info.txt";
    QString pub_key_path = MeFolderPath + "pubkey.txt";
    QString pri_key_path = MeFolderPath + "prikey.txt";
    QString owner_images_path = MeFolderPath + "images.txt";

    QFileInfo check_info(info_path);
    QFileInfo check_pub_key_path(pub_key_path);
    QFileInfo check_pri_key_path(pri_key_path);
    QFileInfo check_owner_images_path(owner_images_path);
    if(!check_info.exists() || !check_info.isFile() ||
            !check_pub_key_path.exists() || !check_pub_key_path.isFile() ||
            !check_pri_key_path.exists() || !check_pri_key_path.isFile() ||
            !check_owner_images_path.exists() || !check_owner_images_path.isFile()){
        qDebug() << "Unable to log in. Missing files.";
        return false;
    }

    QFile info_file(info_path);
    QFile owner_images_file(owner_images_path);
    if(info_file.open(QIODevice::ReadOnly) && owner_images_file.open(QIODevice::ReadOnly)){ // Successful open file.
        QTextStream info_stream(&info_file);
        info_stream >> my_name;
        info_stream >> next_image_ID;

        if(!my_public_key.readFromFile(pub_key_path) || !my_private_key.readFromFile(pri_key_path, true)){
            qDebug() << "Unable to log in. Error reading keys.";
            return false;
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

        qDebug() << "Sign in successful.";
        return true;
    }
    else{
        qDebug() << "Error during sign in.";
        return false;
    }
}

// Local invocation.
bool PeerProgram::signUp(QString Username){

    QString info_path = MeFolderPath + "info.txt";
    QFileInfo check_file(info_path);
    if(check_file.exists()){ // Already signed up.
        qDebug() << "You already signed up.";
        return false;
    }

    // First time to sign up.
    QString pub_key_path = MeFolderPath + "pubkey.txt";
    QString pri_key_path = MeFolderPath + "prikey.txt";
    QString owner_images_path = MeFolderPath + "images.txt";
    QFile info_file(info_path);
    QFile owner_images_file(owner_images_path);

    if(!info_file.open(QIODevice::ReadWrite)){
        qDebug() << "Error during sign up";
        return false;
    }
    if(!owner_images_file.open(QIODevice::ReadWrite)){
        info_file.close();
        qDebug() << "Error during sign up";
        return false;
    }

    QTextStream out_stream(&info_file);
    out_stream << Username + " 0" << endl;
    info_file.close();

    owner_images_file.close(); // Empty owner images file.

    //Generate keys.
    if(!Key::generate(my_public_key, my_private_key) || !my_public_key.writeToFile(pub_key_path) || !my_private_key.writeToFile(pri_key_path))
    {
        qDebug() << "Unable to generate keys";
        //Should delete fils on failure
        return false;
    }

    qDebug() << "Sign up successful.";
    return true;
}

// Stub entry.
bool PeerProgram::updatePeers(){
    if(!my_public_key.isReady()){
        qDebug() << "You need to sign in first!";
        return false;
    }
    ServerMessage msg(P2S_UPDATE_PEER_LIST);
    msg.setPayload(my_public_key.getAsString().toStdString().c_str(), my_public_key.getAsString().size());
    if(!Client.connect(msg, 1000)){
        qDebug() << "Unable to connect to worker.";
        return false;
    }

    onlinePeers online;
    if(!Client.recieveObject(&online)){
        qDebug() << "Unable to recieve object";
        return false;
    }

    //Actually update

    Client.disconnect(); // Disconnect from worker.
    qDebug() << "Refresh done!";
    return true;
}


ServerView PeerProgram::Server(4000);
ClientView PeerProgram::Client("10.40.55.97", 5000);
//PeriodicUpdate PeerProgram::PU(10);
//ServerThread PeerProgram::ST;

Key PeerProgram::my_public_key; // Fetch from pubkey file.
Key PeerProgram::my_private_key; // Fetch from prikey file.
QString PeerProgram::my_name; // Fetch from info file.
int PeerProgram::next_image_ID; // Fetch from info file.
