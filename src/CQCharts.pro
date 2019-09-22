TEMPLATE = lib

TARGET = CQCharts

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++14 \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

CONFIG += staticlib
CONFIG += c++14

SOURCES += \
CQCharts.cpp \
\
CQChartsFilterModel.cpp \
CQChartsExprModel.cpp \
CQChartsExprModelFn.cpp \
CQChartsVarsModel.cpp \
CQChartsExprDataModel.cpp \
CQChartsSelectionModel.cpp \
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
CQChartsPointPlot.cpp \
\
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
CQChartsPivotPlot.cpp \
CQChartsRadarPlot.cpp \
CQChartsSankeyPlot.cpp \
CQChartsScatterPlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTreeMapPlot.cpp \
CQChartsXYPlot.cpp \
\
CQChartsLayer.cpp \
CQChartsBuffer.cpp \
\
CQChartsNoDataObj.cpp \
CQChartsPlotObj.cpp \
CQChartsViewPlotObj.cpp \
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
CQChartsAnnotation.cpp \
CQChartsArrow.cpp \
CQChartsEditHandles.cpp \
CQChartsResizeHandle.cpp \
\
CQChartsInterfaceTheme.cpp \
CQChartsThemeName.cpp \
\
CQChartsColumnBucket.cpp \
CQChartsValueSet.cpp \
CQChartsPlotSymbol.cpp \
\
CQChartsCreateAnnotationDlg.cpp \
CQChartsCreatePlotDlg.cpp \
CQChartsEditAnnotationDlg.cpp \
CQChartsLoadModelDlg.cpp \
CQChartsManageModelsDlg.cpp \
CQChartsEditModelDlg.cpp \
CQChartsModelDataWidget.cpp \
CQChartsHelpDlg.cpp \
\
CQChartsModelWidgets.cpp \
CQChartsModelList.cpp \
CQChartsModelControl.cpp \
CQChartsModelDetailsWidget.cpp \
\
CQChartsLoader.cpp \
\
CQChartsModelViewHolder.cpp \
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
\
CQChartsData.cpp \
CQChartsInvalidator.cpp \
\
CQChartsLineDash.cpp \
\
CQChartsRotatedText.cpp \
CQChartsRoundedPolygon.cpp \
\
CQChartsOptInt.cpp \
CQChartsOptLength.cpp \
CQChartsOptPosition.cpp \
CQChartsOptReal.cpp \
CQChartsOptRect.cpp \
\
CQChartsColor.cpp \
CQChartsColorStops.cpp \
CQChartsFont.cpp \
CQChartsNamePair.cpp \
CQChartsPolygonList.cpp \
CQChartsPosition.cpp \
CQChartsLength.cpp \
CQChartsRect.cpp \
CQChartsPolygon.cpp \
CQChartsPlotMargin.cpp \
CQChartsConnectionList.cpp \
CQChartsSides.cpp \
CQChartsFillUnder.cpp \
CQChartsPaletteName.cpp \
\
CQChartsSymbol.cpp \
CQChartsPath.cpp \
CQChartsStyle.cpp \
CQChartsBoxWhisker.cpp \
CQChartsDensity.cpp \
CQChartsGrahamHull.cpp \
\
CQChartsAxisSide.cpp \
CQChartsAxisTickLabelPlacement.cpp \
CQChartsAxisValueType.cpp \
CQChartsFillPattern.cpp \
CQChartsKeyLocation.cpp \
CQChartsKeyPressBehavior.cpp \
CQChartsTitleLocation.cpp \
\
CQChartsModelUtil.cpp \
CQChartsValueInd.cpp \
CQChartsNameValues.cpp \
CQChartsEnv.cpp \
\
CQChartsPaintDevice.cpp \
CQChartsDrawUtil.cpp \
CQChartsUtil.cpp \
CQChartsWidgetUtil.cpp \
\
CQChartsAnalyzeFile.cpp \
CQChartsAnalyzeModel.cpp \
\
CQChartsDelaunay.cpp \
CQChartsDendrogram.cpp \
CQChartsHull3D.cpp \
\
CQChartsTitleEdit.cpp \
CQChartsKeyEdit.cpp \
CQChartsAxisEdit.cpp \
\
CQChartsAlphaEdit.cpp \
CQChartsArrowDataEdit.cpp \
CQChartsAxisSideEdit.cpp \
CQChartsAxisTickLabelPlacementEdit.cpp \
CQChartsAxisValueTypeEdit.cpp \
CQChartsBoxDataEdit.cpp \
CQChartsColorEdit.cpp \
CQChartsColumnEdit.cpp \
CQChartsColumnsEdit.cpp \
CQChartsEditBase.cpp \
CQChartsFillDataEdit.cpp \
CQChartsFillPatternEdit.cpp \
CQChartsFillUnderEdit.cpp \
CQChartsFontEdit.cpp \
CQChartsKeyLocationEdit.cpp \
CQChartsKeyPressBehaviorEdit.cpp \
CQChartsLengthEdit.cpp \
CQChartsLineDashEdit.cpp \
CQChartsLineDataEdit.cpp \
CQChartsLineEditBase.cpp \
CQChartsPaletteNameEdit.cpp \
CQChartsPolygonEdit.cpp \
CQChartsPositionEdit.cpp \
CQChartsRectEdit.cpp \
CQChartsShapeDataEdit.cpp \
CQChartsSidesEdit.cpp \
CQChartsStrokeDataEdit.cpp \
CQChartsSymbolDataEdit.cpp \
CQChartsSymbolEdit.cpp \
CQChartsTextBoxDataEdit.cpp \
CQChartsTextDataEdit.cpp \
CQChartsTitleLocationEdit.cpp \
CQChartsUnitsEdit.cpp \
\
CQChartsEnumEdit.cpp \
\
CQChartsPropertyViewTree.cpp \
CQChartsPropertyViewEditor.cpp \
\
CQCsvModel.cpp \
CQTsvModel.cpp \
CQJsonModel.cpp \
CQGnuDataModel.cpp \
CQFoldedModel.cpp \
\
CQBaseModel.cpp \
CQDataModel.cpp \
CQModelDetails.cpp \
CQModelNameValues.cpp \
CQModelUtil.cpp \
CQModelVisitor.cpp \
CQSortModel.cpp \
CQValueSet.cpp \
\
CQSummaryModel.cpp \
CQSubSetModel.cpp \
CQTransposeModel.cpp \
CQBucketModel.cpp \
CQCollapseModel.cpp \
CQPivotModel.cpp \
CQBucketer.cpp \
CQTrie.cpp \
\
CQTabSplit.cpp \
CQFilename.cpp \
CQRangeScroll.cpp \
\
CQCheckBox.cpp \
CLeastSquaresFit.cpp \

