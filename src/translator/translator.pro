include('../../try8085.pri')

QT -= gui

DESTDIR = $$TRY8085_LIB_DESTDIR

TARGET = try8085-translator
TEMPLATE = lib
CONFIG += staticlib

SOURCES += translator.cpp \
    arithmetic.cpp \
    asm_id.cpp
HEADERS += translator.h \
    arithmetic.h \
    asm_id.h \
    asm_source.h

