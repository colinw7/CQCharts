#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsAxisRug.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsValueSet.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsColumnType.h>
#include <CQChartsDataLabel.h>
#include <CQChartsVariant.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsBivariateDensity.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsColumnCombo.h>
#include <CQChartsLengthEdit.h>
#include <CQChartsFontSizeRangeSlider.h>
#include <CQChartsColumnControlGroup.h>
#include <CQChartsMapKey.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQThreadObject.h>
#include <CQPerfMonitor.h>
#include <CQGroupBox.h>
#include <CQEnumCombo.h>
#include <CHexMap.h>

#include <QMenu>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QApplication>

namespace {

using HexMap = CHexMap<void>;

}

//---

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
    setRequired().setNumericColumn().setPropPath("columns.x").setTip("X Value Column");
  addColumnParameter("y", "Y", "yColumn").
    setRequired().setNumericColumn().setPropPath("columns.y").setTip("Y Value Column");

  addColumnParameter("name", "Name", "nameColumn").
    setStringColumn().setPropPath("columns.name").setTip("Group Name Column");
  addColumnParameter("label", "Label", "labelColumn").
    setStringColumn().setPropPath("columns.label").setTip("Point Label Column");

  //--

  // options
  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("NONE"      , static_cast<int>(CQChartsScatterPlot::PlotType::NONE      )).
    addNameValue("SYMBOLS"   , static_cast<int>(CQChartsScatterPlot::PlotType::SYMBOLS   )).
    addNameValue("GRID_CELLS", static_cast<int>(CQChartsScatterPlot::PlotType::GRID_CELLS)).
    addNameValue("HEX_CELLS" , static_cast<int>(CQChartsScatterPlot::PlotType::HEX_CELLS )).
    setPropPath("options.plotType").setTip("Plot type");

  addBoolParameter("pointLabels", "Point Labels", "pointLabels").
    setPropPath("labels.visible").setTip("Show Label at Point");

  addBoolParameter("pareto", "Pareto Front", "pareto").
    setPropPath("overlays.pareto.visible").setTip("Show pareto front");

  addMiscParameters();

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
      p("Scatter plots are usually used for non-monotonic/non-sequential data. If the x values "
        "are monotonic/sequential then consider using an xy plot instead.").
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
 CQChartsObjLineData         <CQChartsScatterPlot>(this),
 CQChartsObjGridCellShapeData<CQChartsScatterPlot>(this),
 CQChartsObjParetoShapeData  <CQChartsScatterPlot>(this)
{
}

CQChartsScatterPlot::
~CQChartsScatterPlot()
{
  CQChartsScatterPlot::term();
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

  QFontMetrics fm(qApp->font());
  setSymbolSize(Length::pixel(fm.height()/3.0));

  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(Color::makePalette());

  setLinesColor(Color::makePalette());

  setGridCellFilled (true);
  setGridCellStroked(true);
  setGridCellStrokeColor(Color::makeInterfaceValue(0.1));

  setDataLabelPosition(CQChartsLabelPosition::TOP_OUTSIDE);

  setParetoStrokeColor(Color::makePalette());
  setParetoStrokeWidth(Length::pixel(5));

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
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsScatterPlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

//---

void
CQChartsScatterPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
}

void
CQChartsScatterPlot::
setYColumn(const Column &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged(); } );
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

QString
CQChartsScatterPlot::
columnValueToString(const Column &column, const QVariant &var) const
{
  bool ok;

  if (column == xColumn() && CQChartsVariant::isReal(var))
    return xStr(CQChartsVariant::toReal(var, ok));

  if (column == yColumn() && CQChartsVariant::isReal(var))
    return yStr(CQChartsVariant::toReal(var, ok));

  return CQChartsVariant::toString(var, ok);
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
  CQChartsUtil::testAndSet(plotType_, type, [&]() {
    updateRangeAndObjs(); updateProperties(); Q_EMIT customDataChanged();
  } );
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
setConnected(bool b)
{
  CQChartsUtil::testAndSet(connected_, b, [&]() { updateObjs(); } );
}

void
CQChartsScatterPlot::
setGroupSymbol(bool b)
{
  CQChartsUtil::testAndSet(groupSymbol_, b, [&]() { updateObjs(); } );
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
        densityObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(densityMapLayer()));
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
  if (b != isXDensity()) { xAxisDensity_->setVisible(b); resetExtraFitBBox(); drawObjs(); }
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
  if (b != isYDensity()) { yAxisDensity_->setVisible(b); resetExtraFitBBox(); drawObjs(); }
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
  if (b != isXWhisker()) { xAxisWhisker_->setVisible(b); resetExtraFitBBox(); drawObjs(); }
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
  if (b != isYWhisker()) { yAxisWhisker_->setVisible(b); resetExtraFitBBox(); drawObjs(); }
}

//---

void
CQChartsScatterPlot::
setPareto(bool b)
{
  CQChartsUtil::testAndSet(paretoData_.visible, b, [&]() {
    if (rootPlot()) rootPlot()->updateRootChild(this);

    drawObjs();
  });
}

void
CQChartsScatterPlot::
setParetoOriginType(const ParetoOriginType &t)
{
  CQChartsUtil::testAndSet(paretoData_.originType, t, [&]() {
    drawObjs();
  });
}

void
CQChartsScatterPlot::
setParetoOriginColor(const Color &c)
{
  CQChartsUtil::testAndSet(paretoData_.originColor, c, [&]() {
    drawObjs();
  });
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
  addProp("points", "outlineSelected", "outlineSelected", "Outline selected point");

  addProp("options", "plotType" , "plotType" , "Plot type");

  addProp("filter", "minSymbolSize", "", "Min symbol size");
  addProp("filter", "minLabelSize" , "", "Min label size");

  //---

  // connected
  addProp("connected", "connected", "visible", "Connect points");

  addLineProperties("connected/stroke", "lines", "");

  //---

  addSplitGroupsProperties();

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
  addProp(densityMapPropPath, "densityMapGridSize", "gridSize", "Density map grid size in pixels");
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
  addProp("points", "adjustText", "adjustText", "Adjust text placement");
  addProp("points", "imageAlign", "imageAlign", "Image alignment");

  dataLabel()->addPathProperties("points/labels", "Labels");

  //---

  // grid
  addProp("gridCells", "gridNumX", "nx", "Number of x grid cells");
  addProp("gridCells", "gridNumY", "ny", "Number of y grid cells");

  addStyleProp     ("gridCells/fill"  , "gridCellFilled" , "visible", "Grid cell fill visible");
  addFillProperties("gridCells/fill"  , "gridCellFill"   , "Grid cell");
  addStyleProp     ("gridCells/stroke", "gridCellStroked", "visible", "Grid cell stroke visible");
  addLineProperties("gridCells/stroke", "gridCellStroke" , "Grid cell");

  hideProp(this, "gridCells.stroke.dash");
  hideProp(this, "gridCells.stroke.cap");

  //---

  auto paretoPropPath = QString("overlays/pareto");

  addProp(paretoPropPath, "pareto"           , "visible"    , "Show pareto");
  addProp(paretoPropPath, "paretoOriginType" , "originType" , "Pareto origin type");
  addProp(paretoPropPath, "paretoOriginColor", "originColor", "Pareto origin color");

  addLineProperties(paretoPropPath + "/stroke", "paretoStroke", "Pareto");

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

  //---

  updateProperties();
}

//--

