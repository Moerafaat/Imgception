#include "globals.h"
#include <QDir>

const QString Globals::ApplicationRoot = QDir::homePath() + "/Imgception/";
const QString Globals::MeFolderPath = Globals::ApplicationRoot + "me/";
const QString Globals::TempFolderPath = Globals::ApplicationRoot + ".temp/";

void Globals::InitFolders(){
    QDir tDir(Globals::ApplicationRoot);
    if(!tDir.exists()) tDir.mkpath(".");
    tDir.setPath(Globals::MeFolderPath);
    if(!tDir.exists()) tDir.mkpath(".");
    tDir.setPath(Globals::TempFolderPath);
    if(!tDir.exists()) tDir.mkpath(".");
}
