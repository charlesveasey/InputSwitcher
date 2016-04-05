QT += widgets serialport

TARGET = InputSwitcher
TEMPLATE = app

HEADERS += \
    window.h \
    serialthread.h

SOURCES += \
    main.cpp \
    window.cpp \
    serialthread.cpp
