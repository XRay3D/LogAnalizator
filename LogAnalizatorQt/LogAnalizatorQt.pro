QT += core gui widgets

TARGET = LogAnalizatorQt
TEMPLATE = app

QMAKE_CXXFLAGS += -std:c++latest

SOURCES += \
    graphicsview.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    usbdevice.cpp \
    wire.cpp

HEADERS  += \
    graphicsview.h \
    mainwindow.h \
    model.h \
    usbdevice.h \
    wire.h

FORMS    += \
    mainwindow.ui

include(HIDAPI.pri)
