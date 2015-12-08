#include "Steganifier.h"

Steganifier::Steganifier(std::string scriptPath,std::string scriptName)
{
  std::string pathOfScript = "import sys; sys.path.insert(0,'";
  pathOfScript+=scriptPath;
  pathOfScript+="')";
  Py_Initialize();
  PyRun_SimpleString (pathOfScript.c_str());
  pName = PyString_FromString(scriptName.c_str());
  pModule = PyImport_Import(pName);
}

Steganifier::~Steganifier()
{
  // Clean up
  Py_DECREF(pModule);
  Py_DECREF(pName);

  // Finish the Python Interpreter
  Py_Finalize();
}

char* Steganifier::Steganify(std::vector<QString> paths, QString operation,int& size)
{
  pFunc = PyObject_GetAttrString(pModule, operation.toStdString().c_str());
  if (PyCallable_Check(pFunc))
  {
    pArgs = PyTuple_New(paths.size());
    for(int i=0;i<paths.size();i++)
    {
      pValue = PyString_FromString(paths[i].toStdString().c_str());
      if(!pValue)
      {
        PyErr_Print();
        throw "Something wrong with the inputs DAWG!";
      }
      PyTuple_SetItem(pArgs,i,pValue);
    }
    pValue = PyObject_CallObject(pFunc, pArgs);
    char* charptrResult = PyString_AsString(pValue);
    size = PyString_Size(pValue);
    if(pArgs!=NULL)
    {
      Py_DECREF(pArgs);
    }
    return charptrResult;
  }
}

QImage Steganifier::Steganify(QString fakePath, QString realPath)
{
    std::vector<QString> paths;
    paths.push_back(fakePath); paths.push_back(realPath);//   /Imageception/.temp/stegtest.png"
    int size;
    const unsigned char* temp = reinterpret_cast<const unsigned char*>(Steganify(paths,QString("Steganify"),size));
    QImage img; img.loadFromData(temp,size,"PNG");
    return img;
}

QImage Steganifier::DeSteganify(QString imageToDeSteganify)
{
    std::vector<QString> paths;
    paths.push_back(imageToDeSteganify);
    int size;
    const unsigned char* temp = reinterpret_cast<const unsigned char*>(Steganify(paths,QString("DeSteganify"),size));
    QImage img; img.loadFromData(temp,size,"PNG");
    return img;
}
