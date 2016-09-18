include(try8085.pri)

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
    vm \
    translator \
    display \
    keyboard \
    indicators \
    adc \
    ide

display.subdir = src/modules/display
keyboard.subdir = src/modules/keyboard
indicators.subdir = src/modules/indicators
adc.subdir = src/modules/adc
vm.subdir = src/vm
translator.subdir = src/translator
ide.subdir = src/ide

ide.depends = vm display keyboard indicators adc
