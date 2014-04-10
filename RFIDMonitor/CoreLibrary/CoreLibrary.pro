#-------------------------------------------------
#
# Project created by QtCreator 2013-08-13T14:41:13
#
#-------------------------------------------------

QT       += core sql serialport

QT       -= gui

TARGET = CoreLibrary
TEMPLATE = lib


SOURCES += \
    coremodule.cpp \
    object/rfiddata.cpp \
    logger.cpp \
    core/interfaces.cpp \
    core/service.cpp \
    applicationsettings.cpp \
    rfidmonitor.cpp \
    json/rfidmonitorsettings.cpp \
    json/synchronizationpacket.cpp \
    json/nodejsmessage.cpp \
    core/connectionpool.cpp \
    core/functions.cpp \
    core/sql/sqlquery.cpp \
    core/sql/exception/sqlconnectionexception.cpp \
    core/sql/exception/sqlexception.cpp \
    core/sql/exception/sqlstatementexception.cpp \
    core/sql/exception/sqltransactionexception.cpp \
    core/sql/exception/sqlunknownexception.cpp

QMAKE_CXXFLAGS += -std=c++11 -Wmissing-field-initializers

HEADERS += \
    coremodule.h \
    object/rfiddata.h \
    logger.h \
    core/interfaces.h \
    core/service.h \
    applicationsettings.h \
    rfidmonitor.h \
    json/jsonrwinterface.h \
    json/rfidmonitorsettings.h \
    json/synchronizationpacket.h \
    json/nodejsmessage.h \
    core/connectionpool.h \
    core/functions.h \
    core/genericdao.h \
    core/sql/sqlquery.h \
    core/sql/exception/sqlconnectionexception.h \
    core/sql/exception/sqlexception.h \
    core/sql/exception/sqlstatementexception.h \
    core/sql/exception/sqltransactionexception.h \
    core/sql/exception/sqlunknownexception.h

unix:{
    #homePath = $$system(echo $HOME)
    homePath = /home/pi
    target.path = /usr/local/lib
    INSTALLS += target
}


#INCLUDEPATH += /home/luisvaldes/Workspace/boost-lib/include/
#LIBS += -L/home/luisvaldes/Workspace/boost-lib/lib
#LIBS += -lboost_log
