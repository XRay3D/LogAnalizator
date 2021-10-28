HEADERS += $$PWD\hidapi\hidapi\hidapi.h

INCLUDEPATH += $$PWD\hidapi\hidapi

macx {
    SOURCES += $$PWD\hidapi\mac\hid.c
    LIBS += -framework CoreFoundation -framework IOkit
#    DESTDIR = mac
#    OBJECTS_DIR = mac
#    MOC_DIR = mac
#    UI_DIR = mac
#    RCC_DIR = mac
}
unix: !macx {
    SOURCES += $$PWD\hidapi\linux\hid.c
    LIBS += -lusb-1.0
#    DESTDIR = linux
#    OBJECTS_DIR = linux
#    MOC_DIR = linux
#    UI_DIR = linux
#    RCC_DIR = linux
}
win32 {
    SOURCES += $$PWD\hidapi\windows\hid.c
    LIBS += -lSetupAPI
#    DESTDIR = windows
#    OBJECTS_DIR = windows
#    MOC_DIR = windows
#    UI_DIR = windows
#    RCC_DIR = windows
}
