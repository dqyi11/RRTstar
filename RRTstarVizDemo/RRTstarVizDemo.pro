#-------------------------------------------------
#
# Project created by QtCreator 2015-06-29T10:43:31
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rrtstar_viz_demo
TEMPLATE = app

DESTDIR = ../bin

INCLUDEPATH += ../RRTstar \
               ../RRTstarViz

LIBS += -L../lib -lrrtstar -lrrtstar_viz

SOURCES += main.cpp\
        mainwindow.cpp \
    configobjdialog.cpp

HEADERS  += mainwindow.h \
    configobjdialog.h
