TEMPLATE = lib

TARGET = CQCharts

QT += widgets svg xml opengl

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQCHARTS_FOLDED_MODEL \
-DCQCHARTS_MODEL_VIEW \
-DCQCHARTS_FLOAT_TIP \
-Wno-maybe-uninitialized \

MOC_DIR = .moc

CONFIG += staticlib
CONFIG += c++17
CONFIG += silent

SOURCES += \
CQCharts.cpp \
\
CQChartsFilterModel.cpp \
CQChartsExprModel.cpp \
CQChartsExprModelFn.cpp \
CQChartsVarsModel.cpp \
CQChartsTclModel.cpp \
CQChartsExprDataModel.cpp \
CQChartsSelectionModel.cpp \
CQChartsCorrelationModel.cpp \
\
CQChartsColumn.cpp \
CQChartsColumnNum.cpp \
CQChartsColumnType.cpp \
CQChartsModelIndex.cpp \
CQChartsModelColumn.cpp \
\
CQChartsTable.cpp \
CQChartsTree.cpp \
CQChartsHeader.cpp \
CQChartsTableDelegate.cpp \
\
CQChartsWindow.cpp \
CQChartsView.cpp \
CQChartsViewError.cpp \
CQChartsViewQuery.cpp \
CQChartsViewExpander.cpp \
CQChartsViewSettings.cpp \
CQChartsViewStatus.cpp \
CQChartsViewToolBar.cpp \
CQChartsViewToolTip.cpp \
CQChartsProbeBand.cpp \
\
CQChartsViewGLWidget.cpp \
CQChartsSymbolEditor.cpp \
\
CQChartsPlotType.cpp \
CQChartsPlotParameter.cpp \
CQChartsPlotParameterEdit.cpp \
CQChartsPlot.cpp \
CQChartsHierPlot.cpp \
CQChartsHierPlotType.cpp \
CQChartsGroupPlot.cpp \
CQChartsPointPlot.cpp \
\
CQChartsPlot3D.cpp \
CQChartsScatterPlot3D.cpp \
CQChartsCamera.cpp \
\
CQChartsBarPlot.cpp \
CQChartsAdjacencyPlot.cpp \
CQChartsBarChartPlot.cpp \
CQChartsBoxPlot.cpp \
CQChartsBubblePlot.cpp \
CQChartsChordPlot.cpp \
CQChartsCompositePlot.cpp \
CQChartsConnectionPlot.cpp \
CQChartsContourPlot.cpp \
CQChartsCorrelationPlot.cpp \
CQChartsDelaunayPlot.cpp \
CQChartsDendrogramPlot.cpp \
CQChartsDistributionPlot.cpp \
CQChartsEmptyPlot.cpp \
CQChartsForceDirectedPlot.cpp \
CQChartsGeometryPlot.cpp \
CQChartsGraphPlot.cpp \
CQChartsGraphVizPlot.cpp \
CQChartsGridPlot.cpp \
CQChartsHierBubblePlot.cpp \
CQChartsHierScatterPlot.cpp \
CQChartsImagePlot.cpp \
CQChartsParallelPlot.cpp \
CQChartsPiePlot.cpp \
CQChartsPivotPlot.cpp \
CQChartsRadarPlot.cpp \
CQChartsSankeyPlot.cpp \
CQChartsScatterPlot.cpp \
CQChartsSummaryPlot.cpp \
CQChartsStripPlot.cpp \
CQChartsTablePlot.cpp \
CQChartsSunburstPlot.cpp \
CQChartsTreeMapPlot.cpp \
CQChartsWheelPlot.cpp \
CQChartsWordCloudPlot.cpp \
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
CQChartsGraphMgr.cpp \
CQChartsWordCloud.cpp \
CQChartsTextPlacer.cpp \
CQChartsRectPlacer.cpp \
\
CQChartsAxis.cpp \
CQChartsAxisRug.cpp \
CQChartsKey.cpp \
CQChartsMapKey.cpp \
CQChartsTitle.cpp \
CQChartsAnnotation.cpp \
CQChartsArrow.cpp \
CQChartsEditHandles.cpp \
CQChartsResizeHandle.cpp \
CQChartsPlotCustomControls.cpp \
CQChartsGroupPlotCustomControls.cpp \
CQChartsPointPlotCustomControls.cpp \
CQChartsColumnControlGroup.cpp \
CQChartsPlotControlWidgets.cpp \
CQChartsPlotPropertyEdit.cpp \
\
CQChartsInterfaceTheme.cpp \
CQChartsThemeName.cpp \
\
CQChartsBucketModelEdit.cpp \
CQChartsHierSepModelEdit.cpp \
CQChartsPivotModelEdit.cpp \
CQChartsSummaryModelEdit.cpp \
CQChartsTransposeModelEdit.cpp \
CQChartsModelProxyControl.cpp \
\
CQChartsColumnBucket.cpp \
CQChartsValueSet.cpp \
CQChartsPlotSymbol.cpp \
\
CQChartsGlobalPropertiesWidget.cpp \
CQChartsViewPropertiesWidget.cpp \
CQChartsPlotPropertiesWidget.cpp \
CQChartsObjectPropertiesWidget.cpp \
CQChartsPropertyViewTreeFilterEdit.cpp \
CQChartsPlotTable.cpp \
CQChartsAnnotationsTable.cpp \
CQChartsLayerTable.cpp \
CQChartsPlotTip.cpp \
CQChartsSymbolSetsList.cpp \
CQChartsSymbolsList.cpp \
CQChartsPaletteCanvas.cpp \
CQChartsPaletteControl.cpp \
CQChartsInterfaceControl.cpp \
\
CQChartsCreateAnnotationDlg.cpp \
CQChartsCreatePlotDlg.cpp \
CQChartsEditAnnotationDlg.cpp \
CQChartsLoadModelDlg.cpp \
CQChartsManageModelsDlg.cpp \
CQChartsEditModelDlg.cpp \
CQChartsModelDataWidget.cpp \
CQChartsTextDlg.cpp \
CQChartsHelpDlg.cpp \
CQChartsCmdLine.cpp \
\
CQChartsModelWidgets.cpp \
CQChartsModelList.cpp \
CQChartsModelControl.cpp \
CQChartsModelColumnDataControl.cpp \
CQChartsModelFoldControl.cpp \
CQChartsModelExprControl.cpp \
CQChartsModelDetailsWidget.cpp \
CQChartsModelDetailsTable.cpp \
CQChartsModelFlattenControl.cpp \
CQChartsModelFilterControl.cpp \
CQChartsModelChooser.cpp \
CQChartsModelTable.cpp \
CQChartsPreviewPlot.cpp \
CQChartsParamEdit.cpp \
\
CQChartsLoader.cpp \
\
CQChartsModelViewHolder.cpp \
CQChartsModelView.cpp \
CQChartsColumnEval.cpp \
CQChartsExprTcl.cpp \
\
CQChartsFilterEdit.cpp \
\
CQChartsModelData.cpp \
CQChartsModelDetails.cpp \
CQChartsModelExprMatch.cpp \
CQChartsModelFilter.cpp \
\
CQChartsPlotModelVisitor.cpp \
CQChartsModelVisitor.cpp \
\
CQChartsData.cpp \
CQChartsArrowData.cpp \
CQChartsInvalidator.cpp \
\
CQChartsLineCap.cpp \
CQChartsLineDash.cpp \
CQChartsLineJoin.cpp \
\
CQChartsRotatedText.cpp \
\
CQChartsOptBool.cpp \
CQChartsOptInt.cpp \
CQChartsOptLength.cpp \
CQChartsOptPosition.cpp \
CQChartsOptReal.cpp \
CQChartsOptRect.cpp \
CQChartsOptString.cpp \
\
CQChartsAlpha.cpp \
CQChartsAngle.cpp \
CQChartsColor.cpp \
CQChartsColorStops.cpp \
CQChartsFont.cpp \
CQChartsNamePair.cpp \
CQChartsObjRef.cpp \
CQChartsObjRefPos.cpp \
CQChartsPolygonList.cpp \
CQChartsPosition.cpp \
CQChartsLength.cpp \
CQChartsMargin.cpp \
CQChartsReals.cpp \
CQChartsPoints.cpp \
CQChartsRect.cpp \
CQChartsPolygon.cpp \
CQChartsPlotMargin.cpp \
CQChartsConnectionList.cpp \
CQChartsSides.cpp \
CQChartsFillUnder.cpp \
CQChartsPaletteName.cpp \
CQChartsArea.cpp \
CQChartsUnits.cpp \
\
CQChartsColorMap.cpp \
CQChartsSymbolSizeMap.cpp \
CQChartsSymbolTypeMap.cpp \
\
CQChartsSymbol.cpp \
CQChartsSymbolType.cpp \
CQChartsSymbolSet.cpp \
CQChartsSymbolBuffer.cpp \
\
CQChartsImage.cpp \
CQChartsWidget.cpp \
CQChartsPath.cpp \
CQChartsStyle.cpp \
CQChartsBoxWhisker.cpp \
CQChartsDensity.cpp \
CQChartsGrahamHull.cpp \
CQChartsBivariateDensity.cpp \
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
CQChartsJS.cpp \
\
CQChartsHtmlPaintDevice.cpp \
CQChartsScriptPaintDevice.cpp \
CQChartsSVGPaintDevice.cpp \
CQChartsViewPlotPaintDevice.cpp \
CQChartsStatsPaintDevice.cpp \
CQChartsPixelPaintDevice.cpp \
CQChartsPaintDevice.cpp \
\
CQChartsPlotDrawUtil.cpp \
CQChartsSVGUtil.cpp \
CQChartsDrawUtil.cpp \
CQChartsDrawObj.cpp \
CQChartsObjUtil.cpp \
CQChartsUtil.cpp \
CQChartsWidgetUtil.cpp \
\
CQChartsVariant.cpp \
CQChartsFile.cpp \
CQChartsGeom.cpp \
\
CQChartsAnalyzeFile.cpp \
CQChartsAnalyzeModel.cpp \
\
CQChartsDelaunay.cpp \
CQChartsDendrogram.cpp \
CQChartsHull3D.cpp \
CQChartsContour.cpp \
\
CQChartsTitleEdit.cpp \
CQChartsKeyEdit.cpp \
CQChartsAxisEdit.cpp \
\
CQChartsAlphaEdit.cpp \
CQChartsAngleEdit.cpp \
CQChartsArrowDataEdit.cpp \
CQChartsAxisSideEdit.cpp \
CQChartsAxisTickLabelPlacementEdit.cpp \
CQChartsAxisValueTypeEdit.cpp \
CQChartsBoxDataEdit.cpp \
CQChartsColorEdit.cpp \
CQChartsColorRangeSlider.cpp \
CQChartsColumnEdit.cpp \
CQChartsColumnsEdit.cpp \
CQChartsColumnNumEdit.cpp \
CQChartsColumnCombo.cpp \
CQChartsColumnTypeCombo.cpp \
CQChartsColumnExprEdit.cpp \
CQChartsDocument.cpp \
CQChartsEditBase.cpp \
CQChartsFillDataEdit.cpp \
CQChartsFillPatternEdit.cpp \
CQChartsFillUnderEdit.cpp \
CQChartsFontEdit.cpp \
CQChartsFontSizeRangeSlider.cpp \
CQChartsImageEdit.cpp \
CQChartsKeyLocationEdit.cpp \
CQChartsKeyPressBehaviorEdit.cpp \
CQChartsLengthEdit.cpp \
CQChartsLineCapEdit.cpp \
CQChartsLineDashEdit.cpp \
CQChartsLineDataEdit.cpp \
CQChartsLineEditBase.cpp \
CQChartsLineJoinEdit.cpp \
CQChartsMarginEdit.cpp \
\
CQChartsOptBoolEdit.cpp \
CQChartsOptIntEdit.cpp \
CQChartsOptRealEdit.cpp \
CQChartsOptPositionEdit.cpp \
CQChartsOptLengthEdit.cpp \
CQChartsOptRectEdit.cpp \
CQChartsOptStringEdit.cpp \
CQChartsOptEdit.cpp \
\
CQChartsPaletteNameEdit.cpp \
CQChartsPolygonEdit.cpp \
CQChartsPositionEdit.cpp \
CQChartsRectEdit.cpp \
CQChartsRegionMgr.cpp \
CQChartsShapeDataEdit.cpp \
CQChartsSidesEdit.cpp \
CQChartsStrokeDataEdit.cpp \
CQChartsSymbolDataEdit.cpp \
CQChartsSymbolEdit.cpp \
CQChartsSymbolTypeEdit.cpp \
CQChartsSymbolSizeRangeSlider.cpp \
CQChartsSymbolTypeRangeSlider.cpp \
CQChartsSymbolSetEdit.cpp \
CQChartsTextBoxDataEdit.cpp \
CQChartsTextDataEdit.cpp \
CQChartsTitleLocationEdit.cpp \
CQChartsUnitsEdit.cpp \
\
CQChartsGeomBBoxEdit.cpp \
CQChartsGeomPointEdit.cpp \
\
CQChartsEnumEdit.cpp \
CQChartsLineEdit.cpp \
\
CQChartsPropertyViewTree.cpp \
CQChartsPropertyViewEditor.cpp \
\
CQCsvModel.cpp \
CQTsvModel.cpp \
CQJsonModel.cpp \
CQGnuDataModel.cpp \
CQFoldedModel.cpp \
CQHierSepModel.cpp \
\
CQSummaryModel.cpp \
CQSubSetModel.cpp \
CQTransposeModel.cpp \
CQBucketModel.cpp \
CQCollapseModel.cpp \
CQPivotModel.cpp \
CQBucketer.cpp \
\
CSVGUtil.cpp \
\
CQHandDrawnPainter.cpp \
CQFilename.cpp \
CQRangeScroll.cpp \
CQFileWatcher.cpp \
CQThreadObject.cpp \
\
CQDoubleRangeSlider.cpp \
CQIntRangeSlider.cpp \
CQTimeRangeSlider.cpp \
CQRangeSlider.cpp \
\
CQBusyButton.cpp \
\
CQFillTexture.cpp \
CQCommand.cpp \
CQFloatTip.cpp \
CQWidgetTest.cpp \
CLeastSquaresFit.cpp \
CBuchHeim.cpp \
CForceDirected.cpp \
\
CQTclUtil.cpp \
CTclParse.cpp \
\
CQGLControl.cpp \
CGLUtil.cpp \
CGLTexture.cpp \
\
CCircleFactor.cpp \

