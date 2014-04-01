#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T13:33:27
#
#-------------------------------------------------

QT       += core network sql

TARGET = Synchronizer
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

HEADERS += \
    synchronizationmodule.h \
    synchronizationservice.h \
    packagerservice.h \
    data/object/packet.h \
    data/dao/packetdao.h
SOURCES += \
    synchronizationmodule.cpp \
    synchronizationservice.cpp \
    packagerservice.cpp \
    data/object/packet.cpp \
    data/dao/packetdao.cpp

OTHER_FILES += SynchronizationModule.json \
    SynchronizationModule.json

# A finalidade eh mandar os modulos para a pasta onde esta o executavel para poder fazer debug do projeto
buildPath = $$OUT_PWD
coreLibPath = $$replace(buildPath, $${TARGET}Module, "")Main

DESTDIR += $$coreLibPath/modules

unix: {
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = $$homePath/FishMonitoring/modules
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
