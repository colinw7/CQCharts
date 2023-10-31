#include <CQChartsSummaryPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTitle.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsValueSet.h>
#include <CQChartsDrawObj.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsFitData.h>
#include <CQChartsRotatedText.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsAnnotation.h>

#include <CQChartsScatterPlot.h>
#include <CQChartsDistributionPlot.h>
#include <CQChartsParallelPlot.h>
#include <CQChartsBoxPlot.h>
#include <CQChartsPiePlot.h>
#include <CQChartsGeomMinMaxEdit.h>

#include <CQPropertyViewItem.h>
#include <CQTableWidget.h>
#include <CQPerfMonitor.h>
#include <CQIconButton.h>
#include <CQGroupBox.h>
#include <CMathCorrelation.h>

#include <QMenu>
#include <QCheckBox>
#include <QVBoxLayout>

CQChartsSummaryPlotType::
CQChartsSummaryPlotType()
{
}

void
CQChartsSummaryPlotType::
addParameters()
{
  using SummaryPlot = CQChartsSummaryPlot;

  addColumnsParameter("columns", "Columns", "columns").
    setRequired().setPropPath("columns.columns").setTip("Columns");

  addColumnParameter("group", "Group", "groupColumn").
    setGroupable().setBasic().setPropPath("columns.group").setTip("Group column");

  addColumnParameter("symbolType", "Symbol Type", "symbolTypeColumn").
    setPropPath("columns.symbolType").setTip("Custom Symbol Type").setMapped().
    setMapMinMax(CQChartsSymbolType::minFillValue(), CQChartsSymbolType::maxFillValue());

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
    setPropPath("columns.symbolSize").setTip("Custom Symbol Size").setMapped().
    setMapMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue());

  // options
  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("MATRIX"  , static_cast<int>(SummaryPlot::PlotType::MATRIX  )).
    addNameValue("PARALLEL", static_cast<int>(SummaryPlot::PlotType::PARALLEL)).
    setTip("Plot Type");

  addEnumParameter("diagonalType", "Diagonal Cell Type", "diagonalType").
    addNameValue("NONE"        , static_cast<int>(SummaryPlot::DiagonalType::NONE        )).
    addNameValue("BOXPLOT"     , static_cast<int>(SummaryPlot::DiagonalType::BOXPLOT     )).
    addNameValue("DISTRIBUTION", static_cast<int>(SummaryPlot::DiagonalType::DISTRIBUTION)).
    addNameValue("PIE"         , static_cast<int>(SummaryPlot::DiagonalType::PIE         )).
    setTip("Diagonal Cell Type");
  addEnumParameter("upperDiagonalType", "Upper Diagonal Cell Type", "upperDiagonalType").
    addNameValue("NONE"       , static_cast<int>(SummaryPlot::OffDiagonalType::NONE       )).
    addNameValue("SCATTER"    , static_cast<int>(SummaryPlot::OffDiagonalType::SCATTER    )).
    addNameValue("CORRELATION", static_cast<int>(SummaryPlot::OffDiagonalType::CORRELATION)).
    setTip("Upper Diagonal Cell Type");
  addEnumParameter("lowerDiagonalType", "Lower Diagonal Cell Type", "lowerDiagonalType").
    addNameValue("NONE"       , static_cast<int>(SummaryPlot::OffDiagonalType::NONE       )).
    addNameValue("SCATTER"    , static_cast<int>(SummaryPlot::OffDiagonalType::SCATTER    )).
    addNameValue("CORRELATION", static_cast<int>(SummaryPlot::OffDiagonalType::CORRELATION)).
    setTip("Lower Diagonal Cell Type");

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsSummaryPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Summary Plot").
     h3("Summary").
      p("Draws summary plots of multiple column datas in a grid.").
      p("Each off diagonal plot is a 2d plot of associated columns for grid "
        "column (x) and grid row (y).").
      p("Each diagonal plot is a 1d plot of column grid column/row.").
     h3("Customization").
      p("The plot types for the diagonal, upper off diagonal and lower off diagonal "
        " can be customized from a selection of 1d and 2d plot types.").
      p("The appearance of each plot type can be configured.").
     h3("Functionality").
      p("Selecting a plot and pushing in displays the associated data in the a "
        "single plot of the specified type").
      p("Selecting a range in a plot will define a data range for the associated column(s) "
        "and select associated data in that range.").
      p("The filtered values for each columns can be cross selected back to the model.").
     h3("Limitations").
      p("The summary plots have limted functionality compared to the associated "
        "expanded plot of the associated type.").
     h3("Example").
      p(IMG("images/summary_plot.png"));
}

void
CQChartsSummaryPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Columns columns;

  int nc = details->numColumns();

  for (int i = 0; i < nc; ++i)
    columns.addColumn(CQChartsColumn(i));

  analyzeModelData.parameterNameColumns["columns"] = columns;
}

CQChartsPlot *
CQChartsSummaryPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsSummaryPlot(view, model);
}

//------

CQChartsSummaryPlot::
CQChartsSummaryPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("summary"), model),
 CQChartsObjPlotCellShapeData    <CQChartsSummaryPlot>(this),
 CQChartsObjScatterPointData     <CQChartsSummaryPlot>(this),
 CQChartsObjDistributionShapeData<CQChartsSummaryPlot>(this),
 CQChartsObjBoxPlotShapeData     <CQChartsSummaryPlot>(this),
 CQChartsObjBoxPlotTextData      <CQChartsSummaryPlot>(this),
 CQChartsObjPieShapeData         <CQChartsSummaryPlot>(this),
 CQChartsObjCorrelationTextData  <CQChartsSummaryPlot>(this),
 CQChartsObjXLabelTextData       <CQChartsSummaryPlot>(this),
 CQChartsObjYLabelTextData       <CQChartsSummaryPlot>(this),
 CQChartsObjRegionShapeData      <CQChartsSummaryPlot>(this)
{
}

CQChartsSummaryPlot::
~CQChartsSummaryPlot()
{
  CQChartsSummaryPlot::term();
}

//---

void
CQChartsSummaryPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  setId("summaryPlot");

  addTitle();

  setPlotFilled(false);
  setDataFilled(false);
  setFitFilled (false);

  setPlotClip(false);
  setDataClip(false);
  setFitClip (false);

  setXLabelTextAlign(Qt::AlignHCenter | Qt::AlignTop);
  setYLabelTextAlign(Qt::AlignRight | Qt::AlignVCenter);

  setXLabelTextFont(CQChartsFont().decFontSize(4));
  setYLabelTextFont(CQChartsFont().decFontSize(4));

  //---

  setPlotCellFillColor  (Color::makeInterfaceValue(0.15));
  setPlotCellStrokeColor(Color::makeInterfaceValue(1.00));
  setPlotCellStrokeAlpha(Alpha(0.1));

  //---

  scatterPlot_      = dynamic_cast<ScatterPlot      *>(createNamedPlot("scatter"));
  distributionPlot_ = dynamic_cast<DistributionPlot *>(createNamedPlot("distribution"));
  parallelPlot_     = dynamic_cast<ParallelPlot     *>(createNamedPlot("parallel"));
  boxPlot_          = dynamic_cast<BoxPlot          *>(createNamedPlot("box"));
  piePlot_          = dynamic_cast<PiePlot          *>(createNamedPlot("pie"));

  assert(scatterPlot_ && distributionPlot_ && parallelPlot_ && boxPlot_ && piePlot_);

  //--

  scatterPlot_->setXColumn(Column::makeRow());
  scatterPlot_->setYColumn(Column::makeRow());

  scatterPlot_->setParetoOriginColor(paretoOriginColor());
  scatterPlot_->setParetoStrokeWidth(paretoWidth());

  scatterPlot_->setColorMapped(false);
  scatterPlot_->setSymbolTypeMapped(false);
  scatterPlot_->setSymbolSizeMapped(false);

  scatterPlot_->title()->setVisible(false);
  scatterPlot_->key  ()->setVisible(false);

  //--

  distributionPlot_->setValueColumns(Columns(Column::makeRow()));

//distributionPlot_->setParetoStrokeWidth(paretoWidth());

  distributionPlot_->title()->setVisible(false);
  distributionPlot_->key  ()->setVisible(false);

  //--

  parallelPlot_->setYColumns(Columns(Column::makeRow()));

  parallelPlot_->title()->setVisible(false);

  //--

  boxPlot_->setValueColumns(Columns(Column::makeRow()));

  boxPlot_->title()->setVisible(false);

  //--

  piePlot_->setValueColumns(Columns(Column::makeRow()));
  piePlot_->setBucketed(true);

  piePlot_->title()->setVisible(false);

  //---

  setXLabelTextAngle(Angle::degrees(0));
  setYLabelTextAngle(Angle::degrees(90));

  setScatterSymbol(Symbol::circle());

  setScatterSymbolStroked(true);
  setScatterSymbolStrokeAlpha(Alpha(0.3));
  setScatterSymbolFilled (true);
  setScatterSymbolFillColor(Color::makePalette());
  setScatterSymbolSize(Length());

  setDistributionStroked(true);
  setDistributionStrokeAlpha(Alpha(0.3));
  setDistributionFilled (true);
  setDistributionFillColor(Color::makePalette());

  setBoxPlotStroked(true);
  setBoxPlotStrokeAlpha(Alpha(0.3));
  setBoxPlotFilled (true);
  setBoxPlotFillColor(Color::makePalette());
  setBoxPlotTextFont(CQChartsFont().decFontSize(8));

  setPieStroked(true);
  setPieStrokeAlpha(Alpha(0.3));
  setPieFilled (true);
  setPieFillColor(Color::makePalette());

  setCorrelationTextFont(CQChartsFont().decFontSize(8));

  setRegionStrokeColor(Color::makePalette());
  setRegionStrokeAlpha(Alpha(0.5));
  setRegionStrokeWidth(Length::pixel(3));
  setRegionFillAlpha  (Alpha(0.2));

  //---

  // left, top, right, bottom
  setOuterMargin(PlotMargin(Length::plot(0.1), Length::plot(0.1),
                            Length::plot(0.1), Length::plot(0.1)));

  setPixelMarginSize(32);

  //---

  addAxes();

  xAxis()->setVisible(false);
  xAxis()->setTickInside(true);
  xAxis()->setAxesTickLabelTextFont(CQChartsFont().decFontSize(4));
  xAxis()->setAxesTickLabelTextAngle(Angle::degrees(90));
  xAxis()->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);
  xAxis()->setSide(CQChartsAxis::AxisSide(CQChartsAxis::AxisSide::Type::TOP_RIGHT));
  xAxis()->setMajorTickLen(4);

  yAxis()->setVisible(false);
  yAxis()->setTickInside(true);
  yAxis()->setAxesTickLabelTextFont(CQChartsFont().decFontSize(4));
  yAxis()->setAxesTickLabelTextAngle(Angle::degrees(0));
  yAxis()->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);
  yAxis()->setSide(CQChartsAxis::AxisSide(CQChartsAxis::AxisSide::Type::TOP_RIGHT));
  yAxis()->setMajorTickLen(4);

  //---

  setMinDataScaleX(1.0);
  setMinDataScaleY(1.0);

  //---

  currentPlot_ = this;
}

void
CQChartsSummaryPlot::
term()
{
}

//---

CQChartsPlot *
CQChartsSummaryPlot::
createNamedPlot(const QString &typeName)
{
  auto *plot = createNamedPlotInstance(typeName);

  plot->setVisible(false);

  int n = view()->numPlots();

  plot->setId(QString("Chart.%1.%2").arg(n + 1).arg(typeName));

  double vr = CQChartsView::viewportRange();

  BBox bbox(0, 0, vr, vr);

  view()->addPlot(plot, bbox);

  plot->setRootPlot(this);

  return plot;
}

CQChartsPlot *
CQChartsSummaryPlot::
createNamedPlotInstance(const QString &typeName)
{
  const auto &model = this->currentModel();

  auto *type = charts()->plotType(typeName);
  assert(type);

  return type->createAndInit(view(), model);
}

//---

