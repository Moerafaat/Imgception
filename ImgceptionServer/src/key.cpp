#include <QTextStream>
#include <cstdio>
#include <openssl/pem.h>
#include <QFile>
#include "key.h"
#include "globals.h"
using namespace std;

const unsigned int Key::PubKeySize = 426;

bool Key::generate(Key& PubKey, Key& PriKey){
    const QString FilePath = Globals::TempFolderPath + "key";
    RSA *temp = RSA_generate_key(2048, 3, nullptr, nullptr); // Generate the owner key.
    if(temp == nullptr) return false;
    PriKey.key = *temp;
    delete temp;
    PriKey.is_private = false;
    PriKey.is_ready = true;
    if(!PriKey.writeToFile(FilePath)) return false;
    PriKey.is_private = true;

    PubKey.is_ready = false;
    return PubKey.readFromFile(FilePath);
}

Key::Key(){
    is_ready = false;
}

Key::Key(QString FilePath, bool IsPrivate){
    is_ready = false;
    if(!readFromFile(FilePath, IsPrivate))
        throw("Unable to read key.");
}

Key& Key::operator=(const Key& cpy){
    key = cpy.key;
    is_private = cpy.is_private;
    is_ready = cpy.is_ready;
}

unsigned char * Key::encrypt(const unsigned char *Message, const unsigned int MessageSize, unsigned int& EncryptSize){
    if(!is_ready || is_private) return nullptr;
    const unsigned int rsa_size = 256;
    const unsigned int frag_size = 200;
    unsigned int nFrag = (MessageSize + frag_size - 1) / frag_size;
    EncryptSize = rsa_size * nFrag;
    unsigned char *encrypt = new unsigned char[EncryptSize];

    for(int i=0; i<MessageSize; i+=frag_size)
        if(RSA_public_encrypt(min(frag_size, MessageSize - i), Message+i, encrypt + i, &key, RSA_PKCS1_OAEP_PADDING) == -1){
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
    last_frag_size = RSA_private_decrypt(rsa_size, Encrypt + EncryptSize - rsa_size, tmp, &key, RSA_PKCS1_OAEP_PADDING);
    if(last_frag_size == -1 || last_frag_size > frag_size){
        delete [] tmp;
        return nullptr;
    }
    DecryptSize = EncryptSize/rsa_size * frag_size - frag_size + last_frag_size;
    unsigned char *decrypt = new unsigned char[DecryptSize];
    for(int i = rsa_size; i < EncryptSize; i += rsa_size)
        if(RSA_private_decrypt(rsa_size, Encrypt + i - rsa_size, decrypt + i - rsa_size, &key, RSA_PKCS1_OAEP_PADDING) == -1)
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
    FILE* file = fopen(FilePath.toStdString().c_str(), "r");
    if(file == nullptr) return false;
    RSA *temp;
    if(is_private = IsPrivate)
        temp = PEM_read_RSAPrivateKey(file, nullptr, nullptr, nullptr);
    else
        temp = PEM_read_RSAPublicKey(file, nullptr, nullptr, nullptr);
    fclose(file);
    if(temp == nullptr) return false;
    key = *temp;
    delete [] temp;
    is_ready = true;
    return true;
}

bool Key::writeToFile(QString FilePath) const{
    if(!is_ready) return false;
    int ret_code;
    FILE* file = fopen(FilePath.toStdString().c_str(), "w");
    if(file == nullptr) return false;
    if(is_private)
        ret_code = PEM_write_RSAPrivateKey(file, &key, nullptr, 0, 0, nullptr, nullptr);
    else
        ret_code = PEM_write_RSAPublicKey(file, &key);
    fclose(file);
    return ret_code;
}

QString Key::getAsString() const{
    if(!is_ready || !writeToFile(Globals::TempFolderPath + "key")) return "";
    QFile file(Globals::TempFolderPath + "key");
    if(!file.open(QIODevice::ReadOnly)) return "";
    return QTextStream(&file).readAll();
}

bool Key::setFromString(QString str, bool IsPrivate){
    QFile file(Globals::TempFolderPath + "key");
    if(!file.open(QIODevice::WriteOnly)) return false;
    QTextStream stream(&file);
    stream << str;
    file.close();
    return readFromFile(Globals::TempFolderPath + "key", IsPrivate);
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
