#-------------------------------------------------
#
# Project created by QtCreator 2014-01-21T15:34:05
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = Server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tcpserver.cpp\
    ../Client/datapacket.cpp

HEADERS += \
    tcpserver.h
