#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsColorSet.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>
#include <CQChartsDensity.h>
#include <CQChartsRand.h>

#include <QMenu>
#include <QAction>
#include <QPainter>

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
  addColumnsParameter("value", "Value", "valueColumns", "0").
    setRequired().setTip("Value column(s)");

  addColumnParameter("name", "Name", "nameColumn").setTip("Custom group name");

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

  addColumnParameter("color", "Color", "colorColumn").setTip("Custom bar color");

  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("draw bars horizontal");
  addBoolParameter("stacked"   , "Stacked"   , "stacked"   ).setTip("Stack grouped values");
  addBoolParameter("sideBySide", "SideBySide", "sideBySide").setTip("Grouped values side by side");
  addBoolParameter("overlay"   , "Overlay"   , "overlay"   ).setTip("overlay groups");
  addBoolParameter("skipEmpty" , "SkipEmpty" , "skipEmpty" ).setTip("skip empty buckets");
  addBoolParameter("rangeBar"  , "RangeBar"  , "rangeBar"  ).setTip("show value range");
  addBoolParameter("sorted"    , "Sorted"    , "sorted"    ).setTip("sort by count");
  addBoolParameter("density"   , "Density"   , "density"   ).setTip("show density");
  addBoolParameter("scatter"   , "Scatter"   , "scatter"   ).setTip("scatter grid");
  addBoolParameter("dotLines"  , "DotLines"  , "dotLines"  ).setTip("draw bars as lines with dot");
  addBoolParameter("rug"       , "Rug"       , "rug"       ).setTip("draw rug points");

  endParameterGroup();

  //---

  // group data
  CQChartsGroupPlotType::addParameters();

  //---

  CQChartsPlotType::addParameters();
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
 CQChartsPlotMeanLineData<CQChartsDistributionPlot>(this),
 CQChartsPlotDotPointData<CQChartsDistributionPlot>(this),
 CQChartsPlotRugPointData<CQChartsDistributionPlot>(this),
 dataLabel_(this)
{
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
}

//---

void
CQChartsDistributionPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

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

  for (auto &ib : groupBucketer_)
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

  for (auto &ib : groupBucketer_)
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

  for (auto &ib : groupBucketer_)
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

  for (auto &ib : groupBucketer_)
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
  CQChartsPlot::addProperties();

  CQChartsBarPlot::addProperties();

  addProperty("columns", this, "nameColumn", "name");

  addProperty("bucket", this, "bucketed"        , "enabled");
  addProperty("bucket", this, "autoBucket"      , "auto"   );
  addProperty("bucket", this, "startBucketValue", "start"  );
  addProperty("bucket", this, "deltaBucketValue", "delta"  );
  addProperty("bucket", this, "numAutoBuckets"  , "num"    );

  addProperty("options", this, "skipEmpty");
  addProperty("options", this, "sorted"   );

  addProperty("placement", this, "stacked"   );
  addProperty("placement", this, "sideBySide");
  addProperty("placement", this, "overlay"   );

  addProperty("rangeBar", this, "rangeBar", "enabled");

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

  dataLabel_.addPathProperties("dataLabel");
}

//---

void
CQChartsDistributionPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() {
    dataLabel_.setDirection(horizontal_ ?
      CQChartsDataLabel::Direction::HORIZONTAL : CQChartsDataLabel::Direction::VERTICAL);

    updateRangeAndObjs();
  } );
}

void
CQChartsDistributionPlot::
setStacked(bool b)
{
  CQChartsUtil::testAndSet(stacked_, b, [&]() { updateRangeAndObjs(); } );

  if (b) {
    setSideBySide(false);
    setOverlay   (false);
    setScatter   (false);
    setDensity   (false);
  }
}

void
CQChartsDistributionPlot::
setSideBySide(bool b)
{
  CQChartsUtil::testAndSet(sideBySide_, b, [&]() { updateRangeAndObjs(); } );

  if (b) {
    setStacked(false);
    setOverlay(false);
    setScatter(false);
    setDensity(false);
  }
}

void
CQChartsDistributionPlot::
setOverlay(bool b)
{
  CQChartsUtil::testAndSet(overlay_, b, [&]() { updateRangeAndObjs(); } );

  if (b) {
    setSideBySide(false);
    setStacked   (false);
    setScatter   (false);
    setDensity   (false);
  }
}

