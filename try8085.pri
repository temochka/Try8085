CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/target/debug
} else {
    DESTDIR = $$PWD/target/release
}

CONFIG(release) {
    CONFIG += static
}

TRY8085_MODULE_DESTDIR = $$DESTDIR/module
TRY8085_LIB_DESTDIR = $$DESTDIR/lib
