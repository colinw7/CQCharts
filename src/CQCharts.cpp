#include <CQCharts.h>
#include <CQChartsView.h>

// plot types
#include <CQChartsAdjacencyPlot.h>
#include <CQChartsBarChartPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsBubblePlot.h>
#include <CQChartsChordPlot.h>
#include <CQChartsCompositePlot.h>
#include <CQChartsContourPlot.h>
#include <CQChartsCorrelationPlot.h>
#include <CQChartsDelaunayPlot.h>
#include <CQChartsDendrogramPlot.h>
#include <CQChartsDistributionPlot.h>
#include <CQChartsEmptyPlot.h>
#include <CQChartsForceDirectedPlot.h>
#include <CQChartsGeometryPlot.h>
#include <CQChartsGraphPlot.h>
#include <CQChartsGraphVizPlot.h>
#include <CQChartsGridPlot.h>
#include <CQChartsHierBubblePlot.h>
#include <CQChartsHierScatterPlot.h>
#include <CQChartsImagePlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsPivotPlot.h>
#include <CQChartsRadarPlot.h>
#include <CQChartsSankeyPlot.h>
#include <CQChartsScatterPlot.h>
#include <CQChartsScatterPlot3D.h>
#include <CQChartsStripPlot.h>
#include <CQChartsSummaryPlot.h>
#include <CQChartsSunburstPlot.h>
#include <CQChartsTablePlot.h>
#include <CQChartsTreeMapPlot.h>
#include <CQChartsWheelPlot.h>
#include <CQChartsWordCloudPlot.h>
#include <CQChartsXYPlot.h>

#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsFileType.h>
#include <CQChartsImage.h>
#include <CQChartsWidget.h>
#include <CQChartsModelIndex.h>
#include <CQChartsModelColumn.h>
#include <CQChartsModelChooser.h>
#include <CQChartsModelFilterControl.h>
#include <CQChartsModelFlattenControl.h>
#include <CQChartsModelFoldControl.h>

#include <CQChartsGlobalPropertiesWidget.h>
#include <CQChartsViewPropertiesWidget.h>
#include <CQChartsPlotPropertiesWidget.h>

#include <CQChartsPlotTable.h>
#include <CQChartsAnnotationsTable.h>

// editors
#include <CQChartsAlphaEdit.h>
#include <CQChartsAngleEdit.h>
#include <CQChartsArrowDataEdit.h>
#include <CQChartsAxisSideEdit.h>
#include <CQChartsAxisTickLabelPlacementEdit.h>
#include <CQChartsAxisValueTypeEdit.h>
#include <CQChartsBoxDataEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsColumnEdit.h>
#include <CQChartsColumnsEdit.h>
#include <CQChartsColumnNumEdit.h>
#include <CQChartsFillDataEdit.h>
#include <CQChartsFillPatternEdit.h>
#include <CQChartsFillUnderEdit.h>
#include <CQChartsFontEdit.h>
#include <CQChartsGeomBBoxEdit.h>
#include <CQChartsGeomPointEdit.h>
#include <CQChartsImageEdit.h>
#include <CQChartsKeyLocationEdit.h>
#include <CQChartsKeyPressBehaviorEdit.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsLineDataEdit.h>
#include <CQChartsLineDashEdit.h>
#include <CQChartsLineCapEdit.h>
#include <CQChartsLineJoinEdit.h>
#include <CQChartsMarginEdit.h>
#include <CQChartsPaletteNameEdit.h>
#include <CQChartsPolygonEdit.h>
#include <CQChartsPositionEdit.h>
#include <CQChartsRectEdit.h>
#include <CQChartsShapeDataEdit.h>
#include <CQChartsSidesEdit.h>
#include <CQChartsStrokeDataEdit.h>
#include <CQChartsTextBoxDataEdit.h>
#include <CQChartsTextDataEdit.h>
#include <CQChartsTitleLocationEdit.h>
#include <CQChartsUnitsEdit.h>

#include <CQChartsOptBoolEdit.h>
#include <CQChartsOptIntEdit.h>
#include <CQChartsOptRealEdit.h>
#include <CQChartsOptStringEdit.h>
#include <CQChartsOptLengthEdit.h>
#include <CQChartsOptPositionEdit.h>
#include <CQChartsOptRectEdit.h>

// types
#include <CQChartsPolygonList.h>
#include <CQChartsNamePair.h>
#include <CQChartsSides.h>
#include <CQChartsFillUnder.h>
#include <CQChartsWindow.h>
#include <CQChartsPath.h>
#include <CQChartsVariant.h>
#include <CQChartsPoints.h>
#include <CQChartsReals.h>
#include <CQChartsObjRef.h>
#include <CQChartsObjRefPos.h>

#include <CQChartsOptBool.h>
#include <CQChartsOptInt.h>
#include <CQChartsOptReal.h>
#include <CQChartsOptString.h>
#include <CQChartsOptLength.h>
#include <CQChartsOptPosition.h>
#include <CQChartsOptRect.h>

#include <CQChartsFont.h>
#include <CQChartsInterfaceTheme.h>
#include <CQChartsColorStops.h>
#include <CQChartsHtml.h>

#include <CQChartsColorMap.h>
#include <CQChartsSymbolTypeMap.h>
#include <CQChartsSymbolSizeMap.h>

// dialogs
#include <CQChartsLoadModelDlg.h>
#include <CQChartsManageModelsDlg.h>
#include <CQChartsEditModelDlg.h>
#include <CQChartsCreatePlotDlg.h>

// widgets
#include <CQChartsPlotControlWidgets.h>
#include <CQChartsModelViewHolder.h>
#include <CQChartsModelDetailsTable.h>
#include <CQChartsModelDetailsWidget.h>
#include <CQChartsPlotPropertyEdit.h>
#include <CQChartsModelColumnDataControl.h>
#include <CQChartsModelExprControl.h>
#include <CQChartsModelControl.h>
#include <CQChartsModelTable.h>
#include <CQChartsModelDataWidget.h>

// colors
#include <CQChartsPaletteCanvas.h>
#include <CQChartsPaletteControl.h>
#include <CQChartsInterfaceControl.h>
#include <CQColorsEditList.h>
#include <CQColorsEditControl.h>
#include <CQColorsPalette.h>
#include <CQColorsTheme.h>
#include <CQColors.h>

// symbols
#include <CQChartsSymbolSet.h>
#include <CQChartsSymbolDataEdit.h>
#include <CQChartsSymbolTypeEdit.h>
#include <CQChartsSymbolSetEdit.h>
#include <CQChartsSymbolSetsList.h>
#include <CQChartsSymbolsList.h>
#include <CQChartsSymbolEdit.h>
#include <CQChartsSymbolEditor.h>

// layer
#include <CQChartsLayerTable.h>

// property view
#include <CQPropertyView.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

// generic widgets
#include <CQIntRangeSlider.h>
#include <CQDoubleRangeSlider.h>
#include <CQTimeRangeSlider.h>

#include <CQAlignEdit.h>
#include <CQCheckBox.h>
#include <CQColorEdit.h>
#include <CQCustomCombo.h>
#include <CQDragLabel.h>
#include <CQFilename.h>
#include <CQFontEdit.h>
#include <CQGroupBox.h>
#include <CQIconCombo.h>
#include <CQIntegerSpin.h>
#include <CQRadioButtons.h>
#include <CQRealSpin.h>
#include <CQSwitch.h>
#include <CQTabWidget.h>
#include <CQTableWidget.h>
#include <CQWinWidget.h>

#include <CQWidgetFactory.h>
#include <CQTclUtil.h>

#include <QTimer>
#include <QFileInfo>

#include <iostream>

// svg files
#include <svg/select_light_svg.h>
#include <svg/select_dark_svg.h>

#include <svg/zoom_in_light_svg.h>
#include <svg/zoom_in_dark_svg.h>

#include <svg/zoom_out_light_svg.h>
#include <svg/zoom_out_dark_svg.h>

#include <svg/pan_light_svg.h>
#include <svg/pan_dark_svg.h>

#include <svg/query_light_svg.h>
#include <svg/query_dark_svg.h>

#include <svg/probe_light_svg.h>
#include <svg/probe_dark_svg.h>

#include <svg/edit_light_svg.h>
#include <svg/edit_dark_svg.h>

#include <svg/zoom_fit_light_svg.h>
#include <svg/zoom_fit_dark_svg.h>

#include <svg/region_light_svg.h>
#include <svg/region_dark_svg.h>

#include <svg/ruler_light_svg.h>
#include <svg/ruler_dark_svg.h>

