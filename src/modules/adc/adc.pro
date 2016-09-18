include("../../../try8085.pri")
DESTDIR = $$TRY8085_MODULE_DESTDIR
QT += widgets
TARGET = adc
TEMPLATE = app
SOURCES += main.cpp \
    adc.cpp \
    adc_graph.cpp
HEADERS += \
    adc.h \
    adc_graph.h
