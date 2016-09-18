include("../../../try8085.pri")

QT += widgets

DESTDIR = $$TRY8085_MODULE_DESTDIR

TARGET = display
TEMPLATE = app
SOURCES += main.cpp \
    display.cpp
HEADERS += display.h

RESOURCES += \
    display.qrc
