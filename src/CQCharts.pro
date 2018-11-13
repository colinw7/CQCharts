TEMPLATE = lib

TARGET = CQCharts

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++14 \
-DCQCharts_USE_TCL \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

CONFIG += staticlib

SOURCES += \
CQCharts.cpp \
\
CQChartsFilterModel.cpp \
CQChartsExprModel.cpp \
CQChartsExprModelFn.cpp \
\
CQChartsColumn.cpp \
CQChartsColumnType.cpp \
\
CQChartsTable.cpp \
CQChartsTree.cpp \
CQChartsHeader.cpp \
CQChartsTableDelegate.cpp \
\
CQChartsWindow.cpp \
CQChartsView.cpp \
CQChartsViewExpander.cpp \
CQChartsViewSettings.cpp \
CQChartsViewStatus.cpp \
CQChartsViewToolBar.cpp \
CQChartsViewToolTip.cpp \
CQChartsProbeBand.cpp \
\
CQChartsPlotType.cpp \
CQChartsPlotParameter.cpp \
CQChartsPlot.cpp \
CQChartsHierPlot.cpp \
CQChartsHierPlotType.cpp \
CQChartsGroupPlot.cpp \
CQChartsBarPlot.cpp \
CQChartsAdjacencyPlot.cpp \
CQChartsBarChartPlot.cpp \
CQChartsBoxPlot.cpp \
CQChartsBubblePlot.cpp \
CQChartsChordPlot.cpp \
CQChartsDelaunayPlot.cpp \
CQChartsDendrogramPlot.cpp \
CQChartsDistributionPlot.cpp \
CQChartsForceDirectedPlot.cpp \
CQChartsGeometryPlot.cpp \
CQChartsHierBubblePlot.cpp \
CQChartsHierScatterPlot.cpp \
CQChartsImagePlot.cpp \
CQChartsParallelPlot.cpp \
CQChartsPiePlot.cpp \
CQChartsRadarPlot.cpp \
CQChartsSankeyPlot.cpp \
CQChartsScatterPlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTreeMapPlot.cpp \
CQChartsXYPlot.cpp \
CQChartsLayer.cpp \
\
CQChartsNoDataObj.cpp \
CQChartsPlotObj.cpp \
CQChartsPlotObjTree.cpp \
CQChartsBoxObj.cpp \
CQChartsRotatedTextBoxObj.cpp \
CQChartsTextBoxObj.cpp \
CQChartsObj.cpp \
CQChartsDataLabel.cpp \
\
CQChartsAxis.cpp \
CQChartsKey.cpp \
CQChartsTitle.cpp \
CQChartsTheme.cpp \
CQChartsAnnotation.cpp \
CQChartsArrow.cpp \
CQChartsEditHandles.cpp \
CQChartsResizeHandle.cpp \
\
CQChartsColumnBucket.cpp \
CQChartsValueSet.cpp \
CQChartsColorSet.cpp \
CQChartsPlotSymbol.cpp \
\
CQChartsLoadDlg.cpp \
CQChartsPlotDlg.cpp \
CQChartsLoader.cpp \
CQChartsModelDlg.cpp \
CQChartsModelWidgets.cpp \
CQChartsModelList.cpp \
CQChartsModelControl.cpp \
CQChartsModelView.cpp \
CQChartsEval.cpp \
\
CQChartsFilterEdit.cpp \
\
CQChartsModelData.cpp \
CQChartsModelDetails.cpp \
CQChartsModelExprMatch.cpp \
CQChartsModelFilter.cpp \
CQChartsModelVisitor.cpp \
CQChartsColumnEdit.cpp \
\
CQChartsInvalidator.cpp \
\
CQChartsGradientPaletteCanvas.cpp \
CQChartsGradientPalette.cpp \
CQChartsGradientPaletteControl.cpp \
\
CQChartsLineDash.cpp \
CQChartsLineDashEdit.cpp \
\
CQChartsRotatedText.cpp \
CQChartsRoundedPolygon.cpp \
\
CQChartsColor.cpp \
CQChartsNamePair.cpp \
CQChartsPolygonList.cpp \
CQChartsPosition.cpp \
CQChartsLength.cpp \
CQChartsRect.cpp \
CQChartsPlotMargin.cpp \
CQChartsConnectionList.cpp \
CQChartsSides.cpp \
CQChartsFillUnder.cpp \
CQChartsKeyLocation.cpp \
\
CQChartsDrawUtil.cpp \
CQChartsSymbol.cpp \
CQChartsPath.cpp \
CQChartsFillPattern.cpp \
CQChartsStyle.cpp \
CQChartsBoxWhisker.cpp \
CQChartsDensity.cpp \
CQChartsGrahamHull.cpp \
CQChartsUtil.cpp \
CQChartsEnv.cpp \
\
CQChartsAnalyzeFile.cpp \
CQChartsAnalyzeModel.cpp \
\
CQCsvModel.cpp \
CQTsvModel.cpp \
CQJsonModel.cpp \
CQGnuDataModel.cpp \
CQSortModel.cpp \
CQFoldedModel.cpp \
CQDataModel.cpp \
CQBaseModel.cpp \
\
CQSummaryModel.cpp \
CQSubSetModel.cpp \
CQTransposeModel.cpp \
CQBucketer.cpp \
\
CQHistoryLineEdit.cpp \
CQFilename.cpp \
CQRangeScroll.cpp \
\
CQChartsDelaunay.cpp \
CQChartsDendrogram.cpp \
CQChartsHull3D.cpp \
\
CLeastSquaresFit.cpp \
CInterval.cpp \

