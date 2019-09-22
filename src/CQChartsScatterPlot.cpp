#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsValueSet.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsDataLabel.h>
#include <CQChartsVariant.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>
#include <CMathCorrelation.h>
#include <CMathRound.h>

#include <QMenu>

CQChartsScatterPlotType::
CQChartsScatterPlotType()
{
}

void
CQChartsScatterPlotType::
addParameters()
{
  startParameterGroup("Scatter");

  // columns
  addColumnParameter("x", "X", "xColumn").
    setRequired().setNumeric().setTip("X Value Column");
  addColumnParameter("y", "Y", "yColumn").
    setRequired().setNumeric().setTip("Y Value Column");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Optional Name Column").setString();
  addColumnParameter("label", "Label", "labelColumn").
    setTip("Custom Label").setString();

  //--

  // options
  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("SYMBOLS"   , int(CQChartsScatterPlot::PlotType::SYMBOLS   )).
    addNameValue("GRID_CELLS", int(CQChartsScatterPlot::PlotType::GRID_CELLS)).
    setTip("Plot type");

  addBoolParameter("pointLabels", "Point Labels", "pointLabels").
    setTip("Show Label at Point");

  endParameterGroup();

  //---

  addMappingParameters();

  //---

  CQChartsPointPlotType::addParameters();
}

QString
CQChartsScatterPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Scatter Plot").
    h3("Summary").
     p("Draws scatter plot of x, y points with support for grouping and customization of "
       "point symbol type, symbol size and symbol color.").
     p("The points can have individual labels in which case the label font size can "
       "also be customized.").
    h3("Grouping").
     p("The points can be grouped by specifying a " + B("Name") + " column, all values "
       "with the same name are placed in that group and will be default colored by the "
       "group index.").
    h3("Columns").
     p("The points are specified by the " + B("X") + " and " + B("Y") + " columns.").
     p("An optional " + B("SymbolType") + " column can be specified to supply the type of the "
       "symbol drawn at the point. An optional " + B("SymbolSize") + " column can be specified "
       "to supply the size of the symbol drawn at the point. An optional " + B("Color") + " "
       "column can be specified to supply the fill color of the symbol drawn at the point.").
     p("An optional point label can be specified using the " + B("Label") + " column or the " +
       B("Name") + " column. The font size of the label can be specified using the " +
       B("FontSize") + " column.").
    h3("Customization").
     p("When there a lot of points in the data an NxM grid can be created as an alternative "
       "display where the grid cell is colored by the number of points in each cell.").
     p("The points can have an overlaid best fit line, convex hull, statistic lines "
       " and/or a density map.").
     p("The x and y axes can have rug lines, density plot and/or whisker plot.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/scatterplot.png"));
}

CQChartsPlot *
CQChartsScatterPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsScatterPlot(view, model);
}

//---

CQChartsScatterPlot::
CQChartsScatterPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPointPlot(view, view->charts()->plotType("scatter"), model),
 CQChartsObjPointData        <CQChartsScatterPlot>(this),
 CQChartsObjBestFitShapeData <CQChartsScatterPlot>(this),
 CQChartsObjStatsLineData    <CQChartsScatterPlot>(this),
 CQChartsObjHullShapeData    <CQChartsScatterPlot>(this),
 CQChartsObjRugPointData     <CQChartsScatterPlot>(this),
 CQChartsObjGridCellShapeData<CQChartsScatterPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setHullFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setRugSymbolType(CQChartsSymbol::Type::NONE);
  setRugSymbolSize(CQChartsLength("5px"));

  setBestFit(false);
  setBestFitStrokeDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));
  setBestFitFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setBestFitFillAlpha(0.5);

  setStatsLines(false);
  setStatsLinesDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setGridCellFilled (true);
  setGridCellStroked(true);
  setGridCellStrokeColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.1));

  //---

  addAxes();

  addKey();

  addTitle();
}

CQChartsScatterPlot::
~CQChartsScatterPlot()
{
  for (const auto &ghull : groupHull_)
    delete ghull.second;
}

//------

void
CQChartsScatterPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { resetAxes(); updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { resetAxes(); updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setGridNumX(int n)
{
  CQChartsUtil::testAndSet(gridData_.nx, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setGridNumY(int n)
{
  CQChartsUtil::testAndSet(gridData_.ny, n, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setSkipBad(bool b)
{
  CQChartsUtil::testAndSet(skipBad_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setSymbolMapKey(bool b)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.displayed, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyAlpha(double a)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.alpha, a, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyMargin(double m)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.margin, m, [&]() { drawObjs(); } );
}

//---

void
CQChartsScatterPlot::
setSymbols(bool)
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      CQChartsScatterPlot *splot = qobject_cast<CQChartsScatterPlot *>(plot);

      if (splot)
        splot->plotType_ = PlotType::SYMBOLS;
    });

    updateRangeAndObjs();
  }
  else {
    CQChartsUtil::testAndSet(plotType_, PlotType::SYMBOLS, [&]() { updateRangeAndObjs(); } );
  }
}

void
CQChartsScatterPlot::
setGridCells(bool b)
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      CQChartsScatterPlot *splot = qobject_cast<CQChartsScatterPlot *>(plot);

      if (splot)
        splot->plotType_ = (b ? PlotType::GRID_CELLS : PlotType::SYMBOLS);
    });

    updateRangeAndObjs();
  }
  else {
    CQChartsUtil::testAndSet(plotType_,
     (b ? PlotType::GRID_CELLS : PlotType::SYMBOLS), [&]() { updateRangeAndObjs(); } );
  }
}

//---

