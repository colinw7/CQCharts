#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsDataLabel.h>
#include <CQChartsValueSet.h>
#include <CQCharts.h>
#include <CQChartsDensity.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsRand.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>
#include <CQIconRadio.h>
#include <CQDoubleRangeSlider.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>
#include <CQGroupBox.h>
#include <CQLabel.h>

#include <CMathRound.h>

#include <QMenu>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QAction>

CQChartsDistributionPlotType::
CQChartsDistributionPlotType()
{
}

void
CQChartsDistributionPlotType::
addParameters()
{
  startParameterGroup("Distribution");

  // values (auto bucketed)
  addColumnsParameter("values", "Values", "valueColumns").
    setRequired().setNumericColumn().setPropPath("columns.values").setTip("Value column(s)");

  addColumnParameter("name", "Name", "nameColumn").
    setStringColumn().setPropPath("columns.name").setTip("Custom group name");

  addColumnParameter("data", "Data", "dataColumn").
    setPropPath("columns.data").setTip("Alternate data column for bucket values");

  // bucket
  addBoolParameter("bucketed", "Bucketed", "bucketed", true).
   setTip("Bucket grouped values");

  addBoolParameter("autoBucket", "Auto Bucket", "autoBucket", true).
   setTip("Automatically determine bucket ranges from data range and number of buckets");

  addRealParameter("startBucketValue", "Start Value", "startBucketValue", 0.0).
    setRequired().setTip("Start value for manual bucket");
  addRealParameter("deltaBucketValue", "Delta Value", "deltaBucketValue", 1.0).
    setRequired().setTip("Delta value for manual bucket");

  addIntParameter("numAutoBuckets", "Num Auto Buckets", "numAutoBuckets", 10).
    setRequired().setTip("Number of automatic buckets");

  // options
  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", static_cast<int>(Qt::Horizontal)).
    addNameValue("VERTICAL"  , static_cast<int>(Qt::Vertical  )).
    setTip("Bars orientation");

  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("NORMAL"      , static_cast<int>(Plot::PlotType::NORMAL      )).
    addNameValue("STACKED"     , static_cast<int>(Plot::PlotType::STACKED     )).
    addNameValue("SIDE_BY_SIDE", static_cast<int>(Plot::PlotType::SIDE_BY_SIDE)).
    addNameValue("OVERLAY"     , static_cast<int>(Plot::PlotType::OVERLAY     )).
    addNameValue("SCATTER"     , static_cast<int>(Plot::PlotType::SCATTER     )).
    addNameValue("DENSITY"     , static_cast<int>(Plot::PlotType::DENSITY     )).
    setTip("Plot type");

  addEnumParameter("valueType", "Value Type", "valueType").
   addNameValue("COUNT", static_cast<int>(Plot::ValueType::COUNT)).
   addNameValue("RANGE", static_cast<int>(Plot::ValueType::RANGE)).
   addNameValue("MIN"  , static_cast<int>(Plot::ValueType::MIN  )).
   addNameValue("MAX"  , static_cast<int>(Plot::ValueType::MAX  )).
   addNameValue("MEAN" , static_cast<int>(Plot::ValueType::MEAN )).
   addNameValue("SUM"  , static_cast<int>(Plot::ValueType::SUM  )).
   setTip("Bar value type");

  addBoolParameter("percent"  , "Percent"   , "percent"  ).setTip("Show value as percentage");
  addBoolParameter("skipEmpty", "Skip Empty", "skipEmpty").setTip("Skip empty buckets");
  addBoolParameter("sorted"   , "Sorted"    , "sorted"   ).setTip("Sort by count");
  addBoolParameter("dotLines" , "Dot Lines" , "dotLines" ).setTip("Draw bars as lines with dot");
  addBoolParameter("rug"      , "Rug"       , "rug"      ).setTip("Draw rug points");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsDistributionPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Distribution Plot").
    h3("Summary").
     p("Draws bars with heights for the counts of set of values.").
    h3("Columns").
     p("The values to be counted are taken from the " + B("Values") + " columns and "
       "grouped depending on the column value type. By default integer values are "
       "grouped by matching value, real values are automatically bucketed into ranges "
       "and strings are grouped by matching value.").
     p("The automatic bucketing of real values can overridden by turning it off using "
       "the " + B("autoBucket") + " parameter and specifying the " + B("startBucketValue") +
       " and " + B("deltaBucketValue") + " parameters.").
     p("The color of the bar can be overridden using the " + B("Color") + " column.").
    h3("Plot Types").
     p("Normally the plot is draw with a bar per data bucket per group. This can be customized "
       "to stack the grouped values on top of each other or overlaid instead of side by side "
       "or to place the bars side by side by bucket instead of by group.").
     p("The data can also be displayed as a scattered points each representing a fixed size "
       "bucket of points").
     p("The data can also be displayed as a density plot per bucket as long as the values "
       "are numeric").
    h3("Value Type").
     p("Normally the number of values in each bucket is displayed as the height of the bar. "
       "This can be customized to display statistical data for the min, max, mean, range or "
       "sum of values in the bucket. In this case the " + B("Data") + " column can be used "
       "to provide the values calculate the associated statistical value.").
    h3("Options").
     p("Enabling the " + B("Horizontal") + " option draws the bars horizontally "
       "or vertically.").
    h3("Grouping").
     p("Standard grouping can be applied to the values to split the values to be "
       "grouped into individual value sets before final grouping. This second level "
       "of grouping creates multiple sets of grouped values which can be displayed "
       "sequentially or overlaid with common buckets.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/distribution.png"));
}

CQChartsPlot *
CQChartsDistributionPlotType::
create(View *view, const ModelP &model) const
{
  return new Plot(view, model);
}

//------

CQChartsDistributionPlot::
CQChartsDistributionPlot(View *view, const ModelP &model) :
 CQChartsBarPlot(view, view->charts()->plotType("distribution"), model),
 CQChartsObjStatsLineData<CQChartsDistributionPlot>(this),
 CQChartsObjDotPointData <CQChartsDistributionPlot>(this),
 CQChartsObjRugPointData <CQChartsDistributionPlot>(this)
{
}

CQChartsDistributionPlot::
~CQChartsDistributionPlot()
{
  term();
}

//---

void
CQChartsDistributionPlot::
init()
{
  CQChartsBarPlot::init();

  //---

  NoUpdate noUpdate(this);

  setBucketType(CQBucketer::Type::REAL_AUTO);
  setNumAutoBuckets(15);

  setStatsLines(false);
  setStatsLinesDash(LineDash(LineDash::Lengths({2, 2}), 0));

  setDotSymbol         (Symbol::circle());
  setDotSymbolSize     (Length::pixel(7));
  setDotSymbolFilled   (true);
  setDotSymbolFillColor(Color::makePalette());

  setRugSymbolSize       (Length::pixel(5));
  setRugSymbolStroked    (true);
  setRugSymbolStrokeColor(Color::makePalette());

  //---

  // need to update axis labels on placement change
  connect(xAxis(), SIGNAL(tickPlacementChanged()), this, SLOT(updateObjsSlot()));
  connect(yAxis(), SIGNAL(tickPlacementChanged()), this, SLOT(updateObjsSlot()));

  //---

  addColorMapKey();
}

void
CQChartsDistributionPlot::
term()
{
  clearGroupValues();
  clearGroupBuckets();
}

//---

void
CQChartsDistributionPlot::
setValueColumns(const Columns &c)
{
  if (c != valueColumns()) {
    exactValue_ = false;

    bucketer_.setType(CQBucketer::Type::REAL_AUTO);

    CQChartsBarPlot::setValueColumns(c);
  }
}

void
CQChartsDistributionPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDistributionPlot::
setDataColumn(const Column &c)
{
  CQChartsUtil::testAndSet(dataColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsDistributionPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name") c = this->nameColumn();
  else if (name == "data") c = this->dataColumn();
  else                     c = CQChartsBarPlot::getNamedColumn(name);

  return c;
}

void
CQChartsDistributionPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name") this->setNameColumn(c);
  else if (name == "data") this->setDataColumn(c);
  else                     CQChartsBarPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsDistributionPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else                  c = CQChartsBarPlot::getNamedColumns(name);

  return c;
}

void
CQChartsDistributionPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsBarPlot::setNamedColumns(name, c);
}

//---

