#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T10:16:22
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = RFIDMonitorDaemon
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../RFIDMonitor/

LIBS += -lCoreLibrary

#DEFINES += DEBUG_VERBOSE
DEFINES += DEBUG_LOGGER

SOURCES += main.cpp \
    rfidmonitordaemon.cpp \
    configmanager.cpp
HEADERS += \
    rfidmonitordaemon.h \
    configmanager.h


unix: {
    #homePath = $$system(echo $HOME)
    homePath = /home/pi
    target.path = $$homePath/FishMonitoring
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
