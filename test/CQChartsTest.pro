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
../../CReadLine/include \
../../CFile/include \
../../CStrUtil/include \

unix:LIBS += \
-L../lib \
-L../../CReadLine/lib \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../COS/lib \
-lCQCharts \
-lCReadLine -lCFile -lCStrUtil \
-lCOS \
-lreadline