void
CQChartsScatterPlot::
updateProperties()
{
  bool hasGrid = (isGridCells() || isHexCells());

  enableProp(this, "gridCells.nx", hasGrid);
  enableProp(this, "gridCells.ny", hasGrid);

  enableProp(this, "gridCells.fill.visible", hasGrid);
  enableProp(this, "gridCells.fill.color"  , hasGrid);
  enableProp(this, "gridCells.fill.alpha"  , hasGrid);
  enableProp(this, "gridCells.fill.pattern", hasGrid);

  enableProp(this, "gridCells.stroke.visible", hasGrid);
  enableProp(this, "gridCells.stroke.color"  , hasGrid);
  enableProp(this, "gridCells.stroke.alpha"  , hasGrid);
  enableProp(this, "gridCells.stroke.width"  , hasGrid);

  CQChartsPointPlot::updateProperties();
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

bool
CQChartsScatterPlot::
checkColumns()
{
  // check columns
  columnsValid_ = true;

  if (! checkColumn(xColumn(), "X", xColumnType_, /*required*/true))
    columnsValid_ = false;
  if (! checkColumn(yColumn(), "Y", yColumnType_, /*required*/true))
    columnsValid_ = false;

  if (! checkColumn(nameColumn (), "Name" ))
    columnsValid_ = false;
  if (! checkColumn(labelColumn(), "Label"))
    columnsValid_ = false;

  return columnsValid_;
}

//---

CQChartsGeom::Range
CQChartsScatterPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsScatterPlot::calcRange");

  if (! isVisible())
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsScatterPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  if (! th->checkColumns())
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  initGroupData(Columns(), Column());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    using ScatterPlot = CQChartsScatterPlot;

   public:
    RowVisitor(const ScatterPlot *scatterPlot) :
     scatterPlot_(scatterPlot) {
      hasGroups_ = (scatterPlot_->numGroups() > 1);
    }

    bool calcGroupHidden(int groupInd) const {
      return (hasGroups_ && scatterPlot_->isSetHidden(groupInd));
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (scatterPlot_->isInterrupt())
        return State::TERMINATE;

      ModelIndex xModelInd(scatterPlot_, data.row, scatterPlot_->xColumn(), data.parent);
      ModelIndex yModelInd(scatterPlot_, data.row, scatterPlot_->yColumn(), data.parent);

      // init group
      int groupInd = scatterPlot_->rowGroupInd(xModelInd);

      auto updateRange = [&](double x, double y) {
        range_               .updateRange(x, y);
        groupRange_[groupInd].updateRange(x, y);
      };

      bool hidden = calcGroupHidden(groupInd);

      if (! hidden) {
        double x   { 0.0  }, y   { 0.0  };
        bool   okx { true }, oky { true };

        double xDefVal = scatterPlot_->getModelBadValue(xModelInd.column(), data.row);
        double yDefVal = scatterPlot_->getModelBadValue(yModelInd.column(), data.row);

        //---

        if      (scatterPlot_->xColumnType() == ColumnType::REAL ||
                 scatterPlot_->xColumnType() == ColumnType::INTEGER) {
          okx = scatterPlot_->modelMappedReal(xModelInd, x, scatterPlot_->isLogX(), xDefVal);
        }
        else if (scatterPlot_->xColumnType() == ColumnType::TIME) {
          x = scatterPlot_->modelReal(xModelInd, okx);
        }
        else {
          x = uniqueId(data, scatterPlot_->xColumn()); ++numUniqueX_;
          if (x == -1) okx = false;
        }

        //---

        if      (scatterPlot_->yColumnType() == ColumnType::REAL ||
                 scatterPlot_->yColumnType() == ColumnType::INTEGER) {
          oky = scatterPlot_->modelMappedReal(yModelInd, y, scatterPlot_->isLogY(), yDefVal);
        }
        else if (scatterPlot_->yColumnType() == ColumnType::TIME) {
          y = scatterPlot_->modelReal(yModelInd, oky);
        }
        else {
          y = uniqueId(data, scatterPlot_->yColumn()); ++numUniqueY_;
          if (y == -1) oky = false;
        }

        if (scatterPlot_->isPolar()) {
          auto r = x;
          auto a = CMathUtil::Deg2Rad(y); // radians

          x = r*std::cos(a);
          y = r*std::sin(a);
        }

        //---

        // check for bad value
        bool skipBad = false;

        if (scatterPlot_->isSkipBad() && (! okx || ! oky))
          skipBad = true;

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          skipBad = true;

        if (skipBad)
          return State::SKIP;

        updateRange(x, y);
      }

      return State::OK;
    }

    int uniqueId(const VisitData &data, const Column &column) {
      ModelIndex columnInd(scatterPlot_, data.row, column, data.parent);

      bool ok;

      auto var = scatterPlot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const Column &column) {
      if (! details_) {
        auto *modelData = scatterPlot_->currentModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

    const Range &range() const { return range_; }

    const GroupRange &groupRange() const { return groupRange_; }

    bool isUniqueX() const { return numUniqueX_ == numRows(); }
    bool isUniqueY() const { return numUniqueY_ == numRows(); }

   private:
    const ScatterPlot*    scatterPlot_ { nullptr };
    int                   hasGroups_   { false };
    Range                 range_;
    GroupRange            groupRange_;
    CQChartsModelDetails* details_     { nullptr };
    int                   numUniqueX_  { 0 };
    int                   numUniqueY_  { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  Range dataRange;

  if (isSplitGroups()) {
    int ng = numVisibleGroups();

    dataRange = Range(0.0, 0.0, ng, 1.0);

    th->range_      = visitor.range();
    th->groupRange_ = visitor.groupRange();
  }
  else {
    dataRange = visitor.range();

    auto *xDetails = this->columnDetails(xColumn());
    auto *yDetails = this->columnDetails(yColumn());

    if (xDetails && xDetails->type() == ColumnType::REAL) {
      auto y = dataRange.ymid();

      bool ok;
      auto x1 = xDetails->minValue().toReal(&ok);
      auto x2 = xDetails->maxValue().toReal(&ok);

      dataRange.updateRange(x1, y);
      dataRange.updateRange(x2, y);
    }

    if (yDetails && yDetails->type() == ColumnType::REAL) {
      auto x = dataRange.xmid();

      bool ok;
      auto y1 = yDetails->minValue().toReal(&ok);
      auto y2 = yDetails->maxValue().toReal(&ok);

      dataRange.updateRange(x, y1);
      dataRange.updateRange(x, y2);
    }
  }

  if (isPolar())
    dataRange.updateRange(0.0, 0.0);

  //---

  th->uniqueX_ = visitor.isUniqueX();
  th->uniqueY_ = visitor.isUniqueY();

  //---

  // if unique values (string values) then adjust range
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

  // update data range if unset
  dataRange.makeNonZero();

  //---

  if (isGridCells()) {
    th->initGridData(dataRange);

    //---

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
  if (! currentModelData()) {
    xAxis()->setDefLabel("", /*notify*/false);
    yAxis()->setDefLabel("", /*notify*/false);

    xAxis()->clearTickLabels();
    yAxis()->clearTickLabels();

    return;
  }

  updateAxes();
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
updateAxes()
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

  xAxis()->setDefLabel("", /*notify*/false);
  yAxis()->setDefLabel("", /*notify*/false);

  //---

  // set x axis column
  Column xAxisColumn = xColumn();

  if (! isSplitGroups()) {
    xAxis()->setColumn(xAxisColumn);
  }
  else {
    xAxis()->setColumn(Column());
  }

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

  if (! isSplitGroups()) {
    auto xType = xAxis()->valueType();

    if (xType != CQChartsAxisValueType::integer() && xType != CQChartsAxisValueType::real())
      xType = CQChartsAxisValueType::real();

    if (isLogX   ()) xType = CQChartsAxisValueType::log();
    if (isUniqueX()) xType = CQChartsAxisValueType::integer();

    if (xColumnType_ == ColumnType::TIME)
      xType = CQChartsAxisValueType::date();

    xAxis()->setValueType(CQChartsAxisValueType(xType), /*notify*/false);
  }
  else {
    xAxis()->setValueType(CQChartsAxisValueType::integer(), /*notify*/false);
  }

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

  auto yType = yAxis()->valueType();

  if (yType != CQChartsAxisValueType::integer() && yType != CQChartsAxisValueType::real())
    yType = CQChartsAxisValueType::real();

  if (isLogY   ()) yType = CQChartsAxisValueType::log();
  if (isUniqueY()) yType = CQChartsAxisValueType::integer();

  if (yColumnType_ == ColumnType::TIME)
    yType = CQChartsAxisValueType::date();

  yAxis()->setValueType(CQChartsAxisValueType(yType), /*notify*/false);

  //---

  if (isOverlay() && isFirstPlot())
    setOverlayPlotsAxisNames();

  //---

  // set x axis labels
  xAxis()->clearTickLabels();

  if (! isSplitGroups()) {
    if (isUniqueX()) {
      auto *columnDetails = this->columnDetails(xColumn());

      for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i) {
        bool ok;
        xAxis()->setTickLabel(i, CQChartsVariant::toString(columnDetails->uniqueValue(i), ok));
      }
    }
  }

  //---

  yAxis()->clearTickLabels();

  if (! isSplitGroups()) {
    if (isUniqueY()) {
      auto *columnDetails = this->columnDetails(yColumn());

      for (int i = 0; columnDetails && i < columnDetails->numUnique(); ++i) {
        bool ok;
        yAxis()->setTickLabel(i, CQChartsVariant::toString(columnDetails->uniqueValue(i), ok));
      }
    }
  }

  //---

  auto numXAxes = size_t(isSplitGroups() ? numVisibleGroups() : 0);

  CQChartsUtil::makeArraySize(xaxes_, numXAxes, [&]() {
    auto *axis = new CQChartsAxis(this, Qt::Horizontal, 0, 1);

    axis->moveToThread(this->thread());

    axis->setParent(this);
    axis->setPlot  (this);

    axis->setUpdatesEnabled(false);

    return axis;
  });

  auto numYAxes = size_t(isSplitGroups() && ! isSplitSharedY() ? numXAxes : 0);

  CQChartsUtil::makeArraySize(yaxes_, numYAxes, [&]() {
    auto *axis = new CQChartsAxis(this, Qt::Vertical, 0, 1);

    axis->moveToThread(this->thread());

    axis->setParent(this);
    axis->setPlot  (this);

    axis->setUpdatesEnabled(false);

    return axis;
  });

  //---

  auto sm = splitMargin();

  for (size_t i = 0; i < xaxes_.size(); ++i) {
    auto *xaxis = xaxes_[i];

    int ig = unmapVisibleGroup(int(i));

    //---

    xaxis->setColumn(xAxisColumn);

    auto groupName = groupIndName(ig);

    xaxis->setDefLabel(groupName, /*notify*/false);

    //---

    // set range
    const auto &range = getGroupRange(ig);

    auto xi = double(i);

    double xmin = (i >            0 ? xi + sm       : xi      );
    double xmax = (i < numXAxes - 1 ? xi + 1.0 - sm : xi + 1.0);

    xaxis->setRange(xmin, xmax);

    xaxis->setValueRange(range.xmin(), range.xmax());
  }

  //---

  auto *masterYAxis = yAxis();

  for (size_t i = 0; i < yaxes_.size(); ++i) {
    auto *yaxis = yaxes_[i];

    int ig = unmapVisibleGroup(int(i));

    //---

    // set range
    const auto &range = getGroupRange(ig);

    yaxis->setRange(0.0, 1.0);

    double ymin = range.ymin();

    if (masterYAxis->isIncludeZero())
      ymin = std::min(ymin, 0.0);

    yaxis->setValueRange(ymin, range.ymax());
  }

  //---

  if (isSplitGroups()) {
    double ymin = range_.ymin();

    if (masterYAxis->isIncludeZero())
      ymin = std::min(ymin, 0.0);

    yAxis()->setValueRange(ymin, range_.ymax());
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

bool
CQChartsScatterPlot::
isXAxisVisible() const
{
  if (isSplitGroups())
    return false;

  return CQChartsPlot::isXAxisVisible();
}

//------

CQChartsGeom::BBox
CQChartsScatterPlot::
dataFitBBox() const
{
  BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    if (! plotObj->isVisible())
      continue;

    if (plotObj->isFiltered())
      continue;

    bbox += plotObj->rect();
  }

  return bbox;
}

//------

void
CQChartsScatterPlot::
clearPlotObjects()
{
  groupInds_        .clear();
  groupNameValues_  .clear();
  groupNameGridData_.clear();

  for (auto &pg : groupNameHexData_) {
    for (auto &pn : pg.second) {
      auto *hexMap = reinterpret_cast<HexMap *>(pn.second);

      delete hexMap;
    }
  }

  groupNameHexData_.clear();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsScatterPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsScatterPlot::createObjs");

  if (! isVisible())
    return true;

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsScatterPlot *>(this);

  //---

  // init name values
  th->gridData_.setMaxN(0);

  if (th->hexMap_) {
    auto *hexMap = reinterpret_cast<HexMap *>(th->hexMap_);

    hexMap->clear();

    th->hexMapMaxN_ = 0;
  }

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

  // plot types (one of)
  if      (isSymbols())
    addPointObjects(objs);
  else if (isGridCells())
    addGridObjects(objs);
  else if (isHexCells())
    addHexObjects(objs);

  //---

  // overlays
  if (isConnected())
    addConnectedObjects(objs);

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

  const auto &model = this->currentModel();

  columnTypeMgr->startCache(model.data());

  //---

  initSymbolTypeData();
  initSymbolSizeData();
  initFontSizeData  ();

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = int(groupInds_.size());

  if (! hasGroups) {
    if (ng <= 1 && parentPlot()) {
      ig = parentPlot()->childPlotIndex(this);
      ng = parentPlot()->numChildPlots();
    }
  }

  //---

  auto calcSetHidden = [&](int setInd) {
    return (! hasGroups && isSetHidden(setInd));
  };

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  //---

  // process each group
  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      break;

    int         groupInd   = groupNameValue.first;
    const auto &nameValues = groupNameValue.second;

    //---

    bool hidden = calcGroupHidden(groupInd);

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto pg = th->groupPoints_.find(groupInd);

    if (pg == th->groupPoints_.end())
      pg = th->groupPoints_.emplace_hint(pg, groupInd, Points());

    auto &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = int(nameValues.size());

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        break;

      bool hidden = calcSetHidden(is);

      if (hidden) { ++is; continue; }

      //---

    //const auto &name   = nameValue.first;
      const auto &values = nameValue.second.values;

      auto nv = values.size();

      for (size_t iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          break;

        //---

        // get point position
        const auto &valuePoint = values[iv];

        const auto &p = valuePoint.p;

        //---

        // get symbol size (needed for bounding box)
        Length          symbolSize;
        Qt::Orientation symbolSizeDir { Qt::Horizontal };

        if (symbolSizeColumn().isValid()) {
          if (! columnSymbolSize(valuePoint.row, valuePoint.ind.parent(), symbolSize,
                                 symbolSizeDir))
            symbolSize = Length();
        }

        auto symbolSize1 = symbolSize;

        if (! symbolSize1.isValid())
          symbolSize1 = this->symbolSize();

        double sx, sy;

        plotSymbolSize(symbolSize1, sx, sy, symbolSizeDir);

        //---

        // create point object
        auto is1 = ColorInd(is, ns);
        auto ig1 = ColorInd(ig, ng);
        auto iv1 = ColorInd(int(iv), int(nv));

        auto gp = adjustGroupPoint(groupInd, p);

        BBox gbbox(gp.x - sx, gp.y - sy, gp.x + sx, gp.y + sy);

        auto *pointObj = createPointObj(groupInd, gbbox, gp, is1, ig1, iv1);

        pointObj->connectDataChanged(this, SLOT(updateSlot()));

        if (valuePoint.ind.isValid())
          pointObj->setModelInd(valuePoint.ind);

        if (symbolSize.isValid()) {
          pointObj->setSymbolSize(symbolSize);
          pointObj->setSymbolDir (symbolSizeDir);
        }

        objs.push_back(pointObj);

        points.push_back(gp);

        //---

        if (! symbolSizeVisible(symbolSize))
          pointObj->setFiltered(true);

        //---

        // set optional symbol
        Symbol symbol;

        if (symbolTypeColumn().isValid()) {
          if (! columnSymbolType(valuePoint.row, valuePoint.ind.parent(), symbol))
            symbol = Symbol();
        }

        if (! symbol.isValid() && isGroupSymbol() && ng > 1) {
          auto *symbolSet = defaultSymbolSet();

          symbol = symbolSet->interpI(ig).symbol;
        }

        if (symbol.isValid()) {
          pointObj->setSymbol(symbol);

          if (! symbolTypeVisible(symbol))
            pointObj->setFiltered(true);
        }

        //---

        // set optional font size
        Length          fontSize;
        Qt::Orientation fontSizeDir { Qt::Horizontal };

        if (fontSizeColumn().isValid()) {
          if (! columnFontSize(valuePoint.row, valuePoint.ind.parent(), fontSize, fontSizeDir))
            fontSize = Length();
        }

        if (fontSize.isValid()) {
          pointObj->setFontSize(fontSize);
          pointObj->setLabelDir(fontSizeDir);
        }

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

        if (symbolColor.isValid()) {
          auto c = interpColor(symbolColor, ColorInd());

          if (! colorVisible(c))
            pointObj->setFiltered(true);

          pointObj->setColor(symbolColor);
        }

        //---

        // set optional symbol fill alpha
        Alpha symbolAlpha;

        if (alphaColumn().isValid()) {
          if (! alphaColumnAlpha(valuePoint.row, valuePoint.ind.parent(), symbolAlpha))
            symbolAlpha = Alpha();
        }

        if (symbolAlpha.isSet())
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

  columnTypeMgr->endCache(model.data());

  //---

  std::sort(objs.begin(), objs.end(), [](const PlotObj *lhs, const PlotObj *rhs) {
    return lhs->rect().area() > rhs->rect().area();
  });
}

void
CQChartsScatterPlot::
addGridObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  auto calcSetHidden = [&](int setInd) {
    return (! hasGroups && isSetHidden(setInd));
  };

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  //---

  int maxN = gridData_.maxN();

  int ig = 0;
  int ng = int(groupNameGridData_.size());

  for (const auto &pg : groupNameGridData_) {
    if (isInterrupt())
      break;

    int         groupInd     = pg.first;
    const auto &nameGridData = pg.second;

    //---

    bool hidden = calcGroupHidden(groupInd);

    if (hidden) { ++ig; continue; }

    //---

    int is = 0;
    int ns = int(nameGridData.size());

    for (const auto &pn : nameGridData) {
      if (isInterrupt())
        break;

      bool hidden = calcSetHidden(is);

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

          BBox bbox(xmin, ymin, xmax, ymax); // already adjusted

          auto *cellObj = createCellObj(groupInd, bbox, is1, ig1, ix, iy, points, maxN);

          cellObj->connectDataChanged(this, SLOT(updateSlot()));

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

  auto calcSetHidden = [&](int setInd) {
    return (! hasGroups && isSetHidden(setInd));
  };

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  //---

  int maxN = hexMapMaxN_;

  int ig = 0;
  int ng = int(groupNameHexData_.size());

  for (const auto &pg : groupNameHexData_) {
    if (isInterrupt())
      break;

    int         groupInd    = pg.first;
    const auto &nameHexData = pg.second;

    //---

    bool hidden = calcGroupHidden(groupInd);

    if (hidden) { ++ig; continue; }

    //---

    int is = 0;
    int ns = int(nameHexData.size());

    for (const auto &pn : nameHexData) {
      if (isInterrupt())
        break;

      bool hidden = calcSetHidden(is);

      if (hidden) { ++is; continue; }

      //---

      auto *hexMap = reinterpret_cast<HexMap *>(pn.second);

    //int maxN = hexMap->numData();

      for (const auto &px : hexMap->data()) {
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

          int n = int(dataArray.size());

          HexMap::Polygon ipolygon;

          hexMap->indexPolygon(i, j, ipolygon);

          Polygon polygon;

          for (auto &p : ipolygon) {
            Point pv(p.x, p.y);

            auto pw = viewToWindow(pv);

            polygon.addPoint(pw);
          }

          //---

          ColorInd is1(is, ns);
          ColorInd ig1(ig, ng);

          auto bbox = polygon.boundingBox(); // already adjusted

          auto *hexObj = createHexObj(groupInd, bbox, is1, ig1, i, j, polygon, n, maxN);

          hexObj->connectDataChanged(this, SLOT(updateSlot()));

          objs.push_back(hexObj);
        }
      }

      ++is;
    }

    ++ig;
  }
}

//---

void
CQChartsScatterPlot::
addConnectedObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  auto addConnectedObj = [&](int groupInd, const QString &name, const ColorInd &ig,
                             const ColorInd &is, const BBox &bbox) {
    auto *connectedObj = createConnectedObj(groupInd, name, ig, is, bbox);

    connectedObj->connectDataChanged(this, SLOT(updateSlot()));

    //connectedObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(connectedLayer()));

    objs.push_back(connectedObj);
  };

  //---

  // one connected line per group (multiple groups) or set (name values)
  int ng = int(groupInds_.size());

  if (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      bool hidden = calcGroupHidden(groupInd);
      if (hidden) continue;

      auto bbox = drawRange(groupInd);

      auto gbbox = adjustGroupBBox(groupInd, bbox);

      addConnectedObj(groupInd, "", ColorInd(ig, ng), ColorInd(), gbbox);

      ++ig;
    }
  }
  else {
    auto bbox = calcDataRange(/*adjust*/false);

    const auto &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = int(nameValues.size());

    for (const auto &nameValue : nameValues) {
      addConnectedObj(-1, nameValue.first, ColorInd(), ColorInd(is, ns), bbox);

      ++is;
    }
  }
}

CQChartsScatterConnectedObj *
CQChartsScatterPlot::
createConnectedObj(int groupInd, const QString &name, const ColorInd &ig, const ColorInd &is,
                   const BBox &rect) const
{
  return new CQChartsScatterConnectedObj(this, groupInd, name, ig, is, rect);
}

//---

void
CQChartsScatterPlot::
addBestFitObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  //---

  // one best fit per group (multiple groups) or set (name values)
  int ng = int(groupInds_.size());

  if (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      bool hidden = calcGroupHidden(groupInd);
      if (hidden) continue;

      auto bbox = drawRange(groupInd);

      auto gbbox = adjustGroupBBox(groupInd, bbox);

      auto *bestFitObj = createBestFitObj(groupInd, "", ColorInd(ig, ng), ColorInd(), gbbox);

      bestFitObj->connectDataChanged(this, SLOT(updateSlot()));

      bestFitObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(bestFitLayer()));

      objs.push_back(bestFitObj);

      ++ig;
    }
  }
  else {
    auto bbox = calcDataRange(/*adjust*/false);

    const auto &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = int(nameValues.size());

    for (const auto &nameValue : nameValues) {
      auto *bestFitObj = createBestFitObj(-1, nameValue.first, ColorInd(), ColorInd(is, ns), bbox);

      bestFitObj->connectDataChanged(this, SLOT(updateSlot()));

      bestFitObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(bestFitLayer()));

      objs.push_back(bestFitObj);

      ++is;
    }
  }
}

