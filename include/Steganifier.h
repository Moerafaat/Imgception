#ifndef STEGANIFIER_H
#define STEGANIFIER_H
#include <string>
#include <vector>
#include </usr/include/python2.7/Python.h>
#include <QString>
#include <QImage>
class Steganifier
{
public:
  Steganifier(std::string scriptPath,std::string scriptName);
  ~Steganifier();
  char* Steganify(std::vector<QString> paths, QString operation,int&size);
  QImage Steganify(QString fakePath, QString realPath);
  QImage DeSteganify(QString imageToDesteganify);
private:
  PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pValue;
};

#endif
