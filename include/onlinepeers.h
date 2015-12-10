#ifndef ONLINEPEERS_H
#define ONLINEPEERS_H

#include <QVector>
#include "transmittable.h"
#include "key.h"

class onlinePeers : public Transmittable{
    onlinePeers(const onlinePeers&) = delete;
    onlinePeers(onlinePeers&&) = delete;

public:
    onlinePeers();
    ~onlinePeers() = default;

    onlinePeers& operator=(const onlinePeers&);

    bool pushPeer(QString, QString, const unsigned int, const unsigned short);
    bool pushPeer(Key&, QString, const unsigned int, const unsigned short);
    unsigned int getPeerCount() const;
    Key getPeerKey(unsigned int) const;
    unsigned int getPeerIP(unsigned int) const;
    unsigned short getPeerPort(unsigned int) const;
    QString getPeerName(unsigned int) const;

    char *serialize(unsigned int&) const;
    bool deserialize(const char * const, const unsigned int);
private:
    unsigned int nPeers;
    QVector<QString> peerKey;
    QVector<QString> peerName;
    QVector<unsigned int> peerIP;
    QVector<unsigned short> peerPort;
};

#endif // ONLINEPEERS_H
