APPNAME = CQChartsTestModel

include($$(MAKE_DIR)/qt_app.mk)

QT += svg xml opengl

QMAKE_CXXFLAGS += \
-DCQ_APP_H \
-DCQCHARTS_FOLDED_MODEL \
-DCQCHARTS_DATA_FRAME \

SOURCES += \
CQChartsTestModel.cpp \

HEADERS += \
CQChartsTestModel.h \

INCLUDEPATH += \
$(INC_DIR)/CQCharts \
$(INC_DIR)/CQPropertyView \
$(INC_DIR)/CQColors \
\
$(INC_DIR)/CQCsv \
$(INC_DIR)/CQTsv \
$(INC_DIR)/CQJson \
$(INC_DIR)/CJson \
\
$(INC_DIR)/CQBaseModel \
$(INC_DIR)/CQBucketModel \
$(INC_DIR)/CQCollapseModel \
$(INC_DIR)/CQExprModel \
$(INC_DIR)/CQFoldedModel \
$(INC_DIR)/CQHierSepModel \
$(INC_DIR)/CQLorenzModel \
$(INC_DIR)/CQPivotModel \
$(INC_DIR)/CQSubSetModel \
$(INC_DIR)/CQSummaryModel \
$(INC_DIR)/CQTransposeModel \
\
$(INC_DIR)/CQBucketer \
$(INC_DIR)/CQExcel \
\
$(INC_DIR)/CQTableWidget \
$(INC_DIR)/CQModelView \
$(INC_DIR)/CQTableView \
$(INC_DIR)/CQTreeView \
\
$(INC_DIR)/CQTabSplit \
$(INC_DIR)/CQAppWindow \
$(INC_DIR)/CQRangeScroll \
$(INC_DIR)/CQRealSpin \
$(INC_DIR)/CQComboSlider \
$(INC_DIR)/CQApp \
$(INC_DIR)/CQStyle \
$(INC_DIR)/CQUtil \
$(INC_DIR)/CQStrParse \
$(INC_DIR)/CQHistoryLineEdit \
$(INC_DIR)/CQAlignEdit \
$(INC_DIR)/CQLineEdit \
$(INC_DIR)/CQRangeSlider \
\
$(INC_DIR)/CQFillTexture \
$(INC_DIR)/CQDataFrame \
\
$(INC_DIR)/CReadLine \
$(INC_DIR)/CHistory \
$(INC_DIR)/CImageLib \
$(INC_DIR)/CFont \
$(INC_DIR)/CConfig \
$(INC_DIR)/CUnixFile \
$(INC_DIR)/CFile \
$(INC_DIR)/CFileType \
$(INC_DIR)/CDisplayRange2D \
$(INC_DIR)/CDisplayTransform2D \
$(INC_DIR)/CInterval \
$(INC_DIR)/CQCommand \
$(INC_DIR)/CQScrollArea \
$(INC_DIR)/CQPerfMonitor \
$(INC_DIR)/CQWidgetTest \
$(INC_DIR)/CMath \
$(INC_DIR)/CStrParse \
$(INC_DIR)/CStrUtil \
$(INC_DIR)/CTrace \
$(INC_DIR)/CRGBA \
$(INC_DIR)/CGlob \
$(INC_DIR)/CHRTimer \
$(INC_DIR)/CHRTime \
$(INC_DIR)/COS \
$(INC_DIR)/CQTclCmd \
$(INC_DIR)/CQTclUtil \
$(INC_DIR)/CTclUtil \
$(INC_DIR)/CUtil \
$(INC_DIR)/CLeastSquaresFit \
/usr/include/tcl \

PRE_TARGETDEPS = \
$(LIB_DIR)/libCQCharts.a \
$(LIB_DIR)/libCQApp.a \
$(LIB_DIR)/libCQStyle.a \
$(LIB_DIR)/libCQAppWindow.a \
$(LIB_DIR)/libCQPropertyView.a \
$(LIB_DIR)/libCQThreadObject.a \
$(LIB_DIR)/libCQColors.a \
$(LIB_DIR)/libCQColorsEdit.a \
$(LIB_DIR)/libCQRangeScroll.a \
$(LIB_DIR)/libCQTableWidget.a \
$(LIB_DIR)/libCQTableView.a \
$(LIB_DIR)/libCQTreeView.a \
$(LIB_DIR)/libCQHeaderView.a \
$(LIB_DIR)/libCQToolTip.a \
$(LIB_DIR)/libCQFloatTip.a \
$(LIB_DIR)/libCQSummaryModel.a \
$(LIB_DIR)/libCQCsv.a \
$(LIB_DIR)/libCQTsv.a \
$(LIB_DIR)/libCQJson.a \
$(LIB_DIR)/libCJson.a \
$(LIB_DIR)/libCQGnuData.a \
$(LIB_DIR)/libCQFoldedModel.a \
$(LIB_DIR)/libCQHierSepModel.a \
$(LIB_DIR)/libCQBucketModel.a \
$(LIB_DIR)/libCQSubSetModel.a \
$(LIB_DIR)/libCQTransposeModel.a \
$(LIB_DIR)/libCQCollapseModel.a \
$(LIB_DIR)/libCQPivotModel.a \
$(LIB_DIR)/libCQBucketer.a \
$(LIB_DIR)/libCQBaseModel.a \
$(LIB_DIR)/libCQHandDrawn.a \
$(LIB_DIR)/libCQFillTexture.a \
$(LIB_DIR)/libCQTclCmd.a \
$(LIB_DIR)/libCQMetaEdit.a \
$(LIB_DIR)/libCQModelView.a \
$(LIB_DIR)/libCQFontDialog.a \
$(LIB_DIR)/libCQSwitch.a \
$(LIB_DIR)/libCQCheckBox.a \
$(LIB_DIR)/libCQColorChooser.a \
$(LIB_DIR)/libCQColorEdit.a \
$(LIB_DIR)/libCQFontEdit.a \
$(LIB_DIR)/libCQTabWidget.a \
$(LIB_DIR)/libCQTabSplit.a \
$(LIB_DIR)/libCQGroupBox.a \
$(LIB_DIR)/libCQEnumCombo.a \
$(LIB_DIR)/libCQDragLabel.a \
$(LIB_DIR)/libCQLineEdit.a \
$(LIB_DIR)/libCQRangeSlider.a \
$(LIB_DIR)/libCQUtil.a \
$(LIB_DIR)/libCQCommand.a \
$(LIB_DIR)/libCQDataFrame.a \
$(LIB_DIR)/libCQScrollArea.a \
$(LIB_DIR)/libCQPerfMonitor.a \
$(LIB_DIR)/libCQPixmapCache.a \
$(LIB_DIR)/libCQWidgetTest.a \
$(LIB_DIR)/libCQWinWidget.a \
$(LIB_DIR)/libCQLabel.a \
$(LIB_DIR)/libCQGraphViz.a \
$(LIB_DIR)/libCGraphViz.a \
$(LIB_DIR)/libCXML.a \
$(LIB_DIR)/libCBuchHeim.a \
$(LIB_DIR)/libCInterval.a \
$(LIB_DIR)/libCStrUtil.a \
$(LIB_DIR)/libCOS.a \

