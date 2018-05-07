TEMPLATE = app

TARGET = CQChartsTest

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

SOURCES += \
CQChartsTest.cpp \
CQChartsCmds.cpp \
CQAppWindow.cpp \

HEADERS += \
CQChartsTest.h \
CQChartsCmds.h \
CQAppWindow.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
../../CExpr/include \
../../CImageLib/include \
../../CFont/include \
../../CReadLine/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \

unix:LIBS += \
-L../lib \
-L../../CQPropertyView/lib \
-L../../CQUtil/lib \
-L../../CExpr/lib \
-L../../CConfig/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CMath/lib \
-L../../CReadLine/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CUtil/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-L../../COS/lib \
-lCQCharts -lCQPropertyView -lCQUtil \
-lCExpr -lCConfig -lCImageLib -lCFont -lCMath \
-lCReadLine -lCFileUtil -lCFile -lCRegExp -lCStrUtil \
-lCUtil -lCOS \
-lreadline -lpng -ljpeg -ltre
