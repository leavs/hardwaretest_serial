#-------------------------------------------------
#
# Project created by QtCreator 2017-02-09T22:03:15
#
# NOTE: Modify the qextserialenumerator_linux.cpp
#       Add ttymxc support(155 line).
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hardwaretest_serial
TEMPLATE = app

# The path of target platform
target.path = /usr/bin
INSTALLS += target

include(qextserialport/src/qextserialport.pri)

SOURCES += main.cpp\
        dialog.cpp \
    hled.cpp

HEADERS  += dialog.h \
    hled.h

FORMS    += dialog.ui
