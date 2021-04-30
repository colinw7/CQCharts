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
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsBivariateDensity.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsFontSizeRangeSlider.h>
#include <CQChartsColumnControlGroup.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsMapKey.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQThreadObject.h>
#include <CQPerfMonitor.h>
#include <CQTabSplit.h>
#include <CQGroupBox.h>
//#include <CQCheckBox.h>
#include <CQEnumCombo.h>

#include <QMenu>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>

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
    setRequired().setNumeric().setPropPath("columns.x").setTip("X Value Column");
  addColumnParameter("y", "Y", "yColumn").
    setRequired().setNumeric().setPropPath("columns.y").setTip("Y Value Column");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setPropPath("columns.name").setTip("Group Name Column").setString();
  addColumnParameter("label", "Label", "labelColumn").
    setPropPath("columns.label").setTip("Point Label Column").setString();

  //--

  // options
  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("NONE"      , int(CQChartsScatterPlot::PlotType::NONE      )).
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
}

CQChartsScatterPlot::
~CQChartsScatterPlot()
{
  term();
}

//---

void
CQChartsScatterPlot::
init()
{
  CQChartsPointPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  setLayerActive(CQChartsLayer::Type::BG_PLOT, true);
  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  //---

  setSymbol(Symbol::circle());
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(Color(Color::Type::PALETTE));

  setGridCellFilled (true);
  setGridCellStroked(true);
  setGridCellStrokeColor(Color(Color::Type::INTERFACE_VALUE, 0.1));

  setDataClip(false);

  setDataLabelPosition(CQChartsLabelPosition::TOP_OUTSIDE);

  //---

  addAxes();

  addKey();

  addTitle();

  //---

  auto createAxisDensity = [&](Qt::Orientation orient) {
    auto *density = new AxisDensity(this, orient);

    density->setVisible(false);
    density->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT));

    return density;
  };

  xAxisDensity_ = createAxisDensity(Qt::Horizontal);
  yAxisDensity_ = createAxisDensity(Qt::Vertical);

  //---

  auto createAxisWhisker = [&](Qt::Orientation orient) {
    auto *whisker = new AxisBoxWhisker(this, orient);

    whisker->setVisible(false);
    whisker->setSide(CQChartsAxisSide(CQChartsAxisSide::Type::TOP_RIGHT));

    return whisker;
  };

  xAxisWhisker_ = createAxisWhisker(Qt::Horizontal);
  yAxisWhisker_ = createAxisWhisker(Qt::Vertical);

  //---

  addColorMapKey();

  addSymbolSizeMapKey();
  addSymbolTypeMapKey();
}

void
CQChartsScatterPlot::
term()
{
  clearDensityData();

  for (const auto &groupWhisker_ : groupXWhiskers_)
    delete groupWhisker_.second;

  for (const auto &groupWhisker_ : groupYWhiskers_)
    delete groupWhisker_.second;

  delete xAxisDensity_;
  delete yAxisDensity_;

  delete xAxisWhisker_;
  delete yAxisWhisker_;
}

//---

void
CQChartsScatterPlot::
clearDensityData()
{
  for (const auto &pn : groupNamedDensity_)
    for (const auto &pd : pn.second)
      delete pd.second;

  groupNamedDensity_.clear();
}

//---

void
CQChartsScatterPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsScatterPlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

void
CQChartsScatterPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

void
CQChartsScatterPlot::
setYColumn(const Column &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged(); } );
}

//---

CQChartsColumn
CQChartsScatterPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "x"   ) c = this->xColumn();
  else if (name == "y"   ) c = this->yColumn();
  else if (name == "name") c = this->nameColumn();
  else                     c = CQChartsPointPlot::getNamedColumn(name);

  return c;
}

void
CQChartsScatterPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "x"   ) this->setXColumn(c);
  else if (name == "y"   ) this->setYColumn(c);
  else if (name == "name") this->setNameColumn(c);
  else                     CQChartsPointPlot::setNamedColumn(name, c);
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
setNoType(bool b)
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      auto *splot = qobject_cast<CQChartsScatterPlot *>(plot);

      if (splot)
        splot->plotType_ = (b ? PlotType::NONE : PlotType::SYMBOLS);
    });

    updateRangeAndObjs();
  }
  else {
    CQChartsUtil::testAndSet(plotType_,
     (b ? PlotType::NONE : PlotType::SYMBOLS), [&]() { updateRangeAndObjs(); } );
  }
}

void
CQChartsScatterPlot::
setSymbols(bool b)
{
  if (isOverlay()) {
    processOverlayPlots([&](CQChartsPlot *plot) {
      auto *splot = qobject_cast<CQChartsScatterPlot *>(plot);

      if (splot)
        splot->plotType_ = (b ? PlotType::SYMBOLS : PlotType::NONE);
    });

    updateRangeAndObjs();
  }
  else {
    CQChartsUtil::testAndSet(plotType_,
     (b ? PlotType::SYMBOLS : PlotType::NONE), [&]() { updateRangeAndObjs(); } );
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
     (b ? PlotType::GRID_CELLS : PlotType::NONE), [&]() { updateRangeAndObjs(); } );
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
     (b ? PlotType::HEX_CELLS : PlotType::NONE), [&]() { updateRangeAndObjs(); } );
  }
}

//------