MODEL_LIBS += \
-lCQCsv -lCCsv -lCQTsv -lCTsv -lCQJson -lCJson -lCQGnuData \
-lCQFoldedModel -lCQHierSepModel -lCQBucketModel -lCQSubSetModel -lCQTransposeModel -lCQExcel \
-lCQModelDbg -lCQLorenzModel -lCLorenzCalc -lCPickoverCalc -lCDragon3DCalc -lCLeaf3DCalc \
-lCQBucketer -lCQBaseModel -lCQSummaryModel -lCQCollapseModel -lCQPivotModel -lCQExprModel \
-lCExpr -lCQTrie \

PLOT_LIBS += \
-lCQFillTexture -lCQHandDrawn -lCQThreadObject \
-lCQGraphViz -lCGraphViz -lCDelaunay -lCHull3D -lCBuchHeim \
-lCLeastSquaresFit -lCInterval \
-lCCircleFactor \

WIDGET_LIBS += \
-lCQPropertyView \
-lCQRangeScroll -lCQColorsEdit -lCQColors -lCQToolTip -lCQFloatTip -lCQModelView -lCQItemDelegate \
-lCQTableWidget -lCQTableView -lCQTreeView -lCQHeaderView -lCQTabWidget -lCQTabSplit \
-lCQFloatEdit -lCQAutoHide -lCQGroupBox -lCQEnumCombo -lCQDragLabel -lCQFilename -lCQCustomCombo \
-lCQAppWindow -lCQLabel $$QT_APP_LIBS -lCQDialog \
-lCQLinkLabel -lCQRotatedText -lCQRoundedPolygon -lCQColorChooser -lCQColorEdit \
-lCQFontEdit -lCQRealSpin -lCQAlignEdit -lCQComboSlider -lCQSlider -lCQAngleSpinBox \
-lCQBBox2DEdit -lCQRectEdit -lCQPoint2DEdit -lCQLineDash -lCQIconCombo -lCQIntegerSpin \
-lCQIntegerEdit -lCQSwitch -lCQCheckBox -lCQWidgetMenu -lCQRadioButtons -lCQLineEdit \
-lCQIconButton -lCQPixmapButton -lCQWinWidget -lCQFileWatcher -lCQStrParse -lCQRangeSlider \
-lCQScrollArea -lCQWidgetTest -lCQImageButton \

DATA_FRAME_LIBS += \
-lCQDataFrame -lCQCommand -lCCommand

TCL_LIBS += \
-lCQTclUtil -lCTclUtil -lCQTclCmd -lCTclParse

PERF_LIBS += \
-lCQPerfMonitor \

GL_LIBS += \
-lCQGLControl -lCGLUtil -lCGLTexture \

EXTRA_LIBS += \
-lCFileMatch -lCEscape -lCEvent -lCRGBUtil -lCSymbol2D -lCReadLine -lCHistory \
-lCSVGUtil -lCXML -lCImageLib -lCFont -lCConfig -lCFile -lCFileParse -lCFileUtil -lCTempFile \
-lCStrParse -lCArgs -lCUtil -lCEnv -lCGlob -lCRegExp \
-lCMath -lCStrUtil -lCPrintF -lCOS \

SYS_LIBS += \
-ltre -lpng -ljpeg -ltcl -lX11 -lreadline -lcurses \
-lGLU -lglut

unix:LIBS += \
-lCQCharts \
 $$PLOT_LIBS \
 $$MODEL_LIBS \
 $$DATA_FRAME_LIBS \
 $$TCL_LIBS \
 $$PERF_LIBS \
 $$WIDGET_LIBS \
 $$GL_LIBS \
 $$EXTRA_LIBS \
 $$SYS_LIBS \