#include <svg/left_light_svg.h>
#include <svg/left_dark_svg.h>

#include <svg/right_light_svg.h>
#include <svg/right_dark_svg.h>

#include <svg/models_light_svg.h>
#include <svg/models_dark_svg.h>

#include <svg/charts_light_svg.h>
#include <svg/charts_dark_svg.h>

#include <svg/options_light_svg.h>
#include <svg/options_dark_svg.h>

#include <svg/settings_light_svg.h>
#include <svg/settings_dark_svg.h>

#include <svg/table_light_svg.h>
#include <svg/table_dark_svg.h>

#include <svg/console_light_svg.h>
#include <svg/console_dark_svg.h>

#include <svg/legend_light_svg.h>
#include <svg/legend_dark_svg.h>

#include <svg/columns_light_svg.h>
#include <svg/columns_dark_svg.h>

#include <svg/numeric_columns_light_svg.h>
#include <svg/numeric_columns_dark_svg.h>

#include <svg/filter_light_svg.h>
#include <svg/filter_dark_svg.h>

#include <svg/search_light_svg.h>
#include <svg/search_dark_svg.h>

#include <svg/add_light_svg.h>
#include <svg/add_dark_svg.h>

#include <svg/remove_light_svg.h>
#include <svg/remove_dark_svg.h>

#include <svg/region_light_svg.h>
#include <svg/region_dark_svg.h>

#include <svg/error_light_svg.h>
#include <svg/error_dark_svg.h>

#include <svg/refresh_light_svg.h>
#include <svg/refresh_dark_svg.h>

#include <svg/file_dialog_light_svg.h>
#include <svg/file_dialog_dark_svg.h>

#include <svg/info_light_svg.h>
#include <svg/info_dark_svg.h>

#include <svg/bucket_fixed_light_svg.h>
#include <svg/bucket_fixed_dark_svg.h>

#include <svg/bucket_stops_light_svg.h>
#include <svg/bucket_stops_dark_svg.h>

#include <svg/bucket_range_light_svg.h>
#include <svg/bucket_range_dark_svg.h>

#include <svg/bucket_unique_light_svg.h>
#include <svg/bucket_unique_dark_svg.h>

QString
CQCharts::
description()
{
  auto LI  = [](const QString &str) { return CQChartsHtml::Str(str); };
//auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };
  auto IMG = [](const QString &) { return QString(); };

  return CQChartsHtml().
   h2("Introduction").
    p("CQCharts is a Qt based charting library to support the display of an interactive "
      "chart from a data model (QAbstractItemModel) with support for cross selection using "
      "the data model's selection model (QItemSelectionModel).").
    p("Charts are live in that changes to the data model automatically update "
      "the chart. This allows the programmer to build interactive charts where "
      "the user can select items from the model using a table or tree view and/or "
      "the displayed chart data.").
    p("The library comes with a test program which supports scripting of charts using the "
      "'tcl' programming language. A number of example plot scripts are available in the "
      "data directory.").
   h3("Chart Types").
    p("Chart types supported:").
    ul({
     LI("Adjacency : tabular connectivity table (needs special column value syntax "
        "for connections) " + IMG("images/adjacency.png")),
     LI("BarChart : 1d bar chart for column values " + IMG("images/barchart.png")),
     LI("Box : Box plot of aggregated values from value and group columns " +
        IMG("images/boxplot.png") + IMG("images/boxplot_connected.png")),
     LI("Bubble : Bubble plot where circle is size of column value" +
        IMG("images/bubbleplot.png")),
     LI("Chord : Circlular connectivity chart (needs special column value syntax for "
        "connections) " + IMG("images/chord_plot.png")),
     LI("Contour : Contour plot" +
        IMG("images/contour.png")),
     LI("Correlation : Correlation plot" +
        IMG("images/correlation.png")),
     LI("Delaunay : Delaunay/Voronoi geometric connectivity chart" +
        IMG("images/delaunay.png")),
     LI("Dendrogram : Dendrogram plot" +
        IMG("images/dendrogram.png")),
     LI("Distribution : Distribution count of range based column values" +
        IMG("images/distribution.png")),
     LI("ForceDirected : Force directed connectivity chart" +
        IMG("images/forcedirected.png")),
     LI("Geometry : General polygon geometry colored by value" +
        IMG("images/geometryplot.png")),
     LI("Graph : Graph plot of connected values" +
        IMG("images/graph.png")),
     LI("HierBubble : Hierarchical bubble plot where circle is size of column value" +
        IMG("images/hierbubble.png")),
     LI("Image : Image/regular grid plot (x, y, color)" +
        IMG("images/imageplot.png")),
     LI("Parallel : Parallel lines plot using multiple connected value sets" +
        IMG("images/parallelplot.png")),
     LI("Pie : Pie chart" +
        IMG("images/piechart.png")),
     LI("Pivot : Pivot chart" +
        IMG("images/pivot.png")),
     LI("Radar : Radar (polygon pie chart)" +
        IMG("images/radar.png")),
     LI("Sankey : Sankey plot of connected values" +
        IMG("images/sankey.png")),
     LI("Scatter : Scatter plot of disparate x/y column values" +
        IMG("images/scatterplot.png")),
     LI("Scatter 3D : Scatter plot of disparate x/y/z column values" +
        IMG("images/scatter3d.png")),
     LI("Sunburst : Hierarchical pie chart plot" +
        IMG("images/sunburst.png")),
     LI("Table : Table in a plot" +
        IMG("images/table.png")),
     LI("TreeMap : Hierarchical tree map" +
        IMG("images/treemap.png")),
     LI("Wheel : Wheel plot" +
        IMG("images/wheel.png")),
     LI("Word Cloud : Word Cloud plot" +
        IMG("images/wordcloud.png")),
     LI("XY : x/y monotonic value plot" +
        IMG("images/xychart.png") + IMG("images/goal_scorers.png")) }).
   h3("Adjacency").
    p("Uses table cell coloring to show number of connections between two items.").
    p("Items names are displayed in the row and column headers and the number of connections "
      "is used to color the cell (row/column intersection).").
    p("Cell colors are derived from the blend of the colors for each item's group "
      "and the color intensity is calculated from the relative value.").
    p("The following values can be customized:").
     ul({
      LI("table background color"),
      LI("empty cell color"),
      LI("margin"),
      LI("cell border color, alpha and corner size"),
      LI("header text color and font")}).
    p("Input data model can contain either node data (name, id, group) and a list of "
      "connections (id, count) [old format] or a '/' separated connection data "
      "(from name/to name, value, count).").
   h3("BarChart").
    p("1d bar chart for column value or values.").
    p("Bars can be grouped using a group/category column.").
    p("Custom bar colors and data labels can be provided in additional columns.").
    p("Bars can be stacked next to each other or on top of each other and can be "
      "drawn vertically (default) or horizontally.").
   h3("Box").
    p("Box plot of aggregated values from value and group columns.").
    p("Input data is a set of y values for an associated common x value.").
    p("Data can also be additional grouped by an extra column.").
    p("Values can be displayed as a candlestick bar or connected into a single "
      "solid bar of the value range.").
   h3("Bubble").
    p("Bubble plot where circle is size of column value.").
    p("Circles are packed in minimum enclosing circle.").
   h3("Chord").
    p("Circlular connectivity chart.").
    p("Input data model can contain either node data (name, id, group) and a list of "
      "connections (id, count) (old format) or '/' separated connection data "
      "(from name/to name, value, count).").
   h3("Delaunay").
    p("Delaunay/Voronoi geometric connectivity chart").
   h3("Distribution").
    p("Distribution count of range based values").
   h3("ForceDirected").
    p("Force directed connectivity chart").
   h3("Geometry").
    p("General polygon geometry colored by value").
   h3("Graph").
    p("Graph plot of connected values").
   h3("HierBubble").
    p("Hierarchical bubble plot where circle is size of column value.").
    p("Hierarchical circles are packed in minimum enclosing circle.").
   h3("Image").
    p("Image plot (x, y, color)").
   h3("Parallel").
    p("Parallel lines plots multiple overlaid value sets").
   h3("Pie").
    p("Pie chart").
   h3("Radar").
    p("Radar (polygon pie chart)").
   h3("Sankey").
    p("Sankey plot of connected values").
   h3("Scatter").
    p("Scatter plot of disparate values").
   h3("Sunburst").
    p("Hierarchical pie chart plot").
   h3("TreeMap").
    p("Hierarchical tree map").
   h3("XY").
    p("Plot of x,y values. x values should be monotonic.").
    p("Multiple y columns can be supplied to produce multi line plot.").
    p("Binariate plot using two y columns.").
    p("Display of connecing lines and individual points can be customized.").
  h2("Parameter and Properties").
   p("Each plot type has a set of parameters which allow the major plot controls "
     "to be configured.").
   p("The plot also support properties for fine tuning the plot display.").
   p("The parameter and properties use Qt variants (QVariant)  and are displayed in a "
     "property view tree which can be displayed at the side of the plot.").
  h2(" Data Model").
   p("The data model can be viewed in a table or tree view in the plot and can be "
     "sorted and filtered to update the plot.");
}

