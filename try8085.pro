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

CONFIG(release, release|debug) {
    macx:SUBDIRS += dmg
}

display.subdir = src/modules/display
keyboard.subdir = src/modules/keyboard
indicators.subdir = src/modules/indicators
adc.subdir = src/modules/adc
vm.subdir = src/vm
translator.subdir = src/translator
ide.subdir = src/ide
dmg.subdir = src/dmg

ide.depends = vm display keyboard indicators adc
dmg.depends = ide
