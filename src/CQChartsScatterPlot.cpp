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
#include <CQChartsBivariateDensity.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>

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
    setTip("Show Label at Point").setPropPath("labels.visible");

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
  setBestFitFillAlpha(CQChartsAlpha(0.5));

  setStatsLines(false);
  setStatsLinesDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setGridCellFilled (true);
  setGridCellStroked(true);
  setGridCellStrokeColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.1));

  setDataClip(false);

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

  for (const auto &groupWhisker_ : groupWhiskers_)
    delete groupWhisker_.second;
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
  if (n != gridData_.nx()) {
    gridData_.setNX(n); updateRangeAndObjs();
  }
}

void
CQChartsScatterPlot::
setGridNumY(int n)
{
  if (n != gridData_.ny()) {
    gridData_.setNY(n); updateRangeAndObjs();
  }
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
setSymbolMapKey(bool b)
{
  CQChartsUtil::testAndSet(symbolMapKeyData_.displayed, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot::
setSymbolMapKeyAlpha(const CQChartsAlpha &a)
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
      auto *splot = qobject_cast<CQChartsScatterPlot *>(plot);

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
      auto *splot = qobject_cast<CQChartsScatterPlot *>(plot);

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
setDensityAlpha(const CQChartsAlpha &a)
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
setWhiskerAlpha(const CQChartsAlpha &a)
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

  addBaseProperties();

  // columns
  addProp("columns", "xColumn", "x", "X column");
  addProp("columns", "yColumn", "y", "Y column");

  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "labelColumn", "label", "Label column");

  // options
  addProp("options", "plotType", "plotType", "Plot type");

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

  addStyleProp("density/fill", "densityAlpha", "alpha", "Axis density curve alpha");

  // whisker axis
  addProp("whisker"  , "whiskerWidth" , "width"  , "Axis whisker width");
  addProp("whisker"  , "whiskerMargin", "margin" , "Axis whisker margin in pixels");
  addProp("whisker/x", "xWhisker"     , "visible", "Show x axis whisker");
  addProp("whisker/x", "xWhiskerSide" , "side"   , "X axis whisker side");
  addProp("whisker/y", "yWhisker"     , "visible", "Show y axis whisker");
  addProp("whisker/y", "yWhiskerSide" , "side"   , "Y axis whisker side");

  addStyleProp("whisker/fill", "whiskerAlpha" , "alpha", "Axis whisker alpha");

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
  addProp     ("symbol/key"     , "symbolMapKey"      , "visible",
               "Symbol size key visible");
  addProp     ("symbol/key"     , "symbolMapKeyMargin", "margin" ,
               "Symbol size key margin in pixels")->setMinValue(0.0);
  addStyleProp("symbol/key/fill", "symbolMapKeyAlpha" , "alpha"  ,
               "Symbol size key fill alpha");

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

  auto *th = const_cast<CQChartsScatterPlot *>(this);

  //---

  // check columns
  bool columnsValid = true;

  th->clearErrors();

  if (! checkColumn(xColumn(), "X", th->xColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumn(yColumn(), "Y", th->yColumnType_, /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;

  if (! columnsValid)
    return CQChartsGeom::Range(0.0, 0.0, 1.0, 1.0);

  //---

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

      CQChartsModelIndex xModelInd(data.row, plot_->xColumn(), data.parent);
      CQChartsModelIndex yModelInd(data.row, plot_->yColumn(), data.parent);

      // init group
      int groupInd = plot_->rowGroupInd(xModelInd);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));

      if (! hidden) {
        double x   { 0.0  }, y   { 0.0 };
        bool   okx { true }, oky { true };

        //---

        if      (plot_->xColumnType() == CQBaseModelType::REAL ||
                 plot_->xColumnType() == CQBaseModelType::INTEGER) {
          okx = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
        }
        else if (plot_->xColumnType() == CQBaseModelType::TIME) {
          x = plot_->modelReal(xModelInd, okx);
        }
        else {
          x = uniqueId(data, plot_->xColumn()); ++uniqueX_;
        }

        //---

        if      (plot_->yColumnType() == CQBaseModelType::REAL ||
                 plot_->yColumnType() == CQBaseModelType::INTEGER) {
          oky = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);
        }
        else if (plot_->yColumnType() == CQBaseModelType::TIME) {
          y = plot_->modelReal(yModelInd, oky);
        }
        else {
          y = uniqueId(data, plot_->yColumn()); ++uniqueY_;
        }

        //---

        if (plot_->isSkipBad() && (! okx || ! oky))
          return State::SKIP;

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        range_.updateRange(x, y);
      }

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      CQChartsModelIndex columnInd(data.row, column, data.parent);

      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
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

  auto dataRange = visitor.range();

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

  th->initGridData(dataRange);

  //---

  if (isGridCells()) {
    if (dataRange.isSet()) {
      dataRange.updateRange(gridData().xStart(), gridData().yStart());
      dataRange.updateRange(gridData().xEnd  (), gridData().yEnd  ());
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
    gridData_.setXInterval(dataRange.xmin(), dataRange.xmax());
    gridData_.setYInterval(dataRange.ymin(), dataRange.ymax());
  }
  else {
    gridData_.resetXInterval();
    gridData_.resetYInterval();
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

  CQChartsAxisValueType xType = xAxis_->valueType();
  CQChartsAxisValueType yType = yAxis_->valueType();

  if (xType != CQChartsAxisValueType::Type::INTEGER && xType != CQChartsAxisValueType::Type::REAL)
    xType = CQChartsAxisValueType::Type::REAL;
  if (yType != CQChartsAxisValueType::Type::INTEGER && yType != CQChartsAxisValueType::Type::REAL)
    yType = CQChartsAxisValueType::Type::REAL;

  if (isLogX()) xType = CQChartsAxisValueType::Type::LOG;
  if (isLogY()) yType = CQChartsAxisValueType::Type::LOG;

  if (uniqueX) xType = CQChartsAxisValueType::Type::INTEGER;
  if (uniqueY) yType = CQChartsAxisValueType::Type::INTEGER;

  xAxis_->setValueType(xType, /*notify*/false);
  yAxis_->setValueType(yType, /*notify*/false);

  //---

  if (xColumnType_ == CQBaseModelType::TIME)
    xAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);

  if (yColumnType_ == CQBaseModelType::TIME)
    yAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);
}

QString
CQChartsScatterPlot::
xColumnName(const QString &def) const
{
  if (xLabel().length())
    return xLabel();

  bool ok;

  QString xname = modelHHeaderString(xColumn(), ok);

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

  QString yname = modelHHeaderString(yColumn(), ok);

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

  auto *th = const_cast<CQChartsScatterPlot *>(this);

  //---

  // init value set
  //initValueSets();

  //---

  // init name values
  th->gridData_.setMaxN(0);

  if (groupNameValues_.empty())
    addNameValues();

  for (const auto &ghull : th->groupHull_)
    delete ghull.second;

  for (const auto &groupWhisker_ : groupWhiskers_)
    delete groupWhisker_.second;

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
  CQChartsColumnTypeMgr *columnTypeMgr = charts()->columnTypeMgr();

  columnTypeMgr->startCache(model().data());

  //---

  initSymbolTypeData();
  initSymbolSizeData();
  initFontSizeData  ();

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = groupNameValues_.size();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      break;

    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto *th = const_cast<CQChartsScatterPlot *>(this);

    auto pg = th->groupPoints_.find(groupInd);

    if (pg == th->groupPoints_.end())
      pg = th->groupPoints_.insert(pg, GroupPoints::value_type(groupInd, Points()));

    Points &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        break;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const QString &name   = nameValue.first;
      const Values  &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          break;

        //---

        // get point position
        const ValueData &valuePoint = values[iv];

        const auto &p = valuePoint.p;

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

        CQChartsGeom::BBox bbox(p.x - sx, p.y - sy, p.x + sx, p.y + sy);

        auto *pointObj = new CQChartsScatterPointObj(this, groupInd, bbox, p, is1, ig1, iv1);

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
          if (! colorColumnColor(valuePoint.row, valuePoint.ind.parent(), symbolColor))
            symbolColor = CQChartsColor(CQChartsColor::Type::NONE);
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional point label
        QString pointName;

        if (labelColumn().isValid() || nameColumn().isValid()) {
          bool ok;

          if (labelColumn().isValid()) {
            CQChartsModelIndex labelInd(valuePoint.row, labelColumn(), valuePoint.ind.parent());

            pointName = modelString(labelInd, ok);
          }
          else {
            CQChartsModelIndex nameInd(valuePoint.row, nameColumn(), valuePoint.ind.parent());

            pointName = modelString(nameInd, ok);
          }

          if (! ok)
            pointName = "";
        }

        if (pointName.length())
          pointObj->setName(pointName);

        //---

        // set optional image
        QImage image;

        if (imageColumn().isValid()) {
          CQChartsModelIndex imageModelInd(valuePoint.row, imageColumn(), valuePoint.ind.parent());

          bool ok;

          QVariant imageVar = modelValue(imageModelInd, ok);

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

  //---

  columnTypeMgr->endCache(model().data());
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
      break;

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
        break;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

      const CQChartsGridCell &cellPointData = pn.second;

      int maxN = cellPointData.maxN();

      for (const auto &px : cellPointData.xyPoints()) {
        if (isInterrupt())
          break;

        int                              ix      = px.first;
        const CQChartsGridCell::YPoints &yPoints = px.second;

        double xmin, xmax;

        gridData().xIValues(ix, xmin, xmax);

        for (const auto &py : yPoints) {
          if (isInterrupt())
            break;

          int                             iy     = py.first;
          const CQChartsGridCell::Points &points = py.second;

          double ymin, ymax;

          gridData().yIValues(iy, ymin, ymax);

          //---

          ColorInd is1(is, ns);
          ColorInd ig1(ig, ng);

          CQChartsGeom::BBox bbox(xmin, ymin, xmax, ymax);

          auto *cellObj = new CQChartsScatterCellObj(this, groupInd, bbox, is1, ig1,
                                                     ix, iy, points, maxN);

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
      CQChartsModelIndex xModelInd(data.row, plot_->xColumn(), data.parent);
      CQChartsModelIndex yModelInd(data.row, plot_->yColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(xModelInd);

      //---

      // get x, y value
      QModelIndex xInd  = plot_->modelIndex(xModelInd);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      double x   { 0.0  }, y   { 0.0 };
      bool   okx { true }, oky { true };

      //---

      if      (plot_->xColumnType() == CQBaseModelType::REAL ||
               plot_->xColumnType() == CQBaseModelType::INTEGER) {
        okx = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
      }
      else if (plot_->xColumnType() == CQBaseModelType::TIME) {
        x = plot_->modelReal(xModelInd, okx);
      }
      else {
        x = uniqueId(data, plot_->xColumn());
      }

      //---

      if      (plot_->yColumnType() == CQBaseModelType::REAL ||
               plot_->yColumnType() == CQBaseModelType::INTEGER) {
        oky = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);
      }
      else if (plot_->yColumnType() == CQBaseModelType::TIME) {
        y = plot_->modelReal(yModelInd, oky);
      }
      else {
        y = uniqueId(data, plot_->yColumn());
      }

      //---

      if (plot_->isSkipBad() && (! okx || ! oky))
        return State::SKIP;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional grouping name (name column, title, x axis)
      QString name;

      if (plot_->nameColumn().isValid()) {
        CQChartsModelIndex nameColumnInd(data.row, plot_->nameColumn(), data.parent);

        bool ok;

        name = plot_->modelString(nameColumnInd, ok);
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
        (void) plot_->colorColumnColor(data.row, data.parent, color);
      }

      //---

      auto *plot = const_cast<CQChartsScatterPlot *>(plot_);

      plot->addNameValue(groupInd, name, x, y, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      CQChartsModelIndex columnInd(data.row, column, data.parent);

      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
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
  CQChartsGeom::Point p(x, y);

  if (isGridCells()) {
    auto pi = groupNameGridData_.find(groupInd);

    if (pi == groupNameGridData_.end())
      pi = groupNameGridData_.insert(pi, GroupNameGridData::value_type(groupInd, NameGridData()));

    NameGridData &nameGridData = (*pi).second;

    auto pn = nameGridData.find(name);

    if (pn == nameGridData.end())
      pn = nameGridData.insert(pn, NameGridData::value_type(name, gridData_));

    CQChartsGridCell &cellPointData = (*pn).second;

    cellPointData.addPoint(p);

    gridData_.setMaxN(std::max(gridData_.maxN(), cellPointData.maxN()));
  }
  else {
    ValuesData &valuesData = groupNameValues_[groupInd][name];

    valuesData.xrange.add(p.x);
    valuesData.yrange.add(p.y);

    valuesData.values.emplace_back(p, row, xind, color);
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

      auto *colorItem = new CQChartsScatterKeyColor(this, groupInd , ic);
      auto *textItem  = new CQChartsKeyText        (this, groupName, ic);

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

        auto *colorItem = new CQChartsScatterKeyColor(this, -1  , ic);
        auto *textItem  = new CQChartsKeyText        (this, name, ic);

        key->addItem(colorItem, is, 0);
        key->addItem(textItem , is, 1);

        //--

        if (colorColumn().isValid()) {
          int nv = values.size();

          if (nv > 0) {
            const ValueData &valuePoint = values[0];

            CQChartsColor color;

            if (colorColumnColor(valuePoint.row, valuePoint.ind.parent(), color))
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
  auto *item = new CQChartsScatterGridKeyItem(this);

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

  auto c = obj->rect().getCenter();

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
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name, bool isSet, const char *slot) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  menu->addSeparator();

  auto *typeMenu = new QMenu("Plot Type", menu);

  (void) addMenuCheckedAction(typeMenu, "Symbols"   , isSymbols  (), SLOT(setSymbols(bool)));
  (void) addMenuCheckedAction(typeMenu, "Grid Cells", isGridCells(), SLOT(setGridCells(bool)));

  menu->addMenu(typeMenu);

  //---

  auto *overlaysMenu = new QMenu("Overlays", menu);

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

  auto *xMenu = new QMenu("X Axis Annotation", menu);
  auto *yMenu = new QMenu("Y Axis Annotation", menu);

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
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsScatterPlot::calcAnnotationBBox");

  CQChartsGeom::BBox bbox;

  if (isXRug() || isYRug() || isXDensity() || isYDensity() || isXWhisker() || isYWhisker()) {
    const auto &dataRange = this->dataRange();

    //---

    // rug axis
    double sx, sy;

    plotSymbolSize(rugSymbolSize(), sx, sy);

    if (isXRug()) {
      double y = (xRugSide() == YSide::BOTTOM ? dataRange.ymin() - 2*sy :
                                                dataRange.ymax() + 2*sy);

      CQChartsGeom::Point p(dataRange.xmax(), y);

      bbox += p;
    }

    if (isYRug()) {
      double x = (yRugSide() == XSide::LEFT ? dataRange.xmin() - 2*sx :
                                              dataRange.xmax() + 2*sx);

      CQChartsGeom::Point p(x, dataRange.ymax());

      bbox += p;
    }

    //---

    // density axis
    if (isXDensity()) {
      double dw = lengthPlotHeight(densityWidth());

      double pos = (xDensitySide() == YSide::BOTTOM ?
        dataRange.ymin() - dw : dataRange.ymax() + dw);

      CQChartsGeom::Point p1(dataRange.xmax(), pos);

      bbox += p1;
    }

    if (isYDensity()) {
      double dw = lengthPlotWidth(densityWidth());

      double pos = (yDensitySide() == XSide::LEFT ?
        dataRange.xmin() - dw : dataRange.xmax() + dw);

      CQChartsGeom::Point p2(pos, dataRange.ymin());

      bbox += p2;
    }

    //---

    // whisker axis
    if (isXWhisker()) {
      int ng = groupNameValues_.size();

      double wm = lengthPlotHeight(whiskerMargin());
      double ww = ng*lengthPlotHeight(whiskerWidth());

      double pos = (xWhiskerSide() == YSide::BOTTOM ?
        dataRange.ymin() - ww - 2*wm : dataRange.ymax() + ww + 2*wm);

      CQChartsGeom::Point p1(dataRange.xmax(), pos);

      bbox += p1;
    }

    if (isYWhisker()) {
      int ng = groupNameValues_.size();

      double wm = lengthPlotWidth(whiskerMargin());
      double ww = ng*lengthPlotWidth(whiskerWidth());

      double pos = (yWhiskerSide() == XSide::LEFT ?
        dataRange.xmin() - ww - 2*wm : dataRange.xmax() + ww + 2*wm);

      CQChartsGeom::Point p2(pos, dataRange.ymin());

      bbox += p2;
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
  auto *th = const_cast<CQChartsScatterPlot *>(this);

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

        CQChartsGeom::Polygon poly;

        for (const auto &p : points) {
          if (! statData.xstat.isOutlier(p.x) && ! statData.ystat.isOutlier(p.y))
            poly.addPoint(p);
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
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  StatData &statData = th->groupStatData_[groupInd];

  if (! statData.xstat.set || ! statData.ystat.set) {
    auto p = groupPoints_.find(groupInd);

    if (p != groupPoints_.end()) {
      const Points &points = (*p).second;

      std::vector<double> x, y;

      for (std::size_t i = 0; i < points.size(); ++i) {
        x.push_back(points[i].x);
        y.push_back(points[i].y);
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
    CQChartsGeom::Polygon bpoly, poly, tpoly;

    auto pl = CQChartsGeom::Point(fitData.xmin(), 0);
    auto pr = CQChartsGeom::Point(fitData.xmax(), 0);

    for (int px = int(pl.x); px <= int(pr.x); ++px) {
      if (isInterrupt())
        return;

      auto p1 = CQChartsGeom::Point(px, 0.0);

      double y2 = fitData.interp(p1.x);

      auto p2 = CQChartsGeom::Point(p1.x, y2);

      poly.addPoint(p2);

      // deviation curve above/below
      if (isBestFitDeviation()) {
        p2 = CQChartsGeom::Point(p1.x, y2 - fitData.deviation());

        bpoly.addPoint(p2);

        p2 = CQChartsGeom::Point(p1.x, y2 + fitData.deviation());

        tpoly.addPoint(p2);
      }
    }

    //---

    if (poly.size()) {
      // set pen and brush
      ColorInd ic(ig, ng);

      CQChartsPenBrush penBrush;

      QColor strokeColor = interpBestFitStrokeColor(ic);
      QColor fillColor   = interpBestFitFillColor  (ic);

      setPenBrush(penBrush,
        CQChartsPenData  (isBestFitStroked(), strokeColor, bestFitStrokeAlpha(),
                          bestFitStrokeWidth(), bestFitStrokeDash()),
        CQChartsBrushData(isBestFitFilled(), fillColor, bestFitFillAlpha(), bestFitFillPattern()));

      updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      //---

      // draw fit deviation shape
      if (isBestFitDeviation()) {
        CQChartsGeom::Polygon dpoly;

        for (int i = 0; i < bpoly.size(); ++i) {
          auto p = bpoly.point(i);

          dpoly.addPoint(p);
        }

        for (int i = tpoly.size() - 1; i >= 0; --i) {
          auto p = tpoly.point(i);

          dpoly.addPoint(p);
        }

        device->drawPolygon(dpoly);
      }

      //---

      // draw fit line
      QPainterPath path = CQChartsDrawUtil::polygonToPath(poly, /*closed*/false);

      device->strokePath(path, penBrush.pen);
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

    CQChartsPenBrush penBrush;

    QColor c = interpStatsLinesColor(ic);

    setPenBrush(penBrush,
      CQChartsPenData  (true, c, statsLinesAlpha(), statsLinesWidth(), statsLinesDash()),
      CQChartsBrushData(false));

    updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    auto drawXStatLine = [&](double x) {
      CQChartsGeom::Point p1(x, statData.ystat.loutlier);
      CQChartsGeom::Point p2(x, statData.ystat.uoutlier);

      device->drawLine(p1, p2);
    };

    auto drawYStatLine = [&](double y) {
      CQChartsGeom::Point p1(statData.xstat.loutlier, y);
      CQChartsGeom::Point p2(statData.xstat.uoutlier, y);

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

    //---

    // get hull for group (add if needed)
    int groupInd = groupNameValue.first;

    auto *th = const_cast<CQChartsScatterPlot *>(this);

    auto ph = th->groupHull_.find(groupInd);

    if (ph == th->groupHull_.end()) {
      ph = th->groupHull_.insert(ph, GroupHull::value_type(groupInd, new CQChartsGrahamHull));

      //---

      CQChartsGrahamHull *hull = (*ph).second;

      const Points &points = th->groupPoints_[groupInd];

      std::vector<double> x, y;

      for (const auto &p : points) {
        if (isInterrupt())
          return;

        hull->addPoint(p);
      }
    }

    const CQChartsGrahamHull *hull = (*ph).second;

    //---

    // set pen/brush
    ColorInd colorInd(ig, ng);

    CQChartsPenBrush penBrush;

    setPenBrush(penBrush, hullPenData(colorInd), hullBrushData(colorInd));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

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

    auto *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);
    auto *cellObj  = dynamic_cast<CQChartsScatterCellObj  *>(plotObj);

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

    auto *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);
    auto *cellObj  = dynamic_cast<CQChartsScatterCellObj  *>(plotObj);

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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawXDensityWhisker(device, *whiskerData, ColorInd(ig, ng));
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawXDensityWhisker(device, *whiskerData, ColorInd(ig, ng));
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawYDensityWhisker(device, *whiskerData, ColorInd(ig, ng));
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawYDensityWhisker(device, *whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXDensityWhisker(CQChartsPaintDevice *device, const CQChartsXYBoxWhisker &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(penBrush,
    CQChartsPenData  (true, strokeColor, symbolStrokeAlpha()),
    CQChartsBrushData(true, fillColor, densityAlpha()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  const auto &dataRange = this->dataRange();

  const CQChartsDensity &density = whiskerData.xWhisker.density();

  double xmin = density.xmin1();
  double xmax = density.xmax1();

  double dh = lengthPlotHeight(densityWidth());

  double pos = (xDensitySide() == YSide::BOTTOM ?  dataRange.ymin() - dh : dataRange.ymax());

  CQChartsGeom::BBox rect(xmin, pos, xmax, pos + dh);

  density.drawDistribution(this, device, rect, Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYDensityWhisker(CQChartsPaintDevice *device, const CQChartsXYBoxWhisker &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(penBrush,
    CQChartsPenData  (true, strokeColor, symbolStrokeAlpha()),
    CQChartsBrushData(true, fillColor, densityAlpha(), symbolFillPattern()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  const auto &dataRange = this->dataRange();

  const CQChartsDensity &density = whiskerData.yWhisker.density();

  double xmin = density.xmin1();
  double xmax = density.xmax1();

  double dw = lengthPlotWidth(densityWidth());

  double pos = (yDensitySide() == XSide::LEFT ?  dataRange.xmin() - dw : dataRange.xmax());

  CQChartsGeom::BBox rect(pos, xmin, pos + dw, xmax);

  density.drawDistribution(this, device, rect, Qt::Vertical);
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

  CQChartsBivariateDensity density;

  CQChartsBivariateDensity::Data data;

  data.gridSize = densityMapGridSize();
  data.delta    = densityMapDelta();

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    const NameValues &nameValues = groupNameValue.second;

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

      const ValuesData &values = nameValue.second;

      data.values.clear();

      for (const auto &v : values.values)
        data.values.push_back(v.p);

      data.xrange = values.xrange;
      data.yrange = values.yrange;

      density.draw(this, device, data);
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawXWhiskerWhisker(device, *whiskerData, ColorInd(ig, ng));
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawXWhiskerWhisker(device, *whiskerData, ColorInd(ig, ng));
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawYWhiskerWhisker(device, *whiskerData, ColorInd(ig, ng));
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
        CQChartsXYBoxWhisker *whiskerData = (*p).second;

        drawYWhiskerWhisker(device, *whiskerData, ColorInd(ig, ng));
      }

      ++ig;
    }
  }
}

void
CQChartsScatterPlot::
drawXWhiskerWhisker(CQChartsPaintDevice *device, const CQChartsXYBoxWhisker &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(penBrush,
    CQChartsPenData  (true, strokeColor, symbolStrokeAlpha()),
    CQChartsBrushData(true, fillColor, whiskerAlpha(), symbolFillPattern()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  double ww = lengthPlotHeight(whiskerWidth ());
  double wm = lengthPlotHeight(whiskerMargin());

  const auto &dataRange = this->dataRange();

  double pos = (xWhiskerSide() == YSide::BOTTOM ?
    dataRange.ymin() - (ig.i + 1)*ww - wm : dataRange.ymax() + ig.i*ww + wm);

  CQChartsGeom::BBox rect(whiskerData.xWhisker.min(), pos, whiskerData.xWhisker.max(), pos + ww);

  CQChartsBoxWhiskerUtil::drawWhisker(this, device, whiskerData.xWhisker,
                                      rect, whiskerWidth(), Qt::Horizontal);
}

void
CQChartsScatterPlot::
drawYWhiskerWhisker(CQChartsPaintDevice *device, const CQChartsXYBoxWhisker &whiskerData,
                    const ColorInd &ig) const
{
  // calc pen/brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(penBrush,
    CQChartsPenData  (true, strokeColor, symbolStrokeAlpha()),
    CQChartsBrushData(true, fillColor, whiskerAlpha(), symbolFillPattern()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  double ww = lengthPlotWidth(whiskerWidth());
  double wm = lengthPlotWidth(whiskerMargin());

  const auto &dataRange = this->dataRange();

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

    auto *th = const_cast<CQChartsScatterPlot *>(this);

    auto pw = th->groupWhiskers_.find(groupInd);

    if (pw == th->groupWhiskers_.end())
      pw = th->groupWhiskers_.insert(pw,
             GroupWhiskers::value_type(groupInd, new CQChartsXYBoxWhisker));

    CQChartsXYBoxWhisker *whiskerData = (*pw).second;

    //---

    // init whisker if needed
    if (! whiskerData->xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          auto *whiskerData1 = const_cast<CQChartsXYBoxWhisker *>(whiskerData);

          whiskerData1->xWhisker.addValue(pointObj->point().x);
        }
      }
    }

    if (! whiskerData->yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterPointObj *pointObj =
          dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          auto *whiskerData1 = const_cast<CQChartsXYBoxWhisker *>(whiskerData);

          whiskerData1->yWhisker.addValue(pointObj->point().y);
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

    auto *th = const_cast<CQChartsScatterPlot *>(this);

    auto pw = th->groupWhiskers_.find(groupInd);

    if (pw == th->groupWhiskers_.end())
      pw = th->groupWhiskers_.insert(pw,
             GroupWhiskers::value_type(groupInd, new CQChartsXYBoxWhisker));

    CQChartsXYBoxWhisker *whiskerData = (*pw).second;

    //---

    // init whisker if needed
    if (! whiskerData->xWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            auto *whiskerData1 = const_cast<CQChartsXYBoxWhisker *>(whiskerData);

            whiskerData1->xWhisker.addValue(p.x);
          }
        }
      }
    }

    if (! whiskerData->yWhisker.numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const CQChartsScatterCellObj *cellObj =
          dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            auto *whiskerData1 = const_cast<CQChartsXYBoxWhisker *>(whiskerData);

            whiskerData1->yWhisker.addValue(p.y);
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

  auto pbbox = calcPlotPixelRect();

  //double px, py;

  //double vx = view()->viewportRange();
  //double vy = 0.0;

  //auto p = view()->windowToPixel(CQChartsGeom::Point(vx, vy));

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

  CQChartsGeom::BBox pbbox1(xm - pr1, ym - 2*pr1, xm + pr1, ym);
  CQChartsGeom::BBox pbbox2(xm - pr2, ym - 2*pr2, xm + pr2, ym);
  CQChartsGeom::BBox pbbox3(xm - pr3, ym - 2*pr3, xm + pr3, ym);

  CQChartsAlpha a = symbolMapKeyAlpha();

  QColor fillColor1 = interpSymbolFillColor(ColorInd(1.0)); fillColor1.setAlphaF(a.value());
  QColor fillColor2 = interpSymbolFillColor(ColorInd(0.5)); fillColor2.setAlphaF(a.value());
  QColor fillColor3 = interpSymbolFillColor(ColorInd(0.0)); fillColor3.setAlphaF(a.value());

  device->setBrush(fillColor1); device->drawEllipse(device->pixelToWindow(pbbox1));
  device->setBrush(fillColor2); device->drawEllipse(device->pixelToWindow(pbbox2));
  device->setBrush(fillColor3); device->drawEllipse(device->pixelToWindow(pbbox3));

  auto drawText = [&](const CQChartsGeom::Point &p, double value) {
    QString text = QString("%1").arg(value);

    QFontMetricsF fm(device->font());

    CQChartsGeom::Point p1(p.x - fm.width(text)/2, p.y);

    auto p2 = device->pixelToWindow(p1);

    CQChartsTextOptions options;

    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p2, text, options);
  };

  drawText(CQChartsGeom::Point(pbbox1.getXMid(), pbbox1.getYMin()), max );
  drawText(CQChartsGeom::Point(pbbox2.getXMid(), pbbox2.getYMin()), mean);
  drawText(CQChartsGeom::Point(pbbox3.getXMid(), pbbox3.getYMin()), min );
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(const CQChartsScatterPlot *plot, int groupInd,
                        const CQChartsGeom::BBox &rect, const CQChartsGeom::Point &pos,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect, is, ig, iv),
 plot_(plot), groupInd_(groupInd), pos_(pos)
{
  setDetailHint(DetailHint::MAJOR);
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
  QString xstr = plot()->xStr(pos_.x);
  QString ystr = plot()->yStr(pos_.y);

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

    CQChartsModelIndex columnInd(modelInd().row(), column, modelInd().parent());

    bool ok;

    QString str = plot_->modelString(columnInd, ok);
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

  auto p1 = plot_->windowToPixel(pos_);

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

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
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

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
  auto ps = plot_->windowToPixel(pos_);

  if (dir != Dir::XY) {
    // Dir::X and Dir::Y are X/Y Rug Symbols
    auto pbbox = plot_->calcDataPixelRect();

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
    auto ps1 = plot_->pixelToWindow(ps);

    plot_->drawSymbol(device, ps1, symbolType, symbolSize, penBrush);
  }
  else {
    double aspect = (1.0*image.width())/image.height();

    if (aspect > 1.0) {
      sy = sx;
      sx = sy*aspect;
    }
    else {
      sy = sx*(1.0/aspect);
    }

    CQChartsGeom::BBox ibbox(ps.x - sx, ps.y - sy, ps.x + 2*sx, ps.y + 2*sy);

    device->drawImageInRect(plot()->pixelToWindow(ibbox), image);
  }

  device->resetColorNames();

  //---

  // draw text labels
  if (plot_->dataLabel()->isVisible()) {
    const CQChartsDataLabel *dataLabel = plot_->dataLabel();

    //---

    // text font color
    ColorInd ic = calcColorInd();

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
    CQChartsGeom::BBox ptbbox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

    dataLabel->draw(device, plot_->pixelToWindow(ptbbox), name_, dataLabel->position(), tpen);

    //---

    // reset font
    if (fontSize.isValid()) {
      const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font);
    }
  }
}

void
CQChartsScatterPointObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd ic = calcColorInd();

  plot_->setSymbolPenBrush(penBrush, ic);

  // override symbol fill color for custom color
  CQChartsColor color = this->color();

  if (color.isValid()) {
    QColor c = plot_->interpColor(color, ic);

    c.setAlphaF(plot_->symbolFillAlpha().value());

    penBrush.brush.setColor(c);
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
}

double
CQChartsScatterPointObj::
xColorValue(bool relative) const
{
  const auto &dataRange = plot_->dataRange();

  if (relative)
    return CMathUtil::map(pos_.x, dataRange.xmin(), dataRange.xmax(), 0.0, 1.0);
  else
    return pos_.x;
}

double
CQChartsScatterPointObj::
yColorValue(bool relative) const
{
  const auto &dataRange = plot_->dataRange();

  if (relative)
    return CMathUtil::map(pos_.y, dataRange.ymin(), dataRange.ymax(), 0.0, 1.0);
  else
    return pos_.y;
}

//------

CQChartsScatterCellObj::
CQChartsScatterCellObj(const CQChartsScatterPlot *plot, int groupInd,
                       const CQChartsGeom::BBox &rect, const ColorInd &is, const ColorInd &ig,
                       int ix, int iy, const Points &points, int maxn) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), ix_(ix), iy_(iy), points_(points), maxn_(maxn)
{
  setDetailHint(DetailHint::MAJOR);
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

  plot_->gridData().xIValues(ix_, xmin, xmax);
  plot_->gridData().yIValues(iy_, ymin, ymax);

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
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw rect
  device->drawRect(rect());

  //---

  device->resetColorNames();
}

void
CQChartsScatterCellObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set pen and brush
  ColorInd ic(points_.size(), maxn_);

  QColor pc = plot_->interpGridCellStrokeColor(ColorInd());
  QColor fc = plot_->interpPaletteColor(ic);

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (plot_->isGridCellStroked(), pc, plot_->gridCellStrokeAlpha(),
                      plot_->gridCellStrokeWidth(), plot_->gridCellStrokeDash()),
    CQChartsBrushData(plot_->isGridCellFilled(), fc, plot_->gridCellFillAlpha(),
                      plot_->gridCellFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsScatterCellObj::
drawRugSymbol(CQChartsPaintDevice *device, const Dir &dir, bool flip) const
{
  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  //---

  // calc stroke and brush
  CQChartsPenBrush penBrush;

  plot_->setSymbolPenBrush(penBrush, ic);

  plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

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

    auto ps = plot_->windowToPixel(p);

    // Dir::X and Dir::Y are X/Y Rug Symbols
    auto pbbox = plot_->calcDataPixelRect();

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
    plot_->drawSymbol(device, device->pixelToWindow(ps), symbolType, symbolSize, penBrush);
  }
}

void
CQChartsScatterCellObj::
writeScriptData(CQChartsScriptPainter *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.count = " << points_.size() << ";\n";
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
  auto *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

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
  auto *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  QColor c;

  if (color_.isValid())
    c = plot_->interpColor(color_, ColorInd());
  else {
    c = plot->interpSymbolFillColor(ic_);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  c.setAlphaF(plot->symbolFillAlpha().value());

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

CQChartsGeom::Size
CQChartsScatterGridKeyItem::
size() const
{
  QFont font = plot_->view()->plotFont(plot_, key_->textFont());

  QFontMetricsF fm(font);

  double fw = fm.width("X");
  double fh = fm.height();

  int n = plot_->gridData().maxN();

  double tw = fm.width(QString("%1").arg(n));

  double ww = plot_->pixelToWindowWidth (2*fw + tw + 6);
  double wh = plot_->pixelToWindowHeight(7*fh + fh + 4);

  return CQChartsGeom::Size(ww, wh);
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

  int n = plot_->gridData().maxN();

  double tw  = fm.width(QString("%1").arg(n));
  double wtw = plot_->pixelToWindowWidth(tw);

  double wxm = plot_->pixelToWindowWidth (2);
  double wym = plot_->pixelToWindowHeight(fh/2 + 2);

  // calc left/right boxes
  CQChartsGeom::BBox lrect(rect.getXMin() + wxm, rect.getYMin() + wym,
                           rect.getXMax() - wtw - 2*wxm, rect.getYMax() - wym);
  CQChartsGeom::BBox rrect(rect.getXMax() - wtw - wxm, rect.getYMin() + wym,
                           rect.getXMax() - wxm, rect.getYMax() - wym);

  auto lprect = plot_->windowToPixel(lrect);
  auto rprect = plot_->windowToPixel(rrect);

  //---

  // draw gradient in left box
  CQChartsGeom::Point pg1(lprect.getXMin(), lprect.getYMax());
  CQChartsGeom::Point pg2(lprect.getXMin(), lprect.getYMin());

  QLinearGradient lg(pg1.x, pg1.y, pg2.x, pg2.y);

  plot_->view()->themePalette()->setLinearGradient(lg, 1.0);

  QBrush brush(lg);

  CQChartsGeom::BBox fbbox(pg1.x                    , pg2.y,
                           pg1.x + lprect.getWidth(), pg2.y + lprect.getHeight());

  device->fillRect(device->pixelToWindow(fbbox), brush);

  //---

  // calc label positions
  int n1 = 0;
  int n5 = n;

  double dn = (n5 - n1)/4.0;

  int n2 = int(n1 + dn);
  int n4 = int(n5 - dn);
  int n3 = int((n5 + n1)/2.0);

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

  plot_->setPen(pen, true, tc, CQChartsAlpha());

  device->setPen(pen);

  //---

  // draw key labels
  auto drawTextLabel = [&](const CQChartsGeom::Point &p, int n) {
    auto p1 = device->pixelToWindow(p);

    QString text = QString("%1").arg(n);

    CQChartsTextOptions options;

    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, text, options);
  };

  double x1 = rprect.getXMin();
  double df = (fm.ascent() - fm.descent())/2.0;

  drawTextLabel(CQChartsGeom::Point(x1, y1 + df), n1);
  drawTextLabel(CQChartsGeom::Point(x1, y2 + df), n2);
  drawTextLabel(CQChartsGeom::Point(x1, y3 + df), n3);
  drawTextLabel(CQChartsGeom::Point(x1, y4 + df), n4);
  drawTextLabel(CQChartsGeom::Point(x1, y5 + df), n5);
}
