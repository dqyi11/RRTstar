#-------------------------------------------------
#
# Project created by QtCreator 2015-06-29T09:46:33
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rrtstar_viz
TEMPLATE = lib
CONFIG += staticlib

DESTDIR = ../lib

INCLUDEPATH += ../RRTstar

LIBS += -L../lib -lrrtstar

SOURCES += \
    path_planning_info.cpp \
    rrtstar_viz.cpp

HEADERS  += \
    path_planning_info.h \
    rrtstar_viz.h

