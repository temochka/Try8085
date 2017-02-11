include("../../../try8085.pri")
DESTDIR = $$TRY8085_MODULE_DESTDIR
QT += widgets
TARGET = indicators
TEMPLATE = app
SOURCES += main.cpp \
    indicators.cpp \
    indicator.cpp
HEADERS += indicators.h \
    indicator.h
RESOURCES += \
    indicators.qrc
ICON = try8085_indicators.icns
