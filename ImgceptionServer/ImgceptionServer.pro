QT += core
QT -= gui

TARGET = ImgceptionServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp\
    src/Views.cpp \
    src/UDPsocket.cpp \
    src/onlinepeers.cpp \
    src/key.cpp \
    src/transmittable.cpp \
    src/server_globals.cpp

HEADERS +=  include/transmittable.h \
    include/Views.h \
    include/UDPsocket.h \
    include/onlinepeers.h \
    include/key.h \
    include/globals.h

INCLUDEPATH += include/

CONFIG += c++11

LIBS += -pthread -lssl -lcrypto