void
CQChartsScatterPlot::
setDensityMap(bool b)
{
  CQChartsUtil::testAndSet(densityMapData_.visible, b, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapGridSize(int s)
{
  CQChartsUtil::testAndSet(densityMapData_.gridSize, s, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapDelta(double d)
{
  CQChartsUtil::testAndSet(densityMapData_.delta, d, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setDensityMapLayer(const DrawLayer &l)
{
  CQChartsUtil::testAndSet(densityMapData_.layer, l, [&]() {
    for (const auto &plotObj : plotObjs_) {
      auto *densityObj = dynamic_cast<CQChartsScatterDensityObj *>(plotObj);

      if (densityObj)
        densityObj->setDrawLayer((CQChartsPlotObj::DrawLayer) densityMapLayer());
    }

    drawObjs();
  } );
}

//------

bool
CQChartsScatterPlot::
isXDensity() const
{
  return (xAxisDensity_ && xAxisDensity_->isVisible());
}

void
CQChartsScatterPlot::
setXDensity(bool b)
{
  if (b != isXDensity()) { xAxisDensity_->setVisible(b); resetExtraBBox(); drawObjs(); }
}

bool
CQChartsScatterPlot::
isYDensity() const
{
  return (yAxisDensity_ && yAxisDensity_->isVisible());
}

void
CQChartsScatterPlot::
setYDensity(bool b)
{
  if (b != isYDensity()) { yAxisDensity_->setVisible(b); resetExtraBBox(); drawObjs(); }
}

//------

bool
CQChartsScatterPlot::
isXWhisker() const
{
  return (xAxisWhisker_ && xAxisWhisker_->isVisible());
}

void
CQChartsScatterPlot::
setXWhisker(bool b)
{
  if (b != isXWhisker()) { xAxisWhisker_->setVisible(b); resetExtraBBox(); drawObjs(); }
}

bool
CQChartsScatterPlot::
isYWhisker() const
{
  return (yAxisWhisker_ && yAxisWhisker_->isVisible());
}

void
CQChartsScatterPlot::
setYWhisker(bool b)
{
  if (b != isYWhisker()) { yAxisWhisker_->setVisible(b); resetExtraBBox(); drawObjs(); }
}

//------

void
CQChartsScatterPlot::
addProperties()
{
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
  addBestFitProperties(/*hasLayer*/true);

  // convex hull shape
  addHullProperties(/*hasLayer*/true);

  // stats
  addStatsProperties();

  //---

  // density map
  auto densityMapPropPath = QString("overlays/densityMap");

  addProp(densityMapPropPath, "densityMap"        , "visible" , "Show density map overlay");
  addProp(densityMapPropPath, "densityMapGridSize", "gridSize", "Density map grid size");
  addProp(densityMapPropPath, "densityMapDelta"   , "delta"   , "Density map delta");
  addProp(densityMapPropPath, "densityMapLayer"   , "layer"   , "Density map draw layer");

  //---

  auto axisAnnotationsPath = QString("axisAnnotations");

  // rug axis
  addRugProperties(axisAnnotationsPath);

  // x/y density axis
  xAxisDensity_->addProperties(axisAnnotationsPath + "/density/x", "X axis density");
  yAxisDensity_->addProperties(axisAnnotationsPath + "/density/y", "Y axis density");

  // x/y whisker axis
  xAxisWhisker_->addProperties(axisAnnotationsPath + "/whisker/x", "X axis whisker");
  yAxisWhisker_->addProperties(axisAnnotationsPath + "/whisker/y", "Y axis whisker");

  //---

  // symbol
  addSymbolProperties("points", "", "");

  // data labels
  dataLabel()->addPathProperties("points/labels", "Labels");

  //---

  // grid
  addProp("gridCells", "gridNumX", "nx", "Number of x grid cells");
  addProp("gridCells", "gridNumY", "ny", "Number of y grid cells");

  addStyleProp     ("gridCells/fill"  , "gridCellFilled" , "visible", "Grid cell fill visible");
  addFillProperties("gridCells/fill"  , "gridCellFill"   , "Grid cell");
  addStyleProp     ("gridCells/stroke", "gridCellStroked", "visible", "Grid cell stroke visible");
  addLineProperties("gridCells/stroke", "gridCellStroke" , "Grid cell");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();

  //---

  // symbol size key
  addSymbolSizeMapKeyProperties();

  //---

  // symbol type key
  addSymbolTypeMapKeyProperties();

  //---

  CQChartsPointPlot::addProperties();

  //---

  CQChartsPointPlot::addPointProperties();
}

//---

QColor
CQChartsScatterPlot::
interpColor(const Color &c, const ColorInd &ind) const
{
  if (c.type() != Color::Type::PALETTE)
    return CQChartsPlot::interpColor(c, ind);

  if (isOverlay()) {
    int i = ind.i;
    int n = ind.n;

    if (prevPlot() || nextPlot()) {
      auto *plot1 = prevPlot();
      auto *plot2 = nextPlot();

      while (plot1) { ++n; plot1 = plot1->prevPlot(); }
      while (plot2) { ++n; plot2 = plot2->nextPlot(); }
    }

    //---

    auto *plot1 = prevPlot();

    while (plot1) {
      ++i;

      plot1 = plot1->prevPlot();
    }

    return view()->interpPaletteColor(ColorInd(i, n), c.isScale(), c.isInvert());
  }
  else {
    return view()->interpPaletteColor(ind, c.isScale(), c.isInvert());
  }
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

  initGroupData(Columns(), Column());

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

      ModelIndex xModelInd(plot_, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot_, data.row, plot_->yColumn(), data.parent);

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

    int uniqueId(const VisitData &data, const Column &column) {
      ModelIndex columnInd(plot_, data.row, column, data.parent);

      bool ok;

      auto var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const Column &column) {
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

  Range dataRange;

  if (xmin().isSet() && ymin().isSet() && xmax().isSet() && ymax().isSet()) {
     dataRange = Range(xmin().real(), ymin().real(), xmax().real(), ymax().real());
  }
  else {
    RowVisitor visitor(this);

    visitModel(visitor);

    dataRange = visitor.range();

    th->uniqueX_ = visitor.isUniqueX();
    th->uniqueY_ = visitor.isUniqueY();
  }

  if (isInterrupt())
    return dataRange;

  //---

  if (dataRange.isSet()) {
    if (isUniqueX() || isUniqueY()) {
      if (isUniqueX()) {
        dataRange.updateRange(dataRange.xmin() - 0.5, dataRange.ymin());
        dataRange.updateRange(dataRange.xmax() + 0.5, dataRange.ymin());
      }

      if (isUniqueY()) {
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

  return dataRange;
}

void
CQChartsScatterPlot::
postCalcRange()
{
  initAxes();
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

  // set x axis column
  xAxis()->setColumn(xColumn());

  //---

  // set x axis name and type
  QString xname;

  (void) xAxisName(xname, "X");

  if (isOverlay()) {
    if (isFirstPlot() || isX1X2())
       xAxis()->setDefLabel(xname, /*notify*/false);
  }
  else {
    xAxis()->setDefLabel(xname, /*notify*/false);
  }

  //--

  auto xType = xAxis()->valueType().type();

  if (xType != CQChartsAxisValueType::Type::INTEGER && xType != CQChartsAxisValueType::Type::REAL)
    xType = CQChartsAxisValueType::Type::REAL;

  if (isLogX   ()) xType = CQChartsAxisValueType::Type::LOG;
  if (isUniqueX()) xType = CQChartsAxisValueType::Type::INTEGER;

  if (xColumnType_ == ColumnType::TIME)
    xType = CQChartsAxisValueType::Type::DATE;

  xAxis()->setValueType(CQChartsAxisValueType(xType), /*notify*/false);

  //---

  // set y axis column
  yAxis()->setColumn(yColumn());

  //---

  // set y axis name and type
  QString yname;

  (void) yAxisName(yname, "Y");

  if (isOverlay()) {
    if (isY1Y2()) {
      yAxis()->setDefLabel(yname, /*notify*/false);
    }
  }
  else {
    yAxis()->setDefLabel(yname, /*notify*/false);
  }

  //--

  auto yType = yAxis()->valueType().type();

  if (yType != CQChartsAxisValueType::Type::INTEGER && yType != CQChartsAxisValueType::Type::REAL)
    yType = CQChartsAxisValueType::Type::REAL;

  if (isLogY   ()) yType = CQChartsAxisValueType::Type::LOG;
  if (isUniqueY()) yType = CQChartsAxisValueType::Type::INTEGER;

  if (yColumnType_ == ColumnType::TIME)
    yType = CQChartsAxisValueType::Type::DATE;

  yAxis()->setValueType(CQChartsAxisValueType(yType), /*notify*/false);

  //---

  if (isOverlay() && isFirstPlot())
    setOverlayPlotsAxisNames();

  //---

  xAxis()->clearTickLabels();

  if (isUniqueX()) {
    auto *columnDetails = this->columnDetails(xColumn());

    for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i)
      xAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());
  }

  //---

  yAxis()->clearTickLabels();

  if (isUniqueY()) {
    auto *columnDetails = this->columnDetails(yColumn());

    for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i)
      yAxis()->setTickLabel(i, columnDetails->uniqueValue(i).toString());
  }
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

  // init name values
  th->gridData_.setMaxN(0);

  th->hexMap_.clear();
  th->hexMapMaxN_ = 0;

  if (groupInds_.empty())
    addNameValues();

  th->groupPoints_  .clear();
  th->groupStatData_.clear();

  //---

  th->clearFitData ();
  th->clearHullData();

  //---

  th->clearDensityData();

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

  if (isSymbols())
    addPointObjects(objs);

  if (isGridCells())
    addGridObjects(objs);

  if (isHexCells())
    addHexObjects(objs);

  if (isBestFit())
    addBestFitObjects(objs);

  if (isHull())
    addHullObjects(objs);

  if (isDensityMap())
    addDensityObjects(objs);

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

//------

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

        if (valuePoint.ind.isValid())
          pointObj->setModelInd(valuePoint.ind);

        if (symbolSize.isValid())
          pointObj->setSymbolSize(symbolSize);

        objs.push_back(pointObj);

        points.push_back(p);

        //---

        // set optional symbol
        Symbol symbol;

        if (symbolTypeColumn().isValid()) {
          if (! columnSymbolType(valuePoint.row, valuePoint.ind.parent(), symbol))
            symbol = Symbol();
        }

        if (symbol.isValid())
          pointObj->setSymbol(symbol);

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
        Color symbolColor;

        if (colorColumn().isValid()) {
          if (! colorColumnColor(valuePoint.row, valuePoint.ind.parent(), symbolColor))
            symbolColor = Color();
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional symbol fill alpha
        Alpha symbolAlpha;

        if (alphaColumn().isValid()) {
          if (! alphaColumnAlpha(valuePoint.row, valuePoint.ind.parent(), symbolAlpha))
            symbolAlpha = Alpha();
        }

        if (symbolAlpha.isValid())
          pointObj->setAlpha(symbolAlpha);

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

          auto imageVar = modelValue(imageModelInd, ok);

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
addBestFitObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  auto bbox = calcDataRange(/*adjust*/false);

  // one best fit per group (multiple groups) or set (name values)
  int ng = groupInds_.size();

  if (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      bool hidden = (hasGroups && isSetHidden(groupInd));
      if (hidden) continue;

      auto *bestFitObj = createBestFitObj(groupInd, "", ColorInd(ig, ng), ColorInd(), bbox);

      bestFitObj->setDrawLayer((CQChartsPlotObj::DrawLayer) bestFitLayer());

      objs.push_back(bestFitObj);

      ++ig;
    }
  }
  else {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      auto *bestFitObj = createBestFitObj(-1, nameValue.first, ColorInd(), ColorInd(is, ns), bbox);

      bestFitObj->setDrawLayer((CQChartsPlotObj::DrawLayer) bestFitLayer());

      objs.push_back(bestFitObj);

      ++is;
    }
  }
}

void
CQChartsScatterPlot::
addHullObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  auto bbox = calcDataRange(/*adjust*/false);

  // one hull per group (multiple groups) or set (name values)
  int ng = groupInds_.size();

  if (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      bool hidden = (hasGroups && isSetHidden(groupInd));
      if (hidden) continue;

      auto *hullObj = createHullObj(groupInd, "", ColorInd(ig, ng), ColorInd(), bbox);

      hullObj->setDrawLayer((CQChartsPlotObj::DrawLayer) hullLayer());

      objs.push_back(hullObj);

      ++ig;
    }
  }
  else {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      auto &name = nameValue.first;

      auto *hullObj = createHullObj(-1, name, ColorInd(), ColorInd(is, ns), bbox);

      hullObj->setDrawLayer((CQChartsPlotObj::DrawLayer) hullLayer());

      objs.push_back(hullObj);

      ++is;
    }
  }
}

//---

void
CQChartsScatterPlot::
addDensityObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  auto bbox = calcDataRange(/*adjust*/false);

  // one map per group
  for (const auto &pg : groupNameValues_) {
    int         groupInd   = pg.first;
    const auto &nameValues = pg.second;

    bool hidden = (hasGroups && isSetHidden(groupInd));
    if (hidden) continue;

    for (const auto &pn : nameValues) {
      const auto &name = pn.first;

      auto *densityObj = createDensityObj(groupInd, name, bbox);

      densityObj->setDrawLayer((CQChartsPlotObj::DrawLayer) densityMapLayer());

      connect(densityObj, SIGNAL(layerChanged()), this, SLOT(updateSlot()));

      objs.push_back(densityObj);
    }
  }
}

CQChartsBivariateDensity *
CQChartsScatterPlot::
getDensity(int groupInd, const QString &name) const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  //---

  bool needsCalc = false;

  // create density data if does not exist
  auto pd = th->groupNamedDensity_.find(groupInd);

  if (pd == th->groupNamedDensity_.end()) {
    // get name values for group
    auto pnv = groupNameValues_.find(groupInd);
    assert(pnv != groupNameValues_.end());

    const auto &nameValues = (*pnv).second;

    //---

    // create bivariate density for each name
    NamedDensity namedDensity;

    for (const auto &pn : nameValues) {
      auto *density = new CQChartsBivariateDensity;

      namedDensity[pn.first] = density;
    }

    pd = th->groupNamedDensity_.insert(pd,
      GroupNamedDensity::value_type(groupInd, namedDensity));

    //---

    needsCalc = true;
  }

  //---

  // check if pixel size matches calculation,
  // if new we need a new calculation for new pixel grid
  if (! needsCalc) {
    auto psize = calcPixelSize();

    if (psize != densityMapData_.psize)
      needsCalc = true;
  }

  //---

  // if new calc needed then start or restart thread calc
  if (needsCalc) {
    auto pt = densityMapData_.groupThread.find(groupInd);

    if (pt == densityMapData_.groupThread.end() || (*pt).second->isReady())
      th->calcDensityMap(groupInd);

    // trigger redraw to check for result
    // TODO: separate timer
    //th->updateSlot();

    return nullptr;
  }

  //---

  // if result not ready then wait
  // TODO: separate timer
  auto pt = densityMapData_.groupThread.find(groupInd);

  if (pt == densityMapData_.groupThread.end() || ! (*pt).second->isReady()) {
    //th->updateSlot();

    return nullptr;
  }

  //---

  // get named density
  const auto &namedDensity = (*pd).second;

  auto pn = namedDensity.find(name);

  if (pn == namedDensity.end())
    return nullptr;

  return (*pn).second;
}

//---

QString
CQChartsScatterPlot::
singleGroupName(ColorInd &ind) const
{
  if (! groupNameValues_.empty()) {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    if (parentPlot() && ! nameValues.empty()) {
      const auto &name = nameValues.begin()->first;

      int ig = parentPlot()->childPlotIndex(this);
      int ng = parentPlot()->numChildPlots();

      ind = ColorInd(ig, ng);

      return name;
    }
  }

  //---

  auto name = groupIndName(0);

  if (name == "") {
    if (isX1X2())
      (void) xAxisName(name);
    else
      (void) yAxisName(name);
  }

  if (name == "")
    name = titleStr();

  int i = 0;
  int n = 1;

  if (parentPlot()) {
    i = parentPlot()->childPlotIndex(this);
    n = parentPlot()->numChildPlots();
  }

  ind = ColorInd(i, n);

  return name;
}

//------

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
      ModelIndex xModelInd(plot_, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot_, data.row, plot_->yColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(xModelInd);

      //---

      // get x, y value
      auto xInd  = plot_->modelIndex(xModelInd);
      auto xInd1 = plot_->normalizeIndex(xInd);

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
        ModelIndex nameColumnInd(plot_, data.row, plot_->nameColumn(), data.parent);

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
      if (plot_->colorColumn().isValid())
        (void) plot_->colorColumnColor(data.row, data.parent, color);

      //---

      auto *plot = const_cast<CQChartsScatterPlot *>(plot_);

      Point p(x, y);

      plot->addNameValue(groupInd, name, p, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const Column &column) {
      ModelIndex columnInd(plot_, data.row, column, data.parent);

      bool ok;

      auto var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const Column &column) {
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

  // add value to grid data if enabled
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

  // add value to hex data if enabled
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

  // add value to values data if symbol or density map enabled (always ?)
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

CQChartsScatterDensityObj *
CQChartsScatterPlot::
createDensityObj(int groupInd, const QString &name, const BBox &rect) const
{
  return new CQChartsScatterDensityObj(this, groupInd, name, rect);
}

//---

void
CQChartsScatterPlot::
addKeyItems(PlotKey *key)
{
//if (isOverlay() && ! isFirstPlot())
//  return;

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
addPointKeyItems(PlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyItem = [&](int groupInd, const QString &name, const ColorInd &is, const ColorInd &ig) {
    auto ic = (is.n > 1 ? is : ig);

    auto *colorItem = new CQChartsScatterKeyColor(this, groupInd, is, ig);
    auto *textItem  = new CQChartsKeyText        (this, name, ic);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addRowItems(colorItem, textItem);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);

    return colorItem;
  };

  //---

  int ng = groupInds_.size();

  // multiple group - key item per group
  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      auto groupName = groupIndName(groupInd);

      auto *colorItem = addKeyItem(groupInd, groupName, ColorInd(), ColorInd(ig, ng));

      //--

      // use color column and color map data if column is valid and is the grouping column
      Color color1;

      if (adjustedGroupColor(groupInd, ng, color1))
        colorItem->setColor(color1);

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

          auto *colorItem = addKeyItem(-2, name, ColorInd(is, ns), ColorInd());

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
        ColorInd ind;

        auto name = singleGroupName(ind);

        (void) addKeyItem(-2, name, ColorInd(), ColorInd(ind.i, ind.n));
      }
    }
  }
  else {
    if (isSymbols()) {
      ColorInd ind;

      auto name = singleGroupName(ind);

      (void) addKeyItem(-2, name, ColorInd(), ColorInd(ind.i, ind.n));
    }
  }
}

