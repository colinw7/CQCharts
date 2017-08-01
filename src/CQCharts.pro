TEMPLATE = lib

TARGET = CQCharts

QT += widgets

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

CONFIG += staticlib

SOURCES += \
CQChartsAxisBase.cpp \
CQChartsAxis.cpp \
CQChartsBarChartPlot.cpp \
CQCharts.cpp \
CQChartsCsv.cpp \
CQChartsData.cpp \
CQChartsHeader.cpp \
CQChartsJson.cpp \
CQChartsModel.cpp \
CQChartsPiePlot.cpp \
CQChartsPlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTable.cpp \
CQChartsTree.cpp \
CQChartsXYPlot.cpp \
\
CQJsonModel.cpp \
CQCsvModel.cpp \
CQRotatedText.cpp \
CQHeaderView.cpp \

HEADERS += \
../include/CQChartsAxisBase.h \
../include/CQChartsAxis.h \
../include/CQChartsBarChartPlot.h \
../include/CQChartsColumn.h \
../include/CQChartsCsv.h \
../include/CQChartsData.h \
../include/CQCharts.h \
../include/CQChartsHeader.h \
../include/CQChartsJson.h \
../include/CQChartsModel.h \
../include/CQChartsPiePlot.h \
../include/CQChartsPlot.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTable.h \
../include/CQChartsTree.h \
../include/CQChartsUtil.h \
../include/CQChartsXYPlot.h \
\
../include/CQJsonModel.h \
../include/CQCsvModel.h \
CQRotatedText.h \
CQHeaderView.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQPropertyTree/include \
../../CQGradientPalette/include \
../../CExpr/include \
../../CJson/include \
../../CQUtil/include \
../../CFont/include \
../../CImageLib/include \
../../CMath/include \
../../CFile/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \
