#-------------------------------------------------
#
# Project created by QtCreator 2013-08-22T14:26:33
#
#-------------------------------------------------

QT       += core sql

TARGET = MainServiceModule
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    mainservicemodule.cpp \
    mainservice.cpp

HEADERS += \
    mainservicemodule.h \
    mainservice.h
OTHER_FILES += MainServiceModule.json

INCLUDEPATH += ../CoreLibrary

buildPath = $$OUT_PWD
coreLibPath = $$replace(buildPath, $$TARGET, "")Main

DESTDIR += $$coreLibPath/modules

unix: {
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = $$homePath/FishMonitoring/modules
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
