TEMPLATE = app

TARGET = CQChartsTest

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++14 -DCQ_CHARTS_TCL

MOC_DIR = .moc

SOURCES += \
CQChartsTest.cpp \
CQChartsModelList.cpp \
CQChartsModelControl.cpp \
CQChartsCmds.cpp \
CQAppWindow.cpp \

HEADERS += \
CQChartsTest.h \
CQChartsModelList.h \
CQChartsModelControl.h \
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
/usr/include/tcl \

unix:LIBS += \
-L../lib \
-L../../CQPropertyView/lib \
-L../../CQDividedArea/lib \
-L../../CQUtil/lib \
-L../../CExpr/lib \
-L../../CJson/lib \
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
-lCQCharts -lCQPropertyView -lCQDividedArea -lCQUtil \
-lCExpr -lCJson -lCConfig -lCImageLib -lCFont -lCMath \
-lCReadLine -lCFileUtil -lCFile -lCRegExp -lCStrUtil \
-lCUtil -lCOS \
-lreadline -lpng -ljpeg -ltre -ltcl