//---

CQCharts::
CQCharts()
{
  // register variant meta types
  CQChartsGeom::registerMetaTypes();

  CQChartsAlpha                 ::registerMetaType();
  CQChartsAngle                 ::registerMetaType();
  CQChartsArea                  ::registerMetaType();
  CQChartsArrowData             ::registerMetaType();
  CQChartsAxisSide              ::registerMetaType();
  CQChartsAxisTickLabelPlacement::registerMetaType();
  CQChartsAxisValueType         ::registerMetaType();
  CQChartsBoxData               ::registerMetaType();
  CQChartsColorMap              ::registerMetaType();
  CQChartsColor                 ::registerMetaType();
  CQChartsColorStops            ::registerMetaType();
  CQChartsColumn                ::registerMetaType();
  CQChartsColumns               ::registerMetaType();
  CQChartsColumnNum             ::registerMetaType();
  CQChartsColumnTypeId          ::registerMetaType();
  CQChartsConnectionList        ::registerMetaType();
  CQChartsFillData              ::registerMetaType();
  CQChartsFillPattern           ::registerMetaType();
  CQChartsFillUnderPos          ::registerMetaType();
  CQChartsFillUnderSide         ::registerMetaType();
  CQChartsFont                  ::registerMetaType();
  CQChartsImage                 ::registerMetaType();
  CQChartsKeyLocation           ::registerMetaType();
  CQChartsKeyPressBehavior      ::registerMetaType();
  CQChartsLength                ::registerMetaType();
  CQChartsLineCap               ::registerMetaType();
  CQChartsLineDash              ::registerMetaType();
  CQChartsLineData              ::registerMetaType();
  CQChartsLineJoin              ::registerMetaType();
  CQChartsMargin                ::registerMetaType();
  CQChartsModelColumn           ::registerMetaType();
  CQChartsModelIndex            ::registerMetaType();
  CQChartsNamePair              ::registerMetaType();
  CQChartsObjRefPos             ::registerMetaType();
  CQChartsObjRef                ::registerMetaType();
  CQChartsPaletteName           ::registerMetaType();
  CQChartsPath                  ::registerMetaType();
  CQChartsPoints                ::registerMetaType();
  CQChartsPolygon               ::registerMetaType();
  CQChartsPolygonList           ::registerMetaType();
  CQChartsPosition              ::registerMetaType();
  CQChartsReals                 ::registerMetaType();
  CQChartsRect                  ::registerMetaType();
  CQChartsShapeData             ::registerMetaType();
  CQChartsSides                 ::registerMetaType();
  CQChartsStrokeData            ::registerMetaType();
  CQChartsStyle                 ::registerMetaType();
  CQChartsSymbol                ::registerMetaType();
  CQChartsSymbolData            ::registerMetaType();
  CQChartsSymbolSizeMap         ::registerMetaType();
  CQChartsSymbolTypeMap         ::registerMetaType();
  CQChartsSymbolType            ::registerMetaType();
  CQChartsTextBoxData           ::registerMetaType();
  CQChartsTextData              ::registerMetaType();
  CQChartsThemeName             ::registerMetaType();
  CQChartsTitleLocation         ::registerMetaType();
  CQChartsUnits                 ::registerMetaType();
  CQChartsWidget                ::registerMetaType();

  CQChartsOptBool    ::registerMetaType();
  CQChartsOptInt     ::registerMetaType();
  CQChartsOptLength  ::registerMetaType();
  CQChartsOptPosition::registerMetaType();
  CQChartsOptReal    ::registerMetaType();
  CQChartsOptRect    ::registerMetaType();
  CQChartsOptString  ::registerMetaType();

  //---

  // init theme
  plotTheme_.setName("default");

  //---

  interfaceTheme_ = std::make_unique<CQChartsInterfaceTheme>();

  interfaceTheme()->setDark(false);

  //---

  addProc(ProcType::SVG, "logMessage", "s",
    "document.getElementById(\"log_message\").innerHTML = s;");

  addProc(ProcType::SVG, "plotObjClick", "id", "logMessage('Click ' + id);");
  addProc(ProcType::SVG, "annotationClick", "id", "logMessage('Click ' + id);");

  //---

  addProc(ProcType::SCRIPT, "logMessage", "s",
    "document.getElementById(\"log_message\").innerHTML = s;");

  addProc(ProcType::SCRIPT, "plotObjClick", "id", "charts.log('Click ' + id);");
  addProc(ProcType::SCRIPT, "annotationClick", "id", "charts.log('Click ' + id);");

  //---

  propertyModel_ = std::make_unique<CQPropertyViewModel>();

  connect(propertyModel_.get(), SIGNAL(valueChanged(QObject *, const QString &)),
          this, SLOT(propertyItemChanged(QObject *, const QString &)));

  addProperties();

  //---

  cmdTcl_ = new CQTcl();
}

CQCharts::
~CQCharts()
{
  for (auto &modelData : modelDatas_)
    delete modelData;
}

