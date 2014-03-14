#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T15:13:10
#
#-------------------------------------------------

QT       += core sql serialport

TARGET = ReaderRFM008B
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

HEADERS += \
    reader_rfm008b.h \
    reader_rfm008bmodule.h
SOURCES += \
    reader_rfm008b.cpp \
    reader_rfm008bmodule.cpp

OTHER_FILES += ReaderRFM08BModule.json

# A finalidade eh mandar os modulos para a pasta onde esta o executavel para poder fazer debug do projeto
buildPath = $$OUT_PWD
coreLibPath = $$replace(buildPath, $${TARGET}Module, "")/Main

DESTDIR += $$coreLibPath/modules

unix: {
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = $$homePath/FishMonitoring/modules
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