HEADERS += \
../include/CQCharts.h \
\
../include/CQChartsFilterModel.h \
../include/CQChartsExprModel.h \
../include/CQChartsExprModelFn.h \
../include/CQChartsVarsModel.h \
../include/CQChartsExprDataModel.h \
../include/CQChartsSelectionModel.h \
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
../include/CQChartsPointPlot.h \
\
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
../include/CQChartsPivotPlot.h \
../include/CQChartsRadarPlot.h \
../include/CQChartsSankeyPlot.h \
../include/CQChartsScatterPlot.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTreeMapPlot.h \
../include/CQChartsXYPlot.h \
\
../include/CQChartsLayer.h \
../include/CQChartsBuffer.h \
\
../include/CQChartsNoDataObj.h \
../include/CQChartsPlotObj.h \
../include/CQChartsViewPlotObj.h \
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
../include/CQChartsAnnotation.h \
../include/CQChartsArrow.h \
../include/CQChartsEditHandles.h \
../include/CQChartsResizeHandle.h \
\
../include/CQChartsInterfaceTheme.h \
../include/CQChartsThemeName.h \
\
../include/CQChartsColumnBucket.h \
../include/CQChartsValueSet.h \
../include/CQChartsPlotSymbol.h \
../include/CQChartsSymbol.h \
../include/CQChartsPath.h \
../include/CQChartsStyle.h \
../include/CQChartsBoxWhisker.h \
../include/CQChartsDensity.h \
../include/CQChartsGrahamHull.h \
\
../include/CQChartsFillPattern.h \
../include/CQChartsTitleLocation.h \
../include/CQChartsAxisSide.h \
../include/CQChartsAxisTickLabelPlacement.h \
../include/CQChartsAxisValueType.h \
../include/CQChartsKeyLocation.h \
../include/CQChartsKeyPressBehavior.h \
\
../include/CQChartsModelUtil.h \
../include/CQChartsValueInd.h \
../include/CQChartsNameValues.h \
../include/CQChartsQuadTree.h \
../include/CQChartsEnv.h \
\
../include/CQChartsPaintDevice.h \
../include/CQChartsDrawUtil.h \
../include/CQChartsUtil.h \
../include/CQChartsWidgetUtil.h \
\
../include/CQChartsAnalyzeFile.h \
../include/CQChartsAnalyzeModel.h \
\
../include/CQChartsCreateAnnotationDlg.h \
../include/CQChartsCreatePlotDlg.h \
../include/CQChartsEditAnnotationDlg.h \
../include/CQChartsLoadModelDlg.h \
../include/CQChartsManageModelsDlg.h \
../include/CQChartsEditModelDlg.h \
../include/CQChartsModelDataWidget.h \
../include/CQChartsHelpDlg.h \
\
../include/CQChartsModelWidgets.h \
../include/CQChartsModelList.h \
../include/CQChartsModelControl.h \
../include/CQChartsModelDetailsWidget.h \
\
../include/CQChartsLoader.h \
\
../include/CQChartsModelViewHolder.h \
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
\
../include/CQChartsObjData.h \
../include/CQChartsData.h \
../include/CQChartsInvalidator.h \
\
../include/CQChartsLineDash.h \
\
../include/CQChartsRotatedText.h \
../include/CQChartsRoundedPolygon.h \
\
../include/CQChartsColor.h \
../include/CQChartsColorStops.h \
../include/CQChartsFont.h \
../include/CQChartsNamePair.h \
../include/CQChartsPolygonList.h \
../include/CQChartsPosition.h \
../include/CQChartsLength.h \
../include/CQChartsRect.h \
../include/CQChartsPolygon.h \
../include/CQChartsPlotMargin.h \
../include/CQChartsConnectionList.h \
../include/CQChartsSides.h \
../include/CQChartsFillUnder.h \
../include/CQChartsPaletteName.h \
\
../include/CQChartsOptInt.h \
../include/CQChartsOptLength.h \
../include/CQChartsOptPosition.h \
../include/CQChartsOptReal.h \
../include/CQChartsOptRect.h \
\
../include/CQChartsDelaunay.h \
../include/CQChartsDendrogram.h \
../include/CQChartsHull3D.h \
\
../include/CQChartsTitleEdit.h \
../include/CQChartsKeyEdit.h \
../include/CQChartsAxisEdit.h \
\
../include/CQChartsAlphaEdit.h \
../include/CQChartsArrowDataEdit.h \
../include/CQChartsAxisSideEdit.h \
../include/CQChartsAxisTickLabelPlacementEdit.h \
../include/CQChartsAxisValueTypeEdit.h \
../include/CQChartsBoxDataEdit.h \
../include/CQChartsColorEdit.h \
../include/CQChartsColumnEdit.h \
../include/CQChartsColumnsEdit.h \
../include/CQChartsEditBase.h \
../include/CQChartsFillDataEdit.h \
../include/CQChartsFillPatternEdit.h \
../include/CQChartsFillUnderEdit.h \
../include/CQChartsFontEdit.h \
../include/CQChartsKeyLocationEdit.h \
../include/CQChartsKeyPressBehaviorEdit.h \
../include/CQChartsLengthEdit.h \
../include/CQChartsLineDashEdit.h \
../include/CQChartsLineDataEdit.h \
../include/CQChartsLineEditBase.h \
../include/CQChartsPaletteNameEdit.h \
../include/CQChartsPolygonEdit.h \
../include/CQChartsPositionEdit.h \
../include/CQChartsRectEdit.h \
../include/CQChartsShapeDataEdit.h \
../include/CQChartsSidesEdit.h \
../include/CQChartsStrokeDataEdit.h \
../include/CQChartsSymbolDataEdit.h \
../include/CQChartsSymbolEdit.h \
../include/CQChartsTextBoxDataEdit.h \
../include/CQChartsTextDataEdit.h \
../include/CQChartsTitleLocationEdit.h \
../include/CQChartsUnitsEdit.h \
\
../include/CQChartsEnumEdit.h \
\
../include/CQChartsPropertyViewTree.h \
../include/CQChartsPropertyViewEditor.h \
\
../include/CQCsvModel.h \
../include/CQTsvModel.h \
../include/CQJsonModel.h \
../include/CQGnuDataModel.h \
../include/CQFoldedModel.h \
../include/CQSummaryModel.h \
../include/CQSubSetModel.h \
../include/CQTransposeModel.h \
../include/CQBucketModel.h \
../include/CQCollapseModel.h \
../include/CQPivotModel.h \
../include/CQBucketer.h \
../include/CQTrie.h \
\
../include/CQBaseModel.h \
../include/CQBaseModelTypes.h \
../include/CQDataModel.h \
../include/CQModelDetails.h \
../include/CQModelUtil.h \
../include/CQModelVisitor.h \
../include/CQSortModel.h \
../include/CQStatData.h \
../include/CQValueSet.h \
\
../include/CQTabSplit.h \
../include/CQFilename.h \
../include/CQRangeScroll.h \
\
../include/CUnixFile.h \
\
../include/CQCheckBox.h \
../include/CLeastSquaresFit.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQPropertyView/include \
../../CQModelView/include \
../../CQColors/include \
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
