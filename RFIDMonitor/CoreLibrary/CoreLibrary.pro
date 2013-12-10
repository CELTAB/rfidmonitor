#-------------------------------------------------
#
# Project created by QtCreator 2013-08-13T14:41:13
#
#-------------------------------------------------

QT       += core sql serialport

QT       -= gui

TARGET = CoreLibrary
TEMPLATE = lib


SOURCES += servicemanager.cpp \
    coremodule.cpp \
    object/rfiddata.cpp \
    logger.cpp

QMAKE_CXXFLAGS += -std=c++11 -Wmissing-field-initializers

HEADERS += \
    servicemanager.h \
    coremodule.h \
    object/rfiddata.h \
    logger.h

unix:{
    homePath = $$system(echo $HOME)
#    homePath = /home/pi
    target.path = /usr/local/lib
    INSTALLS += target
}


#INCLUDEPATH += /home/luisvaldes/Workspace/boost-lib/include/
#LIBS += -L/home/luisvaldes/Workspace/boost-lib/lib
#LIBS += -lboost_log
