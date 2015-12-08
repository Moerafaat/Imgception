#ifndef PEER_H
#define PEER_H


#include <QVector>
#include <QString>
#include <QMap>
#include "image.h"

class Peer{
public:
    Peer(int, QString, unsigned int, unsigned short);
private:
    int key; // Peer identifier.
    QString name; // Peer name.
    QVector<Image> images; // Images belonging to this peer that the current user has seen at some point in time (On startup + Server update).
    QVector<int> image_views; // Number of views per image.
    bool online; // Indicate whether the peer is online or offline.
    unsigned int IP; // Peer IP.
    unsigned short port; // Peer port.

    QMap<int, int> image_key_to_index; // For fast access.
};

#endif // PEER_H