void
CQCharts::
init()
{
  plotTypeMgr_   = std::make_unique<CQChartsPlotTypeMgr>();
  columnTypeMgr_ = std::make_unique<CQChartsColumnTypeMgr>(this);
  symbolSetMgr_  = std::make_unique<CQChartsSymbolSetMgr>(this);

  //---

  // add plot types
  plotTypeMgr()->addTypeT<CQChartsAdjacencyPlotType    >("adjacency"    );
  plotTypeMgr()->addTypeT<CQChartsBarChartPlotType     >("barchart"     );
  plotTypeMgr()->addTypeT<CQChartsBoxPlotType          >("box"          );
  plotTypeMgr()->addTypeT<CQChartsBubblePlotType       >("bubble"       );
  plotTypeMgr()->addTypeT<CQChartsChordPlotType        >("chord"        );
  plotTypeMgr()->addTypeT<CQChartsCompositePlotType    >("composite"    );
  plotTypeMgr()->addTypeT<CQChartsContourPlotType      >("contour"      );
  plotTypeMgr()->addTypeT<CQChartsCorrelationPlotType  >("correlation"  );
  plotTypeMgr()->addTypeT<CQChartsDelaunayPlotType     >("delaunay"     );
  plotTypeMgr()->addTypeT<CQChartsDendrogramPlotType   >("dendrogram"   );
  plotTypeMgr()->addTypeT<CQChartsDistributionPlotType >("distribution" );
  plotTypeMgr()->addTypeT<CQChartsGraphVizPlotType     >("graphviz"     );
  plotTypeMgr()->addTypeT<CQChartsEmptyPlotType        >("empty"        );
  plotTypeMgr()->addTypeT<CQChartsForceDirectedPlotType>("forcedirected");
  plotTypeMgr()->addTypeT<CQChartsGeometryPlotType     >("geometry"     );
  plotTypeMgr()->addTypeT<CQChartsGraphPlotType        >("graph"        );
  plotTypeMgr()->addTypeT<CQChartsGridPlotType         >("grid"         );
  plotTypeMgr()->addTypeT<CQChartsHierBubblePlotType   >("hierbubble"   );
  plotTypeMgr()->addTypeT<CQChartsHierScatterPlotType  >("hierscatter"  );
  plotTypeMgr()->addTypeT<CQChartsImagePlotType        >("image"        );
  plotTypeMgr()->addTypeT<CQChartsParallelPlotType     >("parallel"     );
  plotTypeMgr()->addTypeT<CQChartsPiePlotType          >("pie"          );
  plotTypeMgr()->addTypeT<CQChartsPivotPlotType        >("pivot"        );
  plotTypeMgr()->addTypeT<CQChartsRadarPlotType        >("radar"        );
  plotTypeMgr()->addTypeT<CQChartsSankeyPlotType       >("sankey"       );
  plotTypeMgr()->addTypeT<CQChartsScatterPlotType      >("scatter"      );
  plotTypeMgr()->addTypeT<CQChartsScatterPlot3DType    >("scatter3d"    );
  plotTypeMgr()->addTypeT<CQChartsStripPlotType        >("strip"        );
  plotTypeMgr()->addTypeT<CQChartsSummaryPlotType      >("summary"      );
  plotTypeMgr()->addTypeT<CQChartsSunburstPlotType     >("sunburst"     );
  plotTypeMgr()->addTypeT<CQChartsTablePlotType        >("table"        );
  plotTypeMgr()->addTypeT<CQChartsTreeMapPlotType      >("treemap"      );
  plotTypeMgr()->addTypeT<CQChartsWheelPlotType        >("wheel"        );
  plotTypeMgr()->addTypeT<CQChartsWordCloudPlotType    >("wordCloud"    );
  plotTypeMgr()->addTypeT<CQChartsXYPlotType           >("xy"           );

  //---

  // add column types
  columnTypeMgr()->addTypeT<CQChartsColumnStringType        >(CQBaseModelType::STRING         );
  columnTypeMgr()->addTypeT<CQChartsColumnIntegerType       >(CQBaseModelType::INTEGER        );
  columnTypeMgr()->addTypeT<CQChartsColumnRealType          >(CQBaseModelType::REAL           );
  columnTypeMgr()->addTypeT<CQChartsColumnBooleanType       >(CQBaseModelType::BOOLEAN        );
  columnTypeMgr()->addTypeT<CQChartsColumnTimeType          >(CQBaseModelType::TIME           );
  columnTypeMgr()->addTypeT<CQChartsColumnColorType         >(CQBaseModelType::COLOR          );
  columnTypeMgr()->addTypeT<CQChartsColumnFontType          >(CQBaseModelType::FONT           );
  columnTypeMgr()->addTypeT<CQChartsColumnRectType          >(CQBaseModelType::RECT           );
  columnTypeMgr()->addTypeT<CQChartsColumnLengthType        >(CQBaseModelType::LENGTH         );
  columnTypeMgr()->addTypeT<CQChartsColumnPolygonType       >(CQBaseModelType::POLYGON        );
  columnTypeMgr()->addTypeT<CQChartsColumnPolygonListType   >(CQBaseModelType::POLYGON_LIST   );
  columnTypeMgr()->addTypeT<CQChartsColumnImageType         >(CQBaseModelType::IMAGE          );
  columnTypeMgr()->addTypeT<CQChartsColumnSymbolTypeType    >(CQBaseModelType::SYMBOL         );
  columnTypeMgr()->addTypeT<CQChartsColumnSymbolSizeType    >(CQBaseModelType::SYMBOL_SIZE    );
  columnTypeMgr()->addTypeT<CQChartsColumnFontSizeType      >(CQBaseModelType::FONT_SIZE      );
  columnTypeMgr()->addTypeT<CQChartsColumnPathType          >(CQBaseModelType::PATH           );
  columnTypeMgr()->addTypeT<CQChartsColumnStyleType         >(CQBaseModelType::STYLE          );
  columnTypeMgr()->addTypeT<CQChartsColumnNamePairType      >(CQBaseModelType::NAME_PAIR      );
  columnTypeMgr()->addTypeT<CQChartsColumnConnectionListType>(CQBaseModelType::CONNECTION_LIST);

  //---

  // add data types and widget factories
  static bool typesInitialized = false;

  if (! typesInitialized) {
    typesInitialized = true;

    //---

    // add property types (and editors)
    auto *viewMgr = CQPropertyViewMgrInst;

    viewMgr->addTypeT<CQChartsAlphaPropertyViewType           >("CQChartsAlpha"           );
    viewMgr->addTypeT<CQChartsAnglePropertyViewType           >("CQChartsAngle"           );
    viewMgr->addTypeT<CQChartsArrowDataPropertyViewType       >("CQChartsArrowData"       );
    viewMgr->addTypeT<CQChartsAxisSidePropertyViewType        >("CQChartsAxisSide"        );
    viewMgr->addTypeT<CQChartsAxisTickLabelPlacementPropertyViewType>
                                                               ("CQChartsAxisTickLabelPlacement");
    viewMgr->addTypeT<CQChartsAxisValueTypePropertyViewType   >("CQChartsAxisValueType"   );
    viewMgr->addTypeT<CQChartsBoxDataPropertyViewType         >("CQChartsBoxData"         );
    viewMgr->addTypeT<CQChartsColorPropertyViewType           >("CQChartsColor"           );
    viewMgr->addTypeT<CQChartsColumnPropertyViewType          >("CQChartsColumn"          );
    viewMgr->addTypeT<CQChartsColumnsPropertyViewType         >("CQChartsColumns"         );
    viewMgr->addTypeT<CQChartsColumnNumPropertyViewType       >("CQChartsColumnNum"       );
    viewMgr->addTypeT<CQChartsFillDataPropertyViewType        >("CQChartsFillData"        );
    viewMgr->addTypeT<CQChartsFillPatternPropertyViewType     >("CQChartsFillPattern"     );
    viewMgr->addTypeT<CQChartsFillUnderPosPropertyViewType    >("CQChartsFillUnderPos"    );
    viewMgr->addTypeT<CQChartsFillUnderSidePropertyViewType   >("CQChartsFillUnderSide"   );
    viewMgr->addTypeT<CQChartsFontPropertyViewType            >("CQChartsFont"            );
    viewMgr->addTypeT<CQChartsGeomBBoxPropertyViewType        >("CQChartsGeom::BBox"      );
    viewMgr->addTypeT<CQChartsGeomPointPropertyViewType       >("CQChartsGeom::Point"     );
    viewMgr->addTypeT<CQChartsImagePropertyViewType           >("CQChartsImage"           );
    viewMgr->addTypeT<CQChartsKeyLocationPropertyViewType     >("CQChartsKeyLocation"     );
    viewMgr->addTypeT<CQChartsKeyPressBehaviorPropertyViewType>("CQChartsKeyPressBehavior");
    viewMgr->addTypeT<CQChartsTitleLocationPropertyViewType   >("CQChartsTitleLocation"   );
    viewMgr->addTypeT<CQChartsLengthPropertyViewType          >("CQChartsLength"          );
    viewMgr->addTypeT<CQChartsLineCapPropertyViewType         >("CQChartsLineCap"         );
    viewMgr->addTypeT<CQChartsLineDashPropertyViewType        >("CQChartsLineDash"        );
    viewMgr->addTypeT<CQChartsLineDataPropertyViewType        >("CQChartsLineData"        );
    viewMgr->addTypeT<CQChartsLineJoinPropertyViewType        >("CQChartsLineJoin"        );
    viewMgr->addTypeT<CQChartsMarginPropertyViewType          >("CQChartsMargin"          );
//  viewMgr->addTypeT<CQChartsObjRefPropertyViewType          >("CQChartsObjRef"          );
//  viewMgr->addTypeT<CQChartsObjRefPosPropertyViewType       >("CQChartsObjRefPos"       );
    viewMgr->addTypeT<CQChartsPaletteNamePropertyViewType     >("CQChartsPaletteName"     );
//  viewMgr->addTypeT<CQChartsPointsPropertyViewType          >("CQChartsPoints"          );
    viewMgr->addTypeT<CQChartsPolygonPropertyViewType         >("CQChartsPolygon"         );
    viewMgr->addTypeT<CQChartsPositionPropertyViewType        >("CQChartsPosition"        );
//  viewMgr->addTypeT<CQChartsRealsPropertyViewType           >("CQChartsReals"           );
    viewMgr->addTypeT<CQChartsRectPropertyViewType            >("CQChartsRect"            );
    viewMgr->addTypeT<CQChartsShapeDataPropertyViewType       >("CQChartsShapeData"       );
    viewMgr->addTypeT<CQChartsSidesPropertyViewType           >("CQChartsSides"           );
    viewMgr->addTypeT<CQChartsStrokeDataPropertyViewType      >("CQChartsStrokeData"      );
    viewMgr->addTypeT<CQChartsSymbolDataPropertyViewType      >("CQChartsSymbolData"      );
    viewMgr->addTypeT<CQChartsSymbolPropertyViewType          >("CQChartsSymbol"          );
//  viewMgr->addTypeT<CQChartsSymbolSetPropertyViewType       >("CQChartsSymbolSet"       );
//  viewMgr->addTypeT<CQChartsSymbolTypePropertyViewType      >("CQChartsSymbolType"      );
    viewMgr->addTypeT<CQChartsTextBoxDataPropertyViewType     >("CQChartsTextBoxData"     );
    viewMgr->addTypeT<CQChartsTextDataPropertyViewType        >("CQChartsTextData"        );
    viewMgr->addTypeT<CQChartsUnitsPropertyViewType           >("CQChartsUnits"           );

    viewMgr->addTypeT<CQChartsOptBoolPropertyViewType    >("CQChartsOptBool"    );
    viewMgr->addTypeT<CQChartsOptIntPropertyViewType     >("CQChartsOptInt"     );
    viewMgr->addTypeT<CQChartsOptRealPropertyViewType    >("CQChartsOptReal"    );
    viewMgr->addTypeT<CQChartsOptStringPropertyViewType  >("CQChartsOptString"  );
    viewMgr->addTypeT<CQChartsOptLengthPropertyViewType  >("CQChartsOptLength"  );
    viewMgr->addTypeT<CQChartsOptPositionPropertyViewType>("CQChartsOptPosition");
    viewMgr->addTypeT<CQChartsOptRectPropertyViewType    >("CQChartsOptRect"    );

    //---

    // add widget factories
    auto *widgetMgr = CQWidgetFactoryMgrInst;

    // controls
    widgetMgr->addWidgetFactoryT<CQChartsPlotControlFrame>("CQChartsPlotControlFrame");
    widgetMgr->addWidgetFactoryT<CQChartsPlotRealControl> ("CQChartsPlotRealControl");
    widgetMgr->addWidgetFactoryT<CQChartsPlotIntControl>  ("CQChartsPlotIntControl");
    widgetMgr->addWidgetFactoryT<CQChartsPlotTimeControl> ("CQChartsPlotTimeControl");
    widgetMgr->addWidgetFactoryT<CQChartsPlotValueControl>("CQChartsPlotValueControl");

    // model view
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelViewHolder   >("CQChartsModelViewHolder");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelDetailsTable >("CQChartsModelDetailsTable");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelDetailsWidget>("CQChartsModelDetailsWidget");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelTableControl >("CQChartsModelTableControl");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelTable        >("CQChartsModelTable");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelDataWidget   >("CQChartsModelDataWidget");

    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsModelColumnDataControl>("CQChartsModelColumnDataControl");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsModelExprControl>("CQChartsModelExprControl");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsModelControl>("CQChartsModelControl");
#if 0
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelFoldControl>("CQChartsModelFoldControl");
#endif

    // properties
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsGlobalPropertiesWidget>("CQChartsGlobalPropertiesWidget");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsViewPropertiesControl>("CQChartsViewPropertiesControl");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsViewPropertiesWidget>("CQChartsViewPropertiesWidget");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsPlotPropertiesControl>("CQChartsPlotPropertiesControl");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsPlotPropertiesWidget>("CQChartsPlotPropertiesWidget");

    // plots
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsPlotTableControl>("CQChartsPlotTableControl");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsPlotTable>("CQChartsPlotTable");

    // annotations
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsViewAnnotationsControl>("CQChartsViewAnnotationsControl");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsPlotAnnotationsControl>("CQChartsPlotAnnotationsControl");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsViewAnnotationsTable>("CQChartsViewAnnotationsTable");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsPlotAnnotationsTable>("CQChartsPlotAnnotationsTable");

    // colors
    widgetMgr->addWidgetFactoryT<CQColorsEditList>("CQColorsEditList");
    widgetMgr->addWidgetFactoryT<CQChartsPaletteCanvas>("CQChartsPaletteCanvas");
    widgetMgr->addWidgetFactoryT<CQChartsPaletteControl>("CQChartsPaletteControl");
    widgetMgr->addWidgetFactoryT<CQChartsInterfaceControl>("CQChartsInterfaceControl");
    //widgetMgr->addWidgetFactoryT<CQColorsEditControl>("CQColorsEditControl");

    // misc
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelChooser       >("CQChartsModelChooser");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelFilterControl >("CQChartsModelFilterControl");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelFlattenControl>("CQChartsModelFlattenControl");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsModelFoldControl   >("CQChartsModelFoldControl");

    // other edits (all ?)
    widgetMgr->addWidgetFactoryT<CQChartsAlphaEdit   >("CQChartsAlphaEdit"     );
    widgetMgr->addWidgetFactoryT<CQChartsAngleEdit   >("CQChartsAngleEdit"     );
    widgetMgr->addWidgetFactoryT<CQChartsColorEdit   >("CQChartsColorEdit"     );
    widgetMgr->addWidgetFactoryT<CQChartsColumnEdit  >("CQChartsColumnEdit"    );
    widgetMgr->addWidgetFactoryT<CQChartsColumnsEdit >("CQChartsColumnsEdit"   );
    widgetMgr->addWidgetFactoryT<CQChartsFontEdit    >("CQChartsFontEdit"      );
    widgetMgr->addWidgetFactoryT<CQChartsImageEdit   >("CQChartsImageEdit"     );
    widgetMgr->addWidgetFactoryT<CQChartsLengthEdit  >("CQChartsLengthEdit"    );
    widgetMgr->addWidgetFactoryT<CQChartsLineEdit    >("CQChartsLineEdit"      );
    widgetMgr->addWidgetFactoryT<CQChartsPolygonEdit >("CQChartsPolygonEdit"   );
    widgetMgr->addWidgetFactoryT<CQChartsPositionEdit>("CQChartsPositionEdit"  );
    widgetMgr->addWidgetFactoryT<CQChartsRectEdit    >("CQChartsRectEdit"      );
    widgetMgr->addWidgetFactoryT<CQChartsUnitsEdit   >("CQChartsUnitsEdit"     );

    // symbols
    widgetMgr->addWidgetFactoryT<CQChartsSymbolTypeEdit>("CQChartsSymbolTypeEdit");
    widgetMgr->addWidgetFactoryT<CQChartsSymbolSetEdit> ("CQChartsSymbolSetEdit" );
    widgetMgr->addWidgetFactoryT<CQChartsSymbolEdit    >("CQChartsSymbolEdit"    );

    widgetMgr->addWidgetFactoryNoArgsT<CQChartsSymbolSetsList    >("CQChartsSymbolSetsList"    );
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsSymbolsList       >("CQChartsSymbolsList"       );
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsSymbolsListControl>("CQChartsSymbolsListControl");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsSymbolEditor      >("CQChartsSymbolEditor"      );

    widgetMgr->addWidgetFactoryNoArgsT<CQChartsViewLayerTable>("CQChartsViewLayerTable");
    widgetMgr->addWidgetFactoryNoArgsT<CQChartsPlotLayerTable>("CQChartsPlotLayerTable");

    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsPlotPropertyEditGroup>("CQChartsPlotPropertyEditGroup");
    widgetMgr->addWidgetFactoryNoArgsT<
      CQChartsPlotPropertyEdit>("CQChartsPlotPropertyEdit");

    // basic
    widgetMgr->addWidgetFactoryT<CQIntRangeSlider   >("CQIntRangeSlider");
    widgetMgr->addWidgetFactoryT<CQDoubleRangeSlider>("CQDoubleRangeSlider");
    widgetMgr->addWidgetFactoryT<CQTimeRangeSlider  >("CQTimeRangeSlider");

    widgetMgr->addWidgetFactoryT<CQAlignEdit   >("CQAlignEdit"   );
    widgetMgr->addWidgetFactoryT<CQCheckBox    >("CQCheckBox"    );
    widgetMgr->addWidgetFactoryT<CQColorEdit   >("CQColorEdit"   );
    widgetMgr->addWidgetFactoryT<CQCustomCombo >("CQCustomCombo" );
    widgetMgr->addWidgetFactoryT<CQDragLabel   >("CQDragLabel"   );
    widgetMgr->addWidgetFactoryT<CQFilename    >("CQFilename"    );
    widgetMgr->addWidgetFactoryT<CQFontEdit    >("CQFontEdit"    );
    widgetMgr->addWidgetFactoryT<CQGroupBox    >("CQGroupBox"    );
    widgetMgr->addWidgetFactoryT<CQIconCombo   >("CQIconCombo"   );
    widgetMgr->addWidgetFactoryT<CQIntegerSpin >("CQIntegerSpin" );
    widgetMgr->addWidgetFactoryT<CQRadioButtons>("CQRadioButtons");
    widgetMgr->addWidgetFactoryT<CQRealSpin    >("CQRealSpin"    );
    widgetMgr->addWidgetFactoryT<CQSwitch      >("CQSwitch"      );
    widgetMgr->addWidgetFactoryT<CQTableWidget >("CQTableWidget" );
    widgetMgr->addWidgetFactoryT<CQTabWidget   >("CQTabWidget"   );
    widgetMgr->addWidgetFactoryT<CQWinWidget   >("CQWinWidget"   );
  }

  //---

  // add symbol sets
  auto *allSymbolSet = createSymbolSet("all");

  auto addSymbol = [&](CQChartsSymbolSet *symbolSet, CQChartsSymbolType::Type type, bool filled) {
    CQChartsSymbol symbol(type);
    symbol.setFilled(filled);
    symbolSet->addSymbol(symbol);
  };

  addSymbol(allSymbolSet, CQChartsSymbolType::Type::DOT      , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::PLUS     , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::CROSS    , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::Y        , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::Z        , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::TRIANGLE , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::ITRIANGLE, false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::BOX      , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::DIAMOND  , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::STAR5    , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::STAR6    , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::CIRCLE   , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::PENTAGON , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::IPENTAGON, false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::HEXAGON  , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::OCTAGON  , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::HLINE    , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::VLINE    , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::PAW      , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::HASH     , false);
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::DOT      , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::PLUS     , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::CROSS    , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::Y        , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::Z        , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::TRIANGLE , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::ITRIANGLE, true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::BOX      , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::DIAMOND  , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::STAR5    , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::STAR6    , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::CIRCLE   , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::PENTAGON , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::IPENTAGON, true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::HEXAGON  , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::OCTAGON  , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::HLINE    , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::VLINE    , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::PAW      , true );
  addSymbol(allSymbolSet, CQChartsSymbolType::Type::HASH     , true );

  auto *outlineSymbolSet = createSymbolSet("outline");

  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::PLUS     , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::CROSS    , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::Y        , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::Z        , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::TRIANGLE , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::CIRCLE   , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::BOX      , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::DIAMOND  , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::STAR5    , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::PENTAGON , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::HASH     , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::ITRIANGLE, false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::STAR6    , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::IPENTAGON, false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::HEXAGON  , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::OCTAGON  , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::DOT      , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::PAW      , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::HLINE    , false);
  addSymbol(outlineSymbolSet, CQChartsSymbolType::Type::VLINE    , false);

  auto *filledSymbolSet = createSymbolSet("filled");

  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::TRIANGLE , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::CIRCLE   , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::BOX      , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::DIAMOND  , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::STAR5    , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::PENTAGON , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::ITRIANGLE, true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::STAR6    , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::IPENTAGON, true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::HEXAGON  , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::OCTAGON  , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::PLUS     , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::CROSS    , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::Y        , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::Z        , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::HASH     , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::DOT      , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::PAW      , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::HLINE    , true);
  addSymbol(filledSymbolSet, CQChartsSymbolType::Type::VLINE    , true);

  //auto *charSymbolSet = createSymbolSet("char");

  //charSymbolSet->addSymbol(CQChartsSymbol(CQChartsSymbol::CharData("\u2639", "white frown")));
  //charSymbolSet->addSymbol(CQChartsSymbol(CQChartsSymbol::CharData("\u263a", "white smile")));
  //charSymbolSet->addSymbol(CQChartsSymbol(CQChartsSymbol::CharData("\u263b", "black smile")));
}

