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
    terminal/terminalwindow.cpp \
    terminal/terminalcommunicator.cpp \
    rasp/configraspwindow.cpp

HEADERS  += mainwindow.h \
    terminal/terminalcommunicator.h \
    terminal/terminalwindow.h \
    rasp/configraspwindow.h

FORMS    += mainwindow.ui \
    rasp/configraspwindow.ui \
    terminal/terminalwindow.ui

TRANSLATIONS = rfidmonitordesktop_pt-br.ts

OTHER_FILES += \
    translations/rfidmonitordesktop_pt-br.ts
