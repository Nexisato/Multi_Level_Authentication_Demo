QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    certreq.cpp \
    genkeypair.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=/usr/include/openssl

LIBS+= -lssl
LIBS+= -lcrypto

RC_FILE = certificate.rc