void
CQChartsDistributionPlot::
setSkipEmpty(bool b)
{
  CQChartsUtil::testAndSet(skipEmpty_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setRangeBar(bool b)
{
  CQChartsUtil::testAndSet(rangeBar_, b, [&]() { updateRangeAndObjs(); } );
}

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
  CQChartsUtil::testAndSet(density_, b, [&]() { updateRangeAndObjs(); } );

  if (b) {
    setSideBySide(false);
    setStacked   (false);
    setScatter   (false);
    setOverlay   (false);
  }
}

void
CQChartsDistributionPlot::
setDensityOffset(double o)
{
  CQChartsUtil::testAndSet(densityOffset_, o, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensitySamples(int n)
{
  CQChartsUtil::testAndSet(densitySamples_, n, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityGradient(bool b)
{
  CQChartsUtil::testAndSet(densityGradient_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDensityBars(bool b)
{
  CQChartsUtil::testAndSet(densityBars_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setScatter(bool b)
{
  CQChartsUtil::testAndSet(scatter_, b, [&]() { updateRangeAndObjs(); } );

  if (b) {
    setSideBySide(false);
    setStacked   (false);
    setOverlay   (false);
    setDensity   (false);
  }
}

void
CQChartsDistributionPlot::
setScatterFactor(double f)
{
  CQChartsUtil::testAndSet(scatterFactor_, f, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
setDotLines(bool b)
{
  CQChartsUtil::testAndSet(dotLines_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDistributionPlot::
setDotLineWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(dotLineWidth_, l, [&]() { invalidateLayers(); } );
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
setShowMean(bool b)
{
  CQChartsUtil::testAndSet(showMean_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
calcRange()
{
  dataRange_.reset();

  //---

  // init grouping
  initGroupData(valueColumns(), nameColumn());

  //---

  clearGroupValues();

  //---

  // process model data (build grouped sets of values)
  class DistributionVisitor : public ModelVisitor {
   public:
    DistributionVisitor(CQChartsDistributionPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      plot_->addRow(data.parent, data.row);

      return State::OK;
    }

   private:
    CQChartsDistributionPlot *plot_ { nullptr };
  };

  DistributionVisitor distributionVisitor(this);

  visitModel(distributionVisitor);

  //---

  // bucket values in groups
  for (auto &groupValues : groupValues_) {
    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    CQBucketer &bucketer = groupBucketer(groupInd);

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

    for (auto &ind : values->inds) {
      int      bucket = -1;
      QVariant value;

      bool ok;

      if (isBucketed()) {
        if      (type == CQChartsValueSet::Type::REAL) {
          double r = modelReal(ind.row, ind.column, ind.parent, ok);
          if (! ok || CMathUtil::isNaN(r)) continue;

          bucket = calcBucket(groupInd, r);
          value  = QVariant(r);
        }
        else if (type == CQChartsValueSet::Type::INTEGER) {
          int i = modelInteger(ind.row, ind.column, ind.parent, ok);
          if (! ok) continue;

          bucket = calcBucket(groupInd, i);
          value  = QVariant(i);
        }
        else {
          QString str = modelString(ind.row, ind.column, ind.parent, ok);
          if (! ok) continue;

          bucket = bucketer.stringBucket(str);
          value  = QVariant(str);
        }
      }
      else {
        value = modelValue(ind.row, ind.column, ind.parent, ok);
        if (! ok) continue;
      }

      values->bucketValues[bucket].inds.emplace_back(value, ind);
    }
  }

  //---

  // calc range (number of bars and max height)

  int i1 = 0;
  int i2 = 0;

  CQChartsGeom::IMinMax nRange(i1);
  CQChartsGeom::IMinMax bucketRange;
  CQChartsGeom::RMinMax valueRange;
  CQChartsGeom::BBox    densityBBox;

  groupBucketRange_.clear();

  double doffset = 0.0;

  int ig = 0;
  int ng = groupValues_.size();

  int maxValues = 0;

  using Totals = std::map<int,int>;

  Totals groupTotals;

  for (auto &groupValues : groupValues_) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

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
    else if (isScatter()) {
    }
    else {
      maxValues = std::max(maxValues, int(values->bucketValues.size()));

      (void) getMeanValue(groupInd, values->mean);

      int iv = 0;

      for (auto &bucketValues : values->bucketValues) {
        int              bucket   = bucketValues.first;
        VariantIndsData &varsData = bucketValues.second;

        bucketRange.add(bucket);

        groupBucketRange_[groupInd].add(bucket);

        int n = varsData.inds.size();

        nRange.add(n);

        CQChartsGeom::RMinMax valueRange1;

        for (int j = 0; j < n; ++j) {
          const VariantInd &var = varsData.inds[j];

          bool ok;

          double r = var.var.toReal(&ok);

          if (ok)
            valueRange1.add(r);
        }

        varsData.min = valueRange1.min(0);
        varsData.max = valueRange1.max(0);

        valueRange.add(varsData.min);
        valueRange.add(varsData.max);

        ++i2;

        //---

        groupTotals[iv] += n;

        ++iv;
      }
    }

    ++ig;
  }

  //---

  // set range
  if      (isDensity()) {
    if (densityBBox.isSet()) {
      dataRange_.updateRange(densityBBox.getXMin(), densityBBox.getYMin());
      dataRange_.updateRange(densityBBox.getXMax(), densityBBox.getYMax());
    }
    else {
      dataRange_.updateRange(0, 0);
      dataRange_.updateRange(1, 1);
    }
  }
  else if (isScatter()) {
    int nv = 0;

    for (auto &groupValues : groupValues_) {
      const Values *values = groupValues.second;

      int nv1 = values->bucketValues.size();

      nv = std::max(nv, nv1);
    }

    dataRange_.updateRange(   - 0.5,    - 0.5);
    dataRange_.updateRange(ng - 0.5, nv - 0.5);
  }
  else {
    double n1, n2;

    if (isRangeBar()) {
      n1 = valueRange.min(0);
      n2 = valueRange.max(0);
    }
    else {
      n1 = 0;
      n2 = std::max(nRange.max(), 1);
    }

    if      (isOverlay()) {
      int bucket1 = bucketRange.min(0);
      int bucket2 = bucketRange.max(0);

      if (! isHorizontal()) {
        dataRange_.updateRange(bucket1 - 1.0, n1);
        dataRange_.updateRange(bucket2 + 1.0, n2);
      }
      else {
        dataRange_.updateRange(n1, bucket1 - 1.0);
        dataRange_.updateRange(n2, bucket2 + 1.0);
      }
    }
    else if (! isSkipEmpty()) {
      if      (isStacked()) {
        int n = 0;

        for (const auto &gt : groupTotals)
          n = std::max(n, gt.second);

        if (! isHorizontal()) {
          dataRange_.updateRange(          - 0.5, 0);
          dataRange_.updateRange(maxValues - 0.5, n);
        }
        else {
          dataRange_.updateRange(0,           - 0.5);
          dataRange_.updateRange(n, maxValues - 0.5);
        }
      }
      else if (isSideBySide()) {
        if (! isHorizontal()) {
          dataRange_.updateRange(          - 0.5, n1);
          dataRange_.updateRange(maxValues - 0.5, n2);
        }
        else {
          dataRange_.updateRange(n1,           - 0.5);
          dataRange_.updateRange(n2, maxValues - 0.5);
        }
      }
      else {
        int nb = 0;

        for (auto &bucketRange : groupBucketRange_)
          nb += bucketRange.second.max(0) - bucketRange.second.min(0) + 1;

        int i1 = 0;

        if (! isHorizontal()) {
          dataRange_.updateRange(i1 - 1.0, n1);
          dataRange_.updateRange(nb      , n2);
        }
        else {
          dataRange_.updateRange(n1, i1 - 1.0);
          dataRange_.updateRange(n2, nb      );
        }
      }
    }
    else {
      int i1 = 0;

      if (! isHorizontal()) {
        dataRange_.updateRange(i1 - 1.0, 0 );
        dataRange_.updateRange(i2      , n1);
      }
      else {
        dataRange_.updateRange(0 , i1 - 1.0);
        dataRange_.updateRange(n1, i2      );
      }
    }
  }
}

void
CQChartsDistributionPlot::
clearGroupValues()
{
  for (auto &groupValues : groupValues_)
    delete groupValues.second;

  groupValues_.clear();

  groupBucketer_.clear();
}

void
CQChartsDistributionPlot::
addRow(const QModelIndex &parent, int row)
{
  for (const auto &column : valueColumns())
    addRowColumn(CQChartsModelIndex(row, column, parent));
}

void
CQChartsDistributionPlot::
addRowColumn(const CQChartsModelIndex &ind)
{
  // get optional group for value
  int groupInd = rowGroupInd(ind);

  bool ok;

  QVariant value = modelValue(ind.row, ind.column, ind.parent, ok);

  //---

  // check push/pop filter
  if (! checkFilter(groupInd, value))
    return;

  //---

  // get value set for group
  auto pg = groupValues_.find(groupInd);

  if (pg == groupValues_.end()) {
    ValueSet *valueSet = new ValueSet(this);

    valueSet->setColumn(ind.column);

    pg = groupValues_.insert(pg, GroupValues::value_type(groupInd, new Values(valueSet)));
  }

  Values *values = (*pg).second;

  //---

  // add value
  if (ok)
    values->valueSet->addValue(value);

  values->inds.push_back(ind);
}

//------

CQChartsGeom::BBox
CQChartsDistributionPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  // add data labels
  CQChartsDataLabel::Position position = dataLabel().position();

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
  CQBucketer &bucketer = groupBucketer(groupInd);

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
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  // init color value set
  initValueSets();

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

    countAxis()->setIntegral           (true);
    countAxis()->setGridMid            (false);
    countAxis()->setMajorIncrement     (0);
    countAxis()->setMinorTicksDisplayed(false);
    countAxis()->setRequireTickLabel   (false);
  }

  //---

  int gpos = 0;
  int vpos = 0;

  int ig = 0;
  int ng = groupValues_.size();

  int offset = 0;
  int count  = 0;

  double doffset = 0.0;

  using Totals = std::map<int,int>;

  Totals groupTotals;

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
    for (auto &groupValues : groupValues_) {
      int           groupInd = groupValues.first;
      const Values *values   = groupValues.second;

      for (const auto &bucketValues : values->bucketValues) {
        int                    bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        int n;

        if (isRangeBar())
          n = varsData.max;
        else
          n = varsData.inds.size();

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

  for (auto &groupValues : groupValues_) {
    if (ng > 1 && isSetHidden(ig)) { ++ig; continue; }

    //---

    int vpos1 = 0;

    //---

    int           groupInd = groupValues.first;
    const Values *values   = groupValues.second;

    //---

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

        addPlotObject(barObj);
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

        VariantIndsData *pVarsData = const_cast<VariantIndsData *>(&varsData);

        int sbucket = bucket;

        if (isSorted()) {
          const Buckets &sortedBuckets = getSortedBuckets(groupInd);

          sbucket = sortedBuckets[iv];

          auto p = values->bucketValues.find(sbucket);
          assert(p != values->bucketValues.end());

          const VariantIndsData &varsData1 = (*p).second;

          pVarsData = const_cast<VariantIndsData *>(&varsData1);
        }

        //---

        int n = pVarsData->inds.size();

        CQChartsGeom::BBox bbox = CQChartsGeom::BBox(ig - 0.5, iv - 0.5, ig + 0.5, iv + 0.5);

        CQChartsDistributionScatterObj *scatterObj =
          new CQChartsDistributionScatterObj(this, bbox, groupInd, sbucket, n, ig, ng, iv, nv);

        addPlotObject(scatterObj);

        QString bucketName = bucketStr(groupInd, bucket);

        countAxis()->setTickLabel(iv, bucketName);

        ++iv;
      }

      QString groupName = groupIndName(groupInd);

      valueAxis()->setTickLabel(ig, groupName);
    }
    else {
      auto pb = groupBucketRange_.find(groupInd);
      assert(pb != groupBucketRange_.end());

      const CQChartsGeom::IMinMax &bucketRange = (*pb).second;

      if (! isOverlay() && ! isSkipEmpty())
        offset = -bucketRange.min(0);

      //---

      bool isNumeric = values->valueSet->isNumeric();

      int iv = 0;
      int nv = values->bucketValues.size();

      for (auto &bucketValues : values->bucketValues) {
        if (ng == 1 && isSetHidden(iv)) { ++iv; continue; }

        //---

        int                    bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        //---

        VariantIndsData *pVarsData = const_cast<VariantIndsData *>(&varsData);

        int sbucket = bucket;

        if (isSorted()) {
          const Buckets &sortedBuckets = getSortedBuckets(groupInd);

          sbucket = sortedBuckets[iv];

          auto p = values->bucketValues.find(sbucket);
          assert(p != values->bucketValues.end());

          const VariantIndsData &varsData1 = (*p).second;

          pVarsData = const_cast<VariantIndsData *>(&varsData1);
        }

        //---

        int bucket1 = bucket + offset + count;

        double n1, n2;

        if (isRangeBar()) {
          if (! isNumeric)
            continue;

          n1 = pVarsData->min;
          n2 = pVarsData->max;
        }
        else {
          n1 = 0;
          n2 = pVarsData->inds.size();
        }

        //---

        CQChartsGeom::BBox bbox;

        if      (isOverlay()) {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(bucket - 0.5, n1, bucket + 0.5, n2);
          else
            bbox = CQChartsGeom::BBox(n1, bucket - 0.5, n2, bucket + 0.5);
        }
        else if (! isSkipEmpty()) {
          if      (isStacked()) {
            int total = groupTotals[iv];

            if (! isHorizontal())
              bbox = CQChartsGeom::BBox(vpos1 - 0.5, n1 + total, vpos1 + 0.5, n2 + total);
            else
              bbox = CQChartsGeom::BBox(n1 + total, vpos1 - 0.5, n2 + total, vpos1 + 0.5);
          }
          else if (isSideBySide()) {
            double tpos1 = vpos1 - 0.5 + barWidth*gpos;
            double tpos2 = tpos1 + barWidth;

            if (! isHorizontal())
              bbox = CQChartsGeom::BBox(tpos1, n1, tpos2, n2);
            else
              bbox = CQChartsGeom::BBox(n1, tpos1, n2, tpos2);
          }
          else {
            if (! isHorizontal())
              bbox = CQChartsGeom::BBox(bucket1 - 0.5, n1, bucket1 + 0.5, n2);
            else
              bbox = CQChartsGeom::BBox(n1, bucket1 - 0.5, n2, bucket1 + 0.5);
          }
        }
        else {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(vpos - 0.5, n1, vpos + 0.5, n2);
          else
            bbox = CQChartsGeom::BBox(n1, vpos - 0.5, n2, vpos + 0.5);
        }

        CQChartsDistributionBarObj *barObj =
          new CQChartsDistributionBarObj(this, bbox, groupInd, sbucket, n1, n2, ig, ng, iv, nv);

        addPlotObject(barObj);

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

        groupTotals[iv] += n2;

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

  // value axis label (x)
  if (isBucketed()) {
    QStringList groupLabels;

    for (const auto &groupValues : groupValues_) {
      int groupInd = groupValues.first;

      QString groupName = groupIndName(groupInd);

      groupLabels.push_back(groupName);
    }

    valueAxis()->setLabel(groupLabels.join(", "));
  }
  else
    valueAxis()->setLabel("");

  if (xLabel().length())
    valueAxis()->setLabel(xLabel());

  // count axis label (y)
  if      (isDensity()) {
    countAxis()->setLabel("Density");
  }
  else if (isScatter()) {
  }
  else {
    if (isRangeBar())
      countAxis()->setLabel("Range");
    else
      countAxis()->setLabel("Count");
  }

  if (yLabel().length())
    countAxis()->setLabel(yLabel());

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsDistributionPlot::
getInds(int groupInd, int bucket, VariantInds &inds) const
{
  const Values *values = getValues(groupInd);
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

  const Values *values = getValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == CQBaseModel::Type::INTEGER) {
    const CQChartsIValues &ivals = values->valueSet->ivals();

    mean = ivals.mean();

    std::vector<double> xvals;

    for (int i = 0; i < ivals.size(); ++i)
      vals.push_back(*ivals.value(i));
  }
  else if (values->valueSet->type() == CQBaseModel::Type::REAL) {
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

  const Values *values = getValues(groupInd);
  if (! values) return false;

  if      (values->valueSet->type() == CQBaseModel::Type::INTEGER) {
    const CQChartsIValues &ivals = values->valueSet->ivals();

    mean = ivals.mean();
  }
  else if (values->valueSet->type() == CQBaseModel::Type::REAL) {
    const CQChartsRValues &rvals = values->valueSet->rvals();

    mean = rvals.mean();
  }
  else {
    return false;
  }

  return true;
}

const CQChartsDistributionPlot::Values *
CQChartsDistributionPlot::
getValues(int groupInd) const
{
  auto pg = groupValues_.find(groupInd);
  if (pg == groupValues_.end()) return nullptr;

  return (*pg).second;
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
    CQChartsKeyText         *keyText  = new CQChartsKeyText        (this, name);

    key->addItem(keyColor, row, 0);
    key->addItem(keyText , row, 1);

    ++row;
  };

  //---

  int ng = groupValues_.size();

  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupValues : groupValues_) {
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

      CQChartsColorSet *colorSet = getColorSet("color");

      int n = colorSet->numValues();

      using ValueSet = std::set<QString>;

      ValueSet valueSet;

      for (int i = 0; i < n; ++i)
        valueSet.insert(colorSet->value(i).toString());

      int nv = valueSet.size();
      int iv = 0;

      for (const auto &value : valueSet) {
        addKeyRow(iv, nv, value);

        ++iv;
      }
    }
    else {
      auto pg = groupValues_.begin();

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
  for (const auto &groupValues : groupValues_) {
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

  CQBucketer &bucketer = groupBucketer(groupInd);

  bool isNumeric = (values ? values->valueSet->isNumeric() : false);

  if (isNumeric) {
    double value1, value2;

    bucketValues(groupInd, bucket, value1, value2);

    if      (type == BucketValueType::ALL)
      return bucketer.bucketName(value1, value2, /*arrow*/true);
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

  CQBucketer &bucketer = groupBucketer(groupInd);

  bool isAuto = (! filterStack_.empty() || isAutoBucket());

  if (isAuto)
    bucketer.autoBucketValues(bucket, value1, value2);
  else
    bucketer.bucketRValues(bucket, value1, value2);

  if (CMathUtil::isZero(value1)) value1 = 0.0;
  if (CMathUtil::isZero(value2)) value2 = 0.0;
}

CQBucketer &
CQChartsDistributionPlot::
groupBucketer(int groupInd) const
{
  // use consistent bucketer when stacked/side by side
  if (isStacked() || isSideBySide() || isScatter())
    groupInd = 0;

  CQChartsDistributionPlot *th = const_cast<CQChartsDistributionPlot *>(this);

  auto p = th->groupBucketer_.find(groupInd);

  if (p == th->groupBucketer_.end()) {
    CQBucketer bucketer;

    bucketer.setType(bucketer_.type());

    bucketer.setRStart(bucketer_.rstart());
    bucketer.setRDelta(bucketer_.rdelta());

    bucketer.setNumAuto(bucketer_.numAuto());

    p = th->groupBucketer_.insert(p, GroupBucketer::value_type(groupInd, bucketer));
  }

  return (*p).second;
}

//------

bool
CQChartsDistributionPlot::
addMenuItems(QMenu *menu)
{
  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
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
  (void) addCheckedAction("Stacked"   , isStacked   (), SLOT(setStacked   (bool)));
  (void) addCheckedAction("SideBySide", isSideBySide(), SLOT(setSideBySide(bool)));
  (void) addCheckedAction("Overlay"   , isOverlay   (), SLOT(setOverlay   (bool)));
  (void) addCheckedAction("Scatter"   , isScatter   (), SLOT(setScatter   (bool)));
  (void) addCheckedAction("Skip Empty", isSkipEmpty (), SLOT(setSkipEmpty (bool)));
  (void) addCheckedAction("Range Bar" , isRangeBar  (), SLOT(setRangeBar  (bool)));
  (void) addCheckedAction("Sorted"    , isSorted    (), SLOT(setSorted    (bool)));
  (void) addCheckedAction("Density"   , isDensity   (), SLOT(setDensity   (bool)));
  (void) addCheckedAction("Dot Lines" , isDotLines  (), SLOT(setDotLines  (bool)));
  (void) addCheckedAction("Rug"       , isRug       (), SLOT(setRug       (bool)));
  (void) addCheckedAction("Mean Line" , isShowMean  (), SLOT(setShowMean  (bool)));

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
drawForeground(QPainter *painter)
{
  if (! isDensity() && ! isScatter()) {
    if (isShowMean())
      drawMeanLine(painter);
  }
}

void
CQChartsDistributionPlot::
drawMeanLine(QPainter *painter)
{
  // set pen
  QColor bc = interpMeanLinesColor(0, 1);

  QPen pen;

  setPen(pen, true, bc, meanLinesAlpha(), meanLinesWidth(), meanLinesDash());

  painter->setPen(pen);

  //---

  const CQChartsGeom::Range &dataRange = this->dataRange();

  int ig = 0;
  int ng = groupValues_.size();

  for (const auto &groupValues : groupValues_) {
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

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                           int groupInd, int bucket, double n1, double n2,
                           int is, int ns, int iv, int nv) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), bucket_(bucket),
 n1_(n1), n2_(n2), is_(is), ns_(ns), iv_(iv), nv_(nv)
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

  if (plot_->isRangeBar()) {
    tableTip.addTableRow("Min", minValue());
    tableTip.addTableRow("Max", maxValue());
  }
  else
    tableTip.addTableRow("Count", count());

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
  return n2_;
}

int
CQChartsDistributionBarObj::
minValue() const
{
  return n1_;
}

int
CQChartsDistributionBarObj::
maxValue() const
{
  return n2_;
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
dataLabelRect() const
{
  if (! plot_->dataLabel().isVisible())
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox rect = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(rect);

  QString ystr;

  if (plot_->isRangeBar())
    ystr = QString("%1-%2").arg(minValue()).arg(maxValue());
  else
    ystr = QString("%1").arg(count());

  return plot_->dataLabel().calcRect(qrect, ystr);
}

void
CQChartsDistributionBarObj::
getSelectIndices(Indices &inds) const
{
  CQChartsDistributionPlot::VariantInds vinds;

  plot_->getInds(groupInd_, bucket_, vinds);

  for (auto &vind : vinds) {
    const CQChartsModelIndex &ind = vind.ind;

    addSelectIndex(inds, ind.row, ind.column, ind.parent);
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
      const CQChartsModelIndex &ind = vind.ind;

      addSelectIndex(inds, ind.row, column, ind.parent);
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
}

void
CQChartsDistributionBarObj::
drawFg(QPainter *painter)
{
  CQChartsGeom::BBox pbbox = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(pbbox);

  //---

  QString ystr;

  if (plot_->isRangeBar())
    ystr = QString("%1-%2").arg(minValue()).arg(maxValue());
  else
    ystr = QString("%1").arg(count());

  plot_->dataLabel().draw(painter, qrect, ystr);

  //---

  if (plot_->isRug())
    drawRug(painter);
}

void
CQChartsDistributionBarObj::
drawRug(QPainter *painter)
{
  CQChartsSymbol symbol = plot_->rugSymbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = (! plot_->isHorizontal() ? CQChartsSymbol::Type::VLINE :
                                        CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);

  int ic = (ns_ > 1 ? is_ : iv_);
  int nc = (ns_ > 1 ? ns_ : nv_);

  QColor strokeColor = plot_->interpRugSymbolStrokeColor(ic, nc);
  QColor barColor    = plot_->interpRugSymbolFillColor  (ic, nc);

  QPen   pen;
  QBrush brush;

  plot_->setPenBrush(pen, brush,
                     plot_->isRugSymbolStroked(), strokeColor, plot_->rugSymbolStrokeAlpha(),
                     plot_->rugSymbolStrokeWidth(), plot_->rugSymbolStrokeDash(),
                     plot_->isRugSymbolFilled(), barColor,
                     plot_->rugSymbolFillAlpha(), plot_->rugSymbolFillPattern());

  painter->setPen  (pen);
  painter->setBrush(brush);

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

    if (! plot_->colorSetColor("color", ind.row, color))
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
  QColor fc = color.interpColor(plot_, 0, 1);

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

  //---

  if (! plot_->isDotLines()) {
    // draw rect
    painter->setPen(pen);
    painter->setBrush(barBrush);

    if (! useLine) {
      double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
      double cys = plot_->lengthPixelHeight(plot_->cornerSize());

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

    painter->setPen(pen);
    painter->setBrush(barBrush);

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

    // draw dot
    CQChartsSymbol symbol = plot_->dotSymbolType();

    double sx, sy;

    plot_->pixelSymbolSize(plot_->dotSymbolSize(), sx, sy);

    int ic = (ns_ > 1 ? is_ : iv_);
    int nc = (ns_ > 1 ? ns_ : nv_);

    //---

    QPen   dotPen;
    QBrush dotBrush;

    QColor bc = plot_->interpDotSymbolStrokeColor(ic, nc);
    QColor fc = plot_->interpDotSymbolFillColor(ic, nc);

    plot_->setPenBrush(dotPen, dotBrush,
      plot_->isDotSymbolStroked(), bc, plot_->dotSymbolStrokeAlpha(),
      plot_->dotSymbolStrokeWidth(), plot_->dotSymbolStrokeDash(),
      plot_->isDotSymbolFilled(), fc, plot_->dotSymbolFillAlpha(), plot_->dotSymbolFillPattern());

    painter->setPen  (dotPen);
    painter->setBrush(dotBrush);

    //---

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
CQChartsDistributionDensityObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                               int groupInd, const Data &data, double doffset, int is, int ns) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), data_(data),
 doffset_(doffset), is_(is), ns_(ns)
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
  QColor fc = plot_->interpBarFillColor(is_, ns_);

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

  // draw buckets as bars
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
drawFg(QPainter *painter)
{
  if (plot_->isShowMean())
    drawMeanLine(painter);

  if (plot_->isRug())
    drawRug(painter);
}

void
CQChartsDistributionDensityObj::
drawMeanLine(QPainter *painter)
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
drawRug(QPainter *painter)
{
  CQChartsSymbol symbol = plot_->rugSymbolType();

  if (symbol == CQChartsSymbol::Type::NONE)
    symbol = (! plot_->isHorizontal() ? CQChartsSymbol::Type::VLINE :
                                        CQChartsSymbol::Type::HLINE);

  double sx, sy;

  plot_->pixelSymbolSize(plot_->rugSymbolSize(), sx, sy);

  //---

  QColor fillColor = plot_->interpBarFillColor(is_, ns_);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, fillColor, 0.5, CQChartsLength("0px"), CQChartsLineDash());
  plot_->setBrush(brush, true, fillColor, 0.5, CQChartsFillPattern());

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
CQChartsDistributionScatterObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                               int groupInd, int bucket, int n, int is, int ns, int iv, int nv) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), bucket_(bucket), n_(n),
 is_(is), ns_(ns), iv_(iv), nv_(nv)
{
  assert(is >= 0 && is < ns);
  assert(iv >= 0 && iv < nv);

  int nf = CMathUtil::clamp(int(n_*plot_->scatterFactor()), 1, n_);

  points_.resize(nf);

  double m = 0.05;

  CQChartsRand::RealInRange rand(m, 1.0 - m);

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
  int ic = (ns_ > 1 ? is_ : iv_);
  int nc = (ns_ > 1 ? ns_ : nv_);

  QColor c = plot_->interpBarFillColor(ic, nc);

  QPen   pen;
  QBrush brush;

  plot_->setPen  (pen  , true, Qt::black, 1.0, CQChartsLength("0px"), CQChartsLineDash());
  plot_->setBrush(brush, true, c, 1.0, CQChartsFillPattern());

  painter->setPen(pen);
  painter->setBrush(brush);

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
selectPress(const CQChartsGeom::Point &)
{
  setSetHidden(! isSetHidden());

  plot_->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsDistKeyColorBox::
fillBrush() const
{
  QColor c = plot_->interpBarFillColor(i_, n_);

  if (isSetHidden())
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

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
