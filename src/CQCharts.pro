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
CQChartsAxis.cpp \
CQChartsBarChartPlot.cpp \
CQChartsBoxObj.cpp \
CQChartsBoxPlot.cpp \
CQChartsBubblePlot.cpp \
CQChartsColumn.cpp \
CQChartsCsv.cpp \
CQChartsData.cpp \
CQChartsDataLabel.cpp \
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
CQChartsPlotSymbol.cpp \
CQChartsProbeBand.cpp \
CQChartsScatterPlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTable.cpp \
CQChartsTextBoxObj.cpp \
CQChartsTitle.cpp \
CQChartsTree.cpp \
CQChartsTreeMapPlot.cpp \
CQChartsTsv.cpp \
CQChartsView.cpp \
CQChartsViewExpander.cpp \
CQChartsViewSettings.cpp \
CQChartsViewStatus.cpp \
CQChartsViewToolBar.cpp \
CQChartsViewToolTip.cpp \
CQChartsXYPlot.cpp \
\
CQPropertyViewDelegate.cpp \
CQPropertyViewEditor.cpp \
CQPropertyViewFilter.cpp \
CQPropertyViewItem.cpp \
CQPropertyViewModel.cpp \
CQPropertyViewTree.cpp \
\
CQGradientControlIFace.cpp \
CQGradientControlPlot.cpp \
CGradientPalette.cpp \
\
CQCsvModel.cpp \
CQTsvModel.cpp \
CQJsonModel.cpp \
CJson.cpp \
\
CQColorChooser.cpp \
CQAlphaButton.cpp \
CQFontChooser.cpp \
CQBBox2DEdit.cpp \
CQPoint2DEdit.cpp \
CQRealSpin.cpp \
CQToolTip.cpp \
CQPixmapCache.cpp \
CQUtil.cpp \
\
CQHeaderView.cpp \
CQRotatedText.cpp \
CQFilename.cpp \
CQStrParse.cpp \
\
CDelaunay.cpp \
CHull3D.cpp \
COSNaN.cpp \

HEADERS += \
../include/CQCharts.h \
../include/CQChartsAdjacencyPlot.h \
../include/CQChartsAxis.h \
../include/CQChartsBarChartPlot.h \
../include/CQChartsBoxObj.h \
../include/CQChartsBoxPlot.h \
../include/CQChartsBubblePlot.h \
../include/CQChartsColumn.h \
../include/CQChartsCsv.h \
../include/CQChartsData.h \
../include/CQChartsDataLabel.h \
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
../include/CQChartsPlotSymbol.h \
../include/CQChartsProbeBand.h \
../include/CQChartsQuadTree.h \
../include/CQChartsScatterPlot.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTable.h \
../include/CQChartsTextBoxObj.h \
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
../include/CQChartsViewToolTip.h \
../include/CQChartsXYPlot.h \
\
CQPropertyViewDelegate.h \
CQPropertyViewEditor.h \
CQPropertyViewFilter.h \
CQPropertyViewItem.h \
CQPropertyViewModel.h \
CQPropertyViewTree.h \
\
CQGradientControlIFace.h \
CQGradientControlPlot.h \
CGradientPalette.h \
\
../include/CQCsvModel.h \
../include/CQTsvModel.h \
../include/CQJsonModel.h \
../include/CJson.h \
\
CQAlphaButton.h \
CQBBox2DEdit.h \
CQColorChooser.h \
CQFilename.h \
CQFontChooser.h \
CQHeaderView.h \
CQPixmapCache.h \
CQPoint2DEdit.h \
CQRealSpin.h \
CQRotatedText.h \
CQStrParse.h \
CQToolTip.h \
../include/CQUtil.h \
\
CDelaunay.h \
CHull3D.h \
CListLink.h \
CStateIterator.h \
../include/CUnixFile.h \
../include/COSNaN.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
