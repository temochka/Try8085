CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/target/debug
} else {
    DESTDIR = $$PWD/target/release
}

CONFIG(release) {
    CONFIG += static
}

PROJECT_ROOT = $$PWD
TRY8085_MODULE_DESTDIR = $$DESTDIR/module
TRY8085_LIB_DESTDIR = $$DESTDIR/lib

DISTFILES += COPYING

GPLV3_TEXT.files = COPYING
GPLV3_TEXT.path = Contents/Resources

QMAKE_BUNDLE_DATA += \
    GPLV3_TEXT

GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

macx {
    DEFINES += MAC_OS_X
}
