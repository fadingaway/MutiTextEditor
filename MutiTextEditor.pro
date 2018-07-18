QT += printsupport

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = MutiTextEditor
TEMPLATE = app

HEADERS += \
    mainwindow.h \
    mymdi.h \
    tabdialog.h \

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    mymdi.cpp \
    tabdialog.cpp \
