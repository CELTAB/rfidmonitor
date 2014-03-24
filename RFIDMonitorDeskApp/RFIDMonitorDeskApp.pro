#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T14:27:31
#
#-------------------------------------------------

QT       += core gui serialport sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RFIDMonitorDeskApp
TEMPLATE = app


SOURCES += main.cpp\
    gui/mainwindow.cpp \
    gui/serialconnconfigwidget.cpp \
    communication/serialcommunication.cpp \
    communication/networkcommunication.cpp \
    gui/readerinteractorwidget.cpp \
    gui/rfidmonitorinteractorwidget.cpp \
    gui/networkconnconfigwidget.cpp \
    settings.cpp \
    persistence.cpp \
    gui/systemmessageswidget.cpp

HEADERS  += mainwindow.h \
    gui/serialconnconfigwidget.h \
    communication/serialcommunication.h \
    communication/networkcommunication.h \
    gui/readerinteractorwidget.h \
    gui/rfidmonitorinteractorwidget.h \
    gui/networkconnconfigwidget.h \
    settings.h \
    persistence.h \
    gui/systemmessageswidget.h

FORMS    += \
    gui/mainwindow.ui \
    gui/serialconnconfigwidget.ui \
    gui/readerinteractorwidget.ui \
    gui/rfidmonitorinteractorwidget.ui \
    gui/networkconnconfigwidget.ui \
    gui/systemmessageswidget.ui
