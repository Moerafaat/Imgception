#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileInfoList>
#include <QCryptographicHash>
#include "peerprogram.h"
#include "onlinepeers.h"

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

    //PU.start();
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
    for(int i=0;i<peer_list.size();i++) if(peer_list[i].image_list.size()==0){
        QDir peerDir(ApplicationRoot+peer_list[i].name+"/");
        if(peerDir.exists() && peerDir.removeRecursively()) throw ("Unable to delete Peer Director");
    }
    else{
        if(!QDir(ApplicationRoot+peer_list[i].name+"/").exists()) throw("Unexpected error while updating peer");
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

    qDebug() << online.getPeerCount();
    for(int i = 0; i < online.getPeerCount(); i++)
        qDebug() << online.getPeerKey(i).getAsString();
    //Actually update
    //Don't forget to create Peer folder if needed

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
QVector<QString> PeerProgram::getPeerKeys(){
    QVector<QString> ret;
    ret.resize(peer_list.size());
    for(int i = 0; i < peer_list.size(); i++)
        ret[i] = peer_list[i].key.getAsString();
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
