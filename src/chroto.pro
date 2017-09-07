
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chroto
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp mainwindow.cpp ./easyexif/exif.cpp

HEADERS += mainwindow.h ./easyexif/exif.h ./date/date.h

FORMS   += mainwindow.ui

RESOURCES += qdarkstyle/style.qrc

LIBS += -L"$$_PRO_FILE_PWD_/exiv2" -lexiv2

QMAKE_CXXFLAGS += -DWITHEXIV2
#QMAKE_CXXFLAGS_RELEASE += -DWITHEXIV2
