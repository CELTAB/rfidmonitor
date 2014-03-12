#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T16:47:45
#
#-------------------------------------------------

QT  += core sql

TARGET = PersistenceModule
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../CoreLibrary

SOURCES += \
    core/sql/exception/sqlunknownexception.cpp \
    core/sql/exception/sqltransactionexception.cpp \
    core/sql/exception/sqlstatementexception.cpp \
    core/sql/exception/sqlexception.cpp \
    core/sql/exception/sqlconnectionexception.cpp \
    core/sql/sqlquery.cpp \
    core/functions.cpp \
    core/connectionpool.cpp \
    data/dao/rfiddatadao.cpp \
    persistencemodule.cpp \
    persistenceservice.cpp

HEADERS += \
    core/sql/exception/sqlunknownexception.h \
    core/sql/exception/sqltransactionexception.h \
    core/sql/exception/sqlstatementexception.h \
    core/sql/exception/sqlexception.h \
    core/sql/exception/sqlconnectionexception.h \
    core/sql/sqlquery.h \
    core/genericdao.h \
    core/functions.h \
    core/connectionpool.h \
    data/dao/rfiddatadao.h \
    persistencemodule.h \
    persistenceservice.h

OTHER_FILES += PersistenceModule.json

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
