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
CQChartsBoxPlot.cpp \
CQCharts.cpp \
CQChartsCsv.cpp \
CQChartsData.cpp \
CQChartsHeader.cpp \
CQChartsJson.cpp \
CQChartsLoader.cpp \
CQChartsModel.cpp \
CQChartsParallelPlot.cpp \
CQChartsPiePlot.cpp \
CQChartsPlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTable.cpp \
CQChartsTree.cpp \
CQChartsTsv.cpp \
CQChartsXYPlot.cpp \
\
CQCsvModel.cpp \
CQHeaderView.cpp \
CQJsonModel.cpp \
CQRotatedText.cpp \
CQTsvModel.cpp \
CQFilename.cpp \

HEADERS += \
../include/CQChartsAxisBase.h \
../include/CQChartsAxis.h \
../include/CQChartsBarChartPlot.h \
../include/CQChartsBoxPlot.h \
../include/CQChartsColumn.h \
../include/CQChartsCsv.h \
../include/CQChartsData.h \
../include/CQCharts.h \
../include/CQChartsHeader.h \
../include/CQChartsJson.h \
../include/CQChartsLoader.h \
../include/CQChartsModel.h \
../include/CQChartsParallelPlot.h \
../include/CQChartsPiePlot.h \
../include/CQChartsPlot.h \
../include/CQChartsPlotObj.h \
../include/CQChartsQuadTree.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTable.h \
../include/CQChartsTree.h \
../include/CQChartsTsv.h \
../include/CQChartsUtil.h \
../include/CQChartsXYPlot.h \
../include/CQCsvModel.h \
../include/CQJsonModel.h \
../include/CSymbolType.h \
../include/CUnixFile.h \
\
CCsv.h \
CParseLine.h \
CQHeaderView.h \
CQRotatedText.h \
CQTsvModel.h \
CQFilename.h \

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
