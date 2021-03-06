#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QFileInfoList>
#include <QCryptographicHash>
#include "peerprogram.h"
#include "onlinepeers.h"

using namespace std;

const QString PeerProgram::ApplicationRoot = QDir::homePath() + "/Imgception/";
const QString PeerProgram::MeFolderPath = PeerProgram::ApplicationRoot + "__me__/";
const QString PeerProgram::TempFolderPath = PeerProgram::ApplicationRoot + ".temp/";
const QString PeerProgram::UpdateCache = PeerProgram::TempFolderPath + "__UpdateCache__";

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
    QString info_path = MeFolderPath + "info";
    QString pub_key_path = MeFolderPath + "pubkey";
    QString pri_key_path = MeFolderPath + "prikey";
    QString owner_images_path = MeFolderPath + "images";

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

    QFile file;
    file.setFileName(info_path);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream fin(&file);
        fin >> my_name;
        fin >> next_image_ID;
        file.close();
    }
    else{
        qDebug() << "Unable to read my info.";
        return false;
    }

    if(!my_public_key.readFromFile(pub_key_path) || !my_private_key.readFromFile(pri_key_path, true)){
        qDebug() << "Unable to log in. Error reading keys.";
        return false;
    }

    file.setFileName(owner_images_path);
    if(file.open(QIODevice::ReadOnly)){ // Successful open file.
        QTextStream fin(&file);
        int nImg; fin >> nImg;
        for(int i = 0; i < nImg; i++){
            int imgID; fin >> imgID;
            QString imgName; fin >> imgName;
            int nShares; fin >> nShares;
            own_images.push_back(Image(imgID, my_public_key, PeerProgram::MeFolderPath + QString::number(imgID) + ".png", imgName, 0, -1));
            own_img_key_to_index.insert(imgID, own_images.size() - 1);
            authorized_peers.push_back(QMap<QString, int>());
            for(int j = 0; j < nShares; j++){
                fin.skipWhiteSpace();
                QString peer_key = fin.read(Key::PubKeySize);
                int vlimit; fin >> vlimit;
                authorized_peers.back()[peer_key] = vlimit;
            }
        }
        file.close();
    }
    else{
        qDebug() << "Unable to read my images.";
        return false;
    }

    QDir application_directory(PeerProgram::ApplicationRoot);
    QFileInfoList folder_info_list = application_directory.entryInfoList();
    QStringList folder_name_list = application_directory.entryList();
    for(int i=0; i<folder_info_list.size(); i++){
        if(!folder_info_list[i].isDir() || folder_name_list[i] == "." || folder_name_list[i] == ".." || folder_name_list[i] == "__me__" || folder_name_list[i] == ".temp") continue; // Not a directory, me, or temp.

        Key key(PeerProgram::ApplicationRoot + folder_name_list[i] + "/pubkey");
        peer_key_to_index.insert(key.getAsString(), peer_list.size());

        file.setFileName(PeerProgram::ApplicationRoot + folder_name_list[i] + "/info");
        if(!file.open(QIODevice::ReadOnly)){
            qDebug() << "Unable to open peer info file";
            return false;
        }
        QTextStream fin(&file);
        QString peer_name = folder_name_list[i];
        peer_list.push_back(Peer(key, peer_name, false, -1, -1));

        int nImg; fin >> nImg;
        for(int j = 0; j < nImg; j++){
            int imgID;  fin >> imgID;
            QString imgName; fin >> imgName;
            int upCnt;  fin >> upCnt;
            int vLimit; fin >> vLimit;
            peer_list.back().image_list.push_back(Image(imgID, key, PeerProgram::ApplicationRoot + folder_name_list[i] + "/" + QString::number(imgID) + ".png", imgName, upCnt, vLimit));
            peer_list.back().image_key_to_index.insert(imgID, peer_list.back().image_list.size() - 1);
        }
        file.close();
    }

    Server.setCallbackFunc(P2P_SEND_IMAGE, HandelNewImage);
    PU.start();
    ST.start();
    qDebug() << "Sign in successful.";
    return true;
}

