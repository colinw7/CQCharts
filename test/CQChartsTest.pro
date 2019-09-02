TEMPLATE = app

TARGET = CQChartsTest

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++14 \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

SOURCES += \
CQAppWindow.cpp \
CQChartsAppWindow.cpp \
CQChartsCmdArg.cpp \
CQChartsCmdArgs.cpp \
CQChartsCmdBase.cpp \
CQChartsCmdGroup.cpp \
CQChartsCmds.cpp \
CQChartsCmdsSlot.cpp \
CQChartsInitData.cpp \
CQChartsReadLine.cpp \
CQChartsTest.cpp \

HEADERS += \
CQAppWindow.h \
CQChartsAppWindow.h \
CQChartsCmdArg.h \
CQChartsCmdArgs.h \
CQChartsCmdBase.h \
CQChartsCmdGroup.h \
CQChartsCmdsArgs.h \
CQChartsCmds.h \
CQChartsCmdsSlot.h \
CQChartsInitData.h \
CQChartsInput.h \
CQChartsNameValueData.h \
CQChartsPaletteColorData.h \
CQChartsReadLine.h \
CQChartsTest.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \
../../CQPropertyView/include \
../../CQColors/include \
../../CQUtil/include \
../../CQPerfMonitor/include \
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
-L../../CQModelView/lib \
-L../../CQColors/lib \
-L../../CQDividedArea/lib \
-L../../CQCustomCombo/lib \
-L../../CQUtil/lib \
-L../../CJson/lib \
-L../../CQPerfMonitor/lib \
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
-lCQCharts -lCQPropertyView -lCQModelView -lCQColors \
-lCQDividedArea -lCQCustomCombo -lCQUtil -lCQPerfMonitor -lCJson \
-lCConfig -lCImageLib -lCFont -lCMath \
-lCReadLine -lCFileUtil -lCFile -lCRegExp \
-lCUtil -lCStrUtil -lCOS \
-lreadline -lpng -ljpeg -ltre -ltcl
