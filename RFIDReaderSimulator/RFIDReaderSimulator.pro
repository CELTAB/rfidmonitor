#-------------------------------------------------
#
# Project created by QtCreator 2013-09-18T09:54:10
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RFIDReaderSimulator
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    datareader.cpp

HEADERS  += widget.h \
    datareader.h

FORMS    += widget.ui


QMAKE_CXXFLAGS += -std=c++11
