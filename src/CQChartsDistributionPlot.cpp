#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>
#include <CQChartsDensity.h>
#include <CQChartsRand.h>
#include <CQPerfMonitor.h>

#include <QPainter>
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
   setTip("bucket grouped values");

  addBoolParameter("autoBucket", "Auto Bucket", "autoBucket", true).
   setTip("automatically determine bucket ranges");

  addRealParameter("startBucketValue", "Start Value", "startBucketValue", 0.0).
    setRequired().setTip("Start value for manual bucket");
  addRealParameter("deltaBucketValue", "Delta Value", "deltaBucketValue", 1.0).
    setRequired().setTip("Delta value for manual bucket");

  addIntParameter("numAutoBuckets", "Num Auto Buckets", "numAutoBuckets", 10).
    setRequired().setTip("Number of auto buckets");

  // options
  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("draw bars horizontal");

  addEnumParameter("plotType", "Plot Type", "plotType").
    addNameValue("Normal"      , int(CQChartsDistributionPlot::PlotType::NORMAL      )).
    addNameValue("Stacked"     , int(CQChartsDistributionPlot::PlotType::STACKED     )).
    addNameValue("Side By Side", int(CQChartsDistributionPlot::PlotType::SIDE_BY_SIDE)).
    addNameValue("Overlay"     , int(CQChartsDistributionPlot::PlotType::OVERLAY     )).
    addNameValue("Scatter"     , int(CQChartsDistributionPlot::PlotType::SCATTER     )).
    addNameValue("Density"     , int(CQChartsDistributionPlot::PlotType::DENSITY     )).
    setTip("Plot type");

  addEnumParameter("valueType", "Value Type", "valueType").
   addNameValue("Count", int(CQChartsDistributionPlot::ValueType::COUNT)).
   addNameValue("Range", int(CQChartsDistributionPlot::ValueType::RANGE)).
   addNameValue("Min"  , int(CQChartsDistributionPlot::ValueType::MIN  )).
   addNameValue("Max"  , int(CQChartsDistributionPlot::ValueType::MAX  )).
   addNameValue("Mean" , int(CQChartsDistributionPlot::ValueType::MEAN )).
   addNameValue("Sum"  , int(CQChartsDistributionPlot::ValueType::SUM  )).
   setTip("Bar value type");

  addBoolParameter("percent"  , "Percent"   , "percent"  ).setTip("Show value is percentage");
  addBoolParameter("skipEmpty", "Skip Empty", "skipEmpty").setTip("skip empty buckets");
  addBoolParameter("sorted"   , "Sorted"    , "sorted"   ).setTip("sort by count");
  addBoolParameter("dotLines" , "Dot Lines" , "dotLines" ).setTip("draw bars as lines with dot");
  addBoolParameter("rug"      , "Rug"       , "rug"      ).setTip("draw rug points");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsDistributionPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws bars with heights for the counts of set of values.</p>\n"
         "<h2>Columns</h2>\n"
         "<p>The values to be counted are taken from the <b>Value</b> columns "
         "and grouped depending on the column value type. By default integer "
         "values are grouped by matching value, real values are automatically "
         "bucketed into ranges and strings are grouped by matching value.</p>\n"
         "<p>The automatic bucketing of real values can overridden by turning off using "
         "the <b>autoBucket</b> parameter and specifying the <b>startBucketValue</b> and "
         "<b>deltaBucketValue</b> parameters.</p>"
         "<p>The color of the bar can be overridden using the <b>Color</b> column.</p>\n"
         "<h2>Options</h2>\n"
         "<p>Enabling the <b>Horizontal</b> otpions draws the bars horizontally "
         "of vertically.</p>"
         "<h2>Grouping</h2>\n"
         "<p>Standard grouping can be applied to the values to split the values to be "
         "grouped into individual value sets before final grouping. This second level "
         "if grouping creates multiple sets of grouped values which can be displayed "
         "sequentially or overlaid with common buckets.</p>";
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
 CQChartsObjMeanLineData<CQChartsDistributionPlot>(this),
 CQChartsObjDotPointData<CQChartsDistributionPlot>(this),
 CQChartsObjRugPointData<CQChartsDistributionPlot>(this)
{
  NoUpdate noUpdate(this);

  dataLabel_ = new CQChartsDataLabel(this);

  setAutoBucket    (true);
  setNumAutoBuckets(20);

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

  delete dataLabel_;
}

//---

void
CQChartsDistributionPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDataColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(dataColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setBucketed(bool b)
{
  CQChartsUtil::testAndSet(bucketed_, b, [&]() { queueUpdateRangeAndObjs(); } );
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

  queueUpdateRangeAndObjs();
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

  queueUpdateRangeAndObjs();
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

  queueUpdateRangeAndObjs();
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

  queueUpdateRangeAndObjs();
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
  CQChartsPlot::addProperties();

  CQChartsBarPlot::addProperties();

  addProperty("columns", this, "nameColumn", "name");
  addProperty("columns", this, "dataColumn", "data");

  addProperty("bucket", this, "bucketed"        , "enabled");
  addProperty("bucket", this, "autoBucket"      , "auto"   );
  addProperty("bucket", this, "startBucketValue", "start"  );
  addProperty("bucket", this, "deltaBucketValue", "delta"  );
  addProperty("bucket", this, "numAutoBuckets"  , "num"    );

  addProperty("options", this, "plotType" , "plotType" );
  addProperty("options", this, "valueType", "valueType");

  addProperty("options", this, "percent"  );
  addProperty("options", this, "skipEmpty");
  addProperty("options", this, "sorted"   );

  addProperty("density", this, "density"        , "enabled" );
  addProperty("density", this, "densityOffset"  , "offset"  );
  addProperty("density", this, "densitySamples" , "samples" );
  addProperty("density", this, "densityGradient", "gradient");
  addProperty("density", this, "densityBars"    , "bars"    );

  addProperty("scatter", this, "scatter"      , "enabled");
  addProperty("scatter", this, "scatterFactor", "factor" );

  addProperty("meanLine", this, "showMean", "visible");

  addLineProperties("meanLine/line", "meanLines");

  addProperty("dotLines"     , this, "dotLines"    , "enabled");
  addProperty("dotLines/line", this, "dotLineWidth", "width"  );

  addSymbolProperties("dotLines", "dot");

  addProperty("rug", this, "rug", "enabled");

  addSymbolProperties("rug", "rug");

  CQChartsGroupPlot::addProperties();

  dataLabel_->addPathProperties("dataLabel");
}

//---

void
CQChartsDistributionPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() {
    dataLabel_->setDirection(horizontal_ ? Qt::Horizontal : Qt::Vertical);

    queueUpdateRangeAndObjs();
  } );
}