HEADERS += \
../include/CQCharts.h \
\
../include/CQChartsFilterModel.h \
../include/CQChartsExprModel.h \
../include/CQChartsExprModelFn.h \
../include/CQChartsVarsModel.h \
../include/CQChartsTclModel.h \
../include/CQChartsExprDataModel.h \
../include/CQChartsSelectionModel.h \
../include/CQChartsCorrelationModel.h \
\
../include/CQChartsColumn.h \
../include/CQChartsColumnNum.h \
../include/CQChartsColumnType.h \
../include/CQChartsModelIndex.h \
../include/CQChartsModelColumn.h \
\
../include/CQChartsTable.h \
../include/CQChartsTree.h \
../include/CQChartsHeader.h \
../include/CQChartsTableDelegate.h \
\
../include/CQChartsWindow.h \
../include/CQChartsView.h \
../include/CQChartsViewError.h \
../include/CQChartsViewQuery.h \
../include/CQChartsViewExpander.h \
../include/CQChartsViewSettings.h \
../include/CQChartsViewStatus.h \
../include/CQChartsViewToolBar.h \
../include/CQChartsViewToolTip.h \
../include/CQChartsProbeBand.h \
\
../include/CQChartsViewGLWidget.h \
../include/CQChartsSymbolEditor.h \
\
../include/CQChartsPlotType.h \
../include/CQChartsPlotParameter.h \
../include/CQChartsPlotParameterEdit.h \
../include/CQChartsPlot.h \
../include/CQChartsHierPlot.h \
../include/CQChartsHierPlotType.h \
../include/CQChartsGroupPlot.h \
../include/CQChartsPointPlot.h \
\
../include/CQChartsPlot3D.h \
../include/CQChartsScatterPlot3D.h \
../include/CQChartsCamera.h \
\
../include/CQChartsBarPlot.h \
../include/CQChartsAdjacencyPlot.h \
../include/CQChartsBarChartPlot.h \
../include/CQChartsBoxPlot.h \
../include/CQChartsBubblePlot.h \
../include/CQChartsChordPlot.h \
../include/CQChartsCompositePlot.h \
../include/CQChartsConnectionPlot.h \
../include/CQChartsContourPlot.h \
../include/CQChartsCorrelationPlot.h \
../include/CQChartsDelaunayPlot.h \
../include/CQChartsDendrogramPlot.h \
../include/CQChartsDistributionPlot.h \
../include/CQChartsEmptyPlot.h \
../include/CQChartsForceDirectedPlot.h \
../include/CQChartsGeometryPlot.h \
../include/CQChartsGraphPlot.h \
../include/CQChartsGraphVizPlot.h \
../include/CQChartsGridPlot.h \
../include/CQChartsHierBubblePlot.h \
../include/CQChartsHierScatterPlot.h \
../include/CQChartsImagePlot.h \
../include/CQChartsParallelPlot.h \
../include/CQChartsPiePlot.h \
../include/CQChartsPivotPlot.h \
../include/CQChartsRadarPlot.h \
../include/CQChartsSankeyPlot.h \
../include/CQChartsScatterPlot.h \
../include/CQChartsSummaryPlot.h \
../include/CQChartsStripPlot.h \
../include/CQChartsSunburstPlot.h \
../include/CQChartsTablePlot.h \
../include/CQChartsTreeMapPlot.h \
../include/CQChartsWheelPlot.h \
../include/CQChartsWordCloudPlot.h \
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
../include/CQChartsGraphMgr.h \
../include/CQChartsWordCloud.h \
../include/CQChartsTextPlacer.h \
../include/CQChartsRectPlacer.h \
\
../include/CQChartsAxis.h \
../include/CQChartsAxisRug.h \
../include/CQChartsKey.h \
../include/CQChartsMapKey.h \
../include/CQChartsTitle.h \
../include/CQChartsAnnotation.h \
../include/CQChartsArrow.h \
../include/CQChartsEditHandles.h \
../include/CQChartsResizeHandle.h \
../include/CQChartsPlotCustomControls.h \
../include/CQChartsGroupPlotCustomControls.h \
../include/CQChartsPointPlotCustomControls.h \
../include/CQChartsColumnControlGroup.h \
../include/CQChartsPlotControlWidgets.h \
../include/CQChartsPlotPropertyEdit.h \
\
../include/CQChartsInterfaceTheme.h \
../include/CQChartsThemeName.h \
\
../include/CQChartsBucketModelEdit.h \
../include/CQChartsHierSepModelEdit.h \
../include/CQChartsPivotModelEdit.h \
../include/CQChartsSummaryModelEdit.h \
../include/CQChartsTransposeModelEdit.h \
../include/CQChartsModelProxyControl.h \
\
../include/CQChartsColumnBucket.h \
../include/CQChartsValueSet.h \
../include/CQChartsPlotSymbol.h \
\
../include/CQChartsSymbol.h \
../include/CQChartsSymbolType.h \
../include/CQChartsSymbolSet.h \
../include/CQChartsSymbolBuffer.h \
\
../include/CQChartsImage.h \
../include/CQChartsWidget.h \
../include/CQChartsPath.h \
../include/CQChartsStyle.h \
../include/CQChartsBoxWhisker.h \
../include/CQChartsDensity.h \
../include/CQChartsGrahamHull.h \
../include/CQChartsBivariateDensity.h \
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
../include/CQChartsHtmlPaintDevice.h \
../include/CQChartsScriptPaintDevice.h \
../include/CQChartsSVGPaintDevice.h \
../include/CQChartsViewPlotPaintDevice.h \
../include/CQChartsStatsPaintDevice.h \
../include/CQChartsPixelPaintDevice.h \
../include/CQChartsPaintDevice.h \
\
../include/CQChartsPlotDrawUtil.h \
../include/CQChartsSVGUtil.h \
../include/CQChartsDrawUtil.h \
../include/CQChartsDrawObj.h \
../include/CQChartsObjUtil.h \
../include/CQChartsUtil.h \
../include/CQChartsWidgetUtil.h \
\
../include/CQChartsVariant.h \
../include/CQChartsGeom.h \
\
../include/CQChartsAnalyzeFile.h \
../include/CQChartsAnalyzeModel.h \
\
../include/CQChartsGlobalPropertiesWidget.h \
../include/CQChartsViewPropertiesWidget.h \
../include/CQChartsPlotPropertiesWidget.h \
../include/CQChartsObjectPropertiesWidget.h \
../include/CQChartsPropertyViewTreeFilterEdit.h \
../include/CQChartsPlotTable.h \
../include/CQChartsAnnotationsTable.h \
../include/CQChartsLayerTable.h \
../include/CQChartsPlotTip.h \
../include/CQChartsSymbolSetsList.h \
../include/CQChartsSymbolsList.h \
../include/CQChartsPaletteCanvas.h \
../include/CQChartsPaletteControl.h \
../include/CQChartsInterfaceControl.h \
\
../include/CQChartsCreateAnnotationDlg.h \
../include/CQChartsCreatePlotDlg.h \
../include/CQChartsEditAnnotationDlg.h \
../include/CQChartsLoadModelDlg.h \
../include/CQChartsManageModelsDlg.h \
../include/CQChartsEditModelDlg.h \
../include/CQChartsModelDataWidget.h \
../include/CQChartsTextDlg.h \
../include/CQChartsHelpDlg.h \
../include/CQChartsCmdLine.h \
\
../include/CQChartsModelWidgets.h \
../include/CQChartsModelList.h \
../include/CQChartsModelControl.h \
../include/CQChartsModelColumnDataControl.h \
../include/CQChartsModelFoldControl.h \
../include/CQChartsModelExprControl.h \
../include/CQChartsModelDetailsWidget.h \
../include/CQChartsModelDetailsTable.h \
../include/CQChartsModelFlattenControl.h \
../include/CQChartsModelFilterControl.h \
../include/CQChartsModelChooser.h \
../include/CQChartsModelTable.h \
../include/CQChartsPreviewPlot.h \
../include/CQChartsParamEdit.h \
\
../include/CQChartsLoader.h \
\
../include/CQChartsModelViewHolder.h \
../include/CQChartsModelView.h \
../include/CQChartsColumnEval.h \
../include/CQChartsExprTcl.h \
\
../include/CQChartsFilterEdit.h \
\
../include/CQChartsModelData.h \
../include/CQChartsModelDetails.h \
../include/CQChartsModelExprMatch.h \
../include/CQChartsModelFilter.h \
\
../include/CQChartsPlotModelVisitor.h \
../include/CQChartsModelVisitor.h \
\
../include/CQChartsObjData.h \
../include/CQChartsData.h \
../include/CQChartsArrowData.h \
../include/CQChartsInvalidator.h \
\
../include/CQChartsLineCap.h \
../include/CQChartsLineDash.h \
../include/CQChartsLineJoin.h \
\
../include/CQChartsRotatedText.h \
\
../include/CQChartsAlpha.h \
../include/CQChartsAngle.h \
../include/CQChartsColor.h \
../include/CQChartsColorStops.h \
../include/CQChartsFont.h \
../include/CQChartsNamePair.h \
../include/CQChartsPolygonList.h \
../include/CQChartsObjRef.h \
../include/CQChartsObjRefPos.h \
../include/CQChartsPosition.h \
../include/CQChartsLength.h \
../include/CQChartsMargin.h \
../include/CQChartsReals.h \
../include/CQChartsPoints.h \
../include/CQChartsRect.h \
../include/CQChartsPolygon.h \
../include/CQChartsPlotMargin.h \
../include/CQChartsConnectionList.h \
../include/CQChartsSides.h \
../include/CQChartsFillUnder.h \
../include/CQChartsPaletteName.h \
../include/CQChartsArea.h \
../include/CQChartsUnits.h \
\
../include/CQChartsColorMap.h \
../include/CQChartsSymbolSizeMap.h \
../include/CQChartsSymbolTypeMap.h \
../include/CQChartsColorColumnData.h \
../include/CQChartsSymbolSizeData.h \
../include/CQChartsSymbolTypeData.h \
\
../include/CQChartsAlphaColumnData.h \
../include/CQChartsAnalyzeModelData.h \
../include/CQChartsArcData.h \
../include/CQChartsBrushData.h \
../include/CQChartsCirclePack.h \
../include/CQChartsDisplayRange.h \
../include/CQChartsEnclosingCircle.h \
../include/CQChartsEnum.h \
../include/CQChartsExprCmdValues.h \
../include/CQChartsFile.h \
../include/CQChartsFileType.h \
../include/CQChartsFitData.h \
../include/CQChartsFontSizeData.h \
../include/CQChartsForceDirected.h \
../include/CQChartsGridCell.h \
../include/CQChartsGroupData.h \
../include/CQChartsInputData.h \
../include/CQChartsModelTypes.h \
../include/CQChartsPenBrush.h \
../include/CQChartsPenData.h \
../include/CQChartsPointsCircle.h \
../include/CQChartsRand.h \
../include/CQChartsRegExp.h \
../include/CQChartsRow.h \
../include/CQChartsSmooth.h \
../include/CQChartsTextOptions.h \
../include/CQChartsTip.h \
../include/CQChartsTmpl.h \
../include/CQChartsTreeMapPlace.h \
../include/CQChartsTypes.h \
\
../include/CQChartsOptBool.h \
../include/CQChartsOptInt.h \
../include/CQChartsOptLength.h \
../include/CQChartsOptPosition.h \
../include/CQChartsOptReal.h \
../include/CQChartsOptRect.h \
../include/CQChartsOptString.h \
\
../include/CQChartsHtml.h \
../include/CQChartsJS.h \
\
../include/CQChartsDelaunay.h \
../include/CQChartsDendrogram.h \
../include/CQChartsHull3D.h \
../include/CQChartsContour.h \
\
../include/CQChartsTitleEdit.h \
../include/CQChartsKeyEdit.h \
../include/CQChartsAxisEdit.h \
\
../include/CQChartsAlphaEdit.h \
../include/CQChartsAngleEdit.h \
../include/CQChartsArrowDataEdit.h \
../include/CQChartsAxisSideEdit.h \
../include/CQChartsAxisTickLabelPlacementEdit.h \
../include/CQChartsAxisValueTypeEdit.h \
../include/CQChartsBoxDataEdit.h \
../include/CQChartsColorEdit.h \
../include/CQChartsColorRangeSlider.h \
../include/CQChartsColumnEdit.h \
../include/CQChartsColumnsEdit.h \
../include/CQChartsColumnNumEdit.h \
../include/CQChartsColumnCombo.h \
../include/CQChartsColumnTypeCombo.h \
../include/CQChartsColumnExprEdit.h \
../include/CQChartsDocument.h \
../include/CQChartsEditBase.h \
../include/CQChartsFillDataEdit.h \
../include/CQChartsFillPatternEdit.h \
../include/CQChartsFillUnderEdit.h \
../include/CQChartsFontEdit.h \
../include/CQChartsFontSizeRangeSlider.h \
../include/CQChartsImageEdit.h \
../include/CQChartsKeyLocationEdit.h \
../include/CQChartsKeyPressBehaviorEdit.h \
../include/CQChartsLengthEdit.h \
../include/CQChartsLineCapEdit.h \
../include/CQChartsLineDashEdit.h \
../include/CQChartsLineDataEdit.h \
../include/CQChartsLineEditBase.h \
../include/CQChartsLineJoinEdit.h \
../include/CQChartsMarginEdit.h \
\
../include/CQChartsOptBoolEdit.h \
../include/CQChartsOptIntEdit.h \
../include/CQChartsOptRealEdit.h \
../include/CQChartsOptPositionEdit.h \
../include/CQChartsOptLengthEdit.h \
../include/CQChartsOptRectEdit.h \
../include/CQChartsOptStringEdit.h \
../include/CQChartsOptEdit.h \
\
../include/CQChartsPaletteNameEdit.h \
../include/CQChartsPolygonEdit.h \
../include/CQChartsPositionEdit.h \
../include/CQChartsRectEdit.h \
../include/CQChartsRegionMgr.h \
../include/CQChartsShapeDataEdit.h \
../include/CQChartsSidesEdit.h \
../include/CQChartsStrokeDataEdit.h \
../include/CQChartsSymbolDataEdit.h \
../include/CQChartsSymbolEdit.h \
../include/CQChartsSymbolTypeEdit.h \
../include/CQChartsSymbolSizeRangeSlider.h \
../include/CQChartsSymbolTypeRangeSlider.h \
../include/CQChartsSymbolSetEdit.h \
../include/CQChartsTextBoxDataEdit.h \
../include/CQChartsTextDataEdit.h \
../include/CQChartsTitleLocationEdit.h \
../include/CQChartsUnitsEdit.h \
\
../include/CQChartsGeomBBoxEdit.h \
../include/CQChartsGeomPointEdit.h \
\
../include/CQChartsEnumEdit.h \
../include/CQChartsLineEdit.h \
\
../include/CQChartsPropertyViewTree.h \
../include/CQChartsPropertyViewEditor.h \
\
../include/CQCsvModel.h \
../include/CQTsvModel.h \
../include/CQJsonModel.h \
../include/CQGnuDataModel.h \
../include/CQFoldedModel.h \
../include/CQHierSepModel.h \
../include/CQSummaryModel.h \
../include/CQSubSetModel.h \
../include/CQTransposeModel.h \
../include/CQBucketModel.h \
../include/CQCollapseModel.h \
../include/CQPivotModel.h \
../include/CQBucketer.h \
../include/CQExprModelCmdValues.h \
../include/CQExprModelFn.h \
../include/CQExprUtil.h \
\
../include/CSVGUtil.h \
\
../include/CQHandDrawnPainter.h \
../include/CQFilename.h \
../include/CQRangeScroll.h \
../include/CQFileWatcher.h \
../include/CQThreadObject.h \
\
../include/CQDoubleRangeSlider.h \
../include/CQIntRangeSlider.h \
../include/CQTimeRangeSlider.h \
../include/CQRangeSlider.h \
\
../include/CUnixFile.h \
\
../include/CQFillTexture.h \
../include/CQCommand.h \
../include/CQFloatTip.h \
../include/CQWidgetTest.h \
../include/CLeastSquaresFit.h \
../include/CBuchHeim.h \
../include/CForceDirected.h \
../include/CHexMap.h \
\
../include/CQTclUtil.h \
../include/CTclUtil.h \
../include/CTclParse.h \
\
../include/CQGLControl.h \
../include/CGLUtil.h \
../include/CGLTexture.h \
../include/Springy.h \
\
CQBusyButton.h \
\
CCircleFactor.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQPropertyView/include \
../../CQModelView/include \
../../CQBaseModel/include \
../../CQColors/include \
../../CQDividedArea/include \
../../CQCustomCombo/include \
../../CQUtil/include \
../../CJson/include \
../../CTsv/include \
../../CCsv/include \
../../CXML/include \
../../CQPerfMonitor/include \
../../CQDot/include \
../../CCommand/include \
../../CImageLib/include \
../../CFont/include \
../../CConfig/include \
../../CReadLine/include \
../../CFile/include \
../../CFileUtil/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../CRegExp/include \
../../CGlob/include \
../../COS/include \
/usr/include/tcl \