void
CQChartsSummaryPlot::
setPlotType(const PlotType &t, bool update)
{
  CQChartsUtil::testAndSet(plotType_, t, [&]() {
    if (update) updatePlots();

    Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setExpanded(bool b)
{
  CQChartsUtil::testAndSet(expanded_, b, [&]() {
    updatePlots(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
updatePlotsSlot()
{
  updatePlots();
}

void
CQChartsSummaryPlot::
updatePlots()
{
  currentPlot_ = this;

  if (! isExpanded() && (plotType() == PlotType::PARALLEL)) {
    parallelPlot()->setYColumns(columns());

    for (const auto &cv : columnVisible_) {
      if (! cv.second)
        parallelPlot()->setYColumnVisible(cv.first, false);
    }

    currentPlot_ = parallelPlot();
  }
  else
    parallelPlot()->setYColumns(Columns(Column::makeRow()));

  if (isExpanded() && (expandRow() != expandCol())) {
    auto column1 = visibleColumns().getColumn(expandCol());
    auto column2 = visibleColumns().getColumn(expandRow());

    auto *cellObj = getCellObj(expandRow(), expandCol());

    scatterPlot()->setXColumn(column1);
    scatterPlot()->setYColumn(column2);
    scatterPlot()->setGroupColumn(groupColumn());
    scatterPlot()->setTipColumns(visibleColumns());

    if (groupColumn().isValid()) {
      scatterPlot()->setSymbol(Symbol());
      scatterPlot()->setSymbolFillColor(Color::makePalette());
      scatterPlot()->setGroupSymbol(true);
    }
    else {
      if (cellObj)
        scatterPlot()->setSymbolFillColor(cellObj->pointColor());
      else
        scatterPlot()->setSymbolFillColor(Color::makePalette());

      scatterPlot()->setSymbol(scatterSymbol());
      scatterPlot()->setGroupSymbol(false);
    }

    scatterPlot()->setSymbolFillAlpha(scatterSymbolFillAlpha());

    scatterPlot()->setSymbolStrokeColor(scatterSymbolStrokeColor());
    scatterPlot()->setSymbolStrokeAlpha(scatterSymbolStrokeAlpha());

    scatterPlot()->setSymbolSize(calcScatterSymbolSize());

//  ColorInd ic;
//  if (cellObj && ! groupColumn().isValid())
//    ic = ColorInd(cellObj->row(), visibleColumns().count());
//  scatterPlot()->setParetoColorInd(ic);

    // TODO: more axis settings ?
    scatterPlot()->xAxis()->setGridLinesDisplayed(xAxis()->gridLinesDisplayed());
    scatterPlot()->yAxis()->setGridLinesDisplayed(yAxis()->gridLinesDisplayed());

//  scatterPlot()->xAxis()->setAutoFormat(isAutoFormatValues());
//  scatterPlot()->yAxis()->setAutoFormat(isAutoFormatValues());

    // TODO: more settings ?
    scatterPlot()->setDefaultPalette(defaultPalette());

    currentPlot_ = scatterPlot();
  }
  else {
    scatterPlot()->setXColumn(Column::makeRow());
    scatterPlot()->setYColumn(Column::makeRow());
  }

  if (isExpanded() && (expandRow() == expandCol())) {
    auto column = visibleColumns().getColumn(expandRow());

    auto *cellObj = getCellObj(expandRow(), expandRow());

    if      (diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION) {
      distributionPlot()->setValueColumns(Columns(column));
      distributionPlot()->setTipColumns(visibleColumns());

      if (cellObj)
        distributionPlot()->setBarFillColor(cellObj->barColor());
      else
        distributionPlot()->setBarFillColor(Color::makePalette());

      // TODO: more axis settings ?
      distributionPlot()->xAxis()->setGridLinesDisplayed(xAxis()->gridLinesDisplayed());
      distributionPlot()->yAxis()->setGridLinesDisplayed(yAxis()->gridLinesDisplayed());

//    distributionPlot()->valueAxis()->setAutoFormat(isAutoFormatValues());
//    distributionPlot()->countAxis()->setAutoFormat(false);

      distributionPlot()->valueAxis()->setTickLabelPlacement(
       CQChartsAxisTickLabelPlacement(CQChartsAxisTickLabelPlacement::Type::BETWEEN));
      distributionPlot()->countAxis()->setTickLabelPlacement(
       CQChartsAxisTickLabelPlacement(CQChartsAxisTickLabelPlacement::Type::MIDDLE));

      // TODO: more settings ?
      distributionPlot()->setDefaultPalette(defaultPalette());

      currentPlot_ = distributionPlot();
    }
    else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT) {
      boxPlot()->setValueColumns(Columns(column));
      boxPlot()->setTipColumns(visibleColumns());

      // TODO: more settings ?
      boxPlot()->setDefaultPalette(defaultPalette());

      currentPlot_ = boxPlot();
    }
    else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE) {
      piePlot()->setValueColumns(Columns(column));
      piePlot()->setTipColumns(visibleColumns());

      // TODO: more settings ?
      piePlot()->setDefaultPalette(defaultPalette());

      currentPlot_ = piePlot();
    }
  }
  else {
    distributionPlot()->setValueColumns(Columns(Column::makeRow()));
  }

  currentPlot_->setVisible(true);

  currentPlot_->zoomFull();

  view()->setCurrentPlot(currentPlot_);

  if (this != currentPlot_)
    this->setVisible(false);

  if (parallelPlot() != currentPlot_)
    parallelPlot()->setVisible(false);

  if (scatterPlot() != currentPlot_)
    scatterPlot()->setVisible(false);

  if (distributionPlot() != currentPlot_)
    distributionPlot()->setVisible(false);

  if (boxPlot() != currentPlot_)
    boxPlot()->setVisible(false);

  if (piePlot() != currentPlot_)
    piePlot()->setVisible(false);
}

//---

void
CQChartsSummaryPlot::
setShowBucketCount(bool b)
{
  CQChartsUtil::testAndSet(showBucketCount_, b, [&]() { drawObjs(); } );
}

void
CQChartsSummaryPlot::
setShowDistributionRange(bool b)
{
  CQChartsUtil::testAndSet(showDistributionRange_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsSummaryPlot::
setAutoScaleLabels(bool b)
{
  CQChartsUtil::testAndSet(autoScaleLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsSummaryPlot::
setHideFirstAxis(bool b)
{
  CQChartsUtil::testAndSet(hideFirstAxis_, b, [&]() { drawObjs(); } );
}

#if 0
void
CQChartsSummaryPlot::
setAutoFormatValues(bool b)
{
  CQChartsUtil::testAndSet(autoFormatValues_, b, [&]() {
    xAxis()->setAutoFormat(isAutoFormatValues());
    yAxis()->setAutoFormat(isAutoFormatValues());

    scatterPlot()->xAxis()->setAutoFormat(isAutoFormatValues());
    scatterPlot()->yAxis()->setAutoFormat(isAutoFormatValues());

    drawObjs();
  } );
}
#endif

//---

void
CQChartsSummaryPlot::
setDiagonalType(const DiagonalType &t)
{
  CQChartsUtil::testAndSet(diagonalType_, t, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setLowerDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(lowerDiagonalType_, t, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setUpperDiagonalType(const OffDiagonalType &t)
{
  CQChartsUtil::testAndSet(upperDiagonalType_, t, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
setOrientation(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(orientation_, o, [&]() {
    distributionPlot_->setOrientation(o);
    boxPlot_         ->setOrientation(o);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setInnerBorder(bool b)
{
  CQChartsUtil::testAndSet(innerBorder_, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
setSelectMode(const SelectMode &m)
{
  CQChartsUtil::testAndSet(selectMode_, m, [&]() {
    resetObjTips(); drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

CQChartsLength
CQChartsSummaryPlot::
calcScatterSymbolSize() const
{
  auto symbolSize = this->scatterSymbolSize();

  if (! symbolSize.isSet())
    symbolSize = Length::percent(2.5);

  return symbolSize;
}

//---

void
CQChartsSummaryPlot::
percentRefSize(const BBox &, double &refWidth, double &refHeight) const
{
  refWidth  = 1.0;
  refHeight = 1.0;
}

void
CQChartsSummaryPlot::
percentPixelRefSize(const BBox &, double &refWidth, double &refHeight) const
{
  refWidth  = windowToPixelWidth (1.0);
  refHeight = windowToPixelHeight(1.0);
}

//---

void
CQChartsSummaryPlot::
setBestFit(bool b)
{
  CQChartsUtil::testAndSet(bestFit_, b, [&]() {
    scatterPlot()->setBestFit(b);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setDensity(bool b)
{
  CQChartsUtil::testAndSet(density_, b, [&]() {
    distributionPlot()->setDensity(b);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setPareto(bool b)
{
  CQChartsUtil::testAndSet(paretoData_.visible, b, [&]() {
    scatterPlot()->setPareto(b);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setParetoWidth(const Length &l)
{
  CQChartsUtil::testAndSet(paretoData_.lineWidth, l, [&]() {
    scatterPlot     ()->setParetoStrokeWidth(l);
//  distributionPlot()->setParetoStrokeWidth(l); // origin symbol width

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setParetoLineColor(const Color &c)
{
  CQChartsUtil::testAndSet(paretoData_.lineColor, c, [&]() {
    scatterPlot()->setParetoStrokeColor(c);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setParetoOriginType(const ParetoOriginType &t)
{
  CQChartsUtil::testAndSet(paretoData_.originType, t, [&]() {
    scatterPlot     ()->setParetoOriginType(
      static_cast<CQChartsScatterPlot::ParetoOriginType>(t));
//  distributionPlot()->setParetoOriginType(
//    static_cast<CQChartsDistributionPlot::ParetoOriginType>(t));

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setParetoOriginColor(const Color &c)
{
  CQChartsUtil::testAndSet(paretoData_.originColor, c, [&]() {
    scatterPlot     ()->setParetoOriginColor(c);
//  distributionPlot()->setParetoOriginColor(c);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setParetoOriginSize(const Length &s)
{
  CQChartsUtil::testAndSet(paretoData_.originSize, s, [&]() {
//  scatterPlot     ()->setParetoOriginSize(s);
//  distributionPlot()->setParetoOriginSize(s);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setParetoOriginSymbol(const Symbol &s)
{
  CQChartsUtil::testAndSet(paretoData_.originSymbol, s, [&]() {
//  scatterPlot     ()->setParetoOriginSymbol(s);
//  distributionPlot()->setParetoOriginSymbol(s);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

int
CQChartsSummaryPlot::
numBuckets() const
{
  auto *modelData = currentModelData();
  auto *details   = (modelData ? modelData->details() : nullptr);

  return (details ? details->numBuckets() : numBuckets_);
}

void
CQChartsSummaryPlot::
setNumBuckets(int n)
{
  if (n != numBuckets_) {
    numBuckets_ = n;

    auto *modelData = currentModelData();
    auto *details   = (modelData ? modelData->details() : nullptr);

    if (details) details->setNumBuckets(n);

    updateRangeAndObjs();
  }
}

void
CQChartsSummaryPlot::
setDistributionMargin(const Length &l)
{
  CQChartsUtil::testAndSet(distributionMargin_, l, [&]() {
    distributionPlot()->setMargin(l);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
modelTypeChangedSlot(int modelInd)
{
  auto *modelData = currentModelData();

  if (modelData && modelData->isInd(modelInd))
    updateRangeAndObjs();
}

//---

void
CQChartsSummaryPlot::
setColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(columns_, c, [&]() {
    visibleColumns_ = columns_;

    resetColumnVisible();

    updateRangeAndObjs();

    Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setGroupColumn(const Column &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() {
    resetSetHidden();

    if (isExpanded() && (expandRow() != expandCol())) {
      scatterPlot()->setGroupColumn(groupColumn());
    }

    Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setColorColumn(const Column &c)
{
  if (c != colorColumn()) {
    CQChartsPlot::setColorColumn(c);
    scatterPlot_->setColorColumn(c);

    drawObjs(); Q_EMIT customDataChanged();
  }
}

void
CQChartsSummaryPlot::
setSymbolTypeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(symbolTypeColumn_, c, [&]() {
    scatterPlot_->setSymbolTypeColumn(c);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setSymbolSizeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(symbolSizeColumn_, c, [&]() {
    scatterPlot_->setSymbolSizeColumn(c);

    drawObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsSummaryPlot::
setBorder(const Margin &l)
{
  CQChartsUtil::testAndSet(border_, l, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setUndefRegionGray(double r)
{
  CQChartsUtil::testAndSet(undefRegionGray_, r, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setUndefRegionWidth(const Length &l)
{
  CQChartsUtil::testAndSet(undefRegionWidth_, l, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsSummaryPlot::
setOutsideGray(double r)
{
  CQChartsUtil::testAndSet(outsideGray_, r, [&]() { drawObjs(); } );
}

void
CQChartsSummaryPlot::
setOutsideAlpha(double r)
{
  CQChartsUtil::testAndSet(outsideAlpha_, r, [&]() { drawObjs(); } );
}

//---

CQChartsColumn
CQChartsSummaryPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if (name == "group") c = this->groupColumn();
  else                 c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsSummaryPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if (name == "group") this->setGroupColumn(c);
  else                 CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsSummaryPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "columns") c = this->columns();
  else                   c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsSummaryPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "columns") this->setColumns(c);
  else                   CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsSummaryPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "columns"    , "columns", "Columns");
  addProp("columns", "groupColumn", "group"  , "Group column");

  addProp("columns", "symbolTypeColumn", "symbolType", "Symbol type column");
  addProp("columns", "symbolSizeColumn", "symbolSize", "Symbol size column");

  // style
  addProp("options", "plotType"    , "plotType"    , "Plot type");
  addProp("options", "selectMode"  , "selectMode"  , "Select mode");
  addProp("options", "selectInside", "selectInside", "Select only runs inside region");

  addProp("options", "outsideGray" , "outsideGray" , "Outside gray factor");
  addProp("options", "outsideAlpha", "outsideAlpha", "Outside alpha");

  addProp("options", "autoScaleLabels", "autoScaleLabels",
          "Auto scale axis labels to fit");
  addProp("options", "hideFirstAxis"  , "hideFirstAxis",
          "Hide first cell axis");
#if 0
  addProp("options", "autoFormatValues", "autoFormatValues",
          "Auto format axis values to add numeric prefix");
#endif

  // x/y axis label text
  addProp("xaxis/text", "xLabels", "visible", "X labels visible");

  addTextProperties("xaxis/text", "xLabelText", "X label",
                    CQChartsTextOptions::ValueType::ALL);

  addProp("yaxis/text", "yLabels", "visible", "Y labels visible");

  addTextProperties("yaxis/text", "yLabelText", "Y label",
                    CQChartsTextOptions::ValueType::ALL);

  // plot cell
  addProp("cell/fill", "plotCellFilled", "visible", "Cell fill visible");

  addFillProperties("cell/fill", "plotCellFill", "Cell");

  addProp("cell/stroke", "plotCellStroked", "visible", "Cell stroke visible");

  addLineProperties("cell/stroke", "plotCellStroke", "Cell");

  // cell types
  addProp("cell", "diagonalType"     , "diagonal"     , "Diagonal cell type");
  addProp("cell", "lowerDiagonalType", "lowerDiagonal", "Lower Diagonal cell type");
  addProp("cell", "upperDiagonalType", "upperDiagonal", "Upper Diagonal cell type");
  addProp("cell", "border"           , "border"       , "Border Margin");

  // options
  addProp("options", "orientation", "orientation", "Plot orientation");
  addProp("options", "innerBorder", "innerBorder", "Draw border inside");

  // scatter plot
  addSymbolProperties("scatter/symbol", "scatter", "Scatter Symbol");

  // distribution plot
  addProp("distribution/fill", "distributionFilled", "visible", "Distribution fill visible");

  addFillProperties("distribution/fill", "distributionFill", "Distribution");

  addProp("distribution/stroke", "distributionStroked", "visible", "Distribution stroke visible");

  addLineProperties("distribution/stroke", "distributionStroke", "Distribution");

  addPropI("distribution", "showDistributionRange", "showRange",
           "Show range on distribution plot");
  addPropI("distribution", "showBucketCount", "showBucketCount",
           "Show bucket count on y axis");

  addProp("distribution", "numBuckets"        , "numBuckets"  , "Number of buckets");
  addProp("distribution", "barSelectTarget"   , "selectTarget", "Target for selection of bar");
  addProp("distribution", "distributionMargin", "margin"      , "Bar margin");

  // box plot
  addProp("box/fill", "boxPlotFilled", "visible", "Box fill visible");

  addFillProperties("box/fill", "boxPlotFill", "Box");

  addProp("box/stroke", "boxPlotStroked", "visible", "Box stroke visible");

  addLineProperties("box/stroke", "boxPlotStroke", "Box");

  addTextProperties("box/text", "boxPlotText", "Box value",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // pie plot
  addProp("pie/fill", "pieFilled", "visible", "Pie fill visible");
  addFillProperties("pie/fill", "pieFill", "Pie");

  addProp("pie/stroke", "pieStroked", "visible", "Pie stroke visible");
  addLineProperties("pie/stroke", "pieStroke", "Pie");

  // correlation plot
  addProp("correlation", "correlationMargin", "margin", "Correlation margin");

  addTextProperties("correlation/text", "correlationText", "Correlation",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // overlays
  addProp("overlays/bestFit", "bestFit", "visible", "Show best fit on scatter");
  addProp("overlays/density", "density", "visible", "Show density on distribution");

  addProp("overlays/pareto", "pareto"            , "visible"     , "Show pareto front on scatter");
  addProp("overlays/pareto", "paretoWidth"       , "lineWidth"   , "Pareto front line width");
  addProp("overlays/pareto", "paretoLineColor"   , "lineColor"   , "Pareto front line color");
  addProp("overlays/pareto", "paretoOriginType"  , "originType"  , "Pareto front origin type");
  addProp("overlays/pareto", "paretoOriginColor" , "originColor" , "Pareto front origin color");
  addProp("overlays/pareto", "paretoOriginSize"  , "originSize"  , "Pareto front origin size");
  addProp("overlays/pareto", "paretoOriginSymbol", "originSymbol", "Pareto front origin symbol");

  // region
  addProp("region/fill", "regionFilled", "visible", "Box fill visible");
  addFillProperties("region/fill", "regionFill", "Box");

  addProp("region/stroke", "regionStroked", "visible", "Box stroke visible");
  addLineProperties("region/stroke", "regionStroke", "Box");

  addProp("region", "regionSelectMargin", "selectMargin", "Region select margin");
  addProp("region", "regionSelectWidth" , "selectWidth" , "Region select width");
  addProp("region", "regionSelectFill"  , "selectFill"  , "Region select fill");
  addProp("region", "undefRegionGray"   , "undefGray"   , "Undefined region gray");
  addProp("region", "undefRegionWidth"  , "undefWidth"  , "Undefined region width");

  addProp("region", "regionEditStroke", "editStroke", "Region edit stroke");
  addProp("region", "regionEditFill"  , "editFill"  , "Region edit fill");
  addProp("region", "regionEditWidth" , "editWidth" , "Region edit width");
}

//---

void
CQChartsSummaryPlot::
setXLabels(bool b)
{
  CQChartsUtil::testAndSet(xLabels_, b, [&]() { drawObjs(); } );
}

void
CQChartsSummaryPlot::
setYLabels(bool b)
{
  CQChartsUtil::testAndSet(yLabels_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsSummaryPlot::
diagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text().toLower();

    if      (name == "none"        ) setDiagonalType(DiagonalType::NONE);
    else if (name == "distribution") setDiagonalType(DiagonalType::DISTRIBUTION);
    else if (name == "pie"         ) setDiagonalType(DiagonalType::PIE);
  }
  else
    setDiagonalType(DiagonalType::NONE);
}

void
CQChartsSummaryPlot::
upperDiagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text().toLower();

    if      (name == "none"       ) setUpperDiagonalType(OffDiagonalType::NONE);
    else if (name == "scatter"    ) setUpperDiagonalType(OffDiagonalType::SCATTER);
    else if (name == "correlation") setUpperDiagonalType(OffDiagonalType::CORRELATION);
  }
  else
    setUpperDiagonalType(OffDiagonalType::NONE);
}

void
CQChartsSummaryPlot::
lowerDiagonalTypeSlot(bool b)
{
  if (b) {
    auto name = qobject_cast<QAction *>(sender())->text();

    if      (name == "None"       ) setLowerDiagonalType(OffDiagonalType::NONE);
    else if (name == "Scatter"    ) setUpperDiagonalType(OffDiagonalType::SCATTER);
    else if (name == "Correlation") setUpperDiagonalType(OffDiagonalType::CORRELATION);
  }
  else
    setLowerDiagonalType(OffDiagonalType::NONE);
}

//---

CQChartsGeom::Range
CQChartsSummaryPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSummaryPlot::calcRange");

  //---

  const_cast<CQChartsSummaryPlot *>(this)->updateVisibleColumns();

  // square (nc, nc)
  int nc = std::max(visibleColumns().count(), 1);

  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(nc , nc );

  //---

  return dataRange;
}

void
CQChartsSummaryPlot::
updateVisibleColumns()
{
  visibleColumns_ = Columns();

  int nc = columns().count();

  for (int ic = 0; ic < nc; ++ic) {
    if (! isColumnVisible(ic))
      continue;

    visibleColumns_.addColumn(columns().getColumn(ic));
  }
}

bool
CQChartsSummaryPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSummaryPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  int nc = visibleColumns().count();

  for (int ir = 0; ir < nc; ++ir) {
    for (int ic = 0; ic < nc; ++ic) {
      auto *obj = createCellObj(cellBBox(ir, ic), ir, ic);

      obj->connectDataChanged(this, SLOT(updateSlot()));

      objs.push_back(obj);
    }
  }

  //---

  for (auto *obj : objs) {
    auto *cellObj = dynamic_cast<CellObj *>(obj);

    if (cellObj)
      cellObj->initCoords();
  }

  return true;
}

CQChartsGeom::BBox
CQChartsSummaryPlot::
cellBBox(int row, int col) const
{
  int nc = visibleColumns().count();

  double pymax = nc - row;
  double pymin = pymax - 1;

  double pxmin = col;
  double pxmax = pxmin + 1;

  return BBox(pxmin, pymin, pxmax, pymax);
}

//---

QString
CQChartsSummaryPlot::
posStr(const Point &w) const
{
  auto *obj = cellObjAtPoint(w);
  if (! obj) return "";

  auto p = obj->parentToPlot(w);

  return xStr(p.x) + " " + yStr(p.y);
}

QString
CQChartsSummaryPlot::
getSelText() const
{
  auto n = numModelSelectedRows();
  if (n == 0) return QString();

  return QString::number(n);
}

//---

void
CQChartsSummaryPlot::
updateRootChild(Plot *plot)
{
  auto *scatter = dynamic_cast<ScatterPlot *>(plot);

  if (scatter) {
    if (scatter->isPareto() != isPareto())
      setPareto(scatter->isPareto());

    if (scatter->isBestFit() != isBestFit())
      setBestFit(scatter->isBestFit());
  }
}

//---

bool
CQChartsSummaryPlot::
addMenuItems(QMenu *menu, const Point &)
{
  menuObj_ = selectedCellObj();

  if (! menuObj_) {
    auto gpos = view()->menuPos();
    auto pos  = view()->mapFromGlobal(QPointF(gpos.x, gpos.y).toPoint());

    auto wpos = pixelToWindow(Point(pos));

    menuObj_ = cellObjAtPoint(wpos);
  }

  if (menuObj_) {
    menu->addSeparator();

    addMenuAction(menu, "Expand", SLOT(expandSlot()));
  }

  return true;
}

//---

bool
CQChartsSummaryPlot::
pointSelect(const Point &, SelMod)
{
  return true;
}

bool
CQChartsSummaryPlot::
rectSelect(const BBox &r, SelMod)
{
  auto p = r.getCenter();

  auto *obj = cellObjAtPoint(p);
  if (! obj) return false;

  auto p1 = obj->parentToPlot(r.getLL());
  auto p2 = obj->parentToPlot(r.getUR());

  auto r1 = BBox(p1, p2);

  auto cellType = obj->getCellType();

  if      (cellType == CellType::SCATTER) {
    auto column1 = obj->colColumn();
    auto column2 = obj->rowColumn();

    setColumnRange(column1, r1.getXMin(), r1.getXMax());
    setColumnRange(column2, r1.getYMin(), r1.getYMax());
  }
  else if (cellType == CellType::DISTRIBUTION) {
    auto column = obj->rowColumn();

    setColumnRange(column, r1.getXMin(), r1.getXMax());
  }
  else if (cellType == CellType::BOXPLOT) {
    auto column = obj->rowColumn();

    setColumnRange(column, r1.getYMin(), r1.getYMax());
  }
  else
    return false;

  updateColumnRanges();

  return true;
}

//---

CQChartsSummaryCellObj *
CQChartsSummaryPlot::
selectedCellObj() const
{
  PlotObjs objs;

  selectedPlotObjs(objs);
  if (objs.empty()) return nullptr;

  auto *obj = *objs.begin();

  return dynamic_cast<CQChartsSummaryCellObj *>(obj);
}

CQChartsSummaryPlot::CellObj *
CQChartsSummaryPlot::
cellObjAtPoint(const Point &p) const
{
  PlotObjs objs;

  plotObjsAtPoint(p, objs, Constraints::SELECTABLE);

  return dynamic_cast<CellObj *>(! objs.empty() ? *objs.begin() : nullptr);
}

bool
CQChartsSummaryPlot::
anyColumnRange() const
{
  return ! columnRange_.empty();
}

bool
CQChartsSummaryPlot::
hasColumnRange(const Column &c) const
{
  auto pc = columnRange_.find(c);

  return (pc != columnRange_.end());
}

CQChartsSummaryPlot::MinMax
CQChartsSummaryPlot::
columnRange(const Column &c) const
{
  auto pc = columnRange_.find(c);

  return (pc != columnRange_.end() ? (*pc).second : MinMax());
}

void
CQChartsSummaryPlot::
setColumnRange(const Column &c, double min, double max)
{
  double cmin, cmax;
  if (calcColumnRange(c, cmin, cmax)) {
    min = std::max(min, cmin);
    max = std::min(max, cmax);
  }

  columnRange_[c] = MinMax(min, max);
}

void
CQChartsSummaryPlot::
resetColumnRange(const Column &c)
{
  columnRange_.erase(c);
}

void
CQChartsSummaryPlot::
clearColumnRanges()
{
  columnRange_.clear();

  updateColumnRanges();
}

bool
CQChartsSummaryPlot::
calcColumnRange(const Column &c, double &min, double &max) const
{
  min = 0.0;
  max = 1.0;

  auto *details = columnDetails(c);
  if (! details) return false;

  if (details->isNumeric()) {
    bool ok;
    min = CQChartsVariant::toReal(details->minValue(), ok);
    max = CQChartsVariant::toReal(details->maxValue(), ok);
  }
  else {
    min = 0.0;
    max = details->numUnique();
  }

  // adjust to bucket range (might be wider than data range)
  double bmin = min, bmax = max;

  if (diagonalType() == DiagonalType::DISTRIBUTION && details->isNumeric()) {
    BucketCount bucketCount;
    int         maxCount = 0;

    calcBucketCounts(c, bucketCount, maxCount, bmin, bmax);

    min = bmin; max = bmax;
  }

  return true;
}

void
CQChartsSummaryPlot::
updateColumnRanges(bool notify)
{
  updateSelectedRows();

  drawObjs();

  if (notify)
    Q_EMIT customDataChanged();
}

void
CQChartsSummaryPlot::
updateSelectedRows() const
{
  rangeSelectedRows_.clear();

  int nc = visibleColumns().count();

  for (int ic = 0; ic < nc; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    auto *details = columnDetails(column);
    if (! details) continue;

    int n = details->numRows();

    auto range = columnRange(column);

    for (uint i = 0; i < uint(n); ++i) {
      if (range.isSet()) {
        double x = 0.0;

        if (details->isNumeric()) {
          bool ok;
          x = CQChartsVariant::toReal(details->value(i), ok);
          if (! ok) continue;
        }
        else
          x = details->uniqueId(details->value(i));

        if (range.insideHalfOpen(x))
          rangeSelectedRows_[i].insert(ic);
      }
      else
        rangeSelectedRows_[i].insert(ic);
    }
  }

  //---

  clearModelSelectedRows();

  auto *modelData = currentModelData();

  if (modelData) {
    const auto &sel = modelData->selection();

    auto indices = sel.indexes();

    for (int i = 0; i < indices.size(); ++i) {
      const auto &ind = indices[i];

      auto ind1 = normalizeIndex(ind);

      addModelSelectedRow(ind1.row(), ind1.column(), false);
    }
  }

  view()->updateSelText();
}

void
CQChartsSummaryPlot::
selectColumnRanges()
{
  auto *modelData = currentModelData();

  QItemSelection sel;

  int nc = visibleColumns().count();

  for (int ic = 0; ic < nc; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    auto range = columnRange(column);
    if (! range.isSet()) continue;

    auto *details = columnDetails(column);
    if (! details) continue;

    int n = details->numRows();

    std::set<int> rows;

    for (uint i = 0; i < uint(n); ++i) {
      double x = 0.0;

      if (details->isNumeric()) {
        bool ok;
        x = CQChartsVariant::toReal(details->value(i), ok);
        if (! ok) continue;
      }
      else
        x = details->uniqueId(details->value(i));

      if (range.insideHalfOpen(x))
        rows.insert(i);
    }

    for (const int &row : rows) {
      auto ind = modelIndex(row, column, QModelIndex());

      sel.select(ind, ind);
    }
  }

  modelData->select(sel);

  updateSelectedRows();

  const_cast<CQChartsSummaryPlot *>(this)->drawObjs();
}

bool
CQChartsSummaryPlot::
isRangeSelectedRow(int r) const
{
  if (columnRange_.empty()) return false;

  auto pr = rangeSelectedRows_.find(r);
  if (pr == rangeSelectedRows_.end()) return false;

  return (int((*pr).second.size()) == nc_);
}

int
CQChartsSummaryPlot::
numRangeSelectedRows() const
{
  int n = 0;

  if (! columnRange_.empty()) {
    for (const auto &pr : rangeSelectedRows_) {
      if (int(pr.second.size()) == nc_)
        ++n;
    }
  }

  return n;
}

//---

void
CQChartsSummaryPlot::
clearModelSelectedRows(bool update) const
{
  modelSelectedRows_.clear();

  if (update)
    view()->updateSelText();
}

void
CQChartsSummaryPlot::
addModelSelectedRow(int row, int col, bool update) const
{
  modelSelectedRows_[row].insert(col);

  if (update)
    view()->updateSelText();
}

bool
CQChartsSummaryPlot::
isModelSelectedRow(int r) const
{
  auto pr = modelSelectedRows_.find(r);
  if (pr == modelSelectedRows_.end()) return false;

  return true;
}

uint
CQChartsSummaryPlot::
numModelSelectedRows() const
{
  return modelSelectedRows_.size();
}

#if 0
void
CQChartsSummaryPlot::
updateSelectionObjects(const std::set<QModelIndex> &selectIndices)
{
  clearModelSelectedRows(/*update*/false);

  for (const auto &ind : selectIndices) {
    // selection is in base model, data in proxy model
    auto ind1 = unnormalizeIndex(ind);

    addModelSelectedRow(ind1.row(), ind1.column(), /*update*/false);
  }

  if (currentPlot_ != this)
    currentPlot_->updateSelectionObjects(selectIndices);

  view()->updateSelText();

  drawObjs();
}
#endif

void
CQChartsSummaryPlot::
updateSelectionRows() const
{
  auto *modelData = currentModelData();

  QItemSelection sel;

  for (const auto &rowCols : modelSelectedRows_) {
    auto r = rowCols.first;

    for (const auto &c : rowCols.second) {
      auto ind1 = modelIndex(r, Column(c), QModelIndex());

      sel.select(ind1, ind1);
    }
  }

  modelData->select(sel);
}

//---

void
CQChartsSummaryPlot::
clearSelection() const
{
  selectCellPoint(nullptr, -1, /*add*/false);
}

void
CQChartsSummaryPlot::
selectCellPoint(CellObj *obj, int ind, bool add) const
{
  auto *modelData = currentModelData();

  QItemSelection sel;

  if (add)
    sel = modelData->selection();

  if (ind >= 0) {
    auto ind1 = modelIndex(ind, obj->rowColumn(), QModelIndex());
    auto ind2 = modelIndex(ind, obj->colColumn(), QModelIndex());

    sel.select(ind1, ind1);
    sel.select(ind2, ind2);
  }

  modelData->select(sel);

  //---

  auto *th = const_cast<CQChartsSummaryPlot *>(this);

  th->updateSelectedRows();

  th->drawObjs();
}

#if 0
void
CQChartsSummaryPlot::
selectCellRect(CellObj *obj, const MinMax &minMax, bool add) const
{
  auto *modelData = currentModelData();

  auto column = obj->rowColumn();

  auto *details = columnDetails(column);
  if (! details) return;

  int n = details->numRows();

  std::set<int> rows;

  for (uint i = 0; i < uint(n); ++i) {
    double x = 0.0;

    if (details->isNumeric()) {
      bool ok;
      x = CQChartsVariant::toReal(details->value(i), ok);
      if (! ok) continue;
    }
    else
      x = details->uniqueId(details->value(i));

    if (minMax.insideHalfOpen(x))
      rows.insert(i);
  }

  QItemSelection sel;

  if (add)
    sel = modelData->selection();

  for (const int &row : rows) {
    auto ind1 = modelIndex(row, column, QModelIndex());

    sel.select(ind1, ind1);
  }

  modelData->select(sel);

  updateSelectedRows();

  const_cast<CQChartsSummaryPlot *>(this)->drawObjs();
}
#endif

//---

void
CQChartsSummaryPlot::
expandSlot()
{
  auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(menuObj_);
  if (! cellObj) return;

  expandCell(cellObj);
}

void
CQChartsSummaryPlot::
expandCell(CellObj *cellObj)
{
  expandRow_ = cellObj->row();
  expandCol_ = cellObj->col();

  setExpanded(true);
}

void
CQChartsSummaryPlot::
collapseCell()
{
  if (isExpanded()) {
    if (expandRow() != expandCol()) {
      scatterPlot()->collapseRoot();
    }
    else {
      if      (diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION)
        distributionPlot()->collapseRoot();
      else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT)
        boxPlot()->collapseRoot();
      else if (diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE)
        piePlot()->collapseRoot();
    }
  }
}

void
CQChartsSummaryPlot::
notifyCollapse()
{
  setPlotType(PlotType::MATRIX, /*update*/false);

  setExpanded(false);
}

bool
CQChartsSummaryPlot::
plotToSubPlot(int r, int c, const Point &p, Point &pp) const
{
  auto *cellObj = getCellObj(r, c);
  if (! cellObj) return false;

  pp = cellObj->parentToPlot(p);

  return true;
}

bool
CQChartsSummaryPlot::
subPlotToPlot(int r, int c, const Point &p, Point &pp) const
{
  auto *cellObj = getCellObj(r, c);
  if (! cellObj) return false;

  pp = cellObj->plotToParent(p);

  return true;
}

CQChartsSummaryPlot::Point
CQChartsSummaryPlot::
plotToSubPlot(const Point &p) const
{
  auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(currentObj());

  auto p1 = (cellObj ? cellObj->parentToPlot(p) : p);

  return p1;
}

CQChartsSummaryPlot::Point
CQChartsSummaryPlot::
subPlotToPlot(const Point &p) const
{
  auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(currentObj());

  auto p1 = (cellObj ? cellObj->plotToParent(p) : p);

  return p1;
}

//------

void
CQChartsSummaryPlot::
postResize()
{
  CQChartsPlot::postResize();

  updateRangeAndObjs();
}

//------

bool
CQChartsSummaryPlot::
hasBackground() const
{
  return true;
}

void
CQChartsSummaryPlot::
execDrawBackground(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryPlot::execDrawBackground");

  //---

  xLabelBBox_ = BBox();
  yLabelBBox_ = BBox();

  nc_ = visibleColumns().count();

  //---

  double fontScale = 1.0;

  if (isAutoScaleLabels()) {
    setPainterFont(device, this->font());

    if (isXLabels() || isYLabels()) {
      auto xm = pixelToWindowWidth (4.0);
      auto ym = pixelToWindowHeight(4.0);

      for (int ic = 0; ic < nc_; ++ic) {
        auto column = visibleColumns().getColumn(ic);

        bool ok;
        auto str = modelHHeaderString(column, ok);

        if (isXLabels()) {
          auto textOptions = xLabelTextOptions();

          auto tsize = CQChartsRotatedText::calcBBox(0.0, 0.0, str, device->font(),
                                                     textOptions).size();

          if (tsize.width() - xm > 1.0) {
            auto xf = 1.0/(tsize.width() - xm);

            fontScale = std::min(fontScale, xf);
          }
        }

        if (isYLabels()) {
          auto textOptions = yLabelTextOptions();

          auto tsize = CQChartsRotatedText::calcBBox(0.0, 0.0, str, device->font(),
                                                     textOptions).size();

          if (tsize.height() - ym > 1.0) {
            auto yf = 1.0/(tsize.height() - ym);

            fontScale = std::min(fontScale, yf);
          }
        }
      }
    }
  }

  //---

  ColorInd colorInd;

  auto fc = interpDataFillColor(colorInd);

  PenBrush penBrush;

  setPenBrush(penBrush, PenData(false), BrushData(isDataFilled(), fc, dataFillAlpha()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawRect(BBox(0, 0, nc_, nc_));

  //---

  for (int ic = 0; ic < nc_; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    auto bbox = cellBBox(ic, ic);

    bool ok;
    auto str = modelHHeaderString(column, ok);

    //---

    if (isXLabels()) {
      auto xstr = str;

      if (isAutoScaleLabels())
        device->setFont(CQChartsUtil::scaleFontSize(device->font(), fontScale), /*scale*/false);
      else
        setPainterFont(device, xLabelTextFont());

      auto textOptions = xLabelTextOptions(device);

      textOptions.clipLength = lengthPixelWidth(Length::plot(1.0));
      textOptions.clipped    = true;

      textOptions = adjustTextOptions(textOptions);

      //---

      auto tsize = CQChartsRotatedText::calcBBox(0.0, 0.0, xstr, device->font(),
                                                 textOptions).size();

      //---

      ColorInd colorInd;
      PenBrush tpenBrush;

      setXLabelTextPenBrush(tpenBrush, colorInd);

      CQChartsDrawUtil::setPenBrush(device, tpenBrush);

      //---

      auto thh = pixelToWindowHeight(tsize.height());

      BBox tbbox;

      if (xAxis()->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
        tbbox = BBox(bbox.getXMin(), -thh, bbox.getXMax(), 0.0);
      else
        tbbox = BBox(bbox.getXMin(), nc_, bbox.getXMax(), nc_ + thh);

      //---

      if (tbbox.isValid())
        CQChartsDrawUtil::drawTextInBox(device, tbbox, xstr, textOptions);

      //---

      xLabelBBox_ += tbbox;
    }

    //---

    if (isYLabels()) {
      auto ystr = str;

      if (diagonalType() == DiagonalType::DISTRIBUTION && isShowBucketCount()) {
        auto *details = columnDetails(column);

        if (details->isNumeric()) {
          BucketCount bucketCount;
          int         maxCount = 0;
          double      bmin = 0.0, bmax = 0.0;

          calcBucketCounts(column, bucketCount, maxCount, bmin, bmax);

          ystr += QString(" (%1)").arg(maxCount);
        }
      }

      //---

      if (isAutoScaleLabels())
        device->setFont(CQChartsUtil::scaleFontSize(device->font(), fontScale), /*scale*/false);
      else
        setPainterFont(device, yLabelTextFont());

      auto textOptions = yLabelTextOptions(device);

      textOptions.clipLength = lengthPixelHeight(Length::plot(1.0));
      textOptions.clipped    = true;

      textOptions = adjustTextOptions(textOptions);

      //---

      auto tsize = CQChartsRotatedText::calcBBox(0.0, 0.0, ystr, device->font(),
                                                 textOptions).size();

      //---

      ColorInd colorInd;
      PenBrush tpenBrush;

      setYLabelTextPenBrush(tpenBrush, colorInd);

      CQChartsDrawUtil::setPenBrush(device, tpenBrush);

      //---

      auto thw = pixelToWindowWidth(tsize.width());

      BBox tbbox;

      if (yAxis()->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
        tbbox = BBox(-thw, bbox.getYMin(), 0.0, bbox.getYMax());
      else
        tbbox = BBox(nc_, bbox.getYMin(), nc_ + thw, bbox.getYMax());

      //---

      if (tbbox.isValid())
        CQChartsDrawUtil::drawTextInBox(device, tbbox, ystr, textOptions);

      //---

      yLabelBBox_ += tbbox;
    }
  }
}

bool
CQChartsSummaryPlot::
hasBgAxes() const
{
  return false;
}

void
CQChartsSummaryPlot::
redrawAxis(CQChartsAxis *, bool wait)
{
  if (wait)
    drawObjs();
  else
    invalidateLayers();
}

void
CQChartsSummaryPlot::
drawXAxis(PaintDevice *) const
{
  // drawn by cell
}

void
CQChartsSummaryPlot::
drawYAxis(PaintDevice *) const
{
  // drawn by cell
}

void
CQChartsSummaryPlot::
drawXGrid(PaintDevice *) const
{
  // drawn by cell
}

void
CQChartsSummaryPlot::
drawYGrid(PaintDevice *) const
{
  // drawn by cell
}

bool
CQChartsSummaryPlot::
hasOverlay() const
{
  return true;
}

void
CQChartsSummaryPlot::
execDrawOverlay(PaintDevice *device) const
{
  for (const auto &plotObj : plotObjects()) {
    auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (! cellObj) continue;

    cellObj->drawOverlay(device);
  }
}

//------

CQChartsGeom::BBox
CQChartsSummaryPlot::
fitBBox() const
{
  nc_ = visibleColumns().count();

#if 1
  auto bbox = BBox(0, 0, nc_, nc_);

  if (xLabelBBox_.isValid()) bbox += xLabelBBox_;
  if (yLabelBBox_.isValid()) bbox += yLabelBBox_;

  for (const auto &plotObj : plotObjects()) {
    auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (! cellObj) continue;

    bbox += cellObj->fitBBox();
  }
#else
  auto xfont = view()->viewFont(xLabelTextFont());
  auto yfont = view()->viewFont(yLabelTextFont());

  auto xTextOptions = xLabelTextOptions();
  auto yTextOptions = yLabelTextOptions();

  //---

  double maxWidth  = 0.0;
  double maxHeight = 0.0;

  for (int ic = 0; ic < nc_; ++ic) {
    auto column = visibleColumns().getColumn(ic);

    bool ok;
    auto str = modelHHeaderString(column, ok);

  //auto xtsize = CQChartsDrawUtil::calcTextSize(str, xfont, xTextOptions);
    auto xtsize = CQChartsRotatedText::calcBBox(0.0, 0.0, str, xfont, xTextOptions).size();

  //auto ytsize = CQChartsDrawUtil::calcTextSize(str, yfont, yTextOptions);
    auto ytsize = CQChartsRotatedText::calcBBox(0.0, 0.0, str, yfont, yTextOptions).size();

    maxWidth  = std::max(maxWidth , pixelToWindowWidth (ytsize.width ()));
    maxHeight = std::max(maxHeight, pixelToWindowHeight(xtsize.height()));
  }

  if (maxWidth  > 1.0) maxWidth  = 1.0;
  if (maxHeight > 1.0) maxHeight = 1.0;

  //---

  auto bbox = CQChartsPlot::fitBBox();

  bbox += Point(-maxWidth, -maxHeight);
#endif

  return bbox;
}

//-----

void
CQChartsSummaryPlot::
resetColumnVisible()
{
  columnVisible_.clear();
}

bool
CQChartsSummaryPlot::
isColumnVisible(int ic) const
{
  auto p = columnVisible_.find(ic);
  if (p == columnVisible_.end()) return true;

  return (*p).second;
}

void
CQChartsSummaryPlot::
setColumnVisible(int ic, bool visible)
{
  columnVisible_[ic] = visible;

  if (! isExpanded() && (plotType() == PlotType::PARALLEL))
    parallelPlot()->setYColumnVisible(ic, visible);

  updateRangeAndObjs();

  updateVisibleColumns();

  updateSelectedRows();

  Q_EMIT customDataChanged();
}

//------

void
CQChartsSummaryPlot::
calcBucketCounts(const Column &column, BucketCount &bucketCount, int &maxCount,
                 double &rmin, double &rmax) const
{
  bucketCount = BucketCount();
  maxCount    = 0;
  rmin        = 0.0;
  rmax        = 0.0;

  auto *details = columnDetails(column);
  if (! details) return;

  bool ok;
  double min = CQChartsVariant::toReal(details->minValue(), ok);
  double max = CQChartsVariant::toReal(details->maxValue(), ok);

  int n = details->numRows();

  for (uint i = 0; i < uint(n); ++i) {
    auto value = details->value(i);
    if (! value.isValid()) continue;

    int bucket = details->bucket(value);

    bucketCount[bucket].insert(i);
  }

  maxCount = 0;

  rmin = min;
  rmax = max;

  for (const auto &pb : bucketCount) {
    auto n = pb.second.size();

    maxCount = std::max(maxCount, int(n));

    QVariant vmin, vmax;

    details->bucketRange(pb.first, vmin, vmax);

    bool ok;
    double rmin1 = CQChartsVariant::toReal(vmin, ok);
    double rmax1 = CQChartsVariant::toReal(vmax, ok);

    rmin = std::min(rmin, rmin1);
    rmax = std::max(rmax, rmax1);
  }
}

void
CQChartsSummaryPlot::
calcValueCounts(int ic, ValueCounts &valueCounts, int &maxCount) const
{
  auto column = visibleColumns().getColumn(ic);

  calcValueCounts(column, valueCounts, maxCount);
}

void
CQChartsSummaryPlot::
calcValueCounts(const Column &column, ValueCounts &valueCounts, int &maxCount) const
{
  valueCounts = ValueCounts();
  maxCount    = 0;

  auto *details = columnDetails(column);
  if (! details) return;

  maxCount = 0;

  valueCounts = details->uniqueValueCounts();

  for (const auto &vc : valueCounts)
    maxCount = std::max(maxCount, vc.second);
}

//------

bool
CQChartsSummaryPlot::
canRectSelect() const
{
  return CQChartsPlot::canRectSelect();
}

bool
CQChartsSummaryPlot::
handleSelectPress(const Point &p, SelMod selMod)
{
  if (selectMode() == SelectMode::CELL)
    return CQChartsPlot::handleSelectPress(p, selMod);

  //---

  if (selMod == CQChartsSelMod::REMOVE)
    return CQChartsPlot::handleSelectPress(p, CQChartsSelMod::REPLACE);

  if (selMod != CQChartsSelMod::ADD) {
    bool changed { false };
    deselectAll1(SelectTypes::ALL, changed);
  }

  if (insideObject() && ! dynamic_cast<CQChartsSummaryCellObj *>(insideObject()))
    return CQChartsPlot::handleSelectPress(p, selMod);

  //---

  selectPressed_ = true;
  selectAdd_     = (selMod == SelMod::ADD);

  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  CQChartsSummaryCellObj *cellObj = nullptr;

  for (auto *plotObj : plotObjs) {
    cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (cellObj) break;
  }

  if (cellObj)
    cellObj->handleSelectPress(p, selMod);
  else
    clearSelection();

  return true;
}

bool
CQChartsSummaryPlot::
handleSelectMove(const Point &p, Constraints constraints, bool first)
{
  if (selectMode() == SelectMode::CELL)
    return CQChartsPlot::handleSelectMove(p, constraints, first);

  //---

  bool changed = false;

  for (const auto &plotObj : plotObjects()) {
    auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (! cellObj) continue;

    bool rc1 = cellObj->resetInside();
    bool rc2 = cellObj->updateInside();

    if (rc1 || rc2)
      changed = true;
  }

  if (changed)
    invalidateOverlay();

  //---

  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  CQChartsSummaryCellObj *cellObj = nullptr;

  for (auto *plotObj : plotObjs) {
    cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (cellObj) break;
  }

  //---

  // get annotations at point
  Annotations annotations;

  annotationsAtPoint(p, annotations, Constraints::SELECTABLE);

  //---

  Objs objs;

  if (cellObj)
    objs.push_back(cellObj);

  for (auto *annotation : annotations)
    objs.push_back(annotation);

  setInsideObjects(p, objs);

  //---

  if (cellObj) {
    if (! selectPressed_ && view()->mode() == View::Mode::EDIT)
      cellObj->updateRangeInside(p);

    cellObj->handleSelectMove(p, constraints, first);
  }

  //---

  QString objText;

  if (cellObj) {
    auto objText1 = cellObj->objText();

    if (objText1 != "")
      objText += objText1;
  }

  if (objText != "")
    view()->setStatusText(objText);

  return true;
}

bool
CQChartsSummaryPlot::
handleSelectRelease(const Point &p)
{
  if (selectMode() == SelectMode::CELL)
    return CQChartsPlot::handleSelectRelease(p);

  if (insideObject() && ! dynamic_cast<CQChartsSummaryCellObj *>(insideObject()))
    return CQChartsPlot::handleSelectRelease(p);

  //---

  selectPressed_ = false;

  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  CQChartsSummaryCellObj *cellObj = nullptr;

  for (auto *plotObj : plotObjs) {
    cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (cellObj) break;
  }

  if (cellObj)
    cellObj->handleSelectRelease(p, selectAdd_);

  return true;
}

bool
CQChartsSummaryPlot::
handleSelectDoubleClick(const Point &p, SelMod /*selMod*/)
{
  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  CQChartsSummaryCellObj *cellObj = nullptr;

  for (auto *plotObj : plotObjs) {
    cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (cellObj) break;
  }

  if (cellObj)
    expandCell(cellObj);

  return true;
}

//------

bool
CQChartsSummaryPlot::
handleEditPress(const Point &, const Point &p, bool)
{
  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  modifyCellObj_ = nullptr;

  for (auto *plotObj : plotObjs) {
    modifyCellObj_ = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (modifyCellObj_) break;
  }

  if (modifyCellObj_)
    modifyCellObj_->handleEditPress(p);

  return true;
}

bool
CQChartsSummaryPlot::
handleEditMove(const Point &, const Point &p, bool)
{
  if (modifyCellObj_)
    modifyCellObj_->handleEditMove(p);

  return true;
}

bool
CQChartsSummaryPlot::
handleEditRelease(const Point &, const Point &p)
{
  if (modifyCellObj_)
    modifyCellObj_->handleEditRelease(p);

  return true;
}

//---

CQChartsSummaryPlot::CellObj *
CQChartsSummaryPlot::
getCellObj(int r, int c) const
{
  for (const auto &plotObj : plotObjects()) {
    auto *cellObj = dynamic_cast<CQChartsSummaryCellObj *>(plotObj);
    if (! cellObj) continue;

    if (cellObj->row() == r && cellObj->col() == c)
      return cellObj;
  }

  return nullptr;
}

//------

CQChartsSummaryCellObj *
CQChartsSummaryPlot::
createCellObj(const BBox &bbox, int row, int col) const
{
  return new CQChartsSummaryCellObj(this, bbox, row, col);
}

//---

CQChartsPlotCustomControls *
CQChartsSummaryPlot::
createCustomControls()
{
  auto *controls = new CQChartsSummaryPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsSummaryCellObj::
CQChartsSummaryCellObj(const SummaryPlot *summaryPlot, const BBox &bbox, int row, int col) :
 CQChartsPlotObj(const_cast<SummaryPlot *>(summaryPlot), bbox, ColorInd(), ColorInd(), ColorInd()),
 summaryPlot_(summaryPlot), row_(row), col_(col)
{
}

CQChartsColumn
CQChartsSummaryCellObj::
rowColumn() const
{
  return visibleColumn(row_);
}

CQChartsColumn
CQChartsSummaryCellObj::
colColumn() const
{
  return visibleColumn(col_);
}

CQChartsColumn
CQChartsSummaryCellObj::
visibleColumn(int i) const
{
  return summaryPlot_->visibleColumns().getColumn(i);
}

QString
CQChartsSummaryCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(row_).arg(col_);
}

bool
CQChartsSummaryCellObj::
dynamicTipId() const
{
  return (summaryPlot_->selectMode() == CQChartsSummaryPlot::SelectMode::DATA);
}

QString
CQChartsSummaryCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  if (summaryPlot_->selectMode() == CQChartsSummaryPlot::SelectMode::DATA) {
    if      (! selectPointDatas_.empty()) {
      auto selectPointDataInd = getCurrentSelectPointInd();
      if (selectPointDataInd < 0 || selectPointDataInd >= int(pointDatas_.size()))
        return QString();

      const auto &selectPointData = pointDatas_[selectPointDataInd];

      auto column1 = colColumn();
      auto column2 = rowColumn();

      auto name1 = summaryPlot_->columnHeaderName(column1, /*tip*/true, "X");
      auto name2 = summaryPlot_->columnHeaderName(column2, /*tip*/true, "Y");

      auto *details1 = summaryPlot_->columnDetails(column1);

      if (details1) {
        if (details1->isNumeric())
          tableTip.addTableRow(name1, selectPointData.p.x);
        else
          tableTip.addTableRow(name1, details1->value(selectPointData.ind));
      }

      auto *details2 = summaryPlot_->columnDetails(column2);

      if (details2) {
        if (details2->isNumeric())
          tableTip.addTableRow(name2, selectPointData.p.y);
        else
          tableTip.addTableRow(name2, details2->value(selectPointData.ind));
      }

      for (const auto &c : summaryPlot_->tipColumns().columns()) {
        auto *details = summaryPlot_->columnDetails(c);
        if (! details) continue;

        auto name = summaryPlot_->columnHeaderName(c, /*tip*/true);

        tableTip.addTableRow(name, details->value(selectPointDataInd));
      }

      return tableTip.str();
    }
    else if (selectRectData_) {
      auto column = rowColumn();

      auto *details = summaryPlot_->columnDetails(column);

      auto name = summaryPlot_->columnHeaderName(column, /*tip*/true, "X");

      if (details && details->isNumeric())
        tableTip.addTableRow(name, QString("[%1,%2)").arg(selectRectData_->bbox.getXMin()).
                                                      arg(selectRectData_->bbox.getXMax()));
      else
        tableTip.addTableRow(name, selectRectData_->name);

      if (summaryPlot_->orientation() == Qt::Vertical)
        tableTip.addTableRow("Count", selectRectData_->bbox.getYMax());
      else
        tableTip.addTableRow("Count", selectRectData_->bbox.getXMax());

      return tableTip.str();
    }
  }

  auto cellType = getCellType();

  // off diagonal
  if (row_ != col_) {
    // column info
    auto column1 = colColumn();
    auto column2 = rowColumn();

    bool ok;
    auto xtip = summaryPlot_->modelHHeaderTip(column1, ok);
    auto ytip = summaryPlot_->modelHHeaderTip(column2, ok);

    tableTip.addTableRow("X Column", xtip);
    tableTip.addTableRow("Y Column", ytip);

    //---

    // data details
    auto *details1 = summaryPlot_->columnDetails(column1);
    auto *details2 = summaryPlot_->columnDetails(column2);

    if (details1 && details2 && details1->isNumeric() && details2->isNumeric()) {
      if (! summaryPlot_->groupColumn().isValid()) {
        const auto &model = summaryPlot_->currentModel();

        auto correlation = summaryPlot_->modelDetails(model)->correlation(column1, column2);

        tableTip.addTableRow("Correlation", correlation);
      }
      else {
        auto *groupDetails = summaryPlot_->columnDetails(summaryPlot_->groupColumn());

        const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

        for (const auto &pg : groupValues_.groupIndData) {
          int         ig      = pg.first;
          const auto &indData = pg.second;

          if (indData.x.size() == indData.y.size()) {
            auto correlation = CMathCorrelation::calc(indData.x, indData.y);

            auto groupVar = (groupDetails ? groupDetails->uniqueValue(ig) : QVariant());

            tableTip.addTableRow(QString("Correlation (%1)").arg(groupVar.toString()), correlation);
          }
        }
      }
    }
  }
  // diagonal
  else {
    // column info
    auto column = rowColumn();

    bool ok;
    auto tip = summaryPlot_->modelHHeaderTip(column, ok);

    tableTip.addTableRow("Column", tip);

    //---

    // data details
    auto *details = summaryPlot_->columnDetails(column);

    if (details) {
      tableTip.addTableRow("Num Values", details->valueCount());
      tableTip.addTableRow("Num Unique", details->numUnique());
      tableTip.addTableRow("Max Unique", details->maxUnique());
      tableTip.addTableRow("Num Null"  , details->numNull());

      if (details->isNumeric()) {
        tableTip.addTableRow("Min"   , details->minValue());
        tableTip.addTableRow("Max"   , details->maxValue());
        tableTip.addTableRow("Mean"  , details->meanValue());
        tableTip.addTableRow("StdDev", details->stdDevValue());
      }
    }

    //---

    // plot data
    if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
      tableTip.addTableRow("Count Max", maxCount_);
  }

  return tableTip.str();
}

void
CQChartsSummaryCellObj::
draw(PaintDevice *device) const
{
  if (summaryPlot_->selectMode() == CQChartsSummaryPlot::SelectMode::DATA) {
    if (summaryPlot_->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER)
      return;
  }

  //---

  CQPerfTrace trace("CQChartsSummaryCellObj::draw");

  //---

  xAxisBBox_ = BBox();
  yAxisBBox_ = BBox();

  //---

  summaryPlot_->setDrawCell(row_, col_);

  initCoords();

  //---

  auto bbox = this->rect();

  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if (summaryPlot_->isInnerBorder())
    device->drawRect(innerRect());
  else
    device->drawRect(bbox);

  //---

  nc_ = summaryPlot_->visibleColumns().count();

  //---

  drawXGrid(device);
  drawYGrid(device);

  //---

  bool drawXAxis = false;

  auto *xaxis = summaryPlot_->xAxis();

  // only draw x axis on first (above) or last (below) row
  if (xaxis->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
    drawXAxis = (row_ == nc_ - 1);
  else
    drawXAxis = (row_ == 0);

  if (summaryPlot_->orientation() == Qt::Horizontal && col_ == 0)
    drawXAxis = false;

  // hide x axis on just distribution cell
  if (summaryPlot_->isHideFirstAxis()) {
    if (drawXAxis) {
      drawXAxis = false;

      for (int row = 0; row < nc_; ++row) {
        if (getCellType(row, col_) == CQChartsSummaryPlot::CellType::SCATTER) {
          drawXAxis = true;
          break;
        }
      }
    }
  }

  if (drawXAxis)
    this->drawXAxis(device);

  //---

  bool drawYAxis = false;

  auto *yaxis = summaryPlot_->yAxis();

  // only draw y axis on first (left) or last (right) column
  if (yaxis->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
    drawYAxis = (col_ == nc_ - 1);
  else
    drawYAxis = (col_ == 0);

  if (summaryPlot_->orientation() == Qt::Vertical && row_ == nc_ - 1)
    drawYAxis = false;

  // hide y axis on just distribution cell
  if (summaryPlot_->isHideFirstAxis()) {
    if (drawYAxis) {
      drawYAxis = false;

      for (int col = 0; col < nc_; ++col) {
        if (getCellType(row_, col) == CQChartsSummaryPlot::CellType::SCATTER) {
          drawYAxis = true;
          break;
        }
      }
    }
  }

  if (drawYAxis)
    this->drawYAxis(device);

  //---

  auto cellType = getCellType();

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER)
    drawScatter(device);
  else if (cellType == CQChartsSummaryPlot::CellType::CORRELATION)
    drawCorrelation(device);
  else if (cellType == CQChartsSummaryPlot::CellType::BOXPLOT)
    drawBoxPlot(device);
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
    drawDistribution(device);
  else if (cellType == CQChartsSummaryPlot::CellType::PIE)
    drawPie(device);

  //---

  summaryPlot_->setDrawCell(-1, -1);
}

void
CQChartsSummaryCellObj::
initCoords() const
{
  bx1_ = summaryPlot_->lengthPlotWidth (summaryPlot_->border().left  ());
  by1_ = summaryPlot_->lengthPlotHeight(summaryPlot_->border().bottom());
  bx2_ = summaryPlot_->lengthPlotWidth (summaryPlot_->border().right ());
  by2_ = summaryPlot_->lengthPlotHeight(summaryPlot_->border().top   ());

  auto bbox = this->rect();

  pxmin_ = bbox.getXMin() + bx1_;
  pymin_ = bbox.getYMin() + by1_;
  pxmax_ = bbox.getXMax() - bx2_;
  pymax_ = bbox.getYMax() - by2_;
}

CQChartsGeom::BBox
CQChartsSummaryCellObj::
innerRect() const
{
  return BBox(pxmin_, pymin_, pxmax_, pymax_);
}

void
CQChartsSummaryCellObj::
updateRangeBox() const
{
  double xmin, xmax, ymin, ymax;
  getDataRange(xmin, ymin, xmax, ymax);

  //---

  // calc cell range box (if set)
  rangeBox_     = BBox();
  rangeDefined_ = false;

  if (row_ != col_) {
    auto column1 = colColumn();
    auto column2 = rowColumn();

    auto range1 = summaryPlot_->columnRange(column1);
    auto range2 = summaryPlot_->columnRange(column2);

    if (range1.isSet() || range2.isSet()) {
      rangeBox_ = BBox(xmin, ymin, xmax, ymax);

      if (range1.isSet()) {
        rangeBox_.setXMin(range1.min());
        rangeBox_.setXMax(range1.max());
      }

      if (range2.isSet()) {
        rangeBox_.setYMin(range2.min());
        rangeBox_.setYMax(range2.max());
      }

      rangeDefined_ = (range1.isSet() && range2.isSet());
    }
  }
  else {
    bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

    auto column1 = rowColumn();

    auto range1 = summaryPlot_->columnRange(column1);

    if (range1.isSet()) {
      rangeBox_ = BBox(xmin, ymin, xmax, ymax);

      if (! invert) {
        rangeBox_.setXMin(range1.min());
        rangeBox_.setXMax(range1.max());
      }
      else {
        rangeBox_.setYMin(range1.min());
        rangeBox_.setYMax(range1.max());
      }

      rangeDefined_ = true;
    }
  }
}

//---

void
CQChartsSummaryCellObj::
drawXAxis(PaintDevice *device) const
{
  auto *xaxis = summaryPlot_->xAxis();
  if (! xaxis->isVisible()) return;

  //---

  initXAxis(/*buckets*/false);

  auto clip = summaryPlot_->isDataClip();
  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(false, /*notify*/false);

  device->save();
  summaryPlot_->setClipRect(device);

  xaxis->draw(summaryPlot_, device);

  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(clip, /*notify*/false);

  xAxisBBox_ += xaxis->bbox();

  device->restore();
}

void
CQChartsSummaryCellObj::
drawXGrid(PaintDevice *device) const
{
  auto *xaxis = summaryPlot_->xAxis();
  if (! xaxis->isDrawGrid()) return;

  //---

  bool showGrid = false;
  bool buckets  = false;

  auto cellType = getCellType();

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER)
    showGrid = true;
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    buckets  = (summaryPlot_->orientation() == Qt::Horizontal);
    showGrid = true;
  }

  if (! showGrid)
    return;

  //---

  initXAxis(buckets);

  auto clip = summaryPlot_->isDataClip();
  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(false, /*notify*/false);

  device->save();
  device->setClipRect(summaryPlot_->isInnerBorder() ? innerRect() : rect());

  xaxis->drawGrid(summaryPlot_, device);

  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(clip, /*notify*/false);

  device->restore();
}

void
CQChartsSummaryCellObj::
initXAxis(bool buckets) const
{
  auto column = colColumn();

  double xmin, xmax;
  summaryPlot_->calcColumnRange(column, xmin, xmax);

  auto *details = summaryPlot_->columnDetails(column);

  auto type = CQBaseModelType::INTEGER;

  if (buckets) {
    CQChartsSummaryPlot::BucketCount bucketCount;
    int                              maxCount = 0;
    double                           bmin = 0.0, bmax = 0.0;

    summaryPlot_->calcBucketCounts(column, bucketCount, maxCount, bmin, bmax);

    xmin = 0;
    xmax = maxCount;
  }
  else {
    if (details)
      type = details->type();
  }

  //---

  auto *xaxis = summaryPlot_->xAxis();

  xaxis->setUpdatesEnabled(false);

  if (xaxis->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
    xaxis->setPosition(CQChartsAxis::OptReal(row_ + 1));
  else
    xaxis->setPosition(CQChartsAxis::OptReal(row_));

  xaxis->setRange(pxmin_, pxmax_);
  xaxis->setColumn(column);

  if (type == CQBaseModelType::REAL)
    xaxis->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::REAL), false);
  else
    xaxis->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER), false);

  xaxis->setValueRange(xmin, xmax);

  xaxis->setUpdatesEnabled(true);
}

void
CQChartsSummaryCellObj::
drawYAxis(PaintDevice *device) const
{
  auto *yaxis = summaryPlot_->yAxis();
  if (! yaxis->isVisible()) return;

  //---

  initYAxis(/*buckets*/false);

  auto clip = summaryPlot_->isDataClip();
  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(false, /*notify*/false);

  device->save();
  summaryPlot_->setClipRect(device);

  yaxis->draw(summaryPlot_, device);

  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(clip, /*notify*/false);

  yAxisBBox_ += yaxis->bbox();

  device->restore();
}

void
CQChartsSummaryCellObj::
drawYGrid(PaintDevice *device) const
{
  auto *yaxis = summaryPlot_->yAxis();
  if (! yaxis->isDrawGrid()) return;

  //---

  bool showGrid = false;
  bool buckets  = false;

  auto cellType = getCellType();

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER)
    showGrid = true;
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    buckets  = (summaryPlot_->orientation() == Qt::Vertical);
    showGrid = true;
  }

  if (! showGrid)
    return;

  //---

  initYAxis(buckets);

  auto clip = summaryPlot_->isDataClip();
  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(false, /*notify*/false);

  device->save();
  device->setClipRect(summaryPlot_->isInnerBorder() ? innerRect() : rect());

  yaxis->drawGrid(summaryPlot_, device);

  const_cast<CQChartsSummaryPlot *>(summaryPlot_)->setDataClip(clip, /*notify*/false);

  device->restore();
}

void
CQChartsSummaryCellObj::
initYAxis(bool buckets) const
{
  auto column = rowColumn();

  double ymin, ymax;
  summaryPlot_->calcColumnRange(column, ymin, ymax);

  auto *details = summaryPlot_->columnDetails(column);

  auto type = CQBaseModelType::INTEGER;

  if (buckets) {
    CQChartsSummaryPlot::BucketCount bucketCount;
    int                              maxCount = 0;
    double                           bmin = 0.0, bmax = 0.0;

    summaryPlot_->calcBucketCounts(column, bucketCount, maxCount, bmin, bmax);

    ymin = 0;
    ymax = maxCount;
  }
  else {
    if (details)
      type = details->type();
  }

  //---

  auto *yaxis = summaryPlot_->yAxis();

  yaxis->setUpdatesEnabled(false);

  if (yaxis->side().type() == CQChartsAxisSide::Type::TOP_RIGHT)
    yaxis->setPosition(CQChartsAxis::OptReal(col_ + 1));
  else
    yaxis->setPosition(CQChartsAxis::OptReal(col_));

  yaxis->setRange(pymin_, pymax_);
  yaxis->setColumn(column);

  if (type == CQBaseModelType::REAL)
    yaxis->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::REAL), false);
  else
    yaxis->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER), false);

  yaxis->setValueRange(ymin, ymax);

  yaxis->setUpdatesEnabled(true);
}

//---

CQChartsSummaryPlot::CellType
CQChartsSummaryCellObj::
getCellType() const
{
  return getCellType(row_, col_);
}

CQChartsSummaryPlot::CellType
CQChartsSummaryCellObj::
getCellType(int row, int col) const
{
  if (row != col) {
    bool lower = (row > col);

    auto isOffDiagonalType = [&](CQChartsSummaryPlot::OffDiagonalType type) {
      return ((  lower && summaryPlot_->lowerDiagonalType() == type) ||
              (! lower && summaryPlot_->upperDiagonalType() == type));
    };

    bool isScatter     = isOffDiagonalType(CQChartsSummaryPlot::OffDiagonalType::SCATTER);
    bool isCorrelation = isOffDiagonalType(CQChartsSummaryPlot::OffDiagonalType::CORRELATION);

    if      (isScatter)
      return CQChartsSummaryPlot::CellType::SCATTER;
    else if (isCorrelation)
      return CQChartsSummaryPlot::CellType::CORRELATION;
  }
  else {
    if      (summaryPlot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::BOXPLOT)
      return CQChartsSummaryPlot::CellType::BOXPLOT;
    else if (summaryPlot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::DISTRIBUTION)
      return CQChartsSummaryPlot::CellType::DISTRIBUTION;
    else if (summaryPlot_->diagonalType() == CQChartsSummaryPlot::DiagonalType::PIE)
      return CQChartsSummaryPlot::CellType::PIE;
  }

  return CQChartsSummaryPlot::CellType::NONE;
}

void
CQChartsSummaryCellObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  ColorInd colorInd;

  auto bc = summaryPlot_->interpPlotCellStrokeColor(colorInd);
  auto fc = summaryPlot_->interpPlotCellFillColor(colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->plotCellPenData(bc), summaryPlot_->plotCellBrushData(fc));

  if (updateState)
    summaryPlot_->updateObjPenBrushState(this, penBrush, drawType());
}

void
CQChartsSummaryCellObj::
drawScatter(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawScatter");

  //---

  updateRangeBox();

  drawParetoDir(device);

 //---

  pointDatas_.clear();

  poly_ = Polygon();

  //---

  // draw outside points first if dim outside
  if (summaryPlot_->regionPointType() == CQChartsSummaryPlot::RegionPointType::DIM_OUTSIDE) {
    bool anyRange = summaryPlot_->anyColumnRange();

    if (anyRange)
      drawScatterPoints(device, /*outside*/true);
  }

  drawScatterPoints(device, /*outside*/false);

  //---

  if (summaryPlot_->isBestFit())
    drawBestFit(device);

  if (summaryPlot_->isPareto())
    drawPareto(device, paretoInds_);

  //---

  drawRangeBox(device);
}

void
CQChartsSummaryCellObj::
drawScatterPoints(PaintDevice *device, bool outside) const
{
  auto colColumn = this->colColumn();
  auto rowColumn = this->rowColumn();

  auto *details1 = summaryPlot_->columnDetails(colColumn);
  auto *details2 = summaryPlot_->columnDetails(rowColumn);
  if (! details1 || ! details2) return;

  int nx = details1->numRows();
  int ny = details2->numRows();
  if (nx != ny) return;

  if (pointDatas_.empty())
    pointDatas_.resize(nx);

  //---

  auto *groupDetails = (summaryPlot_->groupColumn().isValid() ?
    summaryPlot_->columnDetails(summaryPlot_->groupColumn()) : nullptr);

  int ng = (groupDetails ? groupDetails->numUnique() : 0);

  auto *colorDetails      = (summaryPlot_->colorColumn().isValid() ?
    summaryPlot_->columnDetails(summaryPlot_->colorColumn()) : nullptr);
  auto *symbolTypeDetails = (summaryPlot_->symbolTypeColumn().isValid() ?
    summaryPlot_->columnDetails(summaryPlot_->symbolTypeColumn()) : nullptr);
  auto *symbolSizeDetails = (summaryPlot_->symbolSizeColumn().isValid() ?
    summaryPlot_->columnDetails(summaryPlot_->symbolSizeColumn()) : nullptr);

  //---

  summaryPlot_->calcColumnRange(colColumn, xmin_, xmax_);
  summaryPlot_->calcColumnRange(rowColumn, ymin_, ymax_);

  //---

  auto symbol     = summaryPlot_->scatterSymbol();
  auto symbolSize = summaryPlot_->calcScatterSymbolSize();

  //---

  bool anyRange = summaryPlot_->anyColumnRange();

  //---

  ColorInd colorInd1(row_, nc_);
  ColorInd colorInd2(col_, nc_);

  PenBrush penBrush1, penBrush2;

  summaryPlot_->setScatterSymbolPenBrush(penBrush1, colorInd1);
  summaryPlot_->setScatterSymbolPenBrush(penBrush2, colorInd2);

  for (uint i = 0; i < uint(nx); ++i) {
    // skip if outside does not match requirement
    bool rangeSelected = summaryPlot_->isRangeSelectedRow(i);

    if (summaryPlot_->regionPointType() == CQChartsSummaryPlot::RegionPointType::DIM_OUTSIDE) {
      bool outside1 = (anyRange && ! rangeSelected);
      if (outside != outside1) continue;
    }

    //---

    // get scatter point
    double x = 0.0, y = 0.0;

    if (details1->isNumeric()) {
      bool ok;
      x = CQChartsVariant::toReal(details1->value(i), ok);
      if (! ok) continue;
    }
    else
      x = details1->uniqueId(details1->value(i));

    if (details2->isNumeric()) {
      bool ok;
      y = CQChartsVariant::toReal(details2->value(i), ok);
      if (! ok) continue;
    }
    else
      y = details2->uniqueId(details2->value(i));

    auto p = Point(x, y);

    //---

    bool modelSelected = summaryPlot_->isModelSelectedRow(i);

    //---

    // set pen/brush
    PenBrush       groupPenBrush1, groupPenBrush2;
    ColorInd       groupColorInd1, groupColorInd2;
    CQChartsSymbol groupSymbol;

    if (groupDetails) {
      auto var = groupDetails->value(i);

      int ig = groupDetails->uniqueId(var);

      groupColorInd1 = ColorInd(ig, ng);

      summaryPlot_->setScatterSymbolPenBrush(groupPenBrush1, groupColorInd1);

      groupColorInd2 = groupColorInd1;
      groupPenBrush2 = groupPenBrush1;

      // get index symbol
      auto *symbolSet = summaryPlot_->defaultSymbolSet();

      groupSymbol = symbolSet->interpI(ig).symbol;
    }
    else {
      groupColorInd1 = colorInd1;
      groupColorInd2 = colorInd2;
      groupPenBrush1 = penBrush1;
      groupPenBrush2 = penBrush2;
      groupSymbol    = symbol;
    }

    auto symbolSize1 = symbolSize;

    //---

    if (colorDetails) {
      // TODO: support mapping ?
      bool ok;
      auto color = CQChartsVariant::toColor(colorDetails->value(i), ok);

      if (ok) {
        auto c1 = summaryPlot_->interpColor(color, groupColorInd1);
        auto c2 = summaryPlot_->interpColor(color, groupColorInd2);

        groupPenBrush1.brush.setColor(c1);
        groupPenBrush2.brush.setColor(c2);
      }
    }

    if (symbolTypeDetails) {
      // TODO: support mapping ?
      bool ok;
      auto symbol = CQChartsVariant::toSymbol(symbolTypeDetails->value(i), ok);

      if (ok)
        groupSymbol = symbol;
    }

    if (symbolSizeDetails) {
      // TODO: support mapping ?
      bool ok;
      auto size = CQChartsVariant::toLength(symbolSizeDetails->value(i), ok);

      if (ok)
        symbolSize1 = size;
    }

    //---

    auto blendAlpha = CMathUtil::avg(groupPenBrush1.brush.color().alphaF(),
                                     groupPenBrush2.brush.color().alphaF());
    auto blendColor = CQChartsUtil::blendColors(groupPenBrush1.brush.color(),
                                                groupPenBrush2.brush.color(), 0.5);
    blendColor.setAlphaF(blendAlpha);

    groupPenBrush1.brush.setColor(blendColor);

    CQChartsDrawUtil::setPenBrush(device, groupPenBrush1);

    //---

    if (summaryPlot_->regionPointType() == CQChartsSummaryPlot::RegionPointType::DIM_OUTSIDE) {
      if (anyRange && ! rangeSelected) {
        CQChartsDrawUtil::setBrushGray(groupPenBrush1.brush, summaryPlot_->outsideGray());
        CQChartsDrawUtil::setPenAlpha (groupPenBrush1.pen  , summaryPlot_->outsideAlpha());
      }
    }

    //---

    auto ps = plotToParent(p);

    double psx, psy;
    summaryPlot_->pixelSymbolSize(symbolSize1, psx, psy, /*scale*/true);
//  auto psx = summaryPlot_->lengthPixelWidth (symbolSize1);
//  auto psy = summaryPlot_->lengthPixelHeight(symbolSize1);

    auto ss = Length::pixel(std::min(psx, psy));

    CQChartsDrawUtil::drawSymbol(device, groupPenBrush1, groupSymbol,
                                 ps, ss, ss, /*scale*/false);

    //---

    PointData pointData;

    pointData.ind        = i;
    pointData.p          = p;
    pointData.color      = Color(blendColor);
    pointData.symbol     = groupSymbol;
    pointData.symbolSize = symbolSize1;

    pointData.rangeSelected = rangeSelected;
    pointData.modelSelected = modelSelected;

    //---

    pointDatas_[i] = pointData;

    //---

    poly_.addPoint(Point(x, y)); // untransformed point
  }
}

void
CQChartsSummaryCellObj::
drawBestFit(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawBestFit");

  //---

  auto bbox = this->rect();

  auto drawFitPoly = [&](const Polygon &polygon) {
    auto pbbox = polygon.boundingBox();
    if (! pbbox.isValid()) return;

    CQChartsFitData fitData;

    fitData.calc(polygon, 3);

    double dx = pbbox.getWidth()/100.0;

    Polygon fitPoly;

    for (int i = 0; i <= 100; ++i) {
      double x = pbbox.getXMin() + i*dx;
      double y = fitData.interp(x);

      auto p = plotToParent(Point(x, y));

      fitPoly.addPoint(p);
    }

    //---

    auto path = CQChartsDrawUtil::polygonToPath(fitPoly, /*closed*/false);

    device->save();
    device->setClipRect(bbox);

    device->strokePath(path, device->pen());

    device->restore();
  };

  if (! summaryPlot_->groupColumn().isValid()) {
    drawFitPoly(poly_);
  }
  else {
    const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

    for (const auto &pg : groupValues_.groupIndData) {
      const auto &indData = pg.second;

      drawFitPoly(indData.poly);
    }
  }
}

void
CQChartsSummaryCellObj::
drawCorrelation(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawCorrelation");

  //---

  auto column1 = colColumn();
  auto column2 = rowColumn();

  auto *details1 = summaryPlot_->columnDetails(column1);
  auto *details2 = summaryPlot_->columnDetails(column2);
  if (! details1 || ! details2) return;

  //---

  summaryPlot_->setPainterFont(device, summaryPlot_->correlationTextFont());

  //---

  PenBrush tpenBrush;

  summaryPlot_->setCorrelationTextPenBrush(tpenBrush, ColorInd());

  CQChartsDrawUtil::setPenBrush(device, tpenBrush);

  //---

  auto textOptions = summaryPlot_->correlationTextOptions(device);

  textOptions.angle     = CQChartsAngle();
  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  //---

  if (details1->isNumeric() && details2->isNumeric()) {
    auto *drawObj = new CQChartsDrawObj;

    auto m = summaryPlot_->correlationMargin();

    double bx = summaryPlot_->pixelToWindowWidth (m);
    double by = summaryPlot_->pixelToWindowHeight(m);

    double x = 0.0;
    double y = 0.0;

    if (! summaryPlot_->groupColumn().isValid()) {
      const auto &model = summaryPlot_->currentModel();

      auto correlation = summaryPlot_->modelDetails(model)->correlation(column1, column2);

      auto cstr = CQChartsUtil::realToString(correlation, 5);

      auto pt = Point(x, y);

      auto *drawText = new CQChartsDrawText(pt, cstr, device->font(), tpenBrush,
                                            textOptions, /*scale*/false);

      drawObj->addChild(drawText);
    }
    else {
      const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

      int ng = int(groupValues_.groupIndData.size());

      QFontMetricsF fm(device->font());

      double dx = summaryPlot_->pixelToWindowWidth (fm.height());
      double dy = summaryPlot_->pixelToWindowHeight(fm.height());

      y += double(groupValues_.groupIndData.size() - 1)*(dy + by);

      for (const auto &pg : groupValues_.groupIndData) {
        int         ig      = pg.first;
        const auto &indData = pg.second;

        PenBrush rpenBrush;

        auto fc = summaryPlot_->interpPaletteColor(ColorInd(ig, ng));

        summaryPlot_->setPenBrush(rpenBrush, PenData(false), BrushData(true, fc));

        auto *drawRect = new CQChartsDrawRect(BBox(x, y, x + dx, y + dy), rpenBrush);

        drawObj->addChild(drawRect);

        //---

        if (indData.x.size() == indData.y.size()) {
          auto correlation = CMathCorrelation::calc(indData.x, indData.y);

          auto cstr = CQChartsUtil::realToString(correlation, 5);

          auto pt = Point(x + dx + 2*bx, y + dy/2.0);

          auto *drawText = new CQChartsDrawText(pt, cstr, device->font(), tpenBrush,
                                                textOptions, /*scale*/false);

          drawObj->addChild(drawText);
        }

        //---

        y -= dy + by;
      }
    }

    //---

    BBox pbbox(pxmin_ + bx, pymin_ + by, pxmax_ - bx, pymax_ - by);

    drawObj->setMaxScale(1.5);

    drawObj->place(device, pbbox);

    drawObj->draw(device);

    delete drawObj;
  }
}

void
CQChartsSummaryCellObj::
drawBoxPlot(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawBoxPlot");

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  auto bbox = innerRect();

  //---

  PenBrush penBrush;

  ColorInd colorInd(row_, nc_);

  auto bc = summaryPlot_->interpBoxPlotStrokeColor(colorInd);
  auto fc = summaryPlot_->interpBoxPlotFillColor  (colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->boxPlotPenData(bc), summaryPlot_->boxPlotBrushData(fc));

  //---

  if (details->isNumeric()) {
    bool ok;
    double min  = CQChartsVariant::toReal(details->minValue (), ok);
    double max  = CQChartsVariant::toReal(details->maxValue (), ok);
    double mean = CQChartsVariant::toReal(details->meanValue(), ok);

    int n = details->numRows();

    CQChartsBoxWhisker whisker;

    for (uint i = 0; i < uint(n); ++i) {
      bool ok;
      double r = CQChartsVariant::toReal(details->value(i), ok);
      if (! ok) continue;

      double r1;

      if (summaryPlot_->orientation() == Qt::Vertical)
        r1 = CMathUtil::map(r, min, max, pymin_, pymax_);
      else
        r1 = CMathUtil::map(r, min, max, pxmin_, pxmax_);

      whisker.addValue(r1);
    }

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsBoxWhiskerUtil::DrawData  drawData;
    CQChartsBoxWhiskerUtil::PointData pointData;

    drawData.width       = Length::plot(0.1);
    drawData.orientation = summaryPlot_->orientation();
    drawData.median      = true;

    CQChartsBoxWhiskerUtil::drawWhisker(device, whisker, bbox, drawData, pointData);

    //---

    if (summaryPlot_->isBoxPlotTextVisible()) {
      summaryPlot_->setPainterFont(device, summaryPlot_->boxPlotTextFont());

      //---

      PenBrush tpenBrush;

      summaryPlot_->setBoxPlotTextPenBrush(tpenBrush, ColorInd());

      CQChartsDrawUtil::setPenBrush(device, tpenBrush);

      //---

      auto textOptions = summaryPlot_->boxPlotTextOptions(device);

      if (drawData.orientation == Qt::Vertical)
        textOptions.angle = CQChartsAngle();
      else
        textOptions.angle = CQChartsAngle::degrees(90);

      textOptions.formatted = false;
      textOptions.scaled    = false;
      textOptions.html      = false;

      bool centered = (drawData.orientation != Qt::Vertical);

      if (drawData.orientation == Qt::Vertical)
        textOptions.align = Qt::AlignRight | Qt::AlignVCenter;
      else
        textOptions.align = Qt::AlignHCenter | Qt::AlignTop;

      auto minStr  = CQChartsUtil::scaledNumberString(min);
      auto maxStr  = CQChartsUtil::scaledNumberString(max);
      auto meanStr = CQChartsUtil::scaledNumberString(mean);

      CQChartsDrawUtil::drawTextAtPoint(device, pointData.min, minStr, textOptions, centered);
      CQChartsDrawUtil::drawTextAtPoint(device, pointData.max, maxStr, textOptions, centered);

      if (drawData.orientation == Qt::Vertical)
        textOptions.align = Qt::AlignLeft | Qt::AlignVCenter;
      else
        textOptions.align = Qt::AlignHCenter | Qt::AlignBottom;

      CQChartsDrawUtil::drawTextAtPoint(device, pointData.med, meanStr, textOptions, centered);
    }

    //---

    bmin_ = min;
    bmax_ = max;
  }
  else {
    CQChartsTextOptions options;

    CQChartsDrawUtil::drawTextInBox(device, bbox, "N/A", options);

    bmin_ = 0.0;
    bmax_ = 1.0;

    return;
  }

  //---

  drawRangeBox(device);
}

void
CQChartsSummaryCellObj::
drawDistribution(PaintDevice *device) const
{
  using MinMax = CQChartsSummaryPlot::MinMax;

  CQPerfTrace trace("CQChartsSummaryCellObj::drawDistribution");

  rectDatas_.clear();

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  bool isGroup = (summaryPlot_->groupColumn() == column);

  //---

  PenBrush penBrush;

  ColorInd colorInd(row_, nc_);

  auto bc = summaryPlot_->interpDistributionStrokeColor(colorInd);
  auto fc = summaryPlot_->interpDistributionFillColor  (colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->distributionPenData(bc), summaryPlot_->distributionBrushData(fc));

  //---

  bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

  //---

  auto drawRect = [&](const BBox &bbox) {
    auto pw = (invert ?
      device->windowToPixelHeight(bbox.getHeight()) :
      device->windowToPixelWidth (bbox.getWidth ()));

    if (pw <= 2) {
      auto mid = (invert ? bbox.getYMid() : bbox.getXMid());

      device->setPen(device->brush().color());

      if (invert)
        device->drawLine(Point(bbox.getXMin(), mid), Point(bbox.getXMax(), mid));
      else
        device->drawLine(Point(mid, bbox.getYMin()), Point(mid, bbox.getYMax()));
    }
    else {
      device->drawRect(bbox);
    }
  };

  //---

  updateRangeBox();

  bool anyRange = summaryPlot_->anyColumnRange();

  //---

  double xgap = 0.0, ygap = 0.0;

  if (summaryPlot_->orientation() == Qt::Vertical)
    xgap = summaryPlot_->lengthPlotWidth(summaryPlot_->distributionMargin());
  else
    ygap = summaryPlot_->lengthPlotHeight(summaryPlot_->distributionMargin());

  //---

  if (details->isNumeric()) {
    CQChartsSummaryPlot::BucketCount bucketCount;

    summaryPlot_->calcBucketCounts(column, bucketCount, maxCount_, bmin_, bmax_);

    int ig = 0;
    int ng = int(bucketCount.size());

    for (const auto &pb : bucketCount) {
      PenBrush penBrush1;

      if (isGroup) {
        ColorInd colorInd(ig, ng);

        auto fc1 = summaryPlot_->interpDistributionFillColor(colorInd);

        summaryPlot_->setPenBrush(penBrush1,
          summaryPlot_->distributionPenData(bc), summaryPlot_->distributionBrushData(fc1));
      }
      else {
        penBrush1 = penBrush;
      }

      //---

      QVariant vmin, vmax;

      details->bucketRange(pb.first, vmin, vmax);

      bool ok;
      double rmin1 = CQChartsVariant::toReal(vmin, ok);
      double rmax1 = CQChartsVariant::toReal(vmax, ok);

      auto n = pb.second.size();

      double x1, y1, x2, y2;

      if (invert) {
        x1 = CMathUtil::map(0, 0, maxCount_, pxmin_, pxmax_);
        x2 = CMathUtil::map(n, 0, maxCount_, pxmin_, pxmax_);

        y1 = CMathUtil::map(rmin1, bmin_, bmax_, pymin_, pymax_);
        y2 = CMathUtil::map(rmax1, bmin_, bmax_, pymin_, pymax_);
      }
      else {
        x1 = CMathUtil::map(rmin1, bmin_, bmax_, pxmin_, pxmax_);
        x2 = CMathUtil::map(rmax1, bmin_, bmax_, pxmin_, pxmax_);

        y1 = CMathUtil::map(0, 0, maxCount_, pymin_, pymax_);
        y2 = CMathUtil::map(n, 0, maxCount_, pymin_, pymax_);
      }

      auto bbox = BBox(x1, y1, x2, y2);

      auto xgap1 = std::min(xgap, bbox.getWidth ());
      auto ygap1 = std::min(ygap, bbox.getHeight());

      bbox.expand(xgap1/2.0, ygap1/2.0, -xgap1/2.0, -ygap1/2.0);

      //---

      bool rangeSelected = false;

      if (rangeBox_.isSet()) {
        MinMax r1(rangeBox_.getXMin(), rangeBox_.getXMax());
        MinMax r2(rmin1, rmax1);

        if (r1.overlapsHalfOpen(r2))
          rangeSelected = true;
      }

      size_t numModelSelected = 0;

      for (const auto &row : pb.second) {
        if (summaryPlot_->isModelSelectedRow(row))
          ++numModelSelected;
      }

      bool modelSelected = (numModelSelected == pb.second.size());

      //---

      PenBrush penBrush2 = penBrush1;

      if (summaryPlot_->regionPointType() == CQChartsSummaryPlot::RegionPointType::DIM_OUTSIDE) {
        bool bucketSelected = false;

        if (anyRange) {
          for (const auto &r : pb.second) { // bucket rows
            if (summaryPlot_->isRangeSelectedRow(r)) {
              bucketSelected = true;
              break;
            }
          }
        }
        else
          bucketSelected = true;

        if (! bucketSelected) {
          CQChartsDrawUtil::setBrushGray(penBrush2.brush, summaryPlot_->outsideGray());
          CQChartsDrawUtil::setPenAlpha (penBrush2.pen  , summaryPlot_->outsideAlpha());
        }
      }

      //---

      CQChartsDrawUtil::setPenBrush(device, penBrush2);

      drawRect(bbox);

      //---

      RectData rectData;

      rectData.ind   = ig;
      rectData.rows  = pb.second;
      rectData.bbox  = (invert ? BBox(0, rmin1, n, rmax1) : BBox(rmin1, 0, rmax1, n));
      rectData.pbbox = bbox;

      rectData.rangeSelected = rangeSelected;
      rectData.modelSelected = modelSelected;

      rectDatas_.push_back(rectData);

      //--

      ++ig;
    }

    //---

    if (summaryPlot_->isDensity())
      drawDensity(device);
  }
  else {
    CQChartsSummaryPlot::ValueCounts valueCounts;

    summaryPlot_->calcValueCounts(row_, valueCounts, maxCount_);

    int nc = int(valueCounts.size());

    bmin_ = 0;
    bmax_ = nc;

    // calc delta per bar
    double dn = (invert ?
      (nc > 0 ? (pymax_ - pymin_)/nc : 0.0) : (nc > 0 ? (pxmax_ - pxmin_)/nc : 0.0));

    for (const auto &vc : valueCounts) {
      auto name = vc.first;
      auto n    = vc.second;

      int ic = details->uniqueId(name);

      PenBrush penBrush1;

      if (isGroup) {
        ColorInd colorInd(ic, nc);

        auto fc1 = summaryPlot_->interpDistributionFillColor(colorInd);

        summaryPlot_->setPenBrush(penBrush1,
          summaryPlot_->distributionPenData(bc), summaryPlot_->distributionBrushData(fc1));
      }
      else {
        penBrush1 = penBrush;
      }

      //---

      auto h1 = (invert ? pxmin_ : pymin_);
      auto h2 = CMathUtil::map(n, 0, maxCount_, pymin_, pymax_);

      auto pos1 = (invert ? CMathUtil::map(ic, 0, nc, pymin_, pymax_) :
                            CMathUtil::map(ic, 0, nc, pxmin_, pxmax_));
      auto pos2 = pos1 + dn;

      auto bbox = (invert ? BBox(h1, pos1, h2, pos2) : BBox(pos1, h1, pos2, h2));

      auto xgap1 = std::min(xgap, bbox.getWidth ());
      auto ygap1 = std::min(ygap, bbox.getHeight());

      bbox.expand(xgap1/2.0, ygap1/2.0, -xgap1/2.0, -ygap1/2.0);

      //---

      bool rangeSelected = false;

      if (rangeBox_.isSet()) {
        MinMax r1(rangeBox_.getXMin(), rangeBox_.getXMax());
        MinMax r2(ic, ic + 1);

        if (r1.overlapsHalfOpen(r2))
          rangeSelected = true;
      }

      int numModelSelected = 0;

      std::set<int> rows;

      int nr = details->numRows();

      for (uint i = 0; i < uint(nr); ++i) {
        auto value = details->value(i);

        if (value == name) {
          rows.insert(i);

          if (summaryPlot_->isModelSelectedRow(i))
            ++numModelSelected;
        }
      }

      bool modelSelected = (numModelSelected == n);

      //---

      PenBrush penBrush2 = penBrush1;

      if (summaryPlot_->regionPointType() == CQChartsSummaryPlot::RegionPointType::DIM_OUTSIDE) {
        bool bucketSelected = false;

        if (anyRange) {
          for (const auto &r : rows) { // bucket rows
            if (summaryPlot_->isRangeSelectedRow(r)) {
              bucketSelected = true;
              break;
            }
          }
        }
        else
          bucketSelected = true;

        if (! bucketSelected) {
          CQChartsDrawUtil::setBrushGray(penBrush2.brush, summaryPlot_->outsideGray());
          CQChartsDrawUtil::setPenAlpha (penBrush2.pen  , summaryPlot_->outsideAlpha());
        }
      }

      //---

      CQChartsDrawUtil::setPenBrush(device, penBrush2);

      drawRect(bbox);

      //---

      RectData rectData;

      rectData.ind   = ic;
      rectData.name  = name;
      rectData.rows  = rows;
      rectData.bbox  = (invert ? BBox(0, ic, n, ic + 1) : BBox(ic, 0, ic + 1, n));
      rectData.pbbox = bbox;

      rectData.rangeSelected = rangeSelected;
      rectData.modelSelected = modelSelected;

      rectDatas_.push_back(rectData);
    }
  }

  //---

  drawParetoDir(device);

  //---

  drawRangeBox(device);
}

void
CQChartsSummaryCellObj::
drawDistributionRange(PaintDevice *device) const
{
  PenBrush tpenBrush;

  auto tc = summaryPlot_->xAxis()->interpAxesLabelTextColor(ColorInd());

  CQChartsUtil::setPen(tpenBrush.pen, true, tc);

  device->setPen(tpenBrush.pen);

  //---

  summaryPlot_->setPainterFont(device, summaryPlot_->xAxis()->axesTickLabelTextFont());

  auto textOptions = summaryPlot_->boxPlotTextOptions(device);

  textOptions.formatted = false;
  textOptions.scaled    = false;
  textOptions.html      = false;

  Point   p1, p2;
  QString str1, str2;

  if (summaryPlot_->orientation() == Qt::Vertical) {
    str1 = CQChartsUtil::scaledNumberString(rangeBox_.getXMin());
    str2 = CQChartsUtil::scaledNumberString(rangeBox_.getXMax());
  }
  else {
    str1 = CQChartsUtil::scaledNumberString(rangeBox_.getYMin());
    str2 = CQChartsUtil::scaledNumberString(rangeBox_.getYMax());
  }

  if (summaryPlot_->orientation() == Qt::Vertical) {
    auto p1 = plotToParent(rangeBox_.getLL());
    auto p2 = plotToParent(rangeBox_.getLR());

    bool centered = false;

    textOptions.align = Qt::AlignRight | Qt::AlignTop;
    CQChartsDrawUtil::drawTextAtPoint(device, p1, str1, textOptions, centered);

    textOptions.align = Qt::AlignLeft | Qt::AlignTop;
    CQChartsDrawUtil::drawTextAtPoint(device, p2, str2, textOptions, centered);
  }
  else {
    auto p1 = plotToParent(rangeBox_.getLR());
    auto p2 = plotToParent(rangeBox_.getUR());

    bool centered = true;

    textOptions.angle = CQChartsAngle::degrees(90);
    textOptions.align = Qt::AlignHCenter | Qt::AlignTop;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, str1, textOptions, centered);
    CQChartsDrawUtil::drawTextAtPoint(device, p2, str2, textOptions, centered);
  }
}

void
CQChartsSummaryCellObj::
drawRangeBox(PaintDevice *device, bool overlay) const
{
  updateRangeBox();

  if (! rangeBox_.isValid())
    return;

  auto rbbox = BBox(plotToParent(rangeBox_.getLL()), plotToParent(rangeBox_.getUR()));

  auto cellType = getCellType();

  //---

  auto drawRectSides = [&](const BBox &bbox) {
    if (summaryPlot_->orientation() == Qt::Vertical) {
      device->drawLine(bbox.getLL(), bbox.getUL());
      device->drawLine(bbox.getLR(), bbox.getUR());
    }
    else {
      device->drawLine(bbox.getLL(), bbox.getLR());
      device->drawLine(bbox.getUL(), bbox.getUR());
    }
  };

  //---

  if (! overlay) {
    PenBrush penBrush;

    ColorInd colorInd;

    auto bc = summaryPlot_->interpRegionStrokeColor(colorInd);
    auto fc = summaryPlot_->interpRegionFillColor  (colorInd);

    summaryPlot_->setPenBrush(penBrush,
      summaryPlot_->regionPenData(bc), summaryPlot_->regionBrushData(fc));

    if (! rangeDefined_) {
      CQChartsDrawUtil::setPenGray(penBrush.pen, summaryPlot_->undefRegionGray());

      auto w = summaryPlot_->lengthPixelWidth(summaryPlot_->undefRegionWidth());
      penBrush.pen.setWidthF(w);
    }

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
      drawRectSides(rbbox);
    else
      device->drawRect(rbbox);
  }

  //---

  if (overlay && rangeInside_) {
    PenBrush penBrush;

    auto sc = summaryPlot_->interpColor(summaryPlot_->regionEditStroke(), ColorInd());

    summaryPlot_->setPenBrush(penBrush,
      PenData(true, sc, Alpha(), Length::pixel(summaryPlot_->regionEditWidth())),
      BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    if (! rangeBoxSide_) {
      if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
        drawRectSides(rbbox);
      else
        device->drawRect(rbbox);
    }
    else {
      if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
        if (summaryPlot_->orientation() == Qt::Vertical)
          rangeBoxSide_ &= ~(Qt::AlignBottom | Qt::AlignTop);
        else
          rangeBoxSide_ &= ~(Qt::AlignLeft | Qt::AlignRight);
      }

      if      (rangeBoxSide_ & Qt::AlignLeft  )
        device->drawLine(rbbox.getLL(), rbbox.getUL());
      else if (rangeBoxSide_ & Qt::AlignRight )
        device->drawLine(rbbox.getLR(), rbbox.getUR());
      if      (rangeBoxSide_ & Qt::AlignBottom)
        device->drawLine(rbbox.getLL(), rbbox.getLR());
      else if (rangeBoxSide_ & Qt::AlignTop   )
        device->drawLine(rbbox.getUL(), rbbox.getUR());
    }
  }
}

void
CQChartsSummaryCellObj::
drawDensity(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawDensity");

  //---

  auto bbox = this->rect();

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  //---

  auto pc = summaryPlot_->interpInterfaceColor(1.0);

  CQChartsDensity density;

  density.setDrawType(CQChartsDensity::DrawType::DISTRIBUTION);

  CQChartsDensity::XVals xvals;

  int n = details->numRows();

  for (uint i = 0; i < uint(n); ++i) {
    auto value = details->value(i);

    bool ok;
    double r = CQChartsVariant::toReal(value, ok);
    if (! ok) continue;

    double x = CMathUtil::map(r, bmin_, bmax_, pxmin_, pxmax_);

    xvals.push_back(x);
  }

  density.setXVals(xvals);

  //---

  PenBrush penBrush;

  // TODO: config ?
  summaryPlot_->setPenBrush(penBrush, PenData(true, pc, Alpha(0.5)), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->save();
  device->setClipRect(bbox);

  CQChartsDensity::DrawData drawData;

  drawData.scaled = true;

  density.draw(summaryPlot_, device, bbox, drawData);

  device->restore();
}

void
CQChartsSummaryCellObj::
drawParetoDir(PaintDevice *device) const
{
  if (summaryPlot_->paretoOriginType() == CQChartsSummaryPlot::ParetoOriginType::NONE)
    return;

  CQPerfTrace trace("CQChartsSummaryCellObj::drawParetoDirection");

  //---

  auto bbox = this->rect();

  //---

  bool invX = false, invY = false;

  auto cellType = getCellType();

  bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    auto column1 = colColumn();
    auto column2 = rowColumn();

    auto *xDetails = summaryPlot_->columnDetails(column1);
    auto *yDetails = summaryPlot_->columnDetails(column2);

    invX = (xDetails ? xDetails->decreasing().toBool() : false);
    invY = (yDetails ? yDetails->decreasing().toBool() : false);
  }
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    auto column = colColumn();

    auto *details = summaryPlot_->columnDetails(column);

    if (! invert)
      invX = (details ? details->decreasing().toBool() : false);
    else
      invY = (details ? details->decreasing().toBool() : false);
  }

  //---

  // calc origin
  auto pbbox = BBox(parentToPlot(bbox.getLL()), parentToPlot(bbox.getUR()));

  auto origin = Point(invX ? pbbox.getXMax() : pbbox.getXMin(),
                      invY ? pbbox.getYMax() : pbbox.getYMin());

  //---

  auto originColor = summaryPlot_->interpColor(summaryPlot_->paretoOriginColor(), ColorInd());

  // draw origin symbol
  if     (summaryPlot_->paretoOriginType() == CQChartsSummaryPlot::ParetoOriginType::SYMBOL) {
    PenBrush penBrush;

    summaryPlot_->setPenBrush(penBrush,
      PenData(true, originColor, Alpha()), BrushData(true, originColor));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    auto symbolSize = summaryPlot_->paretoOriginSize();

    auto pxs = summaryPlot_->lengthPlotWidth (symbolSize);
    auto pys = summaryPlot_->lengthPlotHeight(symbolSize);

    auto o1 = plotToParent(origin);
    auto o2 = Point(o1.x + (invX ? -pxs : pxs), o1.y + (invY ? -pys : pys));

    double psx, psy;
    summaryPlot_->pixelSymbolSize(symbolSize, psx, psy, /*scale*/true);
//  auto psx = summaryPlot_->lengthPixelWidth (symbolSize);
//  auto psy = summaryPlot_->lengthPixelHeight(symbolSize);

    auto ss = Length::pixel(std::min(psx, psy));

    CQChartsDrawUtil::drawSymbol(device, penBrush, summaryPlot_->paretoOriginSymbol(), o2,
                                 ss, ss, /*scale*/false);
  }
  // draw origin gradient
  else if (summaryPlot_->paretoOriginType() == CQChartsSummaryPlot::ParetoOriginType::GRADIENT) {
    auto bgColor = QColor(0, 0, 0, 0);

    auto o1 = plotToParent(origin);

    CQChartsDrawUtil::drawParetoGradient(device, o1, bbox, originColor, bgColor);
  }
  else if (summaryPlot_->paretoOriginType() == CQChartsSummaryPlot::ParetoOriginType::CORNER) {
    PenBrush penBrush;

    summaryPlot_->setPenBrush(penBrush,
      PenData(true, originColor, Alpha(), summaryPlot_->paretoWidth()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    auto opposite = Point(invX ? pbbox.getXMin() : pbbox.getXMax(),
                          invY ? pbbox.getYMin() : pbbox.getYMax());

    double dx = (opposite.x - origin.x)/10;
    double dy = (opposite.y - origin.y)/10;

    auto o1 = plotToParent(origin);
    auto o2 = plotToParent(origin + Point(dx, 0.0));
    auto o3 = plotToParent(origin + Point(0.0, dy));

    if      (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
      device->drawLine(o1, o2);
      device->drawLine(o1, o3);
    }
    else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
      if (! invert)
        device->drawLine(o1, o3);
      else
        device->drawLine(o1, o2);
    }
  }
}

void
CQChartsSummaryCellObj::
drawPareto(PaintDevice *device, std::set<int> &inds) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawPareto");

  //---

  auto bbox = this->rect();

  //---

  auto column1 = colColumn();
  auto column2 = rowColumn();

  auto *xDetails = summaryPlot_->columnDetails(column1);
  auto *yDetails = summaryPlot_->columnDetails(column2);

  bool invX = (xDetails ? xDetails->decreasing().toBool() : false);
  bool invY = (yDetails ? yDetails->decreasing().toBool() : false);

  //---

  // calc origin
  auto pbbox = BBox(parentToPlot(bbox.getLL()), parentToPlot(bbox.getUR()));

  auto origin = Point(invX ? pbbox.getXMax() : pbbox.getXMin(),
                      invY ? pbbox.getYMax() : pbbox.getYMin());

  //---

  auto drawPareto = [&](const Polygon &polygon, const ColorInd &ic) {
    // calc pareto
    std::vector<Point> points;
    for (int i = 0; i < polygon.size(); ++i)
      points.push_back(polygon.point(i));

    auto front = CQChartsGeom::calcParetoFront(points, origin);

    Polygon frontPoly;

    for (const auto &p : front) {
      auto p1 = plotToParent(Point(p));

      frontPoly.addPoint(p1);
    }

    if (frontPoly.empty())
      return;

    //---

    std::map<Point, std::set<int>> spoints;

    for (int i = 0; i < polygon.size(); ++i)
      spoints[polygon.point(i)].insert(i);

    for (const auto &p : front) {
      auto pi = spoints.find(p);
      if (pi == spoints.end()) continue;

      for (auto i : (*pi).second)
        inds.insert(i);
    }

    //---

    // draw pareto front
    PenBrush penBrush;

    auto lineColor = summaryPlot_->interpColor(summaryPlot_->paretoLineColor(), ic);

    summaryPlot_->setPenBrush(penBrush,
      PenData(true, lineColor, Alpha(), summaryPlot_->paretoWidth()),
      BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    auto path = CQChartsDrawUtil::polygonToPath(frontPoly, /*closed*/false);

    device->save();
    device->setClipRect(bbox);

    device->strokePath(path, device->pen());

    device->restore();
  };

  //---

  if (! summaryPlot_->groupColumn().isValid()) {
    int nr = summaryPlot_->visibleColumns().count();

    drawPareto(poly_, ColorInd(row(), nr));
  }
  else {
    const_cast<CQChartsSummaryCellObj *>(this)->initGroupedValues();

    int ig = 0;
    int ng = int(groupValues_.groupIndData.size());

    for (const auto &pg : groupValues_.groupIndData) {
      const auto &indData = pg.second;

      drawPareto(indData.poly, ColorInd(ig++, ng));
    }
  }
}

void
CQChartsSummaryCellObj::
drawPie(PaintDevice *device) const
{
  CQPerfTrace trace("CQChartsSummaryCellObj::drawPie");

  //---

  auto column = rowColumn();

  auto *details = summaryPlot_->columnDetails(column);
  if (! details) return;

  //---

  PenBrush penBrush;

  ColorInd colorInd(row_, nc_);

  auto bc = summaryPlot_->interpPieStrokeColor(colorInd);
  auto fc = summaryPlot_->interpPieFillColor  (colorInd);

  summaryPlot_->setPenBrush(penBrush,
    summaryPlot_->piePenData(bc), summaryPlot_->pieBrushData(fc));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  if (details->isNumeric()) {
    CQChartsSummaryPlot::BucketCount bucketCount;
    int                              maxCount = 0;

    summaryPlot_->calcBucketCounts(column, bucketCount, maxCount, bmin_, bmax_);

    CQChartsRValues values;

    for (const auto &pb : bucketCount) {
      auto n = pb.second.size();

      values.addValue(CQChartsRValues::OptReal(n));
    }

    auto bbox1 = innerRect();

    CQChartsPlotDrawUtil::drawPie(const_cast<CQChartsSummaryPlot *>(summaryPlot_), device,
                                  values, bbox1, summaryPlot_->defaultPalette(), penBrush);
  }
  else {
    auto bbox = innerRect();

    CQChartsTextOptions options;

    CQChartsDrawUtil::drawTextInBox(device, bbox, "N/A", options);

    bmin_ = 0.0;
    bmax_ = 1.0;
  }
}

void
CQChartsSummaryCellObj::
drawOverlay(PaintDevice *device) const
{
  auto cellType = getCellType();

  if (rangeInside_)
    drawRangeBox(device, /*overlay*/true);

  //---

  auto *th = const_cast<CQChartsSummaryCellObj *>(this);

  PenBrush penBrush;

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    th->updateInside();

    for (const auto &pointData : pointDatas_) {
      auto ps = plotToParent(pointData.p);

      //---

      // highlight selected
      if (pointData.rangeSelected || pointData.modelSelected) {
        double psx, psy;
        summaryPlot_->pixelSymbolSize(pointData.symbolSize, psx, psy, /*scale*/true);

        auto pss = 0.75*std::min(psx, psy);
        auto pps = summaryPlot_->windowToPixel(ps);

        auto pbbox = BBox(pps.x - pss, pps.y - pss, pps.x + pss, pps.y + pss);
        auto bbox  = summaryPlot_->pixelToWindow(pbbox);

        if (pointData.rangeSelected) {
          auto fillColor = summaryPlot_->interpColor(summaryPlot_->regionSelectFill(), ColorInd());
          PenBrush penBrush1;
          summaryPlot_->setPenBrush(penBrush1, PenData(false), BrushData(true, fillColor));

          CQChartsDrawUtil::setPenBrush(device, penBrush1);
          drawPointSelection(device, bbox, pointData, penBrush1, SelectionType::RANGE);
        }

        if (pointData.modelSelected) {
          auto penBrush1 = penBrush;
          summaryPlot_->updatePenBrushState(ColorInd(), penBrush1,
            /*selected*/true, /*inside*/false);

          CQChartsDrawUtil::setPenBrush(device, penBrush1);
          drawPointSelection(device, bbox, pointData, penBrush1, SelectionType::MODEL);
        }
      }

      //---

      if (summaryPlot_->insideObject() == this && pointData.inside) {
        auto penBrush1 = penBrush;
        summaryPlot_->updatePenBrushState(ColorInd(), penBrush1,
          /*selected*/false, /*inside*/true);

        CQChartsDrawUtil::setPenBrush(device, penBrush1);

        double psx, psy;
        summaryPlot_->pixelSymbolSize(pointData.symbolSize, psx, psy, /*scale*/true);
        //auto psx = summaryPlot_->lengthPixelWidth (pointData.symbolSize);
        //auto psy = summaryPlot_->lengthPixelHeight(pointData.symbolSize);

        auto ss = Length::pixel(std::min(psx, psy));

        CQChartsDrawUtil::drawSymbol(device, penBrush1, pointData.symbol, ps,
                                     ss, ss, /*scale*/false);
      }
    }
  }
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    th->updateInside();

    for (const auto &rectData : rectDatas_) {
      if (rectData.rangeSelected || rectData.modelSelected) {
        if (rectData.rangeSelected) {
          auto fillColor = summaryPlot_->interpColor(summaryPlot_->regionSelectFill(), ColorInd());
          PenBrush penBrush1;
          summaryPlot_->setPenBrush(penBrush1,
            PenData(true, fillColor, Alpha(0.6), Length::pixel(2)), BrushData(false));

          CQChartsDrawUtil::setPenBrush(device, penBrush1);
          device->drawRect(rectData.pbbox);
        }

        if (rectData.modelSelected) {
          auto penBrush1 = penBrush;
          summaryPlot_->updatePenBrushState(ColorInd(), penBrush1,
            /*selected*/true, /*inside*/false);

          CQChartsDrawUtil::setPenBrush(device, penBrush1);
          device->drawRect(rectData.pbbox);
        }
      }

      if (rectData.inside) {
        auto penBrush1 = penBrush;
        summaryPlot_->updatePenBrushState(ColorInd(), penBrush1,
          /*selected*/false, /*inside*/true);

        CQChartsDrawUtil::setPenBrush(device, penBrush1);
        device->drawRect(rectData.pbbox);
      }
    }
  }

  //---

  if (modifyBox_.isSet()) {
    PenBrush penBrush;

    auto sc = summaryPlot_->interpColor(summaryPlot_->regionEditStroke(), ColorInd());
    auto fc = summaryPlot_->interpColor(summaryPlot_->regionEditFill  (), ColorInd());
    auto fa = summaryPlot_->regionEditAlpha();

    summaryPlot_->setPenBrush(penBrush,
      PenData(true, sc), BrushData(true, fc, Alpha(fa)));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    auto p1 = plotToParent(modifyBox_.getLL());
    auto p2 = plotToParent(modifyBox_.getUR());

    device->drawRect(BBox(p1, p2));
  }

  //---

  if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    if (rangeBox_.isValid() && summaryPlot_->isShowDistributionRange())
      drawDistributionRange(device);
  }
}

void
CQChartsSummaryCellObj::
drawPointSelection(PaintDevice *device, const BBox &bbox, const PointData &pointData,
                   const PenBrush &penBrush, SelectionType type) const
{
  if      (type == SelectionType::RANGE) {
    if (summaryPlot_->regionPointType() == CQChartsSummaryPlot::RegionPointType::OUTLINE_INSIDE) {
      CQChartsDrawUtil::drawSelectedOutline(device, bbox,
                                            summaryPlot_->regionSelectMargin(),
                                            summaryPlot_->regionSelectWidth());
    }
  }
  else if (type == SelectionType::MODEL) {
    if (pointData.symbol.symbolType().type() != CQChartsSymbolType::Type::CIRCLE) {
      auto ps = bbox.getCenter();

      double psx, psy;
      summaryPlot_->pixelSymbolSize(pointData.symbolSize, psx, psy, /*scale*/true);
      //auto psx = summaryPlot_->lengthPixelWidth (pointData.symbolSize);
      //auto psy = summaryPlot_->lengthPixelHeight(pointData.symbolSize);

      auto ss = Length::pixel(std::min(psx, psy));

      CQChartsDrawUtil::drawSymbol(device, penBrush, pointData.symbol, ps,
                                   ss, ss, /*scale*/false);
    }
    else
      device->drawEllipse(bbox);
  }
}

//---

CQChartsGeom::BBox
CQChartsSummaryCellObj::
fitBBox() const
{
  auto bbox = rect();

  if (xAxisBBox_.isValid()) bbox += xAxisBBox_;
  if (yAxisBBox_.isValid()) bbox += yAxisBBox_;

  return bbox;
}

//---

CQChartsGeom::Point
CQChartsSummaryCellObj::
plotToParent(const Point &w) const
{
  double xmin, xmax, ymin, ymax;
  getDataRange(xmin, ymin, xmax, ymax);

  auto px = CMathUtil::map(w.x, xmin, xmax, pxmin_, pxmax_);
  auto py = CMathUtil::map(w.y, ymin, ymax, pymin_, pymax_);

  return Point(px, py);
}

CQChartsGeom::Point
CQChartsSummaryCellObj::
parentToPlot(const Point &p) const
{
  double xmin, xmax, ymin, ymax;
  getDataRange(xmin, ymin, xmax, ymax);

  auto wx = CMathUtil::map(p.x, pxmin_, pxmax_, xmin, xmax);
  auto wy = CMathUtil::map(p.y, pymin_, pymax_, ymin, ymax);

  return Point(wx, wy);
}

void
CQChartsSummaryCellObj::
getDataRange(double &xmin, double &ymin, double &xmax, double &ymax) const
{
  auto cellType = getCellType();

  bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

  if      (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    if (! invert) {
      xmin = bmin_; xmax = bmax_;
      ymin = 0    ; ymax = maxCount_;
    }
    else {
      xmin = 0    ; xmax = maxCount_;
      ymin = bmin_; ymax = bmax_;
    }
  }
  else if (cellType == CQChartsSummaryPlot::CellType::BOXPLOT) {
    if (! invert) {
      xmin = 0.0  ; xmax = 1.0;
      ymin = bmin_; ymax = bmax_;
    }
    else {
      xmin = bmin_; xmax = bmax_;
      ymin = 0.0  ; ymax = 1.0;
    }
  }
  else {
    xmin = xmin_; xmax = xmax_;
    ymin = ymin_; ymax = ymax_;
  }
}

void
CQChartsSummaryCellObj::
initGroupedValues()
{
  CQPerfTrace trace("CQChartsSummaryCellObj::initGroupedValues");

  //---

  if (groupValues_.set)
    return;

  groupValues_.set = true;

  if (! summaryPlot_->groupColumn().isValid())
    return;

  auto *groupDetails = summaryPlot_->columnDetails(summaryPlot_->groupColumn());
  if (! groupDetails) return;

  int nr = groupDetails->numRows();

  if (row_ != col_) {
    auto column1 = colColumn();
    auto column2 = rowColumn();

    auto *details1 = summaryPlot_->columnDetails(column1);
    auto *details2 = summaryPlot_->columnDetails(column2);
    if (! details1 || ! details2) return;

    for (uint ir = 0; ir < uint(nr); ++ir) {
      auto var = groupDetails->value(ir);

      int ig = groupDetails->uniqueId(var);

      auto &indData = groupValues_.groupIndData[ig];

      auto value1 = details1->value(ir);
      auto value2 = details2->value(ir);

      indData.xvals.push_back(value1);
      indData.yvals.push_back(value2);

      double x = 0.0, y = 0.0;

      if (details1->isNumeric()) {
        bool ok;
        x = CQChartsVariant::toReal(value1, ok);

        indData.x.push_back(x);
      }

      if (details2->isNumeric()) {
        bool ok;
        y = CQChartsVariant::toReal(value2, ok);

        indData.y.push_back(y);
      }

      indData.poly.addPoint(Point(x, y));
    }
  }
  else {
    auto column = rowColumn();

    auto *details = summaryPlot_->columnDetails(column);
    if (! details) return;

    for (uint ir = 0; ir < uint(nr); ++ir) {
      auto var = groupDetails->value(ir);

      int ig = groupDetails->uniqueId(var);

      auto &indData = groupValues_.groupIndData[ig];

      auto value = details->value(ir);

      indData.xvals.push_back(value);
      indData.yvals.push_back(value);

      if (details->isNumeric()) {
        bool ok;
        double r = CQChartsVariant::toReal(value, ok);

        indData.x.push_back(r);
        indData.y.push_back(r);

        indData.poly.addPoint(Point(r, r));
      }
    }
  }
}

bool
CQChartsSummaryCellObj::
handleSelectPress(const Point &, SelMod)
{
  //updateSelectData(p);

  return true;
}

bool
CQChartsSummaryCellObj::
handleSelectMove(const Point &p, Constraints, bool)
{
  updateSelectData(p);

  updateInside();

  if (! selectPointDatas_.empty() || selectRectData_)
    const_cast<CQChartsSummaryPlot *>(summaryPlot_)->invalidateOverlay();

  return true;
}

bool
CQChartsSummaryCellObj::
handleSelectRelease(const Point &p, bool add)
{
  updateSelectData(p);

  auto cellType = getCellType();

  if      (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    if (selectRectData_) {
      auto *summaryPlot = const_cast<CQChartsSummaryPlot *>(summaryPlot_);

      // update region from select
      if (summaryPlot_->barSelectTarget() == CQChartsSummaryPlot::SelectTarget::REGION) {
        bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

        auto minMax = (invert ?
          MinMax(selectRectData_->bbox.getYMin(), selectRectData_->bbox.getYMax()) :
          MinMax(selectRectData_->bbox.getXMin(), selectRectData_->bbox.getXMax()));

        auto column = rowColumn();

        if (add && summaryPlot->hasColumnRange(column)) {
          auto range = summaryPlot->columnRange(column);

          minMax.add(range.min());
          minMax.add(range.max());
        }

        summaryPlot->setColumnRange(column, minMax.min(), minMax.max());

        summaryPlot->updateColumnRanges();
      }
      else {
        if (! add)
          summaryPlot->clearModelSelectedRows(/*update*/false);

        for (const auto &r : selectRectData_->rows)
          summaryPlot->addModelSelectedRow(r, rowColumn().column(), /*update*/false);

        summaryPlot->updateSelectionRows();

        summaryPlot->view()->updateSelText();

        summaryPlot->drawObjs();
      }

      //---

//    summaryPlot_->selectCellRect(this, minMax, add);
    }
#if 0
    else {
      if (! add)
        summaryPlot_->clearSelection();
    }
#endif
  }
  else if (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    if (! selectPointDatas_.empty()) {
      auto selectPointDataInd = getCurrentSelectPointInd();

      summaryPlot_->selectCellPoint(this, selectPointDataInd, add);
    }
    else {
      if (! add)
        summaryPlot_->clearSelection();
    }
  }

  return true;
}

//---

bool
CQChartsSummaryCellObj::
handleEditPress(const Point &p)
{
  modifyPress_ = parentToPlot(p);

  if (rangeInside_)
    modifyBox_ = rangeBox_;
  else
    modifyBox_ = BBox();

  return true;
}

bool
CQChartsSummaryCellObj::
handleEditMove(const Point &p)
{
  auto modifyMove = parentToPlot(p);

  if (modifyBox_.isSet()) {
    double xmin, xmax, ymin, ymax;
    getDataRange(xmin, ymin, xmax, ymax);

    double dx = modifyMove.x - modifyPress_.x;
    double dy = modifyMove.y - modifyPress_.y;

    auto cellType = getCellType();

    if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
      bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

      if (! invert)
        dy = 0.0;
      else
        dx = 0.0;
    }

    if (rangeBoxSide_) {
      if      (rangeBoxSide_ & Qt::AlignLeft) {
        modifyBox_.setXMin(modifyBox_.getXMin() + dx);
        if (modifyBox_.getXMin() < xmin) modifyBox_.setXMin(xmin);
      }
      else if (rangeBoxSide_ & Qt::AlignRight) {
        modifyBox_.setXMax(modifyBox_.getXMax() + dx);
        if (modifyBox_.getXMax() > xmax) modifyBox_.setXMax(xmax);
      }
      if      (rangeBoxSide_ & Qt::AlignBottom) {
        modifyBox_.setYMin(modifyBox_.getYMin() + dy);
        if (modifyBox_.getYMin() < ymin) modifyBox_.setYMin(ymin);
      }
      else if (rangeBoxSide_ & Qt::AlignTop) {
        modifyBox_.setYMax(modifyBox_.getYMax() + dy);
        if (modifyBox_.getYMax() > ymax) modifyBox_.setYMax(ymax);
      }
    }
    else {
      modifyBox_ = modifyBox_.translated(dx, dy);

      dx = 0.0;
      dy = 0.0;

      if      (modifyBox_.getXMin() < xmin) dx = xmin - modifyBox_.getXMin();
      else if (modifyBox_.getXMax() > xmax) dx = xmax - modifyBox_.getXMax();
      if      (modifyBox_.getYMin() < ymin) dy = ymin - modifyBox_.getYMin();
      else if (modifyBox_.getYMax() > ymax) dy = ymax - modifyBox_.getYMax();

      modifyBox_ = modifyBox_.translated(dx, dy);
    }

    const_cast<CQChartsSummaryPlot *>(summaryPlot_)->invalidateOverlay();
  }

  modifyPress_ = modifyMove;

  return true;
}

bool
CQChartsSummaryCellObj::
handleEditRelease(const Point &)
{
  if (modifyBox_.isSet()) {
    double xmin, xmax, ymin, ymax;
    getDataRange(xmin, ymin, xmax, ymax);

    modifyBox_ = BBox(std::max(xmin, modifyBox_.getXMin()), std::max(ymin, modifyBox_.getYMin()),
                      std::min(xmax, modifyBox_.getXMax()), std::min(ymax, modifyBox_.getYMax()));

    if (! modifyBox_.isValid())
      modifyBox_ = BBox();

    //---

    auto cellType = getCellType();

    if      (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
      auto column1 = colColumn();
      auto column2 = rowColumn();

      auto *summaryPlot = const_cast<CQChartsSummaryPlot *>(summaryPlot_);

      if (modifyBox_.isValid()) {
        summaryPlot->setColumnRange(column1, modifyBox_.getXMin(), modifyBox_.getXMax());
        summaryPlot->setColumnRange(column2, modifyBox_.getYMin(), modifyBox_.getYMax());
      }
      else {
        summaryPlot->resetColumnRange(column1);
        summaryPlot->resetColumnRange(column2);
      }

      summaryPlot->updateColumnRanges();
    }
    else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
      auto column = colColumn();

      auto *summaryPlot = const_cast<CQChartsSummaryPlot *>(summaryPlot_);

      if (modifyBox_.isValid()) {
        bool invert = (summaryPlot_->orientation() == Qt::Horizontal);

        if (! invert)
          summaryPlot->setColumnRange(column, modifyBox_.getXMin(), modifyBox_.getXMax());
        else
          summaryPlot->setColumnRange(column, modifyBox_.getYMin(), modifyBox_.getYMax());
      }
      else
        summaryPlot->resetColumnRange(column);

      summaryPlot->updateColumnRanges();
    }
  }

  modifyBox_ = BBox();

  return true;
}

//----

void
CQChartsSummaryCellObj::
updateRangeInside(const Point &p)
{
  rangeInside_ = false;

  updateRangeBox();

  if (rangeBox_.isValid()) {
    double margin = 4; // pixels

    auto rbbox  = BBox(plotToParent(rangeBox_.getLL()), plotToParent(rangeBox_.getUR()));
    auto prbbox = summaryPlot_->windowToPixel(rbbox);
    prbbox.expand(-margin, -margin, margin, margin);
    rbbox = summaryPlot_->pixelToWindow(prbbox);

    if (rbbox.inside(p)) {
      rangeInside_ = true;

      auto p1 = plotToParent(rangeBox_.getLL());
      auto p2 = plotToParent(rangeBox_.getUR());

      double dx1 = summaryPlot_->windowToPixelWidth (std::abs(p1.x - p.x));
      double dy1 = summaryPlot_->windowToPixelHeight(std::abs(p1.y - p.y));
      double dx2 = summaryPlot_->windowToPixelWidth (std::abs(p2.x - p.x));
      double dy2 = summaryPlot_->windowToPixelHeight(std::abs(p2.y - p.y));

      rangeBoxSide_ = Qt::Alignment();

      if (dx1 <= 4) rangeBoxSide_ |= Qt::AlignLeft;
      if (dy1 <= 4) rangeBoxSide_ |= Qt::AlignBottom;
      if (dx2 <= 4) rangeBoxSide_ |= Qt::AlignRight;
      if (dy2 <= 4) rangeBoxSide_ |= Qt::AlignTop;

      const_cast<CQChartsSummaryPlot *>(summaryPlot_)->invalidateOverlay();
    }
  }
}

//---

bool
CQChartsSummaryCellObj::
resetInside()
{
  bool changed = false;

  for (auto &pointData : pointDatas_) {
    if (pointData.inside) {
      pointData.inside = false;
      changed = true;
    }
  }

  for (auto &rectData : rectDatas_) {
    if (rectData.inside) {
      rectData.inside = false;
      changed = true;
    }
  }

  if (rangeInside_) {
    rangeInside_ = false;
    changed = true;
  }

  return changed;
}

void
CQChartsSummaryCellObj::
updateSelectData(const Point &p)
{
  if (p.distanceTo(selectPointPos_) < 1E-6)
    return;

  selectPointPos_ = p;

  selectPointDatas_.clear();

  selectRectData_ = nullptr;
  selectInd_      = 0;

  auto cellType = getCellType();

  bool selectInside = summaryPlot_->isSelectInside();

  if (! summaryPlot_->anyColumnRange())
    selectInside = false;

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    auto symbolSize = summaryPlot_->calcScatterSymbolSize();

    double sx, sy;
    summaryPlot_->plotSymbolSize(symbolSize, sx, sy, /*scale*/true);

    auto s = std::min(sx, sy);

    for (auto &pointData : pointDatas_) {
      if (selectInside && ! pointData.rangeSelected)
        continue;

      auto p2 = plotToParent(pointData.p);

      auto d = p.distanceTo(p2);
      if (d > s) continue;

      selectPointDatas_.push_back(pointData.ind);
    }

    //if (! selectPointDatas_.empty()) {
    //  auto selectPointDataInd = *selectPointDatas_.begin();
    //  std::cerr << "Point " << selectPointDataInd << "\n";
    //}
  }
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION) {
    for (auto &rectData : rectDatas_) {
      if (rectData.pbbox.inside(p)) {
        selectRectData_ = &rectData;
        break;
      }
    }

    //if (selectRectData_)
    //  std::cerr << "Rect " << selectRectData_->ind << "\n";
  }
}

bool
CQChartsSummaryCellObj::
updateInside()
{
  bool changed = false;

  if (! selectPointDatas_.empty()) {
    int ind = 0;

    for (const auto &selectPointDataInd : selectPointDatas_) {
      if (selectPointDataInd < 0 || selectPointDataInd >= int(pointDatas_.size()))
        continue;

      auto &selectPointData = pointDatas_[selectPointDataInd];

      bool inside = (summaryPlot_->insideObject() == this && selectInd_ == ind);

      if (inside != selectPointData.inside) {
        changed = true;

        selectPointData.inside = inside;
      }

      ++ind;
    }
  }

  if (selectRectData_) {
    bool inside = (summaryPlot_->insideObject() == this);

    if (inside != selectRectData_->inside) {
      changed = true;

      selectRectData_->inside = inside;
    }
  }

  return changed;
}

#if 0
int
CQChartsSummaryCellObj::
numTipObjs() const
{
  if (summaryPlot_->selectMode() != CQChartsSummaryPlot::SelectMode::DATA)
    return 1;

  auto cellType = getCellType();

  if      (cellType == CQChartsSummaryPlot::CellType::SCATTER)
    return int(selectPointDatas_.size());
  else if (cellType == CQChartsSummaryPlot::CellType::DISTRIBUTION)
    return (selectRectData_ ? 1 : 0);
  else
    return 0;
}

int
CQChartsSummaryCellObj::
tipObjNum() const
{
  if (summaryPlot_->selectMode() != CQChartsSummaryPlot::SelectMode::DATA)
    return 0;

  auto cellType = getCellType();

  if (cellType == CQChartsSummaryPlot::CellType::SCATTER)
    return selectInd_;
  else
    return 0;
}

bool
CQChartsSummaryCellObj::
nextInsideInd(bool force) const
{
  if (summaryPlot_->selectMode() != CQChartsSummaryPlot::SelectMode::DATA)
    return false;

  auto cellType = getCellType();

  if (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    if (! force && selectInd_ >= int(selectPointDatas_.size()) - 1)
      return false;

    ++selectInd_;

    if (selectInd_ >= int(selectPointDatas_.size()))
      selectInd_ = 0;

    if (const_cast<CQChartsSummaryCellObj *>(this)->updateInside() || force)
      const_cast<CQChartsSummaryPlot *>(summaryPlot_)->invalidateOverlay();

    return true;
  }
  else
    return false;
}

bool
CQChartsSummaryCellObj::
prevInsideInd(bool force) const
{
  if (summaryPlot_->selectMode() != CQChartsSummaryPlot::SelectMode::DATA)
    return false;

  auto cellType = getCellType();

  if (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    if (! force && selectInd_ <= 0)
      return false;

    --selectInd_;

    if (selectInd_ < 0)
      selectInd_ = std::max(int(selectPointDatas_.size()) - 1, 0);

    if (const_cast<CQChartsSummaryCellObj *>(this)->updateInside() || force)
      const_cast<CQChartsSummaryPlot *>(summaryPlot_)->invalidateOverlay();

    return true;
  }
  else
    return false;
}

bool
CQChartsSummaryCellObj::
resetInsideInd(bool force) const
{
  if (summaryPlot_->selectMode() != CQChartsSummaryPlot::SelectMode::DATA)
    return false;

  auto cellType = getCellType();

  if (cellType == CQChartsSummaryPlot::CellType::SCATTER) {
    if (! force && selectInd_ == 0)
      return false;

    selectInd_ = 0;

    if (const_cast<CQChartsSummaryCellObj *>(this)->updateInside() || force)
      const_cast<CQChartsSummaryPlot *>(summaryPlot_)->invalidateOverlay();

    return true;
  }
  else
    return false;
}
#endif

CQChartsSummaryCellObj::Color
CQChartsSummaryCellObj::
pointColor() const
{
  ColorInd colorInd1(row_, nc_);
  ColorInd colorInd2(col_, nc_);

  auto fc1 = summaryPlot_->interpScatterSymbolFillColor(colorInd1);
  auto fc2 = summaryPlot_->interpScatterSymbolFillColor(colorInd2);

  auto c = CQChartsUtil::blendColors(fc1, fc2, 0.5);

  return Color(c);
}

CQChartsSummaryCellObj::Color
CQChartsSummaryCellObj::
barColor() const
{
  ColorInd colorInd(row_, nc_);

  auto c = summaryPlot_->interpDistributionFillColor(colorInd);

  return Color(c);
}

QString
CQChartsSummaryCellObj::
objText() const
{
  QString str;

  auto *idDetails = (summaryPlot_->idColumn().isValid() ?
    summaryPlot_->columnDetails(summaryPlot_->idColumn()) : nullptr);

  auto addInd = [&](int ind) {
    bool ok = false;
    QString str1;
    if (idDetails)
      str1 = CQChartsVariant::toString(idDetails->value(ind), ok);
    if (! ok)
      str1 = QString::number(ind);
    if (str != "") str += ", ";
    str += str1;
  };

  if (! selectPointDatas_.empty()) {
    auto selectPointDataInd = getCurrentSelectPointInd();

    addInd(selectPointDataInd);
  }

  if (selectRectData_) {
    for (const auto &r : selectRectData_->rows)
      addInd(r);
  }

  return str;
}

int
CQChartsSummaryCellObj::
getCurrentSelectPointInd() const
{
  int selectInd = selectInd_;

  if (selectInd < 0 || selectInd >= int(selectPointDatas_.size())) selectInd = 0;

  return selectPointDatas_[selectInd];
}

//------

CQChartsSummaryPlotGroupStats::
CQChartsSummaryPlotGroupStats(CQChartsSummaryPlot *summaryPlot) :
 summaryPlot_(summaryPlot)
{
  setObjectName("groupStats");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  valueList_ = CQUtil::makeWidget<CQTableWidget>("valueList");

  layout->addWidget(valueList_);
}

void
CQChartsSummaryPlotGroupStats::
updateWidgets()
{
  auto groupColumn = (summaryPlot_ ? summaryPlot_->groupColumn() : CQChartsColumn());

  setVisible(groupColumn.isValid());

  valueList_->clear();

  if (! groupColumn.isValid())
    return;

  auto *groupDetails = summaryPlot_->columnDetails(summaryPlot_->groupColumn());
  if (! groupDetails) return;

  //---

  auto createHeaderItem = [&](int c, const QString &name) {
    valueList_->setHorizontalHeaderItem(c, new QTableWidgetItem(name));
  };

  valueList_->setColumnCount(3);

  createHeaderItem(0, "Color");
  createHeaderItem(1, "Value");
  createHeaderItem(2, "Count");

  //---

  auto createColorTableItem = [&](const QColor &color) {
    auto *item = new CQTableWidgetColorItem(valueList_, color);

    item->setToolTip(color.name());
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    return item;
  };

  auto createStringTableItem = [&](const QString &str) {
    auto *item = new QTableWidgetItem(str);

    item->setToolTip(str);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    return item;
  };

  if (groupDetails->isNumeric()) {
    CQChartsSummaryPlot::BucketCount bucketCount;
    int                              maxCount = 0;
    double                           bmin, bmax;

    summaryPlot_->calcBucketCounts(summaryPlot_->groupColumn(), bucketCount,
                                   maxCount, bmin, bmax);

    int ig = 0;
    int ng = int(bucketCount.size());

    valueList_->setRowCount(ng);

    for (const auto &bc : bucketCount) {
      QVariant vmin, vmax;

      groupDetails->bucketRange(bc.first, vmin, vmax);

      bool ok;
      auto rmin = CQChartsVariant::toReal(vmin, ok);
      auto rmax = CQChartsVariant::toReal(vmax, ok);

      auto c = summaryPlot_->interpPaletteColor(CQChartsUtil::ColorInd(ig, ng));

      auto nb = bc.second.size();

      auto *colorItem = createColorTableItem(c);
      auto *valueItem = createStringTableItem(QString("[%1,%2)").arg(rmin).arg(rmax));
      auto *countItem = createStringTableItem(QString::number(nb));

      valueList_->setItem(ig, 0, colorItem);
      valueList_->setItem(ig, 1, valueItem);
      valueList_->setItem(ig, 2, countItem);

      ++ig;
    }
  }
  else {
    int ig = 0;
    int ng = groupDetails->numUnique();

    valueList_->setRowCount(ng);

    for (const auto &valueCount : groupDetails->uniqueValueCounts()) {
      auto c = summaryPlot_->interpPaletteColor(CQChartsUtil::ColorInd(ig, ng));

      auto *colorItem = createColorTableItem(c);
      auto *valueItem = createStringTableItem(valueCount.first.toString());
      auto *countItem = createStringTableItem(QString::number(valueCount.second));

      valueList_->setItem(ig, 0, colorItem);
      valueList_->setItem(ig, 1, valueItem);
      valueList_->setItem(ig, 2, countItem);

      ++ig;
    }
  }

  setMinimumHeight(sizeHint().height());

  valueList_->fixTableColumnWidths();
}

QSize
CQChartsSummaryPlotGroupStats::
sizeHint() const
{
  QFontMetrics fm(font());

  int nr = std::min(valueList_->rowCount() + 1, 6);

  return QSize(fm.horizontalAdvance("X")*40, (fm.height() + 6)*nr + 8);
}

//------

CQChartsSummaryPlotRangeList::
CQChartsSummaryPlotRangeList(CQChartsSummaryPlot *summaryPlot) :
 summaryPlot_(summaryPlot)
{
  setObjectName("rangeList");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  auto *scrollArea = CQUtil::makeWidget<QScrollArea>("scroll");

  layout->addWidget(scrollArea);

  auto *listFrame = CQUtil::makeWidget<QFrame>("listFrame");

  scrollArea->setWidget(listFrame);
  scrollArea->setWidgetResizable(true);

  layout_ = CQUtil::makeLayout<QGridLayout>(listFrame, 0, 0);
}

void
CQChartsSummaryPlotRangeList::
updateWidgets()
{
  int nc = (summaryPlot_ ? summaryPlot_->columns().count() : 0);

  if (int(widgets_.size()) != nc) {
    QLayoutItem *child;

    while ((child = layout_->takeAt(0)) != nullptr) {
      delete child->widget();
      delete child;
    }

    widgets_.resize(nc);

    for (int i = 0; i < nc; ++i) {
      auto &widget = widgets_[i];

      widget.label = new CQChartsSummaryPlotRangeLabel(this, "");
      widget.label->setObjectName(QString("label%1").arg(i));

      layout_->addWidget(widget.label, i, 0);

      widget.frame = CQUtil::makeWidget<QFrame>(QString("editFrame%1").arg(i));

      auto *editLayout = CQUtil::makeLayout<QHBoxLayout>(widget.frame);

      layout_->addWidget(widget.frame, i, 1);

      widget.edit   = new CQChartsGeomMinMaxEdit; widget.edit->setObjectName("edit");
      widget.noedit = new CQChartsSummaryPlotRangeNoEdit(this);

      widget.edit->setProperty("column", i);

      widget.noedit->setProperty("column", i);

      connect(widget.edit, SIGNAL(valueChanged()), this, SLOT(rangeChanged()));

      editLayout->addWidget(widget.edit);
      editLayout->addWidget(widget.noedit);

      widget.clear = CQUtil::makeWidget<CQIconButton>("clear");
      widget.clear->setIcon("CLEAR_BUTTON");
      widget.clear->setToolTip("Clear Range");

      widget.clear->setProperty("column", i);

      connect(widget.clear, SIGNAL(clicked()), this, SLOT(clearRange()));

      editLayout->addWidget(widget.clear);
    }

    layout_->setColumnStretch(1, 1);
    layout_->setRowStretch(nc, 1);
  }

  for (int i = 0; i < nc; ++i) {
    auto &widget = widgets_[i];

    const auto &column = summaryPlot_->columns().getColumn(i);

    bool ok;
    auto name = summaryPlot_->modelHHeaderString(column, ok);
    if (name == "") name = QString::number(i);

    widget.label->setText(name);

    bool hasRange = summaryPlot_->hasColumnRange(column);

    if (hasRange) {
      widget.edit->setValue(summaryPlot_->columnRange(column));

//    widget.edit->setToolTip(QString("%1 (%2 - %3)").
//      arg(name).arg(widget.edit->getValue().min()).arg(widget.edit->getValue().max()));
    }

    widget.edit  ->setVisible(hasRange);
    widget.noedit->setVisible(! hasRange);

    widget.clear->setEnabled(hasRange);
  }
}

void
CQChartsSummaryPlotRangeList::
rangeChanged()
{
  if (! summaryPlot_) return;

  auto *rangeEdit = qobject_cast<CQChartsGeomMinMaxEdit *>(sender());
  if (! rangeEdit) return;

  bool ok;
  int i = rangeEdit->property("column").toInt(&ok);
  if (! ok) return;

  int nc = summaryPlot_->columns().count();
  if (i < 0 || i >= nc) return;

  const auto &column = summaryPlot_->columns().getColumn(i);

  auto range = rangeEdit->getValue();

  summaryPlot_->setColumnRange(column, range.min(), range.max());

  summaryPlot_->updateColumnRanges(/*update*/false);
}

void
CQChartsSummaryPlotRangeList::
clearRange()
{
  if (! summaryPlot_) return;

  auto *button = qobject_cast<CQIconButton *>(sender());
  if (! button) return;

  bool ok;
  int i = button->property("column").toInt(&ok);
  if (! ok) return;

  int nc = summaryPlot_->columns().count();
  if (i < 0 || i >= nc) return;

  const auto &column = summaryPlot_->columns().getColumn(i);

  summaryPlot_->resetColumnRange(column);

  summaryPlot_->updateColumnRanges();
}

QSize
CQChartsSummaryPlotRangeList::
sizeHint() const
{
  QFontMetrics fm(font());

  int nc = (summaryPlot_ ? summaryPlot_->columns().count() : 0);

  int nr = std::min(nc + 1, 6);

  return QSize(fm.horizontalAdvance("X")*40, (fm.height() + 6)*nr + 8);
}

//---

CQChartsSummaryPlotRangeLabel::
CQChartsSummaryPlotRangeLabel(CQChartsSummaryPlotRangeList *list, const QString &label) :
 list_(list)
{
  setObjectName("label");

  setText(label);

  auto font = this->font();
  font.setBold(true);
  setFont(font);
};

CQChartsSummaryPlotRangeNoEdit::
CQChartsSummaryPlotRangeNoEdit(CQChartsSummaryPlotRangeList *list) :
 list_(list)
{
  setObjectName("noedit");
  setText("No Range");

  setToolTip("No Range - double click to enter range");

  auto font = this->font();
  font.setItalic(true);
  setFont(font);
}

void
CQChartsSummaryPlotRangeNoEdit::
mouseDoubleClickEvent(QMouseEvent *)
{
  bool ok;
  int i = property("column").toInt(&ok);
  if (! ok) return;

  auto *summaryPlot = list_->summaryPlot();

  int nc = summaryPlot->columns().count();
  if (i < 0 || i >= nc) return;

  const auto &column = summaryPlot->columns().getColumn(i);

  double cmin, cmax;
  if (! summaryPlot->calcColumnRange(column, cmin, cmax))
    return;

  summaryPlot->setColumnRange(column, cmin, cmax);

  summaryPlot->updateColumnRanges();
}

//------

CQChartsSummaryPlotColumnChooser::
CQChartsSummaryPlotColumnChooser(CQChartsSummaryPlot *summaryPlot) :
 CQChartsPlotColumnChooser(summaryPlot), summaryPlot_(summaryPlot)
{
  setShowColumnColor(true);
}

void
CQChartsSummaryPlotColumnChooser::
setPlot(Plot *plot)
{
  summaryPlot_ = qobject_cast<CQChartsSummaryPlot *>(plot);

  CQChartsPlotColumnChooser::setPlot(plot);
}

const CQChartsColumns &
CQChartsSummaryPlotColumnChooser::
getColumns() const
{
  return summaryPlot_->columns();
}

bool
CQChartsSummaryPlotColumnChooser::
isColumnVisible(int ic) const
{
  return summaryPlot_->isColumnVisible(ic);
}

void
CQChartsSummaryPlotColumnChooser::
setColumnVisible(int ic, bool visible)
{
  summaryPlot_->setColumnVisible(ic, visible);
}

QColor
CQChartsSummaryPlotColumnChooser::
columnColor(int ic) const
{
  int nc = summaryPlot_->visibleColumns().count();

  CQChartsUtil::ColorInd colorInd(ic, nc);

  auto fc = summaryPlot_->interpDistributionFillColor(colorInd);

  return fc;
}

//------

CQChartsSummaryPlotCustomControls::
CQChartsSummaryPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "summary")
{
}

void
CQChartsSummaryPlotCustomControls::
init()
{
  addWidgets();

  connectSlots(true);
}

void
CQChartsSummaryPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addGroupStatsWidgets();

  addRangeList();

  addOptionsWidgets();

  addOverview();

  addLayoutStretch();

  addExpandControls();

  addRangeControls();
}

void
CQChartsSummaryPlotCustomControls::
addColumnWidgets()
{
  // columns frame
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addNamedColumnWidgets(QStringList() << "columns", columnsFrame);

  chooser_ = new CQChartsSummaryPlotColumnChooser;

  addFrameWidget(columnsFrame, chooser_);
}

void
CQChartsSummaryPlotCustomControls::
addGroupStatsWidgets()
{
  // group frame
  auto groupFrame = createGroupFrame("Group", "groupFrame");

  addNamedColumnWidgets(QStringList() << "group", groupFrame);

  stats_ = new CQChartsSummaryPlotGroupStats;

  addFrameWidget(groupFrame, stats_);
}

void
CQChartsSummaryPlotCustomControls::
addRangeList()
{
  // group frame
  auto rangeFrame = createGroupFrame("Column Ranges", "rangeFrame");

  rangeList_ = new CQChartsSummaryPlotRangeList;

  addFrameWidget(rangeFrame, rangeList_);
}

void
CQChartsSummaryPlotCustomControls::
addOptionsWidgets()
{
  // options frame
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  plotTypeCombo_ = createEnumEdit("plotType");

  diagonalTypeCombo_      = createEnumEdit("diagonalType");
  upperDiagonalTypeCombo_ = createEnumEdit("upperDiagonalType");
  lowerDiagonalTypeCombo_ = createEnumEdit("lowerDiagonalType");

  addFrameWidget(optionsFrame_, "Plot Type", plotTypeCombo_);

  addFrameWidget(optionsFrame_, "Diagonal Type"  , diagonalTypeCombo_);
  addFrameWidget(optionsFrame_, "Upper Cell Type", upperDiagonalTypeCombo_);
  addFrameWidget(optionsFrame_, "Lower Cell Type", lowerDiagonalTypeCombo_);

  bestFitCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Best Fit", "bestFitCheck");
  densityCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Density" , "densityCheck");
  paretoCheck_  = CQUtil::makeLabelWidget<QCheckBox>("Pareto"  , "paretoCheck");

  bestFitCheck_->setToolTip("Show Best Fit");
  densityCheck_->setToolTip("Show Density");
  paretoCheck_ ->setToolTip("Show Pareto Front");

  auto *boolFrame  = CQUtil::makeWidget<QFrame>("boolFrame");
  auto *boolLayout = CQUtil::makeLayout<QHBoxLayout>(boolFrame, 2, 2);

  boolLayout->addWidget(bestFitCheck_);
  boolLayout->addWidget(densityCheck_);
  boolLayout->addWidget(paretoCheck_);
  boolLayout->addStretch(1);

  addFrameColWidget(optionsFrame_, boolFrame, 2);
}

void
CQChartsSummaryPlotCustomControls::
addExpandControls()
{
  // buttons group
  buttonsFrame_ = createGroupFrame("Functions", "buttonsFrame", FrameOpts::makeHBox());

  expandButton_ = CQUtil::makeLabelWidget<QPushButton>("Expand", "expandButton");

  buttonsFrame_.box->addWidget (expandButton_);
  buttonsFrame_.box->addStretch(1);
}

void
CQChartsSummaryPlotCustomControls::
addRangeControls()
{
  // ranges group
  rangesFrame_ = createGroupFrame("Ranges", "rangesFrame", FrameOpts::makeHBox());

  deselectButton_ = CQUtil::makeLabelWidget<QPushButton>("Clear Ranges" , "deselectButton");
  selectButton_   = CQUtil::makeLabelWidget<QPushButton>("Select Ranges", "selectButton");

  deselectButton_->setToolTip("Reset Ranges");
  selectButton_  ->setToolTip("Select Model Items from Range");

  rangesFrame_.box->addWidget (deselectButton_);
  rangesFrame_.box->addWidget (selectButton_);
  rangesFrame_.box->addStretch(1);
}

void
CQChartsSummaryPlotCustomControls::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    diagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(diagonalTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    upperDiagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(upperDiagonalTypeSlot()));
  CQUtil::optConnectDisconnect(b,
    lowerDiagonalTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(lowerDiagonalTypeSlot()));

  CQUtil::optConnectDisconnect(b,
    bestFitCheck_, SIGNAL(stateChanged(int)), this, SLOT(bestFitSlot(int)));
  CQUtil::optConnectDisconnect(b,
    densityCheck_, SIGNAL(stateChanged(int)), this, SLOT(densitySlot(int)));
  CQUtil::optConnectDisconnect(b,
    paretoCheck_ , SIGNAL(stateChanged(int)), this, SLOT(paretoSlot(int)));

  CQUtil::optConnectDisconnect(b,
    expandButton_, SIGNAL(clicked()), this, SLOT(expandSlot()));
  CQUtil::optConnectDisconnect(b,
    deselectButton_, SIGNAL(clicked()), this, SLOT(deselectSlot()));
  CQUtil::optConnectDisconnect(b,
    selectButton_, SIGNAL(clicked()), this, SLOT(selectSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsSummaryPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && summaryPlot_) {
    disconnect(summaryPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
    disconnect(summaryPlot_, SIGNAL(selectionChanged()), this, SLOT(updateWidgets()));
  }

  summaryPlot_ = dynamic_cast<CQChartsSummaryPlot *>(plot);

  stats_    ->setPlot(summaryPlot_);
  chooser_  ->setPlot(summaryPlot_);
  rangeList_->setPlot(summaryPlot_);

  CQChartsPlotCustomControls::setPlot(plot);

  if (summaryPlot_) {
    connect(summaryPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
    connect(summaryPlot_, SIGNAL(selectionChanged()), this, SLOT(updateWidgets()));
  }
}

void
CQChartsSummaryPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  plotTypeCombo_         ->setCurrentValue(static_cast<int>(summaryPlot_->plotType()));
  diagonalTypeCombo_     ->setCurrentValue(static_cast<int>(summaryPlot_->diagonalType()));
  upperDiagonalTypeCombo_->setCurrentValue(static_cast<int>(summaryPlot_->upperDiagonalType()));
  lowerDiagonalTypeCombo_->setCurrentValue(static_cast<int>(summaryPlot_->lowerDiagonalType()));

  bestFitCheck_->setChecked(summaryPlot_->isBestFit());
  densityCheck_->setChecked(summaryPlot_->isDensity());
  paretoCheck_ ->setChecked(summaryPlot_->isPareto());

  stats_    ->updateWidgets();
  chooser_  ->updateWidgets();
  rangeList_->updateWidgets();

  //---

  if (! summaryPlot_->isExpanded()) {
    expandButton_->setText("Expand");
    expandButton_->setToolTip("Expand Selected Plot");
    expandButton_->setEnabled(summaryPlot_->selectedCellObj());
  }
  else {
    expandButton_->setText("Collapse");
    expandButton_->setToolTip("Collapse back to Summary");
    expandButton_->setEnabled(true);
  }

  if (rangesFrame_.groupBox) {
    int n = summaryPlot_->numRangeSelectedRows();

    if (n > 0)
      rangesFrame_.groupBox->setTitle(QString("Ranges (#%1 Rows)").arg(n));
    else
      rangesFrame_.groupBox->setTitle("Ranges");
  }

  //---

  connectSlots(true);

  //---

  CQChartsPlotCustomControls::updateWidgets();
}

void
CQChartsSummaryPlotCustomControls::
plotTypeSlot()
{
  summaryPlot_->setPlotType(
    static_cast<CQChartsSummaryPlot::PlotType>(plotTypeCombo_->currentValue()), /*update*/false);

  QTimer::singleShot(10, summaryPlot_, SLOT(updatePlotsSlot()));
}

void
CQChartsSummaryPlotCustomControls::
diagonalTypeSlot()
{
  summaryPlot_->setDiagonalType(
    static_cast<CQChartsSummaryPlot::DiagonalType>(diagonalTypeCombo_->currentValue()));
}

void
CQChartsSummaryPlotCustomControls::
upperDiagonalTypeSlot()
{
  summaryPlot_->setUpperDiagonalType(
    static_cast<CQChartsSummaryPlot::OffDiagonalType>(upperDiagonalTypeCombo_->currentValue()));
}

void
CQChartsSummaryPlotCustomControls::
lowerDiagonalTypeSlot()
{
  summaryPlot_->setLowerDiagonalType(
    static_cast<CQChartsSummaryPlot::OffDiagonalType>(lowerDiagonalTypeCombo_->currentValue()));
}

void
CQChartsSummaryPlotCustomControls::
bestFitSlot(int state)
{
  summaryPlot_->setBestFit(state);
}

void
CQChartsSummaryPlotCustomControls::
densitySlot(int state)
{
  summaryPlot_->setDensity(state);
}

void
CQChartsSummaryPlotCustomControls::
paretoSlot(int state)
{
  summaryPlot_->setPareto(state);
}

void
CQChartsSummaryPlotCustomControls::
expandSlot()
{
  if (! summaryPlot_->isExpanded())
    summaryPlot_->expandCell(summaryPlot_->selectedCellObj());
  else
    summaryPlot_->collapseCell();
}

void
CQChartsSummaryPlotCustomControls::
deselectSlot()
{
  summaryPlot_->clearColumnRanges();
}

void
CQChartsSummaryPlotCustomControls::
selectSlot()
{
  summaryPlot_->selectColumnRanges();
}
