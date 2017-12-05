TEMPLATE = app

TARGET = CQChartsTest

DEPENDPATH += .

QT += widgets svg

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
CQChartsTest.cpp \
CQAppWindow.cpp \
CQHistoryLineEdit.cpp \

HEADERS += \
CQChartsTest.h \
CQAppWindow.h \
CQHistoryLineEdit.h \

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
