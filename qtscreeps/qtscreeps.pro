#-------------------------------------------------
#
# Project created by QtCreator 2015-12-03T19:27:47
#
#-------------------------------------------------

QT       += core gui websockets network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++11

TARGET = qtscreeps
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    screepslogindialog.cpp \
    screepsnetworkmanager.cpp \
    screepsrngutils.cpp \
    screepslocalconfigstore.cpp \
    screepslogwindow.cpp

HEADERS  += mainwindow.h \
    screepslogindialog.h \
    screepsnetworkmanager.h \
    screepsrngutils.h \
    screepslocalconfigstore.h \
    screepslogwindow.h

FORMS    += mainwindow.ui \
    screepslogindialog.ui \
    screepslogwindow.ui

