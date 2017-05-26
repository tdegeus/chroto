
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chroto
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp mainwindow.cpp ./easyexif/exif.cpp

HEADERS += mainwindow.h ./easyexif/exif.h

FORMS   += mainwindow.ui
