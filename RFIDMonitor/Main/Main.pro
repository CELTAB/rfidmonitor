#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T15:12:25
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = RFIDMonitor
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../CoreLibrary

buildPath = $$OUT_PWD
coreLibPath = $$replace(buildPath, $$TARGET, "")

LIBS += -L$$coreLibPath/CoreLibrary
LIBS += -lCoreLibrary

SOURCES += main.cpp

QMAKE_CXXFLAGS += -std=c++11


unix: {
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = $$homePath/FishMonitoring
    INSTALLS += target
}

HEADERS +=
