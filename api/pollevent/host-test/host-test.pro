TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../include
HEADERS += ../include/poll_event_api.h ../include/timer_api.h

LIBS += -lpthread
SOURCES += \
    ../poll_event_api.cpp \
    ../timer_api.cpp \
    ../thread_notify.cpp \
    ../poll_event_demo_main.cpp
