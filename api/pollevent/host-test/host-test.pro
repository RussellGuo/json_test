TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../include
HEADERS += ../include/poll_event_api.h ../include/timer_api.h
SOURCES += \
    ../main.c \
    ../poll_event_api.cpp \
    ../timer_api.cpp
