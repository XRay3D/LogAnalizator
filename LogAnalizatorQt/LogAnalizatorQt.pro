QT += core gui widgets

TARGET = LogAnalizatorQt
TEMPLATE = app

QMAKE_CXXFLAGS += -std:c++latest

SOURCES += \
    main.cpp \
    hid_pnp.cpp \
    MainWindow.cpp

HEADERS  += \
    hid_pnp.h \
    MainWindow.h

FORMS    += \
    MainWindow.ui

include(HIDAPI.pri)
