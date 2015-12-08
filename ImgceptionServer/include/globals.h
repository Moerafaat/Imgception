#ifndef GLOBALS_H
#define GLOBALS_H


#include <QString>

class Globals{
    Globals() = delete;
    Globals(const Globals&) = delete;
    Globals(Globals&&) = delete;
    ~Globals() = delete;
public:
    static const QString ApplicationRoot;
    static const QString MeFolderPath;
    static const QString TempFolderPath;
};

#endif // GLOBALS_H
