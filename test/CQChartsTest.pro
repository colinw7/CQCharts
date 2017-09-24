TEMPLATE = app

TARGET = CQChartsTest

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
CQChartsTest.cpp \
CQAppWindow.cpp \

HEADERS += \
CQChartsTest.h \
CQAppWindow.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \

unix:LIBS += \
-L../lib \
-lCQCharts
