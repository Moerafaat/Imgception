#ifndef PEER_H
#define PEER_H

#include <QVector>
#include <QString>
#include <QMap>
#include "key.h"
#include "image.h"

class PeerProgram;

class Peer{
    friend class PeerProgram;
public:
    Peer() = default;
    Peer(Key, QString, bool, unsigned int=-1, unsigned short=-1);
private:
    Key key; // Peer identifier.
    QString name; // Peer name.
    bool online; // Indicate whether the peer is online or offline.
    unsigned int IP; // Peer IP.
    unsigned short port; // Peer port.

    QVector<Image> image_list; // Images belonging to this peer that the current user has seen at some point in time (On startup + Server update).

    QMap<int, int> image_key_to_index; // For fast access.
};

#endif // PEER_H
