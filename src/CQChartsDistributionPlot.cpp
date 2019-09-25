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
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <QMenu>
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

  // values
  addColumnsParameter("value", "Value", "valueColumns").
    setRequired().setNumeric().setTip("Value column(s)");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Custom group name");

  addColumnParameter("data", "Data", "dataColumn").
    setTip("Extra data column");

  // bucket
  addBoolParameter("bucketed", "Bucketed", "bucketed", true).
   setTip("Bucket grouped values");

  addBoolParameter("autoBucket", "Auto Bucket", "autoBucket", true).
   setTip("Automatically determine bucket ranges");

  addRealParameter("startBucketValue", "Start Value", "startBucketValue", 0.0).
    setRequired().setTip("Start value for manual bucket");
  addRealParameter("deltaBucketValue", "Delta Value", "deltaBucketValue", 1.0).
    setRequired().setTip("Delta value for manual bucket");

  addIntParameter("numAutoBuckets", "Num Auto Buckets", "numAutoBuckets", 10).
    setRequired().setTip("Number of automatic buckets");

  // options
  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("Draw bars horizontal");

  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("NORMAL"      , int(CQChartsDistributionPlot::PlotType::NORMAL      )).
    addNameValue("STACKED"     , int(CQChartsDistributionPlot::PlotType::STACKED     )).
    addNameValue("SIDE_BY_SIDE", int(CQChartsDistributionPlot::PlotType::SIDE_BY_SIDE)).
    addNameValue("OVERLAY"     , int(CQChartsDistributionPlot::PlotType::OVERLAY     )).
    addNameValue("SCATTER"     , int(CQChartsDistributionPlot::PlotType::SCATTER     )).
    addNameValue("DENSITY"     , int(CQChartsDistributionPlot::PlotType::DENSITY     )).
    setTip("Plot type");

  addEnumParameter("valueType", "Value Type", "valueType").
   addNameValue("COUNT", int(CQChartsDistributionPlot::ValueType::COUNT)).
   addNameValue("RANGE", int(CQChartsDistributionPlot::ValueType::RANGE)).
   addNameValue("MIN"  , int(CQChartsDistributionPlot::ValueType::MIN  )).
   addNameValue("MAX"  , int(CQChartsDistributionPlot::ValueType::MAX  )).
   addNameValue("MEAN" , int(CQChartsDistributionPlot::ValueType::MEAN )).
   addNameValue("SUM"  , int(CQChartsDistributionPlot::ValueType::SUM  )).
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
     p("The values to be counted are taken from the " + B("Value") + " columns and "
       "grouped depending on the column value type. By default integer values are "
       "grouped by matching value, real values are automatically bucketed into ranges "
       "and strings are grouped by matching value.").
     p("The automatic bucketing of real values can overridden by turning it off using "
       "the " + B("autoBucket") + " parameter and specifying the " + B("startBucketValue") +
       " and " + B("deltaBucketValue") + " parameters.").
     p("The color of the bar can be overridden using the " + B("Color") + " column.").
    h3("Options").
     p("Enabling the " + B("Horizontal") + " otpions draws the bars horizontally "
       "of vertically.").
    h3("Grouping").
     p("Standard grouping can be applied to the values to split the values to be "
       "grouped into individual value sets before final grouping. This second level "
       "if grouping creates multiple sets of grouped values which can be displayed "
       "sequentially or overlaid with common buckets.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/distribution.png"));
}

CQChartsPlot *
CQChartsDistributionPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsDistributionPlot(view, model);
}

//------

CQChartsDistributionPlot::
CQChartsDistributionPlot(CQChartsView *view, const ModelP &model) :
 CQChartsBarPlot(view, view->charts()->plotType("distribution"), model),
 CQChartsObjStatsLineData<CQChartsDistributionPlot>(this),
 CQChartsObjDotPointData <CQChartsDistributionPlot>(this),
 CQChartsObjRugPointData <CQChartsDistributionPlot>(this)
{
  NoUpdate noUpdate(this);

  setAutoBucket    (true);
  setNumAutoBuckets(20);

  setStatsLines(false);
  setStatsLinesDash(CQChartsLineDash(CQChartsLineDash::Lengths({2, 2}), 0));

  setDotSymbolType     (CQChartsSymbol::Type::CIRCLE);
  setDotSymbolSize     (CQChartsLength("7px"));
  setDotSymbolFilled   (true);
  setDotSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setRugSymbolType       (CQChartsSymbol::Type::NONE);
  setRugSymbolSize       (CQChartsLength("5px"));
  setRugSymbolStroked    (true);
  setRugSymbolStrokeColor(CQChartsColor(CQChartsColor::Type::PALETTE));
}

CQChartsDistributionPlot::
~CQChartsDistributionPlot()
{
  clearGroupValues();
}

//---

void
CQChartsDistributionPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDataColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(dataColumn_, c, [&]() { updateRangeAndObjs(); } );
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
  return (bucketer_.type() == CQBucketer::Type::REAL_AUTO);
}

void
CQChartsDistributionPlot::
setAutoBucket(bool b)
{
  bucketer_.setType(b ? CQBucketer::Type::REAL_AUTO : CQBucketer::Type::REAL_RANGE);

  for (auto &ib : groupData_.groupBucketer)
    ib.second.setType(bucketer_.type());

  updateRangeAndObjs();
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
  bucketer_.setRStart(r);

  for (auto &ib : groupData_.groupBucketer)
    ib.second.setRStart(r);

  updateRangeAndObjs();
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
  bucketer_.setRDelta(r);

  for (auto &ib : groupData_.groupBucketer)
    ib.second.setRDelta(r);

  updateRangeAndObjs();
}

int
CQChartsDistributionPlot::
numAutoBuckets() const
{
  return bucketer_.numAuto();
}

void
CQChartsDistributionPlot::
setNumAutoBuckets(int i)
{
  bucketer_.setNumAuto(i);

  for (auto &ib : groupData_.groupBucketer)
    ib.second.setNumAuto(i);

  updateRangeAndObjs();
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  CQChartsBarPlot::addProperties();

  // columns
  addProp("columns", "nameColumn", "name", "Custom group name column");
  addProp("columns", "dataColumn", "data", "Extra data column");

  // bucketing
  addProp("bucket", "bucketed"        , "enabled", "Bucket grouped values");
  addProp("bucket", "autoBucket"      , "auto"   , "Automatically determine bucket ranges");
  addProp("bucket", "startBucketValue", "start"  , "Start value for manual bucket");
  addProp("bucket", "deltaBucketValue", "delta"  , "Delta value for manual bucket");
  addProp("bucket", "numAutoBuckets"  , "num"    , "Number of auto buckets");

  addProp("bucket", "underflowBucket", "underflow", "Underflow bucket threshhold");
  addProp("bucket", "overflowBucket" , "overflow" , "Overflow bucket threshhold");

  // options
  addProp("options", "plotType" , "plotType" , "Plot type");
  addProp("options", "valueType", "valueType", "Bar value type");

  addProp("options", "percent"   , "", "Show value as percentage");
  addProp("options", "skipEmpty" , "", "Skip empty buckets");
  addProp("options", "sorted"    , "", "Sort by count");
  addProp("options", "minBarSize", "", "Minimum bar size")->setHidden(true);

  // density
  addProp("density", "density"        , "visible" , "Show density plot");
  addProp("density", "densityOffset"  , "offset"  , "Density plot offset");
  addProp("density", "densitySamples" , "samples" , "Density samples");
  addProp("density", "densityGradient", "gradient", "Drag density gradient");
  addProp("density", "densityBars"    , "bars"    , "Draw density bars");

  // scatter
  addProp("scatter", "scatter"      , "visible", "Draw scatter points");
  addProp("scatter", "scatterFactor", "factor" , "Scatter factor (0-1)");

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

  CQChartsGroupPlot::addProperties();
}

//---

