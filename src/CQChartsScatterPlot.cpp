#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsAxisRug.h>
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
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
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
    addNameValue("HEX_CELLS" , int(CQChartsScatterPlot::PlotType::HEX_CELLS )).
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
create(View *view, const ModelP &model) const
{
  return new CQChartsScatterPlot(view, model);
}

//---

CQChartsScatterPlot::
CQChartsScatterPlot(View *view, const ModelP &model) :
 CQChartsPointPlot(view, view->charts()->plotType("scatter"), model),
 CQChartsObjPointData        <CQChartsScatterPlot>(this),
 CQChartsObjGridCellShapeData<CQChartsScatterPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(Color(Color::Type::PALETTE));

  setGridCellFilled (true);
  setGridCellStroked(true);
  setGridCellStrokeColor(Color(Color::Type::INTERFACE_VALUE, 0.1));

  setDataClip(false);

  //---

  addAxes();

  addKey();

  addTitle();

  //---

  auto createAxisDensity = [&](Qt::Orientation orient) {
    auto *density = new AxisDensity(this, orient);

    density->setVisible(false);
    density->setSide(AxisDensity::Side::TOP_RIGHT);

    return density;
  };

  xAxisDensity_ = createAxisDensity(Qt::Horizontal);
  yAxisDensity_ = createAxisDensity(Qt::Vertical);

  //---

  auto createAxisWhisker = [&](Qt::Orientation orient) {
    auto *whisker = new AxisBoxWhisker(this, orient);

    whisker->setVisible(false);
    whisker->setSide(AxisBoxWhisker::Side::TOP_RIGHT);

    return whisker;
  };

  xAxisWhisker_ = createAxisWhisker(Qt::Horizontal);
  yAxisWhisker_ = createAxisWhisker(Qt::Vertical);
}

CQChartsScatterPlot::
~CQChartsScatterPlot()
{
  for (const auto &ghull : groupHull_)
    delete ghull.second;

  for (const auto &groupWhisker_ : groupXWhiskers_)
    delete groupWhisker_.second;

  for (const auto &groupWhisker_ : groupYWhiskers_)
    delete groupWhisker_.second;

  delete xAxisDensity_;
  delete yAxisDensity_;

  delete xAxisWhisker_;
  delete yAxisWhisker_;
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
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot::
setGridNumX(int n)
{
  if (n != gridData_.nx()) {
    gridData_.setNX(n);

    if (! isSymbols())
      updateRangeAndObjs();
  }
}

void
CQChartsScatterPlot::
setGridNumY(int n)
{
  if (n != gridData_.ny()) {
    gridData_.setNY(n);

    if (! isSymbols())
      updateRangeAndObjs();
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
setSymbolMapKeyAlpha(const Alpha &a)
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

void
CQChartsScatterPlot::
setHexCells(bool b)
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      auto *splot = qobject_cast<CQChartsScatterPlot *>(plot);

      if (splot)
        splot->plotType_ = (b ? PlotType::HEX_CELLS : PlotType::SYMBOLS);
    });

    updateRangeAndObjs();
  }
  else {
    CQChartsUtil::testAndSet(plotType_,
     (b ? PlotType::HEX_CELLS : PlotType::SYMBOLS), [&]() { updateRangeAndObjs(); } );
  }
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

bool
CQChartsScatterPlot::
isXDensity() const
{
  return xAxisDensity_->isVisible();
}

void
CQChartsScatterPlot::
setXDensity(bool b)
{
  if (b != isXDensity()) { xAxisDensity_->setVisible(b); resetAnnotationBBox(); drawObjs(); }
}

bool
CQChartsScatterPlot::
isYDensity() const
{
  return yAxisDensity_->isVisible();
}

void
CQChartsScatterPlot::
setYDensity(bool b)
{
  if (b != isYDensity()) { yAxisDensity_->setVisible(b); resetAnnotationBBox(); drawObjs(); }
}

//------

bool
CQChartsScatterPlot::
isXWhisker() const
{
  return xAxisWhisker_->isVisible();
}

void
CQChartsScatterPlot::
setXWhisker(bool b)
{
  if (b != isXWhisker()) { xAxisWhisker_->setVisible(b); resetAnnotationBBox(); drawObjs(); }
}

bool
CQChartsScatterPlot::
isYWhisker() const
{
  return yAxisWhisker_->isVisible();
}

void
CQChartsScatterPlot::
setYWhisker(bool b)
{
  if (b != isYWhisker()) { yAxisWhisker_->setVisible(b); resetAnnotationBBox(); drawObjs(); }
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
    auto *item = addProp(path, name, alias, desc);
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

  //---

  // options
  addProp("options", "plotType", "plotType", "Plot type");

  //---

  // best fit line and deviation fill
  addBestFitProperties();

  // convex hull shape
  addHullProperties();

  // stats
  void addStatsProperties();

  //---

  // density map
  addProp("densityMap", "densityMap"        , "visible" , "Show density map overlay");
  addProp("densityMap", "densityMapGridSize", "gridSize", "Density map grid size");
  addProp("densityMap", "densityMapDelta"   , "delta"   , "Density map delta");

  // rug axis
  addRugProperties();

  // x/y density axis
  xAxisDensity_->addProperties("density/x", "X axis density");
  yAxisDensity_->addProperties("density/y", "Y axis density");

  // x/y whisker axis
  xAxisWhisker_->addProperties("whisker/x", "X axis whisker");
  yAxisWhisker_->addProperties("whisker/y", "Y axis whisker");

  //---

  // symbol
  addSymbolProperties("symbol", "", "");

  // data labels
  dataLabel()->addPathProperties("labels", "Labels");

  //---

  // grid
  addProp("gridCells", "gridNumX", "nx", "Number of x grid cells");
  addProp("gridCells", "gridNumY", "ny", "Number of y grid cells");

  addStyleProp     ("gridCells/fill"  , "gridCellFilled" , "visible", "Grid cell fill visible");
  addFillProperties("gridCells/fill"  , "gridCellFill"   , "Grid cell");
  addStyleProp     ("gridCells/stroke", "gridCellStroked", "visible", "Grid cell stroke visible");
  addLineProperties("gridCells/stroke", "gridCellStroke" , "Grid cell");

  //---

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

  //---

  // color map
  addColorMapProperties();
}

//---