void
CQChartsScatterPlot::
addGridKeyItems(PlotKey *key)
{
  int n = gridData().maxN();

  auto *item = new CQChartsScatterGridKeyItem(this, n);

  key->addItem(item, 0, 0);
}

void
CQChartsScatterPlot::
addHexKeyItems(PlotKey *key)
{
  int n = hexMapMaxN();

  auto *item = new CQChartsScatterHexKeyItem(this, n);

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

  probeData.xvals.emplace_back(c.x, "", "");
  probeData.yvals.emplace_back(c.y, "", "");

  return true;
}

//---

bool
CQChartsScatterPlot::
addMenuItems(QMenu *menu)
{
  menu->addSeparator();

  auto *typeMenu = new QMenu("Plot Type", menu);

  (void) addMenuCheckedAction(typeMenu, "None"      , isNoType   (), SLOT(setNoType(bool)));
  (void) addMenuCheckedAction(typeMenu, "Symbols"   , isSymbols  (), SLOT(setSymbols(bool)));
  (void) addMenuCheckedAction(typeMenu, "Grid Cells", isGridCells(), SLOT(setGridCells(bool)));
  (void) addMenuCheckedAction(typeMenu, "Hex Cells" , isHexCells (), SLOT(setHexCells(bool)));

  menu->addMenu(typeMenu);

  //---

  if (labelColumn().isValid() || nameColumn().isValid())
    addMenuCheckedAction(menu, "Labels", isPointLabels(), SLOT(setPointLabels(bool)));

  //---

  if (canDrawColorMapKey() || canDrawSymbolSizeMapKey() || canDrawSymbolTypeMapKey()) {
    auto *keysMenu = new QMenu("Symbol Keys", menu);

    if (canDrawColorMapKey())
      addColorMapKeySubItems(keysMenu);

    if (canDrawSymbolSizeMapKey())
      addSymbolSizeMapKeySubItems(keysMenu);

    if (canDrawSymbolTypeMapKey())
      addSymbolTypeMapKeySubItems(keysMenu);

    menu->addMenu(keysMenu);
  }

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

void
CQChartsScatterPlot::
addSymbolSizeMapKeySubItems(QMenu *keysMenu)
{
  auto addSubMenu = [](QMenu *menu, const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  //---

  auto *sizeKeyMenu = addSubMenu(keysMenu, "Size Key");

  addMenuCheckedAction(sizeKeyMenu, "Visible", isSymbolSizeMapKey(),
                       SLOT(setSymbolSizeMapKey(bool)));

  //---

  view()->addKeyLocationActions(sizeKeyMenu, symbolSizeMapKey_->location(),
                                this, SLOT(symbolSizeMapKeyPositionSlot(QAction *)),
                                /*includeAuto*/false);

  //---

  bool insideXChecked = symbolSizeMapKey_->isInsideX();
  bool insideYChecked = symbolSizeMapKey_->isInsideY();

  (void) addMenuCheckedAction(sizeKeyMenu, "Inside X", insideXChecked,
                              SLOT(symbolSizeMapKeyInsideXSlot(bool)));
  (void) addMenuCheckedAction(sizeKeyMenu, "Inside Y", insideYChecked,
                              SLOT(symbolSizeMapKeyInsideYSlot(bool)));
}

void
CQChartsScatterPlot::
symbolSizeMapKeyPositionSlot(QAction *action)
{
  CQChartsKeyLocation::Type location;

  if (! CQChartsKeyLocation::decodeString(action->text(), location))
    assert(false);

  symbolSizeMapKey_->setLocation(CQChartsKeyLocation(location));
}

void
CQChartsScatterPlot::
symbolSizeMapKeyInsideXSlot(bool b)
{
  if (b != symbolSizeMapKey_->isInsideX())
    symbolSizeMapKey_->setInsideX(b);
}

void
CQChartsScatterPlot::
symbolSizeMapKeyInsideYSlot(bool b)
{
  if (b != symbolSizeMapKey_->isInsideY())
    symbolSizeMapKey_->setInsideY(b);
}

void
CQChartsScatterPlot::
addSymbolTypeMapKeySubItems(QMenu *keysMenu)
{
  auto addSubMenu = [](QMenu *menu, const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  //---

  auto *typeKeyMenu = addSubMenu(keysMenu, "Type Key");

  addMenuCheckedAction(typeKeyMenu, "Visible", isSymbolTypeMapKey(),
                       SLOT(setSymbolTypeMapKey(bool)));

  //---

  view()->addKeyLocationActions(typeKeyMenu, symbolTypeMapKey_->location(),
                                this, SLOT(symbolTypeMapKeyPositionSlot(QAction *)),
                                /*includeAuto*/false);

  //---

  bool insideXChecked = symbolTypeMapKey_->isInsideX();
  bool insideYChecked = symbolTypeMapKey_->isInsideY();

  (void) addMenuCheckedAction(typeKeyMenu, "Inside X", insideXChecked,
                              SLOT(symbolTypeMapKeyInsideXSlot(bool)));
  (void) addMenuCheckedAction(typeKeyMenu, "Inside Y", insideYChecked,
                              SLOT(symbolTypeMapKeyInsideYSlot(bool)));
}

void
CQChartsScatterPlot::
symbolTypeMapKeyPositionSlot(QAction *action)
{
  CQChartsKeyLocation::Type location;

  if (! CQChartsKeyLocation::decodeString(action->text(), location))
    assert(false);

  symbolTypeMapKey_->setLocation(CQChartsKeyLocation(location));
}

void
CQChartsScatterPlot::
symbolTypeMapKeyInsideXSlot(bool b)
{
  if (b != symbolTypeMapKey_->isInsideX())
    symbolTypeMapKey_->setInsideX(b);
}

void
CQChartsScatterPlot::
symbolTypeMapKeyInsideYSlot(bool b)
{
  if (b != symbolTypeMapKey_->isInsideY())
    symbolTypeMapKey_->setInsideY(b);
}

//------

CQChartsGeom::BBox
CQChartsScatterPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsScatterPlot::calcExtraFitBBox");

  BBox bbox;

  if (isXRug() || isYRug() || isXDensity() || isYDensity() || isXWhisker() || isYWhisker()) {
    double dx = 0.0, dy = 0.0;

    auto addBBoxX = [&](const BBox &bbox1) {
      if (bbox1.isSet()) {
        dy += bbox1.getHeight();

        bbox += bbox1;
      }
    };

    auto addBBoxY = [&](const BBox &bbox1) {
      if (bbox1.isSet()) {
        dx += bbox1.getWidth();

        bbox += bbox1;
      }
    };

    // x/y rug axis
    if (isXRug()) addBBoxX(xRug_->calcBBox());
    if (isYRug()) addBBoxY(yRug_->calcBBox());

    // x/y density axis
    if (isXDensity()) addBBoxX(xAxisDensity_->calcDeltaBBox(dy));
    if (isYDensity()) addBBoxY(yAxisDensity_->calcDeltaBBox(dx));

    // x/y whisker axis (one per group)
    int ng = groupInds_.size();

    if (isXWhisker()) addBBoxX(xAxisWhisker_->calcNDeltaBBox(ng, dy));
    if (isYWhisker()) addBBoxY(yAxisWhisker_->calcNDeltaBBox(ng, dx));
  }

  return bbox;
}

double
CQChartsScatterPlot::
xAxisHeight(const CQChartsAxisSide::Type &side) const
{
  double h = CQChartsPlot::xAxisHeight(side);

  if (isXRug() || isXDensity() || isXWhisker()) {
    auto addHeight = [&](const BBox &bbox) {
      if (bbox.isSet())
        h += bbox.getHeight();
    };

    //---

    // x rug axis
    if (isXRug() && xRug_->side().type() == side)
      addHeight(xRug_->calcBBox());

    // x density axis
    if (isXDensity() && xAxisDensity_->side().type() == side)
      addHeight(xAxisDensity_->calcDeltaBBox(0.0));

    // x whisker axis (one per group)
    int ng = groupInds_.size();

    if (isXWhisker() && xAxisWhisker_->side().type() == side)
      addHeight(xAxisWhisker_->calcNDeltaBBox(ng, 0.0));
  }

  return h;
}

double
CQChartsScatterPlot::
yAxisWidth(const CQChartsAxisSide::Type &side) const
{
  double w = CQChartsPlot::yAxisWidth(side);

  if (isYRug() || isYDensity() || isYWhisker()) {
    auto addWidth = [&](const BBox &bbox) {
      if (bbox.isSet())
        w += bbox.getWidth();
    };

    //---

    // y rug axis
    if (isYRug() && yRug_->side().type() == side)
      addWidth(yRug_->calcBBox());

    // y density axis
    if (isYDensity() && yAxisDensity_->side().type() == side)
      addWidth(yAxisDensity_->calcDeltaBBox(0.0));

    // y whisker axis (one per group)
    int ng = groupInds_.size();

    if (isYWhisker() && yAxisWhisker_->side().type() == side)
      addWidth(yAxisWhisker_->calcNDeltaBBox(ng, 0.0));
  }

  return w;
}

//------

bool
CQChartsScatterPlot::
hasBackground() const
{
  if (isStatsLines()) return true;

  if (isXRug()) return true;
  if (isYRug()) return true;

  if (isXWhisker()) return true;
  if (isYWhisker()) return true;

  if (isXDensity()) return true;
  if (isYDensity()) return true;

  return false;
}

void
CQChartsScatterPlot::
execDrawBackground(PaintDevice *device) const
{
  CQChartsPlot::execDrawBackground(device);

  // draw stats lines on background
  if (isStatsLines()) drawStatsLines(device);

  //---

  // drawn axis annotatons in inside->outside order
  xAxisSideHeight_[CQChartsAxisSide::Type::BOTTOM_LEFT] =
    xAxisSideDelta(CQChartsAxisSide::Type::BOTTOM_LEFT);
  xAxisSideHeight_[CQChartsAxisSide::Type::TOP_RIGHT] =
    xAxisSideDelta(CQChartsAxisSide::Type::TOP_RIGHT);

  yAxisSideWidth_[CQChartsAxisSide::Type::BOTTOM_LEFT] =
    yAxisSideDelta(CQChartsAxisSide::Type::BOTTOM_LEFT);
  yAxisSideWidth_[CQChartsAxisSide::Type::TOP_RIGHT] =
    yAxisSideDelta(CQChartsAxisSide::Type::TOP_RIGHT);

  //---

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
  if (isColorMapKey())
    drawColorMapKey(device);

  if (isSymbolSizeMapKey())
    drawSymbolSizeMapKey(device);

  if (isSymbolTypeMapKey())
    drawSymbolTypeMapKey(device);
}

//---

void
CQChartsScatterPlot::
preDrawObjs(PaintDevice *) const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  th->dataLabelDatas_.clear();
}

void
CQChartsScatterPlot::
postDrawObjs(PaintDevice *device) const
{
  drawDataLabelDatas(device);
}

//---

void
CQChartsScatterPlot::
drawXAxisAt(PaintDevice *device, CQChartsPlot *plot, double pos) const
{
  auto addHeight = [&](const BBox &bbox) {
    if (! bbox.isSet()) return;

    if (xAxis()->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
      pos += bbox.getHeight();
    else
      pos -= bbox.getHeight();
  };

  if (isXRug() && xRug_->side().type() == xAxis()->side().type())
    addHeight(xRug_->calcBBox());

  if (isXDensity() && xAxisDensity_->side().type() == xAxis()->side().type())
    addHeight(xAxisDensity_->calcDeltaBBox(0.0));

  if (isXWhisker() && xAxisWhisker_->side().type() == xAxis()->side().type())
    addHeight(xAxisWhisker_->calcNDeltaBBox(groupInds_.size(), 0.0));

  CQChartsPlot::drawXAxisAt(device, plot, pos);
}

void
CQChartsScatterPlot::
drawYAxisAt(PaintDevice *device, CQChartsPlot *plot, double pos) const
{
  auto addWidth = [&](const BBox &bbox) {
    if (! bbox.isSet()) return;

    if (yAxis()->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
      pos += bbox.getWidth();
    else
      pos -= bbox.getWidth();
  };

  if (isYRug() && yRug_->side().type() == yAxis()->side().type())
    addWidth(yRug_->calcBBox());

  if (isYDensity() && yAxisDensity_->side().type() == yAxis()->side().type())
    addWidth(yAxisDensity_->calcDeltaBBox(0.0));

  if (isYWhisker() && yAxisWhisker_->side().type() == yAxis()->side().type())
    addWidth(yAxisWhisker_->calcNDeltaBBox(groupInds_.size(), 0.0));

  CQChartsPlot::drawYAxisAt(device, plot, pos);
}

//---

void
CQChartsScatterPlot::
drawStatsLines(PaintDevice *device) const
{
  int ng = groupInds_.size();

  int nf;

  if (ng > 1) {
    // init stats data
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      if (isInterrupt())
        return;

      initGroupStats(ig++, QVariant(groupInd), /*isGroup*/true);
    }

    nf = ng;
  }
  else {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    int ns = nameValues.size();

    //---

    // init fit data
    int is = 0;

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        return;

      initGroupStats(is++, nameValue.first, /*isGroup*/false);
    }

    nf = ns;
  }

  //---

  // draw stats data
  for (int i = 0; i < nf; ++i) {
    if (isInterrupt())
      return;

    auto ps = groupStatData_.find(i);
    assert(ps != groupStatData_.end());

    const auto &statData = (*ps).second;

    //---

    // calc pen and brush
    ColorInd ic(i, nf);

    PenBrush penBrush;

    auto c = interpStatsLinesColor(ic);

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

    //---

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
  }
}

//------

void
CQChartsScatterPlot::
drawXRug(PaintDevice *device) const
{
  double delta = xAxisSideHeight_[xRug_->side().type()];

  drawXYRug(device, xRug_, delta);

  xAxisSideHeight_[xRug_->side().type()] += xRug_->calcBBox().getHeight();
}

void
CQChartsScatterPlot::
drawYRug(PaintDevice *device) const
{
  double delta = yAxisSideWidth_[yRug_->side().type()];

  drawXYRug(device, yRug_, delta);

  yAxisSideWidth_ [yRug_->side().type()] += yRug_->calcBBox().getWidth();
}

void
CQChartsScatterPlot::
drawXYRug(PaintDevice *device, const RugP &rug, double delta) const
{
  rug->clearPoints();

  for (const auto &plotObj : plotObjects()) {
    if (isInterrupt())
      return;

    //---

    auto *pointObj = dynamic_cast<CQChartsScatterPointObj *>(plotObj);

    if (pointObj) {
      PenBrush penBrush;

      pointObj->calcPenBrush(penBrush, /*updateState*/false);

      if (rug->direction() == Qt::Horizontal)
        rug->addPoint(CQChartsAxisRug::RugPoint(pointObj->point().x, penBrush.pen.color()));
      else
        rug->addPoint(CQChartsAxisRug::RugPoint(pointObj->point().y, penBrush.pen.color()));
    }

    //---

    auto *cellObj = dynamic_cast<CQChartsScatterCellObj *>(plotObj);

    if (cellObj) {
      PenBrush penBrush;

      cellObj->calcRugPenBrush(penBrush, /*updateState*/false);

      for (const auto &p : cellObj->points()) {
        if (rug->direction() == Qt::Horizontal)
          rug->addPoint(CQChartsAxisRug::RugPoint(p.x, penBrush.pen.color()));
        else
          rug->addPoint(CQChartsAxisRug::RugPoint(p.y, penBrush.pen.color()));
      }
    }
  }

  rug->draw(device, delta);
}

//------

void
CQChartsScatterPlot::
drawXDensity(PaintDevice *device) const
{
  double delta = xAxisSideHeight_[xAxisDensity_->side().type()];

  initWhiskerData();

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

  xAxisSideHeight_[xAxisDensity_->side().type()] +=
    xAxisDensity_->calcDeltaBBox(delta).getHeight();
}

void
CQChartsScatterPlot::
drawYDensity(PaintDevice *device) const
{
  double delta = yAxisSideWidth_[yAxisDensity_->side().type()];

  initWhiskerData();

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

  yAxisSideWidth_[yAxisDensity_->side().type()] +=
    yAxisDensity_->calcDeltaBBox(delta).getWidth();
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
  PenBrush penBrush;

  auto strokeColor = interpSymbolStrokeColor(ig);
  auto fillColor   = interpSymbolFillColor  (ig);

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
  double delta = xAxisSideHeight_[xAxisWhisker_->side().type()];

  initWhiskerData();

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

  //---

  xAxisSideHeight_[xAxisWhisker_->side().type()] +=
    xAxisWhisker_->calcNDeltaBBox(ng, delta).getHeight();
}

void
CQChartsScatterPlot::
drawYWhisker(PaintDevice *device) const
{
  double delta = yAxisSideWidth_[yAxisWhisker_->side().type()];

  initWhiskerData();

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

  //---

  yAxisSideWidth_[yAxisWhisker_->side().type()] +=
    yAxisWhisker_->calcNDeltaBBox(ng, delta).getWidth();
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
  PenBrush penBrush;

  auto strokeColor = interpSymbolStrokeColor(ig);
  auto fillColor   = interpSymbolFillColor  (ig);

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
calcDensityMap(int groupInd)
{
  auto pt = densityMapData_.groupThread.find(groupInd);

  if (pt == densityMapData_.groupThread.end() || ! (*pt).second) {
    densityMapData_.groupThread[groupInd] = new CQThreadObject;

    pt = densityMapData_.groupThread.find(groupInd);
  }

  (*pt).second->exec(calcDensityMapThread, this, groupInd);
}

void
CQChartsScatterPlot::
calcDensityMapThread(CQChartsScatterPlot *plot, int groupInd)
{
  plot->calcDensityMapImpl(groupInd);
}

void
CQChartsScatterPlot::
calcDensityMapImpl(int groupInd)
{
  auto pnv = groupNameValues_.find(groupInd);
  assert(pnv != groupNameValues_.end());

  auto pd = groupNamedDensity_.find(groupInd);
  assert(pd != groupNamedDensity_.end());

  //---

  CQChartsBivariateDensity::Data data;

  data.gridSize = densityMapGridSize();
  data.delta    = densityMapDelta();

  const auto &nameValues = (*pnv).second;

  for (const auto &pn : nameValues) {
    if (isInterrupt())
      return;

    const auto &name   = pn.first;
    const auto &values = pn.second;

    data.values.clear();

    for (const auto &v : values.values)
      data.values.push_back(v.p);

    data.xrange = values.xrange;
    data.yrange = values.yrange;

    auto pdn = (*pd).second.find(name);
    assert(pdn != (*pd).second.end());

    auto *density = (*pdn).second;

    density->calc(this, data);
  }

  //---

  // set pixel size of result
  auto psize = calcPixelSize();

  densityMapData_.psize = psize;

  //---

  auto pt = densityMapData_.groupThread.find(groupInd);

  (*pt).second->end();

  //---

  // trigger redraw to show calculated results
  updateSlot();
}

//------

void
CQChartsScatterPlot::
initWhiskerData() const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  // add group points
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

  // add group grid cells
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
addDataLabelData(const BBox &bbox, const QString &text, const CQChartsLabelPosition &position,
                 const PenBrush &penBrush, const Font &font)
{
  DataLabelData dataLabelData;

  dataLabelData.bbox     = bbox;
  dataLabelData.text     = text;
  dataLabelData.position = position;
  dataLabelData.penBrush = penBrush;
  dataLabelData.font     = font;

  dataLabelDatas_.push_back(dataLabelData);
}

void
CQChartsScatterPlot::
drawDataLabelDatas(PaintDevice *device) const
{
  const auto *dataLabel = this->dataLabel();

  for (const auto &dataLabelData : dataLabelDatas_)
    dataLabel->draw(device, dataLabelData.bbox, dataLabelData.text,
                    (CQChartsDataLabel::Position) dataLabelData.position,
                    dataLabelData.penBrush, dataLabelData.font);
}

//---

void
CQChartsScatterPlot::
getGroupObjs(int ig, PlotObjs &objs) const
{
  for (const auto &plotObj : plotObjs_) {
    auto *pointObj = dynamic_cast<PointObj *>(plotObj);
    if (! pointObj) continue;

    if (pointObj->is().n == 1 && pointObj->ig().n > 1 && pointObj->ig().i == ig)
      objs.push_back(pointObj);
  }
}

//---

CQChartsPlotCustomControls *
CQChartsScatterPlot::
createCustomControls()
{
  auto *controls = new CQChartsScatterPlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(const Plot *plot, int groupInd, const BBox &rect, const Point &pos,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ig, iv), plot_(plot),
 groupInd_(groupInd), pos_(pos)
{
//setDetailHint(DetailHint::MAJOR);
}

//---

CQChartsSymbol
CQChartsScatterPointObj::
calcSymbol() const
{
  auto symbol = this->symbol();

  if (! symbol.isValid()) {
    symbol = plot_->symbol();

    //if (! symbol.isValid())
    //  symbol = Symbol::circle();
  }

  return symbol;
}

CQChartsLength
CQChartsScatterPointObj::
calcSymbolSize() const
{
  auto symbolSize = this->symbolSize();

  if (! symbolSize.isValid())
    symbolSize = plot()->symbolSize();

  return symbolSize;
}

CQChartsLength
CQChartsScatterPointObj::
calcFontSize() const
{
  auto fontSize = this->fontSize();

  if (! fontSize.isValid())
    fontSize = plot()->dataLabelFontSize();

  return fontSize;
}

//---

QString
CQChartsScatterPointObj::
calcId() const
{
  auto ind1 = plot_->unnormalizeIndex(modelInd());

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
    auto groupName = plot_->groupIndName(groupInd_);

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

  auto addColumnRowValue = [&](const Column &column) {
    if (! column.isValid()) return;

    if (tableTip.hasColumn(column))
      return;

    ModelIndex columnInd(plot_, modelInd().row(), column, modelInd().parent());

    bool ok;

    auto str = plot_->modelString(columnInd, ok);
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
  CQChartsPointPlot::ValueData valuePoint;

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

  // add alpha column
  if (valuePoint.alpha.isSet())
    tableTip.addTableRow(plot_->alphaHeaderName(/*tip*/true), valuePoint.alpha.toString());
  else
    addColumnRowValue(plot_->alphaColumn());

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

  plot_->pixelSymbolSize(this->calcSymbolSize(), sx, sy);

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
  addColumnSelectIndex(inds, plot_->alphaColumn     ());
}

//---

void
CQChartsScatterPointObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // get symbol and size
  auto symbol     = this->calcSymbol();
  auto symbolSize = this->calcSymbolSize();

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

    if (symbol.isValid())
      CQChartsDrawUtil::drawSymbol(device, penBrush, symbol, ps1, symbolSize);
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
  if (plot_->isPointLabels() && name().length())
    drawDataLabel(device);
}

void
CQChartsScatterPointObj::
drawDataLabel(PaintDevice *) const
{
  const auto *dataLabel = plot_->dataLabel();

  //---

  // text font color
  PenBrush penBrush;

  auto tc = dataLabel->interpTextColor(calcColorInd());

  plot_->setPenBrush(penBrush,
    PenData(true, tc, dataLabel->textAlpha()), BrushData(false));

  //---

  // get custom font size (from font column)
  auto fontSize = this->calcFontSize();

  //---

  // get label font
  auto font  = plot_->dataLabelFont();
  auto font1 = font;

  if (fontSize.isValid()) {
    double fontPixelSize = plot_->lengthPixelHeight(fontSize);

    // scale to font size
    fontPixelSize = plot_->limitFontSize(fontPixelSize);

    font1.setPointSizeF(fontPixelSize);
  }

  //---

  // draw text
  auto ps = plot_->windowToPixel(pos_);

  double sx, sy;

  plot_->pixelSymbolSize(calcSymbolSize(), sx, sy);

  BBox ptbbox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

  const_cast<CQChartsScatterPlot *>(plot_)->
    addDataLabelData(plot_->pixelToWindow(ptbbox), name(),
                     plot_->dataLabelPosition(), penBrush, font1);
}

void
CQChartsScatterPointObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
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

  //plot_->setSymbolPenBrush(penBrush, ic);

  auto fc = plot_->interpSymbolFillColor(ic);
  auto fa = plot_->symbolFillAlpha();
  auto sc = plot_->interpSymbolStrokeColor(ic);
  auto sa = plot_->symbolStrokeAlpha();

  // override symbol fill color for custom color (TODO: calc)
  auto color = this->color();

  if (color.isValid())
    fc = plot_->interpColor(color, ic);

  // override symbol fill alpha for custom alpha (TODO: calc)
  auto alpha = this->alpha();

  if (alpha.isSet())
    fa = alpha;

  //---

  bool filled  = plot_->isSymbolFilled();
  bool stroked = plot_->isSymbolStroked();

  if      (! this->symbol().isFilled()) {
    filled  = false;
    stroked = true;

    // use fill color for stroke
    sc = fc;
    sa = fa;
  }
  else if (! this->symbol().isStroked()) {
    filled  = false;
    stroked = true;
  }

  //---

  plot_->setPenBrush(penBrush,
    CQChartsPenData(stroked, sc, sa, plot_->symbolStrokeWidth(), plot_->symbolStrokeDash()),
    CQChartsBrushData(filled, fc, fa, plot_->symbolFillPattern()));

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush, drawType());
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
                       const ColorInd &ig, int ix, int iy, const Points &points, int maxN) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
 groupInd_(groupInd), ix_(ix), iy_(iy), points_(points), maxN_(maxN)
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
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

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
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set pen and brush
  ColorInd ic(points_.size(), maxN_);

  auto pc = plot_->interpGridCellStrokeColor(ColorInd());
  auto fc = plot_->interpPaletteColor(ic);

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
calcRugPenBrush(PenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  auto ic = (ig_.n > 1 ? ig_ : is_);

  plot_->setSymbolPenBrush(penBrush, ic);

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
}

void
CQChartsScatterCellObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.count = " << points_.size() << ";\n";
}