void
CQChartsScatterPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.visible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitOutliers(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.includeOutliers, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitDeviation(bool b)
{
  CQChartsUtil::testAndSet(bestFitData_.showDeviation, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setBestFitOrder(int o)
{
  CQChartsUtil::testAndSet(bestFitData_.order, o, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setHull(bool b)
{
  CQChartsUtil::testAndSet(hullData_.visible, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsScatterPlot::
setStatsLinesSlot(bool b)
{
  if (b != isStatsLines()) {
    setStatsLines(b);

    drawObjs();
  }
}

//---

void
CQChartsScatterPlot::
setXRug(bool b)
{
  CQChartsUtil::testAndSet(axisRugData_.xVisible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setYRug(bool b)
{
  CQChartsUtil::testAndSet(axisRugData_.yVisible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setXRugSide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisRugData_.xSide, s, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setYRugSide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisRugData_.ySide, s, [&]() { drawObjs(); } );
}

//------

void
CQChartsScatterPlot::
setXDensity(bool b)
{
  CQChartsUtil::testAndSet(axisDensityData_.xVisible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setYDensity(bool b)
{
  CQChartsUtil::testAndSet(axisDensityData_.yVisible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setXDensitySide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisDensityData_.xSide, s, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setYDensitySide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisDensityData_.ySide, s, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisDensityData_.width, l, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityAlpha(double a)
{
  CQChartsUtil::testAndSet(axisDensityData_.alpha, a, [&]() { drawObjs(); } );
}

//------

void
CQChartsScatterPlot::
setDensityMap(bool b)
{
  CQChartsUtil::testAndSet(densityMapData_.visible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapGridSize(int s)
{
  CQChartsUtil::testAndSet(densityMapData_.gridSize, s, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapDelta(double d)
{
  CQChartsUtil::testAndSet(densityMapData_.delta, d, [&]() { drawObjs(); } );
}

//------

void
CQChartsScatterPlot::
setXWhisker(bool b)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.xVisible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setYWhisker(bool b)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.yVisible, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setXWhiskerSide(const YSide &s)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.xSide, s, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setYWhiskerSide(const XSide &s)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.ySide, s, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setWhiskerWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.width, l, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setWhiskerMargin(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.margin, l, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setWhiskerAlpha(double a)
{
  CQChartsUtil::testAndSet(axisWhiskerData_.alpha, a, [&]() { drawObjs(); } );
}

//------

void
CQChartsScatterPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "xColumn", "x", "X column");
  addProp("columns", "yColumn", "y", "Y column");

  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "labelColumn", "label", "Label column");

  // options
  addProp("options", "plotType", "plotType", "Plot type");
  addProp("options", "skipBad" , "skipbad" , "Skip bad values");

  // best fit line and deviation fill
  addProp("bestFit", "bestFit"         , "visible"  , "Show best fit overlay");
  addProp("bestFit", "bestFitOutliers" , "outliers" , "Best fit include outliers");
  addProp("bestFit", "bestFitOrder"    , "order"    , "Best fit curve order");
  addProp("bestFit", "bestFitDeviation", "deviation", "Best fit standard deviation");

  addFillProperties("bestFit/fill"  , "bestFitFill"  , "Best fit");
  addLineProperties("bestFit/stroke", "bestFitStroke", "Best fit");

  // stats
  addProp("statsData", "statsLines", "visible", "Statistic lines visible");

  addLineProperties("statsData/stroke", "statsLines", "Statistic lines");

  // convex hull shape
  addProp("hull", "hull", "visible", "Show convex hull overlay");

  addFillProperties("hull/fill"  , "hullFill"  , "Convex hull");
  addLineProperties("hull/stroke", "hullStroke", "Convex hull");

  // density map
  addProp("densityMap", "densityMap"        , "visible" , "Show density map overlay");
  addProp("densityMap", "densityMapGridSize", "gridSize", "Density map grid size");
  addProp("densityMap", "densityMapDelta"   , "delta"   , "Density map delta");

  // rug axis
  addProp("rug/x", "xRug"         , "visible", "Show x axis density symbols");
  addProp("rug/x", "xRugSide"     , "side"   , "X axis density symbols side");
  addProp("rug/y", "yRug"         , "visible", "Show y axis symbols density");
  addProp("rug/y", "yRugSide"     , "side"   , "Y axis density symbols side");

  addProp("rug/symbol", "rugSymbolType", "type", "Axis density symbol type");
  addProp("rug/symbol", "rugSymbolSize", "size", "Axis density symbol size");

  // density axis
  addProp("density"  , "densityWidth", "width"  , "Axis density curve width");
  addProp("density/x", "xDensity"    , "visible", "Show x axis density curve");
  addProp("density/x", "xDensitySide", "side"   , "X axis density curve side");
  addProp("density/y", "yDensity"    , "visible", "Show y axis density curve");
  addProp("density/y", "yDensitySide", "side"   , "Y axis density curve side");

  addStyleProp("density/fill", "densityAlpha", "alpha"  , "Axis density curve alpha");

  // whisker axis
  addProp("whisker"  , "whiskerWidth" , "width"  , "Axis whisker width");
  addProp("whisker"  , "whiskerMargin", "margin" , "Axis whisker margin");
  addProp("whisker/x", "xWhisker"     , "visible", "Show x axis whisker");
  addProp("whisker/x", "xWhiskerSide" , "side"   , "X axis whisker side");
  addProp("whisker/y", "yWhisker"     , "visible", "Show y axis whisker");
  addProp("whisker/y", "yWhiskerSide" , "side"   , "Y axis whisker side");

  addStyleProp("whisker/fill", "whiskerAlpha" , "alpha"  , "Axis whisker alpha");

  // symbol
  addSymbolProperties("symbol", "", "");

  // data labels
  dataLabel()->addPathProperties("labels", "Labels");

  // grid
  addProp("gridCells", "gridNumX", "nx", "Number of x grid cells");
  addProp("gridCells", "gridNumY", "ny", "Number of y grid cells");

  addStyleProp     ("gridCells/fill"  , "gridCellFilled" , "visible", "Grid cell fill visible");
  addFillProperties("gridCells/fill"  , "gridCellFill"   , "Grid cell");
  addStyleProp     ("gridCells/stroke", "gridCellStroked", "visible", "Grid cell stroke visible");
  addLineProperties("gridCells/stroke", "gridCellStroke" , "Grid cell");

  // symbol key
  addProp     ("symbol/key"     , "symbolMapKey"      , "visible", "Symbol size key visible");
  addProp     ("symbol/key"     , "symbolMapKeyMargin", "margin" , "Symbol size key margin");
  addStyleProp("symbol/key/fill", "symbolMapKeyAlpha" , "alpha"  , "Symbol size key fill alpha");

  //---

  CQChartsPointPlot::addProperties();

  //---

  CQChartsPointPlot::addPointProperties();

  // color map
  addColorMapProperties();
}

//---

CQChartsGeom::Range
CQChartsScatterPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsScatterPlot::calcRange");

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot *plot) :
     plot_(plot) {
      hasGroups_ = (plot_->numGroups() > 1);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (plot_->isInterrupt())
        return State::TERMINATE;

      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // init group
      int groupInd = plot_->rowGroupInd(ind);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));

      if (! hidden) {
        double x, y;

        bool ok1 = plot_->modelMappedReal(data.row, plot_->xColumn(), data.parent,
                                          x, plot_->isLogX(), data.row);
        bool ok2 = plot_->modelMappedReal(data.row, plot_->yColumn(), data.parent,
                                          y, plot_->isLogY(), data.row);

        if (plot_->isSkipBad() && (! ok1 || ! ok2))
          return State::SKIP;

        if (! ok1) { x = uniqueId(data, plot_->xColumn()); ++uniqueX_; }
        if (! ok2) { y = uniqueId(data, plot_->yColumn()); ++uniqueY_; }

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        range_.updateRange(x, y);
      }

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      bool ok;

      QVariant var = plot_->modelValue(data.row, column, data.parent, ok);
      if (! var.isValid()) return -1;

      CQChartsModelColumnDetails *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        CQChartsModelData *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

    const CQChartsGeom::Range &range() const { return range_; }

    bool isUniqueX() const { return uniqueX_ == numRows(); }
    bool isUniqueY() const { return uniqueY_ == numRows(); }

   private:
    const CQChartsScatterPlot* plot_      { nullptr };
    int                        hasGroups_ { false };
    CQChartsGeom::Range        range_;
    CQChartsModelDetails*      details_   { nullptr };
    int                        uniqueX_   { 0 };
    int                        uniqueY_   { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

  bool uniqueX = visitor.isUniqueX();
  bool uniqueY = visitor.isUniqueY();

  if (isInterrupt())
    return dataRange;

  //---

  if (dataRange.isSet()) {
    if (uniqueX || uniqueY) {
      if (uniqueX) {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(xColumn());

        for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i)
          xAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

        dataRange.updateRange(dataRange.xmin() - 0.5, dataRange.ymin());
        dataRange.updateRange(dataRange.xmax() + 0.5, dataRange.ymin());
      }

      if (uniqueY) {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(yColumn());

        for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i)
          yAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

        dataRange.updateRange(dataRange.xmin(), dataRange.ymin() - 0.5);
        dataRange.updateRange(dataRange.xmax(), dataRange.ymax() + 0.5);
      }
    }
  }

  //---

  dataRange = adjustDataRange(dataRange);

  //---

  // update data range if unset
  dataRange.makeNonZero();

  //---

  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  th->initGridData(dataRange);

  //---

  if (isGridCells()) {
    if (dataRange.isSet()) {
      dataRange.updateRange(gridData_.xinterval.calcStart(), gridData_.yinterval.calcStart());
      dataRange.updateRange(gridData_.xinterval.calcEnd  (), gridData_.yinterval.calcEnd  ());
    }
  }

  //---

  th->initAxes(uniqueX, uniqueY);

  //---

  return dataRange;
}

void
CQChartsScatterPlot::
initGridData(const CQChartsGeom::Range &dataRange)
{
  if (dataRange.isSet()) {
    gridData_.xinterval.setStart   (dataRange.xmin());
    gridData_.xinterval.setEnd     (dataRange.xmax());
    gridData_.xinterval.setNumMajor(gridData_.nx);

    gridData_.yinterval.setStart   (dataRange.ymin());
    gridData_.yinterval.setEnd     (dataRange.ymax());
    gridData_.yinterval.setNumMajor(gridData_.ny);
  }
  else {
    gridData_.xinterval.setStart   (0);
    gridData_.xinterval.setEnd     (1);
    gridData_.xinterval.setNumMajor(1);

    gridData_.yinterval.setStart   (0);
    gridData_.yinterval.setEnd     (1);
    gridData_.yinterval.setNumMajor(1);
  }
}

void
CQChartsScatterPlot::
resetAxes()
{
  xAxis_->setLabel("");
  yAxis_->setLabel("");
}

void
CQChartsScatterPlot::
initAxes(bool uniqueX, bool uniqueY)
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  if (xAxis_->label() == "") {
    QString xname = xColumnName("");

    xAxis_->setLabel(xname);
  }

  if (yAxis_->label() == "") {
    QString yname = yColumnName("");

    yAxis_->setLabel(yname);
  }

  CQChartsAxisValueType xType = (isLogX() ?
    CQChartsAxisValueType::Type::LOG : CQChartsAxisValueType::Type::REAL);
  CQChartsAxisValueType yType = (isLogY() ?
    CQChartsAxisValueType::Type::LOG : CQChartsAxisValueType::Type::REAL);

  if (uniqueX) xType = CQChartsAxisValueType::Type::INTEGER;
  if (uniqueY) yType = CQChartsAxisValueType::Type::INTEGER;

  xAxis_->setValueType(xType);
  yAxis_->setValueType(yType);

  //---

  ColumnType xColumnType = columnValueType(xColumn());

  if (xColumnType == CQBaseModelType::TIME)
    xAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);
}

QString
CQChartsScatterPlot::
xColumnName(const QString &def) const
{
  if (xLabel().length())
    return xLabel();

  bool ok;

  QString xname = modelHeaderString(xColumn(), ok);

  if (! ok || ! xname.length())
    xname = def;

  return xname;
}

QString
CQChartsScatterPlot::
yColumnName(const QString &def) const
{
  if (yLabel().length())
    return yLabel();

  bool ok;

  QString yname = modelHeaderString(yColumn(), ok);

  if (! ok || ! yname.length())
    yname = def;

  return yname;
}

//------

void
CQChartsScatterPlot::
clearPlotObjects()
{
  groupNameValues_  .clear();
  groupNameGridData_.clear();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsScatterPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsScatterPlot::createObjs");

  NoUpdate noUpdate(this);

  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  //---

  // init value set
  //initValueSets();

  //---

  // init name values
  th->gridData_.maxN = 0;

  if (groupNameValues_.empty())
    addNameValues();

  for (const auto &ghull : th->groupHull_)
    delete ghull.second;

  th->groupPoints_  .clear();
  th->groupFitData_ .clear();
  th->groupStatData_.clear();
  th->groupHull_    .clear();
  th->groupWhiskers_.clear();

  //---

  th->updateColumnNames();

  //---

  addPointObjects(objs);

  addGridObjects(objs);

  //---

  return true;
}

void
CQChartsScatterPlot::
updateColumnNames()
{
  // set column header names
  CQChartsPlot::updateColumnNames();

  columnNames_[xColumn()] = xColumnName();
  columnNames_[yColumn()] = yColumnName();

  setColumnHeaderName(nameColumn      (), "Name"      );
  setColumnHeaderName(labelColumn     (), "Label"     );
  setColumnHeaderName(symbolTypeColumn(), "SymbolType");
  setColumnHeaderName(symbolSizeColumn(), "SymbolSize");
  setColumnHeaderName(fontSizeColumn  (), "FontSize"  );
}

void
CQChartsScatterPlot::
addPointObjects(PlotObjs &objs) const
{
  initSymbolTypeData();
  initSymbolSizeData();
  initFontSizeData  ();

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto pg = groupPoints_.find(groupInd);

    if (pg == groupPoints_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto pg1 = th->groupPoints_.find(groupInd);

      if (pg1 == th->groupPoints_.end())
        pg1 = th->groupPoints_.insert(pg1, GroupPoints::value_type(groupInd, Points()));

      pg = groupPoints_.find(groupInd);
    }

    Points &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const QString &name   = nameValue.first;
      const Values  &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          return;

        //---

        // get point position
        const ValueData &valuePoint = values[iv];

        const QPointF &p = valuePoint.p;

        //---

        // get symbol size (needed for bounding box)
        CQChartsLength symbolSize(CQChartsUnits::NONE, 0.0);

        if (symbolSizeColumn().isValid()) {
          if (! columnSymbolSize(valuePoint.row, valuePoint.ind.parent(), symbolSize))
            symbolSize = CQChartsLength(CQChartsUnits::NONE, 0.0);
        }

        double sx, sy;

        plotSymbolSize(symbolSize.isValid() ? symbolSize : this->symbolSize(), sx, sy);

        //---

        // create point object
        ColorInd is1(is, ns);
        ColorInd ig1(ig, ng);
        ColorInd iv1(iv, nv);

        CQChartsGeom::BBox bbox(p.x() - sx, p.y() - sy, p.x() + sx, p.y() + sy);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, groupInd, bbox, p, is1, ig1, iv1);

        pointObj->setModelInd(valuePoint.ind);

        if (symbolSize.isValid())
          pointObj->setSymbolSize(symbolSize);

        objs.push_back(pointObj);

        points.push_back(p);

        //---

        // set optional symbol type
        CQChartsSymbol symbolType(CQChartsSymbol::Type::NONE);

        if (symbolTypeColumn().isValid()) {
          if (! columnSymbolType(valuePoint.row, valuePoint.ind.parent(), symbolType))
             symbolType = CQChartsSymbol(CQChartsSymbol::Type::NONE);
        }

        if (symbolType.isValid())
          pointObj->setSymbolType(symbolType);

        //---

        // set optional font size
        CQChartsLength fontSize(CQChartsUnits::NONE, 0.0);

        if (fontSizeColumn().isValid()) {
          if (! columnFontSize(valuePoint.row, valuePoint.ind.parent(), fontSize))
            fontSize = CQChartsLength(CQChartsUnits::NONE, 0.0);
        }

        if (fontSize.isValid())
          pointObj->setFontSize(fontSize);

        //---

        // set optional symbol fill color
        CQChartsColor symbolColor(CQChartsColor::Type::NONE);

        if (colorColumn().isValid()) {
          if (! columnColor(valuePoint.row, valuePoint.ind.parent(), symbolColor))
             symbolColor = CQChartsColor(CQChartsColor::Type::NONE);
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional point label
        QString pointName;

        if (labelColumn().isValid() || nameColumn().isValid()) {
          bool ok;

          if (labelColumn().isValid())
            pointName = modelString(valuePoint.row, labelColumn(), valuePoint.ind.parent(), ok);
          else
            pointName = modelString(valuePoint.row, nameColumn(), valuePoint.ind.parent(), ok);

          if (! ok)
            pointName = "";
        }

        if (pointName.length())
          pointObj->setName(pointName);

        //---

        // set optional image
        QImage image;

        if (imageColumn().isValid()) {
          bool ok;

          QVariant imageVar =
            modelValue(valuePoint.row, imageColumn(), valuePoint.ind.parent(), ok);

          if (ok && imageVar.type() == QVariant::Image)
            image = imageVar.value<QImage>();
        }

        if (! image.isNull())
          pointObj->setImage(image);
      }

      ++is;
    }

    ++ig;
  }
}

void
CQChartsScatterPlot::
addGridObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  int ig = 0;
  int ng = groupNameGridData_.size();

  for (const auto &pg : groupNameGridData_) {
    if (isInterrupt())
      return;

    int                 groupInd     = pg.first;
    const NameGridData &nameGridData = pg.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    int is = 0;
    int ns = nameGridData.size();

    for (const auto &pn : nameGridData) {
      if (isInterrupt())
        return;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const QString       &name          = pn.first;
      const CellPointData &cellPointData = pn.second;

    //int nx = cellPointData.size();

      int maxN = cellPointData.maxN;

      for (const auto &px : cellPointData.xyPoints) {
        if (isInterrupt())
          return;

        int            ix      = px.first;
        const YPoints &yPoints = px.second;

        double xmin, xmax;

        gridData_.xinterval.intervalValues(ix, xmin, xmax);

      //int ny = yPoints.size();

        for (const auto &py : yPoints) {
          if (isInterrupt())
            return;

          int           iy     = py.first;
          const Points &points = py.second;

          double ymin, ymax;

          gridData_.yinterval.intervalValues(iy, ymin, ymax);

          //---

          ColorInd is1(is, ns);
          ColorInd ig1(ig, ng);

          CQChartsGeom::BBox bbox(xmin, ymin, xmax, ymax);

          CQChartsScatterCellObj *cellObj =
            new CQChartsScatterCellObj(this, groupInd, bbox, is1, ig1, ix, iy, points, maxN);

          objs.push_back(cellObj);
        }
      }

      ++is;
    }

    ++ig;
  }
}

void
CQChartsScatterPlot::
addNameValues() const
{
  CQPerfTrace trace("CQChartsScatterPlot::addNameValues");

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      CQChartsModelIndex ind(data.row, plot_->xColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(ind);

      //---

      // get x, y value
      QModelIndex xInd  = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      double x, y;

      bool ok1 = plot_->modelMappedReal(data.row, plot_->xColumn(), data.parent,
                                        x, plot_->isLogX(), data.row);
      bool ok2 = plot_->modelMappedReal(data.row, plot_->yColumn(), data.parent,
                                        y, plot_->isLogY(), data.row);

      if (plot_->isSkipBad() && (! ok1 || ! ok2))
        return State::SKIP;

      if (! ok1) x = uniqueId(data, plot_->xColumn());
      if (! ok2) y = uniqueId(data, plot_->yColumn());

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional grouping name (name column, title, x axis)
      QString name;

      if (plot_->nameColumn().isValid()) {
        bool ok;

        name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);
      }

      if (! name.length() && plot_->title())
        name = plot_->title()->textStr();

      if (! name.length() && plot_->xAxis())
        name = plot_->xAxis()->label();

      //---

      // get symbol type, size, font size and color
      CQChartsColor color;

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        (void) plot_->columnColor(data.row, data.parent, color);
      }

      //---

      CQChartsScatterPlot *plot = const_cast<CQChartsScatterPlot *>(plot_);

      plot->addNameValue(groupInd, name, x, y, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      bool ok;

      QVariant var = plot_->modelValue(data.row, column, data.parent, ok);
      if (! var.isValid()) return -1;

      CQChartsModelColumnDetails *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        CQChartsModelData *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

   private:
    const CQChartsScatterPlot* plot_    { nullptr };
    CQChartsModelDetails*      details_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsScatterPlot::
addNameValue(int groupInd, const QString &name, double x, double y, int row,
             const QModelIndex &xind, const CQChartsColor &color)
{
  if (isGridCells()) {
    int ix = gridData_.xinterval.valueInterval(x);
    int iy = gridData_.yinterval.valueInterval(y);

    CellPointData &cellPointData = groupNameGridData_[groupInd][name];

    Points &points = cellPointData.xyPoints[ix][iy];

    points.push_back(QPointF(x, y));

    cellPointData.maxN = std::max(cellPointData.maxN, int(points.size()));

    gridData_.maxN = std::max(gridData_.maxN, cellPointData.maxN);
  }
  else {
    ValuesData &valuesData = groupNameValues_[groupInd][name];

    valuesData.xrange.add(x);
    valuesData.yrange.add(y);

    valuesData.values.emplace_back(QPointF(x, y), row, xind, color);
  }
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  if (isOverlay() && ! isFirstPlot())
    return;

  if (! isGridCells())
    addPointKeyItems(key);
  else
    addGridKeyItems(key);

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot::
addPointKeyItems(CQChartsPlotKey *key)
{
  int ng = groupNameValues_.size();

  // multiple group - key item per group
  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupNameValue : groupNameValues_) {
      int groupInd = groupNameValue.first;

      QString groupName = groupIndName(groupInd);

      ColorInd ic(ig, ng);

      CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, groupInd , ic);
      CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, groupName, ic);

      key->addItem(colorItem, ig, 0);
      key->addItem(textItem , ig, 1);

      //--

      if (colorColumn().isValid() && colorColumn().isGroup()) {
        if (isColorMapped()) {
          double r = CMathUtil::map(groupInd, colorColumnData_.data_min, colorColumnData_.data_max,
                                    colorMapMin(), colorMapMax());

          CQChartsColor color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r);

          if (color.isValid())
            colorItem->setColor(color);
        }
      }

      //--

      ++ig;
    }
  }
  // single group - key item per value set
  else if (ng > 0) {
    const NameValues &nameValues = (*groupNameValues_.begin()).second;

    int ns = nameValues.size();

    if (ns > 1) {
      int is = 0;

      for (const auto &nameValue : nameValues) {
        const QString &name   = nameValue.first;
        const Values  &values = nameValue.second.values;

        ColorInd ic(is, ns);

        CQChartsScatterKeyColor *colorItem = new CQChartsScatterKeyColor(this, -1  , ic);
        CQChartsKeyText         *textItem  = new CQChartsKeyText        (this, name, ic);

        key->addItem(colorItem, is, 0);
        key->addItem(textItem , is, 1);

        //--

        if (colorColumn().isValid()) {
          int nv = values.size();

          if (nv > 0) {
            const ValueData &valuePoint = values[0];

            CQChartsColor color;

            if (columnColor(valuePoint.row, valuePoint.ind.parent(), color))
              colorItem->setColor(color);
          }
        }

        //--

        ++is;
      }
    }
  }
}

void
CQChartsScatterPlot::
addGridKeyItems(CQChartsPlotKey *key)
{
  CQChartsScatterGridKeyItem *item = new CQChartsScatterGridKeyItem(this);

  key->addItem(item, 0, 0);
}

//---

bool
CQChartsScatterPlot::
probe(ProbeData &probeData) const
{
  CQChartsPlotObj *obj;

  if (! objNearestPoint(probeData.p, obj))
    return false;

  CQChartsGeom::Point c = obj->rect().getCenter();

  probeData.p    = c;
  probeData.both = true;

  probeData.xvals.push_back(c.x);
  probeData.yvals.push_back(c.y);

  return true;
}

//---

bool
CQChartsScatterPlot::
addMenuItems(QMenu *menu)
{
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name,
                                  bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  menu->addSeparator();

  QMenu *typeMenu = new QMenu("Plot Type");

  (void) addMenuCheckedAction(typeMenu, "Symbols"   , isSymbols  (), SLOT(setSymbols(bool)));
  (void) addMenuCheckedAction(typeMenu, "Grid Cells", isGridCells(), SLOT(setGridCells(bool)));

  menu->addMenu(typeMenu);

  //---

  QMenu *overlaysMenu = new QMenu("Overlays");

  (void) addMenuCheckedAction(overlaysMenu, "Best Fit"   ,
                              isBestFit   (), SLOT(setBestFit       (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Hull"       ,
                              isHull      (), SLOT(setHull          (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Statistic Lines",
                              isStatsLines(), SLOT(setStatsLinesSlot(bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Density Map",
                              isDensityMap(), SLOT(setDensityMap    (bool)));

  menu->addMenu(overlaysMenu);

  //---

  QMenu *xMenu = new QMenu("X Axis Annotation");
  QMenu *yMenu = new QMenu("Y Axis Annotation");

  (void) addMenuCheckedAction(xMenu, "Rug"    , isXRug    (), SLOT(setXRug    (bool)));
  (void) addMenuCheckedAction(xMenu, "Density", isXDensity(), SLOT(setXDensity(bool)));
  (void) addMenuCheckedAction(xMenu, "Whisker", isXWhisker(), SLOT(setXWhisker(bool)));

  (void) addMenuCheckedAction(yMenu, "Rug"    , isYRug    (), SLOT(setYRug    (bool)));
  (void) addMenuCheckedAction(yMenu, "Density", isYDensity(), SLOT(setYDensity(bool)));
  (void) addMenuCheckedAction(yMenu, "Whisker", isYWhisker(), SLOT(setYWhisker(bool)));

  menu->addMenu(xMenu);
  menu->addMenu(yMenu);

  return true;
}

//------

CQChartsGeom::BBox
CQChartsScatterPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  if (isXRug() || isYRug() || isXDensity() || isYDensity() || isXWhisker() || isYWhisker()) {
    const CQChartsGeom::Range &dataRange = this->dataRange();

    //---

    // rug axis
    double sx, sy;

    plotSymbolSize(rugSymbolSize(), sx, sy);

    if (isXRug()) {
      double y = (xRugSide() == YSide::BOTTOM ? dataRange.ymin() - 2*sy :
                                                dataRange.ymax() + 2*sy);

      QPointF p(dataRange.xmax(), y);

      bbox += CQChartsGeom::Point(p);
    }

    if (isYRug()) {
      double x = (yRugSide() == XSide::LEFT ? dataRange.xmin() - 2*sx :
                                              dataRange.xmax() + 2*sx);

      QPointF p(x, dataRange.ymax());

      bbox += CQChartsGeom::Point(p);
    }

    //---

    // density axis
    if (isXDensity()) {
      double dw = lengthPlotHeight(densityWidth());

      double pos = (xDensitySide() == YSide::BOTTOM ?
        dataRange.ymin() - dw : dataRange.ymax() + dw);

      QPointF p1(dataRange.xmax(), pos);

      bbox += CQChartsGeom::Point(p1);
    }

    if (isYDensity()) {
      double dw = lengthPlotWidth(densityWidth());

      double pos = (yDensitySide() == XSide::LEFT ?
        dataRange.xmin() - dw : dataRange.xmax() + dw);

      QPointF p2(pos, dataRange.ymin());

      bbox += CQChartsGeom::Point(p2);
    }

    //---

    // whisker axis
    if (isXWhisker()) {
      int ng = groupNameValues_.size();

      double wm = lengthPlotHeight(whiskerMargin());
      double ww = ng*lengthPlotHeight(whiskerWidth());

      double pos = (xWhiskerSide() == YSide::BOTTOM ?
        dataRange.ymin() - ww - 2*wm : dataRange.ymax() + ww + 2*wm);

      QPointF p1(dataRange.xmax(), pos);

      bbox += CQChartsGeom::Point(p1);
    }

    if (isYWhisker()) {
      int ng = groupNameValues_.size();

      double wm = lengthPlotWidth(whiskerMargin());
      double ww = ng*lengthPlotWidth(whiskerWidth());

      double pos = (yWhiskerSide() == XSide::LEFT ?
        dataRange.xmin() - ww - 2*wm : dataRange.xmax() + ww + 2*wm);

      QPointF p2(pos, dataRange.ymin());

      bbox += CQChartsGeom::Point(p2);
    }
  }

  return bbox;
}

//------

bool
CQChartsScatterPlot::
hasBackground() const
{
  if (isHull      ()) return true;
  if (isBestFit   ()) return true;
  if (isStatsLines()) return true;
  if (isDensityMap()) return true;

  if (isXRug    ()) return true;
  if (isXDensity()) return true;
  if (isXWhisker()) return true;

  if (isYRug    ()) return true;
  if (isYDensity()) return true;
  if (isYWhisker()) return true;

  return false;
}

void
CQChartsScatterPlot::
execDrawBackground(CQChartsPaintDevice *device) const
{
  CQChartsPlot::execDrawBackground(device);

  if (isHull      ()) drawHull      (device);
  if (isBestFit   ()) drawBestFit   (device);
  if (isStatsLines()) drawStatsLines(device);
  if (isDensityMap()) drawDensityMap(device);

  if (isXRug    ()) drawXRug    (device);
  if (isXDensity()) drawXDensity(device);
  if (isXWhisker()) drawXWhisker(device);

  if (isYRug    ()) drawYRug    (device);
  if (isYDensity()) drawYDensity(device);
  if (isYWhisker()) drawYWhisker(device);
}

bool
CQChartsScatterPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsScatterPlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  if (isSymbolMapKey())
    drawSymbolMapKey(device);
}

void
CQChartsScatterPlot::
initGroupBestFit(int groupInd) const
{
  // init best fit data
  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  CQChartsFitData &fitData = th->groupFitData_[groupInd];

  if (! fitData.isFitted()) {
    auto p = groupPoints_.find(groupInd);

    if (p != groupPoints_.end()) {
      const Points &points = (*p).second;

      if (! isBestFitOutliers()) {
        initGroupStats(groupInd);

        //---

        auto ps = groupStatData_.find(groupInd);
        assert(ps != groupStatData_.end());

        const StatData &statData = (*ps).second;

        //---

        QPolygonF poly;

        for (const auto &p : points) {
          if (! statData.xstat.isOutlier(p.x()) && ! statData.ystat.isOutlier(p.y()))
            poly.push_back(p);
        }

        //---

        fitData.calc(poly, bestFitOrder());
      }
      else {
        fitData.calc(points, bestFitOrder());
      }
    }
  }
}

void
CQChartsScatterPlot::
initGroupStats(int groupInd) const
{
  // init stats data
  CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

  StatData &statData = th->groupStatData_[groupInd];

  if (! statData.xstat.set || ! statData.ystat.set) {
    auto p = groupPoints_.find(groupInd);

    if (p != groupPoints_.end()) {
      const Points &points = (*p).second;

      std::vector<double> x, y;

      for (std::size_t i = 0; i < points.size(); ++i) {
        x.push_back(points[i].x());
        y.push_back(points[i].y());
      }

      std::sort(x.begin(), x.end());
      std::sort(y.begin(), y.end());

      statData.xstat.calcStatValues(x);
      statData.ystat.calcStatValues(y);
    }
  }
}

void
CQChartsScatterPlot::
drawBestFit(CQChartsPaintDevice *device) const
{
  // init fit data
  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    initGroupBestFit(groupInd);
  }

  //---

  // draw fit data
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    auto pf = groupFitData_.find(groupInd);
    assert(pf != groupFitData_.end());

    const CQChartsFitData &fitData = (*pf).second;

    //---

    // calc fit shape at each pixel
    QPolygonF bpoly, poly, tpoly;

    CQChartsGeom::Point pl = CQChartsGeom::Point(fitData.xmin(), 0);
    CQChartsGeom::Point pr = CQChartsGeom::Point(fitData.xmax(), 0);

    for (int px = pl.x; px <= pr.x; ++px) {
      if (isInterrupt())
        return;

      CQChartsGeom::Point p1 = CQChartsGeom::Point(px, 0.0);

      double y2 = fitData.interp(p1.x);

      CQChartsGeom::Point p2 = CQChartsGeom::Point(p1.x, y2);

      poly << QPointF(p2.x, p2.y);

      // deviation curve above/below
      if (isBestFitDeviation()) {
        p2 = CQChartsGeom::Point(p1.x, y2 - fitData.deviation());

        bpoly << QPointF(p2.x, p2.y);

        p2 = CQChartsGeom::Point(p1.x, y2 + fitData.deviation());

        tpoly << QPointF(p2.x, p2.y);
      }
    }

    //---

    if (poly.size()) {
      // set pen and brush
      ColorInd ic(ig, ng);

      QPen   pen;
      QBrush brush;

      QColor strokeColor = interpBestFitStrokeColor(ic);
      QColor fillColor   = interpBestFitFillColor  (ic);

      setPen(pen, isBestFitStroked(), strokeColor, bestFitStrokeAlpha(),
             bestFitStrokeWidth(), bestFitStrokeDash());

      setBrush(brush, isBestFitFilled(), fillColor, bestFitFillAlpha(), bestFitFillPattern());

      updateObjPenBrushState(this, ic, pen, brush, CQChartsPlot::DrawType::LINE);

      device->setPen  (pen);
      device->setBrush(brush);

      //---

      // draw fit deviation shape
      if (isBestFitDeviation()) {
        QPolygonF dpoly;

        for (int i = 0; i < bpoly.size(); ++i) {
          if (isInterrupt())
            return;

          const QPointF &p = bpoly[i];

          dpoly << p;
        }

        for (int i = tpoly.size() - 1; i >= 0; --i) {
          if (isInterrupt())
            return;

          const QPointF &p = tpoly[i];

          dpoly << p;
        }

        device->drawPolygon(dpoly);
      }

      //---

      // draw fit line
      QPainterPath path;

      const QPointF &p = poly[0];

      path.moveTo(p);

      for (int i = 1; i < poly.size(); ++i) {
        if (isInterrupt())
          return;

        const QPointF &p = poly[i];

        path.lineTo(p);
      }

      device->strokePath(path, pen);
    }

    //---

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawStatsLines(CQChartsPaintDevice *device) const
{
  // init stats data
  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    initGroupStats(groupInd);
  }

  //---

  // draw stats data
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    auto ps = groupStatData_.find(groupInd);
    assert(ps != groupStatData_.end());

    const StatData &statData = (*ps).second;

    //---

    // calc pen and brush
    ColorInd ic(ig, ng);

    QPen   pen;
    QBrush brush;

    QColor c = interpStatsLinesColor(ic);

    setPen(pen, true, c, statsLinesAlpha(), statsLinesWidth(), statsLinesDash());

    setBrush(brush, false);

    updateObjPenBrushState(this, ic, pen, brush, CQChartsPlot::DrawType::LINE);

    device->setPen  (pen);
    device->setBrush(brush);

    //---

    auto drawXStatLine = [&](double x) {
      QPointF p1 = QPointF(x, statData.ystat.loutlier);
      QPointF p2 = QPointF(x, statData.ystat.uoutlier);

      device->drawLine(p1, p2);
    };

    auto drawYStatLine = [&](double y) {
      QPointF p1 = QPointF(statData.xstat.loutlier, y);
      QPointF p2 = QPointF(statData.xstat.uoutlier, y);

      device->drawLine(p1, p2);
    };

    drawXStatLine(statData.xstat.loutlier   );
    drawXStatLine(statData.xstat.lowerMedian);
    drawXStatLine(statData.xstat.median     );
    drawXStatLine(statData.xstat.upperMedian);
    drawXStatLine(statData.xstat.uoutlier   );

    drawYStatLine(statData.ystat.loutlier   );
    drawYStatLine(statData.ystat.lowerMedian);
    drawYStatLine(statData.ystat.median     );
    drawYStatLine(statData.ystat.upperMedian);
    drawYStatLine(statData.ystat.uoutlier   );

    //---

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawHull(CQChartsPaintDevice *device) const
{
  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    int groupInd = groupNameValue.first;

    auto ph = groupHull_.find(groupInd);

    if (ph == groupHull_.end()) {
      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto ph1 = th->groupHull_.insert(th->groupHull_.end(),
                   GroupHull::value_type(groupInd, new CQChartsGrahamHull));

      CQChartsGrahamHull *hull = (*ph1).second;

      const Points &points = th->groupPoints_[groupInd];

      //---

      std::vector<double> x, y;

      for (const auto &p : points) {
        if (isInterrupt())
          return;

        hull->addPoint(p);
      }

      hull->calc();

      //---

      ph = groupHull_.find(groupInd);
    }

    const CQChartsGrahamHull *hull = (*ph).second;

    //---

    ColorInd colorInd(ig, ng);

    QColor strokeColor = interpHullStrokeColor(colorInd);
    QColor fillColor   = interpHullFillColor  (colorInd);

    QPen   pen;
    QBrush brush;

    setPenBrush(pen, brush,
      isHullStroked(), strokeColor, hullStrokeAlpha(), hullStrokeWidth(), hullStrokeDash(),
      isHullFilled(), fillColor, hullFillAlpha(), hullFillPattern());

    device->setPen  (pen);
    device->setBrush(brush);

    //---

    hull->draw(this, device);

    //---

    ++ig;
  }
}

//------

void
CQChartsScatterPlot::
drawXRug(CQChartsPaintDevice *device) const
{
  for (const auto &plotObj : plotObjects()) {
    if (isInterrupt())
      return;

    const CQChartsScatterPointObj *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);
    const CQChartsScatterCellObj  *cellObj  = dynamic_cast<CQChartsScatterCellObj  *>(plotObj);

    if (pointObj)
      pointObj->drawDir(device, CQChartsScatterPointObj::Dir::X, xRugSide() == YSide::TOP);

    if (cellObj)
      cellObj->drawRugSymbol(device, CQChartsScatterCellObj::Dir::X, xRugSide() == YSide::TOP);
  }
}

void
CQChartsScatterPlot::
drawYRug(CQChartsPaintDevice *device) const
{
  for (const auto &plotObj : plotObjects()) {
    if (isInterrupt())
      return;

    const CQChartsScatterPointObj *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);
    const CQChartsScatterCellObj  *cellObj  = dynamic_cast<CQChartsScatterCellObj  *>(plotObj);

    if (pointObj)
      pointObj->drawDir(device, CQChartsScatterPointObj::Dir::Y, yRugSide() == XSide::RIGHT);

    if (cellObj)
      cellObj->drawRugSymbol(device, CQChartsScatterCellObj::Dir::Y, yRugSide() == XSide::RIGHT);
  }
}

//------

void
CQChartsScatterPlot::
drawXDensity(CQChartsPaintDevice *device) const
{
  initWhiskerData();

  //---

  if (! isGridCells()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXDensityWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXDensityWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawYDensity(CQChartsPaintDevice *device) const
{
  initWhiskerData();

  //---

  if (! isGridCells()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYDensityWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYDensityWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXDensityWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, densityAlpha(), CQChartsFillPattern());

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  const CQChartsDensity &density = whiskerData.xWhisker.density();

  double xmin = density.xmin1();
  double xmax = density.xmax1();

  double dh = lengthPlotHeight(densityWidth());

  double pos = (xDensitySide() == YSide::BOTTOM ?  dataRange.ymin() - dh : dataRange.ymax());

  CQChartsGeom::BBox rect(xmin, pos, xmax, pos + dh);

  density.drawWhisker(this, device, rect, Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYDensityWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, densityAlpha(), symbolFillPattern());

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  const CQChartsDensity &density = whiskerData.yWhisker.density();

  double xmin = density.xmin1();
  double xmax = density.xmax1();

  double dw = lengthPlotWidth(densityWidth());

  double pos = (yDensitySide() == XSide::LEFT ?  dataRange.xmin() - dw : dataRange.xmax());

  CQChartsGeom::BBox rect(pos, xmin, pos + dw, xmax);

  density.drawWhisker(this, device, rect, Qt::Vertical);
}

void
CQChartsScatterPlot::
drawDensityMap(CQChartsPaintDevice *device) const
{
  device->save();

  setClipRect(device);

  //---

  initWhiskerData();

  //---

  int gridSize = std::max(densityMapGridSize(), 1);

  double delta = densityMapDelta();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    const NameValues &nameValues = groupNameValue.second;

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

      const ValuesData &values = nameValue.second;

      double xmin = values.xrange.min();
      double xmax = values.xrange.max();
      double ymin = values.yrange.min();
      double ymax = values.yrange.max();

      QRectF pr = device->windowToPixel(QRectF(xmin, ymin, xmax - xmin, ymax - ymin));

      QPointF ll = pr.bottomLeft();
      QPointF ur = pr.topRight  ();

      int x1 = CMathRound::RoundDown(ll.x());
      int x2 = CMathRound::RoundUp  (ur.x());
      int y2 = CMathRound::RoundUp  (ll.y());
      int y1 = CMathRound::RoundDown(ur.y());

      int dx = gridSize;
      int dy = gridSize;

      std::vector<double> xv;
      std::vector<double> yv;

      for (const auto &v : values.values) {
        if (isInterrupt())
          return;

        double x1 = (xmax > xmin ? (v.p.x() - xmin)/(xmax - xmin) : 0.0);
        double y1 = (ymax > ymin ? (v.p.y() - ymin)/(ymax - ymin) : 0.0);

        xv.push_back(x1);
        yv.push_back(y1);
      }

      CMathBivariate bivariate(xv, yv);

      for (int y = y1; y <= y2; y += dy) {
        if (isInterrupt())
          return;

        for (int x = x1; x <= x2; x += dx) {
          if (isInterrupt())
            return;

          QPointF p = pixelToWindow(QPointF(x, y));

          double x1 = (xmax > xmin ? (p.x() - xmin)/(xmax - xmin) : 0.0);
          double y1 = (ymax > ymin ? (p.y() - ymin)/(ymax - ymin) : 0.0);

          double a = 1.0;
          double v = bivariate.calc(x1, y1);

          if (delta > 0.0) {
            double v1 = CMathRound::RoundDown(v/delta)*delta;

            a = CMathUtil::clamp(sqrt(1.0 - (v - v1)), 0.0, 1.0);
          }

          //---

          QBrush brush;

          QColor c = interpPaletteColor(ColorInd(v));

          setBrush(brush, true, c, a, CQChartsFillPattern());

          //---

          QRectF pr1 = QRectF(x, y, dx, dy);

          device->fillRect(device->pixelToWindow(pr1), brush);
        }
      }
    }
  }

  //---

  device->restore();
}

//---

void
CQChartsScatterPlot::
drawXWhisker(CQChartsPaintDevice *device) const
{
  initWhiskerData();

  //---

  if (! isGridCells()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXWhiskerWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawXWhiskerWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawYWhisker(CQChartsPaintDevice *device) const
{
  initWhiskerData();

  //---

  if (! isGridCells()) {
    int ig = 0;
    int ng = groupNameValues_.size();

    for (const auto &groupNameValue : groupNameValues_) {
      if (isInterrupt())
        return;

      int groupInd = groupNameValue.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYWhiskerWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupNameGridData_.size();

    for (const auto &pg : groupNameGridData_) {
      if (isInterrupt())
        return;

      int groupInd = pg.first;

      auto p = groupWhiskers_.find(groupInd);

      if (p != groupWhiskers_.end()) {
        const WhiskerData &whiskerData = (*p).second;

        drawYWhiskerWhisker(device, whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXWhiskerWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, whiskerAlpha(), symbolFillPattern());

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  double ww = lengthPlotHeight(whiskerWidth ());
  double wm = lengthPlotHeight(whiskerMargin());

  const CQChartsGeom::Range &dataRange = this->dataRange();

  double pos = (xWhiskerSide() == YSide::BOTTOM ?
    dataRange.ymin() - (ig.i + 1)*ww - wm : dataRange.ymax() + ig.i*ww + wm);

  CQChartsGeom::BBox rect(whiskerData.xWhisker.min(), pos, whiskerData.xWhisker.max(), pos + ww);

  CQChartsBoxWhiskerUtil::drawWhisker(this, device, whiskerData.xWhisker,
                                      rect, whiskerWidth(), Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYWhiskerWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  QPen   pen;
  QBrush brush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(pen, brush,
    /*stroked*/ true, strokeColor, symbolStrokeAlpha(), CQChartsLength(), CQChartsLineDash(),
    /*filled*/ true, fillColor, whiskerAlpha(), symbolFillPattern());

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  double ww = lengthPlotWidth(whiskerWidth());
  double wm = lengthPlotWidth(whiskerMargin());

  const CQChartsGeom::Range &dataRange = this->dataRange();

  double pos = (yWhiskerSide() == XSide::LEFT ?
    dataRange.xmin() - ig.i*ww - wm : dataRange.xmax() + (ig.i + 1)*ww + wm);

  CQChartsGeom::BBox rect(pos, whiskerData.yWhisker.min(), pos + ww, whiskerData.yWhisker.max());

  CQChartsBoxWhiskerUtil::drawWhisker(this, device, whiskerData.yWhisker,
                                      rect, whiskerWidth(), Qt::Vertical);
}

void
CQChartsScatterPlot::
initWhiskerData() const
{
  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    //---

    // get group whiskers
    int groupInd = groupNameValue.first;

    auto pw = groupWhiskers_.find(groupInd);

    if (pw == groupWhiskers_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto pw1 = th->groupWhiskers_.find(groupInd);

      if (pw1 == th->groupWhiskers_.end()) {
        (void) th->groupWhiskers_[groupInd];
        //pw1 = th->groupWhiskers_.insert(pw1, GroupWhiskers::value_type(groupInd, WhiskerData()));
      }

      pw = groupWhiskers_.find(groupInd);
    }

    const WhiskerData &whiskerData = (*pw).second;

    //---

    // init whisker if needed
    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

          whiskerData1.xWhisker.addValue(pointObj->point().x());
        }
      }
    }

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

          whiskerData1.yWhisker.addValue(pointObj->point().y());
        }
      }
    }
  }

  //---

  for (const auto &pg : groupNameGridData_) {
    if (isInterrupt())
      return;

    //---

    // get group whiskers
    int groupInd = pg.first;

    auto pw = groupWhiskers_.find(groupInd);

    if (pw == groupWhiskers_.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CQChartsScatterPlot *th = const_cast<CQChartsScatterPlot *>(this);

      auto pw1 = th->groupWhiskers_.find(groupInd);

      if (pw1 == th->groupWhiskers_.end()) {
        (void) th->groupWhiskers_[groupInd];
        //pw1 = th->groupWhiskers_.insert(pw1, GroupWhiskers::value_type(groupInd, WhiskerData()));
      }

      pw = groupWhiskers_.find(groupInd);
    }

    const WhiskerData &whiskerData = (*pw).second;

    //---

    // init whisker if needed
    if (! whiskerData.xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

            whiskerData1.xWhisker.addValue(p.x());
          }
        }
      }
    }

    if (! whiskerData.yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            WhiskerData &whiskerData1 = const_cast<WhiskerData &>(whiskerData);

            whiskerData1.yWhisker.addValue(p.y());
          }
        }
      }
    }
  }
}

