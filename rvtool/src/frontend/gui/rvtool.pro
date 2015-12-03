#-------------------------------------------------
#
# Project created by QtCreator 2012-09-23T17:07:12
#
#-------------------------------------------------

QT       += core gui


QWT_LOCATION = /home/ramymedhat/qwt-6.1-rc3

INCLUDEPATH += $$/usr/local/qwt-6.1.0-rc3/include/

LIBS = -L/usr/local/qwt-6.1.0-rc3/lib -lqwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rvtool
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    settingswindow.cpp \
    cpustat.cpp \
    cpuplot.cpp \
    logdialog.cpp

HEADERS  += mainwindow.h \
    settingswindow.h \
    cpustat.h \
    cpuplot.h \
    logdialog.h

FORMS    += mainwindow.ui \
    settingswindow.ui \
    logdialog.ui

OTHER_FILES +=
