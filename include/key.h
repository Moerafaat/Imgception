#ifndef KEY_H
#define KEY_H

#include <QString>
#include <openssl/rsa.h>

class Key{
public:
    Key();
    Key(QString, bool=false);
    Key(const Key&) = default;
    Key(Key&&) = default;
    ~Key();

    static bool generate(Key&, Key&);
    QString getKey();
    unsigned char * encrypt(const unsigned char *, const unsigned int, unsigned int&);
    unsigned char * decrypt(const unsigned char *, const unsigned int, unsigned int&);
    bool readFromFile(QString, bool=false);
    bool writeToFile(QString);

    bool isPublic();
    bool isPrivate();
    bool isReady();
private:
    RSA* key;
    bool is_private;
    bool is_ready;
};

#endif // KEY_H
