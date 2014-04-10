QT       += core network

TARGET = Communicator
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

HEADERS += \
    communicationmodule.h \
    communicationservice.h
SOURCES += \
    communicationmodule.cpp \
    communicationservice.cpp

OTHER_FILES += CommunicationModule.json

# A finalidade eh mandar os modulos para a pasta onde esta o executavel para poder fazer debug do projeto
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
