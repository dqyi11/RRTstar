
TEMPLATE = lib
TARGET = rrtstar
CONFIG += staticlib

DESTDIR = ../lib

SOURCES += rrtstar.cpp \

HEADERS  += \
    kdtree++/region.hpp \
    kdtree++/node.hpp \
    kdtree++/kdtree.hpp \
    kdtree++/iterator.hpp \
    kdtree++/function.hpp \
    kdtree++/allocator.hpp \
    rrtstar.h \
    KDTree2D.h \
    KDTree2D.h