//------

void
CQChartsScatterPlot::
drawSymbolMapKey(CQChartsPaintDevice *device) const
{
  if (! symbolSizeColumn().isValid())
    return;

  //---

  // draw size key
  double min  = symbolSizeData_.data_min;
  double mean = symbolSizeData_.data_mean;
  double max  = symbolSizeData_.data_max;

  CQChartsGeom::BBox pbbox = calcPlotPixelRect();

  //double px, py;

  //double vx = view()->viewportRange();
  //double vy = 0.0;

  //CQChartsGeom::Point p = view()->windowToPixel(CQChartsGeom::Point(vx, vy));

  double px = pbbox.getXMax();
  double py = pbbox.getYMax();

  double pm = symbolMapKeyMargin();

  double pr1 = symbolSizeData_.map_max;
  double pr3 = symbolSizeData_.map_min;

  double pr2 = (pr1 + pr3)/2;

  QColor strokeColor = interpThemeColor(ColorInd(1.0));

  device->setPen(strokeColor);

  double xm = px - pr1 - pm;
  double ym = py - pm;

  QRectF r1(xm - pr1, ym - 2*pr1, 2*pr1, 2*pr1);
  QRectF r2(xm - pr2, ym - 2*pr2, 2*pr2, 2*pr2);
  QRectF r3(xm - pr3, ym - 2*pr3, 2*pr3, 2*pr3);

  double a = symbolMapKeyAlpha();

  QColor fillColor1 = interpSymbolFillColor(ColorInd(1.0)); fillColor1.setAlphaF(a);
  QColor fillColor2 = interpSymbolFillColor(ColorInd(0.5)); fillColor2.setAlphaF(a);
  QColor fillColor3 = interpSymbolFillColor(ColorInd(0.0)); fillColor3.setAlphaF(a);

  device->setBrush(fillColor1); device->drawEllipse(device->pixelToWindow(r1));
  device->setBrush(fillColor2); device->drawEllipse(device->pixelToWindow(r2));
  device->setBrush(fillColor3); device->drawEllipse(device->pixelToWindow(r3));

  auto drawText = [&](CQChartsPaintDevice *device, const QPointF &p, const QString &text) {
    QFontMetricsF fm(device->font());

    CQChartsDrawUtil::drawSimpleText(device, QPointF(p.x() - fm.width(text)/2, p.y()), text);
  };

  drawText(device, pixelToWindow(QPointF(r1.center().x(), r1.top())), QString("%1").arg(max ));
  drawText(device, pixelToWindow(QPointF(r2.center().x(), r2.top())), QString("%1").arg(mean));
  drawText(device, pixelToWindow(QPointF(r3.center().x(), r3.top())), QString("%1").arg(min ));
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(const CQChartsScatterPlot *plot, int groupInd,
                        const CQChartsGeom::BBox &rect, const QPointF &pos,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect, is, ig, iv),
 plot_(plot), groupInd_(groupInd), pos_(pos)
{
}

//---

CQChartsSymbol
CQChartsScatterPointObj::
symbolType() const
{
  CQChartsSymbol symbolType = extraData().symbolType;

  if (! symbolType.isValid())
    symbolType = plot_->symbolType();

  return symbolType;
}

CQChartsLength
CQChartsScatterPointObj::
symbolSize() const
{
  CQChartsLength symbolSize = extraData().symbolSize;

  if (! symbolSize.isValid())
    symbolSize = plot()->symbolSize();

  return symbolSize;
}

CQChartsLength
CQChartsScatterPointObj::
fontSize() const
{
  CQChartsLength fontSize = extraData().fontSize;

  if (! fontSize.isValid()) {
    double dataLabelFontSize = plot()->dataLabel()->textFont().pointSizeF();

    fontSize = CQChartsLength(dataLabelFontSize, CQChartsUnits::PIXEL);
  }

  return fontSize;
}

CQChartsColor
CQChartsScatterPointObj::
color() const
{
  CQChartsColor color = extraData().color;

  return color;
}

//---

QString
CQChartsScatterPointObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsScatterPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  // add name (label or name column) as header
  if (name_.length())
    tableTip.addBoldLine(name_);

  //---

  // TODO: id column

  //---

  // add group column
  if (ig_.n > 1) {
    QString groupName = plot_->groupIndName(groupInd_);

    tableTip.addTableRow("Group", groupName);
  }

  //---

  // add x, y columns
  QString xstr = plot()->xStr(pos_.x());
  QString ystr = plot()->yStr(pos_.y());

  tableTip.addTableRow(plot_->xHeaderName(), xstr);
  tableTip.addTableRow(plot_->yHeaderName(), ystr);

  //---

  // get values for name (grouped id identical names)
  CQChartsScatterPlot::ValueData valuePoint;

  auto pg = plot_->groupNameValues().find(groupInd_);
  assert(pg != plot_->groupNameValues().end());

  auto p = (*pg).second.find(name_);

  if (p != (*pg).second.end()) {
    const CQChartsScatterPlot::Values &values = (*p).second.values;

    valuePoint = values[iv_.i];
  }

  //---

  auto addColumnRowValue = [&](const CQChartsColumn &column) {
    if (! column.isValid()) return;

    bool ok;

    QString str = plot_->modelString(modelInd().row(), column, modelInd().parent(), ok);
    if (! ok) return;

    tableTip.addTableRow(plot_->columnHeaderName(column), str);
  };

  //---

  // add symbol type, symbol size and font size columns
  addColumnRowValue(plot_->symbolTypeColumn());
  addColumnRowValue(plot_->symbolSizeColumn());
  addColumnRowValue(plot_->fontSizeColumn  ());

  //---

  // add color column
  if (valuePoint.color.isValid())
    tableTip.addTableRow(plot_->colorHeaderName(), valuePoint.color.colorStr());
  else
    addColumnRowValue(plot_->colorColumn());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  double sx, sy;

  plot_->pixelSymbolSize(this->symbolSize(), sx, sy);

  QPointF p1 = plot_->windowToPixel(pos_);

  CQChartsGeom::BBox pbbox(p1.x() - sx, p1.y() - sy, p1.x() + sx, p1.y() + sy);

  CQChartsGeom::Point pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());

  addColumnSelectIndex(inds, plot_->symbolTypeColumn());
  addColumnSelectIndex(inds, plot_->symbolSizeColumn());
  addColumnSelectIndex(inds, plot_->fontSizeColumn  ());
  addColumnSelectIndex(inds, plot_->colorColumn     ());
}