//---

void
CQChartsScatterPlot::
addHullObjects(PlotObjs &objs) const
{
  int hasGroups = (numGroups() > 1);

  //---

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  auto addHullObj = [&](int groupInd, const QString &name, const ColorInd &ig,
                        const ColorInd &is, const BBox &bbox) {
    auto *hullObj = createHullObj(groupInd, name, ig, is, bbox);

    hullObj->connectDataChanged(this, SLOT(updateSlot()));

    hullObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(hullLayer()));

    objs.push_back(hullObj);
  };

  //---

  // one hull per group (multiple groups) or set (name values)
  int ng = int(groupInds_.size());

  if (ng > 1) {
    int ig = 0;

    for (const auto &groupInd : groupInds_) {
      bool hidden = calcGroupHidden(groupInd);
      if (hidden) continue;

      auto bbox = drawRange(groupInd);

      auto gbbox = adjustGroupBBox(groupInd, bbox);

      addHullObj(groupInd, "", ColorInd(ig, ng), ColorInd(), gbbox);

      ++ig;
    }
  }
  else {
    auto bbox = calcDataRange(/*adjust*/false);

    const auto &nameValues = (*groupNameValues_.begin()).second;

    int is = 0;
    int ns = int(nameValues.size());

    for (const auto &nameValue : nameValues) {
      auto &name = nameValue.first;

      addHullObj(-1, name, ColorInd(), ColorInd(is, ns), bbox);

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

  //---

  auto calcGroupHidden = [&](int groupInd) {
    return (hasGroups && isSetHidden(groupInd));
  };

  //---

  // one map per group
  for (const auto &pg : groupNameValues_) {
    int         groupInd   = pg.first;
    const auto &nameValues = pg.second;

    bool hidden = calcGroupHidden(groupInd);
    if (hidden) continue;

    auto bbox = drawRange(groupInd);

    auto gbbox = adjustGroupBBox(groupInd, bbox);

    for (const auto &pn : nameValues) {
      const auto &name = pn.first;

      auto *densityObj = createDensityObj(groupInd, name, gbbox);

      densityObj->connectDataChanged(this, SLOT(updateSlot()));

      densityObj->setDrawLayer(static_cast<CQChartsPlotObj::DrawLayer>(densityMapLayer()));

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

    pd = th->groupNamedDensity_.emplace_hint(pd, groupInd, namedDensity);

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

CQChartsGeom::BBox
CQChartsScatterPlot::
drawRange(int groupInd) const
{
  if (isSplitGroups()) {
    const auto &range = getGroupRange(groupInd);

    return range.bbox();
  }
  else {
    return calcDataRange(/*adjust*/false);
  }
}

//---

#if 0
bool
CQChartsScatterPlot::
calcSetHidden(int setInd) const

  return (! numGroups() && isSetHidden(groupInd));
}
#endif

bool
CQChartsScatterPlot::
calcGroupHidden(int groupInd) const
{
  return (numGroups() && isSetHidden(groupInd));
}

int
CQChartsScatterPlot::
numVisibleGroups() const
{
  int ng = numGroups();
  int nv = ng;

  for (int i = 0; i < ng; ++i) {
    if (calcGroupHidden(i))
      --nv;
  }

  return nv;
}

int
CQChartsScatterPlot::
mapVisibleGroup(int groupInd) const
{
  int ng = numGroups();
  int ig = 0;

  for (int i = 0; i < ng; ++i) {
    bool hidden = calcGroupHidden(i);

    if (i == groupInd)
      return (! hidden ? ig : -1);

    if (! hidden)
      ++ig;
  }

  return -1;
}

int
CQChartsScatterPlot::
unmapVisibleGroup(int groupInd) const
{
  int ng = numGroups();
  int ig = 0;

  for (int i = 0; i < ng; ++i) {
    bool hidden = calcGroupHidden(i);

    if (! hidden) {
      if (groupInd == ig)
        return i;

      ++ig;
    }
  }

  return -1;
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

#if 0
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  th->clearSkipColors();
#endif

  class RowVisitor : public ModelVisitor {
   public:
    using ScatterPlot = CQChartsScatterPlot;

   public:
    RowVisitor(const ScatterPlot *scatterPlot) :
     scatterPlot_(scatterPlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<ScatterPlot *>(scatterPlot_);

      //---

      ModelIndex xModelInd(scatterPlot_, data.row, scatterPlot_->xColumn(), data.parent);
      ModelIndex yModelInd(scatterPlot_, data.row, scatterPlot_->yColumn(), data.parent);

      // get group
      int groupInd = scatterPlot_->rowGroupInd(xModelInd);

      //---

      // get x, y value
      auto xInd  = scatterPlot_->modelIndex(xModelInd);
      auto xInd1 = scatterPlot_->normalizeIndex(xInd);

      double x   { 0.0  }, y   { 0.0  };
      bool   okx { true }, oky { true };

      double xDefVal = scatterPlot_->getModelBadValue(xModelInd.column(), data.row);
      double yDefVal = scatterPlot_->getModelBadValue(yModelInd.column(), data.row);

      //---

      if      (scatterPlot_->xColumnType() == ColumnType::REAL ||
               scatterPlot_->xColumnType() == ColumnType::INTEGER) {
        okx = scatterPlot_->modelMappedReal(xModelInd, x, scatterPlot_->isLogX(), xDefVal);
      }
      else if (scatterPlot_->xColumnType() == ColumnType::TIME) {
        x = scatterPlot_->modelReal(xModelInd, okx);
      }
      else {
        x = uniqueId(data, scatterPlot_->xColumn());
      }

      //---

      if      (scatterPlot_->yColumnType() == ColumnType::REAL ||
               scatterPlot_->yColumnType() == ColumnType::INTEGER) {
        oky = scatterPlot_->modelMappedReal(yModelInd, y, scatterPlot_->isLogY(), yDefVal);
      }
      else if (scatterPlot_->yColumnType() == ColumnType::TIME) {
        y = scatterPlot_->modelReal(yModelInd, oky);
      }
      else {
        y = uniqueId(data, scatterPlot_->yColumn());
      }

      //---

      if (scatterPlot_->isPolar()) {
        auto r = x;
        auto a = CMathUtil::Deg2Rad(y); // radians

        x = r*std::cos(a);
        y = r*std::sin(a);
      }

      //---

      // check for bad value
      bool skipBad = false;

      if (scatterPlot_->isSkipBad() && (! okx || ! oky))
        skipBad = true;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        skipBad = true;

      if (skipBad)
        return State::SKIP;

      //---

      Point p(x, y);

      //---

      // get optional grouping name (name column, title, x axis)
      QString name;

      if (scatterPlot_->nameColumn().isValid()) {
        ModelIndex nameColumnInd(scatterPlot_, data.row, scatterPlot_->nameColumn(), data.parent);

        bool ok;

        name = scatterPlot_->modelString(nameColumnInd, ok);
      }

      if (! name.length() && scatterPlot_->title())
        name = scatterPlot_->title()->textStr();

      if (! name.length() && scatterPlot_->xAxis())
        name = scatterPlot_->xAxis()->label().string();

      //---

      // get symbol type, size, font size and color
      Color color;

      // get color label (needed if not string ?)
      if (scatterPlot_->colorColumn().isValid()) {
        (void) scatterPlot_->colorColumnColor(data.row, data.parent, color);

#if 0
        if (skipBad) {
          ModelIndex colorColumnInd(scatterPlot_, data.row, scatterPlot_->colorColumn(),
                                    data.parent);

          bool ok;
          auto var = scatterPlot_->modelValue(colorColumnInd, ok);

          if (ok && var.isValid())
            plot->addSkipColor(var, data.row);
        }

        if (skipBad)
          return State::SKIP;
#endif
      }

      //---

      plot->addNameValue(groupInd, name, p, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const Column &column) {
      ModelIndex columnInd(scatterPlot_, data.row, column, data.parent);

      bool ok;

      auto var = scatterPlot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const Column &column) {
      if (! details_) {
        auto *modelData = scatterPlot_->currentModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

   private:
    const ScatterPlot*    scatterPlot_ { nullptr };
    CQChartsModelDetails* details_     { nullptr };
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
      pi = groupNameGridData_.emplace_hint(pi, groupInd, NameGridData());

    auto &nameGridData = (*pi).second;

    auto pn = nameGridData.find(name);

    if (pn == nameGridData.end())
      pn = nameGridData.emplace_hint(pn, name, gridData_);

    auto &cellPointData = (*pn).second;

    auto gp = adjustGroupPoint(groupInd, p);

    cellPointData.addPoint(gp);

    gridData_.setMaxN(std::max(gridData_.maxN(), cellPointData.maxN()));
  }

  //---

  // add value to hex data if enabled
  if (isHexCells()) {
    auto pi = groupNameHexData_.find(groupInd);

    if (pi == groupNameHexData_.end())
      pi = groupNameHexData_.emplace_hint(pi, groupInd, NameHexData());

    auto &nameHexData = (*pi).second;

    auto pn = nameHexData.find(name);

    if (pn == nameHexData.end()) {
      auto *hexMap = new HexMap;

      const auto &viewBBox = this->viewBBox();

      hexMap->setRange(viewBBox.getXMin(), viewBBox.getYMin(),
                       viewBBox.getXMax(), viewBBox.getYMax());

      hexMap->setNum(gridData_.nx());

      pn = nameHexData.emplace_hint(pn, name, hexMap);
    }

    auto gp = adjustGroupPoint(groupInd, p);

    auto pv = windowToView(gp);

    auto *hexMap = reinterpret_cast<HexMap *>((*pn).second);

    HexMap::Point hp(pv.x, pv.y);

    hexMap->addPoint(hp);

    int hi, hj;

    hexMap->pointToPos(hp, hi, hj);

    hexMapMaxN_ = std::max(hexMapMaxN_, hexMap->numData(hi, hj));
  }

  //---

  // add value to values data if symbol or density map enabled (always ?)
  if (isSymbols() || isDensityMap()) {
    auto pi = groupNameValues_.find(groupInd);

    if (pi == groupNameValues_.end())
      pi = groupNameValues_.emplace_hint(pi, groupInd, NameValues());

    auto &nameValues = (*pi).second;

    auto pn = nameValues.find(name);

    if (pn == nameValues.end())
      pn = nameValues.emplace_hint(pn, name, ValuesData());

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

    auto *colorItem = new CQChartsScatterColorKeyItem(this, groupInd, is, ig);
    auto *textItem  = new CQChartsTextKeyItem        (this, name, ic);

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(colorItem, textItem);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);

    return colorItem;
  };

  //---

  int ng = int(groupInds_.size());

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

      int ns = int(nameValues.size());

      if (ns > 1) {
        int is = 0;

        for (const auto &nameValue : nameValues) {
          const auto &name = nameValue.first;

          auto *colorItem = addKeyItem(-2, name, ColorInd(is, ns), ColorInd());

          //--

          if (colorColumn().isValid()) {
            const auto &values = nameValue.second.values;

            int nv = int(values.size());

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

QString
CQChartsScatterPlot::
posStr(const Point &w) const
{
  if (isSplitGroups()) {
    int ng = numVisibleGroups();

    auto groupInd = std::min(std::max(int(w.x), 0), ng - 1);

    const auto &range = getGroupRange(groupInd);

    double x = CMathUtil::map(w.x, groupInd, groupInd + 1, range.xmin(), range.xmax());
    double y = CMathUtil::map(w.y, 0.0, 1.0, range_.ymin(), range_.ymax());

    return CQChartsPlot::posStr(Point(x, y));
  }

  return CQChartsPlot::posStr(w);
}

//---

bool
CQChartsScatterPlot::
addMenuItems(QMenu *menu, const Point &)
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
                              isBestFit   (), SLOT(setBestFit   (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Hull",
                              isHull      (), SLOT(setHull      (bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Stats Lines",
                              isStatsLines(), SLOT(setStatsLines(bool)));
  (void) addMenuCheckedAction(overlaysMenu, "Density Map",
                              isDensityMap(), SLOT(setDensityMap(bool)));

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

  //---

  addRootMenuItems(menu);

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

  if (isSymbols() && paretoOriginType() != ParetoOriginType::NONE)
    return true;

  return false;
}

void
CQChartsScatterPlot::
execDrawBackground(PaintDevice *device) const
{
  CQChartsPlot::execDrawBackground(device);

  if (isSymbols())
    drawParetoDir(device);

  // draw stats lines on background
  if (isStatsLines())
    drawStatsLines(device);

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

//if (isPareto() && isSymbols())
//  return true;

  return true;
}

void
CQChartsScatterPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isPareto() && isSymbols())
    drawPareto(device);

  CQChartsPointPlot::execDrawForeground(device);
}

//---

CQChartsGeom::BBox
CQChartsScatterPlot::
axesFitBBox() const
{
  if (! isSplitGroups()) {
    return CQChartsPlot::axesFitBBox();
  }
  else {
    BBox bbox;

    for (auto *xaxis : xaxes_)
      bbox += xaxis->fitBBox();

    if (! isSplitSharedY()) {
      for (auto *yaxis : yaxes_)
        bbox += yaxis->fitBBox();
    }
    else {
      if (isYAxisVisible())
        bbox += yAxis()->fitBBox();
    }

    return bbox;
  }
}

//---

bool
CQChartsScatterPlot::
hasFgAxes() const
{
  return true;
}

void
CQChartsScatterPlot::
drawFgAxes(PaintDevice *device) const
{
  if (! isSplitGroups()) {
    CQChartsPlot::drawFgAxes(device);
  }
  else {
    //auto *th = const_cast<CQChartsScatterPlot *>(this);

    auto *masterXAxis = xAxis();

    for (auto *xaxis : xaxes_) {
      xaxis->setAxesLineData         (masterXAxis->axesLineData());
      xaxis->setAxesTickLabelTextData(masterXAxis->axesTickLabelTextData());
      xaxis->setAxesLabelTextData    (masterXAxis->axesLabelTextData());
      xaxis->setAxesMajorGridLineData(masterXAxis->axesMajorGridLineData());
      xaxis->setAxesMinorGridLineData(masterXAxis->axesMinorGridLineData());
      xaxis->setAxesGridFillData     (masterXAxis->axesGridFillData());

      xaxis->draw(this, device);
    }

    if (! isSplitSharedY()) {
      auto *masterYAxis = yAxis();

      int iy = 0;

      auto sm = splitMargin();

      for (auto *yaxis : yaxes_) {
        auto y = (iy == 0 ? iy : iy + sm);

        yaxis->setPosition(CQChartsOptReal(y));

        yaxis->setAxesLineData         (masterYAxis->axesLineData());
        yaxis->setAxesTickLabelTextData(masterYAxis->axesTickLabelTextData());
        yaxis->setAxesLabelTextData    (masterYAxis->axesLabelTextData());
        yaxis->setAxesMajorGridLineData(masterYAxis->axesMajorGridLineData());
        yaxis->setAxesMinorGridLineData(masterYAxis->axesMinorGridLineData());
        yaxis->setAxesGridFillData     (masterYAxis->axesGridFillData());

        yaxis->draw(this, device);

        ++iy;
      }
    }
    else
      drawFgYAxis(device);
  }
}

//---

void
CQChartsScatterPlot::
drawBackgroundRect(PaintDevice *device, const DrawRegion &drawRegion, const BBox &rect,
                   const BrushData &brushData, const PenData &penData, const Sides &sides) const
{
  if (! isSplitGroups() || drawRegion != DrawRegion::DATA)
    return CQChartsPlot::drawBackgroundRect(device, drawRegion, rect, brushData, penData, sides);

  //---

  int ng = numVisibleGroups();

//auto xmin = rect.getXMin();
  auto ymin = rect.getYMin();
//auto xmax = rect.getXMax();
  auto ymax = rect.getYMax();

  for (int i = 0; i < ng; ++i) {
    //auto pg = groupRange_.find(i);
    //assert(pg != groupRange_.end());

    //const auto &range = (*pg).second;

    auto sm = splitMargin();

    auto xmin1 = (i == 0      ? i     : i + sm      );
    auto xmax1 = (i == ng - 1 ? i + 1 : i + 1.0 - sm);

    auto rect1 = BBox(xmin1, ymin, xmax1, ymax);

    CQChartsPlot::drawBackgroundRect(device, drawRegion, rect1, brushData, penData, sides);
  }
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
    int ng = int(groupInds_.size());

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
    int ng = int(groupInds_.size());

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
    int ng = int(groupInds_.size());

    if (isYWhisker() && yAxisWhisker_->side().type() == side)
      addWidth(yAxisWhisker_->calcNDeltaBBox(ng, 0.0));
  }

  return w;
}

//------

void
CQChartsScatterPlot::
preDrawObjs(PaintDevice *) const
{
  auto *th = const_cast<CQChartsScatterPlot *>(this);

  th->dataLabelDatas_.clear();

  if (isAdjustText())
    placer_->clear();
}

void
CQChartsScatterPlot::
postDrawObjs(PaintDevice *device) const
{
  drawDataLabelDatas(device);

  if (isAdjustText()) {
    auto rect = this->calcDataRect();

    placer_->place(rect);

    placer_->draw(device);
  }
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
    addHeight(xAxisWhisker_->calcNDeltaBBox(int(groupInds_.size()), 0.0));

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
    addWidth(yAxisWhisker_->calcNDeltaBBox(int(groupInds_.size()), 0.0));

  CQChartsPlot::drawYAxisAt(device, plot, pos);
}

//---

void
CQChartsScatterPlot::
drawStatsLines(PaintDevice *device) const
{
  int ng = int(groupInds_.size());

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

    int ns = int(nameValues.size());

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

  bool updateState = device->isInteractive();

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

    setStatsPenBrush(penBrush, ic);

    if (updateState)
      updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::BOX);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    // draw lines
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

    auto fillRect = [&](const BBox &bbox, double f=1.0) {
      double alpha = CQChartsDrawUtil::brushAlpha(penBrush.brush);
      CQChartsDrawUtil::setBrushAlpha(penBrush.brush, alpha*f);
      device->fillRect(bbox);
      CQChartsDrawUtil::setBrushAlpha(penBrush.brush, alpha);
    };

    //--

    fillRect(BBox(statData.xstat.loutlier, statData.ystat.loutlier,
                  statData.xstat.uoutlier, statData.ystat.uoutlier), 0.3);
    fillRect(BBox(statData.xstat.loutlier, statData.ystat.lowerMedian,
                  statData.xstat.uoutlier, statData.ystat.upperMedian), 0.5);
    fillRect(BBox(statData.xstat.lowerMedian, statData.ystat.loutlier,
                  statData.xstat.upperMedian, statData.ystat.uoutlier), 0.5);
    fillRect(BBox(statData.xstat.lowerMedian, statData.ystat.lowerMedian,
                  statData.xstat.upperMedian, statData.ystat.upperMedian));

    //--

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

//---

void
CQChartsScatterPlot::
drawParetoDir(PaintDevice *device) const
{
  if (paretoOriginType() == ParetoOriginType::NONE)
    return;

  //---

  const auto &dataRange = this->dataRange();

  auto *xDetails = (xColumn().isValid() ? columnDetails(xColumn()) : nullptr);
  auto *yDetails = (yColumn().isValid() ? columnDetails(yColumn()) : nullptr);

  bool invX = (xDetails ? xDetails->decreasing().toBool() : false);
  bool invY = (yDetails ? yDetails->decreasing().toBool() : false);

  auto origin = Point((invX ? dataRange.xmax() : dataRange.xmin()),
                      (invY ? dataRange.ymax() : dataRange.ymin()));

  //---

  auto originColor = interpColor(paretoOriginColor(), ColorInd());

  // draw origin symbol
  if     (paretoOriginType() == ParetoOriginType::SYMBOL) {
    PenBrush penBrush;

    setPenBrush(penBrush,
      PenData(true, originColor, Alpha(), paretoStrokeWidth()),
      BrushData(true, originColor));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    auto ss = 4.0;

    auto symbolSize = Length::pixel(ss);

    double sx, sy;
    pixelSymbolSize(symbolSize, sx, sy, /*scale*/false);

    drawSymbol(device, origin, CQChartsSymbol::box(), sx, sy,
               penBrush, /*scaled*/false);
  }
  // draw origin gradient
  else if (paretoOriginType() == ParetoOriginType::GRADIENT) {
//  auto bgColor = interpColor(Color::makeInterfaceValue(0.0), ColorInd());
    auto bgColor = QColor(0, 0, 0, 0);

    CQChartsDrawUtil::drawParetoGradient(device, origin, dataRange.bbox(), originColor, bgColor);
  }
}

void
CQChartsScatterPlot::
drawPareto(PaintDevice *device) const
{
  const auto &dataRange = this->dataRange();

  auto *xDetails = (xColumn().isValid() ? columnDetails(xColumn()) : nullptr);
  auto *yDetails = (yColumn().isValid() ? columnDetails(yColumn()) : nullptr);

  bool invX = (xDetails ? xDetails->decreasing().toBool() : false);
  bool invY = (yDetails ? yDetails->decreasing().toBool() : false);

  auto origin = Point((invX ? dataRange.xmax() : dataRange.xmin()),
                      (invY ? dataRange.ymax() : dataRange.ymin()));

  //---

  auto updateBrush = [&](int i, int n) {
    PenBrush penBrush;

    ColorInd ic(i, n);

    auto pc = interpParetoStrokeColor(ic);
    auto fc = interpParetoFillColor(ic);

    setPenBrush(penBrush, paretoPenData(pc), paretoBrushData(fc));

    CQChartsDrawUtil::setPenBrush(device, penBrush);
  };

  auto drawParetoPoints = [&](const Points &points) {
    if (points.empty()) return;

    auto front = CQChartsGeom::calcParetoFront(points, origin);

    Polygon poly;

    for (const auto &p : front)
      poly.addPoint(p);

    device->drawPolyline(poly);
  };

  //---

  int ig = 0;
  int ng = int(groupInds_.size());

  int is = 0;
  int ns = 1;

  if (ng == 1) {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    const auto &values = (*nameValues.begin()).second.values;

    if (values.size() > 1)
      ns = values.size();
  }

  //---

  device->save();

  setClipRect(device);

  for (const auto &groupInd : groupInds_) {
    auto pnv = groupNameValues_.find(groupInd);
    assert(pnv != groupNameValues_.end());

    //---

    const auto &nameValues = (*pnv).second;

    Points points;

    for (const auto &pn : nameValues) {
      const auto &values = pn.second;

      for (const auto &v : values.values)
        points.push_back(v.p);

      if (values.values.size() > 1) {
        updateBrush(is, ns);

        drawParetoPoints(points);

        points.clear();

        ++is;
      }
    }

    //---

    updateBrush(ig, ng);

    drawParetoPoints(points);

    //---

    ++ig;
  }

  device->restore();
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
  int ng = int(groupInds_.size());

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
  int ng = int(groupInds_.size());

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

  setPenBrush(penBrush, symbolPenData(strokeColor), symbolBrushData(fillColor));

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
  int ng = int(groupInds_.size());

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
  int ng = int(groupInds_.size());

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

  setPenBrush(penBrush, symbolPenData(strokeColor), symbolBrushData(fillColor));

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
      xpw = th->groupXWhiskers_.emplace_hint(xpw, groupInd, new AxisBoxWhisker(th, Qt::Horizontal));

    auto *xWhiskerData = (*xpw).second;

    auto ypw = th->groupYWhiskers_.find(groupInd);

    if (ypw == th->groupYWhiskers_.end())
      ypw = th->groupYWhiskers_.emplace_hint(ypw, groupInd, new AxisBoxWhisker(th, Qt::Vertical));

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
      xpw = th->groupXWhiskers_.emplace_hint(xpw, groupInd, new AxisBoxWhisker(th, Qt::Horizontal));

    auto *xWhiskerData = (*xpw).second;

    auto ypw = th->groupYWhiskers_.find(groupInd);

    if (ypw == th->groupYWhiskers_.end())
      ypw = th->groupYWhiskers_.emplace_hint(ypw, groupInd, new AxisBoxWhisker(th, Qt::Vertical));

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

  dataLabelDatas_.push_back(std::move(dataLabelData));
}

void
CQChartsScatterPlot::
drawDataLabelDatas(PaintDevice *device) const
{
  const auto *dataLabel = this->dataLabel();

  for (const auto &dataLabelData : dataLabelDatas_)
    dataLabel->draw(device, dataLabelData.bbox, dataLabelData.text,
                    static_cast<CQChartsDataLabel::Position>(dataLabelData.position),
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

void
CQChartsScatterPlot::
drawDataLabel(PaintDevice *device, const BBox &bbox, const QString &str,
              const PenBrush &penBrush, const Font &font) const
{
  //auto *th = const_cast<CQChartsXYPlot *>(this);

  if (isAdjustText())
    dataLabel_->setTextPlacer(placer_);

  dataLabel()->draw(device, bbox, str,
                    static_cast<CQChartsDataLabel::Position>(dataLabelPosition()),
                    penBrush, font);

  if (isAdjustText())
    dataLabel_->setTextPlacer(nullptr);
}

//---

CQChartsPlotCustomControls *
CQChartsScatterPlot::
createCustomControls()
{
  auto *controls = new CQChartsScatterPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(const ScatterPlot *plot, int groupInd, const BBox &rect, const Point &pos,
                        const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotPointObj(const_cast<ScatterPlot *>(plot), rect, pos, is, ig, iv),
 scatterPlot_(plot), groupInd_(groupInd)
{
}

//---

CQChartsSymbol
CQChartsScatterPointObj::
calcSymbol() const
{
  CQChartsSymbol symbol;

  if (extraData(/*create*/false))
    symbol = this->symbol();

  if (! symbol.isValid())
    symbol = scatterPlot()->symbol();

  return symbol;
}

CQChartsLength
CQChartsScatterPointObj::
calcSymbolSize() const
{
  Length symbolSize;

  if (extraData(/*create*/false))
    symbolSize = this->symbolSize();

  if (! symbolSize.isValid())
    symbolSize = scatterPlot()->symbolSize();

  return symbolSize;
}

CQChartsLength
CQChartsScatterPointObj::
calcFontSize() const
{
  Length fontSize;

  if (extraData(/*create*/false))
    fontSize = this->fontSize();

  if (! fontSize.isValid())
    fontSize = scatterPlot()->dataLabelFontSize();

  return fontSize;
}

CQChartsColor
CQChartsScatterPointObj::
calcColor() const
{
  Color color;

  if (extraData(/*create*/false))
    color = this->color();

  return color;
}

CQChartsAlpha
CQChartsScatterPointObj::
calcAlpha() const
{
  Alpha alpha;

  if (extraData(/*create*/false))
    alpha = this->alpha();

  return alpha;
}

CQChartsFont
CQChartsScatterPointObj::
calcFont() const
{
  Font font;

  if (extraData(/*create*/false))
    font = this->font();

  if (! font.isValid())
    font = scatterPlot()->dataLabelFont();

  return font;
}

Qt::Orientation
CQChartsScatterPointObj::
calcLabelDir() const
{
  Qt::Orientation dir { Qt::Horizontal };

  if (extraData(/*create*/false))
    dir = this->labelDir();

  return dir;
}

CQChartsImage
CQChartsScatterPointObj::
calcImage() const
{
  CQChartsImage image;

  if (extraData(/*create*/false))
    image = this->image();

  return image;
}

//---

CQChartsScatterPointObj::ExtraData *
CQChartsScatterPointObj::
extraData()
{
  if (! edata_)
    edata_ = std::make_unique<ExtraData>();

  return edata_.get();
}

const CQChartsScatterPointObj::ExtraData *
CQChartsScatterPointObj::
extraData(bool create) const
{
  if (! edata_) {
    if (! create)
      return nullptr;

    const_cast<CQChartsScatterPointObj *>(this)->edata_ = std::make_unique<ExtraData>();
  }

  return edata_.get();
}

//---

QString
CQChartsScatterPointObj::
calcId() const
{
  auto ind1 = scatterPlot()->unnormalizeIndex(modelInd());

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

  scatterPlot()->addTipHeader(tableTip, modelInd());

  scatterPlot()->addNoTipColumns(tableTip);

  //---

  if (nameColumn().isValid() && name().length() && ! tableTip.hasColumn(nameColumn())) {
    auto name = scatterPlot()->columnHeaderName(nameColumn(), /*tip*/true);

    tableTip.addTableRow(name, this->name());

    tableTip.addColumn(nameColumn());
  }

  //---

  // add group column (TODO: check group column)
  if (ig_.n > 1) {
    auto groupColumn = scatterPlot()->groupIndColumn();

    if (! tableTip.hasColumn(groupColumn)) {
      auto groupName = scatterPlot()->groupIndName(groupInd_);

      tableTip.addTableRow("Group", groupName);

      tableTip.addColumn(groupColumn);
    }
  }

  //---

  // add x, y columns
  if (! tableTip.hasColumn(scatterPlot()->xColumn())) {
    double x = point().x;

    QString xstr;

    if (scatterPlot()->isUniqueX()) {
      auto *columnDetails = scatterPlot()->columnDetails(scatterPlot()->xColumn());

      bool ok;
      xstr = (columnDetails ?
        CQChartsVariant::toString(columnDetails->uniqueValue(int(x)), ok) :
        scatterPlot()->xStr(x));
    }
    else
      xstr = scatterPlot()->xStr(x);

    tableTip.addTableRow(scatterPlot()->xHeaderName(/*tip*/true), xstr);

    tableTip.addColumn(scatterPlot()->xColumn());
  }

  if (! tableTip.hasColumn(scatterPlot()->yColumn())) {
    double y = point().y;

    QString ystr;

    if (scatterPlot()->isUniqueY()) {
      auto *columnDetails = scatterPlot()->columnDetails(scatterPlot()->yColumn());

      bool ok;
      ystr = (columnDetails ?
        CQChartsVariant::toString(columnDetails->uniqueValue(int(y)), ok) : scatterPlot()->yStr(y));
    }
    else
      ystr = scatterPlot()->yStr(y);

    tableTip.addTableRow(scatterPlot()->yHeaderName(/*tip*/true), ystr);

    tableTip.addColumn(scatterPlot()->yColumn());
  }

  //---

  auto addColumnRowValue = [&](const Column &column) {
    scatterPlot()->addTipColumn(tableTip, column, modelInd());
  };

  //---

  // add symbol type, symbol size and font size columns
  addColumnRowValue(scatterPlot()->symbolTypeColumn());
  addColumnRowValue(scatterPlot()->symbolSizeColumn());
  addColumnRowValue(scatterPlot()->fontSizeColumn  ());

  //---

  // get values for name (grouped id identical names)
  CQChartsPointPlot::ValueData valuePoint;

  auto pg = scatterPlot()->groupNameValues().find(groupInd_);
  assert(pg != scatterPlot()->groupNameValues().end());

  auto p = (*pg).second.find(name());

  if (p != (*pg).second.end()) {
    const auto &values = (*p).second.values;

    valuePoint = values[size_t(iv_.i)];
  }

  //---

  // add color column
  if      (scatterPlot()->colorColumn().isValid())
    addColumnRowValue(scatterPlot()->colorColumn());
  else if (valuePoint.color.isValid())
    tableTip.addTableRow(scatterPlot()->colorHeaderName(/*tip*/true), valuePoint.color.colorStr());

  //---

  // add alpha column
  if      (scatterPlot()->alphaColumn().isValid())
    addColumnRowValue(scatterPlot()->alphaColumn());
  else if (valuePoint.alpha.isSet())
    tableTip.addTableRow(scatterPlot()->alphaHeaderName(/*tip*/true), valuePoint.alpha.toString());

  //---

  scatterPlot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsScatterPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, scatterPlot()->xColumn());
  addColumnSelectIndex(inds, scatterPlot()->yColumn());

  addColumnSelectIndex(inds, scatterPlot()->symbolTypeColumn());
  addColumnSelectIndex(inds, scatterPlot()->symbolSizeColumn());
  addColumnSelectIndex(inds, scatterPlot()->fontSizeColumn  ());
  addColumnSelectIndex(inds, scatterPlot()->colorColumn     ());
  addColumnSelectIndex(inds, scatterPlot()->alphaColumn     ());
}

//---

void
CQChartsScatterPointObj::
draw(PaintDevice *device) const
{
  // skip if too small
  if (this->isMinSymbolSize())
    return;

  //---

  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  //---

  // get symbol and size
  auto symbol = this->calcSymbol();

  double sx, sy;
  calcSymbolPixelSize(sx, sy, /*square*/true, /*enforceMinSize*/false);

  //---

  // draw symbol or image (image only for point)
  auto image = this->calcImage();

  if (! image.isValid()) {
    if (symbol.isValid())
      scatterPlot()->drawSymbol(device, point(), symbol, sx, sy, penBrush, /*scaled*/false);
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

    auto ps = scatterPlot()->windowToPixel(point());

    auto pbbox = BBox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

    auto ialign = scatterPlot()->imageAlign();

    if (ialign != Qt::AlignCenter) {
      if      (ialign & Qt::AlignLeft ) { pbbox.moveBy(Point( sx, 0)); }
      else if (ialign & Qt::AlignRight) { pbbox.moveBy(Point(-sx, 0)); }

      if      (ialign & Qt::AlignBottom) { pbbox.moveBy(Point(0,  sy)); }
      else if (ialign & Qt::AlignTop   ) { pbbox.moveBy(Point(0, -sy)); }
    }

    device->drawImageInRect(scatterPlot()->pixelToWindow(pbbox), image);
  }

  if (scatterPlot()->drawLayerType() == CQChartsLayer::Type::SELECTION) {
    if (isSelected() && scatterPlot()->isOutlineSelected()) {
      drawSelectedOutline(device, penBrush, sx, sy);
    }
  }

  device->resetColorNames();

  //---

  // draw text labels
  if (scatterPlot()->isPointLabels() && name().length())
    drawDataLabel(device);
}

void
CQChartsScatterPointObj::
drawSelectedOutline(PaintDevice *device, const PenBrush &penBrush, double sx, double sy) const
{
  auto ps = scatterPlot()->windowToPixel(point());

  auto pbbox = BBox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);
  auto bbox  = scatterPlot()->pixelToWindow(pbbox);

  auto penBrush1 = penBrush;

  if (penBrush1.brush.style() == Qt::NoBrush)
    penBrush1.brush = QBrush(penBrush1.pen.color());

  penBrush1.pen = QPen(Qt::NoPen);

  CQChartsDrawUtil::setPenBrush(device, penBrush1);

  CQChartsDrawUtil::drawSelectedOutline(device, bbox);
}

bool
CQChartsScatterPointObj::
isMinSymbolSize() const
{
  auto &minSymbolSize = scatterPlot()->minSymbolSize();

  if (! minSymbolSize.isValid())
    return false;

  double sx, sy;

  if      (minSymbolSize.units() == Units::PLOT) {
    scatterPlot()->plotSymbolSize(this->calcSymbolSize(), sx, sy, symbolDir());

    if (symbolDir() == Qt::Horizontal)
      return (minSymbolSize.value() > sx);
    else
      return (minSymbolSize.value() > sy);
  }
  else if (minSymbolSize.units() == Units::PIXEL) {
    scatterPlot()->pixelSymbolSize(this->calcSymbolSize(), sx, sy, symbolDir());

    return (minSymbolSize.value() > sx);
  }
  else
    return false;
}

void
CQChartsScatterPointObj::
drawDataLabel(PaintDevice *device) const
{
  if (this->isMinLabelSize())
    return;

  const auto *dataLabel = scatterPlot()->dataLabel();

  //---

  // text font color
  PenBrush penBrush;

  auto tc = dataLabel->interpTextColor(calcColorInd());

  scatterPlot()->setPenBrush(penBrush, PenData(true, tc, dataLabel->textAlpha()),
                             BrushData(false));

  //---

  // get label font
  auto font = calcLabelFont();

  //---

  // draw text
  auto ps = scatterPlot()->windowToPixel(point());

  double sx, sy;
  calcSymbolPixelSize(sx, sy, /*square*/true, /*enforceMinSize*/false);

  BBox ptbbox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

  auto tbbox = scatterPlot()->pixelToWindow(ptbbox);

  if (scatterPlot()->isAdjustText())
    scatterPlot()->drawDataLabel(device, tbbox, name(), penBrush, font);
  else
    const_cast<ScatterPlot *>(scatterPlot())->
      addDataLabelData(tbbox, name(), scatterPlot()->dataLabelPosition(), penBrush, font);
}

bool
CQChartsScatterPointObj::
isMinLabelSize() const
{
  auto &minLabelSize = scatterPlot()->minLabelSize();

  if (! minLabelSize.isValid())
    return false;

  // get label font
  auto font = calcLabelFont();

  // get text size (pixels)
  const auto *dataLabel = scatterPlot()->dataLabel();

  BBox tbbox(-1, -1, 1, 1); // just need size do use dummy value

  auto wsize = dataLabel->calcRect(tbbox, name(),
                 static_cast<CQChartsDataLabel::Position>(scatterPlot()->dataLabelPosition()),
                 font).size();

  if      (minLabelSize.units() == Units::PLOT) {
    if (calcLabelDir() == Qt::Horizontal)
      return (minLabelSize.value() > wsize.width());
    else
      return (minLabelSize.value() > wsize.height());
  }
  else if (minLabelSize.units() == Units::PIXEL) {
    auto psize = scatterPlot()->windowToPixelSize(wsize);

    return (minLabelSize.value() > psize.width());
  }
  else
    return false;
}

void
CQChartsScatterPointObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  ColorInd ic;

  if (scatterPlot()->colorType() == CQChartsPlot::ColorType::AUTO) {
    // default for scatter is set or group color (not value color !!)
    if      (is_.n > 1)
      ic = is_;
    else if (ig_.n > 1)
      ic = ig_;
  }
  else
    ic = calcColorInd();

  //--

  //scatterPlot()->setSymbolPenBrush(penBrush, ic);

  auto fc = scatterPlot()->interpSymbolFillColor(ic);
  auto fa = scatterPlot()->symbolFillAlpha();
  auto sc = scatterPlot()->interpSymbolStrokeColor(ic);
  auto sa = scatterPlot()->symbolStrokeAlpha();

  // override symbol fill color for custom color
  auto color = this->calcColor();

  if (color.isValid())
    fc = scatterPlot()->interpColor(color, ic);

  // override symbol fill alpha for custom alpha
  auto alpha = this->calcAlpha();

  if (alpha.isSet())
    fa = alpha;

  //---

  bool filled  = scatterPlot()->isSymbolFilled();
  bool stroked = scatterPlot()->isSymbolStroked();

  auto symbol = this->calcSymbol();

  if      (! symbol.isFilled()) {
    filled  = false;
    stroked = true;

    // use fill color for stroke
    sc = fc;
    sa = fa;
  }
  else if (! symbol.isStroked()) {
    filled  = true;
    stroked = false;
  }

  //---

  scatterPlot()->setPenBrush(penBrush,
    (stroked ? scatterPlot()->symbolPenData  (sc, sa) : PenData  (false)),
    (filled  ? scatterPlot()->symbolBrushData(fc, fa) : BrushData(false)));

  if (updateState)
    scatterPlot()->updateObjPenBrushState(this, penBrush, drawType());
}

CQChartsFont
CQChartsScatterPointObj::
calcLabelFont() const
{
  // get custom font size (from font column)
  auto fontSize = this->calcFontSize();

  //---

  // get label font
  auto font  = this->calcFont();
  auto font1 = font;

  if (fontSize.isValid()) {
    double fontPixelSize;

    if (calcLabelDir() == Qt::Horizontal)
      fontPixelSize = scatterPlot()->lengthPixelWidth(fontSize);
    else
      fontPixelSize = scatterPlot()->lengthPixelHeight(fontSize);

    // limit to max font size
    fontPixelSize = scatterPlot()->limitFontSize(fontPixelSize);

    font1.setPointSizeF(fontPixelSize);
  }

  return font1;
}

double
CQChartsScatterPointObj::
xColorValue(bool relative) const
{
  const auto &dataRange = scatterPlot()->dataRange();

  if (relative)
    return CMathUtil::map(point().x, dataRange.xmin(), dataRange.xmax(), 0.0, 1.0);
  else
    return point().x;
}

double
CQChartsScatterPointObj::
yColorValue(bool relative) const
{
  const auto &dataRange = scatterPlot()->dataRange();

  if (relative)
    return CMathUtil::map(point().y, dataRange.ymin(), dataRange.ymax(), 0.0, 1.0);
  else
    return point().y;
}

//------

CQChartsScatterConnectedObj::
CQChartsScatterConnectedObj(const ScatterPlot *plot, int groupInd, const QString &name,
                            const ColorInd &ig, const ColorInd &is, const BBox &rect) :
 CQChartsPlotObj(const_cast<ScatterPlot *>(plot), rect, is, ig, ColorInd()),
 scatterPlot_(plot), groupInd_(groupInd), name_(name)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsScatterConnectedObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig().i).arg(is().i);
}

QString
CQChartsScatterConnectedObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  QString groupName;

  if (name_ == "")
    groupName = scatterPlot_->groupIndName(groupInd_);
  else {
    ColorInd ind;

    groupName = scatterPlot_->singleGroupName(ind);

    if (groupName == "")
      groupName = name_;
  }

  tableTip.addBoldLine("Connected");
  tableTip.addTableRow("Group", groupName);

  return tableTip.str();
}

void
CQChartsScatterConnectedObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsPlotObj::addProperties(model, path);
}

void
CQChartsScatterConnectedObj::
draw(PaintDevice *device) const
{
  using Points = CQChartsScatterPlot::Points;

  Points points;

  if (name_ == "")
    points = scatterPlot_->indPoints(QVariant(groupInd_), /*isGroup*/true);
  else
    points = scatterPlot_->indPoints(QVariant(name_), /*isGroup*/false);

  //---

  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  Polygon poly;

  if (! scatterPlot_->isPolar()) {
    using XPoints = std::map<double, Points>;

    XPoints xPoints;

    for (const auto &p : points) {
      auto gp = scatterPlot_->adjustGroupPoint(groupInd_, p);

      xPoints[gp.x].push_back(gp);
    }

    for (const auto &xp : xPoints)
      for (const auto &p : xp.second)
        poly.addPoint(p);
  }
  else {
    for (const auto &p : points)
      poly.addPoint(p);
  }

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawPolyline(poly);
}

void
CQChartsScatterConnectedObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  ColorInd ic;

  if (name_ != "")
    ic = is_;
  else
    ic = ig_;

  // calc pen and brush
  scatterPlot_->setLineDataPen(penBrush.pen, ic);

  scatterPlot_->setBrush(penBrush, BrushData(false));

  if (updateState)
    scatterPlot_->updateObjPenBrushState(this, ic, penBrush, CQChartsPlot::DrawType::LINE);
}

//------

CQChartsScatterCellObj::
CQChartsScatterCellObj(const ScatterPlot *plot, int groupInd, const BBox &rect, const ColorInd &is,
                       const ColorInd &ig, int ix, int iy, const Points &points, int maxN) :
 CQChartsPlotObj(const_cast<ScatterPlot *>(plot), rect, is, ig, ColorInd()),
 scatterPlot_(plot), groupInd_(groupInd), ix_(ix), iy_(iy), points_(points), maxN_(maxN)
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

  scatterPlot_->gridData().xIValues(ix_, xmin, xmax);
  scatterPlot_->gridData().yIValues(iy_, ymin, ymax);

  tableTip.addTableRow("X Range", QString("%1 %2").arg(xmin).arg(xmax));
  tableTip.addTableRow("Y Range", QString("%1 %2").arg(ymin).arg(ymax));
  tableTip.addTableRow("Count"  , points_.size());

  //---

  //scatterPlot()->addTipColumns(tableTip, ind);

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
  ColorInd ic(int(points_.size()), maxN_);

  auto pc = scatterPlot_->interpGridCellStrokeColor(ColorInd());
  auto fc = scatterPlot_->interpPaletteColor(ic);

  scatterPlot_->setPenBrush(penBrush, scatterPlot_->gridCellPenData(pc),
                            scatterPlot_->gridCellBrushData(fc));

  if (updateState)
    scatterPlot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsScatterCellObj::
calcRugPenBrush(PenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  auto ic = (ig_.n > 1 ? ig_ : is_);

  scatterPlot_->setSymbolPenBrush(penBrush, ic);

  if (updateState)
    scatterPlot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
}

//------

CQChartsScatterHexObj::
CQChartsScatterHexObj(const ScatterPlot *plot, int groupInd, const BBox &rect, const ColorInd &is,
                      const ColorInd &ig, int ix, int iy, const Polygon &poly, int n, int maxN) :
 CQChartsPlotObj(const_cast<ScatterPlot *>(plot), rect, is, ig, ColorInd()), scatterPlot_(plot),
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

  auto pc = scatterPlot_->interpGridCellStrokeColor(ColorInd());
  auto fc = scatterPlot_->interpPaletteColor(ic);

  scatterPlot_->setPenBrush(penBrush, scatterPlot_->gridCellPenData(pc),
                            scatterPlot_->gridCellBrushData(fc));

  if (updateState)
    scatterPlot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsScatterDensityObj::
CQChartsScatterDensityObj(const ScatterPlot *plot, int groupInd, const QString &name,
                          const BBox &rect) :
 CQChartsPlotObj(const_cast<ScatterPlot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 scatterPlot_(plot), groupInd_(groupInd), name_(name)
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
  auto *density = scatterPlot_->getDensity(groupInd_, name_);
  if (! density) return "";

  //---

  CQChartsTableTip tableTip;

  QString groupName;

  if (name_ == "")
    groupName = scatterPlot_->groupIndName(groupInd_);
  else {
    ColorInd ind;

    groupName = scatterPlot_->singleGroupName(ind);

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
  auto *density = scatterPlot_->getDensity(groupInd_, name_);
  if (! density) return;

  //---

  // draw density
  PaintDevice::SaveRestore saveRestore(device);

  scatterPlot_->setClipRect(device);

  density->draw(scatterPlot_, device);
}

void
CQChartsScatterDensityObj::
calcPenBrush(PenBrush &, bool) const
{
  // TODO
}

//------

CQChartsScatterColorKeyItem::
CQChartsScatterColorKeyItem(ScatterPlot *plot, int groupInd, const ColorInd &is,
                            const ColorInd &ig) :
 CQChartsColorBoxKeyItem(plot, is, ig, ColorInd()), scatterPlot_(plot), groupInd_(groupInd)
{
  setClickable(true);
}

void
CQChartsScatterColorKeyItem::
doSelect(SelMod)
{
  CQChartsPlot::PlotObjs objs;

  scatterPlot()->getGroupObjs(ig_.i, objs);
  if (objs.empty()) return;

  //---

  scatterPlot()->selectObjs(objs, /*export*/true);

  key_->redraw(/*wait*/ true);
}

QBrush
CQChartsScatterColorKeyItem::
fillBrush() const
{
  auto ic = (is_.n > 1 ? is_ : ig_);

  QColor c;

  if (color_.isValid())
    c = scatterPlot_->interpColor(color_, ColorInd());
  else {
    c = scatterPlot_->interpSymbolFillColor(ic);

    //c = CQChartsColorBoxKeyItem::fillBrush().color();
  }

  CQChartsDrawUtil::setColorAlpha(c, scatterPlot_->symbolFillAlpha());

  adjustFillColor(c);

  return c;
}

bool
CQChartsScatterColorKeyItem::
calcHidden() const
{
  auto ih = setIndex();

  return scatterPlot_->isSetHidden(ih.i);
}

bool
CQChartsScatterColorKeyItem::
tipText(const Point &, QString &tip) const
{
  if (groupInd_ < -1) return false;

  CQChartsPlot::PlotObjs objs;

  scatterPlot()->getGroupObjs(groupInd_, objs);

  CQChartsTableTip tableTip;

  auto groupName = scatterPlot_->groupIndName(groupInd_);

  tableTip.addTableRow("Name", groupName);

  if (! objs.empty())
    tableTip.addTableRow("Count", objs.size());

  if (isSetHidden())
    tableTip.addTableRow("Hidden", "true");

  tip = tableTip.str();

  return true;
}

CQChartsUtil::ColorInd
CQChartsScatterColorKeyItem::
setIndex() const
{
  return (groupInd_ >= -1 ? ColorInd(groupInd_, 1) : is_);
}

//---

CQChartsScatterGridKeyItem::
CQChartsScatterGridKeyItem(ScatterPlot *plot, int n) :
 CQChartsGradientKeyItem(plot), scatterPlot_(plot)
{
  setMinValue(0);
  setMaxValue(n);
  setInteger (true);
}

//---

CQChartsScatterHexKeyItem::
CQChartsScatterHexKeyItem(ScatterPlot *plot, int n) :
 CQChartsGradientKeyItem(plot), scatterPlot_(plot)
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
}

void
CQChartsScatterPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsScatterPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addGroupColumnWidgets();

  addOptionsWidgets();

  addColorColumnWidgets("Point Color");
  addSymbolSizeWidgets ();
  addSymbolLabelWidgets();
  addFontSizeWidgets   ();

  // color, contrast, ...

  addKeyList();
}

void
CQChartsScatterPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addNamedColumnWidgets(QStringList() << "x" << "y", columnsFrame);

//addNamedColumnWidgets(QStringList() << "x" << "y" << "name", columnsFrame);
}

void
CQChartsScatterPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame", FrameOpts::makeNoStretch());

  bestFitCheck_ = createBoolEdit("bestFit"   , /*choice*/false);
  hullCheck_    = createBoolEdit("convexHull", /*choice*/false);
  statsCheck_   = createBoolEdit("statsLines", /*choice*/false);
  paretoCheck_  = createBoolEdit("pareto"    , /*choice*/false);

  addFrameColWidget (optionsFrame_, bestFitCheck_);
  addFrameColWidget (optionsFrame_, hullCheck_);
  addFrameColWidget (optionsFrame_, statsCheck_);
  addFrameColWidget (optionsFrame_, paretoCheck_);
  addFrameColStretch(optionsFrame_);

#if 0
  plotTypeCombo_ = createEnumEdit("plotType");

  addFrameWidget(optionsFrame_, "Plot Type", plotTypeCombo_);
#endif
}

void
CQChartsScatterPlotCustomControls::
addSymbolLabelWidgets()
{
  // point labels group
  symbolLabelGroup_ = CQUtil::makeWidget<CQGroupBox>("symbolLabelGroup");

  auto *groupLayout = CQUtil::makeLayout<QVBoxLayout>(symbolLabelGroup_, 0, 0);

  symbolLabelGroup_->setTitle("Point Label");
  symbolLabelGroup_->setCheckable(true);

  symbolLabelGroup_->setToolTip("Show per Point Labels");

  symbolLabelGroup_->setTitleScale(0.85);
  symbolLabelGroup_->setTitleColored(true);
  symbolLabelGroup_->setEnableChecked(false);

  layout_->addWidget(symbolLabelGroup_);

  //---

  auto pointLabelsFrame = createFrame("pointLabelsFrame");

  //---

  // label text and font
  labelColumnCombo_ = CQUtil::makeWidget<CQChartsColumnCombo>("labelColumnCombo");
  positionEdit_     = CQUtil::makeWidget<CQEnumCombo>("positionEdit");

  positionEdit_->setPropName("position");

  addFrameWidget(pointLabelsFrame, "Column"  , labelColumnCombo_);
  addFrameWidget(pointLabelsFrame, "Position", positionEdit_);

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
  CQUtil::optConnectDisconnect(b,
    bestFitCheck_, SIGNAL(stateChanged(int)), this, SLOT(bestFitSlot()));
  CQUtil::optConnectDisconnect(b,
    hullCheck_, SIGNAL(stateChanged(int)), this, SLOT(convexHullSlot()));
  CQUtil::optConnectDisconnect(b,
    statsCheck_, SIGNAL(stateChanged(int)), this, SLOT(statsLinesSlot()));
  CQUtil::optConnectDisconnect(b,
    paretoCheck_, SIGNAL(stateChanged(int)), this, SLOT(paretoSlot()));

  CQUtil::optConnectDisconnect(b,
    plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));

  CQUtil::optConnectDisconnect(b,
    symbolLabelGroup_, SIGNAL(clicked(bool)), this, SLOT(pointLabelsSlot()));
  CQUtil::optConnectDisconnect(b,
    labelColumnCombo_, SIGNAL(columnChanged()), this, SLOT(labelColumnSlot()));
  CQUtil::optConnectDisconnect(b,
    positionEdit_, SIGNAL(currentIndexChanged(int)), this, SLOT(positionSlot()));

  CQUtil::optConnectDisconnect(b,
    fontSizeControlGroup_, SIGNAL(groupChanged()), this, SLOT(fontSizeGroupChanged()));
  CQUtil::optConnectDisconnect(b,
    fontSizeEdit_, SIGNAL(lengthChanged()), this, SLOT(fontSizeSlot()));
  CQUtil::optConnectDisconnect(b,
    fontSizeColumnCombo_, SIGNAL(columnChanged()), this, SLOT(fontSizeColumnSlot()));
  CQUtil::optConnectDisconnect(b,
    fontSizeRange_, SIGNAL(sliderRangeChanged(double, double)),
    this, SLOT(fontSizeRangeSlot(double, double)));

  CQChartsPointPlotCustomControls::connectSlots(b);
}

