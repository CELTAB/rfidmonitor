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
    gui/systemmessageswidget.cpp \
    gui/devicemodel.cpp \
    gui/rictlmb2b30widget.cpp

HEADERS  += \
    gui/serialconnconfigwidget.h \
    communication/serialcommunication.h \
    communication/networkcommunication.h \
    gui/readerinteractorwidget.h \
    gui/rfidmonitorinteractorwidget.h \
    gui/networkconnconfigwidget.h \
    settings.h \
    persistence.h \
    gui/systemmessageswidget.h \
    gui/mainwindow.h \
    gui/devicemodel.h \
    gui/rictlmb2b30widget.h

FORMS    += \
    gui/mainwindow.ui \
    gui/serialconnconfigwidget.ui \
    gui/readerinteractorwidget.ui \
    gui/rfidmonitorinteractorwidget.ui \
    gui/networkconnconfigwidget.ui \
    gui/systemmessageswidget.ui \
    gui/rictlmb2b30widget.ui


QMAKE_CXXFLAGS += -std=c++11

TRANSLATIONS =  translations/app/rfidmonitordeskapp_pt_BR.ts \
                translations/app/rfidmonitordeskapp_es.ts