HEADERS += \
../include/CQCharts.h \
\
../include/CQChartsFilterModel.h \
../include/CQChartsExprModel.h \
../include/CQChartsExprModelFn.h \
\
../include/CQChartsColumn.h \
../include/CQChartsColumnType.h \
\
../include/CQChartsTable.h \
../include/CQChartsTree.h \
../include/CQChartsHeader.h \
../include/CQChartsTableDelegate.h \
\
../include/CQChartsWindow.h \
../include/CQChartsView.h \
../include/CQChartsViewExpander.h \
../include/CQChartsViewSettings.h \
../include/CQChartsViewStatus.h \
../include/CQChartsViewToolBar.h \
../include/CQChartsViewToolTip.h \
../include/CQChartsProbeBand.h \
\
../include/CQChartsPlotType.h \
../include/CQChartsPlotParameter.h \
../include/CQChartsPlot.h \
../include/CQChartsHierPlot.h \
../include/CQChartsHierPlotType.h \
../include/CQChartsGroupPlot.h \
../include/CQChartsBarPlot.h \
../include/CQChartsAdjacencyPlot.h \
../include/CQChartsBarChartPlot.h \
../include/CQChartsBoxPlot.h \
../include/CQChartsBubblePlot.h \
../include/CQChartsChordPlot.h \
../include/CQChartsDelaunayPlot.h \
../include/CQChartsDendrogramPlot.h \
../include/CQChartsDistributionPlot.h \
../include/CQChartsForceDirectedPlot.h \
../include/CQChartsGeometryPlot.h \
../include/CQChartsHierBubblePlot.h \
../include/CQChartsHierScatterPlot.h \
../include/CQChartsImagePlot.h \
../include/CQChartsParallelPlot.h \
../include/CQChartsPiePlot.h \
../include/CQChartsRadarPlot.h \
../include/CQChartsSankeyPlot.h \
../include/CQChartsScatterPlot.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTreeMapPlot.h \
../include/CQChartsXYPlot.h \
../include/CQChartsLayer.h \
\
../include/CQChartsNoDataObj.h \
../include/CQChartsPlotObj.h \
../include/CQChartsPlotObjTree.h \
../include/CQChartsBoxObj.h \
../include/CQChartsRotatedTextBoxObj.h \
../include/CQChartsTextBoxObj.h \
../include/CQChartsObj.h \
../include/CQChartsDataLabel.h \
\
../include/CQChartsAxis.h \
../include/CQChartsKey.h \
../include/CQChartsTitle.h \
../include/CQChartsTheme.h \
../include/CQChartsAnnotation.h \
../include/CQChartsArrow.h \
../include/CQChartsEditHandles.h \
../include/CQChartsResizeHandle.h \
\
../include/CQChartsColumnBucket.h \
../include/CQChartsValueSet.h \
../include/CQChartsColorSet.h \
../include/CQChartsPlotSymbol.h \
../include/CQChartsSymbol.h \
../include/CQChartsPath.h \
../include/CQChartsFillPattern.h \
../include/CQChartsStyle.h \
../include/CQChartsBoxWhisker.h \
../include/CQChartsDensity.h \
../include/CQChartsGrahamHull.h \
../include/CQChartsUtil.h \
../include/CQChartsEnv.h \
../include/CQChartsQuadTree.h \
\
../include/CQChartsAnalyzeFile.h \
../include/CQChartsAnalyzeModel.h \
\
../include/CQChartsLoadDlg.h \
../include/CQChartsPlotDlg.h \
../include/CQChartsLoader.h \
../include/CQChartsModelDlg.h \
../include/CQChartsModelWidgets.h \
../include/CQChartsModelList.h \
../include/CQChartsModelControl.h \
../include/CQChartsModelView.h \
../include/CQChartsEval.h \
\
../include/CQChartsFilterEdit.h \
\
../include/CQChartsModelData.h \
../include/CQChartsModelDetails.h \
../include/CQChartsModelExprMatch.h \
../include/CQChartsModelFilter.h \
../include/CQChartsModelVisitor.h \
../include/CQChartsColumnEdit.h \
\
../include/CQChartsObjData.h \
../include/CQChartsData.h \
../include/CQChartsInvalidator.h \
\
../include/CQChartsGradientPaletteCanvas.h \
../include/CQChartsGradientPalette.h \
../include/CQChartsGradientPaletteControl.h \
\
../include/CQChartsLineDash.h \
../include/CQChartsLineDashEdit.h \
\
../include/CQChartsRotatedText.h \
../include/CQChartsRoundedPolygon.h \
\
../include/CQChartsColor.h \
../include/CQChartsNamePair.h \
../include/CQChartsPolygonList.h \
../include/CQChartsPosition.h \
../include/CQChartsLength.h \
../include/CQChartsRect.h \
../include/CQChartsPlotMargin.h \
../include/CQChartsConnectionList.h \
../include/CQChartsSides.h \
../include/CQChartsFillUnder.h \
../include/CQChartsKeyLocation.h \
\
../include/CQChartsDrawUtil.h \
\
../include/CQCsvModel.h \
../include/CQTsvModel.h \
../include/CQJsonModel.h \
../include/CQGnuDataModel.h \
../include/CQSortModel.h \
../include/CQFoldedModel.h \
../include/CQDataModel.h \
../include/CQBaseModel.h \
../include/CQSummaryModel.h \
../include/CQSubSetModel.h \
../include/CQTransposeModel.h \
../include/CQBucketer.h \
\
../include/CQHistoryLineEdit.h \
CQFilename.h \
../include/CQRangeScroll.h \
\
../include/CQChartsDelaunay.h \
../include/CQChartsDendrogram.h \
../include/CQChartsHull3D.h \
CListLink.h \
CStateIterator.h \
../include/CUnixFile.h \
\
../include/CLeastSquaresFit.h \
../include/CInterval.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQPropertyView/include \
../../CQDividedArea/include \
../../CQCustomCombo/include \
../../CQUtil/include \
../../CJson/include \
../../CTsv/include \
../../CCsv/include \
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