//---

void
CQChartsScatterPointObj::
draw(CQChartsPaintDevice *device)
{
  drawDir(device, Dir::XY);
}

void
CQChartsScatterPointObj::
drawDir(CQChartsPaintDevice *device, const Dir &dir, bool flip) const
{
  ColorInd ic = calcColorInd();

  //---

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, ic);

  // override symbol fill color for custom color
  CQChartsColor color = this->color();

  if (color.isValid()) {
    QColor c = plot_->interpColor(color, ic);

    c.setAlphaF(plot_->symbolFillAlpha());

    brush.setColor(c);
  }

  plot_->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // get symbol type and size
  CQChartsSymbol symbolType;
  CQChartsLength symbolSize;

  if (dir != Dir::XY) {
    symbolType = plot_->rugSymbolType();
    symbolSize = plot_->rugSymbolSize();

    if (symbolType == CQChartsSymbol::Type::NONE)
      symbolType = (dir == Dir::X ? CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);
  }
  else {
    symbolType = this->symbolType();
    symbolSize = this->symbolSize();
  }

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy);

  //---

  // get point
  QPointF ps = plot_->windowToPixel(pos_);

  if (dir != Dir::XY) {
    // Dir::X and Dir::Y are X/Y Rug Symbols
    CQChartsGeom::BBox pbbox = plot_->calcDataPixelRect();

    if      (dir == Dir::X) {
      if (! flip)
        ps.setY(pbbox.getYMax() + sy);
      else
        ps.setY(pbbox.getYMin() - sy);
    }
    else if (dir == Dir::Y) {
      if (! flip)
        ps.setX(pbbox.getXMin() - sx);
      else
        ps.setX(pbbox.getXMax() + sx);
    }
  }

  //---

  // draw symbol or image
  QImage image = this->image();

  if (image.isNull()) {
    QPointF ps1 = plot_->pixelToWindow(ps);

    plot_->drawSymbol(device, ps1, symbolType, symbolSize, pen, brush);
  }
  else {
    QRectF irect(ps.x() - sx, ps.y() - sy, 2*sx, 2*sy);

    device->drawImageInRect(plot()->pixelToWindow(irect), image);
  }

  //---

  // draw text labels
  if (plot_->dataLabel()->isVisible()) {
    const CQChartsDataLabel *dataLabel = plot_->dataLabel();

    //---

    // text font color
    QPen tpen;

    QColor tc = dataLabel->interpTextColor(ic);

    plot_->setPen(tpen, true, tc, dataLabel->textAlpha());

    //---

    // get font size
    CQChartsLength fontSize = this->fontSize();

    //---

    // set (temp) font
    CQChartsFont font = dataLabel->textFont();

    if (fontSize.isValid()) {
      double fontPixelSize = plot_->lengthPixelHeight(fontSize);

      // scale to font size
      fontPixelSize = plot_->limitFontSize(fontPixelSize);

      CQChartsFont font1 = font;

      font1.setPointSizeF(fontPixelSize);

      const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font1);
    }

    //---

    // draw text
    QRectF erect(ps.x() - sx, ps.y() - sy, 2*sx, 2*sy);

    dataLabel->draw(device, plot_->pixelToWindow(erect), name_, dataLabel->position(), tpen);

    //---

    // reset font
    if (fontSize.isValid()) {
      const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font);
    }
  }
}