void
CQChartsDistributionPlot::
setPlotType(PlotType type)
{
  CQChartsUtil::testAndSet(plotType_, type, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setNormal(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setStacked(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::STACKED, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setSideBySide(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::SIDE_BY_SIDE, [&]() {
      queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setOverlay(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::OVERLAY, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setPercent(bool b)
{
  CQChartsUtil::testAndSet(percent_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setSkipEmpty(bool b)
{
  CQChartsUtil::testAndSet(skipEmpty_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setValueType(ValueType type)
{
  CQChartsUtil::testAndSet(valueType_, type, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueCount(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueRange(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::RANGE, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueMin(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MIN, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueMax(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MAX, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueMean(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::MEAN, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setValueSum(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(valueType_, ValueType::SUM, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(valueType_, ValueType::COUNT, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setSorted(bool b)
{
  CQChartsUtil::testAndSet(sorted_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setDensity(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::DENSITY, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityOffset(double o)
{
  CQChartsUtil::testAndSet(densityData_.offset, o, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensitySamples(int n)
{
  CQChartsUtil::testAndSet(densityData_.numSamples, n, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityGradient(bool b)
{
  CQChartsUtil::testAndSet(densityData_.gradient, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityBars(bool b)
{
  CQChartsUtil::testAndSet(densityData_.bars, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setScatter(bool b)
{
  if (b)
    CQChartsUtil::testAndSet(plotType_, PlotType::SCATTER, [&]() { queueUpdateRangeAndObjs(); } );
  else
    CQChartsUtil::testAndSet(plotType_, PlotType::NORMAL, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setScatterFactor(double f)
{
  CQChartsUtil::testAndSet(scatterData_.factor, f, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setDotLines(bool b)
{
  CQChartsUtil::testAndSet(dotLineData_.enabled, b, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDotLineWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(dotLineData_.width, l, [&]() { queueDrawObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setRug(bool b)
{
  CQChartsUtil::testAndSet(rug_, b, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setShowMean(bool b)
{
  CQChartsUtil::testAndSet(showMean_, b, [&]() { queueUpdateRangeAndObjs(); } );
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

  // bucket grouped sets of values
  for (auto &groupValues : groupData_.groupValues) {
    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    // init group bucketer
    CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

    CQBucketer &bucketer = th->groupBucketer(groupInd);

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

    bool hierValue = isHierarchical();

    // add each index to associated bucket
    for (auto &ind : values->inds) {
      int      bucket = -1;
      QVariant value;

      //---

      bool ok;

      if (isBucketed()) {
        if      (type == CQChartsValueSet::Type::REAL) {
          double r = modelReal(ind, ok);
          if (! ok || CMathUtil::isNaN(r)) continue;

          bucket = calcBucket(groupInd, r);
          value  = QVariant(r);
        }
        else if (type == CQChartsValueSet::Type::INTEGER) {
          int i = modelInteger(ind, ok);
          if (! ok) continue;

          bucket = calcBucket(groupInd, i);
          value  = QVariant(i);
        }
        else {
          QString str;

          if (hierValue) {
            QVariant value = modelRootValue(ind.row, ind.column, ind.parent, Qt::DisplayRole, ok);

            str = value.toString();
          }
          else {
            str = modelString(ind, ok);
          }

          if (! ok) continue;

          bucket = bucketer.stringBucket(str);
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

  // calc range (number of bars and max height)

  int i1 = 0;
  int i2 = 0;

  CQChartsGeom::IMinMax nRange(i1);
  CQChartsGeom::IMinMax bucketRange;
  CQChartsGeom::RMinMax valueRange;
  CQChartsGeom::BBox    densityBBox;

  th->groupData_.groupBucketRange.clear();

  double doffset = 0.0;

  int ig = 0;
  int ng = groupData_.groupValues.size();

  int maxValues = 0;

  using Totals = std::map<int,int>;

  Totals valueSetRunningTotal;

  for (auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    // density curve per group (optionally offset)
    if      (isDensity()) {
      values->densityData.setNumSamples(densitySamples());

      std::vector<double> xvals;

      (void) getRealValues(groupInd, xvals, values->mean);

      values->densityData.setXVals(xvals);

      values->densityData.calc();

      if (! isHorizontal()) {
        densityBBox.add(values->densityData.xmin1(), values->densityData.ymin1() + doffset);
        densityBBox.add(values->densityData.xmax1(), values->densityData.ymax1() + doffset);
      }
      else {
        densityBBox.add(values->densityData.ymin1() + doffset, values->densityData.xmin1());
        densityBBox.add(values->densityData.ymax1() + doffset, values->densityData.xmax1());
      }

      doffset += densityOffset();
    }
    // scatter is grid of num groups by num value sets
    else if (isScatter()) {
      maxValues = std::max(maxValues, int(values->bucketValues.size()));
    }
    // normal is bar per value set
    else {
      maxValues = std::max(maxValues, int(values->bucketValues.size()));

      (void) getMeanValue(groupInd, values->mean);

      int iv = 0;

      for (auto &bucketValues : values->bucketValues) {
        int              bucket   = bucketValues.first;
        VariantIndsData &varsData = bucketValues.second;

        // update bucket range
        bucketRange.add(bucket);

        th->groupData_.groupBucketRange[groupInd].add(bucket);

        //---

        // update max n per value set
        int n = varsData.inds.size();

        nRange.add(n);

        //---

        // update min/max per value set
        if      (isValueCount()) {
          valueRange.add(n);
        }
        else if (isValueRange()) {
          calcVarIndsData(varsData);

          valueRange.add(varsData.min);
          valueRange.add(varsData.max);
        }
        else if (isValueMin()) {
          calcVarIndsData(varsData);

          valueRange.add(varsData.min);
        }
        else if (isValueMax()) {
          calcVarIndsData(varsData);

          valueRange.add(varsData.max);
        }
        else if (isValueMean()) {
          calcVarIndsData(varsData);

          valueRange.add(varsData.mean);
        }
        else if (isValueSum()) {
          calcVarIndsData(varsData);

          valueRange.add(varsData.sum);
        }

        //---

        ++i2;

        //---

        valueSetRunningTotal[iv] += n;

        ++iv;
      }
    }

    ++ig;
  }

  //---

  // set range
  CQChartsGeom::Range dataRange;

  if      (isDensity()) {
    if (densityBBox.isSet()) {
      dataRange.updateRange(densityBBox.getXMin(), densityBBox.getYMin());
      dataRange.updateRange(densityBBox.getXMax(), densityBBox.getYMax());
    }
    else {
      dataRange.updateRange(0, 0);
      dataRange.updateRange(1, 1);
    }
  }
  else if (isScatter()) {
    dataRange.updateRange(   - 0.5,           - 0.5);
    dataRange.updateRange(ng - 0.5, maxValues - 0.5);
  }
  else {
    double n1 = 0.0, n2 = 0.0;

    if      (isValueCount()) {
      n1 = 0;
      n2 = std::max(nRange.max(), 1);
    }
    else if (isValueRange()) {
      n1 = valueRange.min(0);
      n2 = valueRange.max(0);
    }
    else if (isValueMin()) {
      n1 = 0;
      n2 = valueRange.max(0);
    }
    else if (isValueMax()) {
      n1 = 0;
      n2 = valueRange.max(0);
    }
    else if (isValueMean()) {
      n1 = 0;
      n2 = valueRange.max(0);
    }
    else if (isValueSum()) {
      n1 = 0;
      n2 = valueRange.max(0);
    }

    if      (isOverlay()) {
      int bucket1 = bucketRange.min(0);
      int bucket2 = bucketRange.max(0);

      if (! isHorizontal()) {
        dataRange.updateRange(bucket1 - 1.0, n1);
        dataRange.updateRange(bucket2 + 1.0, n2);
      }
      else {
        dataRange.updateRange(n1, bucket1 - 1.0);
        dataRange.updateRange(n2, bucket2 + 1.0);
      }
    }
    else if (! isSkipEmpty()) {
      if      (isStacked() && isValueCount()) {
        int n = 0;

        for (const auto &gt : valueSetRunningTotal)
          n = std::max(n, gt.second);

        if (! isHorizontal()) {
          if (isPercent()) {
            dataRange.updateRange(          - 0.5, 0);
            dataRange.updateRange(maxValues - 0.5, 1);
          }
          else {
            dataRange.updateRange(          - 0.5, 0);
            dataRange.updateRange(maxValues - 0.5, n);
          }
        }
        else {
          if (isPercent()) {
            dataRange.updateRange(0,           - 0.5);
            dataRange.updateRange(1, maxValues - 0.5);
          }
          else {
            dataRange.updateRange(0,           - 0.5);
            dataRange.updateRange(n, maxValues - 0.5);
          }
        }
      }
      else if (isSideBySide()) {
        if (! isHorizontal()) {
          if (isPercent() && isValueCount()) {
            dataRange.updateRange(          - 0.5, 0);
            dataRange.updateRange(maxValues - 0.5, 1);
          }
          else {
            dataRange.updateRange(          - 0.5, n1);
            dataRange.updateRange(maxValues - 0.5, n2);
          }
        }
        else {
          if (isPercent() && isValueCount()) {
            dataRange.updateRange(0,           - 0.5);
            dataRange.updateRange(1, maxValues - 0.5);
          }
          else {
            dataRange.updateRange(n1,           - 0.5);
            dataRange.updateRange(n2, maxValues - 0.5);
          }
        }
      }
      else {
        int nb = 0;

        for (auto &bucketRange : groupData_.groupBucketRange)
          nb += bucketRange.second.max(0) - bucketRange.second.min(0) + 1;

        int i1 = 0;

        if (! isHorizontal()) {
          if (isPercent() && isValueCount()) {
            dataRange.updateRange(i1 - 1.0, 0);
            dataRange.updateRange(nb      , 1);
          }
          else {
            dataRange.updateRange(i1 - 1.0, n1);
            dataRange.updateRange(nb      , n2);
          }
        }
        else {
          if (isPercent() && isValueCount()) {
            dataRange.updateRange(0, i1 - 1.0);
            dataRange.updateRange(1, nb      );
          }
          else {
            dataRange.updateRange(n1, i1 - 1.0);
            dataRange.updateRange(n2, nb      );
          }
        }
      }
    }
    else {
      int i1 = 0;

      if (! isHorizontal()) {
        dataRange.updateRange(i1 - 1.0, 0 );
        dataRange.updateRange(i2      , n2);
      }
      else {
        dataRange.updateRange(0 , i1 - 1.0);
        dataRange.updateRange(n2, i2      );
      }
    }
  }

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
  auto pg = groupData_.groupValues.find(groupInd);

  if (pg != groupData_.groupValues.end())
    return (*pg).second;

  //---

  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  CQChartsValueSet *valueSet = new CQChartsValueSet(this);

  valueSet->setColumn(ind.column);

  auto pg1 = th->groupData_.groupValues.insert(groupData_.groupValues.end(),
               GroupValues::value_type(groupInd, new Values(valueSet)));

  return (*pg1).second;
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

    bbox += CQChartsUtil::fromQPoint(p1);
    bbox += CQChartsUtil::fromQPoint(p2);
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

  double r = value.toReal(&ok);

  if (! ok)
    return true;

  const Filters &filters = filterStack_.back();

  for (const auto &filter : filters) {
    if (groupInd == filter.groupInd && r >= filter.minValue && r < filter.maxValue)
      return true;
  }

  return false;
}

int
CQChartsDistributionPlot::
calcBucket(int groupInd, double value) const
{
  const CQBucketer &bucketer = groupBucketer(groupInd);

  if (filterStack_.empty()) {
    if (! isBucketed())
      return -1;

    if (isAutoBucket())
      return bucketer.autoRealBucket(value);
    else
      return bucketer.realBucket(value);
  }
  else {
    return bucketer.autoRealBucket(value);
  }
}

//------

bool
CQChartsDistributionPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsDistributionPlot::createObjs");

  NoUpdate noUpdate(const_cast<CQChartsDistributionPlot *>(this));

  //---

  // init color value set
//initValueSets();

  //---

  valueAxis()->clearTickLabels();
  countAxis()->clearTickLabels();

  if      (isDensity()) {
    valueAxis()->setIntegral           (false);
    valueAxis()->setGridMid            (false);
    valueAxis()->setMajorIncrement     (0);
    valueAxis()->setMinorTicksDisplayed(true);
    valueAxis()->setRequireTickLabel   (false);

    countAxis()->setIntegral           (false);
    countAxis()->setGridMid            (false);
    countAxis()->setMajorIncrement     (0);
    countAxis()->setMinorTicksDisplayed(true);
    countAxis()->setRequireTickLabel   (false);
  }
  else if (isScatter()) {
    valueAxis()->setIntegral           (true);
    valueAxis()->setGridMid            (false);
    valueAxis()->setMajorIncrement     (1);
    valueAxis()->setMinorTicksDisplayed(false);
    valueAxis()->setRequireTickLabel   (false);

    countAxis()->setIntegral           (true);
    countAxis()->setGridMid            (false);
    countAxis()->setMajorIncrement     (1);
    countAxis()->setMinorTicksDisplayed(false);
    countAxis()->setRequireTickLabel   (false);
  }
  else {
    valueAxis()->setIntegral           (true);
    valueAxis()->setGridMid            (true);
    valueAxis()->setMajorIncrement     (1);
    valueAxis()->setMinorTicksDisplayed(false);
    valueAxis()->setRequireTickLabel   (true );

    if (isValueCount())
      countAxis()->setIntegral(true);
    else
      countAxis()->setIntegral(false);

    countAxis()->setGridMid            (false);
    countAxis()->setMajorIncrement     (0);
    countAxis()->setMinorTicksDisplayed(false);
    countAxis()->setRequireTickLabel   (false);
  }

  //---

  int gpos = 0;
  int vpos = 0;

  int ng = groupData_.groupValues.size();

  int offset = 0;
  int count  = 0;

  double doffset = 0.0;

  using Totals = std::map<int,int>;

  Totals valueSetRunningTotal, groupTotals, valueSetTotals, groupMax;

  double barWidth = 1.0;

  if (isSideBySide() && ng > 0)
    barWidth /= ng;

  //---

  // get sorted buckets if needed
  using Buckets           = std::vector<int>;
  using CountBuckets      = std::map<int,Buckets>;
  using GroupCountBuckets = std::map<int,CountBuckets>;
  using GroupBuckets      = std::map<int,Buckets>;

  GroupCountBuckets groupCountBuckets;
  GroupBuckets      groupSortedBuckets;

  if (isSorted()) {
    for (auto &groupValues : groupData_.groupValues) {
      int           groupInd = groupValues.first;
      const Values *values   = groupValues.second;

      for (const auto &bucketValues : values->bucketValues) {
        int                    bucket   = bucketValues.first;
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

  //---

  auto getSortedBuckets = [&](int groupInd) {
    if (isStacked() || isSideBySide()) {
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
        if (ng == 1 && isSetHidden(iv)) { ++iv; continue; }

        //---

        const VariantIndsData &varsData = bucketValues.second;

        //---

        const VariantIndsData *pVarsData = &varsData;

        BarValue barValue = varIndsValue(*pVarsData);

        groupTotals   [ig] += barValue.n2;
        valueSetTotals[iv] += barValue.n2;

        if (barValue.n2 > groupMax[ig])
          groupMax[ig] = barValue.n2;

        ++iv;
      }

      ++ig;
    }
  }

  //---

  int ig = 0;

  for (auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int vpos1 = 0;

    //---

    int           groupInd = groupValues.first;
    const Values *values   = groupValues.second;

    //---

    // density curve per group (optionally offset)
    if      (isDensity()) {
      CQChartsDistributionDensityObj::Data data;

      data.points = values->densityData.opoints();
      data.xmin   = values->densityData.xmin1();
      data.xmax   = values->densityData.xmax1();
      data.ymin   = values->densityData.ymin1();
      data.ymax   = values->densityData.ymax1();
      data.mean   = values->mean;

      //---

      data.buckets.clear();

      for (auto &bucketValues : values->bucketValues) {
        int                    bucket   = bucketValues.first;
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
          new CQChartsDistributionDensityObj(this, bbox, groupInd, data, doffset, ig, ng);

        objs.push_back(barObj);
      }

      doffset += densityOffset();
    }
    else if (isScatter()) {
      int iv = 0;
      int nv = values->bucketValues.size();

      for (auto &bucketValues : values->bucketValues) {
        int                    bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        //---

        const VariantIndsData *pVarsData = &varsData;

        int sbucket = bucket;

        if (isSorted()) {
          const Buckets &sortedBuckets = getSortedBuckets(groupInd);

          sbucket = sortedBuckets[iv];

          auto p = values->bucketValues.find(sbucket);
          assert(p != values->bucketValues.end());

          const VariantIndsData &varsData1 = (*p).second;

          pVarsData = &varsData1;
        }

        //---

        int n = pVarsData->inds.size();

        CQChartsGeom::BBox bbox = CQChartsGeom::BBox(ig - 0.5, iv - 0.5, ig + 0.5, iv + 0.5);

        CQChartsDistributionScatterObj *scatterObj =
          new CQChartsDistributionScatterObj(this, bbox, groupInd, sbucket, n, ig, ng, iv, nv);

        objs.push_back(scatterObj);

        QString bucketName = bucketStr(groupInd, bucket);

        countAxis()->setTickLabel(iv, bucketName);

        ++iv;
      }

      QString groupName = groupIndName(groupInd);

      valueAxis()->setTickLabel(ig, groupName);
    }
    else {
      auto pb = groupData_.groupBucketRange.find(groupInd);

      if (groupData_.groupBucketRange.empty())
        continue;

      const CQChartsGeom::IMinMax &bucketRange = (*pb).second;

      if (! isOverlay() && ! isSkipEmpty())
        offset = -bucketRange.min(0);

      //---

      bool isNumeric = values->valueSet->isNumeric();

      //---

      int iv = 0;
      int nv = values->bucketValues.size();

      for (auto &bucketValues : values->bucketValues) {
        if (ng == 1 && isSetHidden(iv)) { ++iv; continue; }

        //---

        int                    bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        //---

        const VariantIndsData *pVarsData = &varsData;

        int sbucket = bucket;

        if (isSorted()) {
          const Buckets &sortedBuckets = getSortedBuckets(groupInd);

          sbucket = sortedBuckets[iv];

          auto p = values->bucketValues.find(sbucket);
          assert(p != values->bucketValues.end());

          const VariantIndsData &varsData1 = (*p).second;

          pVarsData = &varsData1;
        }

        //---

        int bucket1 = bucket + offset + count;

        //if (! isValueCount() && ! isNumeric)
        //  continue;

        BarValue barValue = varIndsValue(*pVarsData);

        //---

        double scale = 1.0;

        if (isPercent() && isValueCount()) {
          if (isStacked() || isSideBySide()) {
            int valueSetTotal = valueSetTotals[iv];

            if (valueSetTotal > 0)
              scale = 1.0/valueSetTotal;
          }
          else {
            int max = groupMax[ig];

            if (max > 0)
              scale = 1.0/max;
          }
        }

        //---

        CQChartsGeom::BBox bbox;

        if      (isOverlay()) {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(bucket - 0.5, barValue.n1, bucket + 0.5, barValue.n2);
          else
            bbox = CQChartsGeom::BBox(barValue.n1, bucket - 0.5, barValue.n2, bucket + 0.5);
        }
        else if (! isSkipEmpty()) {
          if      (isStacked() && isValueCount()) {
            int total = valueSetRunningTotal[iv];

            double v1 = (barValue.n1 + total)*scale;
            double v2 = (barValue.n2 + total)*scale;

            if (! isHorizontal())
              bbox = CQChartsGeom::BBox(vpos1 - 0.5, v1, vpos1 + 0.5, v2);
            else
              bbox = CQChartsGeom::BBox(v1, vpos1 - 0.5, v2, vpos1 + 0.5);
          }
          else if (isSideBySide()) {
            double tpos1 = vpos1 - 0.5 + barWidth*gpos;
            double tpos2 = tpos1 + barWidth;

            if (! isHorizontal())
              bbox = CQChartsGeom::BBox(tpos1, scale*barValue.n1, tpos2, scale*barValue.n2);
            else
              bbox = CQChartsGeom::BBox(scale*barValue.n1, tpos1, scale*barValue.n2, tpos2);
          }
          else {
            if (! isHorizontal())
              bbox = CQChartsGeom::BBox(bucket1 - 0.5, scale*barValue.n1,
                                        bucket1 + 0.5, scale*barValue.n2);
            else
              bbox = CQChartsGeom::BBox(scale*barValue.n1, bucket1 - 0.5,
                                        scale*barValue.n2, bucket1 + 0.5);
          }
        }
        else {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(vpos - 0.5, scale*barValue.n1,
                                      vpos + 0.5, scale*barValue.n2);
          else
            bbox = CQChartsGeom::BBox(scale*barValue.n1, vpos - 0.5,
                                      scale*barValue.n2, vpos + 0.5);
        }

        CQChartsDistributionBarObj *barObj =
          new CQChartsDistributionBarObj(this, bbox, groupInd, sbucket, barValue, ig, ng, iv, nv);

        objs.push_back(barObj);

        //---

        if      (! isBucketed()) {
          QString groupName = groupIndName(groupInd);

          valueAxis()->setTickLabel(bucket1, groupName);
        }
        else if (isNumeric) {
          if (valueAxis()->tickLabelPlacement() == CQChartsAxis::TickLabelPlacement::MIDDLE) {
            QString bucketStr = bucketValuesStr(groupInd, sbucket, values);

            if      (isOverlay())
              valueAxis()->setTickLabel(bucket, bucketStr);
            else if (! isSkipEmpty()) {
              if      (isStacked())
                valueAxis()->setTickLabel(vpos1, bucketStr);
              else if (isSideBySide())
                valueAxis()->setTickLabel(vpos1, bucketStr);
              else
                valueAxis()->setTickLabel(bucket1, bucketStr);
            }
            else
              valueAxis()->setTickLabel(vpos, bucketStr);
          }
          else {
            QString bucketStr1 = bucketValuesStr(groupInd, sbucket, values, BucketValueType::START);
            QString bucketStr2 = bucketValuesStr(groupInd, sbucket, values, BucketValueType::END  );

            if      (isOverlay()) {
              valueAxis()->setTickLabel(bucket    , bucketStr1);
              valueAxis()->setTickLabel(bucket + 1, bucketStr2);
            }
            else if (! isSkipEmpty()) {
              if      (isStacked()) {
                valueAxis()->setTickLabel(vpos1    , bucketStr1);
                valueAxis()->setTickLabel(vpos1 + 1, bucketStr2);
              }
              else if (isSideBySide()) {
                valueAxis()->setTickLabel(vpos1    , bucketStr1);
                valueAxis()->setTickLabel(vpos1 + 1, bucketStr2);
              }
              else {
                valueAxis()->setTickLabel(bucket1    , bucketStr1);
                valueAxis()->setTickLabel(bucket1 + 1, bucketStr2);
              }
            }
            else {
              valueAxis()->setTickLabel(vpos    , bucketStr1);
              valueAxis()->setTickLabel(vpos + 1, bucketStr2);
            }
          }
        }
        else {
          QString bucketStr = bucketValuesStr(groupInd, sbucket, values, BucketValueType::START);

          if      (isOverlay())
            valueAxis()->setTickLabel(bucket, bucketStr);
          else if (! isSkipEmpty()) {
            if      (isStacked())
              valueAxis()->setTickLabel(vpos1, bucketStr);
            else if (isSideBySide())
              valueAxis()->setTickLabel(vpos1, bucketStr);
            else
              valueAxis()->setTickLabel(bucket1, bucketStr);
          }
          else
            valueAxis()->setTickLabel(vpos, bucketStr);
        }

        //---

        valueSetRunningTotal[iv] += barValue.n2;

        ++iv;

        ++vpos;
        ++vpos1;
      }

      //---

      if (! isOverlay() && ! isSkipEmpty())
        count += bucketRange.max(0) - bucketRange.min(0) + 1;
    }

    ++ig;

    ++gpos;
  }

  //---

  auto setXLabel = [&]() {
    if (xLabel().length())
      valueAxis()->setLabel(xLabel());
    else {
      CQChartsColumn c = valueColumns().column();

      bool ok;

      QString xname = modelHeaderString(c, ok);

      if (ok)
        valueAxis()->setLabel(xname);
    }
  };

  auto setXGroupLabel = [&]() {
    if (xLabel().length())
      valueAxis()->setLabel(xLabel());
    else {
      CQChartsColumn c = groupColumn().column();

      bool ok;

      QString xname = modelHeaderString(c, ok);

      if (ok)
        valueAxis()->setLabel(xname);
    }
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
  else if (isScatter()) {
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

    if      (isValueCount()) {
      if (isPercent())
        setCountLabel("Percent");
      else
        setCountLabel("Count");
    }
    else if (isValueRange())
      setCountLabel("Range");
    else if (isValueMin())
      setCountLabel("Min");
    else if (isValueMax())
      setCountLabel("Min");
    else if (isValueMean())
      setCountLabel("Mean");
    else if (isValueSum())
      setCountLabel("Sum");
  }

  if (yLabel().length())
    countAxis()->setLabel(yLabel());

  //---

  return true;
}

void
CQChartsDistributionPlot::
calcVarIndsData(VariantIndsData &varInds) const
{
  CQChartsGeom::RMinMax valueRange;

  int n = varInds.inds.size();

  double sum = 0.0;
  int    n1  = 0;

  for (int i = 0; i < n; ++i) {
    const VariantInd &var = varInds.inds[i];

    double r  = 0.0;
    bool   ok = false;

    if (var.dvar.isValid())
      r = var.dvar.toReal(&ok);

    if (! ok)
      r = var.var.toReal(&ok);

    if (! ok)
      continue;

    sum += r;

    valueRange.add(r);

    ++n1;
  }

  varInds.min  = valueRange.min(0);
  varInds.max  = valueRange.max(0);
  varInds.mean = (n1 > 0 ? sum/n1 : 0.0);
  varInds.sum  = sum;
}

CQChartsDistributionPlot::BarValue
CQChartsDistributionPlot::
varIndsValue(const VariantIndsData &varInds) const
{
  BarValue barValue;

  if      (isValueCount()) {
    barValue.n1 = 0;
    barValue.n2 = varInds.inds.size();
  }
  else if (isValueRange()) {
    barValue.n1 = varInds.min;
    barValue.n2 = varInds.max;
  }
  else if (isValueMin()) {
    barValue.n1 = 0;
    barValue.n2 = varInds.min;
  }
  else if (isValueMax()) {
    barValue.n1 = 0;
    barValue.n2 = varInds.max;
  }
  else if (isValueMean()) {
    barValue.n1 = 0;
    barValue.n2 = varInds.mean;
  }
  else if (isValueSum()) {
    barValue.n1 = 0;
    barValue.n2 = varInds.sum;
  }

  return barValue;
}

void
CQChartsDistributionPlot::
getInds(int groupInd, int bucket, VariantInds &inds) const
{
  const Values *values = getGroupValues(groupInd);
  if (! values) return;

  auto pb = values->bucketValues.find(bucket);
  if (pb == values->bucketValues.end()) return;

  inds = (*pb).second.inds;
}

void
CQChartsDistributionPlot::
getXVals(int groupInd, int bucket, std::vector<double> &xvals) const
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
getRealValues(int groupInd, std::vector<double> &vals, double &mean) const
{
  vals.clear();

  mean = 0.0;

  const Values *values = getGroupValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == CQBaseModelType::INTEGER) {
    const CQChartsIValues &ivals = values->valueSet->ivals();

    mean = ivals.mean();

    std::vector<double> xvals;

    for (int i = 0; i < ivals.size(); ++i)
      vals.push_back(*ivals.value(i));
  }
  else if (values->valueSet->type() == CQBaseModelType::REAL) {
    const CQChartsRValues &rvals = values->valueSet->rvals();

    mean = rvals.mean();

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
getMeanValue(int groupInd, double &mean) const
{
  mean = 0.0;

  const Values *values = getGroupValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == CQBaseModelType::INTEGER) {
    const CQChartsIValues &ivals = values->valueSet->ivals();

    mean = ivals.mean();
  }
  else if (values->valueSet->type() == CQBaseModelType::REAL) {
    const CQChartsRValues &rvals = values->valueSet->rvals();

    mean = rvals.mean();
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
  return (! isHorizontal() ? xAxis_ : yAxis_);
}

CQChartsAxis *
CQChartsDistributionPlot::
countAxis() const
{
  return (! isHorizontal() ? yAxis_ : xAxis_);
}

void
CQChartsDistributionPlot::
addKeyItems(CQChartsPlotKey *key)
{
  int row = key->maxRow();

  auto addKeyRow = [&](int i, int n, const QString &name) {
    CQChartsDistKeyColorBox *keyColor = new CQChartsDistKeyColorBox(this, i, n);
    CQChartsKeyText         *keyText  = new CQChartsKeyText        (this, name, i, n);

    key->addItem(keyColor, row, 0);
    key->addItem(keyText , row, 1);

    ++row;

    return std::pair<CQChartsDistKeyColorBox *,CQChartsKeyText*>(keyColor, keyText);
  };

  //---

  int ng = groupData_.groupValues.size();

  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupValues : groupData_.groupValues) {
      int           groupInd = groupValues.first;
    //const Values *values   = groupValues.second;

      QString groupName = groupIndName(groupInd);

      addKeyRow(ig, ng, groupName);

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

        CQChartsDistKeyColorBox *colorBox = addKeyRow(iv, nv, value.toString()).first;

        bool ok;

        CQChartsColor c = CQChartsVariant::toColor(value, ok);

        if (ok) {
          QColor c1 = charts()->interpColor(c, 0, 1);

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
        int                    bucket   = bucketValues.first;
      //const VariantIndsData &varsData = bucketValues.second;

        QString bucketName = bucketValuesStr(groupInd, bucket, values);

        addKeyRow(iv, nv, bucketName);

        ++iv;
      }
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int groupInd, int bucket, BucketValueType type) const
{
  for (const auto &groupValues : groupData_.groupValues) {
    if (groupInd == groupValues.first)
      return bucketValuesStr(groupInd, bucket, groupValues.second, type);
  }

  return "";
}

QString
CQChartsDistributionPlot::
bucketStr(int groupInd, int bucket, BucketValueType type) const
{
  return bucketValuesStr(groupInd, bucket, nullptr, type);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int groupInd, int bucket, const Values *values, BucketValueType type) const
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
        int ivalue1 = value1;
        int ivalue2 = value2 - 1;

        if (ivalue1 != ivalue2)
          return bucketer.bucketName(ivalue1, ivalue2, /*arrow*/true);
        else
          return QString("%1").arg(ivalue1);
      }
      else {
        return bucketer.bucketName(value1, value2, /*arrow*/true);
      }
    }
    else if (type == BucketValueType::START)
      return QString("%1").arg(value1);
    else
      return QString("%1").arg(value2);
  }
  else {
    return bucketer.bucketString(bucket);

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
bucketValues(int groupInd, int bucket, double &value1, double &value2) const
{
  value1 = 0.0;
  value2 = 0.0;

  if (! isBucketed())
    return;

  const CQBucketer &bucketer = groupBucketer(groupInd);

  bool isAuto = (! filterStack_.empty() || isAutoBucket());

  if (isAuto)
    bucketer.autoBucketValues(bucket, value1, value2);
  else
    bucketer.bucketRValues(bucket, value1, value2);

  if (CMathUtil::isZero(value1)) value1 = 0.0;
  if (CMathUtil::isZero(value2)) value2 = 0.0;
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
  if (isStacked() || isSideBySide() || isScatter())
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

  QMenu *typeMenu = new QMenu("Plot Type");

  (void) addMenuCheckedAction(typeMenu, "Normal"    , isNormal    (), SLOT(setNormal    (bool)));
  (void) addMenuCheckedAction(typeMenu, "Stacked"   , isStacked   (), SLOT(setStacked   (bool)));
  (void) addMenuCheckedAction(typeMenu, "SideBySide", isSideBySide(), SLOT(setSideBySide(bool)));
  (void) addMenuCheckedAction(typeMenu, "Overlay"   , isOverlay   (), SLOT(setOverlay   (bool)));
  (void) addMenuCheckedAction(typeMenu, "Scatter"   , isScatter   (), SLOT(setScatter   (bool)));
  (void) addMenuCheckedAction(typeMenu, "Density"   , isDensity   (), SLOT(setDensity   (bool)));

  menu->addMenu(typeMenu);

  QMenu *valueMenu = new QMenu("Value Type");

  (void) addMenuCheckedAction(valueMenu, "Count", isValueCount(), SLOT(setValueCount(bool)));
  (void) addMenuCheckedAction(valueMenu, "Range", isValueRange(), SLOT(setValueRange(bool)));
  (void) addMenuCheckedAction(valueMenu, "Min"  , isValueMin  (), SLOT(setValueMin  (bool)));
  (void) addMenuCheckedAction(valueMenu, "Max"  , isValueMax  (), SLOT(setValueMax  (bool)));
  (void) addMenuCheckedAction(valueMenu, "Mean" , isValueMean (), SLOT(setValueMean (bool)));
  (void) addMenuCheckedAction(valueMenu, "Sum"  , isValueSum  (), SLOT(setValueSum  (bool)));

  menu->addMenu(valueMenu);

  (void) addCheckedAction("Percent"   , isPercent  (), SLOT(setPercent(bool)));
  (void) addCheckedAction("Skip Empty", isSkipEmpty(), SLOT(setSkipEmpty(bool)));

  //---

  menu->addSeparator();

  (void) addCheckedAction("Sorted"   , isSorted  (), SLOT(setSorted  (bool)));
  (void) addCheckedAction("Dot Lines", isDotLines(), SLOT(setDotLines(bool)));
  (void) addCheckedAction("Rug"      , isRug     (), SLOT(setRug     (bool)));
  (void) addCheckedAction("Mean Line", isShowMean(), SLOT(setShowMean(bool)));

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

  if (! isShowMean())
    return false;

  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsDistributionPlot::
drawForeground(QPainter *painter) const
{
  if (! hasForeground())
    return;

  if (isShowMean())
    drawMeanLine(painter);
}

void
CQChartsDistributionPlot::
drawMeanLine(QPainter *painter) const
{
  // set pen
  QColor bc = interpMeanLinesColor(0, 1);

  QPen pen;

  setPen(pen, true, bc, meanLinesAlpha(), meanLinesWidth(), meanLinesDash());

  painter->setPen(pen);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  int ig = 0;
  int ng = groupData_.groupValues.size();

  for (const auto &groupValues : groupData_.groupValues) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int           groupInd = groupValues.first;
    const Values *values   = groupValues.second;

    double mean  = values->mean;
    double mean1 = mean;

    //---

    for (const auto &plotObj : plotObjects()) {
      const CQChartsDistributionBarObj *barObj =
        dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

      if (! barObj)
        continue;

      int groupInd1 = barObj->groupInd();

      if (groupInd1 != groupInd)
        continue;

      mean1 = barObj->mapValue(mean);

      break;
    }

    //---

    QPointF p1, p2;

    if (! isHorizontal()) {
      p1 = windowToPixel(QPointF(mean1, dataRange.ymin()));
      p2 = windowToPixel(QPointF(mean1, dataRange.ymax()));
    }
    else {
      p1 = windowToPixel(QPointF(dataRange.xmin(), mean1));
      p2 = windowToPixel(QPointF(dataRange.xmax(), mean1));
    }

    painter->drawLine(p1, p2);

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

    CQChartsGeom::Point w;

    pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

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

  queueUpdateRangeAndObjs();
}

void
CQChartsDistributionPlot::
popSlot()
{
  if (! filterStack_.empty()) {
    filterStack_.pop_back();

    queueUpdateRangeAndObjs();
  }
}

void
CQChartsDistributionPlot::
popTopSlot()
{
  if (! filterStack_.empty()) {
    filterStack_.clear();

    queueUpdateRangeAndObjs();
  }
}

//------

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                           int groupInd, int bucket, const BarValue &barValue,
                           int is, int ns, int iv, int nv) :
 CQChartsPlotObj(const_cast<CQChartsDistributionPlot *>(plot), rect), plot_(plot),
 groupInd_(groupInd), bucket_(bucket), barValue_(barValue), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);

  plot_->bucketValues(groupInd_, bucket_, value1_, value2_);
}

QString
CQChartsDistributionBarObj::
calcId() const
{
  return QString("bar:%1:%2").arg(is_).arg(iv_);
}

QString
CQChartsDistributionBarObj::
calcTipId() const
{
  QString groupName = this->groupName();
  QString bucketStr = this->bucketStr();

  CQChartsTableTip tableTip;

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  tableTip.addTableRow("Bucket", bucketStr);

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

    if (name == "")
      name = "Colors";

    tableTip.addTableRow(name, strs.join(" "));
  }

  return tableTip.str();
}

QString
CQChartsDistributionBarObj::
groupName() const
{
  QString groupName;

  if (ns_ > 1)
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

  QRectF qrect = CQChartsUtil::toQRect(rect);

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
draw(QPainter *painter)
{
  CQChartsGeom::BBox pbbox = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(pbbox);

  //---

  bool useLine = false;

  if (! plot_->isDotLines()) {
    double s = (! plot_->isHorizontal() ? qrect.width() : qrect.height());

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
    double size = (! plot_->isHorizontal() ? qrect.height() : qrect.width());

    double dsize = size/colorData_.nv;

    double pos1 = 0.0, pos2 = 0.0;

    for (auto &p : colorData_.colorSet) {
      const CQChartsColor &color = p.first;
      int                  n     = colorData_.colorCount[p.second];

      pos1 = pos2;
      pos2 = pos1 + dsize*n;

      QRectF qrect1;

      if (! plot_->isHorizontal())
        qrect1 = QRectF(qrect.x(), qrect.bottom() - pos2, qrect.width(), pos2 - pos1);
      else
        qrect1 = QRectF(qrect.left() + pos1, qrect.y(), pos2 - pos1, qrect.height());

      //---

      drawRect(painter, qrect1, color, useLine);
    }
  }
  else {
    QColor barColor = this->barColor();

    drawRect(painter, qrect, barColor, useLine);
  }

  if (! image.isNull())
    painter->drawImage(qrect, image);
}

void
CQChartsDistributionBarObj::
drawFg(QPainter *painter) const
{
  CQChartsGeom::BBox pbbox = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(pbbox);

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

  plot_->dataLabel()->draw(painter, qrect, ystr);

  //---

  if (plot_->isRug())
    drawRug(painter);
}

void
CQChartsDistributionBarObj::
drawRug(QPainter *painter) const
{
  // get symbol and size
  CQChartsSymbol symbol = plot_->rugSymbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = (! plot_->isHorizontal() ? CQChartsSymbol::Type::VLINE :
                                        CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);

  //---

  // set pen and brush
  int ic = (ns_ > 1 ? is_ : iv_);
  int nc = (ns_ > 1 ? ns_ : nv_);

  QPen   pen;
  QBrush brush;

  plot_->setRugSymbolPenBrush(pen, brush, ic, nc);

  painter->setPen  (pen);
  painter->setBrush(brush);

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

    plot_->drawSymbol(painter, ps, symbol, CMathUtil::avg(sx, sy), pen, brush);
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

  // get normal bar color
  QColor barColor = this->barColor();

  // get color of individual values
  colorData.nv = 0;

  // get count of unique colors for values
  CQChartsDistributionPlot::VariantInds vinds;

  plot_->getInds(groupInd_, bucket_, vinds);

  for (const auto &vind : vinds) {
    const CQChartsModelIndex &ind = vind.ind;

    CQChartsColor color;

    if (plot_->columnColor(ind.row, ind.parent, color)) {
      QColor c1 = plot_->charts()->interpColor(color, 0, 1);

      c1.setAlphaF(plot_->barFillAlpha());

      color = c1;
    }
    else
      color = barColor;

    auto p = colorData.colorSet.find(color);

    if (p == colorData.colorSet.end()) {
      colorData.colorRows.insert(ind.row);

      int ind = colorData.colorSet.size();

      p = colorData.colorSet.insert(p, ColorSet::value_type(color, ind));
    }

    ++colorData.colorCount[(*p).second];

    ++colorData.nv;
  }

  if (colorData.colorSet.empty()) {
    colorData.colorSet[barColor] = 0;

    colorData.colorCount[0] = 1;

    colorData.nv = 1;
  }

  return true;
}

void
CQChartsDistributionBarObj::
drawRect(QPainter *painter, const QRectF &qrect, const CQChartsColor &color, bool useLine) const
{
  // set pen and brush
  QPen   pen;
  QBrush barBrush;

  QColor bc = plot_->interpBarBorderColor(0, 1);
  QColor fc = plot_->charts()->interpColor(color, 0, 1);

  CQChartsLength bw = plot_->barBorderWidth();

  if (useLine) {
    bw = CQChartsLength("0px");

    if (plot_->isBarFilled())
      bc = fc;
  }

  plot_->setPenBrush(pen, barBrush,
    plot_->isBarBorder(), bc, plot_->barBorderAlpha(), bw, plot_->barBorderDash(),
    plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

  plot_->updateObjPenBrushState(this, pen, barBrush);

  painter->setPen(pen);
  painter->setBrush(barBrush);

  //---

  if (! plot_->isDotLines()) {
    // draw rect
    if (! useLine) {
      double cxs = plot_->lengthPixelWidth (plot_->barCornerSize());
      double cys = plot_->lengthPixelHeight(plot_->barCornerSize());

      CQChartsRoundedPolygon::draw(painter, qrect, cxs, cys);
    }
    else {
      if (! plot_->isHorizontal()) {
        double xc = qrect.center().x();

        painter->drawLine(xc, qrect.bottom(), xc, qrect.top());
      }
      else {
        double yc = qrect.center().y();

        painter->drawLine(qrect.left(), yc, qrect.right(), yc);
      }
    }
  }
  else {
    // draw line
    double lw = plot_->lengthPixelSize(plot_->dotLineWidth(), ! plot_->isHorizontal());

    if (! plot_->isHorizontal()) {
      double xc = qrect.center().x();

      if (lw < 3)
        painter->drawLine(xc, qrect.bottom(), xc, qrect.top());
      else {
        QRectF qrect1(xc - lw/2, qrect.top(), lw, qrect.height());

        CQChartsRoundedPolygon::draw(painter, qrect1, 0, 0);
      }
    }
    else {
      double yc = qrect.center().y();

      if (lw < 3)
        painter->drawLine(qrect.left(), yc, qrect.right(), yc);
      else {
        QRectF qrect1(qrect.left(), yc - lw/2, qrect.width(), lw);

        CQChartsRoundedPolygon::draw(painter, qrect1, 0, 0);
      }
    }

    //---

    // get dot symbol and size
    CQChartsSymbol symbol = plot_->dotSymbolType();

    double sx, sy;

    plot_->pixelSymbolSize(plot_->dotSymbolSize(), sx, sy);

    int ic = (ns_ > 1 ? is_ : iv_);
    int nc = (ns_ > 1 ? ns_ : nv_);

    //---

    // set dot pen and brush
    QPen   dotPen;
    QBrush dotBrush;

    plot_->setDotSymbolPenBrush(dotPen, dotBrush, ic, nc);

    painter->setPen  (dotPen);
    painter->setBrush(dotBrush);

    //---

    // draw dot
    QPointF p;

    if (! plot_->isHorizontal())
      p = QPointF(qrect.center().x(), qrect.top());
    else
      p = QPointF(qrect.right(), qrect.center().y());

    plot_->drawSymbol(painter, p, symbol, CMathUtil::avg(sx, sy));
  }
}

QColor
CQChartsDistributionBarObj::
barColor() const
{
  int ic = (ns_ > 1 ? is_ : iv_);
  int nc = (ns_ > 1 ? ns_ : nv_);

  return plot_->interpBarFillColor(ic, nc);
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
calcRect() const
{
  static double minSize = 3.0;

  //---

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect_, prect);

  double m1 = plot_->lengthPixelSize(plot_->margin(), ! plot_->isHorizontal());
  double m2 = m1;

  if (ns_ > 1) {
    if      (iv_ == 0)
      m1 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
    else if (iv_ == nv_ - 1)
      m2 = plot_->lengthPixelSize(plot_->groupMargin(), ! plot_->isHorizontal());
  }

  double rs = prect.getSize(! plot_->isHorizontal());

  double s1 = rs - 2*std::max(m1, m2);

  if (s1 < minSize) {
    m1 = (rs - minSize)/2.0;
    m2 = m1;
  }

  prect.expandExtent(-m1, -m2, ! plot_->isHorizontal());

  return prect;
}

//------

CQChartsDistributionDensityObj::
CQChartsDistributionDensityObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                               int groupInd, const Data &data, double doffset, int is, int ns) :
 CQChartsPlotObj(const_cast<CQChartsDistributionPlot *>(plot), rect), plot_(plot),
 groupInd_(groupInd), data_(data), doffset_(doffset), is_(is), ns_(ns)
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

    plot_->bucketValues(groupInd_, data_.buckets[i].bucket, value1, value2);

    double dx = (value2 - value1)/(data_.xmax - data_.xmin);

    area += dx*dy;
  }

  bucketScale_ = 1.0/area;
}

QString
CQChartsDistributionDensityObj::
calcId() const
{
  return QString("density:%1").arg(is_);
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

  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsDistributionDensityObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsDistributionDensityObj::
addColumnSelectIndex(Indices &, const CQChartsColumn &) const
{
}

void
CQChartsDistributionDensityObj::
draw(QPainter *painter)
{
  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBarBorderColor(is_, ns_);
  QColor fc = plot_->interpBarFillColor  (is_, ns_);

  plot_->setPenBrush(pen, brush,
    plot_->isBarBorder(), bc, plot_->barBorderAlpha(),
    plot_->barBorderWidth(), plot_->barBorderDash(),
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

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw bars for buckets
  if (plot_->isDensityBars()) {
    int nb = data_.buckets.size();

    for (int i = 0; i < nb; ++i) {
      double y = data_.buckets[i].n*bucketScale_;

      double value1, value2;

      plot_->bucketValues(groupInd_, data_.buckets[i].bucket, value1, value2);

      CQChartsGeom::BBox bbox(value1, 0, value2, y);

      CQChartsGeom::BBox pbbox = plot_->windowToPixel(bbox);

      painter->drawRect(CQChartsUtil::toQRect(pbbox));
    }
  }

  //---

  // draw density polygon
  QPolygonF poly;

  int np = poly_.size();

  for (int i = 0; i < np; ++i)
    poly << plot()->windowToPixel(poly_[i]);

  painter->drawPolygon(poly);
}

void
CQChartsDistributionDensityObj::
drawFg(QPainter *painter) const
{
  if (plot_->isShowMean())
    drawMeanLine(painter);

  if (plot_->isRug())
    drawRug(painter);
}

void
CQChartsDistributionDensityObj::
drawMeanLine(QPainter *painter) const
{
  // set pen
  QColor bc = plot_->interpMeanLinesColor(0, 1);

  QPen pen;

  plot_->setPen(pen, true, bc, plot_->meanLinesAlpha(),
                plot_->meanLinesWidth(), plot_->meanLinesDash());

  painter->setPen(pen);

  //---

  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  QPointF p1, p2;

  if (! plot_->isHorizontal()) {
    p1 = plot()->windowToPixel(QPointF(data_.mean, dataRange.ymin()));
    p2 = plot()->windowToPixel(QPointF(data_.mean, dataRange.ymax()));
  }
  else {
    p1 = plot()->windowToPixel(QPointF(dataRange.xmin(), data_.mean));
    p2 = plot()->windowToPixel(QPointF(dataRange.xmax(), data_.mean));
  }

  painter->drawLine(p1, p2);
}

void
CQChartsDistributionDensityObj::
drawRug(QPainter *painter) const
{
  // get symbol and size
  CQChartsSymbol symbol = plot_->rugSymbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = (! plot_->isHorizontal() ? CQChartsSymbol::Type::VLINE :
                                        CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);

  //---

  // set pen brush
  // TODO: allow control of alpha, and line width
  QPen   pen;
  QBrush brush;

  QColor fillColor = plot_->interpBarFillColor(is_, ns_);

  plot_->setPen  (pen  , true, fillColor, 1.0);
  plot_->setBrush(brush, true, fillColor, 0.5);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  std::vector<double> xvals;
  double              mean;

  (void) plot_->getRealValues(groupInd_, xvals, mean);

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

    plot_->drawSymbol(painter, ps, symbol, CMathUtil::avg(sx, sy), pen, brush);
  }
}

//------

CQChartsDistributionScatterObj::
CQChartsDistributionScatterObj(const CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                               int groupInd, int bucket, int n, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(const_cast<CQChartsDistributionPlot *>(plot), rect), plot_(plot),
 groupInd_(groupInd), bucket_(bucket), n_(n), is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);

  // get factored number of points
  int nf = CMathUtil::clamp(int(n_*plot_->scatterFactor()), 1, n_);

  // generate random points in box (0.0->1.0) with margin
  double m = 0.05;

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
  return QString("scatter:%1:%2").arg(is_).arg(iv_);
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

  return tableTip.str();
}

void
CQChartsDistributionScatterObj::
draw(QPainter *painter)
{
  // set pen brush
  // TODO: allow control of border color, alpha, and line width
  int ic = (ns_ > 1 ? is_ : iv_);
  int nc = (ns_ > 1 ? ns_ : nv_);

  QColor c = plot_->interpBarFillColor(ic, nc);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, Qt::black, 1.0);
  plot_->setBrush(brush, true, c, 1.0);

  painter->setPen(pen);
  painter->setBrush(brush);

  //---

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

#if 0
  painter->drawRect(CQChartsUtil::toQRect(prect));
#endif

  //---

  CQChartsSymbol symbol(CQChartsSymbol::Type::CIRCLE);

  double sx = 6;
  double sy = 6;

  QPointF tl = CQChartsUtil::toQRect(prect).topLeft();

  for (const auto &point : points_) {
    double px = plot_->windowToPixelWidth (point.x());
    double py = plot_->windowToPixelHeight(point.y());

    QPointF p(tl.x() + px, tl.y() + py);

    plot_->drawSymbol(painter, p, symbol, CMathUtil::avg(sx, sy), pen, brush);
  }
}

void
CQChartsDistributionScatterObj::
getSelectIndices(Indices &) const
{
}

void
CQChartsDistributionScatterObj::
addColumnSelectIndex(Indices &, const CQChartsColumn &) const
{
}

//------

CQChartsDistKeyColorBox::
CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n), plot_(plot)
{
  assert(i >= 0 && i < n);
}

bool
CQChartsDistKeyColorBox::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  setSetHidden(! isSetHidden());

  plot_->queueUpdateRangeAndObjs();

  return true;
}

QBrush
CQChartsDistKeyColorBox::
fillBrush() const
{
  if (color_.isValid())
    return plot_->charts()->interpColor(color_, 0, 1);

  QColor c = plot_->interpBarFillColor(i_, n_);

  if (isSetHidden())
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

bool
CQChartsDistKeyColorBox::
isSetHidden() const
{
  return plot_->CQChartsPlot::isSetHidden(i_);
}

void
CQChartsDistKeyColorBox::
setSetHidden(bool b)
{
  plot_->CQChartsPlot::setSetHidden(i_, b);
}
