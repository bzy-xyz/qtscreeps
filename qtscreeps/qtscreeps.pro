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
    screepslogindialog.cpp \
    screepsnetworkmanager.cpp \
    screepsrngutils.cpp \
    screepslocalconfigstore.cpp \
    screepslogwindow.cpp \
    screepsmainwindow.cpp \
    screepsconsolewindow.cpp \
    screepsmaingraphicsview.cpp \
    screepsroom.cpp \
    screepsroomtile.cpp \
    screepsroommap2.cpp \
    screepscoordutils.cpp \
    screepsterraincache.cpp

HEADERS  += \
    screepslogindialog.h \
    screepsnetworkmanager.h \
    screepsrngutils.h \
    screepslocalconfigstore.h \
    screepslogwindow.h \
    screepsmainwindow.h \
    screepsconsolewindow.h \
    screepsmaingraphicsview.h \
    screepsroom.h \
    screepsroomtile.h \
    screepsroommap2.h \
    screepscoordutils.h \
    screepsterraincache.h

FORMS    += \
    screepslogindialog.ui \
    screepslogwindow.ui \
    screepsmainwindow.ui \
    screepsconsolewindow.ui

