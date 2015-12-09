#-------------------------------------------------
#
# Project created by QtCreator 2015-11-28T19:19:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Imgception
TEMPLATE = app

SOURCES += src/main_client.cpp \
    src/image.cpp \
    src/transmittable.cpp \
    src/peer.cpp \
    src/update.cpp \
    src/notification.cpp \
    src/key.cpp \
    src/Views.cpp \
    src/UDPsocket.cpp \
    src/onlinepeers.cpp \
    src/Steganifier.cpp \
    src/PeriodicUpdate.cpp \
    src/serverthread.cpp \
    src/peerprogram.cpp \
    src/dialog.cpp \
    src/mainwindow.cpp \
    src/newupload2.cpp

HEADERS  += include/image.h \
    include/transmittable.h \
    include/peer.h \
    include/update.h \
    include/notification.h \
    include/key.h \
    include/Views.h \
    include/UDPsocket.h \
    include/onlinepeers.h \
    include/Steganifier.h \
    include/PeriodicUpdate.h \
    include/serverthread.h \
    include/peerprogram.h \
    include/dialog.h \
    include/mainwindow.h \
    include/newupload2.h

INCLUDEPATH += include/ \
    /usr/include/python2.7 \
    include/

FORMS    += dialog.ui \
    mainwindow.ui \
    newupload2.ui

CONFIG += c++11

LIBS += -pthread -lssl -lcrypto -lpython2.7

RESOURCES += \
    myApp.qrc
