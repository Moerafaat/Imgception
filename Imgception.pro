#-------------------------------------------------
#
# Project created by QtCreator 2015-11-28T19:19:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Imgception
TEMPLATE = app


SOURCES += src/main.cpp \
    src/application.cpp \
    src/image.cpp \
    src/transmittable.cpp \
    src/peer.cpp \
    src/update.cpp \
    src/notification.cpp \
    src/notificationheader.cpp \
    src/key.cpp \
    src/globals.cpp \
    src/Views.cpp \
    src/UDPsocket.cpp \
    src/onlinepeers.cpp

HEADERS  += include/application.h \
    include/image.h \
    include/transmittable.h \
    include/peer.h \
    include/update.h \
    include/notification.h \
    include/notificationheader.h \
    include/key.h \
    include/globals.h \
    include/Views.h \
    include/UDPsocket.h \
    include/onlinepeers.h

INCLUDEPATH += include/

FORMS    += application.ui

CONFIG += c++11

LIBS += -pthread -lssl -lcrypto
