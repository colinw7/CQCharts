TEMPLATE = app

TARGET = CQChartsTest

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++14 \
-DCQCharts_USE_TCL \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

SOURCES += \
CQChartsTest.cpp \
CQChartsCmds.cpp \
CQChartsAppWindow.cpp \
CQAppWindow.cpp \

HEADERS += \
CQChartsTest.h \
CQChartsCmds.h \
CQChartsAppWindow.h \
CQAppWindow.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
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
-L../../CQCustomCombo/lib \
-L../../CQUtil/lib \
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
-lCQCharts -lCQPropertyView -lCQDividedArea -lCQCustomCombo -lCQUtil \
-lCJson -lCMath \
-lCReadLine -lCFileUtil -lCFile -lCRegExp \
-lCUtil -lCStrUtil -lCOS \
-lreadline -lpng -ljpeg -ltre -ltcl