void
CQChartsDistributionPlot::
setBucketed(bool b)
{
  CQChartsUtil::testAndSet(bucketed_, b, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsDistributionPlot::
isAutoBucket() const
{
  return (bucketType() == CQBucketer::Type::REAL_AUTO);
}

void
CQChartsDistributionPlot::
setAutoBucket(bool b)
{
  setBucketType(b ? CQBucketer::Type::REAL_AUTO : CQBucketer::Type::REAL_RANGE);
}

double
CQChartsDistributionPlot::
startBucketValue() const
{
  return bucketer_.rstart();
}

void
CQChartsDistributionPlot::
setStartBucketValue(double r)
{
  if (r != startBucketValue()) {
    bucketer_.setRStart(r);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

double
CQChartsDistributionPlot::
deltaBucketValue() const
{
  return bucketer_.rdelta();
}

void
CQChartsDistributionPlot::
setDeltaBucketValue(double r)
{
  if (r != deltaBucketValue()) {
    bucketer_.setRDelta(r);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

double
CQChartsDistributionPlot::
minBucketValue() const
{
  return bucketer_.rmin();
}

void
CQChartsDistributionPlot::
setMinBucketValue(double r)
{
  if (r != minBucketValue()) {
    bucketer_.setRMin(r);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

double
CQChartsDistributionPlot::
maxBucketValue() const
{
  return bucketer_.rmax();
}

void
CQChartsDistributionPlot::
setMaxBucketValue(double r)
{
  if (r != maxBucketValue()) {
    bucketer_.setRMax(r);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

void
CQChartsDistributionPlot::
calcMinMaxBucketValue(double &rmin, double &rmax) const
{
  rmin = minBucketValue();
  rmax = maxBucketValue();

  if (underflowBucket().isSet())
    rmin = std::max(underflowBucket().real(), rmin);

  if (overflowBucket().isSet())
    rmax = std::min(overflowBucket().real(), rmax);
}

int
CQChartsDistributionPlot::
numAutoBuckets() const
{
  return bucketer_.numAuto();
}

void
CQChartsDistributionPlot::
setNumAutoBuckets(int n)
{
  if (n != numAutoBuckets()) {
    bucketer_.setNumAuto(n);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

void
CQChartsDistributionPlot::
setExactBucketValue(bool b)
{
  if (b != exactValue_) {
    exactValue_ = b;

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

//---

void
CQChartsDistributionPlot::
setUnderflowBucket(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(underflowBucket_, r, [&]() {
    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDistributionPlot::
setOverflowBucket(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(overflowBucket_, r, [&]() {
    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDistributionPlot::
setBucketStops(const CQChartsReals &r)
{
  CQChartsUtil::testAndSet(bucketStops_, r, [&]() {
    CQBucketer::RStops rstops;

    for (const auto &r : bucketStops_.reals())
      rstops.insert(r);

    bucketer_.setRStops(rstops);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  } );
}

int
CQChartsDistributionPlot::
numUniqueValues() const
{
  return numUnique_;
}

//---

CQBucketer::Type
CQChartsDistributionPlot::
bucketType() const
{
  return bucketer_.type();
}

void
CQChartsDistributionPlot::
setBucketType(const CQBucketer::Type &type)
{
  if (type != bucketType() || exactValue_) {
    exactValue_ = false;

    bucketer_.setType(type);

    updateGroupBucketers(); updateRangeAndObjs(); emit customDataChanged();
  }
}

CQBucketer::Type
CQChartsDistributionPlot::
bucketRealType() const
{
  auto type = bucketer_.type();

  if (type == CQBucketer::Type::STRING)
    type = CQBucketer::Type::REAL_AUTO;

  return type;
}

void
CQChartsDistributionPlot::
updateGroupBucketers()
{
  for (auto &ib : groupData_.groupBucketer) {
    auto &bucketer = ib.second;

    initBucketer(bucketer);
  }
}

void
CQChartsDistributionPlot::
initBucketer(CQBucketer &bucketer)
{
  bucketer.setType(bucketType());

  bucketer.setRStart(startBucketValue());
  bucketer.setRDelta(deltaBucketValue());

  double rmin, rmax;

  calcMinMaxBucketValue(rmin, rmax);

  bucketer.setRMin(rmin);
  bucketer.setRMax(rmax);

  bucketer.setRStops(bucketer_.rstops());

  bucketer.setNumAuto(numAutoBuckets());
}

//---

bool
CQChartsDistributionPlot::
allowZoomX() const
{
  if (isScatter()) return true;

  return CQChartsBarPlot::allowZoomX();
}

bool
CQChartsDistributionPlot::
allowZoomY() const
{
  if (isScatter()) return true;

  return CQChartsBarPlot::allowZoomY();
}

bool
CQChartsDistributionPlot::
allowPanX() const
{
  if (isScatter()) return true;

  return CQChartsBarPlot::allowPanX();
}

bool
CQChartsDistributionPlot::
allowPanY() const
{
  if (isScatter()) return true;

  return CQChartsBarPlot::allowPanY();
}

//---

void
CQChartsDistributionPlot::
addProperties()
{
  addBaseProperties();

  addBoxProperties();

  // columns
  addProp("columns", "nameColumn", "name", "Custom group name column");
  addProp("columns", "dataColumn", "data", "Alternate data column for bucket values");

  // bucketing
  addProp("bucket", "bucketed"        , "enabled", "Bucket grouped values");
  addProp("bucket", "autoBucket"      , "auto"   , "Automatically determine bucket ranges");
  addProp("bucket", "startBucketValue", "start"  , "Start value for manual bucket");
  addProp("bucket", "deltaBucketValue", "delta"  , "Delta value for manual bucket");
  addProp("bucket", "minBucketValue"  , "min"    , "Min value for auto bucket");
  addProp("bucket", "maxBucketValue"  , "max"    , "Max value for auto bucket");
  addProp("bucket", "numAutoBuckets"  , "num"    , "Number of auto buckets");

  addProp("bucket", "underflowBucket", "underflow", "Underflow threshhold for auto bucket");
  addProp("bucket", "overflowBucket" , "overflow" , "Overflow threshhold for auto bucket");

  // options
  addProp("options", "plotType" , "plotType" , "Plot type");
  addProp("options", "valueType", "valueType", "Bar value type");

  addProp("options", "percent"      , "", "Show value as percentage");
  addProp("options", "skipEmpty"    , "", "Skip empty buckets");
  addProp("options", "sorted"       , "", "Sort by count");
  addProp("options", "minBarSize"   , "", "Minimum bar size in pixels", true)->
   setMinValue(0.0);
  addProp("options", "scatterMargin", "", "Scatter bar margin", true)->
   setMinValue(0.0).setMaxValue(1.0);

  addProp("filter", "minValue", "minValue", "Min value");

  // density
  addProp("density", "density"        , "visible" , "Show density plot");
  addProp("density", "densityOffset"  , "offset"  , "Density plot offset");
  addProp("density", "densitySamples" , "samples" , "Density samples");
  addProp("density", "densityGradient", "gradient", "Drag density gradient");
  addProp("density", "densityBars"    , "bars"    , "Draw density bars");

  // scatter
  addProp("scatter", "scatter"      , "visible", "Draw scatter points");
  addProp("scatter", "scatterFactor", "factor" , "Scatter factor (0-1)")->
    setMinValue(0.0).setMaxValue(1.0);

  // stats
  addProp("statsData", "statsLines", "visible", "Statistic lines visible");

  addLineProperties("statsData/stroke", "statsLines", "Statistic lines");

  addProp("statsData", "includeOutlier", "includeOutlier", "Include outlier points");

  // dot lines
  addProp("dotLines"       , "dotLines"    , "visible", "Draw bars as lines with dot");
  addProp("dotLines/stroke", "dotLineWidth", "width"  , "Dot line width");

  addSymbolProperties("dotLines/symbol", "dot", "Dot Line");

  // rug
  addProp("rug", "rug", "visible", "Draw density points on x axis");

  addSymbolProperties("rug/symbol", "rug", "Rug");

  //---

  addGroupingProperties();

  //---

  // color map key
  addColorMapKeyProperties();
}

//---

void
CQChartsDistributionPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDistributionPlot::
setNormal(bool)
{
  setPlotType(PlotType::NORMAL);
}

void
CQChartsDistributionPlot::
setStacked(bool b)
{
  setPlotType(b ? PlotType::STACKED : PlotType::NORMAL);
}

void
CQChartsDistributionPlot::
setSideBySide(bool b)
{
  setPlotType(b ? PlotType::SIDE_BY_SIDE : PlotType::NORMAL);
}

void
CQChartsDistributionPlot::
setOverlaid(bool b)
{
  setPlotType(b ? PlotType::OVERLAY : PlotType::NORMAL);
}

//---

void
CQChartsDistributionPlot::
setPercent(bool b)
{
  CQChartsUtil::testAndSet(percent_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setSkipEmpty(bool b)
{
  CQChartsUtil::testAndSet(skipEmpty_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setValueType(ValueType type)
{
  CQChartsUtil::testAndSet(valueType_, type, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsDistributionPlot::
setValueCount(bool)
{
  setValueType(ValueType::COUNT);
}

void
CQChartsDistributionPlot::
setValueRange(bool b)
{
  setValueType(b ? ValueType::RANGE : ValueType::COUNT);
}

void
CQChartsDistributionPlot::
setValueMin(bool b)
{
  setValueType(b ? ValueType::MIN : ValueType::COUNT);
}

void
CQChartsDistributionPlot::
setValueMax(bool b)
{
  setValueType(b ? ValueType::MAX : ValueType::COUNT);
}

void
CQChartsDistributionPlot::
setValueMean(bool b)
{
  setValueType(b ? ValueType::MEAN : ValueType::COUNT);
}

void
CQChartsDistributionPlot::
setValueSum(bool b)
{
  setValueType(b ? ValueType::SUM : ValueType::COUNT);
}

//---

void
CQChartsDistributionPlot::
setMinValue(const OptReal &r)
{
  CQChartsUtil::testAndSet(minValue_, r, [&]() { updateRangeAndObjs(); } );
}

bool
CQChartsDistributionPlot::
isEmptyValue(double r) const
{
  if (minValue().isSet())
    return r < minValue().real();

  return (r <= 0.0);
}

//---

void
CQChartsDistributionPlot::
setSorted(bool b)
{
  CQChartsUtil::testAndSet(sorted_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setDensity(bool b)
{
  setPlotType(b ? PlotType::DENSITY : PlotType::NORMAL);
}

void
CQChartsDistributionPlot::
setDensityOffset(double o)
{
  CQChartsUtil::testAndSet(densityData_.offset, o, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensitySamples(int n)
{
  CQChartsUtil::testAndSet(densityData_.numSamples, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityGradient(bool b)
{
  CQChartsUtil::testAndSet(densityData_.gradient, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityBars(bool b)
{
  CQChartsUtil::testAndSet(densityData_.bars, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setScatter(bool b)
{
  setPlotType(b ? PlotType::SCATTER : PlotType::NORMAL);
}

void
CQChartsDistributionPlot::
setScatterFactor(double f)
{
  CQChartsUtil::testAndSet(scatterData_.factor, f, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setDotLines(bool b)
{
  CQChartsUtil::testAndSet(dotLineData_.enabled, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDotLineWidth(const Length &l)
{
  CQChartsUtil::testAndSet(dotLineData_.width, l, [&]() { drawObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setIncludeOutlier(bool b)
{
  CQChartsUtil::testAndSet(includeOutlier_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setMinBarSize(double s)
{
  CQChartsUtil::testAndSet(minBarSize_, s, [&]() { drawObjs(); } );
}

void
CQChartsDistributionPlot::
setScatterMargin(double m)
{
  CQChartsUtil::testAndSet(scatterMargin_, m, [&]() { drawObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setRug(bool b)
{
  CQChartsUtil::testAndSet(rug_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setStatsLinesSlot(bool b)
{
  setStatsLines(b);
}

//---

CQChartsGeom::Range
CQChartsDistributionPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsDistributionPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // value columns required
  // name, data, color columns optional

  if (! checkColumns(valueColumns(), "Values", /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(dataColumn (), "Data" )) columnsValid = false;
  if (! checkColumn(colorColumn(), "Color")) columnsValid = false;

  if (! columnsValid)
    return Range();

  //---

  // init grouping
  initGroupData(valueColumns(), nameColumn());

  //---

  clearGroupValues();

  //---

  // process model data (build grouped sets of values)
  class DistributionVisitor : public ModelVisitor {
   public:
    DistributionVisitor(const CQChartsDistributionPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRow(data);

      return State::OK;
    }

   private:
    const CQChartsDistributionPlot *plot_ { nullptr };
  };

  DistributionVisitor distributionVisitor(this);

  visitModel(distributionVisitor);

  //---

  clearGroupBuckets();

  bucketGroupValues();

  auto range = calcBucketRanges();

  //---

  int groupInd = 0;

  auto p = groupData_.groupBucketer.begin();

  if (p != groupData_.groupBucketer.end())
    groupInd = (*p).first;

  const auto &bucketer = th->groupBucketer(groupInd);
  const auto *values   = th->getGroupValues(groupInd);

  // if numeric values then ensure we are using a real bucketer
  bool isNumeric = (values && (values->valueSet->isNumeric() ||
                               (values->valueSet->type() == CQChartsValueSet::Type::TIME)));

  if (isNumeric) {
    auto type = bucketer.type();

    if (type == CQBucketer::Type::STRING)
      type = CQBucketer::Type::REAL_AUTO;

    th->bucketer_.setType(type);

    if (bucketer.type() == CQBucketer::Type::REAL_AUTO) {
      // init preferred real start and delta values
      CInterval interval;

      interval.setStart   (bucketer.rmin());
      interval.setEnd     (bucketer.rmax());
      interval.setNumMajor(numAutoBuckets());

      th->bucketer_.setRStart(interval.calcStart());
      th->bucketer_.setRDelta(interval.calcIncrement());
    }

    // init real range values
    th->bucketer_.setRMin(values->valueSet->rmin());
    th->bucketer_.setRMax(values->valueSet->rmax());
  }
  // if non-numeric values then ensure we are using a string bucketer
  else {
    th->bucketer_.setType(CQBucketer::Type::STRING);
  }

  th->numUnique_ = (values ? values->valueSet->numUnique() : 0);

  th->bucketer_.setType(bucketer.type());

  emit th->customDataChanged();

  //--

  return range;
}

void
CQChartsDistributionPlot::
bucketGroupValues() const
{
  CQPerfTrace trace("CQChartsDistributionPlot::bucketGroupValues");

  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  // initialize bucketers to value range
  if (isConsistentBucketer()) {
    // get consistent bucketer
    auto &bucketer = th->groupBucketer(0);

    //---

    // get consistent type
    auto type = CQChartsValueSet::Type::NONE;

    for (auto &groupValues : groupData_.groupValues) {
      auto *values = groupValues.second;

      auto type1 = values->valueSet->type();

      if (type == CQChartsValueSet::Type::NONE)
        type = type1;

      if (type1 == type)
        continue;

      if      (type1 == CQChartsValueSet::Type::STRING) {
        type = type1;
      }
      else if (type1 == CQChartsValueSet::Type::REAL ||
               type1 == CQChartsValueSet::Type::TIME) {
        if (type == CQChartsValueSet::Type::INTEGER)
          type = type1;
      }
    }

    //---

    // update bucketer with all values
    int iv = 0;

    for (auto &groupValues : groupData_.groupValues) {
      auto *values = groupValues.second;

      if      (type == CQChartsValueSet::Type::INTEGER) {
        bucketer.setType(bucketRealType());

        int imin = values->valueSet->imin();
        int imax = values->valueSet->imax();

        if (underflowBucket().isSet()) imin = std::max(int(underflowBucket().real()), imin);
        if (overflowBucket ().isSet()) imax = std::min(int(overflowBucket ().real()), imax);

        if (iv == 0) {
          bucketer.setIntegral(true);

          bucketer.setIMin(imin);
          bucketer.setIMax(imax);
        }
        else {
          bucketer.setIMin(std::min(bucketer.imin(), imin));
          bucketer.setIMax(std::max(bucketer.imax(), imax));
        }
      }
      else if (type == CQChartsValueSet::Type::REAL ||
               type == CQChartsValueSet::Type::TIME) {
        bucketer.setType(bucketRealType());

        double rmin = values->valueSet->rmin();
        double rmax = values->valueSet->rmax();

        if (underflowBucket().isSet()) rmin = std::max(underflowBucket().real(), rmin);
        if (overflowBucket ().isSet()) rmax = std::min(overflowBucket ().real(), rmax);

        if (iv == 0) {
          bucketer.setIntegral(false);

          bucketer.setRMin(rmin);
          bucketer.setRMax(rmax);
        }
        else {
          bucketer.setRMin(std::min(bucketer.rmin(), rmin));
          bucketer.setRMax(std::max(bucketer.rmax(), rmax));
        }
      }
      else {
        bucketer.setType(CQBucketer::Type::STRING);
      }

      ++iv;
    }
  }
  else {
    // init each group bucketer from value ranges
    for (auto &groupValues : groupData_.groupValues) {
      int   groupInd = groupValues.first;
      auto *values   = groupValues.second;

      // init group bucketer
      auto &bucketer = th->groupBucketer(groupInd);

      //---

      // set bucketer range
      auto type = values->valueSet->type();

      if      (type == CQChartsValueSet::Type::INTEGER) {
        bucketer.setType(bucketRealType());
        bucketer.setIntegral(true);

        int imin = values->valueSet->imin();
        int imax = values->valueSet->imax();

        if (underflowBucket().isSet()) imin = std::max(int(underflowBucket().real()), imin);
        if (overflowBucket ().isSet()) imax = std::min(int(overflowBucket ().real()), imax);

        bucketer.setIMin(imin);
        bucketer.setIMax(imax);
      }
      else if (type == CQChartsValueSet::Type::REAL ||
               type == CQChartsValueSet::Type::TIME) {
        bucketer.setType(bucketRealType());
        bucketer.setIntegral(false);

        double rmin = values->valueSet->rmin();
        double rmax = values->valueSet->rmax();

        if (underflowBucket().isSet()) rmin = std::max(underflowBucket().real(), rmin);
        if (overflowBucket ().isSet()) rmax = std::min(overflowBucket ().real(), rmax);

        bucketer.setRMin(rmin);
        bucketer.setRMax(rmax);
      }
      else {
        bucketer.setType(CQBucketer::Type::STRING);
      }
    }
  }

  //---

  // bucket grouped sets of values
  for (auto &groupValues : groupData_.groupValues) {
    int   groupInd = groupValues.first;
    auto *values   = groupValues.second;

    //---

    // add each index to associated bucket
    for (auto &ind : values->inds) {
      Bucket   bucket;
      QVariant value;

      //---

      bool ok;

      if (isBucketed()) {
        auto type = values->valueSet->type();

        if      (type == CQChartsValueSet::Type::REAL ||
                 type == CQChartsValueSet::Type::TIME) {
          double r = modelReal(ind, ok);
          if (! ok || CMathUtil::isNaN(r)) continue;

          if (! isIncludeOutlier()) {
            bool outlier = values->valueSet->rvals().isOutlier(r);
            if (outlier) continue;
          }

          bucket = calcBucket(groupInd, r);

          if (type == CQChartsValueSet::Type::REAL)
            value = CQChartsVariant::fromReal(r);
          else
            value = modelValue(ind, ok);
        }
        else if (type == CQChartsValueSet::Type::INTEGER) {
          long i = modelInteger(ind, ok);
          if (! ok) continue;

          if (! isIncludeOutlier()) {
            bool outlier = values->valueSet->ivals().isOutlier(int(i));
            if (outlier) continue;
          }

          bucket = calcBucket(groupInd, int(i));
          value  = CQChartsVariant::fromInt(i);
        }
        else {
          bool hierValue = isHierarchical();

          QString str;

          if (hierValue) {
            auto value = modelRootValue(ind, Qt::DisplayRole, ok);

            str = value.toString();
          }
          else {
            str = modelString(ind, ok);
          }

          if (! ok) continue;

          auto &bucketer = th->groupBucketer(groupInd);

          bucket = Bucket(bucketer.stringBucket(str));
          value  = QVariant(str);
        }
      }
      else {
        value = modelValue(ind, ok);
        if (! ok) continue;
      }

      //---

      QVariant dvalue;

      if (dataColumn().isValid()) {
        ModelIndex dind(th, ind.row(), dataColumn(), ind.parent());

        bool ok;

        dvalue = modelValue(dind, ok);
      }

      //---

      VariantInd varInd(value, ind, dvalue);

      values->bucketValues[bucket].inds.push_back(std::move(varInd));
    }
  }
}

CQChartsGeom::Range
CQChartsDistributionPlot::
calcBucketRanges() const
{
  CQPerfTrace trace("CQChartsDistributionPlot::calcBucketRanges");

  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  //---

  bool isStackedActive    = false;
  bool isOverlayActive    = false;
  bool isSideBySideActive = false;

  if (hasGroups()) {
    isStackedActive    = isStacked();
    isOverlayActive    = isOverlaid();
    isSideBySideActive = isSideBySide();
  }

  //---

  // calc range (number of bars and max height)

  int i2 = 0;

  IMinMax nRange(0);
  IMinMax bucketRange;
  RMinMax valueRange;
  BBox    densityBBox;

  th->groupData_.groupBucketRange.clear();

  double doffset = 0.0;

  int ig = 0;
  int ng = int(groupData_.groupValues.size());

  int maxValues = 0;

  using BucketTotals = std::map<Bucket, double>;

  BucketTotals valueSetRunningTotal;

  for (auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    int maxValues1 = 0;

    //---

    int   groupInd = groupValues.first;
    auto *values   = groupValues.second;

    // density curve per group (optionally offset)
    if      (isDensity()) {
      values->densityData->setNumSamples(densitySamples());

      std::vector<double> xvals;

      (void) getRealValues(groupInd, xvals, values->statData);

      values->densityData->setXVals(xvals);

      if (isVertical()) {
        densityBBox.add(values->densityData->xmin1(), values->densityData->ymin1() + doffset);
        densityBBox.add(values->densityData->xmax1(), values->densityData->ymax1() + doffset);
      }
      else {
        densityBBox.add(values->densityData->ymin1() + doffset, values->densityData->xmin1());
        densityBBox.add(values->densityData->ymax1() + doffset, values->densityData->xmax1());
      }

      values->xValueRange.add(values->densityData->xmin1());
      values->xValueRange.add(values->densityData->xmax1());

      values->yValueRange.add(values->densityData->ymin1() + doffset);
      values->yValueRange.add(values->densityData->ymax1() + doffset);

      doffset += densityOffset();
    }
    // scatter is grid of num groups by num value sets
    else if (isScatter()) {
      int nv = int(values->bucketValues.size());

      maxValues1 = std::max(maxValues1, nv);

      values->xValueRange.add(groupInd);
      values->yValueRange.add(nv);
    }
    // normal is bar per value set
    else {
      maxValues1 = std::max(maxValues1, int(values->bucketValues.size()));

      (void) getStatData(groupInd, values->statData);

      bool underFlow = false;
      bool overFlow  = false;

      for (auto &bucketValues : values->bucketValues) {
        const auto &bucket   = bucketValues.first;
        auto       &varsData = bucketValues.second;

        //---

        int n = int(varsData.inds.size());

        if (isSkipEmpty()) {
          if (isEmptyValue(n))
            continue;
        }

        //---

        // get bucket value range
        double value1, value2;

        this->bucketValues(groupInd, bucket, value1, value2);

        if (! bucket.isUnderflow() && ! bucket.isOverflow()) {
          values->xValueRange.add(value1);
          values->xValueRange.add(value2);
        }

        //---

        // update bucket range
        if      (bucket.hasValue()) {
          int bucket1 = bucket.value();

          bucketRange.add(bucket1);

          th->groupData_.groupBucketRange[groupInd].add(bucket1);
        }
        else if (bucket.isUnderflow())
          underFlow = true;
        else if (bucket.isOverflow())
          overFlow = true;

        //---

        // update max n per value set
        nRange.add(n);

        //---

        auto addRangeValue = [&](double y) {
          values->yValueRange.add(y);

          varsData.valueRange.add(y);

          valueRange.add(y);
        };

        // update min/max per value set
        double value = 0;

        if      (isValueCount()) {
          addRangeValue(value = n);
        }
        else if (isValueRange()) {
          calcVarIndsData(varsData);

          addRangeValue(varsData.min);
          addRangeValue(varsData.max);

          value = varsData.max - varsData.min;
        }
        else if (isValueMin()) {
          calcVarIndsData(varsData);

          addRangeValue(value = varsData.min);
        }
        else if (isValueMax()) {
          calcVarIndsData(varsData);

          addRangeValue(value = varsData.max);
        }
        else if (isValueMean()) {
          calcVarIndsData(varsData);

          addRangeValue(value = varsData.statData.mean);
        }
        else if (isValueSum()) {
          calcVarIndsData(varsData);

          addRangeValue(value = varsData.statData.sum);
        }

        //---

        ++i2;

        //---

        valueSetRunningTotal[bucket] += value;
      }

      //---

      if (underFlow) {
        int bucket1 = bucketRange.min(0) - 1;

        bucketRange.add(bucket1);

        th->groupData_.groupBucketRange[groupInd].add(bucket1);
      }

      if (overFlow) {
        int bucket1 = bucketRange.max(0) + 1;

        bucketRange.add(bucket1);

        th->groupData_.groupBucketRange[groupInd].add(bucket1);
      }
    }

    //---

    maxValues = std::max(maxValues, maxValues1);

    //---

    ++ig;
  }

  //---

  // set range
  Range dataRange;

  auto mapValue = [&](double y) {
    return (isLogY() ? logValue(y) : y);
  };

  auto updateRange = [&](double x, double y) {
    if (isVertical())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  auto updateRange2 = [&](double x1, double y1, double x2, double y2) {
    updateRange(x1, y1);
    updateRange(x2, y2);
  };

  //---

  if      (isDensity()) {
    // range already accounts for horizontal/vertical
    if (densityBBox.isSet()) {
      dataRange.updateRange(densityBBox.getXMin(), densityBBox.getYMin());
      dataRange.updateRange(densityBBox.getXMax(), densityBBox.getYMax());
    }
  }
  else if (isScatter()) {
    if (maxValues > 0) {
      updateRange2(-0.5, -0.5, ng - 0.5, maxValues - 0.5);
    }
  }
  else {
    // calc value range (bar height)
    double n1 = 0.0, n2 = 0.0;

    if      (isValueCount()) { n1 =                 0; n2 = std::max(nRange.max(), 1); }
    else if (isValueRange()) { n1 = valueRange.min(0); n2 = valueRange.max(0); }
    else if (isValueMin  ()) { n1 =                 0; n2 = valueRange.max(0); }
    else if (isValueMax  ()) { n1 =                 0; n2 = valueRange.max(0); }
    else if (isValueMean ()) { n1 =                 0; n2 = valueRange.max(0); }
    else if (isValueSum  ()) { n1 =                 0; n2 = valueRange.max(0); }

    if      (isStackedActive) {
      double maxValue = 0;

      if (isPercent()) {
        maxValue = 1;
      }
      else {
        // calc max value of all stacked boxes
        maxValue = 0;

        for (const auto &gt : valueSetRunningTotal)
          maxValue = std::max(maxValue, gt.second);
      }

      //---

      if (! isSkipEmpty()) {
        int bucket1 = bucketRange.min(0);
        int bucket2 = bucketRange.max(0);

        updateRange2(bucket1 - 1.0, 0, bucket2 + 1.0, maxValue);
      }
      else {
        int nv = int(valueSetRunningTotal.size());

        updateRange2(-0.5, 0, nv - 0.5, maxValue);
      }
    }
    else if (isOverlayActive) {
      if (! isSkipEmpty()) {
        int bucket1 = bucketRange.min(0);
        int bucket2 = bucketRange.max(0);

        if (isPercent())
          updateRange2(bucket1 - 1.0, 0, bucket2 + 1.0, 1);
        else
          updateRange2(bucket1 - 1.0, n1, bucket2 + 1.0, n2);
      }
      else {
        int nv = int(valueSetRunningTotal.size());

        if (isPercent())
          updateRange2(-0.5, 0, nv - 0.5, 1);
        else
          updateRange2(-0.5, n1, nv - 0.5, n2);
      }
    }
    else if (isSideBySideActive) {
      if (! isSkipEmpty()) {
        int bucket1 = bucketRange.min(0);
        int bucket2 = bucketRange.max(0);

        if (isPercent())
          updateRange2(bucket1 - 1.0, 0, bucket2 + 1.0, 1);
        else
          updateRange2(bucket1 - 1.0, n1, bucket2 + 1.0, n2);
      }
      else {
        int nv = int(valueSetRunningTotal.size());

        if (isPercent())
          updateRange2(-0.5, 0, nv - 0.5, 1);
        else
          updateRange2(-0.5, n1, nv - 0.5, n2);
      }
    }
    else {
      if (! isSkipEmpty()) {
        int nb = 0;

        for (auto &bucketRange : groupData_.groupBucketRange)
          nb += bucketRange.second.max(0) - bucketRange.second.min(0) + 1;

        if (isPercent())
          updateRange2(-1.0, 0, nb, 1);
        else
          updateRange2(-1.0, (n1 > 0 ? mapValue(n1) : 0.0),
                         nb, (n2 > 0 ? mapValue(n2) : 0.0));
      }
      else {
        if (isPercent())
          updateRange2(-1.0, 0, i2, 1);
        else {
          if (! isEmptyValue(n2))
            updateRange2(-1.0, 0.0, i2, (n2 > 0 ? mapValue(n2) : 0.0));
          else
            updateRange2(-1.0, 0.0, i2, 0.0);
        }
      }
    }
  }

  dataRange.makeNonZero();

  //---

  return dataRange;
}

void
CQChartsDistributionPlot::
clearGroupValues() const
{
  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  for (auto &groupValues : th->groupData_.groupValues)
    delete groupValues.second;

  th->groupData_.groupValues.clear();
}

void
CQChartsDistributionPlot::
clearGroupBuckets() const
{
  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  th->groupData_.groupBucketer.clear();

  for (auto &groupValues : groupData_.groupValues) {
    auto *values = groupValues.second;

    values->bucketValues.clear();
  }
}

void
CQChartsDistributionPlot::
addRow(const ModelVisitor::VisitData &data) const
{
  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  for (const auto &valueColumn : valueColumns())
    addRowColumn(ModelIndex(th, data.row, valueColumn, data.parent));
}

void
CQChartsDistributionPlot::
addRowColumn(const ModelIndex &ind) const
{
  // get optional group for value
  int groupInd = rowGroupInd(ind);

  bool ok;

  auto value = modelValue(ind, ok);

  //---

  // check push/pop filter
  if (ok && ! checkFilter(groupInd, value))
    return;

  //---

  // get value set for group
  auto *values = getGroupIndValues(groupInd, ind);

  //---

  // add value
  if (ok)
    values->valueSet->addValue(value);

  values->inds.push_back(ind);
}

CQChartsDistributionPlot::Values *
CQChartsDistributionPlot::
getGroupIndValues(int groupInd, const ModelIndex &ind) const
{
  auto *values = const_cast<Values *>(getGroupValues(groupInd));

  if (values)
    return values;

  //---

  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  auto *valueSet = new CQChartsValueSet(this);

  valueSet->setColumn(ind.column());

  auto pg = th->groupData_.groupValues.insert(th->groupData_.groupValues.end(),
              GroupValues::value_type(groupInd, new Values(valueSet)));

  values = (*pg).second;

  //---

  return values;
}

const CQChartsDistributionPlot::Values *
CQChartsDistributionPlot::
getGroupValues(int groupInd) const
{
  auto pg = groupData_.groupValues.find(groupInd);
  if (pg == groupData_.groupValues.end()) return nullptr;

  return (*pg).second;
}

//------

CQChartsGeom::BBox
CQChartsDistributionPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsDistributionPlot::calcExtraFitBBox");

  BBox bbox;

  // add data labels
  auto position = dataLabel()->position();

  if (position == CQChartsDataLabel::Position::TOP_OUTSIDE ||
      position == CQChartsDataLabel::Position::BOTTOM_OUTSIDE) {
    for (const auto &plotObj : plotObjs_) {
      auto *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      if (barObj)
        bbox += barObj->dataLabelRect();
    }
  }

  // add rug symbols
  if (isRug()) {
    double sx, sy;

    plotSymbolSize(rugSymbolSize(), sx, sy, /*scale*/false);

    const auto &dataRange = this->dataRange();

    Point p1, p2;

    if (isVertical()) {
      p1 = Point(dataRange.xmin(), dataRange.ymin()       );
      p2 = Point(dataRange.xmax(), dataRange.ymin() - 2*sy);
    }
    else {
      p1 = Point(dataRange.xmin()       , dataRange.ymin());
      p2 = Point(dataRange.xmin() - 2*sx, dataRange.ymax());
    }

    bbox += p1;
    bbox += p2;
  }

  return bbox;
}

//------

bool
CQChartsDistributionPlot::
checkFilter(int groupInd, const QVariant &value) const
{
  if (filterStack_.empty())
    return true;

  bool ok;

  double r = CQChartsVariant::toReal(value, ok);

  if (! ok)
    return true;

  const auto &filters = filterStack_.back();

  for (const auto &filter : filters) {
    if (groupInd == filter.groupInd && r >= filter.minValue && r < filter.maxValue)
      return true;
  }

  return false;
}

CQChartsDistributionPlot::Bucket
CQChartsDistributionPlot::
calcBucket(int groupInd, double value) const
{
  bool exactValue = isExactBucketValue();

  if (! exactValue && bucketType() == CQBucketer::Type::REAL_AUTO) {
    if (underflowBucket().isSet() && value < underflowBucket().real())
      return Bucket(Bucket::Type::UNDERFLOW);

    if (overflowBucket().isSet() && value > overflowBucket().real())
      return Bucket(Bucket::Type::OVERFLOW);
  }

  //---

  int num = -1;

  const auto &bucketer = groupBucketer (groupInd);
  const auto *values   = getGroupValues(groupInd);

  if (filterStack_.empty()) {
    if (! isBucketed())
      return Bucket(-1);

    if      (exactValue) {
      if (values) {
        if (values->valueSet->type() == CQBaseModelType::INTEGER)
          num = values->valueSet->iid(int(value));
        else
          num = values->valueSet->rid(value);
      }
    }
    else if (bucketType() == CQBucketer::Type::REAL_AUTO)
      num = bucketer.autoRealBucket(value);
    else if (bucketType() == CQBucketer::Type::REAL_RANGE)
      num = bucketer.realBucket(value);
    else if (bucketType() == CQBucketer::Type::FIXED_STOPS)
      num = bucketer.stopsRealBucket(value);
  }
  else {
    num = bucketer.autoRealBucket(value);
  }

  if (num == INT_MIN)
    return Bucket(-1);

  return Bucket(num);
}

//------

bool
CQChartsDistributionPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsDistributionPlot::createObjs");

  auto *th = const_cast<CQChartsDistributionPlot *>(this);

  NoUpdate noUpdate(this);

  //---

  bool isStackedActive    = false;
  bool isOverlayActive    = false;
  bool isSideBySideActive = false;

  if (hasGroups()) {
    isStackedActive    = isStacked();
    isOverlayActive    = isOverlaid();
    isSideBySideActive = isSideBySide();
  }

  //---

#if 0
  auto intIncrementForSize = [&](double size) {
    CInterval interval;

    interval.setStart   (0);
    interval.setEnd     (size);
    interval.setIntegral(true);

    return std::max(CMathRound::RoundNearest(interval.calcIncrement()), 1);
  };
#endif

  //---

  // init color value set
//initValueSets();

  //---

  valueAxis()->clearTickLabels();
  countAxis()->clearTickLabels();

#if 0
  const auto &dataRange = this->dataRange();

  double size = dataRange.size(isVertical());

  int inc = intIncrementForSize(size);
#endif

  if      (isDensity()) {
    valueAxis()->setValueType       (CQChartsAxisValueType(CQChartsAxisValueType::Type::REAL),
                                     /*notify*/false);
    valueAxis()->setGridMid         (false);
    valueAxis()->setMajorIncrement  (0);
    valueAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR_AND_MINOR);
    valueAxis()->setRequireTickLabel(false);

    countAxis()->setValueType       (CQChartsAxisValueType(CQChartsAxisValueType::Type::REAL),
                                     /*notify*/false);
    countAxis()->setGridMid         (false);
    countAxis()->setMajorIncrement  (0);
    countAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR_AND_MINOR);
    countAxis()->setRequireTickLabel(false);
  }
  else if (isScatter()) {
    valueAxis()->setValueType       (CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER),
                                     /*notify*/false);
    valueAxis()->setGridMid         (true);
    valueAxis()->setMajorIncrement  (1);
    valueAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    valueAxis()->setRequireTickLabel(false);

    countAxis()->setValueType       (CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER),
                                     /*notify*/false);
    countAxis()->setGridMid         (true);
    countAxis()->setMajorIncrement  (1);
    countAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    countAxis()->setRequireTickLabel(false);
  }
  else {
    valueAxis()->setValueType       (CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER),
                                     /*notify*/false);
    valueAxis()->setGridMid         (true);
    valueAxis()->setMajorIncrement  (1);
    valueAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    valueAxis()->setRequireTickLabel(true);

    if (! CQChartsPlot::isLogY()) {
      if (isValueCount())
        countAxis()->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER),
                                  /*notify*/false);
      else
        countAxis()->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::REAL),
                                  /*notify*/false);
    }
    else
      countAxis()->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::LOG),
                                /*notify*/false);

    countAxis()->setGridMid         (false);
    countAxis()->setMajorIncrement  (0);
    countAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    countAxis()->setRequireTickLabel(false);
  }

  //---

  int gpos = 0;
  int vpos = 0;

  int ng = int(groupData_.groupValues.size());

  int offset = 0;
  int count  = 0;

  double doffset = 0.0;

  using GroupTotals  = std::map<int, double>;
  using BucketTotals = std::map<Bucket, double>;

  GroupTotals  groupMax;
  BucketTotals valueSetRunningTotal, valueSetTotals;

  th->barWidth_ = 1.0;

  if (isSideBySideActive) {
    assert(ng > 0);

    th->barWidth_ /= ng;
  }

  //---

  // get sorted buckets if needed
  using Buckets           = std::vector<Bucket>;
  using CountBuckets      = std::map<int, Buckets>;
  using GroupCountBuckets = std::map<int, CountBuckets>;
  using GroupBuckets      = std::map<int, Buckets>;

  GroupCountBuckets groupCountBuckets;
  GroupBuckets      groupSortedBuckets;

  if (isSorted()) {
    if (isStackedActive || isOverlayActive || isSideBySideActive) {
      // TODO
    }
    else {
      for (auto &groupValues : groupData_.groupValues) {
        int         groupInd = groupValues.first;
        const auto *values   = groupValues.second;

        for (const auto &bucketValues : values->bucketValues) {
          const auto &bucket   = bucketValues.first;
          const auto &varsData = bucketValues.second;

          auto barValue = varIndsValue(varsData);

          int n = int(barValue.n2);

          groupCountBuckets[groupInd][n].push_back(bucket);
        }
      }

      for (const auto &gcb : groupCountBuckets) {
        int         groupInd     = gcb.first;
        const auto &countBuckets = gcb.second;

        for (const auto &cb : countBuckets) {
          const auto &buckets = cb.second;

          for (auto &bucket : buckets)
            groupSortedBuckets[groupInd].push_back(bucket);
        }
      }
    }
  }

  //---

  // assign index to each non-empty bucket
  using BucketInd = std::map<Bucket, int>;

  BucketInd bucketInd;

  for (auto &groupValues : groupData_.groupValues) {
    const auto *values = groupValues.second;

#if 0
    for (const auto &bucketValues : values->bucketValues) {
      const auto &bucket = bucketValues.first;

      bucketInd[bucket] = 0;
    }
#endif

    for (const auto &bucketValues : values->bucketValues) {
      const auto &bucket   = bucketValues.first;
      const auto &varsData = bucketValues.second;

      int n = int(varsData.inds.size());

      if (isSkipEmpty()) {
        if (isEmptyValue(n))
          continue;
      }

      bucketInd[bucket] = 0;
    }
  }

  int i = 0;

  for (auto &p : bucketInd)
    p.second = i++;

  //---

  auto getSortedBuckets = [&](int groupInd) {
    if (isStackedActive || isOverlayActive || isSideBySideActive) {
      auto p = groupSortedBuckets.begin();
      assert(p != groupSortedBuckets.end());

      return (*p).second;
    }
    else {
      auto p = groupSortedBuckets.find(groupInd);
      assert(p != groupSortedBuckets.end());

      return (*p).second;
    }
  };

  //---

  if (isPercent()) {
    int ig = 0;

    for (auto &groupValues : groupData_.groupValues) {
      if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

      //---

      int         groupInd = groupValues.first;
      const auto *values   = groupValues.second;

      //---

      if (groupData_.groupBucketRange.empty())
        continue;

      //---

      int iv = 0;

      for (auto &bucketValues : values->bucketValues) {
        if (! colorColumn().isValid()) {
          if (ng == 1 && isSetHidden(iv)) { ++iv; continue; }
        }

        //---

        const auto &bucket   = bucketValues.first;
        const auto &varsData = bucketValues.second;

        //---

        const auto *pVarsData = &varsData;

        auto barValue = varIndsValue(*pVarsData);

        valueSetTotals[bucket] += barValue.n2 - barValue.n1;

        if (barValue.n2 > groupMax[groupInd])
          groupMax[groupInd] = barValue.n2;

        ++iv;
      }

      ++ig;
    }
  }

  //---

  auto makeBBox = [&](double xmin, double ymin, double xmax, double ymax) {
    if (isVertical())
      return BBox(xmin, ymin, xmax, ymax);
    else
      return BBox(ymin, xmin, ymax, xmax);
  };

  //---

  int ig = 0;

  for (auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int         groupInd = groupValues.first;
    const auto *values   = groupValues.second;

    //---

    // density curve per group (optionally offset)
    if      (isDensity()) {
      CQChartsDistributionDensityObj::Data data;

      data.points   = values->densityData->opoints();
      data.xmin     = values->densityData->xmin1();
      data.xmax     = values->densityData->xmax1();
      data.ymin     = values->densityData->ymin1();
      data.ymax     = values->densityData->ymax1();
      data.statData = values->statData;

      //---

      data.buckets.clear();

      for (auto &bucketValues : values->bucketValues) {
        const auto &bucket   = bucketValues.first;
        const auto &varsData = bucketValues.second;

        int n = int(varsData.inds.size());

        data.buckets.emplace_back(bucket, n);
      }

      //---

      BBox bbox;

      if (isVertical()) {
        bbox.add(data.xmin, data.ymin + doffset);
        bbox.add(data.xmax, data.ymax + doffset);
      }
      else {
        bbox.add(data.ymin + doffset, data.xmin);
        bbox.add(data.ymax + doffset, data.xmin);
      }

      if (bbox.isSet()) {
        auto *barObj = createDensityObj(bbox, groupInd, data, doffset, ColorInd(ig, ng));

        connect(barObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

        objs.push_back(barObj);
      }

      doffset += densityOffset();
    }
    else if (isScatter()) {
      int iv = 0;
      int nv = int(values->bucketValues.size());

      for (auto &bucketValues : values->bucketValues) {
        const auto &bucket   = bucketValues.first;
        const auto &varsData = bucketValues.second;

        //---

        const auto *pVarsData = &varsData;

        auto sbucket = bucket;

        if (isSorted()) {
          if (isStackedActive || isOverlayActive || isSideBySideActive) {
            // TODO
          }
          else {
            const auto &sortedBuckets = getSortedBuckets(groupInd);

            sbucket = sortedBuckets[size_t(iv)];

            auto p = values->bucketValues.find(sbucket);
            assert(p != values->bucketValues.end());

            const auto &varsData1 = (*p).second;

            pVarsData = &varsData1;
          }
        }

        //---

        int n = int(pVarsData->inds.size());

        auto bbox = makeBBox(ig - 0.5, iv - 0.5, ig + 0.5, iv + 0.5);

        auto *scatterObj = createScatterObj(bbox, groupInd, sbucket, n,
                                            ColorInd(ig, ng), ColorInd(iv, nv));

        connect(scatterObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

        objs.push_back(scatterObj);

        auto bucketName = bucketStr(groupInd, bucket);

        countAxis()->setTickLabel(iv, bucketName);

        ++iv;
      }

      auto groupName = groupIndName(groupInd);

      valueAxis()->setTickLabel(ig, groupName);
    }
    else {
      auto pb = groupData_.groupBucketRange.find(groupInd);

      if (groupData_.groupBucketRange.empty())
        continue;

      const auto &bucketRange = (*pb).second;

      int bucketMin = bucketRange.min(0);
      int bucketMax = bucketRange.max(0);

      if (! isOverlayActive)
        offset = -bucketMin;

      //---

      bool isNumeric = (values->valueSet->isNumeric() ||
                        (values->valueSet->type() == CQChartsValueSet::Type::TIME));

      //---

      int iv = 0;
      int nv = int(values->bucketValues.size());

      for (auto &bucketValues : values->bucketValues) {
        if (! colorColumn().isValid()) {
          if (ng == 1 && isSetHidden(iv)) { ++iv; continue; }
        }

        //---

        const auto &bucket   = bucketValues.first;
        const auto &varsData = bucketValues.second;

        //---

        double value1, value2;

        this->bucketValues(groupInd, bucket, value1, value2);

        //---

        const auto *pVarsData = &varsData;

        auto sbucket = bucket;

        if (isSorted()) {
          if (isStackedActive || isOverlayActive || isSideBySideActive) {
            // TODO
          }
          else {
            const auto &sortedBuckets = getSortedBuckets(groupInd);

            sbucket = sortedBuckets[size_t(iv)];

            auto p = values->bucketValues.find(sbucket);
            assert(p != values->bucketValues.end());

            const auto &varsData1 = (*p).second;

            pVarsData = &varsData1;
          }
        }

        //---

        int bucketValue = -1;

        if (bucket.hasValue())
          bucketValue = bucket.value();
        else
          bucketValue = bucket.outsideValue(bucketMin, bucketMax);

        int bucket1 = bucketValue + offset + count;

        //if (! isValueCount() && ! isNumeric)
        //  continue;

        auto barValue = varIndsValue(*pVarsData);

        if (minValue().isSet()) {
          if (barValue.n2 < minValue().value())
            continue;
        }

        //---

        double scale = 1.0;

        if (isPercent()) {
          if (isStackedActive || isOverlayActive || isSideBySideActive) {
            double valueSetTotal = valueSetTotals[bucket];

            if (valueSetTotal > 0)
              scale = 1.0/valueSetTotal;
          }
          else {
            double max = groupMax[groupInd];

            if (max > 0.0)
              scale = 1.0/max;
          }
        }

        //---

        int vpos1 = 0;

        if (isStackedActive || isOverlayActive || isSideBySideActive) {
          if (isSkipEmpty()) {
            int stackInd = bucketInd[bucket];

            vpos1 = stackInd;
          }
        }

        //---

        auto mapValue = [&](double y) {
          return (isLogY() ? logValue(y) : y);
        };

        BBox bbox;
        bool isLine = false;

        if      (isStackedActive) {
          double total = valueSetRunningTotal[bucket];

          double v1 = (barValue.n1 + total)*scale;
          double v2 = (barValue.n2 + total)*scale;

          isLine = (v1 == v2);

          if (! isSkipEmpty())
            bbox = makeBBox(bucketValue - 0.5, v1, bucketValue + 0.5, v2);
          else
            bbox = makeBBox(vpos1 - 0.5, v1, vpos1 + 0.5, v2);
        }
        else if (isOverlayActive) {
          double v1 = barValue.n1*scale;
          double v2 = barValue.n2*scale;

          isLine = (v1 == v2);

          if (! isSkipEmpty())
            bbox = makeBBox(bucketValue - 0.5, v1, bucketValue + 0.5, v2);
          else
            bbox = makeBBox(vpos1 - 0.5, v1, vpos1 + 0.5, v2);
        }
        else if (isSideBySideActive) {
          double tpos1;

          if (! isSkipEmpty())
            tpos1 = bucketValue - 0.5 + barWidth_*gpos;
          else
            tpos1 = vpos1 - 0.5 + barWidth_*gpos;

          double tpos2 = tpos1 + barWidth_;

          double v1 = barValue.n1*scale;
          double v2 = barValue.n2*scale;

          isLine = (v1 == v2);

          bbox = makeBBox(tpos1, v1, tpos2, v2);
        }
        else {
          double v1, v2;

          if (! isPercent()) {
            v1 = (barValue.n1 > 0 ? mapValue(barValue.n1) : 0.0);
            v2 = (barValue.n2 > 0 ? mapValue(barValue.n2) : 0.0);
          }
          else {
            v1 = barValue.n1*scale;
            v2 = barValue.n2*scale;
          }

          isLine = (v1 == v2);

          if (! isSkipEmpty())
            bbox = makeBBox(bucket1 - 0.5, v1, bucket1 + 0.5, v2);
          else
            bbox = makeBBox(vpos - 0.5, v1, vpos + 0.5, v2);
        }

        barValue.xr = RangeValue(value1     , values->xValueRange.min(), values->xValueRange.max());
        barValue.yr = RangeValue(barValue.n2, values->yValueRange.min(), values->yValueRange.max());

        if (bbox.isValid()) {
          auto *barObj = createBarObj(bbox, groupInd, sbucket, barValue,
                                      isLine, ColorInd(ig, ng), ColorInd(iv, nv));

          connect(barObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

          objs.push_back(barObj);
        }

        //---

        if      (! isBucketed()) {
          auto groupName = groupIndName(groupInd);

          valueAxis()->setTickLabel(bucket1, groupName);
        }
        else if (isNumeric) {
          int tpos = (isVertical() ? CMathRound::RoundNearest(bbox.getXMid()) :
                                     CMathRound::RoundNearest(bbox.getYMid()));

          if (valueAxis()->tickLabelPlacement().type() ==
              CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
            auto bucketStr = bucketValuesStr(groupInd, sbucket, values);

            if      (isStackedActive)
              valueAxis()->setTickLabel(tpos, bucketStr);
            else if (isOverlayActive)
              valueAxis()->setTickLabel(tpos, bucketStr);
            else if (isSideBySideActive)
              valueAxis()->setTickLabel(tpos, bucketStr);
            else
              valueAxis()->setTickLabel(tpos, bucketStr);
          }
          else {
            auto bucketStr1 = bucketValuesStr(groupInd, sbucket, values, BucketValueType::START);
            auto bucketStr2 = bucketValuesStr(groupInd, sbucket, values, BucketValueType::END  );

            if      (isStackedActive) {
              valueAxis()->setTickLabel(tpos    , bucketStr1);
              valueAxis()->setTickLabel(tpos + 1, bucketStr2);
            }
            else if (isOverlayActive) {
              valueAxis()->setTickLabel(tpos    , bucketStr1);
              valueAxis()->setTickLabel(tpos + 1, bucketStr2);
            }
            else if (isSideBySideActive) {
              valueAxis()->setTickLabel(tpos    , bucketStr1);
              valueAxis()->setTickLabel(tpos + 1, bucketStr2);
            }
            else {
              valueAxis()->setTickLabel(tpos    , bucketStr1);
              valueAxis()->setTickLabel(tpos + 1, bucketStr2);
            }
          }
        }
        else {
          auto bucketStr = bucketValuesStr(groupInd, sbucket, values, BucketValueType::START);

          int tpos = 0;

          if (isVertical())
            tpos = CMathRound::RoundNearest(bbox.getXMid());
          else
            tpos = CMathRound::RoundNearest(bbox.getYMid());

          if      (isStackedActive)
            valueAxis()->setTickLabel(tpos, bucketStr);
          else if (isOverlayActive)
            valueAxis()->setTickLabel(tpos, bucketStr);
          else if (isSideBySideActive)
            valueAxis()->setTickLabel(tpos, bucketStr);
          else
            valueAxis()->setTickLabel(tpos, bucketStr);
        }

        //---

        valueSetRunningTotal[bucket] += barValue.n2 - barValue.n1;

        ++iv;

        ++vpos;
      }

      //---

      if (! isOverlayActive)
        count += bucketRange.max(0) - bucketRange.min(0) + 1;
    }

    ++ig;

    ++gpos;
  }

  //---

  auto setXLabel = [&]() {
    valueAxis()->setDefLabel(valueColumnName(""));
  };

  auto setXGroupLabel = [&]() {
    valueAxis()->setDefLabel(valueColumnName(""));
  };

  auto setXGroupValuesLabel = [&]() {
    QStringList groupLabels;

    for (const auto &groupValues : groupData_.groupValues) {
      int groupInd = groupValues.first;

      auto groupName = groupIndName(groupInd);

      groupLabels.push_back(groupName);
    }

    valueAxis()->setDefLabel(groupLabels.join(", "));
  };

  // value axis label (x)
  valueAxis()->setDefLabel("");

  if (isBucketed()) {
    if (groupData_.groupValues.size() > 1) {
      if (! isScatter())
        setXGroupValuesLabel();
      else
        setXGroupLabel();
    }
    else {
      setXLabel();
    }
  }
  else {
    setXLabel();
  }

  // count axis label (y)
  if      (isDensity()) {
    countAxis()->setDefLabel("Density");
  }
  else {
    auto setCountLabel = [&](const QString &label) {
      auto label1 = label;

      if (dataColumn().isValid()) {
        bool ok;

        auto header = modelHHeaderString(dataColumn(), ok);

        if (ok && header.length())
          label1 += " (" + header + ")";
      }

      countAxis()->setDefLabel(label1);
    };

    if      (isPercent   ()) setCountLabel("Percent");
    else if (isValueCount()) setCountLabel("Count");
    else if (isValueRange()) setCountLabel("Range");
    else if (isValueMin  ()) setCountLabel("Min");
    else if (isValueMax  ()) setCountLabel("Max");
    else if (isValueMean ()) setCountLabel("Mean");
    else if (isValueSum  ()) setCountLabel("Sum");
  }

  if (yLabel().length())
    countAxis()->setDefLabel(yLabel());

  //---

  return true;
}

QString
CQChartsDistributionPlot::
valueColumnName(const QString &def) const
{
  if (xLabel().length())
    return xLabel();

  auto c = valueColumns().column();

  bool ok;

  auto name = modelHHeaderString(c, ok);

  if (! ok)
    name = def;

  return name;
}

void
CQChartsDistributionPlot::
calcVarIndsData(VariantIndsData &varInds) const
{
  RMinMax valueRange;

  CQChartsRValues rvals;

  int n = int(varInds.inds.size());

  for (int i = 0; i < n; ++i) {
    const auto &var = varInds.inds[size_t(i)];

    double r  = 0.0;
    bool   ok = false;

    if (var.dvar.isValid())
      r = CQChartsVariant::toReal(var.dvar, ok);

    if (! ok)
      r = CQChartsVariant::toReal(var.var, ok);

    if (! ok)
      continue;

    rvals.addValue(r);

    valueRange.add(r);
  }

  varInds.min      = valueRange.min(0);
  varInds.max      = valueRange.max(0);
  varInds.statData = rvals.statData();
}

CQChartsDistributionPlot::BarValue
CQChartsDistributionPlot::
varIndsValue(const VariantIndsData &varInds) const
{
  BarValue barValue;

  if      (isValueCount()) { barValue.n1 = 0          ; barValue.n2 = int(varInds.inds.size()); }
  else if (isValueRange()) { barValue.n1 = varInds.min; barValue.n2 = varInds.max             ; }
  else if (isValueMin  ()) { barValue.n1 = 0          ; barValue.n2 = varInds.min             ; }
  else if (isValueMax  ()) { barValue.n1 = 0          ; barValue.n2 = varInds.max             ; }
  else if (isValueMean ()) { barValue.n1 = 0          ; barValue.n2 = varInds.statData.mean   ; }
  else if (isValueSum  ()) { barValue.n1 = 0          ; barValue.n2 = varInds.statData.sum    ; }

  return barValue;
}

void
CQChartsDistributionPlot::
getInds(int groupInd, const Bucket &bucket, VariantInds &inds) const
{
  const auto *values = getGroupValues(groupInd);
  if (! values) return;

  auto pb = values->bucketValues.find(bucket);
  if (pb == values->bucketValues.end()) return;

  inds = (*pb).second.inds;
}

void
CQChartsDistributionPlot::
getXVals(int groupInd, const Bucket &bucket, std::vector<double> &xvals) const
{
  CQChartsDistributionPlot::VariantInds vinds;

  getInds(groupInd, bucket, vinds);

  xvals.clear();

  for (auto &vind : vinds) {
    const auto &var = vind.var;

    bool ok;

    double r = CQChartsVariant::toReal(var, ok);

    if (ok)
      xvals.push_back(r);
  }
}

bool
CQChartsDistributionPlot::
getRealValues(int groupInd, std::vector<double> &vals, CQStatData &statData) const
{
  vals.clear();

  const auto *values = getGroupValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == ColumnType::INTEGER) {
    const auto &ivals = values->valueSet->ivals();

    statData = ivals.statData();

    std::vector<double> xvals;

    for (int i = 0; i < ivals.size(); ++i)
      vals.push_back(double(*ivals.value(i)));
  }
  else if (values->valueSet->type() == ColumnType::REAL) {
    const auto &rvals = values->valueSet->rvals();

    statData = rvals.statData();

    for (int i = 0; i < rvals.size(); ++i)
      vals.push_back(*rvals.value(i));
  }
  else {
    return false;
  }

  return true;
}

bool
CQChartsDistributionPlot::
getStatData(int groupInd, CQStatData &statData) const
{
  const auto *values = getGroupValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == ColumnType::INTEGER) {
    const auto &ivals = values->valueSet->ivals();

    statData = ivals.statData();
  }
  else if (values->valueSet->type() == ColumnType::REAL) {
    const auto &rvals = values->valueSet->rvals();

    statData = rvals.statData();
  }
  else {
    return false;
  }

  return true;
}

CQChartsAxis *
CQChartsDistributionPlot::
valueAxis() const
{
  return (isVertical() ? xAxis() : yAxis());
}

CQChartsAxis *
CQChartsDistributionPlot::
countAxis() const
{
  return (isVertical() ? yAxis() : xAxis());
}

void
CQChartsDistributionPlot::
addKeyItems(PlotKey *key)
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const ColorInd &ig, const ColorInd &iv, const RangeValue &xv,
                       const RangeValue &yv, const QString &name) {
    auto *colorItem = new CQChartsDistColorKeyItem(this, ig, iv, xv, yv);
    auto *textItem  = new CQChartsDistTextKeyItem  (this, name, iv);

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(colorItem, textItem);

    key->addItem(groupItem, row, col);

    key->nextRowCol(row, col);

    return std::pair<CQChartsDistColorKeyItem *, CQChartsDistTextKeyItem *>(colorItem, textItem);
  };

  //---

  int ng = int(groupData_.groupValues.size());

  // multiple group - key item per group
  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupValues : groupData_.groupValues) {
      int         groupInd = groupValues.first;
    //const auto *values   = groupValues.second;

      auto groupName = groupIndName(groupInd);

      auto items = addKeyRow(ColorInd(ig, ng), ColorInd(), RangeValue(), RangeValue(), groupName);

      // use color column and color map data if column is valid and is the grouping column
      Color color1;

      if (adjustedGroupColor(groupInd, ng, color1))
        items.first->setColor(color1);

      //--

      ++ig;
    }
  }
  // single group - key item per value set
  else if (ng == 1) {
    if (colorColumn().isValid()) {
      bool ok;

      auto header = modelHHeaderString(colorColumn(), ok);

      key->setHeaderStr(header);

      const auto *columnDetails = this->columnDetails(colorColumn());

      if (! columnDetails->isNumeric()) {
        int nv = (columnDetails ? columnDetails->numUnique() : 0);

        for (int iv = 0; iv < nv; ++iv) {
          auto value = columnDetails->uniqueValue(iv);

          auto *colorBox = addKeyRow(ColorInd(), ColorInd(iv, nv), RangeValue(), RangeValue(),
                                     value.toString()).first;

          Color color;

          if (isColorMapped() &&
              plotType() != PlotType::SCATTER && plotType() != PlotType::DENSITY) {
            double r = CMathUtil::map(iv, 0, nv - 1, colorMapMin(), colorMapMax());

            color = colorFromColorMapPaletteValue(r);
          }
          else {
            bool ok;

            color = CQChartsVariant::toColor(value, ok);

            if (ok) {
              auto c = interpColor(color, ColorInd());

              color = Color(CQChartsDrawUtil::setColorAlpha(c, barFillAlpha()));
            }
          }

          colorBox->setColor(color);
        }
      }
      else {
        int nb = (columnDetails ? columnDetails->numBuckets() : 0);

        for (int ib = 0; ib < nb; ++ib) {
          QVariant vmin, vmax;

          columnDetails->bucketRange(ib, vmin, vmax);

          auto label = QString("[%1,%2)").arg(vmin.toString()).arg(vmax.toString());

          auto *colorBox = addKeyRow(ColorInd(), ColorInd(ib, nb), RangeValue(), RangeValue(),
                                     label).first;

          Color color;

          double r = CMathUtil::map(ib, 0, nb - 1, colorMapMin(), colorMapMax());

          color = colorFromColorMapPaletteValue(r);

          colorBox->setColor(color);
        }
      }
    }
    else {
      auto pg = groupData_.groupValues.begin();

      int         groupInd = (*pg).first;
      const auto *values   = (*pg).second;

      int nv = int(values->bucketValues.size());

      int iv = 0;

      for (const auto &bucketValues : values->bucketValues) {
        const auto &bucket   = bucketValues.first;
        const auto &varsData = bucketValues.second;

        double value1, value2;

        this->bucketValues(groupInd, bucket, value1, value2);

        auto barValue = varIndsValue(varsData);

        auto bucketName = bucketValuesStr(groupInd, bucket, values);

        if (values->xValueRange.isSet() &&  values->yValueRange.isSet()) {
          RangeValue xv(CMathUtil::avg(value1, value2),
                        values->xValueRange.min(), values->xValueRange.max());
          RangeValue yv(barValue.n2,
                        values->yValueRange.min(), values->yValueRange.max());

          addKeyRow(ColorInd(), ColorInd(iv, nv), xv, yv, bucketName);
        }

        ++iv;
      }
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int groupInd, const Bucket &bucket, BucketValueType type) const
{
  const auto *values = getGroupValues(groupInd);
  if (! values) return "";

  return bucketValuesStr(groupInd, bucket, values, type);
}

QString
CQChartsDistributionPlot::
bucketStr(int groupInd, const Bucket &bucket, BucketValueType type) const
{
  return bucketValuesStr(groupInd, bucket, nullptr, type);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int groupInd, const Bucket &bucket, const Values *values,
                BucketValueType type) const
{
  if (! isBucketed())
    return "";

  const auto &bucketer = groupBucketer(groupInd);

  bool isNumeric = (values &&
                    (values->valueSet->isNumeric() ||
                     (values->valueSet->type() == CQChartsValueSet::Type::TIME)));

  if (isNumeric) {
    double value1, value2;

    bucketValues(groupInd, bucket, value1, value2);

    if      (isExactBucketValue())
      return QString::number(value1);
    else if (type == BucketValueType::ALL) {
      if (bucketer.isIntegral() && ! CMathUtil::isInf(value1) && ! CMathUtil::isInf(value2)) {
        int ivalue1 = CMathRound::RoundNearest(value1);
        int ivalue2 = CMathRound::RoundNearest(value2);

        if (ivalue1 != ivalue2)
          return CQBucketer::bucketName(ivalue1, ivalue2, CQBucketer::NameFormat::BRACKETED);
        else
          return QString::number(ivalue1);
      }
      else {
        if (values->valueSet->type() == CQChartsValueSet::Type::TIME) {
          auto *columnDetails = this->columnDetails(valueColumns().column());

          class Formatter : public CQBucketer::Formatter {
           public:
            Formatter(CQChartsModelColumnDetails *details) :
             details_(details) {
            }

            QString formatReal(double r) const override {
              return details_->dataName(r).toString();
            }

           private:
            CQChartsModelColumnDetails *details_ { nullptr };
          };

          return CQBucketer::bucketName(value1, value2, Formatter(columnDetails),
                                        CQBucketer::NameFormat::BRACKETED);
        }
        else
          return CQBucketer::bucketName(value1, value2, CQBucketer::NameFormat::BRACKETED);
      }
    }
    else if (type == BucketValueType::START)
      return QString::number(value1);
    else
      return QString::number(value2);
  }
  else {
    if (bucket.hasValue())
      return bucketer.bucketString(bucket.value());
    else
      return "";

#if 0
    if (bucketType() == CQBucketer::Type::REAL_AUTO)
      return values->valueSet->buckets(bucket);
    else
      return values->valueSet->inds(bucket);
#endif
  }
}

void
CQChartsDistributionPlot::
bucketValues(int groupInd, const Bucket &bucket, double &value1, double &value2) const
{
  value1 = 0.0;
  value2 = 0.0;

  if (! isBucketed())
    return;

  if      (bucket.hasValue()) {
    const auto &bucketer = groupBucketer(groupInd);
    const auto *values   = getGroupValues(groupInd);

    if (filterStack_.empty()) {
      bool exactValue = isExactBucketValue();

      if      (exactValue) {
        if (values) {
          if (values->valueSet->type() == CQBaseModelType::INTEGER)
            value1 = double(values->valueSet->idi(bucket.value()));
          else
            value1 = values->valueSet->idr(bucket.value());
        }

        value2 = value1;
      }
      else if (bucketType() == CQBucketer::Type::REAL_AUTO)
        bucketer.autoBucketValues(bucket.value(), value1, value2);
      else if (bucketType() == CQBucketer::Type::REAL_RANGE)
        bucketer.bucketRValues(bucket.value(), value1, value2);
      else if (bucketType() == CQBucketer::Type::FIXED_STOPS)
        bucketer.rstopsBucketValues(bucket.value(), value1, value2);
    }
    else
       bucketer.autoBucketValues(bucket.value(), value1, value2);

    if (CMathUtil::isZero(value1)) value1 = 0.0;
    if (CMathUtil::isZero(value2)) value2 = 0.0;
  }
  else if (bucket.isUnderflow()) {
    value1 = CMathUtil::getNegInf();
    value2 = underflowBucket().real();
  }
  else if (bucket.isOverflow()) {
    value1 = overflowBucket().real();
    value2 = CMathUtil::getPosInf();
  }
}

const CQBucketer &
CQChartsDistributionPlot::
groupBucketer(int groupInd) const
{
  return const_cast<CQChartsDistributionPlot *>(this)->groupBucketer(groupInd);
}

CQBucketer &
CQChartsDistributionPlot::
groupBucketer(int groupInd)
{
  // use consistent bucketer when stacked/side by side
  if (isConsistentBucketer())
    groupInd = 0;

  auto p = groupData_.groupBucketer.find(groupInd);

  if (p == groupData_.groupBucketer.end()) {
    CQBucketer bucketer;

    initBucketer(bucketer);

    p = groupData_.groupBucketer.insert(p, GroupBucketer::value_type(groupInd, bucketer));
  }

  return (*p).second;
}

bool
CQChartsDistributionPlot::
isConsistentBucketer() const
{
  // use consistent bucketer when stacked/side by side
  if (hasGroups()) {
    if (isStacked() || isOverlaid() || isSideBySide() || isScatter())
      return true;
  }

  return false;
}

//------

QString
CQChartsDistributionPlot::
posStr(const Point &w) const
{
  if (isDensity() || isScatter())
    return CQChartsPlot::posStr(w);

  //---

  if (isVertical()) {
    auto xstr = xStr(int(w.x));

    for (const auto &plotObj : plotObjs_) {
      auto *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      double value;

      if (barObj && barObj->bucketXValue(w.x, value)) {
        xstr = QString::number(value);
        break;
      }
    }

    return xstr + " " + yStr(w.y);
  }
  else {
    auto ystr = yStr(int(w.y));

    for (const auto &plotObj : plotObjs_) {
      auto *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      double value;

      if (barObj && barObj->bucketYValue(w.y, value)) {
        ystr = QString::number(value);
        break;
      }
    }

    return xStr(w.x) + " " + ystr;
  }
}

//------

bool
CQChartsDistributionPlot::
addMenuItems(QMenu *menu)
{
  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) {
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  auto addAction = [&](const QString &name, const char *slot) {
    return addMenuAction(menu, name, slot);
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));

  QMenu *typeMenu = nullptr;

  if (hasGroups()) {
    if (! typeMenu)
      typeMenu = new QMenu("Plot Type", menu);

    (void) addMenuCheckedAction(typeMenu, "Normal"    , isNormal    (), SLOT(setNormal    (bool)));
    (void) addMenuCheckedAction(typeMenu, "Stacked"   , isStacked   (), SLOT(setStacked   (bool)));
    (void) addMenuCheckedAction(typeMenu, "Overlay"   , isOverlaid  (), SLOT(setOverlaid  (bool)));
    (void) addMenuCheckedAction(typeMenu, "SideBySide", isSideBySide(), SLOT(setSideBySide(bool)));
  }

  if (! typeMenu)
    typeMenu = new QMenu("Plot Type", menu);

  (void) addMenuCheckedAction(typeMenu, "Scatter", isScatter(), SLOT(setScatter(bool)));
  (void) addMenuCheckedAction(typeMenu, "Density", isDensity(), SLOT(setDensity(bool)));

  if (typeMenu)
    menu->addMenu(typeMenu);

  auto *valueMenu = new QMenu("Value Type", menu);

  (void) addMenuCheckedAction(valueMenu, "Count", isValueCount(), SLOT(setValueCount(bool)));
  (void) addMenuCheckedAction(valueMenu, "Range", isValueRange(), SLOT(setValueRange(bool)));
  (void) addMenuCheckedAction(valueMenu, "Min"  , isValueMin  (), SLOT(setValueMin  (bool)));
  (void) addMenuCheckedAction(valueMenu, "Max"  , isValueMax  (), SLOT(setValueMax  (bool)));
  (void) addMenuCheckedAction(valueMenu, "Mean" , isValueMean (), SLOT(setValueMean (bool)));
  (void) addMenuCheckedAction(valueMenu, "Sum"  , isValueSum  (), SLOT(setValueSum  (bool)));

  menu->addMenu(valueMenu);

  if (hasGroups())
    (void) addCheckedAction("Percent", isPercent(), SLOT(setPercent(bool)));

  (void) addCheckedAction("Skip Empty", isSkipEmpty(), SLOT(setSkipEmpty(bool)));

  //---

  menu->addSeparator();

  (void) addCheckedAction("Sorted"     , isSorted    (), SLOT(setSorted        (bool)));
  (void) addCheckedAction("Dot Lines"  , isDotLines  (), SLOT(setDotLines      (bool)));
  (void) addCheckedAction("Rug"        , isRug       (), SLOT(setRug           (bool)));
  (void) addCheckedAction("Stats Lines", isStatsLines(), SLOT(setStatsLinesSlot(bool)));

  //---

  PlotObjs objs;

  selectedPlotObjs(objs);

  menu->addSeparator();

  auto *pushAction   = addAction("Push"   , SLOT(pushSlot  ()));
  auto *popAction    = addAction("Pop"    , SLOT(popSlot   ()));
  auto *popTopAction = addAction("Pop Top", SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(! filterStack_.empty());
  popTopAction->setEnabled(! filterStack_.empty());

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  //---

  addRootMenuItems(menu);

  return true;
}

//---

bool
CQChartsDistributionPlot::
hasForeground() const
{
  if (isDensity() || isScatter())
    return false;

  if (isStatsLines())
    return true;

  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsDistributionPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isStatsLines())
    drawStatsLines(device);

  if (isColorMapKey())
    drawColorMapKey(device);
}

void
CQChartsDistributionPlot::
drawStatsLines(PaintDevice *device) const
{
  // set pen
  PenBrush penBrush;

  setStatsLineDataPen(penBrush.pen, ColorInd());

  device->setPen(penBrush.pen);

  //---

  const auto &dataRange = this->dataRange();

  int ig = 0;
  int ng = int(groupData_.groupValues.size());

  for (const auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int         groupInd = groupValues.first;
    const auto *values   = groupValues.second;

    auto statData = values->statData;

    //---

    for (const auto &plotObj : plotObjects()) {
      const auto *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      if (! barObj)
        continue;

      int groupInd1 = barObj->groupInd();

      if (groupInd1 != groupInd)
        continue;

      if (! barObj->bucket().hasValue())
        continue;

      statData.lowerMedian = barObj->mapValue(values->statData.lowerMedian);
      statData.median      = barObj->mapValue(values->statData.median);
      statData.upperMedian = barObj->mapValue(values->statData.upperMedian);

      break;
    }

    //---

    auto drawStatLine = [&](double value) {
      Point p1, p2;

      if (isVertical()) {
        p1 = Point(value, dataRange.ymin());
        p2 = Point(value, dataRange.ymax());
      }
      else {
        p1 = Point(dataRange.xmin(), value);
        p2 = Point(dataRange.xmax(), value);
      }

      device->drawLine(p1, p2);
    };

    drawStatLine(statData.lowerMedian);
    drawStatLine(statData.median);
    drawStatLine(statData.upperMedian);

    //---

    ++ig;
  }
}

//---

void
CQChartsDistributionPlot::
pushSlot()
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  if (objs.empty()) {
    auto gpos = view()->menuPos();
    auto pos  = view()->mapFromGlobal(QPointF(gpos.x, gpos.y).toPoint());

    auto w = pixelToWindow(Point(pos));

    plotObjsAtPoint(w, objs, Constraints::SELECTABLE);
  }

  if (objs.empty())
    return;

  Filters filters;

  for (const auto &obj : objs) {
    auto *distObj = qobject_cast<CQChartsDistributionBarObj *>(obj);

    double value1, value2;

    bucketValues(distObj->groupInd(), distObj->bucket(), value1, value2);

    filters.emplace_back(distObj->groupInd(), value1, value2);
  }

  filterStack_.push_back(std::move(filters));

  updateRangeAndObjs();
}

void
CQChartsDistributionPlot::
popSlot()
{
  if (! filterStack_.empty()) {
    filterStack_.pop_back();

    updateRangeAndObjs();
  }
}

void
CQChartsDistributionPlot::
popTopSlot()
{
  if (! filterStack_.empty()) {
    filterStack_.clear();

    updateRangeAndObjs();
  }
}

//------

double
CQChartsDistributionPlot::
getPanX(bool is_shift) const
{
  return windowToViewWidth(is_shift ? 2.0*barWidth_ : 1.0*barWidth_);
}

double
CQChartsDistributionPlot::
getPanY(bool is_shift) const
{
  return windowToViewHeight(is_shift ? 2.0*barWidth_ : 1.0*barWidth_);
}

//------

bool
CQChartsDistributionPlot::
hasGroups() const
{
  return (groupData_.groupValues.size() > 1);
}

//---

CQChartsDistributionDensityObj *
CQChartsDistributionPlot::
createDensityObj(const BBox &rect, int groupInd, const DensityObjData &data, double doffset,
                 const ColorInd &is) const
{
  return new CQChartsDistributionDensityObj(this, rect, groupInd, data, doffset, is);
}

CQChartsDistributionScatterObj *
CQChartsDistributionPlot::
createScatterObj(const BBox &rect, int groupInd, const Bucket &bucket, int n, const ColorInd &is,
                 const ColorInd &iv) const
{
  return new CQChartsDistributionScatterObj(this, rect, groupInd, bucket, n, is, iv);
}

CQChartsDistributionBarObj *
CQChartsDistributionPlot::
createBarObj(const BBox &rect, int groupInd, const Bucket &bucket, const BarValue &barValue,
             bool isLine, const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsDistributionBarObj(this, rect, groupInd, bucket, barValue, isLine, ig, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsDistributionPlot::
createCustomControls()
{
  auto *controls = new CQChartsDistributionPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(const Plot *plot, const BBox &rect, int groupInd, const Bucket &bucket,
                           const BarValue &barValue, bool isLine, const ColorInd &ig,
                           const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, ColorInd(), ig, iv), plot_(plot),
 groupInd_(groupInd), bucket_(bucket), barValue_(barValue), isLine_(isLine)
{
  setDetailHint(DetailHint::MAJOR);

  plot_->bucketValues(groupInd_, bucket_, value1_, value2_);
}

QString
CQChartsDistributionBarObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsDistributionBarObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  //---

  auto groupName = this->groupName();

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  //---

  auto bucketStr = this->bucketStr();

  tableTip.addTableRow("Bucket", bucketStr);

  //---

  // add value
  if      (plot_->isValueCount()) {
    tableTip.addTableRow("Count", count());
  }
  else if (plot_->isValueRange()) {
    tableTip.addTableRow("Min", minValue());
    tableTip.addTableRow("Max", maxValue());
  }
  else if (plot_->isValueMin()) {
    tableTip.addTableRow("Min", maxValue());
  }
  else if (plot_->isValueMax()) {
    tableTip.addTableRow("Max", maxValue());
  }
  else if (plot_->isValueMean()) {
    tableTip.addTableRow("Mean", maxValue());
  }
  else if (plot_->isValueSum()) {
    tableTip.addTableRow("Sum", maxValue());
  }

  //---

  // add color columns
  QStringList strs;

  QModelIndex parent;

  for (const auto &row : colorData_.colorRows) {
    ModelIndex colorColumnInd(plot(), row, plot_->colorColumn(), parent);

    bool ok;

    auto str = plot_->modelString(colorColumnInd, ok);

    if (ok)
      strs.push_back(str);
  }

  if (strs.length()) {
    bool ok;

    auto name = plot_->modelHHeaderString(plot_->colorColumn(), ok);

    QString name1;

    if (name != "")
      name1 = QString("Color (%1)").arg(name);
    else
      name1 = "Colors";

    tableTip.addTableRow(name1, strs.join(" "));
  }

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

//---

void
CQChartsDistributionBarObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  if (! isLine())
    model->addProperty(path1, this, "rect")->setDesc("Bounding box");

//model->addProperty(path1, this, "selected")->setDesc("Is selected");
  model->addProperty(path1, this, "count"   )->setDesc("Number of values");
  model->addProperty(path1, this, "minValue")->setDesc("Min value");
  model->addProperty(path1, this, "maxValue")->setDesc("Max value");
}

//---

QString
CQChartsDistributionBarObj::
groupName() const
{
  QString groupName;

  if (ig_.n > 1)
    groupName = plot_->groupIndName(groupInd_);

  return groupName;
}

QString
CQChartsDistributionBarObj::
bucketStr() const
{
  QString bucketStr;

  if (! plot_->isBucketed())
    bucketStr = plot_->groupIndName(groupInd_);
  else
    bucketStr = plot_->bucketValuesStr(groupInd_, bucket_);

  return bucketStr;
}

bool
CQChartsDistributionBarObj::
bucketXValue(double x, double &value) const
{
  if (! insideX(x))
    return false;

  if (! plot_->isBucketed())
    value = groupInd_;
  else {
    double value1, value2;

    plot_->bucketValues(groupInd_, bucket_, value1, value2);

    value = CMathUtil::map(x, rect().getXMin(), rect().getXMax(), value1, value2);
  }

  return true;
}

bool
CQChartsDistributionBarObj::
bucketYValue(double y, double &value) const
{
  if (! insideY(y))
    return false;

  if (! plot_->isBucketed())
    value = groupInd_;
  else {
    double value1, value2;

    plot_->bucketValues(groupInd_, bucket_, value1, value2);

    value = CMathUtil::map(y, rect().getYMin(), rect().getYMax(), value1, value2);
  }

  return true;
}

int
CQChartsDistributionBarObj::
count() const
{
  return int(barValue_.n2);
}

double
CQChartsDistributionBarObj::
minValue() const
{
  return barValue_.n1;
}

double
CQChartsDistributionBarObj::
maxValue() const
{
  return barValue_.n2;
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
dataLabelRect() const
{
  if (! plot_->dataLabel()->isVisible())
    return BBox();

  auto bbox = calcRect();

  QString ystr;

  if      (plot_->isValueCount()) { ystr = QString::number(count()); }
  else if (plot_->isValueRange()) { ystr = QString("%1-%2").arg(minValue()).arg(maxValue()); }
  else if (plot_->isValueMin  ()) { ystr = QString::number(maxValue()); }
  else if (plot_->isValueMax  ()) { ystr = QString::number(maxValue()); }
  else if (plot_->isValueMean ()) { ystr = QString::number(maxValue()); }
  else if (plot_->isValueSum  ()) { ystr = QString::number(maxValue()); }

  return plot_->dataLabel()->calcRect(bbox, ystr);
}

void
CQChartsDistributionBarObj::
getObjSelectIndices(Indices &inds) const
{
  Plot::VariantInds vinds;

  plot_->getInds(groupInd_, bucket_, vinds);

  for (auto &vind : vinds) {
    const auto &ind = vind.ind;

    addSelectIndex(inds, plot()->normalizeIndex(ind));
  }
}

void
CQChartsDistributionBarObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    Plot::VariantInds vinds;

    plot_->getInds(groupInd_, bucket_, vinds);

    for (auto &vind : vinds) {
      ModelIndex ind = vind.ind;

      ind.setColumn(column);

      addSelectIndex(inds, plot()->normalizeIndex(ind));
    }
  }
}

void
CQChartsDistributionBarObj::
draw(PaintDevice *device) const
{
  auto bbox = calcRect();

  //---

  bool useLine = (isLine() || this->isUseLine());

  barColor_ = QColor();

  //---

  CQChartsImage image;

  if (plot_->imageColumn().isValid()) {
    Plot::VariantInds vinds;

    plot_->getInds(groupInd_, bucket_, vinds);

    for (auto &vind : vinds) {
      ModelIndex ind = vind.ind;

      ind.setColumn(plot_->imageColumn());

      bool ok;

      auto imageVar = plot_->modelValue(ind, ok);
      if (! ok) continue;

      image = CQChartsVariant::toImage(imageVar, ok);
      if (! ok || ! image.isValid()) continue;

      break;
    }
  }

  //---

  // get bar colors
  colorData_ = ColorData();

  if (getBarColoredRects(colorData_)) {
    auto pbbox = plot_->windowToPixel(bbox);

    double size = (plot_->isVertical() ? pbbox.getHeight() : pbbox.getWidth());

    if      (plot_->isValueCount()) {
      double dsize = size/colorData_.nv;

      double pos1 = 0.0, pos2 = 0.0;

      for (auto &p : colorData_.colorSet) {
        const auto &color = p.first.color;
        int         n     = colorData_.colorCount[p.second];

        pos1 = pos2;
        pos2 = pos1 + dsize*n;

        BBox pbbox1;

        if (plot_->isVertical())
          pbbox1 = BBox(pbbox.getXMin(), pbbox.getYMax() - pos2,
                        pbbox.getXMax(), pbbox.getYMax() - pos1);
        else
          pbbox1 = BBox(pbbox.getXMin() + pos1, pbbox.getYMin(),
                        pbbox.getXMin() + pos2, pbbox.getYMax());

        //---

        auto bbox1 = plot_->pixelToWindow(pbbox1);

        drawRect(device, bbox1, color, useLine);

        barColor_ = color.color();
      }

      if (colorData_.colorSet.size() != 1)
        barColor_ = QColor();
    }
    else if (plot_->isValueSum()) {
      double pos1 = 0.0, pos2 = 0.0;

      for (auto &cs : colorData_.colorSizes) {
        const auto &color = cs.indColor.color;
        double      dsize = cs.size;

        pos1 = pos2;
        pos2 = pos1 + size*dsize;

        BBox pbbox1;

        if (plot_->isVertical())
          pbbox1 = BBox(pbbox.getXMin(), pbbox.getYMax() - pos2,
                        pbbox.getXMax(), pbbox.getYMax() - pos1);
        else
          pbbox1 = BBox(pbbox.getXMin() + pos1, pbbox.getYMin(),
                        pbbox.getXMin() + pos2, pbbox.getYMax());

        //---

        auto bbox1 = plot_->pixelToWindow(pbbox1);

        drawRect(device, bbox1, color, useLine);

        barColor_ = color.color();
      }

      if (colorData_.colorSizes.size() != 1)
        barColor_ = QColor();
    }
  }
  else {
    barColor_ = this->barColor();

    drawRect(device, bbox, Color(barColor_), useLine);
  }

  //---

  // draw image
  if (image.isValid()) {
    device->drawImageInRect(bbox, image);

    barColor_ = QColor();
  }
}

void
CQChartsDistributionBarObj::
drawFg(PaintDevice *device) const
{
  if (! isLine()) {
    auto bbox = calcRect();

    //---

    QString ystr;

    if      (plot_->isValueCount()) { ystr = QString::number(count()); }
    else if (plot_->isValueRange()) { ystr = QString("%1-%2").arg(minValue()).arg(maxValue()); }
    else if (plot_->isValueMin  ()) { ystr = QString::number(maxValue()); }
    else if (plot_->isValueMax  ()) { ystr = QString::number(maxValue()); }
    else if (plot_->isValueMean ()) { ystr = QString::number(maxValue()); }
    else if (plot_->isValueSum  ()) { ystr = QString::number(maxValue()); }

    //---

    auto pbbox = plot_->windowToPixel(bbox);

    if (! plot_->dataLabel()->isAdjustedPositionOutside(pbbox, ystr) && barColor_.isValid())
      plot_->charts()->setContrastColor(barColor_);

    plot_->dataLabel()->draw(device, bbox, ystr);

    plot_->charts()->resetContrastColor();
  }

  //---

  if (plot_->isRug())
    drawRug(device);
}

void
CQChartsDistributionBarObj::
drawRug(PaintDevice *device) const
{
  // get symbol and size
  auto symbol     = plot_->rugSymbol();
  auto symbolSize = plot_->rugSymbolSize();

  if (! symbol.isValid())
    symbol = (plot_->isVertical() ? Symbol::vline() : Symbol::hline());

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy, /*scale*/false);

  //---

  // set pen and brush
  auto ic = (ig_.n > 1 ? ig_ : iv_);

  PenBrush penBrush;

  plot_->setRugSymbolPenBrush(penBrush, ic);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw symbols
  const auto &dataRange = plot_->dataRange();

  std::vector<double> xvals;

  plot_->getXVals(groupInd_, bucket_, xvals);

  for (const auto &x : xvals) {
    double x1 = mapValue(x);

    Point p;

    if (plot_->isVertical())
      p = Point(x1, dataRange.ymin());
    else
      p = Point(dataRange.xmin(), x1);

    auto ps = plot_->windowToPixel(p);

    if (plot_->isVertical())
      ps.setY(ps.y + sy);
    else
      ps.setX(ps.x - sx);

    auto p1 = plot_->pixelToWindow(ps);

    CQChartsDrawUtil::drawSymbol(device, symbol, p1, symbolSize, /*scale*/false);
  }
}

double
CQChartsDistributionBarObj::
mapValue(double v) const
{
  double bmin = (plot_->isVertical() ? rect_.getXMin() : rect_.getYMin());
  double bmax = (plot_->isVertical() ? rect_.getXMax() : rect_.getYMax());

  return CMathUtil::map(v, value1_, value2_, bmin, bmax);
}

bool
CQChartsDistributionBarObj::
getBarColoredRects(ColorData &colorData) const
{
  if (! plot_->colorColumn().isValid())
    return false;

  if (! plot_->isValueCount() && ! plot_->isValueSum())
    return false;

  // get normal bar color
  auto barColor = this->barColor();
  auto bgColor  = plot_->interpThemeColor(ColorInd(0.2));

  // get color of individual values
  colorData.nv = 0;

  // for count value type get count of unique colors for values
  // for sum value type get fraction of total for values
  Plot::VariantInds vinds;

  plot_->getInds(groupInd_, bucket_, vinds);

  int nvi = int(vinds.size());
  if (nvi < 1) return false;

  //---

  const auto *columnDetails = plot_->columnDetails(plot_->colorColumn());

  bool isNumeric = (columnDetails ? columnDetails->isNumeric() : false);
  bool isColor   = (columnDetails ? columnDetails->type() == CQBaseModelType::COLOR : false);

  int nv = (columnDetails ? columnDetails->numUnique () : 1);
  int nb = (isNumeric     ? columnDetails->numBuckets() : 1);

  auto colorMapMin = plot_->colorMapMin();
  auto colorMapMax = plot_->colorMapMax();

  //---

  double minAlpha = 0.4;
  double maxAlpha = 1.0;

  double bsize = 1.0/nvi;

  for (const auto &vind : vinds) {
    const auto &ind = vind.ind;

    //---

    // calc relative size
    double bsize1 = bsize;

    if (plot_->isValueSum()) {
      bool ok;

      double value = plot_->modelReal(ind, ok);

      if (ok)
        bsize1 = value/maxValue();
      else
        bsize1 = 0.0;
    }

    //---

    // get color column value from model
    ModelIndex colorInd(plot_, ind.row(), plot_->colorColumn(), ind.parent());

    bool ok;
    auto var = plot_->modelValue(colorInd, ok);

    // set color from value
    Color  color;
    bool   colorSet   = false;
    int    colorIVal  = 0;
    double colorValue = 0.0;

    if (ok && var.isValid()) {
      if (isColor || CQChartsVariant::isColor(var)) {
        bool ok;
        color = CQChartsVariant::toColor(var, ok);
        colorSet = ok;
      }

      if (! colorSet) {
        if (! isNumeric) {
          colorIVal  = (columnDetails ? columnDetails->valueInd(var) : 0);
          colorValue = CMathUtil::map(colorIVal, 0, nv - 1, colorMapMin, colorMapMax);
        }
        else {
          colorIVal  = columnDetails->bucket(var);
          colorValue = CMathUtil::map(colorIVal, 0, nb - 1, colorMapMin, colorMapMax);
        }

        color = plot_->colorFromColorMapPaletteValue(colorValue);
      }

      //---

      auto c1 = plot_->interpColor(color, ColorInd());

      color    = Color(CQChartsDrawUtil::setColorAlpha(c1, plot_->barFillAlpha()));
      colorSet = true;
    }

    if (! colorSet) {
      colorIVal  = colorData.nv;
      colorValue = CMathUtil::map(colorIVal, 0, nvi - 1, minAlpha, maxAlpha);

      auto barColor1 = CQChartsUtil::blendColors(barColor, bgColor, colorValue);

      color = Color(barColor1);
    }

    //---

    IndColor indColor(colorIVal, color);

    // add unique colors to set
    auto p = colorData.colorSet.find(indColor);

    if (p == colorData.colorSet.end()) {
      colorData.colorRows.insert(ind.row());

      int ind = int(colorData.colorSet.size());

      p = colorData.colorSet.insert(p, IndColorSet::value_type(indColor, ind));
    }

    ++colorData.colorCount[(*p).second];

    //---

    colorData.colorSizes.emplace_back(indColor, bsize1);

    ++colorData.nv;
  }

  if (colorData.colorSet.empty()) {
    IndColor indColor(0, Color(barColor));

    colorData.colorSet[indColor] = 0;

    colorData.colorCount[0] = 1;

    colorData.colorSizes.emplace_back(indColor, 1.0);

    colorData.nv = 1;
  }

  return true;
}

void
CQChartsDistributionBarObj::
drawRect(PaintDevice *device, const BBox &bbox, const Color &color, bool useLine) const
{
  // calc pen and brush
  PenBrush barPenBrush;

  bool updateState = device->isInteractive();

  calcBarPenBrush(color, useLine, barPenBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, barPenBrush);

  //---

  if (! plot_->isDotLines()) {
    // draw rect
    if (! useLine) {
      CQChartsDrawUtil::drawRoundedRect(device, bbox, plot_->barCornerSize());
    }
    else {
      if (bbox.getWidth() < bbox.getHeight()) { // vertical
        double xc = bbox.getXMid();

        device->drawLine(Point(xc, bbox.getYMin()), Point(xc, bbox.getYMax()));
      }
      else {
        double yc = bbox.getYMid();

        device->drawLine(Point(bbox.getXMin(), yc), Point(bbox.getXMax(), yc));
      }
    }
  }
  else {
    auto pbbox = plot_->windowToPixel(bbox);

    // draw line
    double lw = plot_->lengthPixelSize(plot_->dotLineWidth(), plot_->isVertical());

    if (plot_->isVertical()) {
      if (lw < 3.0) {
        double xc = bbox.getXMid();

        device->drawLine(Point(xc, bbox.getYMin()), Point(xc, bbox.getYMax()));
      }
      else {
        double pxc = pbbox.getXMid();

        BBox pbbox1(pxc - lw/2.0, pbbox.getYMin(), pxc + lw/2.0, pbbox.getYMax());

        auto bbox1 = plot_->pixelToWindow(pbbox1);

        CQChartsDrawUtil::drawRoundedRect(device, bbox1);
      }
    }
    else {
      if (lw < 3.0) {
        double yc = bbox.getYMid();

        device->drawLine(Point(bbox.getXMin(), yc), Point(bbox.getXMax(), yc));
      }
      else {
        double pyc = pbbox.getYMid();

        BBox pbbox1(pbbox.getXMin(), pyc - lw/2.0, pbbox.getXMax(), pyc + lw/2.0);

        auto bbox1 = plot_->pixelToWindow(pbbox1);

        CQChartsDrawUtil::drawRoundedRect(device, bbox1);
      }
    }

    //---

    // get dot symbol and size
    auto symbol     = plot_->dotSymbol();
    auto symbolSize = plot_->dotSymbolSize();

    auto ic = (ig_.n > 1 ? ig_ : iv_);

    //---

    // set dot pen and brush
    PenBrush dotPenBrush;

    plot_->setDotSymbolPenBrush(dotPenBrush, ic);

    //---

    // draw dot
    Point p;

    if (plot_->isVertical())
      p = Point(bbox.getXMid(), bbox.getYMax());
    else
      p = Point(bbox.getXMax(), bbox.getYMid());

    if (symbol.isValid())
      CQChartsDrawUtil::drawSymbol(device, dotPenBrush, symbol, p, symbolSize, /*scale*/true);
  }

  device->resetColorNames();
}

void
CQChartsDistributionBarObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto barColor = this->barColor();
  bool useLine  = this->isUseLine();

  calcBarPenBrush(Color(barColor), useLine, penBrush, updateState);
}

void
CQChartsDistributionBarObj::
calcBarPenBrush(const Color &color, bool useLine, PenBrush &barPenBrush, bool updateState) const
{
  // set pen and brush
  auto bc = plot_->interpBarStrokeColor(ColorInd());
  auto fc = plot_->interpColor(color, ColorInd());

  auto bw = plot_->barStrokeWidth();

  if (useLine) {
    bw = Length::pixel(0);

    if (plot_->isBarFilled())
      bc = fc;
  }

  plot_->setPenBrush(barPenBrush,
    plot_->barPenData(bc, Alpha(), bw), plot_->barBrushData(fc));

  // adjust pen/brush for selected/mouse over
  if (updateState)
    plot_->updateObjPenBrushState(this, barPenBrush);
}

QColor
CQChartsDistributionBarObj::
barColor() const
{
  auto colorInd = this->calcColorInd();

  return plot_->interpBarFillColor(colorInd);
}

bool
CQChartsDistributionBarObj::
isUseLine() const
{
  bool useLine = false;

  if (! plot_->isDotLines()) {
    auto bbox  = calcRect();
    auto pbbox = plot_->windowToPixel(bbox);

    double s = (plot_->isVertical() ? pbbox.getWidth() : pbbox.getHeight());

    useLine = (s <= 2);
  }

  return useLine;
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
calcRect() const
{
  double minSize = plot_->minBarSize();

  //---

  auto prect = plot_->windowToPixel(rect_);

  //---

  // calc margins

  double ml = plot_->lengthPixelSize(plot_->margin(), plot_->isVertical());
  double mr = ml;

  if (plot_->hasGroups()) {
    if      (plot_->isStacked()) {
    }
    else if (plot_->isOverlaid()) {
    }
    // tight packing for side by side
    else if (plot_->isSideBySide()) {
      ml = 0.0;
      mr = 0.0;

      if      (ig_.i == 0)
        ml = 1.0;
      else if (ig_.i == ig_.n - 1)
        mr = 1.0;
    }
    else {
      // adjust margins for first/last bar in group
      if      (iv_.i == 0)
        ml = plot_->lengthPixelSize(plot_->groupMargin(), plot_->isVertical());
      else if (iv_.i == iv_.n - 1)
        mr = plot_->lengthPixelSize(plot_->groupMargin(), plot_->isVertical());
    }
  }

  //---

  // adjust rect by margins
  double rs = prect.getSize(plot_->isVertical());

  double s1 = rs - 2*std::max(ml, mr);

  if (s1 < minSize) {
    ml = (rs - minSize)/2.0;
    mr = ml;
  }

  prect.expandExtent(-ml, -mr, plot_->isVertical());

  return plot_->pixelToWindow(prect);
}

double
CQChartsDistributionBarObj::
xColorValue(bool relative) const
{
  return (relative ? barValue_.xr.map() : barValue_.xr.v);
}

double
CQChartsDistributionBarObj::
yColorValue(bool relative) const
{
  return (relative ? barValue_.yr.map() : barValue_.yr.v);
}

//------

CQChartsDistributionDensityObj::
CQChartsDistributionDensityObj(const Plot *plot, const BBox &rect, int groupInd, const Data &data,
                               double doffset, const ColorInd &is) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), groupInd_(groupInd),
 data_(data), doffset_(doffset), is_(is)
{
  setDetailHint(DetailHint::MAJOR);

  //---

  // create density polygon
  int np = int(data_.points.size());

  if (np < 2) {
    poly_ = Polygon();
    return;
  }

  double y1 = data_.ymin;

  if (plot->isVertical()) {
    for (int i = 0; i < np; ++i)
      poly_.addPoint(Point(data_.points[size_t(i)].x, data_.points[size_t(i)].y - y1 + doffset_));
  }
  else {
    for (int i = 0; i < np; ++i)
      poly_.addPoint(Point(data_.points[size_t(i)].y - y1 + doffset_, data_.points[size_t(i)].x));
  }

  //----

  // calc scale factor for data bars
  int nb = int(data_.buckets.size());

  double area = 0.0;

  for (int i = 0; i < nb; ++i) {
    double dy = data_.buckets[size_t(i)].n;

    double value1, value2;

    plot_->bucketValues(groupInd_, Bucket(data_.buckets[size_t(i)].bucket), value1, value2);

    double dx = (value2 - value1)/(data_.xmax - data_.xmin);

    area += dx*dy;
  }

  bucketScale_ = 1.0/area;
}

QString
CQChartsDistributionDensityObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(is_.i);
}

QString
CQChartsDistributionDensityObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto groupName = this->groupName();
  auto ns        = this->numSamples();

  tableTip.addTableRow("Name"   , groupName);
  tableTip.addTableRow("Samples", ns);
  tableTip.addTableRow("Min"    , data_.xmin);
  tableTip.addTableRow("Max"    , data_.xmax);

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

QString
CQChartsDistributionDensityObj::
groupName() const
{
  return plot_->groupIndName(groupInd_);
}

int
CQChartsDistributionDensityObj::
numSamples() const
{
  return int(data_.points.size());
}

bool
CQChartsDistributionDensityObj::
inside(const Point &p) const
{
  if (! isVisible())
    return false;

  return poly_.containsPoint(p, Qt::OddEvenFill);
}

void
CQChartsDistributionDensityObj::
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

  // draw bars for buckets
  if (plot_->isDensityBars()) {
    int nb = int(data_.buckets.size());

    for (int i = 0; i < nb; ++i) {
      double y = data_.buckets[size_t(i)].n*bucketScale_;

      double value1, value2;

      plot_->bucketValues(groupInd_, Bucket(data_.buckets[size_t(i)].bucket), value1, value2);

      BBox bbox(value1, 0, value2, y);

      device->drawRect(bbox);
    }
  }

  //---

  // draw density polygon
  device->drawPolygon(poly_);

  device->resetColorNames();
}

void
CQChartsDistributionDensityObj::
drawFg(PaintDevice *device) const
{
  if (plot_->isStatsLines())
    drawStatsLines(device);

  if (plot_->isRug())
    drawRug(device);
}

void
CQChartsDistributionDensityObj::
drawStatsLines(PaintDevice *device) const
{
  // set pen
  PenBrush penBrush;

  plot_->setStatsLineDataPen(penBrush.pen, ColorInd());

  device->setPen(penBrush.pen);

  //---

  const auto &dataRange = plot_->dataRange();

  auto drawStatLine = [&](double value) {
    Point p1, p2;

    if (plot_->isVertical()) {
      p1 = Point(value, dataRange.ymin());
      p2 = Point(value, dataRange.ymax());
    }
    else {
      p1 = Point(dataRange.xmin(), value);
      p2 = Point(dataRange.xmax(), value);
    }

    device->drawLine(p1, p2);
  };

  drawStatLine(data_.statData.lowerMedian);
  drawStatLine(data_.statData.median);
  drawStatLine(data_.statData.upperMedian);
}

void
CQChartsDistributionDensityObj::
drawRug(PaintDevice *device) const
{
  // get symbol and size
  auto symbol     = plot_->rugSymbol();
  auto symbolSize = plot_->rugSymbolSize();

  if (! symbol.isValid())
    symbol = (plot_->isVertical() ? Symbol::vline() : Symbol::hline());

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy, /*scale*/false);

  //---

  // set pen brush
  // TODO: allow control of alpha, and line width
  PenBrush penBrush;

  auto fillColor = plot_->interpBarFillColor(is_);

  plot_->setPenBrush(penBrush,
    PenData(true, fillColor), BrushData(true, fillColor, Alpha(0.5)));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  const auto &dataRange = plot_->dataRange();

  std::vector<double> xvals;
  CQStatData          statData;

  (void) plot_->getRealValues(groupInd_, xvals, statData);

  for (const auto &x1 : xvals) {
    Point p1;

    if (plot_->isVertical())
      p1 = Point(x1, dataRange.ymin());
    else
      p1 = Point(dataRange.xmin(), x1);

    auto ps = plot_->windowToPixel(p1);

    if (plot_->isVertical())
      ps.setY(ps.y + sy);
    else
      ps.setX(ps.x - sx);

    auto p2 = plot_->pixelToWindow(ps);

    CQChartsDrawUtil::drawSymbol(device, symbol, p2, symbolSize, /*scale*/false);
  }
}

void
CQChartsDistributionDensityObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // set pen and brush
  auto bc = plot_->interpBarStrokeColor(is_);
  auto fc = plot_->interpBarFillColor  (is_);

  plot_->setPenBrush(penBrush, plot_->barPenData(bc), plot_->barBrushData(fc));

  //---

  // adjust brush for gradient
  if (plot_->isDensityGradient()) {
    auto pixelRect = plot_->calcPlotPixelRect();

    Point pg1, pg2;

    if (plot_->isVertical()) {
      pg1 = Point(pixelRect.getXMin(), pixelRect.getYMin());
      pg2 = Point(pixelRect.getXMax(), pixelRect.getYMin());
    }
    else {
      pg1 = Point(pixelRect.getXMin(), pixelRect.getYMax());
      pg2 = Point(pixelRect.getXMin(), pixelRect.getYMin());
    }

    QLinearGradient lg(pg1.x, pg1.y, pg2.x, pg2.y);

    auto *palette = plot_->view()->themePalette();

    palette->setLinearGradient(lg, plot_->barFillAlpha().value());

    penBrush.brush = QBrush(lg);
  }

  // adjust pen/brush for selected/mouse over
  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsDistributionScatterObj::
CQChartsDistributionScatterObj(const Plot *plot, const BBox &rect, int groupInd,
                               const Bucket &bucket, int n, const ColorInd &is,
                               const ColorInd &iv) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), groupInd_(groupInd),
 bucket_(bucket), n_(n), is_(is), iv_(iv)
{
  // get factored number of points
  int nf = CMathUtil::clamp(int(n_*plot_->scatterFactor()), 1, n_);

  // generate random points in box (0.0->1.0) with margin
  double m = std::min(std::max(plot_->scatterMargin(), 0.0), 1.0);

  // TODO: constant seed ?
  CQChartsRand::RealInRange rand(m, 1.0 - m);

  points_.resize(size_t(nf));

  for (int i = 0; i < nf; ++i)
    points_[size_t(i)] = Point(rand.gen(), rand.gen());
}

//---

QString
CQChartsDistributionScatterObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(is_.i).arg(iv_.i);
}

QString
CQChartsDistributionScatterObj::
calcTipId() const
{
  auto groupName = plot_->groupIndName(groupInd_);
  auto bucketStr = plot_->bucketStr(groupInd_, bucket_);

  CQChartsTableTip tableTip;

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  tableTip.addTableRow("Bucket", bucketStr);

  tableTip.addTableRow("Num Values", n_);

  //---

  //plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

//---

void
CQChartsDistributionScatterObj::
draw(PaintDevice *device) const
{
  // set pen brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  auto prect = plot_->windowToPixel(rect());

  //---

  auto symbol     = Symbol::circle();
  auto symbolSize = Length::pixel(6);

  auto pll = prect.getLL();

  // points in range (m, 1 - m) where 'm' is margin (< 1)
  for (const auto &point : points_) {
    double px, py;

    if (plot_->isVertical()) {
      px = plot_->windowToPixelWidth (point.x);
      py = plot_->windowToPixelHeight(point.y);
    }
    else {
      px = plot_->windowToPixelWidth (point.y);
      py = plot_->windowToPixelHeight(point.x);
    }

    Point p(pll.x + px, pll.y + py);

    auto p1 = plot_->pixelToWindow(p);

    CQChartsDrawUtil::drawSymbol(device, symbol, p1, symbolSize, /*scale*/true);
  }
}

void
CQChartsDistributionScatterObj::
calcPenBrush(PenBrush &penBrush, bool) const
{
  // TODO: allow control of stroke color, alpha, and line width
  auto ic = (is_.n > 1 ? is_ : iv_);

  auto c = plot_->interpBarFillColor(ic);

  plot_->setPenBrush(penBrush, PenData(true, Qt::black), BrushData(true, c));
}

//------

CQChartsDistColorKeyItem::
CQChartsDistColorKeyItem(Plot *plot, const ColorInd &ig, const ColorInd &iv,
                         const RangeValue &xv, const RangeValue &yv) :
 CQChartsColorBoxKeyItem(plot, ColorInd(), ig, iv, xv, yv), plot_(plot)
{
  setClickable(true);
}

#if 0
bool
CQChartsDistColorKeyItem::
selectPress(const Point &, SelData &)
{
  setSetHidden(! isSetHidden());

  return true;
}
#endif

#if 0
void
CQChartsDistColorKeyItem::
doSelect(SelMod)
{
  CQChartsPlot::PlotObjs objs;

  plot()->getGroupObjs(ig_.i, objs);
  if (objs.empty()) return;

  //---

  plot()->selectObjs(objs, /*export*/true);

  key_->redraw(/*wait*/ true);
}
#endif

QBrush
CQChartsDistColorKeyItem::
fillBrush() const
{
  if (color_.isValid())
    return plot_->interpColor(color_, ColorInd());

  auto colorInd = this->calcColorInd();

  auto c = plot_->interpBarFillColor(colorInd);

  adjustFillColor(c);

  return c;
}

#if 0
bool
CQChartsDistColorKeyItem::
isSetHidden() const
{
  if (ig_.n > 1)
    return plot_->CQChartsPlot::isSetHidden(ig_.i);
  else
    return plot_->CQChartsPlot::isSetHidden(iv_.i);
}

void
CQChartsDistColorKeyItem::
setSetHidden(bool b)
{
  if (ig_.n > 1)
    plot_->CQChartsPlot::setSetHidden(ig_.i, b);
  else
    plot_->CQChartsPlot::setSetHidden(iv_.i, b);
}
#endif

//------

CQChartsDistTextKeyItem::
CQChartsDistTextKeyItem(Plot *plot, const QString &text, const ColorInd &iv) :
 CQChartsTextKeyItem(plot, text, iv)
{
}

QColor
CQChartsDistTextKeyItem::
interpTextColor(const ColorInd &ind) const
{
  auto c = CQChartsTextKeyItem::interpTextColor(ind);

  adjustFillColor(c);

  return c;
}

#if 0
bool
CQChartsDistTextKeyItem::
isSetHidden() const
{
  return plot_->CQChartsPlot::isSetHidden(ic_.i);
}
#endif

//------

CQChartsDistributionPlot::Values::
Values(CQChartsValueSet *valueSet) :
 valueSet(valueSet)
{
  densityData = new CQChartsDensity;
}

CQChartsDistributionPlot::Values::
~Values()
{
  delete valueSet;
  delete densityData;
}

//------

CQChartsDistributionPlotCustomControls::
CQChartsDistributionPlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "distribution")
{
}

void
CQChartsDistributionPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsDistributionPlotCustomControls::
addWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // values, name and data columns
//auto columns = QStringList() << "values" << "name" << "data";
  auto columns = QStringList() << "values" << "data";

  addColumnWidgets(columns, columnsFrame);

  //---

  addOptionsWidgets();

  //---

  // bucket group
  auto bucketFrame = createGroupFrame("Bucket", "bucketFrame");

  auto *bucketCornerFrame  = CQUtil::makeWidget<QFrame>("bucketCornerFrame");
  auto *bucketCornerLayout = CQUtil::makeLayout<QHBoxLayout>(bucketCornerFrame, 0, 2);

  bucketRadioGroup_ = new QButtonGroup(this);

  auto createBucketRadio = [&](const QString &name, const QString &icon, const QString &tip) {
    auto *radio = CQUtil::makeWidget<CQIconRadio>(name);

    radio->setIcon(icon);

    radio->setToolTip(tip);

    bucketRadioGroup_->addButton(radio);

    bucketCornerLayout->addWidget(radio);

    return radio;
  };

  uniqueBucketRadio_ =
    createBucketRadio("uniqueBucketRadio", "BUCKET_UNIQUE", "Bucket per Unique Value");
  fixedBucketRadio_  =
    createBucketRadio("fixedBucketRadio", "BUCKET_FIXED", "Bucket from Start and Delta");
  rangeBucketRadio_  =
    createBucketRadio("rangeBucketRadio", "BUCKET_RANGE", "Bucket from Range and Number");
  stopsBucketRadio_  =
    createBucketRadio("stopsBucketRadio", "BUCKET_STOPS", "Buckets for specified Stops");

  bucketFrame.groupBox->setCornerWidget(bucketCornerFrame);

  //---

  bucketRange_     = CQUtil::makeWidget<CQDoubleRangeSlider>("bucketRange");
  startBucketEdit_ = CQUtil::makeWidget<CQRealSpin>("startBucketEdit");
  deltaBucketEdit_ = CQUtil::makeWidget<CQRealSpin>("deltaBucketEdit");
  numBucketsEdit_  = CQUtil::makeWidget<CQIntegerSpin>("numBucketsEdit");
  bucketStopsEdit_ = CQUtil::makeWidget<QLineEdit>("bucketStopsEdit");
  uniqueCount_     = CQUtil::makeWidget<CQLabel>("uniqueCount");
  rangeLabel_      = CQUtil::makeWidget<QLabel>("rangeLabel");

  bucketRange_    ->setToolTip("Bucket Range");
  startBucketEdit_->setToolTip("Bucket Start");
  deltaBucketEdit_->setToolTip("Bucket Delta");
  numBucketsEdit_ ->setToolTip("Number of Buckets");
  bucketStopsEdit_->setToolTip("Bucket Stops");
  uniqueCount_    ->setToolTip("Number of Unique Values");
  rangeLabel_     ->setToolTip("Value Range");

  addFrameWidget(bucketFrame, "Range"      , bucketRange_);
  addFrameWidget(bucketFrame, "Start"      , startBucketEdit_);
  addFrameWidget(bucketFrame, "Delta"      , deltaBucketEdit_);
  addFrameWidget(bucketFrame, "Num Buckets", numBucketsEdit_);
  addFrameWidget(bucketFrame, "Stops"      , bucketStopsEdit_);
  addFrameWidget(bucketFrame, "Num Unique" , uniqueCount_);
  addFrameWidget(bucketFrame, "Value Range", rangeLabel_);

  //---

  addGroupColumnWidgets();

  //--

  addColorColumnWidgets();

  //---

  addKeyList();
}

void
CQChartsDistributionPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  //---

  orientationCombo_ = createEnumEdit("orientation");
  plotTypeCombo_    = createEnumEdit("plotType");
  valueTypeCombo_   = createEnumEdit("valueType");

  addFrameWidget(optionsFrame_, "Orientation", orientationCombo_);
  addFrameWidget(optionsFrame_, "Plot Type"  , plotTypeCombo_);
  addFrameWidget(optionsFrame_, "Value Type" , valueTypeCombo_);

  //addFrameRowStretch(optionsFrame_);
}

void
CQChartsDistributionPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsDistributionPlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsDistributionPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  orientationCombo_->setCurrentValue(static_cast<int>(plot_->orientation()));
  plotTypeCombo_   ->setCurrentValue(static_cast<int>(plot_->plotType()));
  valueTypeCombo_  ->setCurrentValue(static_cast<int>(plot_->valueType()));

  //---

  bool isUnique = plot_->isExactBucketValue();
  bool isString = (plot_->bucketType() == CQBucketer::Type::STRING);
  bool isFixed  = (plot_->bucketType() == CQBucketer::Type::REAL_RANGE);
  bool isAuto   = (plot_->bucketType() == CQBucketer::Type::REAL_AUTO);
  bool isStops  = (plot_->bucketType() == CQBucketer::Type::FIXED_STOPS);

  if      (! isUnique && isFixed) fixedBucketRadio_ ->setChecked(true);
  else if (! isUnique && isAuto ) rangeBucketRadio_ ->setChecked(true);
  else if (! isUnique && isStops) stopsBucketRadio_ ->setChecked(true);
  else if (isUnique             ) uniqueBucketRadio_->setChecked(true);

  fixedBucketRadio_->setEnabled(! isString);
  rangeBucketRadio_->setEnabled(! isString);
  stopsBucketRadio_->setEnabled(! isString);

  double rmin, rmax;

  plot_->calcMinMaxBucketValue(rmin, rmax);

  bucketRange_    ->setRangeMinMax(rmin, rmax);
  startBucketEdit_->setValue(plot_->startBucketValue());
  deltaBucketEdit_->setValue(plot_->deltaBucketValue());
  numBucketsEdit_ ->setValue(plot_->numAutoBuckets());
  bucketStopsEdit_->setText(plot_->bucketStops().toString());
  uniqueCount_    ->setValue(plot_->numUniqueValues());
  rangeLabel_     ->setText(QString("%1-%2").arg(plot_->minBucketValue()).
                                             arg(plot_->maxBucketValue()));

  setFrameWidgetVisible(bucketRange_    , ! isUnique && isAuto);
  setFrameWidgetVisible(startBucketEdit_, ! isUnique && isFixed);
  setFrameWidgetVisible(deltaBucketEdit_, ! isUnique && isFixed);
  setFrameWidgetVisible(numBucketsEdit_ , ! isUnique && isAuto);
  setFrameWidgetVisible(bucketStopsEdit_, ! isUnique && isStops);
  setFrameWidgetVisible(uniqueCount_    , isUnique || isString);
  setFrameWidgetVisible(rangeLabel_     , isUnique || isFixed || isStops);

  //---

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsDistributionPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::connectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    plotTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(plotTypeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    valueTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(valueTypeSlot()));

  CQChartsWidgetUtil::connectDisconnect(b,
    bucketRadioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
    this, SLOT(bucketRadioGroupSlot(QAbstractButton *)));
  CQChartsWidgetUtil::connectDisconnect(b,
    bucketRange_, SIGNAL(sliderRangeChanged(double, double)), this, SLOT(bucketRangeSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    startBucketEdit_, SIGNAL(valueChanged(double)), this, SLOT(startBucketSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    deltaBucketEdit_, SIGNAL(valueChanged(double)), this, SLOT(deltaBucketSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    numBucketsEdit_, SIGNAL(valueChanged(int)), this, SLOT(numBucketsSlot()));
  CQChartsWidgetUtil::connectDisconnect(b,
    bucketStopsEdit_, SIGNAL(editingFinished()), this, SLOT(bucketStopsSlot()));

  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsDistributionPlotCustomControls::
orientationSlot()
{
  plot_->setOrientation(static_cast<Qt::Orientation>(orientationCombo_->currentValue()));
}

void
CQChartsDistributionPlotCustomControls::
plotTypeSlot()
{
  plot_->setPlotType(static_cast<CQChartsDistributionPlot::PlotType>(
                      plotTypeCombo_->currentValue()));
}

void
CQChartsDistributionPlotCustomControls::
valueTypeSlot()
{
  plot_->setValueType(static_cast<CQChartsDistributionPlot::ValueType>(
                       valueTypeCombo_->currentValue()));
}

void
CQChartsDistributionPlotCustomControls::
bucketRadioGroupSlot(QAbstractButton *button)
{
  if      (button == fixedBucketRadio_)
    plot_->setBucketType(CQBucketer::Type::REAL_RANGE);
  else if (button == rangeBucketRadio_)
    plot_->setBucketType(CQBucketer::Type::REAL_AUTO);
  else if (button == stopsBucketRadio_)
    plot_->setBucketType(CQBucketer::Type::FIXED_STOPS);
  else if (button == uniqueBucketRadio_) {
    if (plot_->bucketType() != CQBucketer::Type::STRING)
      plot_->setExactBucketValue(true);
  }
}

void
CQChartsDistributionPlotCustomControls::
bucketRangeSlot()
{
  connectSlots(false);

  setUpdatesEnabled(false);

  if (bucketRange_->sliderMin() != bucketRange_->rangeMin())
    plot_->setUnderflowBucket(CQChartsOptReal(bucketRange_->sliderMin()));
  else
    plot_->setUnderflowBucket(CQChartsOptReal());

  if (bucketRange_->sliderMax() != bucketRange_->rangeMax())
    plot_->setOverflowBucket(CQChartsOptReal(bucketRange_->sliderMax()));
  else
    plot_->setOverflowBucket(CQChartsOptReal());

  setUpdatesEnabled(true);

  connectSlots(true);

  updateWidgets();
}

void
CQChartsDistributionPlotCustomControls::
startBucketSlot()
{
  plot_->setStartBucketValue(startBucketEdit_->value());
}

void
CQChartsDistributionPlotCustomControls::
deltaBucketSlot()
{
  plot_->setDeltaBucketValue(deltaBucketEdit_->value());
}

void
CQChartsDistributionPlotCustomControls::
numBucketsSlot()
{
  plot_->setNumAutoBuckets(numBucketsEdit_->value());
}

void
CQChartsDistributionPlotCustomControls::
bucketStopsSlot()
{
  plot_->setBucketStops(CQChartsReals(bucketStopsEdit_->text()));
}

CQChartsColor
CQChartsDistributionPlotCustomControls::
getColorValue()
{
  return plot_->barFillColor();
}

void
CQChartsDistributionPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setBarFillColor(c);
}
