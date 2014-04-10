#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T16:47:45
#
#-------------------------------------------------

QT  += core sql

TARGET = Persister
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

SOURCES += \
    data/dao/rfiddatadao.cpp \
    persistencemodule.cpp \
    persistenceservice.cpp

HEADERS += \
    data/dao/rfiddatadao.h \
    persistencemodule.h \
    persistenceservice.h

OTHER_FILES += PersistenceModule.json

buildPath = $$OUT_PWD

coreLibPath = $$replace(buildPath, $${TARGET}Module, "")/Main

DESTDIR += $$coreLibPath/modules

unix: {
    #homePath = $$system(echo $HOME)
    homePath = /home/pi
    target.path = $$homePath/FishMonitoring/modules
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
