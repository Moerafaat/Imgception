#include "globals.h"
#include <QDir>

const QString Globals::ApplicationRoot = QDir::homePath() + "/Imgception/";
const QString Globals::MeFolderPath = Globals::ApplicationRoot + "me/";
const QString Globals::TempFolderPath = Globals::ApplicationRoot + ".temp/";