double
CQChartsScatterPointObj::
xColorValue(bool relative) const
{
  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  if (relative)
    return CMathUtil::map(pos_.x(), dataRange.xmin(), dataRange.xmax(), 0.0, 1.0);
  else
    return pos_.x();
}

double
CQChartsScatterPointObj::
yColorValue(bool relative) const
{
  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  if (relative)
    return CMathUtil::map(pos_.y(), dataRange.ymin(), dataRange.ymax(), 0.0, 1.0);
  else
    return pos_.y();
}

//------

CQChartsScatterCellObj::
CQChartsScatterCellObj(const CQChartsScatterPlot *plot, int groupInd,
                       const CQChartsGeom::BBox &rect, const ColorInd &is, const ColorInd &ig,
                       int ix, int iy, const Points &points, int maxn) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), ix_(ix), iy_(iy), points_(points), maxn_(maxn)
{
}

QString
CQChartsScatterCellObj::
calcId() const
{
  return QString("%1:%2:%3:%4:%5").arg(typeName()).arg(is_.i).arg(ig_.i).arg(ix_).arg(iy_);
}

QString
CQChartsScatterCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  double xmin, xmax, ymin, ymax;

  plot_->gridData().xinterval.intervalValues(ix_, xmin, xmax);
  plot_->gridData().yinterval.intervalValues(iy_, ymin, ymax);

  tableTip.addTableRow("X Range", QString("%1 %2").arg(xmin).arg(xmax));
  tableTip.addTableRow("Y Range", QString("%1 %2").arg(ymin).arg(ymax));
  tableTip.addTableRow("Count"  , points_.size());

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

