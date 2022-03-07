#-------------------------------------------------
#
# Project created by QtCreator 2022-01-18T00:35:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CampusCardManagementSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        widget.cpp \
    mypushbutton.cpp \
    mytoolbutton.cpp \
    campuscard.cpp \
    campuscardmanagement.cpp \
    rectanglepushbutton.cpp \
    circlepushbutton.cpp \
    dialogrecharge.cpp \
    dialogsetdatetime.cpp \
    canteenwindow.cpp \
    canteenmanagement.cpp \
    operationlog.cpp \
    dialogconsumption.cpp \
    dialogopenaccount.cpp \
    stringheap.cpp

HEADERS += \
        widget.h \
    mypushbutton.h \
    mytoolbutton.h \
    campuscard.h \
    campuscardmanagement.h \
    rectanglepushbutton.h \
    circlepushbutton.h \
    dialogrecharge.h \
    dialogsetdatetime.h \
    canteenwindow.h \
    canteenmanagement.h \
    operationlog.h \
    dialogconsumption.h \
    dialogopenaccount.h \
    stringheap.h

FORMS += \
        widget.ui \
    dialogrecharge.ui \
    dialogsetdatetime.ui \
    dialogconsumption.ui \
    dialogopenaccount.ui

RESOURCES += \
    res.qrc

QMAKE_LFLAGS_WINDOWS += -Wl,--stack,32000000