void
CQChartsDistributionPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setNormal(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setStacked(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::STACKED, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setSideBySide(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::SIDE_BY_SIDE, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setOverlay(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::OVERLAY, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
}

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
  CQChartsUtil::testAndSet(valueType_, type, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueCount(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueRange(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::RANGE, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueMin(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MIN, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueMax(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MAX, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueMean(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MEAN, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueSum(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::SUM, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { updateRangeAndObjs(); } );
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
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::DENSITY, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
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
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::SCATTER, [&]() { updateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { updateRangeAndObjs(); } );
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
setDotLineWidth(const CQChartsLength &l)
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
setUnderflowBucket(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(underflowBucket_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setOverflowBucket(const CQChartsOptReal &r)
{
  CQChartsUtil::testAndSet(overflowBucket_, r, [&]() { updateRangeAndObjs(); } );
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

  bucketGroupValues();

  return calcBucketRanges();
}

void
CQChartsDistributionPlot::
bucketGroupValues() const
{
  CQPerfTrace trace("CQChartsDistributionPlot::bucketGroupValues");

  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  // initialize bucketers to value range
  if (isConsistentBucketer()) {
    // get consistent bucketer
    CQBucketer &bucketer = th->groupBucketer(0);

    //---

    // get consistent type
    CQChartsValueSet::Type type = CQChartsValueSet::Type::NONE;

    for (auto &groupValues : groupData_.groupValues) {
      Values *values = groupValues.second;

      CQChartsValueSet::Type type1 = values->valueSet->type();

      if (type == CQChartsValueSet::Type::NONE)
        type = type1;

      if (type1 == type)
        continue;

      if      (type1 == CQChartsValueSet::Type::STRING) {
        type = type1;
      }
      else if (type1 == CQChartsValueSet::Type::REAL) {
        if (type == CQChartsValueSet::Type::INTEGER)
          type = type1;
      }
    }

    //---

    // update bucketer with all values
    int iv = 0;

    for (auto &groupValues : groupData_.groupValues) {
      Values *values = groupValues.second;

      if      (type == CQChartsValueSet::Type::INTEGER) {
        if (iv == 0) {
          bucketer.setIntegral(true);
          bucketer.setIMin    (values->valueSet->imin());
          bucketer.setIMax    (values->valueSet->imax());
        }
        else {
          bucketer.setIMin(std::min(bucketer.imin(), values->valueSet->imin()));
          bucketer.setIMax(std::max(bucketer.imax(), values->valueSet->imax()));
        }
      }
      else if (type == CQChartsValueSet::Type::REAL) {
        if (iv == 0) {
          bucketer.setIntegral(false);
          bucketer.setRMin    (values->valueSet->rmin());
          bucketer.setRMax    (values->valueSet->rmax());
        }
        else {
          bucketer.setRMin(std::min(bucketer.rmin(), values->valueSet->rmin()));
          bucketer.setRMax(std::max(bucketer.rmax(), values->valueSet->rmax()));
        }
      }

      ++iv;
    }
  }
  else {
    // init each group bucketer from value ranges
    for (auto &groupValues : groupData_.groupValues) {
      int     groupInd = groupValues.first;
      Values *values   = groupValues.second;

      // init group bucketer
      CQBucketer &bucketer = th->groupBucketer(groupInd);

      //---

      // set bucketer range
      CQChartsValueSet::Type type = values->valueSet->type();

      if      (type == CQChartsValueSet::Type::INTEGER) {
        bucketer.setIntegral(true);
        bucketer.setIMin    (values->valueSet->imin());
        bucketer.setIMax    (values->valueSet->imax());
      }
      else if (type == CQChartsValueSet::Type::REAL) {
        bucketer.setIntegral(false);
        bucketer.setRMin    (values->valueSet->rmin());
        bucketer.setRMax    (values->valueSet->rmax());
      }
    }
  }

  //---

  // bucket grouped sets of values
  for (auto &groupValues : groupData_.groupValues) {
    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    //---

    // add each index to associated bucket
    for (auto &ind : values->inds) {
      Bucket   bucket;
      QVariant value;

      //---

      bool ok;

      if (isBucketed()) {
        CQChartsValueSet::Type type = values->valueSet->type();

        if      (type == CQChartsValueSet::Type::REAL) {
          double r = modelReal(ind, ok);
          if (! ok || CMathUtil::isNaN(r)) continue;

          if (! isIncludeOutlier()) {
            bool outlier = values->valueSet->rvals().isOutlier(r);
            if (outlier) continue;
          }

          bucket = calcBucket(groupInd, r);
          value  = QVariant(r);
        }
        else if (type == CQChartsValueSet::Type::INTEGER) {
          int i = modelInteger(ind, ok);
          if (! ok) continue;

          if (! isIncludeOutlier()) {
            bool outlier = values->valueSet->ivals().isOutlier(i);
            if (outlier) continue;
          }

          bucket = calcBucket(groupInd, i);
          value  = QVariant(i);
        }
        else {
          bool hierValue = isHierarchical();

          QString str;

          if (hierValue) {
            QVariant value = modelRootValue(ind.row, ind.column, ind.parent, Qt::DisplayRole, ok);

            str = value.toString();
          }
          else {
            str = modelString(ind, ok);
          }

          if (! ok) continue;

          CQBucketer &bucketer = th->groupBucketer(groupInd);

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
        CQChartsModelIndex dind(ind.row, dataColumn(), ind.parent);

        bool ok;

        dvalue = modelValue(dind, ok);
      }

      //---

      VariantInd varInd(value, ind, dvalue);

      values->bucketValues[bucket].inds.push_back(varInd);
    }
  }
}

CQChartsGeom::Range
CQChartsDistributionPlot::
calcBucketRanges() const
{
  CQPerfTrace trace("CQChartsDistributionPlot::calcBucketRanges");

  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  //---

  bool isStackedActive    = false;
  bool isOverlayActive    = false;
  bool isSideBySideActive = false;

  if (hasGroups()) {
    isStackedActive    = isStacked();
    isOverlayActive    = isOverlay();
    isSideBySideActive = isSideBySide();
  }

  //---

  // calc range (number of bars and max height)

  int i2 = 0;

  CQChartsGeom::IMinMax nRange(0);
  CQChartsGeom::IMinMax bucketRange;
  CQChartsGeom::RMinMax valueRange;
  CQChartsGeom::BBox    densityBBox;

  th->groupData_.groupBucketRange.clear();

  double doffset = 0.0;

  int ig = 0;
  int ng = groupData_.groupValues.size();

  int maxValues = 0;

  using BucketTotals = std::map<Bucket,double>;

  BucketTotals valueSetRunningTotal;

  for (auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    int maxValues1 = 0;

    //---

    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    // density curve per group (optionally offset)
    if      (isDensity()) {
      values->densityData->setNumSamples(densitySamples());

      std::vector<double> xvals;

      (void) getRealValues(groupInd, xvals, values->statData);

      values->densityData->setXVals(xvals);

      values->densityData->calc();

      if (! isHorizontal()) {
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
        const Bucket    &bucket   = bucketValues.first;
        VariantIndsData &varsData = bucketValues.second;

        //---

        double value1, value2;

        this->bucketValues(groupInd, bucket, value1, value2);

        values->xValueRange.add(value1);
        values->xValueRange.add(value2);

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
        int n = varsData.inds.size();

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
  CQChartsGeom::Range dataRange;

  auto updateRange = [&](double x, double y) {
    if (! isHorizontal())
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
        int nv = valueSetRunningTotal.size();

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
        int nv = valueSetRunningTotal.size();

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
        int nv = valueSetRunningTotal.size();

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
          updateRange2(-1.0, n1, nb, n2);
      }
      else {
        if (isPercent())
          updateRange2(-1.0, 0, i2, 1);
        else
          updateRange2(-1.0, 0, i2, n2);
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
  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  for (auto &groupValues : th->groupData_.groupValues)
    delete groupValues.second;

  th->groupData_.groupValues.clear();

  th->groupData_.groupBucketer.clear();
}

void
CQChartsDistributionPlot::
addRow(const ModelVisitor::VisitData &data) const
{
  for (const auto &valueColumn : valueColumns())
    addRowColumn(CQChartsModelIndex(data.row, valueColumn, data.parent));
}

void
CQChartsDistributionPlot::
addRowColumn(const CQChartsModelIndex &ind) const
{
  // get optional group for value
  int groupInd = rowGroupInd(ind);

  bool ok;

  QVariant value = modelValue(ind, ok);

  //---

  // check push/pop filter
  if (ok && ! checkFilter(groupInd, value))
    return;

  //---

  // get value set for group
  Values *values = getGroupIndValues(groupInd, ind);

  //---

  // add value
  if (ok)
    values->valueSet->addValue(value);

  values->inds.push_back(ind);
}

CQChartsDistributionPlot::Values *
CQChartsDistributionPlot::
getGroupIndValues(int groupInd, const CQChartsModelIndex &ind) const
{
  Values *values = const_cast<Values *>(getGroupValues(groupInd));

  if (values)
    return values;

  //---

  std::unique_lock<std::mutex> lock(mutex_);

  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  CQChartsValueSet *valueSet = new CQChartsValueSet(this);

  valueSet->setColumn(ind.column);

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
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  // add data labels
  CQChartsDataLabel::Position position = dataLabel()->position();

  if (position == CQChartsDataLabel::TOP_OUTSIDE ||
      position == CQChartsDataLabel::BOTTOM_OUTSIDE) {
    for (const auto &plotObj : plotObjs_) {
      CQChartsDistributionBarObj *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      if (barObj)
        bbox += barObj->dataLabelRect();
    }
  }

  // add rug symbols
  if (isRug()) {
    double sx, sy;

    plotSymbolSize(rugSymbolSize(), sx, sy);

    const CQChartsGeom::Range &dataRange = this->dataRange();

    QPointF p1, p2;

    if (! isHorizontal()) {
      p1 = QPointF(dataRange.xmin(), dataRange.ymin()       );
      p2 = QPointF(dataRange.xmax(), dataRange.ymin() - 2*sy);
    }
    else {
      p1 = QPointF(dataRange.xmin()       , dataRange.ymin());
      p2 = QPointF(dataRange.xmin() - 2*sx, dataRange.ymax());
    }

    bbox += CQChartsGeom::Point(p1);
    bbox += CQChartsGeom::Point(p2);
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

  const Filters &filters = filterStack_.back();

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
  if (underflowBucket_.isSet() && value < underflowBucket_.real())
    return Bucket(Bucket::Type::UNDERFLOW);

  if (overflowBucket_.isSet() && value > overflowBucket_.real())
    return Bucket(Bucket::Type::OVERFLOW);

  //---

  int num = -1;

  const CQBucketer &bucketer = groupBucketer(groupInd);

  if (filterStack_.empty()) {
    if (! isBucketed())
      return Bucket(-1);

    if (isAutoBucket())
      num = bucketer.autoRealBucket(value);
    else
      num = bucketer.realBucket(value);
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

  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  NoUpdate noUpdate(this);

  //---

  bool isStackedActive    = false;
  bool isOverlayActive    = false;
  bool isSideBySideActive = false;

  if (hasGroups()) {
    isStackedActive    = isStacked();
    isOverlayActive    = isOverlay();
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
  const CQChartsGeom::Range &dataRange = this->dataRange();

  double size = dataRange.size(! isHorizontal());

  int inc = intIncrementForSize(size);
#endif

  if      (isDensity()) {
    valueAxis()->setValueType       (CQChartsAxisValueType::Type::REAL, /*notify*/false);
    valueAxis()->setGridMid         (false);
    valueAxis()->setMajorIncrement  (0);
    valueAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR_AND_MINOR);
    valueAxis()->setRequireTickLabel(false);

    countAxis()->setValueType       (CQChartsAxisValueType::Type::REAL, /*notify*/false);
    countAxis()->setGridMid         (false);
    countAxis()->setMajorIncrement  (0);
    countAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR_AND_MINOR);
    countAxis()->setRequireTickLabel(false);
  }
  else if (isScatter()) {
    valueAxis()->setValueType       (CQChartsAxisValueType::Type::REAL, /*notify*/false);
    valueAxis()->setGridMid         (false);
    valueAxis()->setMajorIncrement  (0);
    valueAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    valueAxis()->setRequireTickLabel(false);

    if (isValueCount())
      countAxis()->setValueType(CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
    else
      countAxis()->setValueType(CQChartsAxisValueType::Type::REAL, /*notify*/false);

    countAxis()->setGridMid         (false);
    countAxis()->setMajorIncrement  (0);
    countAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    countAxis()->setRequireTickLabel(false);
  }
  else {
    valueAxis()->setValueType       (CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
    valueAxis()->setGridMid         (true);
    valueAxis()->setMajorIncrement  (1);
    valueAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    valueAxis()->setRequireTickLabel(true);

    if (! isLogY()) {
      if (isValueCount())
        countAxis()->setValueType(CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
      else
        countAxis()->setValueType(CQChartsAxisValueType::Type::REAL, /*notify*/false);
    }
    else
      countAxis()->setValueType(CQChartsAxisValueType::Type::LOG, /*notify*/false);

    countAxis()->setGridMid         (false);
    countAxis()->setMajorIncrement  (0);
    countAxis()->setTicksDisplayed  (CQChartsAxis::TicksDisplayed::MAJOR);
    countAxis()->setRequireTickLabel(false);
  }

  //---

  int gpos = 0;
  int vpos = 0;

  int ng = groupData_.groupValues.size();

  int offset = 0;
  int count  = 0;

  double doffset = 0.0;

  using BucketTotals = std::map<Bucket,double>;

  BucketTotals groupMax, valueSetRunningTotal, valueSetTotals;

  th->barWidth_ = 1.0;

  if (isSideBySideActive) {
    assert(ng > 0);

    th->barWidth_ /= ng;
  }

  //---

  // get sorted buckets if needed
  using Buckets           = std::vector<Bucket>;
  using CountBuckets      = std::map<int,Buckets>;
  using GroupCountBuckets = std::map<int,CountBuckets>;
  using GroupBuckets      = std::map<int,Buckets>;

  GroupCountBuckets groupCountBuckets;
  GroupBuckets      groupSortedBuckets;

  if (isSorted()) {
    if (isStackedActive || isOverlayActive || isSideBySideActive) {
      // TODO
    }
    else {
      for (auto &groupValues : groupData_.groupValues) {
        int           groupInd = groupValues.first;
        const Values *values   = groupValues.second;

        for (const auto &bucketValues : values->bucketValues) {
          const Bucket          &bucket   = bucketValues.first;
          const VariantIndsData &varsData = bucketValues.second;

          BarValue barValue = varIndsValue(varsData);

          int n = barValue.n2;

          groupCountBuckets[groupInd][n].push_back(bucket);
        }
      }

      for (const auto &gcb : groupCountBuckets) {
        int                 groupInd     = gcb.first;
        const CountBuckets &countBuckets = gcb.second;

        for (const auto &cb : countBuckets) {
          const Buckets &buckets = cb.second;

          for (auto &bucket : buckets)
            groupSortedBuckets[groupInd].push_back(bucket);
        }
      }
    }
  }

  //---

  using BucketInd = std::map<Bucket,int>;

  BucketInd bucketInd;

  for (auto &groupValues : groupData_.groupValues) {
    const Values *values = groupValues.second;

    for (const auto &bucketValues : values->bucketValues) {
      const Bucket &bucket = bucketValues.first;

      bucketInd[bucket] = 0;
    }

    for (const auto &bucketValues : values->bucketValues) {
      const Bucket          &bucket   = bucketValues.first;
      const VariantIndsData &varsData = bucketValues.second;

      int n = varsData.inds.size();

      if (isSkipEmpty() && n == 0)
        continue;

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

      const Values *values = groupValues.second;

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

        const Bucket          &bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        //---

        const VariantIndsData *pVarsData = &varsData;

        BarValue barValue = varIndsValue(*pVarsData);

        valueSetTotals[bucket] += barValue.n2 - barValue.n1;

        if (barValue.n2 > groupMax[bucket])
          groupMax[bucket] = barValue.n2;

        ++iv;
      }

      ++ig;
    }
  }

  //---

  auto makeBBox = [&](double xmin, double ymin, double xmax, double ymax) {
    if (! isHorizontal())
      return CQChartsGeom::BBox(xmin, ymin, xmax, ymax);
    else
      return CQChartsGeom::BBox(ymin, xmin, ymax, xmax);
  };

  //---

  int ig = 0;

  for (auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int           groupInd = groupValues.first;
    const Values *values   = groupValues.second;

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
        const Bucket          &bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        int n = varsData.inds.size();

        data.buckets.emplace_back(bucket, n);
      }

      //---

      CQChartsGeom::BBox bbox;

      if (! isHorizontal()) {
        bbox.add(data.xmin, data.ymin + doffset);
        bbox.add(data.xmax, data.ymax + doffset);
      }
      else {
        bbox.add(data.ymin + doffset, data.xmin);
        bbox.add(data.ymax + doffset, data.xmin);
      }

      if (bbox.isSet()) {
        CQChartsDistributionDensityObj *barObj =
          new CQChartsDistributionDensityObj(this, bbox, groupInd, data, doffset, ColorInd(ig, ng));

        objs.push_back(barObj);
      }

      doffset += densityOffset();
    }
    else if (isScatter()) {
      int iv = 0;
      int nv = values->bucketValues.size();

      for (auto &bucketValues : values->bucketValues) {
        const Bucket          &bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        //---

        const VariantIndsData *pVarsData = &varsData;

        Bucket sbucket = bucket;

        if (isSorted()) {
          if (isStackedActive || isOverlayActive || isSideBySideActive) {
            // TODO
          }
          else {
            const Buckets &sortedBuckets = getSortedBuckets(groupInd);

            sbucket = sortedBuckets[iv];

            auto p = values->bucketValues.find(sbucket);
            assert(p != values->bucketValues.end());

            const VariantIndsData &varsData1 = (*p).second;

            pVarsData = &varsData1;
          }
        }

        //---

        int n = pVarsData->inds.size();

        CQChartsGeom::BBox bbox = makeBBox(ig - 0.5, iv - 0.5, ig + 0.5, iv + 0.5);

        CQChartsDistributionScatterObj *scatterObj =
          new CQChartsDistributionScatterObj(this, bbox, groupInd, sbucket, n,
                                             ColorInd(ig, ng), ColorInd(iv, nv));

        objs.push_back(scatterObj);

        QString bucketName = bucketStr(groupInd, bucket);

        valueAxis()->setTickLabel(iv, bucketName);

        ++iv;
      }

      QString groupName = groupIndName(groupInd);

      countAxis()->setTickLabel(ig, groupName);
    }
    else {
      auto pb = groupData_.groupBucketRange.find(groupInd);

      if (groupData_.groupBucketRange.empty())
        continue;

      const CQChartsGeom::IMinMax &bucketRange = (*pb).second;

      int bucketMin = bucketRange.min(0);
      int bucketMax = bucketRange.max(0);

      if (! isOverlayActive)
        offset = -bucketMin;

      //---

      bool isNumeric = values->valueSet->isNumeric();

      //---

      int iv = 0;
      int nv = values->bucketValues.size();

      for (auto &bucketValues : values->bucketValues) {
        if (! colorColumn().isValid()) {
          if (ng == 1 && isSetHidden(iv)) { ++iv; continue; }
        }

        //---

        const Bucket          &bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        //---

        double value1, value2;

        this->bucketValues(groupInd, bucket, value1, value2);

        //---

        const VariantIndsData *pVarsData = &varsData;

        Bucket sbucket = bucket;

        if (isSorted()) {
          if (isStackedActive || isOverlayActive || isSideBySideActive) {
            // TODO
          }
          else {
            const Buckets &sortedBuckets = getSortedBuckets(groupInd);

            sbucket = sortedBuckets[iv];

            auto p = values->bucketValues.find(sbucket);
            assert(p != values->bucketValues.end());

            const VariantIndsData &varsData1 = (*p).second;

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

        BarValue barValue = varIndsValue(*pVarsData);

        //---

        double scale = 1.0;

        if (isPercent()) {
          if (isStackedActive || isOverlayActive || isSideBySideActive) {
            double valueSetTotal = valueSetTotals[bucket];

            if (valueSetTotal > 0)
              scale = 1.0/valueSetTotal;
          }
          else {
            int max = groupMax[bucket];

            if (max > 0)
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

        CQChartsGeom::BBox bbox;

        if      (isStackedActive) {
          double total = valueSetRunningTotal[bucket];

          double v1 = (barValue.n1 + total)*scale;
          double v2 = (barValue.n2 + total)*scale;

          if (! isSkipEmpty())
            bbox = makeBBox(bucketValue - 0.5, v1, bucketValue + 0.5, v2);
          else
            bbox = makeBBox(vpos1 - 0.5, v1, vpos1 + 0.5, v2);
        }
        else if (isOverlayActive) {
          double v1 = barValue.n1*scale;
          double v2 = barValue.n2*scale;

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

          bbox = makeBBox(tpos1, v1, tpos2, v2);
        }
        else {
          double v1 = barValue.n1*scale;
          double v2 = barValue.n2*scale;

          if (! isSkipEmpty())
            bbox = makeBBox(bucket1 - 0.5, v1, bucket1 + 0.5, v2);
          else
            bbox = makeBBox(vpos - 0.5, v1, vpos + 0.5, v2);
        }

        barValue.xr = CQChartsGeom::RangeValue(value1,
          values->xValueRange.min(), values->xValueRange.max());
        barValue.yr = CQChartsGeom::RangeValue(barValue.n2,
          values->yValueRange.min(), values->yValueRange.max());

        CQChartsDistributionBarObj *barObj =
          new CQChartsDistributionBarObj(this, bbox, groupInd, sbucket, barValue,
                                         ColorInd(ig, ng), ColorInd(iv, nv));

        objs.push_back(barObj);

        //---

        if      (! isBucketed()) {
          QString groupName = groupIndName(groupInd);

          valueAxis()->setTickLabel(bucket1, groupName);
        }
        else if (isNumeric) {
          int xm = CMathRound::RoundNearest(bbox.getXMid());

          if (valueAxis()->tickLabelPlacement() == CQChartsAxisTickLabelPlacement::Type::MIDDLE) {
            QString bucketStr = bucketValuesStr(groupInd, sbucket, values);

            if      (isStackedActive)
              valueAxis()->setTickLabel(xm, bucketStr);
            else if (isOverlayActive)
              valueAxis()->setTickLabel(xm, bucketStr);
            else if (isSideBySideActive)
              valueAxis()->setTickLabel(xm, bucketStr);
            else
              valueAxis()->setTickLabel(xm, bucketStr);
          }
          else {
            QString bucketStr1 = bucketValuesStr(groupInd, sbucket, values, BucketValueType::START);
            QString bucketStr2 = bucketValuesStr(groupInd, sbucket, values, BucketValueType::END  );

            if      (isStackedActive) {
              valueAxis()->setTickLabel(xm    , bucketStr1);
              valueAxis()->setTickLabel(xm + 1, bucketStr2);
            }
            else if (isOverlayActive) {
              valueAxis()->setTickLabel(xm    , bucketStr1);
              valueAxis()->setTickLabel(xm + 1, bucketStr2);
            }
            else if (isSideBySideActive) {
              valueAxis()->setTickLabel(xm    , bucketStr1);
              valueAxis()->setTickLabel(xm + 1, bucketStr2);
            }
            else {
              valueAxis()->setTickLabel(xm    , bucketStr1);
              valueAxis()->setTickLabel(xm + 1, bucketStr2);
            }
          }
        }
        else {
          QString bucketStr = bucketValuesStr(groupInd, sbucket, values, BucketValueType::START);

          int xm = CMathRound::RoundNearest(bbox.getXMid());

          if      (isStackedActive)
            valueAxis()->setTickLabel(xm, bucketStr);
          else if (isOverlayActive)
            valueAxis()->setTickLabel(xm, bucketStr);
          else if (isSideBySideActive)
            valueAxis()->setTickLabel(xm, bucketStr);
          else
            valueAxis()->setTickLabel(xm, bucketStr);
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
    valueAxis()->setLabel(valueColumnName(""));
  };

  auto setXGroupLabel = [&]() {
    valueAxis()->setLabel(valueColumnName(""));
  };

  auto setXGroupValuesLabel = [&]() {
    QStringList groupLabels;

    for (const auto &groupValues : groupData_.groupValues) {
      int groupInd = groupValues.first;

      QString groupName = groupIndName(groupInd);

      groupLabels.push_back(groupName);
    }

    valueAxis()->setLabel(groupLabels.join(", "));
  };

  // value axis label (x)
  valueAxis()->setLabel("");

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
    countAxis()->setLabel("Density");
  }
  else {
    auto setCountLabel = [&](const QString &label) {
      QString label1 = label;

      if (dataColumn().isValid()) {
        bool ok;

        QString header = modelHeaderString(dataColumn(), ok);

        if (ok && header.length())
          label1 += " (" + header + ")";
      }

      countAxis()->setLabel(label1);
    };

    if      (isPercent   ()) setCountLabel("Percent");
    else if (isValueCount()) setCountLabel("Count");
    else if (isValueRange()) setCountLabel("Range");
    else if (isValueMin  ()) setCountLabel("Min");
    else if (isValueMax  ()) setCountLabel("Min");
    else if (isValueMean ()) setCountLabel("Mean");
    else if (isValueSum  ()) setCountLabel("Sum");
  }

  if (yLabel().length())
    countAxis()->setLabel(yLabel());

  //---

  return true;
}

QString
CQChartsDistributionPlot::
valueColumnName(const QString &def) const
{
  if (xLabel().length())
    return xLabel();

  CQChartsColumn c = valueColumns().column();

  bool ok;

  QString name = modelHeaderString(c, ok);

  if (! ok)
    name = def;

  return name;
}

void
CQChartsDistributionPlot::
calcVarIndsData(VariantIndsData &varInds) const
{
  CQChartsGeom::RMinMax valueRange;

  CQChartsRValues rvals;

  int n = varInds.inds.size();

  for (int i = 0; i < n; ++i) {
    const VariantInd &var = varInds.inds[i];

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

  if      (isValueCount()) { barValue.n1 = 0          ; barValue.n2 = varInds.inds.size()  ; }
  else if (isValueRange()) { barValue.n1 = varInds.min; barValue.n2 = varInds.max          ; }
  else if (isValueMin  ()) { barValue.n1 = 0          ; barValue.n2 = varInds.min          ; }
  else if (isValueMax  ()) { barValue.n1 = 0          ; barValue.n2 = varInds.max          ; }
  else if (isValueMean ()) { barValue.n1 = 0          ; barValue.n2 = varInds.statData.mean; }
  else if (isValueSum  ()) { barValue.n1 = 0          ; barValue.n2 = varInds.statData.sum ; }

  return barValue;
}

void
CQChartsDistributionPlot::
getInds(int groupInd, const Bucket &bucket, VariantInds &inds) const
{
  const Values *values = getGroupValues(groupInd);
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
    const QVariant &var = vind.var;

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

  const Values *values = getGroupValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == CQBaseModelType::INTEGER) {
    const CQChartsIValues &ivals = values->valueSet->ivals();

    statData = ivals.statData();

    std::vector<double> xvals;

    for (int i = 0; i < ivals.size(); ++i)
      vals.push_back(*ivals.value(i));
  }
  else if (values->valueSet->type() == CQBaseModelType::REAL) {
    const CQChartsRValues &rvals = values->valueSet->rvals();

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
  const Values *values = getGroupValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == CQBaseModelType::INTEGER) {
    const CQChartsIValues &ivals = values->valueSet->ivals();

    statData = ivals.statData();
  }
  else if (values->valueSet->type() == CQBaseModelType::REAL) {
    const CQChartsRValues &rvals = values->valueSet->rvals();

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
  return (! isHorizontal() ? xAxis() : yAxis());
}

CQChartsAxis *
CQChartsDistributionPlot::
countAxis() const
{
  return (! isHorizontal() ? yAxis() : xAxis());
}

void
CQChartsDistributionPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int row = (! key->isHorizontal() ? key->maxRow() : 0);
  int col = (! key->isHorizontal() ? 0 : key->maxCol());

  auto addKeyRow = [&](const ColorInd &ig, const ColorInd &iv, const CQChartsGeom::RangeValue &xv,
                       const CQChartsGeom::RangeValue &yv, const QString &name) {
    CQChartsDistKeyColorBox *keyColor = new CQChartsDistKeyColorBox(this, ig, iv, xv, yv);
    CQChartsDistKeyText     *keyText  = new CQChartsDistKeyText    (this, name, iv);

    if (! key->isHorizontal()) {
      key->addItem(keyColor, row, 0);
      key->addItem(keyText , row, 1);

      ++row;
    }
    else {
      key->addItem(keyColor, 0, col++);
      key->addItem(keyText , 0, col++);
    }

    return std::pair<CQChartsDistKeyColorBox *,CQChartsDistKeyText*>(keyColor, keyText);
  };

  //---

  int ng = groupData_.groupValues.size();

  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupValues : groupData_.groupValues) {
      int           groupInd = groupValues.first;
    //const Values *values   = groupValues.second;

      QString groupName = groupIndName(groupInd);

      addKeyRow(ColorInd(ig, ng), ColorInd(), CQChartsGeom::RangeValue(),
                CQChartsGeom::RangeValue(), groupName);

      ++ig;
    }
  }
  else if (ng == 1) {
    if (colorColumn().isValid()) {
      bool ok;

      QString header = modelHeaderString(colorColumn(), ok);

      key->setHeaderStr(header);

      const CQChartsModelColumnDetails *columnDetails = this->columnDetails(colorColumn());

      int nv = (columnDetails ? columnDetails->numUnique() : 0);

      for (int iv = 0; iv < nv; ++iv) {
        QVariant value = columnDetails->uniqueValue(iv);

        CQChartsDistKeyColorBox *colorBox =
          addKeyRow(ColorInd(), ColorInd(iv, nv), CQChartsGeom::RangeValue(),
                    CQChartsGeom::RangeValue(), value.toString()).first;

        bool ok;

        CQChartsColor c = CQChartsVariant::toColor(value, ok);

        if (ok) {
          QColor c1 = interpColor(c, ColorInd());

          c1.setAlphaF(barFillAlpha());

          colorBox->setColor(c1);
        }
        else
          colorBox->setColor(c);
      }
    }
    else {
      auto pg = groupData_.groupValues.begin();

      int           groupInd = (*pg).first;
      const Values *values   = (*pg).second;

      int nv = values->bucketValues.size();

      int iv = 0;

      for (const auto &bucketValues : values->bucketValues) {
        const Bucket          &bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        double value1, value2;

        this->bucketValues(groupInd, bucket, value1, value2);

        BarValue barValue = varIndsValue(varsData);

        QString bucketName = bucketValuesStr(groupInd, bucket, values);

        CQChartsGeom::RangeValue xv(CMathUtil::avg(value1, value2),
          values->xValueRange.min(), values->xValueRange.max());
        CQChartsGeom::RangeValue yv(barValue.n2,
          values->yValueRange.min(), values->yValueRange.max());

        addKeyRow(ColorInd(), ColorInd(iv, nv), xv, yv, bucketName);

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
  const Values *values = getGroupValues(groupInd);

  if (! values)
    return "";

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

  const CQBucketer &bucketer = groupBucketer(groupInd);

  bool isNumeric = (values ? values->valueSet->isNumeric() : false);

  if (isNumeric) {
    double value1, value2;

    bucketValues(groupInd, bucket, value1, value2);

    if      (type == BucketValueType::ALL) {
      if (bucketer.isIntegral()) {
        int ivalue1 = CMathRound::RoundNearest(value1);
        int ivalue2 = CMathRound::RoundNearest(value2);

        if (ivalue1 != ivalue2)
          return CQBucketer::bucketName(ivalue1, ivalue2, CQBucketer::NameFormat::BRACKETED);
        else
          return QString("%1").arg(ivalue1);
      }
      else {
        return CQBucketer::bucketName(value1, value2, CQBucketer::NameFormat::BRACKETED);
      }
    }
    else if (type == BucketValueType::START)
      return QString("%1").arg(value1);
    else
      return QString("%1").arg(value2);
  }
  else {
    if (bucket.hasValue())
      return bucketer.bucketString(bucket.value());
    else
      return "";

#if 0
    if (isAutoBucket())
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
    const CQBucketer &bucketer = groupBucketer(groupInd);

    bool isAuto = (! filterStack_.empty() || isAutoBucket());

    if (isAuto)
      bucketer.autoBucketValues(bucket.value(), value1, value2);
    else
      bucketer.bucketRValues(bucket.value(), value1, value2);

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
  std::unique_lock<std::mutex> lock(mutex_);

  // use consistent bucketer when stacked/side by side
  if (isConsistentBucketer())
    groupInd = 0;

  auto p = groupData_.groupBucketer.find(groupInd);

  if (p == groupData_.groupBucketer.end()) {
    CQBucketer bucketer;

    bucketer.setType(bucketer_.type());

    bucketer.setRStart(bucketer_.rstart());
    bucketer.setRDelta(bucketer_.rdelta());

    bucketer.setNumAuto(bucketer_.numAuto());

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
    if (isStacked() || isOverlay() || isSideBySide() || isScatter())
      return true;
  }

  return false;
}

//------

QString
CQChartsDistributionPlot::
posStr(const CQChartsGeom::Point &w) const
{
  if (isDensity() || isScatter())
    return CQChartsPlot::posStr(w);

  //---

  if (! isHorizontal()) {
    QString xstr = xStr(int(w.x));

    for (const auto &plotObj : plotObjs_) {
      CQChartsDistributionBarObj *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      double value;

      if (barObj && barObj->bucketXValue(w.x, value)) {
        xstr = QString("%1").arg(value);
        break;
      }
    }

    return xstr + " " + yStr(w.y);
  }
  else {
    QString ystr = yStr(int(w.y));

    for (const auto &plotObj : plotObjs_) {
      CQChartsDistributionBarObj *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      double value;

      if (barObj && barObj->bucketYValue(w.y, value)) {
        ystr = QString("%1").arg(value);
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
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name,
                                  bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  auto addAction = [&](const QString &name, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));

  QMenu *typeMenu = nullptr;

  if (hasGroups()) {
    if (! typeMenu)
      typeMenu = new QMenu("Plot Type");

    (void) addMenuCheckedAction(typeMenu, "Normal"    , isNormal    (), SLOT(setNormal    (bool)));
    (void) addMenuCheckedAction(typeMenu, "Stacked"   , isStacked   (), SLOT(setStacked   (bool)));
    (void) addMenuCheckedAction(typeMenu, "Overlay"   , isOverlay   (), SLOT(setOverlay   (bool)));
    (void) addMenuCheckedAction(typeMenu, "SideBySide", isSideBySide(), SLOT(setSideBySide(bool)));
  }

  if (! typeMenu)
    typeMenu = new QMenu("Plot Type");

  (void) addMenuCheckedAction(typeMenu, "Scatter", isScatter(), SLOT(setScatter(bool)));
  (void) addMenuCheckedAction(typeMenu, "Density", isDensity(), SLOT(setDensity(bool)));

  menu->addMenu(typeMenu);

  QMenu *valueMenu = new QMenu("Value Type");

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

  QAction *pushAction   = addAction("Push"   , SLOT(pushSlot  ()));
  QAction *popAction    = addAction("Pop"    , SLOT(popSlot   ()));
  QAction *popTopAction = addAction("Pop Top", SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(! filterStack_.empty());
  popTopAction->setEnabled(! filterStack_.empty());

  return true;
}

//---

bool
CQChartsDistributionPlot::
hasForeground() const
{
  if (isDensity() || isScatter())
    return false;

  if (! isStatsLines())
    return false;

  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsDistributionPlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  if (! hasForeground())
    return;

  if (isStatsLines())
    drawStatsLines(device);
}

void
CQChartsDistributionPlot::
drawStatsLines(CQChartsPaintDevice *device) const
{
  // set pen
  QColor bc = interpStatsLinesColor(ColorInd());

  QPen pen;

  setPen(pen, true, bc, statsLinesAlpha(), statsLinesWidth(), statsLinesDash());

  device->setPen(pen);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  int ig = 0;
  int ng = groupData_.groupValues.size();

  for (const auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int           groupInd = groupValues.first;
    const Values *values   = groupValues.second;

    CQStatData statData = values->statData;

    //---

    for (const auto &plotObj : plotObjects()) {
      const CQChartsDistributionBarObj *barObj =
        dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

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
      QPointF p1, p2;

      if (! isHorizontal()) {
        p1 = QPointF(value, dataRange.ymin());
        p2 = QPointF(value, dataRange.ymax());
      }
      else {
        p1 = QPointF(dataRange.xmin(), value);
        p2 = QPointF(dataRange.xmax(), value);
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
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(pos));

    plotObjsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  Filters filters;

  for (const auto &obj : objs) {
    CQChartsDistributionBarObj *distObj = qobject_cast<CQChartsDistributionBarObj *>(obj);

    double value1, value2;

    bucketValues(distObj->groupInd(), distObj->bucket(), value1, value2);

    filters.emplace_back(distObj->groupInd(), value1, value2);
  }

  filterStack_.push_back(filters);

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

//------

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                           int groupInd, const Bucket &bucket, const BarValue &barValue,
                           const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsDistributionPlot *>(plot), rect, ColorInd(), ig, iv),
 plot_(plot), groupInd_(groupInd), bucket_(bucket), barValue_(barValue)
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

  QString groupName = this->groupName();

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  //---

  QString bucketStr = this->bucketStr();

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
    bool ok;

    QString str = plot_->modelString(row, plot_->colorColumn(), parent, ok);

    if (ok)
      strs.push_back(str);
  }

  if (strs.length()) {
    bool ok;

    QString name = plot_->modelHeaderString(plot_->colorColumn(), ok);

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
  QString path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
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
  return barValue_.n2;
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
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox rect = calcRect();

  QRectF qrect = rect.qrect();

  QString ystr;

  if      (plot_->isValueCount()) {
    ystr = QString("%1").arg(count());
  }
  else if (plot_->isValueRange()) {
    ystr = QString("%1-%2").arg(minValue()).arg(maxValue());
  }
  else if (plot_->isValueMin()) {
    ystr = QString("%1").arg(maxValue());
  }
  else if (plot_->isValueMax()) {
    ystr = QString("%1").arg(maxValue());
  }
  else if (plot_->isValueMean()) {
    ystr = QString("%1").arg(maxValue());
  }
  else if (plot_->isValueSum()) {
    ystr = QString("%1").arg(maxValue());
  }

  return plot_->dataLabel()->calcRect(qrect, ystr);
}

void
CQChartsDistributionBarObj::
getSelectIndices(Indices &inds) const
{
  CQChartsDistributionPlot::VariantInds vinds;

  plot_->getInds(groupInd_, bucket_, vinds);

  for (auto &vind : vinds) {
    const CQChartsModelIndex &ind = vind.ind;

    addSelectIndex(inds, ind);
  }
}

void
CQChartsDistributionBarObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    CQChartsDistributionPlot::VariantInds vinds;

    plot_->getInds(groupInd_, bucket_, vinds);

    for (auto &vind : vinds) {
      CQChartsModelIndex ind = vind.ind;

      ind.column = column;

      addSelectIndex(inds, ind);
    }
  }
}

void
CQChartsDistributionBarObj::
draw(CQChartsPaintDevice *device)
{
  CQChartsGeom::BBox pbbox = calcRect();

  QRectF pqrect = pbbox.qrect();

  //---

  bool useLine = false;

  if (! plot_->isDotLines()) {
    double s = (! plot_->isHorizontal() ? pqrect.width() : pqrect.height());

    useLine = (s <= 2);
  }

  //---

  QImage image;

  if (plot_->imageColumn().isValid()) {
    CQChartsDistributionPlot::VariantInds vinds;

    plot_->getInds(groupInd_, bucket_, vinds);

    for (auto &vind : vinds) {
      CQChartsModelIndex ind = vind.ind;

      ind.column = plot_->imageColumn();

      bool ok;

      QVariant imageVar = plot_->modelValue(ind, ok);

      if (ok && imageVar.type() == QVariant::Image) {
        image = imageVar.value<QImage>();

        if (! image.isNull())
          break;
      }
    }
  }

  //---

  // get bar colors
  colorData_ = ColorData();

  if (getBarColoredRects(colorData_)) {
    double size = (! plot_->isHorizontal() ? pqrect.height() : pqrect.width());

    if      (plot_->isValueCount()) {
      double dsize = size/colorData_.nv;

      double pos1 = 0.0, pos2 = 0.0;

      for (auto &p : colorData_.colorSet) {
        const CQChartsColor &color = p.first;
        int                  n     = colorData_.colorCount[p.second];

        pos1 = pos2;
        pos2 = pos1 + dsize*n;

        QRectF pqrect1;

        if (! plot_->isHorizontal())
          pqrect1 = QRectF(pqrect.x(), pqrect.bottom() - pos2, pqrect.width(), pos2 - pos1);
        else
          pqrect1 = QRectF(pqrect.left() + pos1, pqrect.y(), pos2 - pos1, pqrect.height());

        //---

        drawRect(device, pqrect1, color, useLine);
      }
    }
    else if (plot_->isValueSum()) {
      double pos1 = 0.0, pos2 = 0.0;

      for (auto &cs : colorData_.colorSizes) {
        const CQChartsColor &color = cs.first;
        double               dsize = cs.second;

        pos1 = pos2;
        pos2 = pos1 + size*dsize;

        QRectF pqrect1;

        if (! plot_->isHorizontal())
          pqrect1 = QRectF(pqrect.x(), pqrect.bottom() - pos2, pqrect.width(), pos2 - pos1);
        else
          pqrect1 = QRectF(pqrect.left() + pos1, pqrect.y(), pos2 - pos1, pqrect.height());

        //---

        drawRect(device, pqrect1, color, useLine);
      }
    }
  }
  else {
    QColor barColor = this->barColor();

    drawRect(device, pqrect, barColor, useLine);
  }

  if (! image.isNull())
    device->drawImageInRect(device->pixelToWindow(pqrect), image);
}

void
CQChartsDistributionBarObj::
drawFg(CQChartsPaintDevice *device) const
{
  CQChartsGeom::BBox pbbox = calcRect();

  QRectF pqrect = pbbox.qrect();

  //---

  QString ystr;

  if      (plot_->isValueCount()) {
    ystr = QString("%1").arg(count());
  }
  else if (plot_->isValueRange()) {
    ystr = QString("%1-%2").arg(minValue()).arg(maxValue());
  }
  else if (plot_->isValueMin()) {
    ystr = QString("%1").arg(maxValue());
  }
  else if (plot_->isValueMax()) {
    ystr = QString("%1").arg(maxValue());
  }
  else if (plot_->isValueMean()) {
    ystr = QString("%1").arg(maxValue());
  }
  else if (plot_->isValueSum()) {
    ystr = QString("%1").arg(maxValue());
  }

  plot_->dataLabel()->draw(device, device->pixelToWindow(pqrect), ystr);

  //---

  if (plot_->isRug())
    drawRug(device);
}

void
CQChartsDistributionBarObj::
drawRug(CQChartsPaintDevice *device) const
{
  // get symbol and size
  CQChartsSymbol symbolType = plot_->rugSymbolType();
  CQChartsLength symbolSize = plot_->rugSymbolSize();

  if (symbolType == CQChartsSymbol::Type::NONE)
    symbolType = (! plot_->isHorizontal() ?
      CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy);

  //---

  // set pen and brush
  ColorInd ic = (ig_.n > 1 ? ig_ : iv_);

  QPen   pen;
  QBrush brush;

  plot_->setRugSymbolPenBrush(pen, brush, ic);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw symbols
  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  std::vector<double> xvals;

  plot_->getXVals(groupInd_, bucket_, xvals);

  for (const auto &x : xvals) {
    double x1 = mapValue(x);

    QPointF p;

    if (! plot_->isHorizontal())
      p = QPointF(x1, dataRange.ymin());
    else
      p = QPointF(dataRange.xmin(), x1);

    QPointF ps = plot_->windowToPixel(p);

    if (! plot_->isHorizontal())
      ps.setY(ps.y() + sy);
    else
      ps.setX(ps.x() - sx);

    plot_->drawSymbol(device, device->pixelToWindow(ps), symbolType, symbolSize, pen, brush);
  }
}

double
CQChartsDistributionBarObj::
mapValue(double v) const
{
  double bmin = (! plot_->isHorizontal() ? rect_.getXMin() : rect_.getYMin());
  double bmax = (! plot_->isHorizontal() ? rect_.getXMax() : rect_.getYMax());

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
  QColor barColor = this->barColor();
  QColor bgColor  = plot_->interpThemeColor(ColorInd(0.2));

  // get color of individual values
  colorData.nv = 0;

  // for count value type get count of unique colors for values
  // for sum value type get fraction of total for values
  CQChartsDistributionPlot::VariantInds vinds;

  plot_->getInds(groupInd_, bucket_, vinds);

  int nvi = vinds.size();

  if (nvi <= 1)
    return false;

  double minAlpha = 0.4;
  double maxAlpha = 1.0;

  double bsize = 1.0/nvi;

  for (const auto &vind : vinds) {
    const CQChartsModelIndex &ind = vind.ind;

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

    // set color from value
    CQChartsColor color;

    if (plot_->columnColor(ind.row, ind.parent, color)) {
      QColor c1 = plot_->interpColor(color, ColorInd());

      c1.setAlphaF(plot_->barFillAlpha());

      color = c1;
    }
    else {
      double alpha = (maxAlpha - minAlpha)*colorData.nv/(nvi - 1.0) + minAlpha;

      QColor barColor1 = CQChartsUtil::blendColors(barColor, bgColor, alpha);

      color = barColor1;
    }

    //---

    // add unique colors to set
    auto p = colorData.colorSet.find(color);

    if (p == colorData.colorSet.end()) {
      colorData.colorRows.insert(ind.row);

      int ind = colorData.colorSet.size();

      p = colorData.colorSet.insert(p, ColorSet::value_type(color, ind));
    }

    ++colorData.colorCount[(*p).second];

    //---

    colorData.colorSizes.push_back(ColorSize(color, bsize1));

    ++colorData.nv;
  }

  if (colorData.colorSet.empty()) {
    colorData.colorSet[barColor] = 0;

    colorData.colorCount[0] = 1;

    colorData.colorSizes.push_back(ColorSize(barColor, 1.0));

    colorData.nv = 1;
  }

  return true;
}

void
CQChartsDistributionBarObj::
drawRect(CQChartsPaintDevice *device, const QRectF &pqrect,
         const CQChartsColor &color, bool useLine) const
{
  // set pen and brush
  QPen   pen;
  QBrush barBrush;

  QColor bc = plot_->interpBarStrokeColor(ColorInd());
  QColor fc = plot_->interpColor(color, ColorInd());

  CQChartsLength bw = plot_->barStrokeWidth();

  if (useLine) {
    bw = CQChartsLength("0px");

    if (plot_->isBarFilled())
      bc = fc;
  }

  plot_->setPenBrush(pen, barBrush,
    plot_->isBarStroked(), bc, plot_->barStrokeAlpha(), bw, plot_->barStrokeDash(),
    plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

  plot_->updateObjPenBrushState(this, pen, barBrush);

  device->setPen  (pen);
  device->setBrush(barBrush);

  //---

  if (! plot_->isDotLines()) {
    // draw rect
    if (! useLine) {
      CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(pqrect),
                                           plot_->barCornerSize(), plot_->barCornerSize());
    }
    else {
      if (! plot_->isHorizontal()) {
        double xc = pqrect.center().x();

        device->drawLine(device->pixelToWindow(QPointF(xc, pqrect.bottom())),
                         device->pixelToWindow(QPointF(xc, pqrect.top   ())));
      }
      else {
        double yc = pqrect.center().y();

        device->drawLine(device->pixelToWindow(QPointF(pqrect.left (), yc)),
                         device->pixelToWindow(QPointF(pqrect.right(), yc)));
      }
    }
  }
  else {
    // draw line
    double lw = plot_->lengthPixelSize(plot_->dotLineWidth(), ! plot_->isHorizontal());

    if (! plot_->isHorizontal()) {
      double xc = pqrect.center().x();

      if (lw < 3)
        device->drawLine(device->pixelToWindow(QPointF(xc, pqrect.bottom())),
                         device->pixelToWindow(QPointF(xc, pqrect.top   ())));
      else {
        QRectF pqrect1(xc - lw/2, pqrect.top(), lw, pqrect.height());

        CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(pqrect1));
      }
    }
    else {
      double yc = pqrect.center().y();

      if (lw < 3)
        device->drawLine(device->pixelToWindow(QPointF(pqrect.left (), yc)),
                         device->pixelToWindow(QPointF(pqrect.right(), yc)));
      else {
        QRectF pqrect1(pqrect.left(), yc - lw/2, pqrect.width(), lw);

        CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(pqrect1));
      }
    }

    //---

    // get dot symbol and size
    CQChartsSymbol symbolType = plot_->dotSymbolType();
    CQChartsLength symbolSize = plot_->dotSymbolSize();

    ColorInd ic = (ig_.n > 1 ? ig_ : iv_);

    //---

    // set dot pen and brush
    QPen   dotPen;
    QBrush dotBrush;

    plot_->setDotSymbolPenBrush(dotPen, dotBrush, ic);

    device->setPen  (dotPen);
    device->setBrush(dotBrush);

    //---

    // draw dot
    QPointF p;

    if (! plot_->isHorizontal())
      p = QPointF(pqrect.center().x(), pqrect.top());
    else
      p = QPointF(pqrect.right(), pqrect.center().y());

    plot_->drawSymbol(device, device->pixelToWindow(p), symbolType, symbolSize);
  }
}

QColor
CQChartsDistributionBarObj::
barColor() const
{
  ColorInd colorInd = this->calcColorInd();

  return plot_->interpBarFillColor(colorInd);
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
calcRect() const
{
  double minSize = plot_->minBarSize();

  //---

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect_);

  //---

  // calc margins

  double ml = plot_->lengthPixelSize(plot_->margin(), ! plot_->isHorizontal());
  double mr = ml;

  if (plot_->hasGroups()) {
    if      (plot_->isStacked()) {
    }
    else if (plot_->isOverlay()) {
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
        ml = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
      else if (iv_.i == iv_.n - 1)
        mr = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
    }
  }

  //---

  // adjust rect by margins
  double rs = prect.getSize(! plot_->isHorizontal());

  double s1 = rs - 2*std::max(ml, mr);

  if (s1 < minSize) {
    ml = (rs - minSize)/2.0;
    mr = ml;
  }

  prect.expandExtent(-ml, -mr, ! plot_->isHorizontal());

  return prect;
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
CQChartsDistributionDensityObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                               int groupInd, const Data &data, double doffset, const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsDistributionPlot *>(plot), rect), plot_(plot),
 groupInd_(groupInd), data_(data), doffset_(doffset), is_(is)
{
  // create density polygon
  int np = data_.points.size();

  if (np < 2) {
    poly_ = QPolygonF();
    return;
  }

  double y1 = data_.ymin;

  if (! plot->isHorizontal()) {
    for (int i = 0; i < np; ++i)
      poly_ << QPointF(data_.points[i].x(), data_.points[i].y() - y1 + doffset_);
  }
  else {
    for (int i = 0; i < np; ++i)
      poly_ << QPointF(data_.points[i].y() - y1 + doffset_, data_.points[i].x());
  }

  //----

  // calc scale factor for data bars
  int nb = data_.buckets.size();

  double area = 0.0;

  for (int i = 0; i < nb; ++i) {
    double dy = data_.buckets[i].n;

    double value1, value2;

    plot_->bucketValues(groupInd_, Bucket(data_.buckets[i].bucket), value1, value2);

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

  QString groupName = this->groupName();
  int     ns        = this->numSamples();

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
  return data_.points.size();
}

bool
CQChartsDistributionDensityObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  return poly_.containsPoint(p.qpoint(), Qt::OddEvenFill);
}

void
CQChartsDistributionDensityObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsDistributionDensityObj::
draw(CQChartsPaintDevice *device)
{
  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBarStrokeColor(is_);
  QColor fc = plot_->interpBarFillColor  (is_);

  plot_->setPenBrush(pen, brush,
    plot_->isBarStroked(), bc, plot_->barStrokeAlpha(),
    plot_->barStrokeWidth(), plot_->barStrokeDash(),
    plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

  //---

  // adjust brush for gradient
  if (plot_->isDensityGradient()) {
    CQChartsGeom::BBox pixelRect = plot_->calcPlotPixelRect();

    QPointF pg1, pg2;

    if (! plot_->isHorizontal()) {
      pg1 = QPointF(pixelRect.getXMin(), pixelRect.getYMin());
      pg2 = QPointF(pixelRect.getXMax(), pixelRect.getYMin());
    }
    else {
      pg1 = QPointF(pixelRect.getXMin(), pixelRect.getYMax());
      pg2 = QPointF(pixelRect.getXMin(), pixelRect.getYMin());
    }

    QLinearGradient lg(pg1.x(), pg1.y(), pg2.x(), pg2.y());

    plot_->view()->themePalette()->setLinearGradient(lg, plot_->barFillAlpha());

    brush = QBrush(lg);
  }

  //---

  // adjust pen/brush for selected/mouse over
  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw bars for buckets
  if (plot_->isDensityBars()) {
    int nb = data_.buckets.size();

    for (int i = 0; i < nb; ++i) {
      double y = data_.buckets[i].n*bucketScale_;

      double value1, value2;

      plot_->bucketValues(groupInd_, Bucket(data_.buckets[i].bucket), value1, value2);

      CQChartsGeom::BBox bbox(value1, 0, value2, y);

      CQChartsGeom::BBox pbbox = plot_->windowToPixel(bbox);

      device->drawRect(device->pixelToWindow(pbbox.qrect()));
    }
  }

  //---

  // draw density polygon
  device->drawPolygon(poly_);
}

void
CQChartsDistributionDensityObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (plot_->isStatsLines())
    drawStatsLines(device);

  if (plot_->isRug())
    drawRug(device);
}

void
CQChartsDistributionDensityObj::
drawStatsLines(CQChartsPaintDevice *device) const
{
  // set pen
  QColor bc = plot_->interpStatsLinesColor(ColorInd());

  QPen pen;

  plot_->setPen(pen, true, bc, plot_->statsLinesAlpha(),
                plot_->statsLinesWidth(), plot_->statsLinesDash());

  device->setPen(pen);

  //---

  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  auto drawStatLine = [&](double value) {
    QPointF p1, p2;

    if (! plot_->isHorizontal()) {
      p1 = QPointF(value, dataRange.ymin());
      p2 = QPointF(value, dataRange.ymax());
    }
    else {
      p1 = QPointF(dataRange.xmin(), value);
      p2 = QPointF(dataRange.xmax(), value);
    }

    device->drawLine(p1, p2);
  };

  drawStatLine(data_.statData.lowerMedian);
  drawStatLine(data_.statData.median);
  drawStatLine(data_.statData.upperMedian);
}

void
CQChartsDistributionDensityObj::
drawRug(CQChartsPaintDevice *device) const
{
  // get symbol and size
  CQChartsSymbol symbolType = plot_->rugSymbolType();
  CQChartsLength symbolSize = plot_->rugSymbolSize();

  if (symbolType == CQChartsSymbol::Type::NONE)
    symbolType = (! plot_->isHorizontal() ?
      CQChartsSymbol::Type::VLINE : CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy);

  //---

  // set pen brush
  // TODO: allow control of alpha, and line width
  QPen   pen;
  QBrush brush;

  QColor fillColor = plot_->interpBarFillColor(is_);

  plot_->setPen  (pen  , true, fillColor, 1.0);
  plot_->setBrush(brush, true, fillColor, 0.5);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  std::vector<double> xvals;
  CQStatData          statData;

  (void) plot_->getRealValues(groupInd_, xvals, statData);

  for (const auto &x1 : xvals) {
    QPointF p1;

    if (! plot_->isHorizontal())
      p1 = QPointF(x1, dataRange.ymin());
    else
      p1 = QPointF(dataRange.xmin(), x1);

    QPointF ps = plot_->windowToPixel(p1);

    if (! plot_->isHorizontal())
      ps.setY(ps.y() + sy);
    else
      ps.setX(ps.x() - sx);

    plot_->drawSymbol(device, device->pixelToWindow(ps), symbolType, symbolSize, pen, brush);
  }
}

//------

CQChartsDistributionScatterObj::
CQChartsDistributionScatterObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                               int groupInd, const Bucket &bucket, int n,
                               const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsDistributionPlot *>(plot), rect), plot_(plot),
 groupInd_(groupInd), bucket_(bucket), n_(n), is_(is), iv_(iv)
{
  // get factored number of points
  int nf = CMathUtil::clamp(int(n_*plot_->scatterFactor()), 1, n_);

  // generate random points in box (0.0->1.0) with margin
  double m = plot_->scatterMargin();

  // TODO: constant seed ?
  CQChartsRand::RealInRange rand(m, 1.0 - m);

  points_.resize(nf);

  for (int i = 0; i < nf; ++i)
    points_[i] = QPointF(rand.gen(), rand.gen());
}

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
  QString groupName = plot_->groupIndName(groupInd_);
  QString bucketStr = plot_->bucketStr(groupInd_, bucket_);

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

void
CQChartsDistributionScatterObj::
draw(CQChartsPaintDevice *device)
{
  // set pen brush
  // TODO: allow control of stroke color, alpha, and line width
  ColorInd ic = (is_.n > 1 ? is_ : iv_);

  QColor c = plot_->interpBarFillColor(ic);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, Qt::black, 1.0);
  plot_->setBrush(brush, true, c, 1.0);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

#if 0
  device->drawRect(rect().qrect());
#endif

  //---

  CQChartsSymbol symbolType(CQChartsSymbol::Type::CIRCLE);
  CQChartsLength symbolSize(6, CQChartsUnits::PIXEL);

  QPointF tl = prect.qrect().topLeft();

  if (! plot_->isHorizontal()) {
    for (const auto &point : points_) {
      double px = plot_->windowToPixelWidth (point.x());
      double py = plot_->windowToPixelHeight(point.y());

      QPointF p(tl.x() + px, tl.y() + py);

      plot_->drawSymbol(device, device->pixelToWindow(p), symbolType, symbolSize, pen, brush);
    }
  }
  else {
    for (const auto &point : points_) {
      double px = plot_->windowToPixelWidth (point.y());
      double py = plot_->windowToPixelHeight(point.x());

      QPointF p(tl.x() + px, tl.y() + py);

      plot_->drawSymbol(device, device->pixelToWindow(p), symbolType, symbolSize, pen, brush);
    }
  }
}

void
CQChartsDistributionScatterObj::
getSelectIndices(Indices &) const
{
}

//------

CQChartsDistKeyColorBox::
CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, const ColorInd &ig, const ColorInd &iv,
                        const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyColorBox(plot, ColorInd(), ig, iv, xv, yv), plot_(plot)
{
}

bool
CQChartsDistKeyColorBox::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  setSetHidden(! isSetHidden());

  plot_->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsDistKeyColorBox::
fillBrush() const
{
  if (color_.isValid())
    return plot_->interpColor(color_, ColorInd());

  ColorInd colorInd = this->calcColorInd();

  QColor c = plot_->interpBarFillColor(colorInd);

  if (isSetHidden())
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

bool
CQChartsDistKeyColorBox::
isSetHidden() const
{
  if (ig_.n > 1)
    return plot_->CQChartsPlot::isSetHidden(ig_.i);
  else
    return plot_->CQChartsPlot::isSetHidden(iv_.i);
}

void
CQChartsDistKeyColorBox::
setSetHidden(bool b)
{
  if (ig_.n > 1)
    plot_->CQChartsPlot::setSetHidden(ig_.i, b);
  else
    plot_->CQChartsPlot::setSetHidden(iv_.i, b);
}

//------

CQChartsDistKeyText::
CQChartsDistKeyText(CQChartsDistributionPlot *plot, const QString &text, const ColorInd &iv) :
 CQChartsKeyText(plot, text, iv)
{
}

QColor
CQChartsDistKeyText::
interpTextColor(const ColorInd &ind) const
{
  QColor c = CQChartsKeyText::interpTextColor(ind);

  if (isSetHidden())
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

bool
CQChartsDistKeyText::
isSetHidden() const
{
  return plot_->CQChartsPlot::isSetHidden(ic_.i);
}

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
