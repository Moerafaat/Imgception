#ifndef GLOBALS_H
#define GLOBALS_H


#include <QString>

#define P2S_UPDATE_PEER_LIST 0x01
#define P2P_SEND_IMAGE 0x02
#define P2P_UPDATE_IMAGE 0x03
#define P2S_NOTIFICATION 0x04
#define S2P_NOTIFICATION 0x05

class Globals{
    Globals() = delete;
    Globals(const Globals&) = delete;
    Globals(Globals&&) = delete;
    ~Globals() = delete;
public:
    static const QString ApplicationRoot;
    static const QString MeFolderPath;
    static const QString TempFolderPath;

    static void InitFolders();
};

#endif // GLOBALS_H