bool
CQChartsScatterCellObj::
inside(const CQChartsGeom::Point &p) const
{
  return CQChartsPlotObj::inside(p);
}

void
CQChartsScatterCellObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsScatterCellObj::
draw(CQChartsPaintDevice *device)
{
  // set pen and brush
  ColorInd ic(points_.size(), maxn_);

  QPen   pen;
  QBrush brush;

  QColor pc = plot_->interpGridCellStrokeColor(ColorInd());
  QColor fc = plot_->interpPaletteColor(ic);

  plot_->setPenBrush(pen, brush,
    plot_->isGridCellStroked(), pc, plot_->gridCellStrokeAlpha(),
    plot_->gridCellStrokeWidth(), plot_->gridCellStrokeDash(),
    plot_->isGridCellFilled(), fc, plot_->gridCellFillAlpha(), plot_->gridCellFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw rect
  device->drawRect(rect().qrect());
}

void
CQChartsScatterCellObj::
drawRugSymbol(CQChartsPaintDevice *device, const Dir &dir, bool flip) const
{
  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  //---

  // calc stroke and brush
  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, ic);

  plot_->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // set symbol type and size
  CQChartsSymbol symbolType = plot_->rugSymbolType();
  CQChartsLength symbolSize = plot_->rugSymbolSize();

  if (symbolType == CQChartsSymbol::Type::NONE)
    symbolType = (dir == Dir::X ? CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy);

  //---

  // draw symbols
  for (const auto &p : points_) {
    if (plot_->isInterrupt())
      return;

    QPointF ps = plot_->windowToPixel(p);

    // Dir::X and Dir::Y are X/Y Rug Symbols
    CQChartsGeom::BBox pbbox = plot_->calcDataPixelRect();

    if      (dir == Dir::X) {
      if (! flip)
        ps.setY(pbbox.getYMax() + sy);
      else
        ps.setY(pbbox.getYMin() - sy);
    }
    else if (dir == Dir::Y) {
      if (! flip)
        ps.setX(pbbox.getXMin() - sx);
      else
        ps.setX(pbbox.getXMax() + sx);
    }

    // draw symbol
    QRectF erect(ps.x() - sx, ps.y() - sy, 2*sx, 2*sy);

    plot_->drawSymbol(device, device->pixelToWindow(ps), symbolType, symbolSize, pen, brush);
  }
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int groupInd, const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ic), groupInd_(groupInd)
{
}