void
CQChartsScatterPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  scatterPlot_ = dynamic_cast<CQChartsScatterPlot *>(plot);

  CQChartsPointPlotCustomControls::setPlot(plot);
}

CQChartsColor
CQChartsScatterPlotCustomControls::
getColorValue()
{
  return scatterPlot_->symbolFillColor();
}

void
CQChartsScatterPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  scatterPlot_->setSymbolFillColor(c);
}

void
CQChartsScatterPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  if (bestFitCheck_) bestFitCheck_->setChecked(scatterPlot_->isBestFit());
  if (hullCheck_   ) hullCheck_   ->setChecked(scatterPlot_->isHull());
  if (statsCheck_  ) statsCheck_  ->setChecked(scatterPlot_->isStatsLines());
  if (paretoCheck_ ) paretoCheck_ ->setChecked(scatterPlot_->isPareto());

  //---

  if (plotTypeCombo_)
    plotTypeCombo_->setCurrentValue(static_cast<int>(scatterPlot_->plotType()));

  //---

  const auto &model = scatterPlot_->currentModel();

  if (labelColumnCombo_) {
    bool hasLabelColumn = scatterPlot_->labelColumn().isValid();

  //if (symbolLabelGroup_) symbolLabelGroup_->setEnabled(hasLabelColumn);
    if (positionEdit_    ) positionEdit_    ->setEnabled(hasLabelColumn);

    if (symbolLabelGroup_) symbolLabelGroup_->setChecked(scatterPlot_->isPointLabels());
    if (labelColumnCombo_) labelColumnCombo_->setModelColumn(scatterPlot_->getModelData(model),
                                                             scatterPlot_->labelColumn());
    if (positionEdit_    ) positionEdit_    ->setObj(scatterPlot_->dataLabel());
  }

  //---

  bool hasFontSizeColumn = scatterPlot_->fontSizeColumn().isValid();

  if (fontSizeEdit_ ) fontSizeEdit_ ->setEnabled(! hasFontSizeColumn);
  if (fontSizeRange_) fontSizeRange_->setEnabled(hasFontSizeColumn);

  if (fontSizeEdit_       ) fontSizeEdit_       ->setLength(scatterPlot_->dataLabelFontSize());
  if (fontSizeColumnCombo_) fontSizeColumnCombo_->setModelColumn(scatterPlot_->getModelData(model),
                                                                 scatterPlot_->fontSizeColumn());
  if (fontSizeRange_      ) fontSizeRange_      ->setPlot(scatterPlot_);

  if (fontSizeControlGroup_ && hasFontSizeColumn)
    fontSizeControlGroup_->setColumn();

  //---

  connectSlots(true);

  //---

  CQChartsPointPlotCustomControls::updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
