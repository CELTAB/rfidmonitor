#-------------------------------------------------
#
# Project created by QtCreator 2014-02-24T16:53:57
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RFIDMonitorDesktop
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    terminalwindow.cpp \
    terminalcommunicator.cpp

HEADERS  += mainwindow.h \
    terminalwindow.h \
    terminalcommunicator.h

FORMS    += mainwindow.ui \
    terminalwindow.ui

TRANSLATIONS = rfidmonitordesktop_pt-br.ts
