include('../../try8085.pri')

QT -= gui

DESTDIR = $$TRY8085_LIB_DESTDIR

TARGET = try8085-vm
TEMPLATE = lib
CONFIG += staticlib

SOURCES += vm.cpp
HEADERS += vm.h
