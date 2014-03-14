#-------------------------------------------------
#
# Project created by QtCreator 2013-08-22T14:33:36
#
#-------------------------------------------------

QT       += core sql

TARGET = Exporter
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

HEADERS += exportmodule.h \
    export/exportlocaldata.h \
    devicethread.h \
    export/blinkled.h \
    export/gpiointerface.h \
    exportservice.h
SOURCES += exportmodule.cpp \
    export/exportlocaldata.cpp \
    devicethread.cpp \
    export/blinkled.cpp \
    export/gpiointerface.c \
    exportservice.cpp

OTHER_FILES += ExportModule.json

buildPath = $$OUT_PWD
coreLibPath = $$replace(buildPath, $${TARGET}Module, "")/Main

DESTDIR = $$coreLibPath/modules

LIBS += -ludev
LIBS += -lrt

unix:{
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = $$homePath/FishMonitoring/modules
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
