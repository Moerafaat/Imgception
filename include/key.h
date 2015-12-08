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
    ~Key() = default;

    Key& operator=(const Key&);

    static bool generate(Key&, Key&);
    unsigned char * encrypt(const unsigned char *, const unsigned int, unsigned int&);
    unsigned char * decrypt(const unsigned char *, const unsigned int, unsigned int&);
    bool readFromFile(QString, bool=false);
    bool writeToFile(QString) const;

    QString getAsString() const;
    bool setFromString(QString, bool=false);

    bool isPublic();
    bool isPrivate();
    bool isReady();
public:
    static const unsigned int PubKeySize;
private:
    mutable RSA key;
    bool is_private;
    bool is_ready;
};

#endif // KEY_H