//------

CQChartsScatterHexObj::
CQChartsScatterHexObj(const Plot *plot, int groupInd, const BBox &rect, const ColorInd &is,
                      const ColorInd &ig, int ix, int iy, const Polygon &poly, int n, int maxN) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ig, ColorInd()), plot_(plot),
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
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

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
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set pen and brush
  ColorInd ic(n_, maxN_);

  auto pc = plot_->interpGridCellStrokeColor(ColorInd());
  auto fc = plot_->interpPaletteColor(ic);

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
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.count = " << n_ << ";\n";
}

//------

CQChartsScatterDensityObj::
CQChartsScatterDensityObj(const Plot *plot, int groupInd, const QString &name, const BBox &rect) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()), plot_(plot),
 groupInd_(groupInd), name_(name)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsScatterDensityObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(groupInd_);
}

QString
CQChartsScatterDensityObj::
calcTipId() const
{
  auto *density = plot_->getDensity(groupInd_, name_);
  if (! density) return "";

  //---

  CQChartsTableTip tableTip;

  QString groupName;

  if (name_ == "")
    groupName = plot_->groupIndName(groupInd_);
  else {
    ColorInd ind;

    groupName = plot_->singleGroupName(ind);

    if (groupName == "")
      groupName = name_;
  }

  tableTip.addBoldLine("Density Map");
  tableTip.addTableRow("Group"   , groupName);
  tableTip.addTableRow("X StdDev", density->xStdDev());
  tableTip.addTableRow("Y StdDev", density->yStdDev());

  return tableTip.str();
}