bestFitSlot()
{
  scatterPlot_->setBestFit(bestFitCheck_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
convexHullSlot()
{
  scatterPlot_->setHull(hullCheck_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
statsLinesSlot()
{
  scatterPlot_->setStatsLines(statsCheck_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
paretoSlot()
{
  scatterPlot_->setPareto(paretoCheck_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
plotTypeSlot()
{
  if (plotTypeCombo_)
    scatterPlot_->setPlotType(
      static_cast<CQChartsScatterPlot::PlotType>(plotTypeCombo_->currentValue()));
}

void
CQChartsScatterPlotCustomControls::
pointLabelsSlot()
{
  scatterPlot_->setPointLabels(symbolLabelGroup_->isChecked());
}

void
CQChartsScatterPlotCustomControls::
labelColumnSlot()
{
  scatterPlot_->setLabelColumn(labelColumnCombo_->getColumn());

  bool hasLabelColumn = scatterPlot_->labelColumn().isValid();

  scatterPlot_->setPointLabels(hasLabelColumn);

  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
positionSlot()
{
  scatterPlot_->setDataLabelPosition(
    static_cast<CQChartsLabelPosition>(positionEdit_->currentIndex()));

  // TODO: need plot signal (property signal ?)
  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
fontSizeGroupChanged()
{
  if (fontSizeControlGroup_->isFixed()) {
    scatterPlot_->setFontSizeColumn(CQChartsColumn());

    // TODO: need plot signal
    updateWidgets();
  }
}

void
CQChartsScatterPlotCustomControls::
fontSizeSlot()
{
  scatterPlot_->setDataLabelFontSize(fontSizeEdit_->length());

  // TODO: need plot signal
  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
fontSizeColumnSlot()
{
  scatterPlot_->setFontSizeColumn(fontSizeColumnCombo_->getColumn());

  // TODO: need plot signal
  updateWidgets();
}

void
CQChartsScatterPlotCustomControls::
fontSizeRangeSlot(double min, double max)
{
  scatterPlot_->setFontSizeUserMapMin(min);
  scatterPlot_->setFontSizeUserMapMax(max);
}