CQChartsGeom::Range
CQChartsScatterPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsScatterPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsScatterPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumn(xColumn(), "X", th->xColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumn(yColumn(), "Y", th->yColumnType_, /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

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

      auto *plot = const_cast<CQChartsScatterPlot *>(plot_);

      ModelIndex xModelInd(plot, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot, data.row, plot_->yColumn(), data.parent);

      // init group
      int groupInd = plot_->rowGroupInd(xModelInd);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));

      if (! hidden) {
        double x   { 0.0  }, y   { 0.0  };
        bool   okx { true }, oky { true };

        //---

        if      (plot_->xColumnType() == ColumnType::REAL ||
                 plot_->xColumnType() == ColumnType::INTEGER) {
          okx = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
        }
        else if (plot_->xColumnType() == ColumnType::TIME) {
          x = plot_->modelReal(xModelInd, okx);
        }
        else {
          x = uniqueId(data, plot_->xColumn()); ++numUniqueX_;
        }

        //---

        if      (plot_->yColumnType() == ColumnType::REAL ||
                 plot_->yColumnType() == ColumnType::INTEGER) {
          oky = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);
        }
        else if (plot_->yColumnType() == ColumnType::TIME) {
          y = plot_->modelReal(yModelInd, oky);
        }
        else {
          y = uniqueId(data, plot_->yColumn()); ++numUniqueY_;
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
      auto *plot = const_cast<CQChartsScatterPlot *>(plot_);

      ModelIndex columnInd(plot, data.row, column, data.parent);

      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        auto *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

    const Range &range() const { return range_; }

    bool isUniqueX() const { return numUniqueX_ == numRows(); }
    bool isUniqueY() const { return numUniqueY_ == numRows(); }

   private:
    const CQChartsScatterPlot* plot_       { nullptr };
    int                        hasGroups_  { false };
    Range                      range_;
    CQChartsModelDetails*      details_    { nullptr };
    int                        numUniqueX_ { 0 };
    int                        numUniqueY_ { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange = visitor.range();

  th->uniqueX_ = visitor.isUniqueX();
  th->uniqueY_ = visitor.isUniqueY();

  if (isInterrupt())
    return dataRange;

  //---

  if (dataRange.isSet()) {
    if (isUniqueX() || isUniqueY()) {
      if (isUniqueX()) {
        auto *columnDetails = this->columnDetails(xColumn());

        for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i)
          xAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());

        dataRange.updateRange(dataRange.xmin() - 0.5, dataRange.ymin());
        dataRange.updateRange(dataRange.xmax() + 0.5, dataRange.ymin());
      }

      if (isUniqueY()) {
        auto *columnDetails = this->columnDetails(yColumn());

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

  th->initAxes();

  //---

  return dataRange;
}

void
CQChartsScatterPlot::
initGridData(const Range &dataRange)
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
initAxes()
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

  xAxis()->setDefLabel("", /*notify*/false);
  yAxis()->setDefLabel("", /*notify*/false);

  //---

  xAxis()->setColumn(xColumn());
  yAxis()->setColumn(yColumn());

  //---

  QString xname;

  (void) xAxisName(xname, "X");

  xAxis()->setDefLabel(xname);

  QString yname;

  (void) yAxisName(yname, "Y");

  yAxis()->setDefLabel(yname);

  //---

  auto xType = xAxis()->valueType();
  auto yType = yAxis()->valueType();

  if (xType != CQChartsAxisValueType::Type::INTEGER && xType != CQChartsAxisValueType::Type::REAL)
    xType = CQChartsAxisValueType::Type::REAL;
  if (yType != CQChartsAxisValueType::Type::INTEGER && yType != CQChartsAxisValueType::Type::REAL)
    yType = CQChartsAxisValueType::Type::REAL;

  if (isLogX()) xType = CQChartsAxisValueType::Type::LOG;
  if (isLogY()) yType = CQChartsAxisValueType::Type::LOG;

  if (isUniqueX()) xType = CQChartsAxisValueType::Type::INTEGER;
  if (isUniqueY()) yType = CQChartsAxisValueType::Type::INTEGER;

  xAxis()->setValueType(xType, /*notify*/false);
  yAxis()->setValueType(yType, /*notify*/false);

  //---

  if (xColumnType_ == ColumnType::TIME)
    xAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);

  if (yColumnType_ == ColumnType::TIME)
    yAxis()->setValueType(CQChartsAxisValueType::Type::DATE, /*notify*/false);

  //---

  if (isOverlay() && isFirstPlot())
    setOverlayPlotsAxisNames();
}

bool
CQChartsScatterPlot::
xAxisName(QString &name, const QString &def) const
{
  if (xLabel().length()) {
    name = xLabel();
    return true;
  }

  bool ok;

  name = modelHHeaderString(xColumn(), ok);

  if (! ok || ! name.length())
    name = def;

  return name.length();
}

bool
CQChartsScatterPlot::
yAxisName(QString &name, const QString &def) const
{
  if (yLabel().length()) {
    name = yLabel();
    return true;
  }

  bool ok;

  name = modelHHeaderString(yColumn(), ok);

  if (! ok || ! name.length())
    name = def;

  return name.length();
}

//------

void
CQChartsScatterPlot::
clearPlotObjects()
{
  groupInds_        .clear();
  groupNameValues_  .clear();
  groupNameGridData_.clear();
  groupNameHexData_ .clear();

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

  th->hexMap_.clear();
  th->hexMapMaxN_ = 0;

  if (groupInds_.empty())
    addNameValues();

  th->groupPoints_  .clear();
  th->groupFitData_ .clear();
  th->groupStatData_.clear();

  //---

  for (const auto &ghull : th->groupHull_)
    delete ghull.second;

  th->groupHull_.clear();

  //---

  for (const auto &groupWhisker_ : groupXWhiskers_)
    delete groupWhisker_.second;

  for (const auto &groupWhisker_ : groupYWhiskers_)
    delete groupWhisker_.second;

  th->groupXWhiskers_.clear();
  th->groupYWhiskers_.clear();

  //---

  th->updateColumnNames();

  //---

  addPointObjects(objs);
  addGridObjects (objs);
  addHexObjects  (objs);

  //---

  return true;
}