// Local invocation.
bool PeerProgram::signUp(QString Username){
    if(Username == "") return false;
    QString info_path = MeFolderPath + "info";
    QFileInfo check_file(info_path);
    if(check_file.exists()){ // Already signed up.
        qDebug() << "You already signed up.";
        return false;
    }

    // First time to sign up.
    QString pub_key_path = MeFolderPath + "pubkey";
    QString pri_key_path = MeFolderPath + "prikey";
    QString owner_images_path = MeFolderPath + "images";
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
    out_stream << Username + " 0";
    info_file.close();

    QTextStream fout(&owner_images_file);
    fout << "0";
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

void PeerProgram::signOut(){
    //update me stuff
    QFile file(MeFolderPath+"info");
    if(file.open(QIODevice::WriteOnly)){
        QTextStream fout(&file);
        fout<<my_name<<" "<<next_image_ID;
        file.close();
    }
    else throw("Unable to open user info file.");
    file.setFileName(MeFolderPath+"images");
    if(file.open(QIODevice::WriteOnly)){
        QTextStream fout(&file);
        fout << own_images.size() << '\n';
        for(int i = 0; i < own_images.size(); i++){
            fout << own_images[i].ID << ' ' << own_images[i].image_name << ' ' << authorized_peers[i].size();
            for(QMap<QString, int>::const_iterator it = authorized_peers[i].begin(); it!=authorized_peers[i].end(); it++)
                fout << '\n' << it.key() << ' ' << it.value() << '\n';
        }
        file.close();
    }
    else throw("Unable to write user images file.");
    //update every peer
    for(int i=0;i<peer_list.size();i++){
        if(!QDir(ApplicationRoot+peer_list[i].name+"/").exists())
            QDir(ApplicationRoot+peer_list[i].name+"/").mkpath(".");
        file.setFileName(ApplicationRoot+peer_list[i].name+"/info");
        if(!file.open(QIODevice::WriteOnly)) throw("Unable to open file.");
        QTextStream fout(&file);
        fout << peer_list[i].image_list.size() << '\n';
        for(int j = 0; j < peer_list[i].image_list.size(); j++)
            fout << peer_list[i].image_list[j].ID << ' ' << peer_list[i].image_list[j].image_name << ' ' << peer_list[i].image_list[j].up_count << ' ' << peer_list[i].image_list[j].view_limit << '\n';
        file.close();
    }
    if(QFileInfo(UpdateCache).exists() && !QFile(UpdateCache).remove()) throw("Unable to delete cache file.");
    Server.cleanExit();
    PU.Exit();
}

// Stub entry.
bool PeerProgram::updatePeers(){
    if(!my_public_key.isReady()){
        qDebug() << "You need to sign in first!";
        return false;
    }
    ServerMessage msg(P2S_UPDATE_PEER_LIST);
    char buf[Key::PubKeySize + my_name.size()];
    memcpy(buf, my_public_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    memcpy(buf + Key::PubKeySize, my_name.toStdString().c_str(), my_name.size());
    msg.setPayload(buf, Key::PubKeySize + my_name.size());
    if(!Client.connect(msg, 1000)){
        qDebug() << "Unable to connect to worker.";
        return false;
    }

    onlinePeers online;
    if(!Client.recieveObject(&online)){
        qDebug() << "Unable to recieve object";
        return false;
    }

    for(int i = 0; i < online.getPeerCount(); i++){
        if(online.getPeerKey(i).getAsString() == my_public_key.getAsString()) continue;
        if(peer_key_to_index.count(online.getPeerKey(i).getAsString()) == 0){
            peer_list.push_back(Peer(online.getPeerKey(i), online.getPeerName(i), true, online.getPeerIP(i), online.getPeerPort(i)));
            peer_key_to_index.insert(peer_list.back().key.getAsString(), peer_list.size() - 1);
            QDir tDir(ApplicationRoot + peer_list.back().name + "/");
            if(!tDir.exists()){
                tDir.mkpath(".");
                peer_list.back().key.writeToFile(ApplicationRoot + peer_list.back().name + "/pubkey");
            }
        }
        else{
            Peer& pr = peer_list[peer_key_to_index[online.getPeerKey(i).getAsString()]];
            pr.online = true;
            pr.IP = online.getPeerIP(i);
            pr.port = online.getPeerPort(i);
            pr.name = online.getPeerName(i);
        }
    }

    Client.disconnect(); // Disconnect from worker.
    qDebug() << "Refresh done!";
    return true;
}

// Local invocation.
QVector<QString> PeerProgram::getAllKeys(){
    QVector<QString> ret;
    ret.resize(peer_list.size() + 1);
    ret[0] = my_public_key.getAsString();
    for(int i = 0; i < peer_list.size(); i++)
        ret[i + 1] = peer_list[i].key.getAsString();
    return ret;
}
QVector<QString> PeerProgram::getOnlinePeerKeys(){
    QVector<QString> ret;
    for(int i = 0; i < peer_list.size(); i++)
        if(peer_list[i].online)
            ret.push_back(peer_list[i].key.getAsString());
    return ret;
}
QString PeerProgram::getNameByKey(QString key_str){
    if(key_str == my_public_key.getAsString()) return my_name;
    else return peer_list[peer_key_to_index[key_str]].name;
}
QVector<QString> PeerProgram::getVectorNameByKey(QVector<QString> keys){
    QVector<QString> ret;
    ret.resize(keys.size());
    for(int i = 0; i < keys.size(); i++)
        ret[i] = getNameByKey(keys[i]);
    return ret;
}
QVector<int> PeerProgram::getAllImageIDByOwner(QString key_str){
    QVector<Image> *ptr;
    if(key_str == my_public_key.getAsString()) ptr = &own_images;
    else ptr = &peer_list[peer_key_to_index[key_str]].image_list;
    QVector<int> ret;
    ret.resize(ptr->size());
    for(int i = 0; i < ptr->size(); i++)
        ret[i] = (*ptr)[i].ID;
    return ret;
}
Image PeerProgram::getImgByID(QString key_str, int imgID){
    if(key_str == my_public_key.getAsString()) return own_images[own_img_key_to_index[imgID]];
    else return peer_list[peer_key_to_index[key_str]].image_list[peer_list[peer_key_to_index[key_str]].image_key_to_index[imgID]];
}
QVector<QString> PeerProgram::getAuthorizedListByImageID(int imgID){
    QVector<QString> ret;
    QMap<QString, int> &ref = authorized_peers[own_img_key_to_index[imgID]];
    for(QMap<QString, int>::iterator it = ref.begin(); it != ref.end(); it++)
        ret.push_back(it.key());
    return ret;
}
int PeerProgram::getAuthorizedLimit(QString KEY, int imgID){
    QMap<QString, int>& ref= authorized_peers[own_img_key_to_index[imgID]];
    if(ref.count(KEY) == 0) return 0;
    return ref[KEY];
}
void PeerProgram::AddOwnImage(Image img){
    own_images.push_back(img);
    own_img_key_to_index.insert(img.ID, own_images.size() - 1);
    authorized_peers.push_back(QMap<QString, int>());
}
int PeerProgram::getNewImageID(){
    return next_image_ID++;
}
bool PeerProgram::AddAuthentication(int imgID, QString key, int vLimit){
    cout << hex << peer_list[peer_key_to_index[key]].IP;
    ClientView tClient(peer_list[peer_key_to_index[key]].IP, 4000);
    if(!tClient.connect(ServerMessage(P2P_SEND_IMAGE), 1000)){
        qDebug() << "Unable to connect to peer worker.";
        return false;
    }
    if(!tClient.sendObject(&own_images[own_img_key_to_index[imgID]])){
        qDebug() << "Unable to send picture.";
        return false;
    }
    tClient.disconnect();
    QMap<QString, int> &mp = authorized_peers[own_img_key_to_index[imgID]];
    if(mp.count(key) == 0) mp.insert(key, vLimit);
    else mp[key] = vLimit;
    return true;
}

ServerView PeerProgram::Server(4000);
ClientView PeerProgram::Client("10.40.55.95", 5000);
PeriodicUpdate PeerProgram::PU(10);
ServerThread PeerProgram::ST;

Key PeerProgram::my_public_key; // Fetch from pubkey file.
Key PeerProgram::my_private_key; // Fetch from prikey file.
QString PeerProgram::my_name; // Fetch from info file.
int PeerProgram::next_image_ID; // Fetch from info file.

QVector<Image> PeerProgram::own_images; // Own images from images file.
QVector<Peer> PeerProgram::peer_list; // All peers (online or offline with previous interaction history.
QVector< QMap<QString, int> > PeerProgram::authorized_peers; // Vector indexed by image to map of authorized peers and view limit.

QMap<int, int> PeerProgram::own_img_key_to_index;
QMap<QString, int> PeerProgram::peer_key_to_index;

void PeerProgram::HandelNewImage(WorkerView& Worker, const ServerMessage& initMsg){
    Image img;
    if(!Worker.recieveObject(&img)){
        qDebug() << "Unable to recieve message.";
        return;
    }
    img.setPath(ApplicationRoot + peer_list[peer_key_to_index[img.owner_key.getAsString()]].name + "/" + QString::number(img.ID) + ".png");
}
