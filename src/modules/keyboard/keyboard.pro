include("../../../try8085.pri")

DESTDIR = $$TRY8085_MODULE_DESTDIR
QT += widgets
TARGET = keyboard
TEMPLATE = app
SOURCES += main.cpp \
    digital_keyboard.cpp
HEADERS += \
    digital_keyboard.h
RESOURCES += \
    keyboard.qrc

ICON = try8085_keyboard.icns