bool
CQChartsScatterKeyColor::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  int ih = hideIndex();

  if (selMod == CQChartsSelMod::ADD) {
    for (int i = 0; i < ic_.n; ++i) {
      plot_->CQChartsPlot::setSetHidden(i, i != ih);
    }
  }
  else {
    plot->setSetHidden(ih, ! plot->isSetHidden(ih));
  }

  plot->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsScatterKeyColor::
fillBrush() const
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  QColor c;

  if (color_.isValid())
    c = plot_->interpColor(color_, ColorInd());
  else {
    c = plot->interpSymbolFillColor(ic_);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  c.setAlphaF(plot->symbolFillAlpha());

  int ih = hideIndex();

  if (plot->isSetHidden(ih))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

int
CQChartsScatterKeyColor::
hideIndex() const
{
  return (groupInd_ >= 0 ? groupInd_ : ic_.i);
}

//---

CQChartsScatterGridKeyItem::
CQChartsScatterGridKeyItem(CQChartsScatterPlot *plot) :
 CQChartsKeyItem(plot->key(), ColorInd()), plot_(plot)
{
}

QSizeF
CQChartsScatterGridKeyItem::
size() const
{
  QFont font = plot_->view()->plotFont(plot_, key_->textFont());

  QFontMetricsF fm(font);

  double fw = fm.width("X");
  double fh = fm.height();

  int n = plot_->gridData().maxN;

  double tw = fm.width(QString("%1").arg(n));

  double ww = plot_->pixelToWindowWidth (2*fw + tw + 6);
  double wh = plot_->pixelToWindowHeight(7*fh + fh + 4);

  return QSizeF(ww, wh);
}

void
CQChartsScatterGridKeyItem::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const
{
  // calc text width
  plot_->view()->setPlotPainterFont(plot_, device, key_->textFont());

  QFontMetricsF fm(device->font());

//double fw = fm.width("X");
  double fh = fm.height();

  int n = plot_->gridData().maxN;

  double tw  = fm.width(QString("%1").arg(n));
  double wtw = plot_->pixelToWindowWidth(tw);

  double wxm = plot_->pixelToWindowWidth (2);
  double wym = plot_->pixelToWindowHeight(fh/2 + 2);

  // calc left/right boxes
  CQChartsGeom::BBox lrect(rect.getXMin() + wxm, rect.getYMin() + wym,
                           rect.getXMax() - wtw - 2*wxm, rect.getYMax() - wym);
  CQChartsGeom::BBox rrect(rect.getXMax() - wtw - wxm, rect.getYMin() + wym,
                           rect.getXMax() - wxm, rect.getYMax() - wym);

  CQChartsGeom::BBox lprect = plot_->windowToPixel(lrect);
  CQChartsGeom::BBox rprect = plot_->windowToPixel(rrect);

  //---

  // draw gradient in left box
  QPointF pg1 = QPointF(lprect.getXMin(), lprect.getYMax());
  QPointF pg2 = QPointF(lprect.getXMin(), lprect.getYMin());

  QLinearGradient lg(pg1.x(), pg1.y(), pg2.x(), pg2.y());

  plot_->view()->themePalette()->setLinearGradient(lg, 1.0);

  QBrush brush(lg);

  QRectF frect(pg1.x(), pg2.y(), lprect.getWidth(), lprect.getHeight());

  device->fillRect(device->pixelToWindow(frect), brush);

  //---

  // calc label positions
  int n1 = 0;
  int n5 = n;

  double dn = (n5 - n1)/4.0;

  int n2 = n1 + dn;
  int n4 = n5 - dn;
  int n3 = (n5 + n1)/2.0;

  double y1 = rprect.getYMax();
  double y5 = rprect.getYMin();
  double dy = (y1 - y5)/4.0;

  double y2 = y1 - dy;
  double y4 = y5 + dy;
  double y3 = (y5 + y1)/2.0;

  //---

  // set text pen
  QPen pen;

  QColor tc = plot_->interpThemeColor(ColorInd(1.0));

  plot_->setPen(pen, true, tc, 1.0);

  device->setPen(pen);

  //---

  // draw key labels
  double df = (fm.ascent() - fm.descent())/2.0;

  CQChartsDrawUtil::drawSimpleText(device,
    device->pixelToWindow(QPointF(rprect.getXMin(), y1 + df)), QString("%1").arg(n1));
  CQChartsDrawUtil::drawSimpleText(device,
    device->pixelToWindow(QPointF(rprect.getXMin(), y2 + df)), QString("%1").arg(n2));
  CQChartsDrawUtil::drawSimpleText(device,
    device->pixelToWindow(QPointF(rprect.getXMin(), y3 + df)), QString("%1").arg(n3));
  CQChartsDrawUtil::drawSimpleText(device,
    device->pixelToWindow(QPointF(rprect.getXMin(), y4 + df)), QString("%1").arg(n4));
  CQChartsDrawUtil::drawSimpleText(device,
    device->pixelToWindow(QPointF(rprect.getXMin(), y5 + df)), QString("%1").arg(n5));
}
