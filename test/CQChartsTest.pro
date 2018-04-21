TEMPLATE = app

TARGET = CQChartsTest

DEPENDPATH += .

QT += widgets svg

QMAKE_CXXFLAGS += -std=c++14

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
../../CExpr/include \
../../CReadLine/include \
../../CFile/include \
../../CStrUtil/include \
../../CUtil/include \

unix:LIBS += \
-L../lib \
-L../../CExpr/lib \
-L../../CMath/lib \
-L../../CReadLine/lib \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../COS/lib \
-lCQCharts \
-lCExpr -lCMath \
-lCReadLine -lCFile -lCStrUtil \
-lCOS \
-lreadline
