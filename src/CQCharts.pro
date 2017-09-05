TEMPLATE = lib

TARGET = CQCharts

QT += widgets

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

CONFIG += staticlib

SOURCES += \
CQCharts.cpp \
CQChartsAdjacencyPlot.cpp \
CQChartsAxisBase.cpp \
CQChartsAxis.cpp \
CQChartsBarChartPlot.cpp \
CQChartsBoxObj.cpp \
CQChartsBoxPlot.cpp \
CQChartsBubblePlot.cpp \
CQChartsCsv.cpp \
CQChartsData.cpp \
CQChartsDelaunayPlot.cpp \
CQChartsGeometryPlot.cpp \
CQChartsHeader.cpp \
CQChartsHierBubblePlot.cpp \
CQChartsKey.cpp \
CQChartsJson.cpp \
CQChartsLoader.cpp \
CQChartsModel.cpp \
CQChartsParallelPlot.cpp \
CQChartsPiePlot.cpp \
CQChartsPlot.cpp \
CQChartsScatterPlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTable.cpp \
CQChartsTitle.cpp \
CQChartsTree.cpp \
CQChartsTreeMapPlot.cpp \
CQChartsTsv.cpp \
CQChartsView.cpp \
CQChartsViewExpander.cpp \
CQChartsViewSettings.cpp \
CQChartsViewStatus.cpp \
CQChartsViewToolBar.cpp \
CQChartsXYPlot.cpp \
\
CQHeaderView.cpp \
CQRotatedText.cpp \
CQFilename.cpp \
CSymbol2D.cpp \
CDelaunay.cpp \
CHull3D.cpp \

HEADERS += \
../include/CQCharts.h \
../include/CQChartsAdjacencyPlot.h \
../include/CQChartsAxisBase.h \
../include/CQChartsAxis.h \
../include/CQChartsBarChartPlot.h \
../include/CQChartsBoxObj.h \
../include/CQChartsBoxPlot.h \
../include/CQChartsBubblePlot.h \
../include/CQChartsColumn.h \
../include/CQChartsCsv.h \
../include/CQChartsData.h \
../include/CQChartsDelaunayPlot.h \
../include/CQChartsGeometryPlot.h \
../include/CQChartsKey.h \
../include/CQChartsHeader.h \
../include/CQChartsHierBubblePlot.h \
../include/CQChartsJson.h \
../include/CQChartsLoader.h \
../include/CQChartsModel.h \
../include/CQChartsParallelPlot.h \
../include/CQChartsPiePlot.h \
../include/CQChartsPlot.h \
../include/CQChartsPlotObj.h \
../include/CQChartsQuadTree.h \
../include/CQChartsScatterPlot.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTable.h \
../include/CQChartsTitle.h \
../include/CQChartsTree.h \
../include/CQChartsTreeMapPlot.h \
../include/CQChartsTsv.h \
../include/CQChartsUtil.h \
../include/CQChartsView.h \
../include/CQChartsViewExpander.h \
../include/CQChartsViewSettings.h \
../include/CQChartsViewStatus.h \
../include/CQChartsViewToolBar.h \
../include/CQChartsXYPlot.h \
\
../include/CSymbolType.h \
../include/CUnixFile.h \
../include/CSymbol2D.h \
\
CParseLine.h \
CQHeaderView.h \
CQRotatedText.h \
CQFilename.h \
CDelaunay.h \
CHull3D.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CJson/qinclude \
../../CCsv/qinclude \
../../CTsv/qinclude \
../../CQPropertyTree/include \
../../CQGradientPalette/include \
../../CExpr/include \
../../CJson/include \
../../CCsv/include \
../../CQUtil/include \
../../CFont/include \
../../CImageLib/include \
../../CMath/include \
../../CFile/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \
