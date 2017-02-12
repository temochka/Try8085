include("../../try8085.pri")

TARGET = dmg

DISTFILES += \
    package.py

CONFIG-=app_bundle

dmg.path = $$DESTDIR/Try8085_$${GIT_VERSION}
dmg.commands = $$PWD/package.py $$PROJECT_ROOT $$DESTDIR/Try8085.app $$dmg.path

QMAKE_EXTRA_TARGETS += dmg
PRE_TARGETDEPS += dmg

SOURCES += \
    main.cpp