//---

void
CQCharts::
addExitTimer(double secs)
{
  exitTimer_ = new QTimer(this);

  connect(exitTimer_, SIGNAL(timeout()), this, SLOT(exitSlot()));

  exitTimer_->start(int(1000.0*secs));
}

void
CQCharts::
exitSlot()
{
  exit(0);
}

//---

void
CQCharts::
getModelTypeNames(QStringList &names) const
{
  names << CQChartsFileTypeUtil::fileTypeNames();
}

//---

bool
CQCharts::
isPlotType(const QString &name) const
{
  return plotTypeMgr()->isType(name);
}

CQChartsPlotType *
CQCharts::
plotType(const QString &name) const
{
  return plotTypeMgr()->type(name);
}

void
CQCharts::
getPlotTypes(PlotTypes &types) const
{
  plotTypeMgr()->getTypes(types);
}

void
CQCharts::
getPlotTypeNames(QStringList &names, QStringList &descs) const
{
  plotTypeMgr()->getTypeNames(names, descs);
}

//---

QColor
CQCharts::
interpColor(const Color &c, const ColorInd &ind) const
{
  return interpColorValueI(c, /*ig*/0, /*ng*/1, ind.value(), ind.c);
}

QColor
CQCharts::
interpColorValueI(const Color &c, int ig, int ng, double value, const QColor &ic) const
{
  if (! c.isValid())
    return QColor();

  if      (c.type() == Color::Type::COLOR)
    return c.color();
  else if (c.type() == Color::Type::PALETTE || c.type() == Color::Type::PALETTE_VALUE) {
    if      (c.hasPaletteIndex()) {
      int ind = c.getPaletteIndex();

      if (c.type() == Color::Type::PALETTE_VALUE)
        return interpIndPaletteColor(ind, c.value(), c.isScale(), c.isInvert());
      else
        return interpIndPaletteColorValue(ind, ig, ng, value, c.isScale(), c.isInvert());
    }
    else if (c.hasPaletteName()) {
      QString name;

      if (c.getPaletteName(name)) {
        if (c.type() == Color::Type::PALETTE_VALUE)
          return interpNamePaletteColor(name, c.value(), c.isScale(), c.isInvert());
        else
          return interpNamePaletteColorValue(name, ig, ng, value, c.isScale(), c.isInvert());
      }
      else {
        if (c.type() == Color::Type::PALETTE_VALUE)
          return interpPaletteColor(ColorInd(c.value()), c.isScale(), c.isInvert());
        else
          return interpPaletteColorValue(ig, ng, value, c.isScale(), c.isInvert());
      }
    }
    else {
      if (c.type() == Color::Type::PALETTE_VALUE)
        return interpPaletteColor(ColorInd(c.value()), c.isScale(), c.isInvert());
      else
        return interpPaletteColorValue(ig, ng, value, c.isScale(), c.isInvert());
    }
  }
  else if (c.type() == Color::Type::INDEXED ||
           c.type() == Color::Type::INDEXED_VALUE) {
    if      (c.hasPaletteIndex()) {
      int ind = c.getPaletteIndex();

      if (c.type() == Color::Type::INDEXED_VALUE)
        return indexIndPaletteColor(ind, int(c.value()), ng);
      else
        return indexIndPaletteColor(ind, ig, ng);
    }
    else if (c.hasPaletteName()) {
      QString name;

      if (c.getPaletteName(name)) {
        if (c.type() == Color::Type::INDEXED_VALUE)
          return indexNamePaletteColor(name, int(c.value()), ng);
        else
          return indexNamePaletteColor(name, ig, ng);
      }
      else {
        if (c.type() == Color::Type::INDEXED_VALUE)
          return indexPaletteColor(int(c.value()), ng);
        else
          return indexPaletteColor(ig, ng);
      }
    }
    else {
      if (c.type() == Color::Type::INDEXED_VALUE)
        return indexPaletteColor(int(c.value()), ng);
      else
        return indexPaletteColor(ig, ng);
    }
  }
  else if (c.type() == Color::Type::INTERFACE ||
           c.type() == Color::Type::INTERFACE_VALUE) {
    if (c.type() == Color::Type::INTERFACE_VALUE)
      return interpThemeColor(ColorInd(c.value()));
    else
      return interpThemeColor(ColorInd(value));
  }
  else if (c.type() == Color::Type::CONTRAST ||
           c.type() == Color::Type::CONTRAST_VALUE) {
    value = 1.0; // no interp ?

    auto cc = contrastColor();

    if (! cc.isValid()) {
      if (c.type() == Color::Type::CONTRAST_VALUE)
        return interpThemeColor(ColorInd(1.0 - c.value()));
      else
        return interpThemeColor(ColorInd(1.0 - value));
    }

    auto c1 = CQChartsUtil::bwColor(cc);
    auto c2 = CQChartsUtil::bwColor(c1);

    if (c.type() == Color::Type::CONTRAST_VALUE)
      c1 = CQChartsUtil::blendColors(c1, c2, CMathUtil::clamp(c.value(), 0.0, 1.0));
    else
      c1 = CQChartsUtil::blendColors(c1, c2, CMathUtil::clamp(value, 0.0, 1.0));

    return c1;
  }
  else if (c.type() == Color::Type::MODEL ||
           c.type() == Color::Type::MODEL_VALUE) {
    if (c.type() == Color::Type::MODEL_VALUE)
      return interpModelColor(c, c.value());
    else
      return interpModelColor(c, value);
  }
  else if (c.type() == Color::Type::LIGHTER ||
           c.type() == Color::Type::LIGHTER_VALUE) {
    if (c.type() == Color::Type::LIGHTER_VALUE)
      return ic.lighter(int(150 + c.value()));
    else
      return ic.lighter();
  }
  else if (c.type() == Color::Type::DARKER ||
           c.type() == Color::Type::DARKER_VALUE) {
    if (c.type() == Color::Type::DARKER_VALUE)
      return ic.darker(int(150 + c.value()));
    else
      return ic.darker();
  }

  return Qt::black; // TODO: default to interface color
}

