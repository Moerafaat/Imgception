#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <cstring>
#include <QByteArray>
#include "image.h"
#include "peerprogram.h"

Steganifier Image::s(QDir::homePath(),QString("Steganify"));

Image::Image(int Id, Key OwnerKey, QString Path, QString Name, int UpCount, int ViewLimit):ID(Id),owner_key(OwnerKey),view_limit(-1),up_count(0),path(Path),image_name(Name){

}

Image::Image(int Id, Key OwnerKey, QString fakePath, QString realPath, QString pathToSaveTo, QString imageName):ID(Id),owner_key(OwnerKey),image_name(imageName),path(pathToSaveTo),view_limit(-1),up_count(0){
    QImage image = s.Steganify(fakePath,realPath);
    image.save(pathToSaveTo+imageName,"PNG");
}

Image& Image::operator=(const Image& image){
    ID = image.ID;
    owner_key = image.owner_key;
    path = image.path;
    image_name = image.image_name;
    up_count = image.up_count;
    view_limit = image.view_limit;
}

char* Image::serialize(unsigned int & Size) const{
    QFile imageFile(path);
    imageFile.open(QIODevice::ReadOnly);
    QByteArray bytearr = imageFile.readAll();

    Size = 4 * 5 + image_name.size() + bytearr.size() + Key::PubKeySize;
    char* data = new char[Size];
    *(int*)data = HostToNetwork(ID);
    *(int*)(data+4)=HostToNetwork(up_count);
    *(int*)(data+8)=HostToNetwork(view_limit);

    *(int*)(data+12)=HostToNetwork(image_name.size());
    memcpy(data+16,image_name.toStdString().c_str(),image_name.size());

    *(int*)(data+16+image_name.size())=HostToNetwork(bytearr.size());
    memcpy(data+16+image_name.size()+4, bytearr.data(), bytearr.size());

    memcpy(data+Size-Key::PubKeySize, owner_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    return data;
}

bool Image::deserialize(const char* const SerializedImage, const unsigned int Size){
    if(Size < 4 * 5 + Key::PubKeySize) return false;
    ID = NetworkToHost(*(int*)SerializedImage);
    up_count=NetworkToHost(*(int*)(SerializedImage+4));
    view_limit=NetworkToHost(*(int*)(SerializedImage+8));
    int image_name_size= NetworkToHost(*(int*)(SerializedImage+12));

    if(Size < 4*5 + Key::PubKeySize + image_name_size) return false;
    char* tempImageName = new char[image_name_size];
    memcpy(tempImageName,(SerializedImage+16),image_name_size);
    image_name = QString::fromStdString(std::string(tempImageName, image_name_size));

    int imageStringSize = NetworkToHost(*(int *)(SerializedImage+16+image_name_size));
    if(Size != 4 * 5 + Key::PubKeySize + image_name_size + imageStringSize){
        delete [] tempImageName;
        return false;
    }
    char * tempImageString = new char[imageStringSize];
    QByteArray bytearr(SerializedImage+16+image_name_size + 4, imageStringSize);

    QFile file(path = PeerProgram::TempFolderPath + "image.png");
    file.open(QIODevice::WriteOnly);
    file.write(bytearr);
    file.close();

    owner_key.setFromString(QString::fromStdString(std::string(SerializedImage+Size-Key::PubKeySize, Key::PubKeySize)));
    delete [] tempImageString;
    return true;
}

// Stub entry.
QImage Image::getImage(){ // Function that returns the image for display.
    return QImage(path);    //Don't forget to remove
    if(up_count < view_limit)
    {
        up_count += view_limit != -1;
        return s.DeSteganify(path);
    }
    else return QImage(path);
}