void
CQChartsScatterPlot::
updateColumnNames()
{
  // set column header names
  CQChartsPlot::updateColumnNames();

  QString xname, yname;

  (void) xAxisName(xname, "X");
  (void) yAxisName(yname, "Y");

  columnNames_[xColumn()] = xname;
  columnNames_[yColumn()] = yname;

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
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  auto *columnTypeMgr = charts()->columnTypeMgr();

  columnTypeMgr->startCache(model().data());

  //---

  initSymbolTypeData();
  initSymbolSizeData();
  initFontSizeData  ();

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = groupInds_.size();

  if (! hasGroups) {
    if (ng <= 1 && parentPlot()) {
      ig = parentPlot()->childPlotIndex(this);
      ng = parentPlot()->numChildPlots();
    }
  }

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      break;

    int         groupInd   = groupNameValue.first;
    const auto &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto pg = th->groupPoints_.find(groupInd);

    if (pg == th->groupPoints_.end())
      pg = th->groupPoints_.insert(pg, GroupPoints::value_type(groupInd, Points()));

    auto &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        break;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const auto &name   = nameValue.first;
      const auto &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          break;

        //---

        // get point position
        const auto &valuePoint = values[iv];

        const auto &p = valuePoint.p;

        //---

        // get symbol size (needed for bounding box)
        Length symbolSize(CQChartsUnits::NONE, 0.0);

        if (symbolSizeColumn().isValid()) {
          if (! columnSymbolSize(valuePoint.row, valuePoint.ind.parent(), symbolSize))
            symbolSize = Length(CQChartsUnits::NONE, 0.0);
        }

        double sx, sy;

        plotSymbolSize(symbolSize.isValid() ? symbolSize : this->symbolSize(), sx, sy);

        //---

        // create point object
        ColorInd is1(is, ns);
        ColorInd ig1(ig, ng);
        ColorInd iv1(iv, nv);

        BBox bbox(p.x - sx, p.y - sy, p.x + sx, p.y + sy);

        auto *pointObj = createPointObj(groupInd, bbox, p, is1, ig1, iv1);

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
        Length fontSize(CQChartsUnits::NONE, 0.0);

        if (fontSizeColumn().isValid()) {
          if (! columnFontSize(valuePoint.row, valuePoint.ind.parent(), fontSize))
            fontSize = Length(CQChartsUnits::NONE, 0.0);
        }

        if (fontSize.isValid())
          pointObj->setFontSize(fontSize);

        //---

        // set optional font
        if (fontColumn().isValid()) {
          CQChartsFont font;

          if (fontColumnFont(valuePoint.row, valuePoint.ind.parent(), font))
            pointObj->setFont(font);
        }

        //---

        // set optional symbol fill color
        Color symbolColor(Color::Type::NONE);

        if (colorColumn().isValid()) {
          if (! colorColumnColor(valuePoint.row, valuePoint.ind.parent(), symbolColor))
            symbolColor = Color(Color::Type::NONE);
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional point label
        QString pointName;
        Column  pointNameColumn;

        if (labelColumn().isValid() || nameColumn().isValid()) {
          bool ok;

          if (labelColumn().isValid()) {
            ModelIndex labelInd(th, valuePoint.row, labelColumn(), valuePoint.ind.parent());

            pointName = modelString(labelInd, ok);
            if (ok) pointNameColumn = labelColumn();
          }

          if (nameColumn().isValid() && ! pointNameColumn.isValid()) {
            ModelIndex nameInd(th, valuePoint.row, nameColumn(), valuePoint.ind.parent());

            pointName = modelString(nameInd, ok);
            if (ok) pointNameColumn = nameColumn();
          }
        }

        if (pointNameColumn.isValid() && pointName.length()) {
          pointObj->setName      (pointName);
          pointObj->setNameColumn(pointNameColumn);
        }

        //---

        // set optional image
        CQChartsImage image;

        if (imageColumn().isValid()) {
          ModelIndex imageModelInd(th, valuePoint.row, imageColumn(), valuePoint.ind.parent());

          bool ok;

          QVariant imageVar = modelValue(imageModelInd, ok);

          if (ok)
            image = CQChartsVariant::toImage(imageVar, ok);
        }

        if (image.isValid())
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

  int maxN = gridData_.maxN();

  int ig = 0;
  int ng = groupNameGridData_.size();

  for (const auto &pg : groupNameGridData_) {
    if (isInterrupt())
      break;

    int         groupInd     = pg.first;
    const auto &nameGridData = pg.second;

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

      const auto &cellPointData = pn.second;

    //int maxN = cellPointData.maxN();

      for (const auto &px : cellPointData.xyPoints()) {
        if (isInterrupt())
          break;

        int         ix      = px.first;
        const auto &yPoints = px.second;

        double xmin, xmax;

        gridData().xIValues(ix, xmin, xmax);

        for (const auto &py : yPoints) {
          if (isInterrupt())
            break;

          int         iy     = py.first;
          const auto &points = py.second;

          double ymin, ymax;

          gridData().yIValues(iy, ymin, ymax);

          //---

          ColorInd is1(is, ns);
          ColorInd ig1(ig, ng);

          BBox bbox(xmin, ymin, xmax, ymax);

          auto *cellObj = createCellObj(groupInd, bbox, is1, ig1, ix, iy, points, maxN);

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
addHexObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  int maxN = hexMapMaxN_;

  int ig = 0;
  int ng = groupNameHexData_.size();

  for (const auto &pg : groupNameHexData_) {
    if (isInterrupt())
      break;

    int         groupInd    = pg.first;
    const auto &nameHexData = pg.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    int is = 0;
    int ns = nameHexData.size();

    for (const auto &pn : nameHexData) {
      if (isInterrupt())
        break;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

      const auto &hexMap = pn.second;

    //int maxN = hexMap.numData();

      for (const auto &px : hexMap.data()) {
        if (isInterrupt())
          break;

        int         i     = px.first;
        const auto &jData = px.second;

        double xmin, xmax;

        gridData().xIValues(i, xmin, xmax);

        for (const auto &py : jData) {
          if (isInterrupt())
            break;

          int         j         = py.first;
          const auto &dataArray = py.second;

          int n = dataArray.size();

          HexMap::Polygon ipolygon;

          hexMap.indexPolygon(i, j, ipolygon);

          Polygon polygon;

          for (auto &p : ipolygon) {
            Point pv(p.x, p.y);

            auto pw = viewToWindow(pv);

            polygon.addPoint(pw);
          }

          auto bbox = polygon.boundingBox();

          //---

          ColorInd is1(is, ns);
          ColorInd ig1(ig, ng);

          auto *hexObj = createHexObj(groupInd, bbox, is1, ig1, i, j, polygon, n, maxN);

          objs.push_back(hexObj);
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
      auto *plot = const_cast<CQChartsScatterPlot *>(plot_);

      ModelIndex xModelInd(plot, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot, data.row, plot_->yColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(xModelInd);

      //---

      // get x, y value
      QModelIndex xInd  = plot_->modelIndex(xModelInd);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      double x   { 0.0  }, y   { 0.0  };
      bool   okx { true }, oky { true };

      //---

      if      (plot_->xColumnType() == ColumnType::REAL ||
               plot_->xColumnType() == ColumnType::INTEGER) {
        okx = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
      }
      else if (plot_->xColumnType() == ColumnType::TIME) {
        x = plot_->modelReal(xModelInd, okx);
      }
      else {
        x = uniqueId(data, plot_->xColumn());
      }

      //---

      if      (plot_->yColumnType() == ColumnType::REAL ||
               plot_->yColumnType() == ColumnType::INTEGER) {
        oky = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);
      }
      else if (plot_->yColumnType() == ColumnType::TIME) {
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
        ModelIndex nameColumnInd(plot, data.row, plot_->nameColumn(), data.parent);

        bool ok;

        name = plot_->modelString(nameColumnInd, ok);
      }

      if (! name.length() && plot_->title())
        name = plot_->title()->textStr();

      if (! name.length() && plot_->xAxis())
        name = plot_->xAxis()->label().string();

      //---

      // get symbol type, size, font size and color
      Color color;

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        (void) plot_->colorColumnColor(data.row, data.parent, color);
      }

      //---

      Point p(x, y);

      plot->addNameValue(groupInd, name, p, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      auto *plot = const_cast<CQChartsScatterPlot *>(plot_);

      ModelIndex columnInd(plot, data.row, column, data.parent);

      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        auto *modelData = plot_->getModelData();

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
addNameValue(int groupInd, const QString &name, const Point &p, int row,
             const QModelIndex &xind, const Color &color)
{
  groupInds_.insert(groupInd);

  //---

  if (isGridCells()) {
    auto pi = groupNameGridData_.find(groupInd);

    if (pi == groupNameGridData_.end())
      pi = groupNameGridData_.insert(pi, GroupNameGridData::value_type(groupInd, NameGridData()));

    auto &nameGridData = (*pi).second;

    auto pn = nameGridData.find(name);

    if (pn == nameGridData.end())
      pn = nameGridData.insert(pn, NameGridData::value_type(name, gridData_));

    auto &cellPointData = (*pn).second;

    cellPointData.addPoint(p);

    gridData_.setMaxN(std::max(gridData_.maxN(), cellPointData.maxN()));
  }

  //---

  if (isHexCells()) {
    auto pi = groupNameHexData_.find(groupInd);

    if (pi == groupNameHexData_.end())
      pi = groupNameHexData_.insert(pi, GroupNameHexData::value_type(groupInd, NameHexData()));

    auto &nameHexData = (*pi).second;

    auto pn = nameHexData.find(name);

    if (pn == nameHexData.end()) {
      HexMap hexMap;

      const auto &viewBBox = this->viewBBox();

      hexMap.setRange(viewBBox.getXMin(), viewBBox.getYMin(),
                      viewBBox.getXMax(), viewBBox.getYMax());

      hexMap.setNum(gridData_.nx());

      pn = nameHexData.insert(pn, NameHexData::value_type(name, hexMap));
    }

    auto pv = windowToView(p);

    auto &hexMap = (*pn).second;

    HexMap::Point hp(pv.x, pv.y);

    hexMap.addPoint(hp);

    int hi, hj;

    hexMap.pointToPos(hp, hi, hj);

    hexMapMaxN_ = std::max(hexMapMaxN_, hexMap.numData(hi, hj));
  }

  //---

  if (isSymbols() || isDensityMap()) {
    auto pi = groupNameValues_.find(groupInd);

    if (pi == groupNameValues_.end())
      pi = groupNameValues_.insert(pi, GroupNameValues::value_type(groupInd, NameValues()));

    auto &nameValues = (*pi).second;

    auto pn = nameValues.find(name);

    if (pn == nameValues.end())
      pn = nameValues.insert(pn, NameValues::value_type(name, ValuesData()));

    auto &valuesData = (*pn).second;

    valuesData.xrange.add(p.x);
    valuesData.yrange.add(p.y);

    valuesData.values.emplace_back(p, row, xind, color);
  }
}

//---

CQChartsScatterPointObj *
CQChartsScatterPlot::
createPointObj(int groupInd, const BBox &rect, const Point &p, const ColorInd &is,
               const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsScatterPointObj(this, groupInd, rect, p, is, ig, iv);
}

CQChartsScatterCellObj *
CQChartsScatterPlot::
createCellObj(int groupInd, const BBox &rect, const ColorInd &is, const ColorInd &ig,
              int ix, int iy, const Points &points, int maxN) const
{
  return new CQChartsScatterCellObj(this, groupInd, rect, is, ig, ix, iy, points, maxN);
}

CQChartsScatterHexObj *
CQChartsScatterPlot::
createHexObj(int groupInd, const BBox &rect, const ColorInd &is, const ColorInd &ig,
             int ix, int iy, const Polygon &poly, int n, int maxN) const
{
  return new CQChartsScatterHexObj(this, groupInd, rect, is, ig, ix, iy, poly, n, maxN);
}

//---

void
CQChartsScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  if (isOverlay() && ! isFirstPlot())
    return;

  if      (isGridCells())
    addGridKeyItems(key);
  else if (isHexCells())
    addHexKeyItems(key);
  else
    addPointKeyItems(key);

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot::
addPointKeyItems(CQChartsPlotKey *key)
{
  // start at next row (verical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyItem = [&](int ind, const QString &name, int i, int n) {
    ColorInd ic(i, n);

    auto *colorItem = new CQChartsScatterKeyColor(this, ind , ic);
    auto *textItem  = new CQChartsKeyText        (this, name, ic);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    if (! key->isFlipped()) {
      groupItem->addItem(colorItem);
      groupItem->addItem(textItem );
    }
    else {
      groupItem->addItem(textItem );
      groupItem->addItem(colorItem);
    }

    if (! key->isHorizontal()) {
      //key->addItem(colorItem, row, col    );
      //key->addItem(textItem , row, col + 1);

      key->addItem(groupItem, row, col);

      // across columns and then next row
      ++col;

      if (col >= key->columns()) {
        col = 0;

        ++row;
      }
    }
    else {
      //key->addItem(colorItem, row, col++);
      //key->addItem(textItem , row, col++);

      key->addItem(groupItem, row, col);

      // across rows and then next column
      ++row;

      if (row >= key->columns()) {
        row = 0;

        ++col;
      }
    }

    return colorItem;
  };

  int ng = groupInds_.size();

  // multiple group - key item per group
  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      QString groupName = groupIndName(groupInd);

      auto *colorItem = addKeyItem(groupInd, groupName, ig, ng);

      //--

      if (colorColumn().isValid() && colorColumn().isGroup()) {
        if (isColorMapped()) {
          double r = CMathUtil::map(groupInd, colorColumnData_.data_min, colorColumnData_.data_max,
                                    colorMapMin(), colorMapMax());

          auto color = Color(Color::Type::PALETTE_VALUE, r);

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
    if (isSymbols()) {
      const auto &nameValues = (*groupNameValues_.begin()).second;

      int ns = nameValues.size();

      if (ns > 1) {
        int is = 0;

        for (const auto &nameValue : nameValues) {
          const auto &name = nameValue.first;

          auto *colorItem = addKeyItem(-1, name, is, ns);

          //--

          if (colorColumn().isValid()) {
            const auto &values = nameValue.second.values;

            int nv = values.size();

            if (nv > 0) {
              const auto &valuePoint = values[0];

              Color color;

              if (colorColumnColor(valuePoint.row, valuePoint.ind.parent(), color))
                colorItem->setColor(color);
            }
          }

          //--

          ++is;
        }
      }
      else {
        if (parentPlot() && ! nameValues.empty()) {
          const auto &name = nameValues.begin()->first;

          int ig = parentPlot()->childPlotIndex(this);
          int ng = parentPlot()->numChildPlots();

          (void) addKeyItem(-1, name, ig, ng);
        }
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

void
CQChartsScatterPlot::
addHexKeyItems(CQChartsPlotKey *key)
{
  auto *item = new CQChartsScatterHexKeyItem(this);

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
  (void) addMenuCheckedAction(typeMenu, "Hex Cells" , isHexCells (), SLOT(setHexCells(bool)));

  menu->addMenu(typeMenu);

  //---

  auto *overlaysMenu = new QMenu("Overlays", menu);

  (void) addMenuCheckedAction(overlaysMenu, "Best Fit",
                              isBestFit   (), SLOT(setBestFit       (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Hull",
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
  (void) addMenuCheckedAction(xMenu, "Whisker", isXWhisker(), SLOT(setXWhisker(bool)));
  (void) addMenuCheckedAction(xMenu, "Density", isXDensity(), SLOT(setXDensity(bool)));

  (void) addMenuCheckedAction(yMenu, "Rug"    , isYRug    (), SLOT(setYRug    (bool)));
  (void) addMenuCheckedAction(yMenu, "Whisker", isYWhisker(), SLOT(setYWhisker(bool)));
  (void) addMenuCheckedAction(yMenu, "Density", isYDensity(), SLOT(setYDensity(bool)));

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

  BBox bbox;

  if (isXRug() || isYRug() || isXDensity() || isYDensity() || isXWhisker() || isYWhisker()) {
    double dx = 0.0, dy = 0.0;

    // x rug axis
    if (isXRug()) {
      auto bbox1 = xRug_->calcBBox();

      dx += bbox1.getWidth();

      bbox += bbox1;
    }

    // y rug axis
    if (isYRug()) {
      auto bbox1 = yRug_->calcBBox();

      dy += bbox1.getHeight();

      bbox += bbox1;
    }

    //---

    // x density axis
    if (isXDensity()) {
      auto bbox1 = xAxisWhisker_->calcDeltaBBox(dx);

      dx += bbox1.getWidth();

      bbox += bbox1;
    }

    // y density axis
    if (isYDensity()) {
      auto bbox1 = yAxisWhisker_->calcDeltaBBox(dy);

      dy += bbox1.getHeight();

      bbox += bbox1;
    }

    //---

    // x whisker axis (one per group)
    if (isXWhisker()) {
      auto bbox1 = xAxisWhisker_->calcNDeltaBBox(groupInds_.size(), dx);

      dx += bbox1.getWidth();

      bbox += bbox1;
    }

    // y whisker axis (one per group)
    if (isYWhisker()) {
      auto bbox1 = yAxisWhisker_->calcNDeltaBBox(groupInds_.size(), dy);

      dy += bbox1.getHeight();

      bbox += bbox1;
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

  if (isXRug()) return true;
  if (isYRug()) return true;

  if (isXDensity()) return true;
  if (isXWhisker()) return true;

  if (isYDensity()) return true;
  if (isYWhisker()) return true;

  return false;
}

void
CQChartsScatterPlot::
execDrawBackground(PaintDevice *device) const
{
  CQChartsPlot::execDrawBackground(device);

  if (isHull      ()) drawHull      (device);
  if (isBestFit   ()) drawBestFit   (device);
  if (isStatsLines()) drawStatsLines(device);
  if (isDensityMap()) drawDensityMap(device);

  // drawn axis annotatons in inside->outside order
  xAxisSideBBox_.clear();
  yAxisSideBBox_.clear();

  if (isXRug()) drawXRug(device);
  if (isYRug()) drawYRug(device);

  if (isXWhisker()) drawXWhisker(device);
  if (isYWhisker()) drawYWhisker(device);

  if (isXDensity()) drawXDensity(device);
  if (isYDensity()) drawYDensity(device);
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
execDrawForeground(PaintDevice *device) const
{
  if (isSymbolMapKey())
    drawSymbolMapKey(device);
}

//---

void
CQChartsScatterPlot::
initGroupBestFit(int groupInd) const
{
  // init best fit data
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  auto &fitData = th->groupFitData_[groupInd];

  if (! fitData.isFitted()) {
    auto p = groupPoints_.find(groupInd);

    if (p != groupPoints_.end()) {
      const auto &points = (*p).second;

      if (! isBestFitOutliers()) {
        initGroupStats(groupInd);

        //---

        auto ps = groupStatData_.find(groupInd);
        assert(ps != groupStatData_.end());

        const auto &statData = (*ps).second;

        //---

        Polygon poly;

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

  auto &statData = th->groupStatData_[groupInd];

  if (! statData.xstat.set || ! statData.ystat.set) {
    auto p = groupPoints_.find(groupInd);

    if (p != groupPoints_.end()) {
      const auto &points = (*p).second;

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
drawBestFit(PaintDevice *device) const
{
  // init fit data
  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    initGroupBestFit(groupInd);
  }

  //---

  // draw fit data
  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    auto pf = groupFitData_.find(groupInd);
    assert(pf != groupFitData_.end());

    const auto &fitData = (*pf).second;

    //---

    ColorInd ic(ig, ng);

    CQChartsPointPlot::drawBestFit(device, fitData, ic);

    //---

    ++ig;
  }
}

void
CQChartsScatterPlot::
drawStatsLines(PaintDevice *device) const
{
  // init stats data
  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    initGroupStats(groupInd);
  }

  //---

  // draw stats data
  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    auto ps = groupStatData_.find(groupInd);
    assert(ps != groupStatData_.end());

    const auto &statData = (*ps).second;

    //---

    // calc pen and brush
    ColorInd ic(ig, ng);

    CQChartsPenBrush penBrush;

    QColor c = interpStatsLinesColor(ic);

    setPenBrush(penBrush,
      PenData  (true, c, statsLinesAlpha(), statsLinesWidth(), statsLinesDash()),
      BrushData(false));

    updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    auto drawXStatLine = [&](double x) {
      Point p1(x, statData.ystat.loutlier);
      Point p2(x, statData.ystat.uoutlier);

      device->drawLine(p1, p2);
    };

    auto drawYStatLine = [&](double y) {
      Point p1(statData.xstat.loutlier, y);
      Point p2(statData.xstat.uoutlier, y);

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
drawHull(PaintDevice *device) const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    //---

    // get hull for group (add if needed)
    auto ph = th->groupHull_.find(groupInd);

    if (ph == th->groupHull_.end()) {
      ph = th->groupHull_.insert(ph, GroupHull::value_type(groupInd, new CQChartsGrahamHull));

      //---

      auto *hull = (*ph).second;

      const auto &points = th->groupPoints_[groupInd];

      std::vector<double> x, y;

      for (const auto &p : points) {
        if (isInterrupt())
          return;

        hull->addPoint(p);
      }
    }

    const auto *hull = (*ph).second;

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
drawXRug(PaintDevice *device) const
{
  drawXYRug(device, xRug_);
}

void
CQChartsScatterPlot::
drawYRug(PaintDevice *device) const
{
  drawXYRug(device, yRug_);
}

void
CQChartsScatterPlot::
drawXYRug(PaintDevice *device, const RugP &rug) const
{
  rug->clearPoints();

  for (const auto &plotObj : plotObjects()) {
    if (isInterrupt())
      return;

    //---

    auto *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);

    if (pointObj) {
      CQChartsPenBrush penBrush;

      pointObj->calcPenBrush(penBrush, /*updateState*/false);

      if (rug->direction() == Qt::Horizontal)
        rug->addPoint(CQChartsAxisRug::RugPoint(pointObj->point().x, penBrush.pen.color()));
      else
        rug->addPoint(CQChartsAxisRug::RugPoint(pointObj->point().y, penBrush.pen.color()));
    }

    //---

    auto *cellObj = dynamic_cast<CQChartsScatterCellObj *>(plotObj);

    if (cellObj) {
      CQChartsPenBrush penBrush;

      cellObj->calcRugPenBrush(penBrush, /*updateState*/false);

      for (const auto &p : cellObj->points()) {
        if (rug->direction() == Qt::Horizontal)
          rug->addPoint(CQChartsAxisRug::RugPoint(p.x, penBrush.pen.color()));
        else
          rug->addPoint(CQChartsAxisRug::RugPoint(p.y, penBrush.pen.color()));
      }
    }
  }

  rug->draw(device);

  if (rug->direction() == Qt::Horizontal)
    xAxisSideBBox_[(XYSide) rug->side()] += rug->calcBBox();
  else
    yAxisSideBBox_[(XYSide) rug->side()] += rug->calcBBox();
}

//------

void
CQChartsScatterPlot::
drawXDensity(PaintDevice *device) const
{
  initWhiskerData();

  //---

  double delta = 0.0;

  auto bbox = xAxisSideBBox_[(XYSide) xAxisDensity_->side()];

  if (bbox.isSet())
    delta = bbox.getHeight();

  //---

  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    auto p = groupXWhiskers_.find(groupInd);

    if (p != groupXWhiskers_.end()) {
      auto *xWhiskerData = (*p).second;

      drawXYDensityWhisker(device, xWhiskerData, ColorInd(ig, ng), delta);
    }

    ++ig;
  }

  //---

  xAxisSideBBox_[(XYSide) xAxisDensity_->side()] += xAxisDensity_->calcDeltaBBox(delta);
}

void
CQChartsScatterPlot::
drawYDensity(PaintDevice *device) const
{
  initWhiskerData();

  //---

  double delta = 0.0;

  auto bbox = yAxisSideBBox_[(XYSide) yAxisDensity_->side()];

  if (bbox.isSet())
    delta = bbox.getWidth();

  //---

  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    auto p = groupYWhiskers_.find(groupInd);

    if (p != groupYWhiskers_.end()) {
      auto *yWhiskerData = (*p).second;

      drawXYDensityWhisker(device, yWhiskerData, ColorInd(ig, ng), delta);
    }

    ++ig;
  }

  //---

  yAxisSideBBox_[(XYSide) yAxisDensity_->side()] += yAxisDensity_->calcDeltaBBox(delta);
}

void
CQChartsScatterPlot::
drawXYDensityWhisker(PaintDevice *device, const AxisBoxWhisker *boxWhisker,
                     const ColorInd &ig, double delta) const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  if (boxWhisker->direction() == Qt::Horizontal)
    th->xAxisDensity_->setWhisker(boxWhisker->whisker());
  else
    th->yAxisDensity_->setWhisker(boxWhisker->whisker());

  // calc pen/brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(penBrush,
    PenData  (true, strokeColor, symbolStrokeAlpha()),
    BrushData(true, fillColor, symbolFillAlpha(), symbolFillPattern()));

  if (boxWhisker->direction() == Qt::Horizontal)
    xAxisDensity_->draw(device, penBrush, delta);
  else
    yAxisDensity_->draw(device, penBrush, delta);
}

//---

void
CQChartsScatterPlot::
drawXWhisker(PaintDevice *device) const
{
  initWhiskerData();

  //---

  double delta = 0.0;

  auto bbox = xAxisSideBBox_[(XYSide) xAxisWhisker_->side()];

  if (bbox.isSet())
    delta = bbox.getHeight();

  //---

  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    auto p = groupXWhiskers_.find(groupInd);

    if (p != groupXWhiskers_.end()) {
      auto *xWhiskerData = (*p).second;

      drawXYWhiskerWhisker(device, xWhiskerData, ColorInd(ig, ng), delta);
    }

    ++ig;
  }

  xAxisSideBBox_[(XYSide) xAxisWhisker_->side()] += xAxisWhisker_->calcNDeltaBBox(ng, delta);
}

void
CQChartsScatterPlot::
drawYWhisker(PaintDevice *device) const
{
  initWhiskerData();

  //---

  double delta = 0.0;

  auto bbox = yAxisSideBBox_[(XYSide) yAxisWhisker_->side()];

  if (bbox.isSet())
    delta = bbox.getWidth();

  //---

  int ig = 0;
  int ng = groupInds_.size();

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    auto p = groupYWhiskers_.find(groupInd);

    if (p != groupYWhiskers_.end()) {
      auto *yWhiskerData = (*p).second;

      drawXYWhiskerWhisker(device, yWhiskerData, ColorInd(ig, ng), delta);
    }

    ++ig;
  }

  yAxisSideBBox_[(XYSide) yAxisWhisker_->side()] += yAxisWhisker_->calcNDeltaBBox(ng, delta);
}

void
CQChartsScatterPlot::
drawXYWhiskerWhisker(PaintDevice *device, const AxisBoxWhisker *boxWhisker,
                     const ColorInd &ig, double delta) const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  if (boxWhisker->direction() == Qt::Horizontal)
    th->xAxisWhisker_->setWhisker(boxWhisker->whisker());
  else
    th->yAxisWhisker_->setWhisker(boxWhisker->whisker());

  // calc pen/brush
  CQChartsPenBrush penBrush;

  QColor strokeColor = interpSymbolStrokeColor(ig);
  QColor fillColor   = interpSymbolFillColor  (ig);

  setPenBrush(penBrush,
    PenData  (true, strokeColor, symbolStrokeAlpha()),
    BrushData(true, fillColor, symbolFillAlpha(), symbolFillPattern()));

  if (boxWhisker->direction() == Qt::Horizontal)
    xAxisWhisker_->draw(device, penBrush, ig.i, delta);
  else
    yAxisWhisker_->draw(device, penBrush, ig.i, delta);
}

//------

void
CQChartsScatterPlot::
drawDensityMap(PaintDevice *device) const
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

  // density only if symbols ...
  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      return;

    const auto &nameValues = groupNameValue.second;

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

      const auto &values = nameValue.second;

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

//------

void
CQChartsScatterPlot::
initWhiskerData() const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    //---

    // get group whiskers
    auto xpw = th->groupXWhiskers_.find(groupInd);

    if (xpw == th->groupXWhiskers_.end())
      xpw = th->groupXWhiskers_.insert(xpw,
              GroupWhiskers::value_type(groupInd, new AxisBoxWhisker(th, Qt::Horizontal)));

    auto *xWhiskerData = (*xpw).second;

    auto ypw = th->groupYWhiskers_.find(groupInd);

    if (ypw == th->groupYWhiskers_.end())
      ypw = th->groupYWhiskers_.insert(ypw,
              GroupWhiskers::value_type(groupInd, new AxisBoxWhisker(th, Qt::Vertical)));

    auto *yWhiskerData = (*ypw).second;

    //---

    // init whisker if needed
    if (! xWhiskerData->numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const auto *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          auto *whiskerData1 = const_cast<AxisBoxWhisker *>(xWhiskerData);

          whiskerData1->addValue(pointObj->point().x);
        }
      }
    }

    if (! yWhiskerData->numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const auto *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);

        if (pointObj && pointObj->groupInd() == groupInd) {
          auto *whiskerData1 = const_cast<AxisBoxWhisker *>(yWhiskerData);

          whiskerData1->addValue(pointObj->point().y);
        }
      }
    }
  }

  //---

  for (const auto &groupInd : groupInds_) {
    if (isInterrupt())
      return;

    //---

    // get group whiskers
    auto xpw = th->groupXWhiskers_.find(groupInd);

    if (xpw == th->groupXWhiskers_.end())
      xpw = th->groupXWhiskers_.insert(xpw,
              GroupWhiskers::value_type(groupInd, new AxisBoxWhisker(th, Qt::Horizontal)));

    auto *xWhiskerData = (*xpw).second;

    auto ypw = th->groupXWhiskers_.find(groupInd);

    if (ypw == th->groupYWhiskers_.end())
      ypw = th->groupYWhiskers_.insert(ypw,
              GroupWhiskers::value_type(groupInd, new AxisBoxWhisker(th, Qt::Vertical)));

    auto *yWhiskerData = (*ypw).second;

    //---

    // init whisker if needed
    if (! xWhiskerData->numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const auto *cellObj = dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            auto *whiskerData1 = const_cast<AxisBoxWhisker *>(xWhiskerData);

            whiskerData1->addValue(p.x);
          }
        }
      }
    }

    if (! yWhiskerData->numValues()) {
      for (const auto &plotObj : plotObjects()) {
        if (isInterrupt())
          return;

        const auto *cellObj = dynamic_cast<CQChartsScatterCellObj *>(plotObj);

        if (cellObj && cellObj->groupInd() == groupInd) {
          for (const auto &p : cellObj->points()) {
            if (isInterrupt())
              return;

            auto *whiskerData1 = const_cast<AxisBoxWhisker *>(yWhiskerData);

            whiskerData1->addValue(p.y);
          }
        }
      }
    }
  }
}

//------

void
CQChartsScatterPlot::
drawSymbolMapKey(PaintDevice *device) const
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

  //auto p = view()->windowToPixel(Point(vx, vy));

  double px = pbbox.getXMax();
  double py = pbbox.getYMax();

  double pm = symbolMapKeyMargin();

  double pr1 = symbolSizeData_.map_max;
  double pr3 = symbolSizeData_.map_min;

  double pr2 = (pr1 + pr3)/2;

  //---

  QColor strokeColor = interpThemeColor(ColorInd(1.0));

  double xm = px - pr1 - pm;
  double ym = py - pm;

  BBox pbbox1(xm - pr1, ym - 2*pr1, xm + pr1, ym);
  BBox pbbox2(xm - pr2, ym - 2*pr2, xm + pr2, ym);
  BBox pbbox3(xm - pr3, ym - 2*pr3, xm + pr3, ym);

  auto a = symbolMapKeyAlpha();

  QColor fillColor1 = interpSymbolFillColor(ColorInd(1.0)); fillColor1.setAlphaF(a.value());
  QColor fillColor2 = interpSymbolFillColor(ColorInd(0.5)); fillColor2.setAlphaF(a.value());
  QColor fillColor3 = interpSymbolFillColor(ColorInd(0.0)); fillColor3.setAlphaF(a.value());

  auto drawEllipse = [&](const QColor &c, const BBox &pbbox) {
    CQChartsPenBrush penBrush;

    setPenBrush(penBrush, PenData(true, strokeColor), BrushData(true, c));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawEllipse(pixelToWindow(pbbox));
  };

  drawEllipse(fillColor1, pbbox1);
  drawEllipse(fillColor2, pbbox2);
  drawEllipse(fillColor3, pbbox3);

  //---

  auto drawText = [&](const Point &p, double value) {
    QString text = QString("%1").arg(value);

    QFontMetricsF fm(device->font());

    Point p1(p.x - fm.width(text)/2, p.y);

    auto p2 = pixelToWindow(p1);

    CQChartsTextOptions options;

    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p2, text, options);
  };

  drawText(Point(pbbox1.getXMid(), pbbox1.getYMin()), max );
  drawText(Point(pbbox2.getXMid(), pbbox2.getYMin()), mean);
  drawText(Point(pbbox3.getXMid(), pbbox3.getYMin()), min );
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(const Plot *plot, int groupInd, const BBox &rect, const Point &pos,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ig, iv), plot_(plot),
 groupInd_(groupInd), pos_(pos)
{
  setDetailHint(DetailHint::MAJOR);
}

//---

CQChartsSymbol
CQChartsScatterPointObj::
symbolType() const
{
  auto symbolType = extraData().symbolType;

  if (! symbolType.isValid())
    symbolType = plot_->symbolType();

  return symbolType;
}

CQChartsLength
CQChartsScatterPointObj::
symbolSize() const
{
  auto symbolSize = extraData().symbolSize;

  if (! symbolSize.isValid())
    symbolSize = plot()->symbolSize();

  return symbolSize;
}

CQChartsLength
CQChartsScatterPointObj::
fontSize() const
{
  auto fontSize = extraData().fontSize;

  if (! fontSize.isValid()) {
    double dataLabelFontSize = plot()->dataLabel()->textFont().pointSizeF();

    fontSize = Length(dataLabelFontSize, CQChartsUnits::PIXEL);
  }

  return fontSize;
}

CQChartsColor
CQChartsScatterPointObj::
color() const
{
  auto color = extraData().color;

  return color;
}

CQChartsFont
CQChartsScatterPointObj::
font() const
{
  auto font = extraData().font;

  return font;
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

  plot()->addNoTipColumns(tableTip);

  //---

  // add name (label or name column) as header
  if (nameColumn().isValid() && name().length()) {
    if (! tableTip.hasColumn(nameColumn())) {
      tableTip.addBoldLine(name());

      tableTip.addColumn(nameColumn());
    }
  }

  //---

  // TODO: id column

  //---

  // add group column (TODO: check group column)
  if (ig_.n > 1) {
    QString groupName = plot_->groupIndName(groupInd_);

    tableTip.addTableRow("Group", groupName);
  }

  //---

  // add x, y columns
  if (! tableTip.hasColumn(plot()->xColumn())) {
    QString xstr;

    if (plot()->isUniqueX()) {
      auto *columnDetails = plot()->columnDetails(plot()->xColumn());

      xstr = (columnDetails ? columnDetails->uniqueValue(int(pos_.x)).toString() :
                              plot()->xStr(pos_.x));
    }
    else
      xstr = plot()->xStr(pos_.x);

    tableTip.addTableRow(plot_->xHeaderName(/*tip*/true), xstr);

    tableTip.addColumn(plot()->xColumn());
  }

  if (! tableTip.hasColumn(plot()->yColumn())) {
    QString ystr;

    if (plot()->isUniqueY()) {
      auto *columnDetails = plot()->columnDetails(plot()->yColumn());

      ystr = (columnDetails ? columnDetails->uniqueValue(int(pos_.y)).toString() :
                              plot()->yStr(pos_.y));
    }
    else
      ystr = plot()->yStr(pos_.y);

    tableTip.addTableRow(plot_->yHeaderName(/*tip*/true), ystr);

    tableTip.addColumn(plot()->yColumn());
  }

  //---

  auto addColumnRowValue = [&](const CQChartsColumn &column) {
    if (! column.isValid()) return;

    if (tableTip.hasColumn(column))
      return;

    ModelIndex columnInd(const_cast<CQChartsScatterPlot *>(plot_),
                         modelInd().row(), column, modelInd().parent());

    bool ok;

    QString str = plot_->modelString(columnInd, ok);
    if (! ok) return;

    tableTip.addTableRow(plot_->columnHeaderName(column, /*tip*/true), str);

    tableTip.addColumn(column);
  };

  //---

  // add symbol type, symbol size and font size columns
  addColumnRowValue(plot_->symbolTypeColumn());
  addColumnRowValue(plot_->symbolSizeColumn());
  addColumnRowValue(plot_->fontSizeColumn  ());

  //---

  // get values for name (grouped id identical names)
  CQChartsScatterPlot::ValueData valuePoint;

  auto pg = plot_->groupNameValues().find(groupInd_);
  assert(pg != plot_->groupNameValues().end());

  auto p = (*pg).second.find(name());

  if (p != (*pg).second.end()) {
    const auto &values = (*p).second.values;

    valuePoint = values[iv_.i];
  }

  //---

  // add color column
  if (valuePoint.color.isValid())
    tableTip.addTableRow(plot_->colorHeaderName(/*tip*/true), valuePoint.color.colorStr());
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
inside(const Point &p) const
{
  double sx, sy;

  plot_->pixelSymbolSize(this->symbolSize(), sx, sy);

  auto p1 = plot_->windowToPixel(pos_);

  BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
getObjSelectIndices(Indices &inds) const
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
draw(PaintDevice *device)
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
  auto symbolType = this->symbolType();
  auto symbolSize = this->symbolSize();

  double sx, sy;

  plot()->pixelSymbolSize(symbolSize, sx, sy);

  //---

  // get point and image
  auto ps    = plot_->windowToPixel(pos_);
  auto image = this->image();

  //---

  // draw symbol or image (image only for point)
  if (! image.isValid()) {
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

    BBox ibbox(ps.x - sx, ps.y - sy, ps.x + 2*sx, ps.y + 2*sy);

    device->drawImageInRect(plot()->pixelToWindow(ibbox), image);
  }

  device->resetColorNames();

  //---

  // draw text labels
  if (plot_->dataLabel()->isVisible()) {
    drawDataLabel(device);
  }
}

void
CQChartsScatterPointObj::
drawDataLabel(PaintDevice *device) const
{
  const auto *dataLabel = plot_->dataLabel();

  auto ps = plot_->windowToPixel(pos_);

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize(), sx, sy);

  //---

  // text font color
  CQChartsPenBrush penBrush;

  QColor tc = dataLabel->interpTextColor(calcColorInd());

  plot_->setPenBrush(penBrush,
    PenData(true, tc, dataLabel->textAlpha()), BrushData(false));

  //---

  // get font
  auto font     = this->font();
  auto fontSize = this->fontSize();

  if (! font.isValid()) {
    font = dataLabel->textFont();

    if (fontSize.isValid()) {
      double fontPixelSize = plot_->lengthPixelHeight(fontSize);

      // scale to font size
      fontPixelSize = plot_->limitFontSize(fontPixelSize);

      font.setPointSizeF(fontPixelSize);
    }
  }

  //---

  // set (temp) font
  const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font);

  //---

  // draw text
  BBox ptbbox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

  dataLabel->draw(device, plot_->pixelToWindow(ptbbox), name(), dataLabel->position(), penBrush);

  //---

  // reset font
  if (fontSize.isValid()) {
    const_cast<CQChartsScatterPlot *>(plot_)->setDataLabelFont(font);
  }
}

void
CQChartsScatterPointObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd ic;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    // default for scatter is set or group color (not value color !!)
    if      (is_.n > 1)
      ic = is_;
    else if (ig_.n > 1)
      ic = ig_;
  }
  else
    ic = calcColorInd();

  //--

  plot_->setSymbolPenBrush(penBrush, ic);

  // override symbol fill color for custom color
  auto color = this->color();

  if (color.isValid()) {
    QColor c = plot_->interpColor(color, ic);

    c.setAlphaF(plot_->symbolFillAlpha().value());

    penBrush.brush.setColor(c);
  }

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
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
CQChartsScatterCellObj(const Plot *plot, int groupInd, const BBox &rect, const ColorInd &is,
                       const ColorInd &ig, int ix, int iy, const Points &points, int maxn) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
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
inside(const Point &p) const
{
  return CQChartsPlotObj::inside(p);
}

void
CQChartsScatterCellObj::
draw(PaintDevice *device)
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
    PenData  (plot_->isGridCellStroked(), pc, plot_->gridCellStrokeAlpha(),
              plot_->gridCellStrokeWidth(), plot_->gridCellStrokeDash()),
    BrushData(plot_->isGridCellFilled(), fc, plot_->gridCellFillAlpha(),
              plot_->gridCellFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsScatterCellObj::
calcRugPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  ColorInd ic = (ig_.n > 1 ? ig_ : is_);

  plot_->setSymbolPenBrush(penBrush, ic);

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
}

void
CQChartsScatterCellObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.count = " << points_.size() << ";\n";
}

//------

CQChartsScatterHexObj::
CQChartsScatterHexObj(const CQChartsScatterPlot *plot, int groupInd, const BBox &rect,
                      const ColorInd &is, const ColorInd &ig, int ix, int iy,
                      const Polygon &poly, int n, int maxN) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), ix_(ix), iy_(iy), poly_(poly), n_(n), maxN_(maxN)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsScatterHexObj::
calcId() const
{
  return QString("%1:%2:%3:%4:%5").arg(typeName()).arg(is_.i).arg(ig_.i).arg(ix_).arg(iy_);
}

QString
CQChartsScatterHexObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Count", n_);

  return tableTip.str();
}

bool
CQChartsScatterHexObj::
inside(const Point &p) const
{
  return CQChartsPlotObj::inside(p);
}

void
CQChartsScatterHexObj::
draw(PaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw polygon
  device->drawPolygon(poly_);

  //---

  device->resetColorNames();
}

void
CQChartsScatterHexObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set pen and brush
  ColorInd ic(n_, maxN_);

  QColor pc = plot_->interpGridCellStrokeColor(ColorInd());
  QColor fc = plot_->interpPaletteColor(ic);

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isGridCellStroked(), pc, plot_->gridCellStrokeAlpha(),
              plot_->gridCellStrokeWidth(), plot_->gridCellStrokeDash()),
    BrushData(plot_->isGridCellFilled(), fc, plot_->gridCellFillAlpha(),
              plot_->gridCellFillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsScatterHexObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.count = " << n_ << ";\n";
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int groupInd, const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ic), groupInd_(groupInd)
{
}

bool
CQChartsScatterKeyColor::
selectPress(const Point &, CQChartsSelMod selMod)
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
 CQChartsGradientKeyItem(plot), plot_(plot)
{
}

int
CQChartsScatterGridKeyItem::
maxN() const
{
  return plot_->gridData().maxN();
}

//---

CQChartsScatterHexKeyItem::
CQChartsScatterHexKeyItem(CQChartsScatterPlot *plot) :
 CQChartsGradientKeyItem(plot), plot_(plot)
{
}

int
CQChartsScatterHexKeyItem::
maxN() const
{
  return plot_->hexMapMaxN();
}
