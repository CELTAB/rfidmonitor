#-------------------------------------------------
#
# Project created by QtCreator 2014-02-24T15:18:33
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = MutlthreadTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    service.cpp \
    readinginterface.cpp \
    synchronizationinterface.cpp \
    packagerinterface.cpp \
    mtapplication.cpp \
    impl/reading.cpp \
    impl/synchronization.cpp \
    impl/packager.cpp

HEADERS += \
    service.h \
    readinginterface.h \
    synchronizationinterface.h \
    packagerinterface.h \
    mtapplication.h \
    impl/reading.h \
    impl/synchronization.h \
    impl/packager.h
