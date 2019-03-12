#-------------------------------------------------
#
# Project created by QtCreator 2017-01-14T12:56:49
#
#-------------------------------------------------

QT       += network widgets\
           xml


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Launch
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    operatefile.cpp \
    heartbeatserver.cpp \
    heartbeatthread.cpp

HEADERS  += widget.h \
    operatefile.h \
    heartbeatserver.h \
    heartbeatthread.h

FORMS    += widget.ui
RC_FILE = launch.rc