//---

void
CQCharts::
setPlotTheme(const CQChartsThemeName &themeName)
{
  CQChartsUtil::testAndSet(plotTheme_, themeName, [&]() { emit themeChanged(); } );
}

bool
CQCharts::
isDark() const
{
  return interfaceTheme()->isDark();
}

void
CQCharts::
setDark(bool b)
{
  interfaceTheme()->setDark(b);

  emit interfaceThemeChanged();
}

//---

QColor
CQCharts::
interpPaletteColor(const ColorInd &ind, bool scale, bool invert) const
{
  return interpIndPaletteColor(/*palette_ind*/-1, ind.value(), scale, invert);
}

QColor
CQCharts::
interpIndPaletteColor(int ind, int i, int n, bool scale, bool invert) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpIndPaletteColor(ind, r, scale, invert);
}

QColor
CQCharts::
interpIndPaletteColor(int ind, double r, bool scale, bool invert) const
{
  return interpIndPaletteColorValue(ind, 0, 1, r, scale, invert);
}

QColor
CQCharts::
interpNamePaletteColor(const QString &name, double r, bool scale, bool invert) const
{
  return interpNamePaletteColorValue(name, 0, 1, r, scale, invert);
}

QColor
CQCharts::
interpGroupPaletteColor(const ColorInd &ig, const ColorInd &iv, bool scale, bool invert) const
{
  return themeGroupPalette(ig.i, ig.n)->getColor(iv.value(), scale, invert);
}

