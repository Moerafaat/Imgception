#include <QDebug>
#include <cstdio>
#include <openssl/pem.h>
#include <QFile>
#include "key.h"
#include "globals.h"
using namespace std;

bool Key::generate(Key& PubKey, Key& PriKey){
    const QString FilePath = Globals::TempFolderPath + "key";

    PriKey.key = RSA_generate_key(2048, 3, nullptr, nullptr); // Generate the owner key.
    if(PriKey.key == nullptr) return false;
    PriKey.is_private = false;
    PriKey.is_ready = true;
    if(!PriKey.writeToFile(FilePath)) return false;
    PriKey.is_private = true;

    PubKey.is_ready = false;
    if(!PubKey.readFromFile(FilePath)) return false;
    PubKey.is_ready = true;

    return true;
}

Key::Key(){
    is_ready = false;
}

Key::Key(QString FilePath, bool IsPrivate){
    is_ready = false;
    if(!readFromFile(FilePath, IsPrivate))
        throw("Unable to read key.");
    is_ready = true;
}

Key::~Key(){
    if(is_ready)
        delete key;
}

QString Key::getKey(){
    if(!is_ready || !writeToFile(Globals::TempFolderPath + "key")) return "";
    QFile file(Globals::TempFolderPath + "key");
    if(!file.open(QIODevice::ReadOnly)) return "";
    return QTextStream(&file).readAll();
}

unsigned char * Key::encrypt(const unsigned char *Message, const unsigned int MessageSize, unsigned int& EncryptSize){
    if(!is_ready || is_private) return nullptr;
    const unsigned int rsa_size = 256;
    const unsigned int frag_size = 200;
    unsigned int nFrag = (MessageSize + frag_size - 1) / frag_size;
    EncryptSize = rsa_size * nFrag;
    unsigned char *encrypt = new unsigned char[EncryptSize];

    for(int i=0; i<MessageSize; i+=frag_size)
        if(RSA_public_encrypt(min(frag_size, MessageSize - i), Message+i, encrypt + i, key, RSA_PKCS1_OAEP_PADDING) == -1){
            delete [] encrypt;
            return nullptr;
        }

    return encrypt;
}

unsigned char * Key::decrypt(const unsigned char *Encrypt, const unsigned int EncryptSize, unsigned int& DecryptSize){
    if(!is_ready || !is_private) return nullptr;
    const unsigned int rsa_size = 256;
    const unsigned int frag_size = 200;
    int last_frag_size;
    unsigned char *tmp = new unsigned char[rsa_size];
    last_frag_size = RSA_private_decrypt(rsa_size, Encrypt + EncryptSize - rsa_size, tmp, key, RSA_PKCS1_OAEP_PADDING);
    if(last_frag_size == -1 || last_frag_size > frag_size){
        delete [] tmp;
        return nullptr;
    }
    DecryptSize = EncryptSize/rsa_size * frag_size - frag_size + last_frag_size;
    unsigned char *decrypt = new unsigned char[DecryptSize];
    for(int i = rsa_size; i < EncryptSize; i += rsa_size)
        if(RSA_private_decrypt(rsa_size, Encrypt + i - rsa_size, decrypt + i - rsa_size, key, RSA_PKCS1_OAEP_PADDING) == -1)
        {
            delete [] tmp;
            delete [] decrypt;
            return nullptr;
        }
    memcpy(decrypt + DecryptSize - last_frag_size, tmp, last_frag_size);
    delete [] tmp;
    return decrypt;
}

bool Key::readFromFile(QString FilePath, bool IsPrivate){
    if(is_ready) delete key;
    FILE* file = fopen(FilePath.toStdString().c_str(), "r");
    if(file == nullptr) return false;
    if(is_private = IsPrivate)
        key = PEM_read_RSAPrivateKey(file, nullptr, nullptr, nullptr);
    else
        key = PEM_read_RSAPublicKey(file, nullptr, nullptr, nullptr);
    fclose(file);
    return key != nullptr;
}

bool Key::writeToFile(QString FilePath){
    if(!is_ready) return false;
    int ret_code;
    FILE* file = fopen(FilePath.toStdString().c_str(), "w");
    if(file == nullptr) return false;
    if(is_private)
        ret_code = PEM_write_RSAPrivateKey(file, key, nullptr, 0, 0, nullptr, nullptr);
    else
        ret_code = PEM_write_RSAPublicKey(file, key);
    fclose(file);
    return ret_code;
}

bool Key::isPublic(){
    return !is_private;
}

bool Key::isPrivate(){
    return is_private;
}

bool Key::isReady(){
    return is_ready;
}
