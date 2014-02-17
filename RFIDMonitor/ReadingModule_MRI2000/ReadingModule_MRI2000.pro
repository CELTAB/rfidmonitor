#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T15:13:10
#
#-------------------------------------------------

QT       += core sql serialport

TARGET = ReadingModule_MRI2000
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

HEADERS += \
    readingmodule.h \
    datareader.h
SOURCES += \
    readingmodule.cpp \
    datareader.cpp

OTHER_FILES += ReadingModule_MRI2000.json

# A finalidade eh mandar os modulos para a pasta onde esta o executavel para poder fazer debug do projeto
buildPath = $$OUT_PWD
coreLibPath = $$replace(buildPath, $$TARGET, "")/RFIDMonitor

DESTDIR += $$coreLibPath/modules

unix: {
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = $$homePath/FishMonitoring/modules
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
