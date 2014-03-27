#-------------------------------------------------
#
# Project created by QtCreator 2014-01-21T15:34:16
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = Client
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tcpclient.cpp \
    datapacket.cpp

HEADERS += \
    tcpclient.h \
    datapacket.h