QColor
CQCharts::
interpPaletteColorValue(int ig, int ng, int i, int n, bool scale, bool invert) const
{
  double r = CMathUtil::norm(i, 0, n - 1);

  return interpIndPaletteColorValue(/*palette_ind*/-1, ig, ng, r, scale, invert);
}

QColor
CQCharts::
interpPaletteColorValue(int ig, int ng, double r, bool scale, bool invert) const
{
  return interpIndPaletteColorValue(/*palette_ind*/-1, ig, ng, r, scale, invert);
}

QColor
CQCharts::
interpIndPaletteColorValue(int ind, int /*ig*/, int /*ng*/, double r,
                           bool scale, bool invert) const
{
  // if ind unset then use default palette number
  if (ind < 0)
    ind = 0;

  auto *palette = this->themePalette(ind);

#if 0
  if (palette->isDistinct() && ng > 0) {
    int nc = palette->numDefinedColors();
    assert(nc > 0);

    int i1 = (ig % nc);

    double r1 = CMathUtil::norm(i1, 0, nc - 1);

    return palette->getColor(r1, /*scale*/false);
  }
#endif

  return palette->getColor(r, scale, invert);
}

QColor
CQCharts::
interpNamePaletteColorValue(const QString &name, int /*ig*/, int /*ng*/, double r,
                            bool scale, bool invert) const
{
  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  if (! palette) return QColor(); // assert ?

  return palette->getColor(r, scale, invert);
}

QColor
CQCharts::
indexPaletteColor(int i, int n) const
{
  return indexIndPaletteColor(0, i, n);
}

QColor
CQCharts::
indexIndPaletteColor(int ind, int i, int n) const
{
  // if ind unset then use default palette number
  if (ind < 0)
    ind = 0;

  auto *palette = this->themePalette(ind);

  return palette->getColor(i, n, CQColorsPalette::WrapMode::REPEAT);
}

QColor
CQCharts::
indexNamePaletteColor(const QString &name, int i, int n) const
{
  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  if (! palette) return QColor(); // assert ?

  return palette->getColor(i, n, CQColorsPalette::WrapMode::REPEAT);
}

QColor
CQCharts::
interpThemeColor(const ColorInd &ind) const
{
  return this->interfaceTheme()->interpColor(ind.value(), /*scale*/true);
}

QColor
CQCharts::
interpInterfaceColor(double r) const
{
  return this->interfaceTheme()->interpColor(r, /*scale*/true);
}

CQColorsPalette *
CQCharts::
themeGroupPalette(int ig, int /*ng*/) const
{
  if (ig < 0)
    ig = 0;

  return theme()->palette(ig);
}

CQColorsPalette *
CQCharts::
themePalette(int ind) const
{
  // if ind unset then use default palette number
  if (ind < 0)
    ind = 0;

  return theme()->palette(ind);
}

const CQColorsTheme *
CQCharts::
theme() const
{
  return plotTheme().obj();
}

CQColorsTheme *
CQCharts::
theme()
{
  return plotTheme().obj();
}

CQChartsColor
CQCharts::
adjustDefaultPalette(const Color &c, const QString &defaultPalette) const
{
  assert(defaultPalette.length());

  if ((c.type() == Color::Type::PALETTE || c.type() == Color::Type::PALETTE_VALUE) &&
      ! c.hasPaletteIndex() && ! c.hasPaletteName()) {
    auto c1 = c;

    c1.setPaletteName(defaultPalette);

    return c1;
  }

  return c;
}

//---

QColor
CQCharts::
interpModelColor(const Color &c, double value) const
{
  int ir, ig, ib;

  c.getModelRGB(ir, ig, ib);

  double r = CQColorsPalette::interpModel(ir, value);
  double g = CQColorsPalette::interpModel(ig, value);
  double b = CQColorsPalette::interpModel(ib, value);

  return CQChartsUtil::rgbToColor(r, g, b);
}

//---

CQChartsModelData *
CQCharts::
initModelData(ModelP &model)
{
  int ind;

  if (! getModelInd(model.data(), ind))
    ind = addModelData(model);

  return getModelDataByInd(ind);
}

CQChartsModelData *
CQCharts::
getModelData(const ModelP &model) const
{
  return getModelData(model.data());
}

CQChartsModelData *
CQCharts::
getModelData(const QAbstractItemModel *model) const
{
  if (! model)
    return nullptr;

  int ind;

  if (! getModelInd(model, ind))
    return nullptr;

  return getModelDataByInd(ind);
}

void
CQCharts::
setCurrentModelData(CQChartsModelData *modelData)
{
  if (modelData)
    setCurrentModelInd(modelData->ind());
  else
    setCurrentModelInd(-1);
}

void
CQCharts::
setCurrentModelInd(int ind)
{
  currentModelInd_ = ind;

  emit currentModelChanged(currentModelInd_);
}

CQChartsModelData *
CQCharts::
currentModelData() const
{
  if (modelDatas_.empty())
    return nullptr;

  if (currentModelInd_ >= 0 ) {
    for (auto &modelData : modelDatas_) {
      if (modelData->ind() == currentModelInd_)
        return modelData;
    }
  }

  return modelDatas_.back();
}

int
CQCharts::
addModelData(ModelP &model)
{
  auto *modelData = new CQChartsModelData(this, model);

  int ind;

  bool rc = assignModelInd(model.data(), ind);

  assert(rc);

  modelData->setInd(ind);

  connect(modelData, SIGNAL(dataChanged()), this, SIGNAL(modelDataDataChanged()));
//connect(modelData, SIGNAL(modelChanged()), this, SIGNAL(modelDataContentsChanged()));
//connect(modelData, SIGNAL(currentModelChanged()), this, SIGNAL(modelDataCurrentModelChanged()));

  //---

  modelDatas_.push_back(modelData);

  ind = modelData->ind();

  emit modelDataAdded(ind);
  emit modelDataChanged();

  return ind;
}

bool
CQCharts::
removeModel(ModelP &model)
{
  auto *modelData = getModelData(model);
  if (! modelData) return false;

  return removeModelData(modelData);
}

bool
CQCharts::
removeModelData(CQChartsModelData *modelData)
{
  int ind = modelData->ind();

  size_t i = 0;
  auto   n = modelDatas_.size();

  for ( ; i < n; ++i) {
    if (modelDatas_[i] == modelData)
      break;
  }

  if (i >= n)
    return false;

  ++i;

  for ( ; i < n; ++i)
    modelDatas_[i - 1] = modelDatas_[i];

  modelDatas_.pop_back();

  modelData->emitDeleted();

  delete modelData;

  emit modelDataRemoved(ind);
  emit modelDataChanged();

  return true;
}

