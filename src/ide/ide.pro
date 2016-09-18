include("../../try8085.pri")

QT += xml
QT += widgets
TARGET = try8085
TEMPLATE = app
SOURCES += main.cpp \
    c80.cpp \
    vm_thread.cpp \
    vm_control_widget.cpp \
    connections_widget.cpp \
    ide.cpp \
    base_converter_widget.cpp \
    errors_list_widget.cpp \
    go_to_address_widget.cpp \
    c80_highlighter.cpp \
    registers_state_widget.cpp \
    flags_state_widget.cpp \
    settings_widget.cpp \
    find_dialog_widget.cpp \
    editor_widget.cpp
TRANSLATIONS = try8085_en.ts \
               try8085_ru.ts

HEADERS += \
    c80.h \
    vm_thread.h \
    vm_control_widget.h \
    connections_widget.h \
    ide.h \
    base_converter_widget.h \
    errors_list_widget.h \
    go_to_address_widget.h \
    c80_highlighter.h \
    registers_state_widget.h \
    flags_state_widget.h \
    settings_widget.h \
    find_dialog_widget.h \
    editor_widget.h \
    globals.h

FORMS += \
    ide.ui

RESOURCES += \
    ide.qrc

OTHER_FILES += appicon.rc

RC_FILE = appicon.rc

INCLUDEPATH += ../vm
INCLUDEPATH += ../translator

LIBS += -L$$TRY8085_LIB_DESTDIR -ltry8085-vm -ltry8085-translator

DISTFILES +=
