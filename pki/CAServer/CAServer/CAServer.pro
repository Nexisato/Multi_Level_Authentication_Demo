QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filerecv.cpp \
    rootsign.cpp \
    rvkcert.cpp \
    rootverify.cpp \
    jsonoper.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=/usr/include/openssl

LIBS+= -lssl
LIBS+= -lcrypto

RC_FILE = certificate_server.rc