bool
CQCharts::
getModelInd(const QAbstractItemModel *model, int &ind) const
{
  ind = -1;

  if (! model)
    return false;

  bool ok;

  auto lind = CQChartsVariant::toInt(model->property("modelInd"), ok);
  if (! ok) return false;

  ind = int(lind);

  return true;
}

bool
CQCharts::
assignModelInd(QAbstractItemModel *model, int &ind)
{
  ind = ++lastModelInd_;

  return setModelInd(model, ind);
}

bool
CQCharts::
setModelInd(QAbstractItemModel *model, int ind)
{
  if (! model)
    return false;

  model->setProperty("modelInd", ind);

  return true;
}

CQChartsModelData *
CQCharts::
getModelDataByInd(int ind) const
{
  for (auto &modelData : modelDatas_)
    if (modelData->ind() == ind)
      return modelData;

  return nullptr;
}

CQChartsModelData *
CQCharts::
getModelDataById(const QString &id) const
{
  for (auto &modelData : modelDatas_)
    if (modelData->id() == id)
      return modelData;

  for (auto &modelData : modelDatas_)
    if (modelData->defId() == id)
      return modelData;

  return nullptr;
}

void
CQCharts::
getModelDatas(ModelDatas &modelDatas) const
{
  modelDatas = modelDatas_;
}

void
CQCharts::
setModelName(CQChartsModelData *modelData, const QString &name)
{
  modelData->setName(name);
}

void
CQCharts::
setModelFileName(CQChartsModelData *modelData, const QString &filename)
{
  modelData->setFilename(filename);
}

//---

CQChartsView *
CQCharts::
addView(const QString &id)
{
  auto *view = createView();

  if (id.length())
    view->setId(id);

  addView(view);

  return view;
}

void
CQCharts::
addView(CQChartsView *view)
{
  connect(view, SIGNAL(plotAdded(CQChartsPlot *)), this, SIGNAL(plotAdded(CQChartsPlot *)));

  //---

  auto id = view->id();

  if (id == "")
    id = QString("view:%1").arg(views_.size() + 1);

  assert(! getView(id));

  view->setId(id);

  view->setObjectName(id);

  views_[id] = view;

  emit viewAdded(view);
}

CQChartsView *
CQCharts::
createView()
{
  auto *view = new CQChartsView(this);

  return view;
}

void
CQCharts::
deleteView(CQChartsView *view)
{
  delete view;
}

CQChartsView *
CQCharts::
getView(const QString &id) const
{
  auto p = views_.find(id);

  if (p == views_.end())
    return nullptr;

  return (*p).second;
}

CQChartsView *
CQCharts::
currentView() const
{
  if (views_.empty())
    return nullptr;

  return views_.rbegin()->second;
}

void
CQCharts::
getViews(Views &views) const
{
  for (const auto &view : views_)
    views.push_back(view.second);
}

void
CQCharts::
getViewIds(QStringList &names) const
{
  for (const auto &view : views_)
    names.push_back(view.second->id());
}

void
CQCharts::
removeView(CQChartsView *view)
{
  emit viewRemoved(view);

  views_.erase(view->id());
}

//---

CQChartsWindow *
CQCharts::
createWindow(CQChartsView *view)
{
  auto *window = CQChartsWindowMgrInst->createWindow(view);

  emit windowCreated(window);

  return window;
}

void
CQCharts::
deleteWindow(CQChartsWindow *window)
{
  emit windowRemoved(window);

  CQChartsWindowMgrInst->removeWindow(window);
}

//---

void
CQCharts::
addProc(ProcType type, const QString &name, const QString &args, const QString &body)
{
  typeProcs_[type][name] = ProcData(name, args, body);
}

void
CQCharts::
removeProc(ProcType type, const QString &name)
{
  typeProcs_[type].erase(name);
}

void
CQCharts::
getProcNames(ProcType type, QStringList &names) const
{
  auto pt = typeProcs_.find(type);
  if (pt == typeProcs_.end()) return;

  const auto &procs = (*pt).second;

  for (const auto &proc : procs) {
    names.push_back(proc.first);
  }
}

bool
CQCharts::
getProcData(ProcType type, const QString &name, QString &args, QString &body) const
{
  auto pt = typeProcs_.find(type);
  if (pt == typeProcs_.end()) return false;

  const auto &procs = (*pt).second;

  auto pn = procs.find(name);
  if (pn == procs.end()) return false;

  args = (*pn).second.args;
  body = (*pn).second.body;

  return true;
}

const CQCharts::Procs &
CQCharts::
procs(ProcType type) const
{
  static Procs noProcs;

  auto pt = typeProcs_.find(type);
  if (pt == typeProcs_.end()) return noProcs;

  return (*pt).second;
}

//---

void
CQCharts::
emitModelTypeChanged(int modelId)
{
  emit modelTypeChanged(modelId);
}

//---

void
CQCharts::
setItemIsStyle(CQPropertyViewItem *item)
{
  item->setProperty("style_prop", true);
}

bool
CQCharts::
getItemIsStyle(const CQPropertyViewItem *item)
{
  return item->property("style_prop").isValid();
}

void
CQCharts::
setItemIsHidden(CQPropertyViewItem *item)
{
  item->setHidden(true);

  item->setProperty("hidden_prop", true);
}

bool
CQCharts::
getItemIsHidden(const CQPropertyViewItem *item)
{
  return item->property("hidden_prop").isValid();
}

//---

bool
CQCharts::
hasSymbolSet(const QString &name) const
{
  return symbolSetMgr()->hasSymbolSet(name);
}

CQChartsSymbolSet *
CQCharts::
createSymbolSet(const QString &name)
{
  auto *symbolSet = new CQChartsSymbolSet(name);

  symbolSetMgr()->addSymbolSet(symbolSet);

  return symbolSet;
}

//---

void
CQCharts::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                    const QString &desc, bool hidden=false) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  addProp("", "viewKey"      , "", "View key");
  addProp("", "maxSymbolSize", "", "Max symbol size");
  addProp("", "maxFontSize"  , "", "Max font size");
  addProp("", "maxLineWidth" , "", "Max line width");
}

CQPropertyViewItem *
CQCharts::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  assert(CQUtil::hasProperty(object, name));

  return propertyModel()->addProperty(path, object, name, alias);
}

void
CQCharts::
propertyItemChanged(QObject *, const QString &)
{
}

//---

CQChartsLoadModelDlg *
CQCharts::
loadModelDlg()
{
  if (! loadModelDlg_)
    loadModelDlg_ = new CQChartsLoadModelDlg(this);

  loadModelDlg_->show();
  loadModelDlg_->raise();

  return loadModelDlg_;
}

CQChartsManageModelsDlg *
CQCharts::
manageModelsDlg()
{
  if (! manageModelsDlg_)
    manageModelsDlg_ = new CQChartsManageModelsDlg(this);

  manageModelsDlg_->show();
  manageModelsDlg_->raise();

  return manageModelsDlg_;
}

CQChartsEditModelDlg *
CQCharts::
editModelDlg(CQChartsModelData *modelData)
{
  if (! editModelDlg_ || editModelDlg_->model() != modelData->currentModel()) {
    delete editModelDlg_;

    editModelDlg_ = new CQChartsEditModelDlg(this, modelData);
  }

  editModelDlg_->show();
  editModelDlg_->raise();

  return editModelDlg_;
}

CQChartsCreatePlotDlg *
CQCharts::
createPlotDlg(CQChartsModelData *modelData)
{
  if (! createPlotDlg_ || createPlotDlg_->model() != modelData->currentModel()) {
    delete createPlotDlg_;

    createPlotDlg_ = new CQChartsCreatePlotDlg(this, modelData);
  }

  createPlotDlg_->show();
  createPlotDlg_->raise();

  return createPlotDlg_;
}

//---

void
CQCharts::
addPath(const QString &path)
{
  auto path1 = path.trimmed();
  assert(path1 != "");

  while (path1[path.length() - 1] == "/")
    path1 = path1.mid(0, path.length() - 1);

  if (path1 == "")
    path1 = "/";

  pathList_.push_back(path1);
}

QString
CQCharts::
lookupFile(const QString &fileName) const
{
  auto fileName1 = fileName.trimmed();
  if (fileName1 == "") return "";

  if (fileName1[0] == "/")
    return fileName1;

  for (const auto &path : pathList_) {
    auto fileName2 = path + "/" + fileName;

    if (QFileInfo(fileName2).exists())
      return fileName2;
  }

  if (QFileInfo(fileName1).exists())
    return fileName1;

  return "";
}

//---

void
CQCharts::
errorMsg(const QString &msg) const
{
  std::cerr << msg.toStdString() << "\n";
}