void
CQChartsScatterDensityObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsPlotObj::addProperties(model, path);
}

void
CQChartsScatterDensityObj::
draw(PaintDevice *device) const
{
  auto *density = plot_->getDensity(groupInd_, name_);
  if (! density) return;

  //---

  // draw density
  CQChartsPaintDevice::SaveRestore saveRestore(device);

  plot_->setClipRect(device);

  density->draw(plot_, device);
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(Plot *plot, int groupInd, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyColorBox(plot, is, ig, ColorInd()), plot_(plot), groupInd_(groupInd)
{
  setClickable(true);
}

#if 0
bool
CQChartsScatterKeyColor::
selectPress(const Point &, SelMod selMod)
{
  auto *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  auto ic = (is_.n > 1 ? is_ : ig_);

  auto ih = setIndex();

  if (selMod == SelMod::ADD) {
    for (int i = 0; i < ic.n; ++i) {
      plot_->CQChartsPlot::setSetHidden(i, i != ih.i);
    }
  }
  else {
    plot->setSetHidden(ih.i, ! plot->isSetHidden(ih.i));
  }

  plot->updateRangeAndObjs();

  return true;
}
#endif

void
CQChartsScatterKeyColor::
doSelect(SelMod)
{
  CQChartsPlot::PlotObjs objs;

  plot()->getGroupObjs(ig_.i, objs);
  if (objs.empty()) return;

  //---

  plot()->selectObjs(objs, /*export*/true);

  key_->redraw(/*wait*/ true);
}

QBrush
CQChartsScatterKeyColor::
fillBrush() const
{
  auto ic = (is_.n > 1 ? is_ : ig_);

  QColor c;

  if (color_.isValid())
    c = plot_->interpColor(color_, ColorInd());
  else {
    c = plot_->interpSymbolFillColor(ic);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  CQChartsDrawUtil::setColorAlpha(c, plot_->symbolFillAlpha());

  auto ih = setIndex();

  if (plot_->isSetHidden(ih.i))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

bool
CQChartsScatterKeyColor::
tipText(const Point &, QString &tip) const
{
  if (groupInd_ < -1) return false;

  CQChartsPlot::PlotObjs objs;

  plot()->getGroupObjs(groupInd_, objs);

  CQChartsTableTip tableTip;

  auto groupName = plot_->groupIndName(groupInd_);

  tableTip.addTableRow("Name", groupName);

  if (! objs.empty())
    tableTip.addTableRow("Count", objs.size());

  if (isSetHidden())
    tableTip.addTableRow("Hidden", "true");

  tip = tableTip.str();

  return true;
}

CQChartsUtil::ColorInd
CQChartsScatterKeyColor::
setIndex() const
{
  return (groupInd_ >= -1 ? ColorInd(groupInd_, 1) : is_);
}

//---

CQChartsScatterGridKeyItem::
CQChartsScatterGridKeyItem(Plot *plot, int n) :
 CQChartsGradientKeyItem(plot), plot_(plot)
{
  setMinValue(0);
  setMaxValue(n);
  setInteger (true);
}

//---

CQChartsScatterHexKeyItem::
CQChartsScatterHexKeyItem(Plot *plot, int n) :
 CQChartsGradientKeyItem(plot), plot_(plot)
{
  setMinValue(0);
  setMaxValue(n);
  setInteger (true);
}

//------

CQChartsScatterPlotCustomControls::
CQChartsScatterPlotCustomControls(CQCharts *charts) :
 CQChartsPointPlotCustomControls(charts, "scatter")
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addColumnWidgets(QStringList() << "x" << "y", columnsFrame);
//addColumnWidgets(QStringList() << "x" << "y" << "name", columnsFrame);

  //---

  addGroupColumnWidgets();

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  bestFitCheck_    = CQUtil::makeLabelWidget<QCheckBox>("Best Fit"   , "bestFitCheck");
  convexHullCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Convex Hull", "convexHullCheck");

  addFrameColWidget(optionsFrame, bestFitCheck_);
  addFrameColWidget(optionsFrame, convexHullCheck_);

  //---

  addColorColumnWidgets("Point Color");
  addSymbolSizeWidgets ();
  addSymbolLabelWidgets();
  addFontSizeWidgets   ();

  //---

  // color, contrast, ...

  //---

#if 0
  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  plotTypeCombo_ = createEnumEdit("plotType");

  addFrameWidget(optionsFrame, "Plot Type", plotTypeCombo_);

  addFrameRowStretch(optionsFrame);
#endif

  //---

  addLayoutStretch();

  //---

  connectSlots(true);
}

void
CQChartsScatterPlotCustomControls::
addSymbolLabelWidgets()
{
  // point labels group
  auto *groupBox    = CQUtil::makeWidget<CQGroupBox>("symbolLabelGroup");
  auto *groupLayout = CQUtil::makeLayout<QVBoxLayout>(groupBox, 0, 0);

  groupBox->setTitle("Point Label");

  groupBox->setTitleScale(0.85);
  groupBox->setTitleColored(true);

  layout_->addWidget(groupBox);

  //---

  auto pointLabelsFrame = createFrame("pointLabelsFrame");

  //---

  // label text and font
//pointLabelsCheck_ = CQUtil::makeWidget<CQCheckBox>("pointLabels");
  labelColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("labelColumnCombo");
  positionEdit_     = CQUtil::makeWidget<CQEnumCombo>("positionEdit");

  positionEdit_->setPropName("position");

//addFrameWidget(pointLabelsFrame, "Visible" , pointLabelsCheck_);
  addFrameWidget(pointLabelsFrame, "Column"  , labelColumnCombo_);
  addFrameWidget(pointLabelsFrame, "Position", positionEdit_);

//addFrameRowStretch(pointLabelsFrame);

  //---

  groupLayout->addWidget(pointLabelsFrame.frame);
}

void
CQChartsScatterPlotCustomControls::
addFontSizeWidgets()
{
  // font size group
  auto fontSizeControlGroupData =
    createColumnControlGroup("fontSizeControlGroup", "Label Font");

  fontSizeControlGroup_ = fontSizeControlGroupData.group;

  fontSizeControlGroup_->setHasKey(false);

  //---

  // font size widgets
  fontSizeEdit_        = CQUtil::makeWidget<CQChartsLengthEdit>("fontEdit");
  fontSizeColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("fontSizeColumnCombo");
  fontSizeRange_       = CQUtil::makeWidget<CQChartsFontSizeRangeSlider>("fontSizeRange");

  addFrameWidget(fontSizeControlGroupData.fixedFrame , "Size"  , fontSizeEdit_);
  addFrameWidget(fontSizeControlGroupData.columnFrame, "Column", fontSizeColumnCombo_);
  addFrameWidget(fontSizeControlGroupData.columnFrame, "Range" , fontSizeRange_);

  addFrameRowStretch(fontSizeControlGroupData.fixedFrame );
  addFrameRowStretch(fontSizeControlGroupData.columnFrame);
}

void
CQChartsScatterPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    bestFitCheck_, SIGNAL(stateChanged(int)), this, SLOT(bestFitSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    convexHullCheck_, SIGNAL(stateChanged(int)), this, SLOT(convexHullSlot()));

  if (plotTypeCombo_)
    CQChartsWidgetUtil::connectDisconnect(b,
      plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));

  if (labelColumnCombo_) {
  //CQChartsWidgetUtil::connectDisconnect(b,
  //  pointLabelsCheck_, SIGNAL(stateChanged(int)), this, SLOT(pointLabelsSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      labelColumnCombo_, SIGNAL(columnChanged()), this, SLOT(labelColumnSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      positionEdit_, SIGNAL(currentIndexChanged(int)), this, SLOT(positionSlot()));
  }

  if (fontSizeEdit_) {
    CQChartsWidgetUtil::connectDisconnect(b,
      fontSizeControlGroup_, SIGNAL(groupChanged()), this, SLOT(fontSizeGroupChanged()));
    CQChartsWidgetUtil::connectDisconnect(b,
      fontSizeEdit_, SIGNAL(lengthChanged()), this, SLOT(fontSizeSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      fontSizeColumnCombo_, SIGNAL(columnChanged()), this, SLOT(fontSizeColumnSlot()));
    CQChartsWidgetUtil::connectDisconnect(b,
      fontSizeRange_, SIGNAL(sliderRangeChanged(double, double)),
      this, SLOT(fontSizeRangeSlot(double, double)));
  }

  CQChartsPointPlotCustomControls::connectSlots(b);
}

void
CQChartsScatterPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsScatterPlot *>(plot);

  CQChartsPointPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

CQChartsColor
CQChartsScatterPlotCustomControls::
getColorValue()
{
  return plot_->symbolFillColor();
}

void
CQChartsScatterPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setSymbolFillColor(c);
}

void
CQChartsScatterPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  if (plotTypeCombo_)
    plotTypeCombo_->setCurrentValue((int) plot_->plotType());

  //---

  if (labelColumnCombo_) {
    bool hasLabelColumn = plot_->labelColumn().isValid();

  //pointLabelsCheck_->setEnabled(hasLabelColumn);
    positionEdit_    ->setEnabled(hasLabelColumn);

  //pointLabelsCheck_->setChecked(plot_->isPointLabels());
    labelColumnCombo_->setModelColumn(plot_->getModelData(), plot_->labelColumn());
    positionEdit_    ->setObj(plot_->dataLabel());
  }

  //---

  if (fontSizeEdit_) {
    bool hasFontSizeColumn = plot_->fontSizeColumn().isValid();

    fontSizeEdit_ ->setEnabled(! hasFontSizeColumn);
    fontSizeRange_->setEnabled(hasFontSizeColumn);

    fontSizeEdit_       ->setLength(plot_->dataLabelFontSize());
    fontSizeColumnCombo_->setModelColumn(plot_->getModelData(), plot_->fontSizeColumn());
    fontSizeRange_      ->setPlot(plot_);

    if (hasFontSizeColumn)
      fontSizeControlGroup_->setColumn();
  }

  //---

  CQChartsPointPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsScatterPlotCustomControls::
bestFitSlot()
{
  plot_->setBestFit(bestFitCheck_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
convexHullSlot()
{
  plot_->setHull(convexHullCheck_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
plotTypeSlot()
{
  if (plotTypeCombo_)
    plot_->setPlotType((CQChartsScatterPlot::PlotType) plotTypeCombo_->currentValue());
}

#if 0
void
CQChartsScatterPlotCustomControls::
pointLabelsSlot()
{
  plot_->setPointLabels(pointLabelsCheck_->isChecked());
}
#endif

void
CQChartsScatterPlotCustomControls::
labelColumnSlot()
{
  plot_->setLabelColumn(labelColumnCombo_->getColumn());

  bool hasLabelColumn = plot_->labelColumn().isValid();

  plot_->setPointLabels(hasLabelColumn);

  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
positionSlot()
{
  plot_->setDataLabelPosition((CQChartsLabelPosition) positionEdit_->currentIndex());

  // TODO: need plot signal (property signal ?)
  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
fontSizeGroupChanged()
{
  if (fontSizeControlGroup_->isFixed()) {
    plot_->setFontSizeColumn(CQChartsColumn());

    // TODO: need plot signal
    updateWidgets();
  }
}

void
CQChartsScatterPlotCustomControls::
fontSizeSlot()
{
  plot_->setDataLabelFontSize(fontSizeEdit_->length());

  // TODO: need plot signal
  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
fontSizeColumnSlot()
{
  plot_->setFontSizeColumn(fontSizeColumnCombo_->getColumn());

  // TODO: need plot signal
  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
fontSizeRangeSlot(double min, double max)
{
  plot_->setFontSizeMapMin(min);
  plot_->setFontSizeMapMax(max);
}
