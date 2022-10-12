TEMPLATE = app

TARGET = CQChartsTest

QT += widgets svg xml opengl

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

CONFIG += c++17
CONFIG += silent

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
\
CQTclCmd.cpp \
\
CQDragon3DModel.cpp \
CQLeaf3DModel.cpp \
CQLorenzModel.cpp \
CQPickoverModel.cpp \
\
CDragon3DCalc.cpp \
CLeaf3DCalc.cpp \
CLorenzCalc.cpp \
CPickoverCalc.cpp \

HEADERS += \
CQAppWindow.h \
CQChartsAppWindow.h \
CQChartsCmdArg.h \
CQChartsCmdArgs.h \
CQChartsCmdBase.h \
CQChartsCmdGroup.h \
CQChartsCmds.h \
CQChartsCmdsSlot.h \
CQChartsInitData.h \
CQChartsInput.h \
CQChartsNameValueData.h \
CQChartsReadLine.h \
CQChartsTest.h \
CQChartsMetaObj.h \
\
CQTclCmd.h \
\
CQDragon3DModel.h \
CQLeaf3DModel.h \
CQLorenzModel.h \
CQPickoverModel.h \
\
CDragon3DCalc.h \
CLeaf3DCalc.h \
CLorenzCalc.h \
CPickoverCalc.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \
../../CQPropertyView/include \
../../CQBaseModel/include \
../../CQColors/include \
../../CQUtil/include \
../../CQPerfMonitor/include \
../../CImageLib/include \
../../CFont/include \
../../CReadLine/include \
../../CConfig/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \
/usr/include/tcl \

PRE_TARGETDEPS = \
../lib/libCQCharts.a \
../../CQPropertyView/lib/libCQPropertyView.a \
../../CQModelView/lib/libCQModelView.a \
../../CQColors/lib/libCQColors.a \

unix:LIBS += \
-L../lib \
-L../../CQPropertyView/lib \
-L../../CQModelView/lib \
-L../../CQBaseModel/lib \
-L../../CQColors/lib \
-L../../CQCustomCombo/lib \
-L../../CQUtil/lib \
-L../../CJson/lib \
-L../../CCsv/lib \
-L../../CTsv/lib \
-L../../CXML/lib \
-L../../CQPerfMonitor/lib \
-L../../CQDot/lib \
-L../../CQDot/graphviz/lib \
-L../../CConfig/lib \
-L../../CCommand/lib \
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
-lCQCharts -lCQPropertyView -lCQModelView -lCQBaseModel -lCQColors \
-lCQCustomCombo -lCQUtil -lCQPerfMonitor -lCQGraphViz -lCGraphViz -lCJson -lCCsv -lCTsv \
-lCXML -lCCommand -lCConfig -lCImageLib -lCFont -lCMath \
-lCReadLine -lCFileUtil -lCFile -lCRegExp \
-lCUtil -lCStrUtil -lCOS \
-lreadline -lpng -ljpeg -ltre -ltcl -lcurses \
-lGLU -lglut
