#-------------------------------------------------
#
# Project created by QtCreator 2018-06-19T16:03:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SnakeDeviceManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# para ver los mensajes de debug en una consola independiente,
# descomentar la sgte linea
#CONFIG += console

# para solucionar la situacion de que no genera un ejecutable,
# sino que un archivo x-sharedlib
QMAKE_LFLAGS += -no-pie

#permite compilar codigo de cryptauthlib
QMAKE_CXXFLAGS += -fpermissive

# para compilar con librerias estaticas, cuando existen problemas de no uso de librerias
# CONFIG += static tambien funciona
# CONFIG += staticlib

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    login.cpp \
    conncalabazachile.cpp \
    connjabulani.cpp \
    conngeckobrasil.cpp \
    writegameform.cpp \
    conexiones.cpp \
    writedongleform.cpp \
    dongle.cpp \
    readdongleform.cpp \
    registerdongledialog.cpp \
    deletedongledialog.cpp \
    printer.cpp \
    contactodialog.cpp \
    printerconfigdialog.cpp \
    printkitdialog.cpp \
    writetinkerform.cpp \
    readtinkerform.cpp \
    tinkerlibs/crc16.cpp \
    tinkerlibs/fram.cpp \
    tinkerlibs/storage.cpp \
    writegametinkerform.cpp \
    tinkerlibs/rtc.cpp \
    tinkerlibs/crypto.cpp

HEADERS += \
        mainwindow.h \
    login.h \
    conncalabazachile.h \
    connjabulani.h \
    conngeckobrasil.h \
    writegameform.h \
    conexiones.h \
    writedongleform.h \
    dongle.h \
    dic32.h \
    readdongleform.h \
    registerdongledialog.h \
    deletedongledialog.h \
    printer.h \
    contactodialog.h \
    printerconfigdialog.h \
    printkitdialog.h \
    writetinkerform.h \
    readtinkerform.h \
    tinkerlibs/crc16.h \
    tinkerlibs/fram.h \
    tinkerlibs/storage.h \
    tinkerinterface.h \
    writegametinkerform.h \
    tinkerlibs/rtc.h \
    tinkerlibs/crypto.h

FORMS += \
        mainwindow.ui \
    login.ui \
    writegameform.ui \
    writedongleform.ui \
    readdongleform.ui \
    registerdongledialog.ui \
    deletedongledialog.ui \
    contactodialog.ui \
    printerconfigdialog.ui \
    printkitdialog.ui \
    writetinkerform.ui \
    readtinkerform.ui \
    writegametinkerform.ui

RESOURCES += \
    imgresources.qrc

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/tinkerlibs
INCLUDEPATH += /usr/local/include/buspirate
INCLUDEPATH += /home/vmuser/sdm/cryptoauthlib/

DEPENDPATH += $$PWD/include

LIBS += -L$$PWD/api/ -lJRockey6Smart
LIBS += -lcups
LIBS += -L/usr/local/lib -lbuspirate
#LIBS += -L/home/vmuser/sdm/cryptoauthlib/bin -lcryptoauthlib
LIBS += /home/vmuser/sdm/cryptoauthlib/bin/cryptoauthlib.a
LIBS += -lcryptsetup


