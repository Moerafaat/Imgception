#ifndef PEERPROGRAM_H
#define PEERPROGRAM_H


#include <QString>
#include "PeriodicUpdate.h"
#include "serverthread.h"
#include "Views.h"
#include "key.h"
#include "peer.h"

#define P2S_UPDATE_PEER_LIST 0x01
#define P2P_SEND_IMAGE 0x02
#define P2P_UPDATE_IMAGE 0x03
#define P2S_NOTIFICATION 0x04
#define S2P_NOTIFICATION 0x05

class PeerProgram{
    PeerProgram() = delete;
    PeerProgram(const PeerProgram&) = delete;
    PeerProgram(PeerProgram&&) = delete;
    ~PeerProgram() = delete;
public:
    static const QString ApplicationRoot;
    static const QString MeFolderPath;
    static const QString TempFolderPath;
    static const QString UpdateCache;

    static void InitFolders();

    static bool login(bool); // Stub entry.
    static bool signUp(QString); // Local invocation.
    static void signOut();
    static bool updatePeers(); // Stub entry.

    static QVector<QString> getAllKeys();
    static QVector<QString> getPeerKeys();
    static QString getNameByKey(QString);
    static QVector<QString> getVectorNameByKey(QVector<QString>);
    static QVector<int> getAllImageIDByOwner(QString);
    static Image getImgByID(QString, int);

    static ServerView Server;
    static ClientView Client;
    static PeriodicUpdate PU;
    static ServerThread ST;

    static Key my_public_key; // Fetch from pubkey file.
    static Key my_private_key; // Fetch from prikey file.
    static QString my_name; // Fetch from info file.
    static int next_image_ID; // Fetch from info file.

    static QVector<Image> own_images; // Own images from images file.
    static QVector<Peer> peer_list; // All peers (online or offline with previous interaction history.
    static QVector< QMap<QString, int> > authorized_peers; // Vector indexed by image to map of authorized peers and view limit.

    static QMap<int, int> own_img_key_to_index;
    static QMap<QString, int> peer_key_to_index; // For fast access.
};

#endif // PEERPROGRAM_H
