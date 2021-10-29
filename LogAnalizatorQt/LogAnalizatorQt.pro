QT += core gui widgets

TARGET = LogAnalizatorQt
TEMPLATE = app

QMAKE_CXXFLAGS += -std:c++latest

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    usbdevice.cpp

HEADERS  += \
    MainWindow.h \
    usbdevice.h

FORMS    += \
    MainWindow.ui

include(HIDAPI.pri)
