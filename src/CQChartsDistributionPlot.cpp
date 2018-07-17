#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>
#include <CQChartsDensity.h>

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
  addBoolParameter("overlay"   , "Overlay"   , "overlay"   ).setTip("overlay groups");
  addBoolParameter("skipEmpty" , "SkipEmpty" , "skipEmpty" ).setTip("skip empty buckets");
  addBoolParameter("rangeBar"  , "RangeBar"  , "rangeBar"  ).setTip("show value range");
  addBoolParameter("density"   , "Density"   , "density"   ).setTip("show density");

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
 CQChartsBarPlot(view, view->charts()->plotType("distribution"), model), dataLabel_(this)
{
  setAutoBucket    (true);
  setNumAutoBuckets(20);
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

void
CQChartsDistributionPlot::
addProperties()
{
  // TODO: placement: overlay, stacked, side-side, grouped

  CQChartsPlot::addProperties();

  CQChartsBarPlot::addProperties();

  addProperty("columns", this, "nameColumn", "name");

  addProperty("bucket", this, "bucketed"        , "enabled");
  addProperty("bucket", this, "autoBucket"      , "auto"   );
  addProperty("bucket", this, "startBucketValue", "start"  );
  addProperty("bucket", this, "deltaBucketValue", "delta"  );
  addProperty("bucket", this, "numAutoBuckets"  , "num"    );

  addProperty("options", this, "overlay"  );
  addProperty("options", this, "skipEmpty");
  addProperty("options", this, "rangeBar" );
  addProperty("options", this, "density"  );
  addProperty("options", this, "showMean" );

  CQChartsGroupPlot::addProperties();

  dataLabel_.addProperties("dataLabel");
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
setOverlay(bool b)
{
  CQChartsUtil::testAndSet(overlay_, b, [&]() { updateRangeAndObjs(); } );
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
setDensity(bool b)
{
  CQChartsUtil::testAndSet(density_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsDistributionPlot::
updateRange(bool apply)
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

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      plot_->addRow(ind, row);

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
  //int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    CQChartsValueSet::Type type = values->valueSet->type();

    if      (type == CQChartsValueSet::Type::INTEGER) {
      bucketer_.setIntegral(true);
      bucketer_.setIMin    (values->valueSet->imin());
      bucketer_.setIMax    (values->valueSet->imax());
    }
    else if (type == CQChartsValueSet::Type::REAL) {
      bucketer_.setIntegral(false);
      bucketer_.setRMin    (values->valueSet->rmin());
      bucketer_.setRMax    (values->valueSet->rmax());
    }

    for (auto &ind : values->inds) {
      int      bucket = -1;
      QVariant value;

      bool ok;

      if (isBucketed()) {
        if      (type == CQChartsValueSet::Type::REAL) {
          double r = modelReal(ind.row, ind.column, ind.parent, ok);
          if (! ok || CQChartsUtil::isNaN(r)) continue;

          bucket = calcBucket(r);
          value  = QVariant(r);
        }
        else if (type == CQChartsValueSet::Type::INTEGER) {
          int i = modelInteger(ind.row, ind.column, ind.parent, ok);
          if (! ok) continue;

          bucket = calcBucket(i);
          value  = QVariant(i);
        }
        else {
          QString str = modelString(ind.row, ind.column, ind.parent, ok);
          if (! ok) continue;

          bucket = bucketer_.stringBucket(str);
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
  // TODO: overlay/side-side

  int i1 = 0;
  int i2 = 0;

  CQChartsGeom::IMinMax nRange(i1);
  CQChartsGeom::IMinMax bucketRange;
  CQChartsGeom::RMinMax valueRange;
  CQChartsGeom::BBox    densityBBox;

  groupBucketRange_.clear();

  for (auto &groupValues : groupValues_) {
    int     groupInd = groupValues.first;
    Values *values   = groupValues.second;

    if (! isDensity()) {
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
      }
    }
    else {
      const CQChartsRValues &rvals = values->valueSet->rvals();

      values->densityData.setNumSamples(100);

      std::vector<double> xvals;

      for (int i = 0; i < rvals.size(); ++i)
        xvals.push_back(*rvals.value(i));

      values->densityData.setXVals(xvals);

      values->densityData.calc();

      const CQChartsDensity::Points &opoints = values->densityData.opoints();

      for (const auto &p : opoints)
        densityBBox.add(p.x(), p.y());

      values->mean = rvals.mean();
    }
  }

  //---

  // set range
  if (! isDensity()) {
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
    else {
      int i1 = 0;

      if (! isHorizontal()) {
        dataRange_.updateRange(i1 - 1.0, 0);
        dataRange_.updateRange(i2      , n1);
      }
      else {
        dataRange_.updateRange(0 , i1 - 1.0);
        dataRange_.updateRange(n1, i2      );
      }
    }
  }
  else {
    dataRange_.updateRange(densityBBox.getXMin(), densityBBox.getYMin());
    dataRange_.updateRange(densityBBox.getXMax(), densityBBox.getYMax());
  }

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsDistributionPlot::
clearGroupValues()
{
  for (auto &groupValues : groupValues_)
    delete groupValues.second;

  groupValues_.clear();
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

  if (! checkFilter(groupInd, value))
    return;

  //---

  auto pg = groupValues_.find(groupInd);

  if (pg == groupValues_.end()) {
    ValueSet *valueSet = new ValueSet(this);

    valueSet->setColumn(ind.column);

    pg = groupValues_.insert(pg, GroupValues::value_type(groupInd, new Values(valueSet)));
  }

  Values *values = (*pg).second;

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

  CQChartsDataLabel::Position position = dataLabel().position();

  if (position != CQChartsDataLabel::TOP_OUTSIDE && position != CQChartsDataLabel::BOTTOM_OUTSIDE)
    return bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsDistributionBarObj *barObj = dynamic_cast<CQChartsDistributionBarObj *>(plotObj);

    if (barObj)
      bbox += barObj->dataLabelRect();
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
calcBucket(double value) const
{
  if (filterStack_.empty()) {
    if (! isBucketed())
      return -1;

    if (isAutoBucket())
      return bucketer_.autoRealBucket(value);
    else
      return bucketer_.realBucket(value);
  }
  else {
    return bucketer_.autoRealBucket(value);
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

  //---

  int pos = 0;

  int is = 0;
  int ns = groupValues_.size();

  int offset = 0;
  int count  = 0;

  for (const auto &groupValues : groupValues_) {
    if (ns > 1 && isSetHidden(is)) { ++is; continue; }

    //---

    int           groupInd = groupValues.first;
    const Values *values   = groupValues.second;

    //---

    if (! isDensity()) {
      auto pb = groupBucketRange_.find(groupInd);
      assert(pb != groupBucketRange_.end());

      CQChartsGeom::IMinMax &bucketRange = (*pb).second;

      if (! isOverlay() && ! isSkipEmpty())
        offset = -bucketRange.min(0);

      //---

      bool isNumeric = values->valueSet->isNumeric();

      int iv = 0;
      int nv = values->bucketValues.size();

      for (const auto &bucketValues : values->bucketValues) {
        if (ns == 1 && isSetHidden(iv)) { ++iv; continue; }

        //---

        int                    bucket   = bucketValues.first;
        const VariantIndsData &varsData = bucketValues.second;

        int bucket1 = bucket + offset + count;

        double n1, n2;

        if (isRangeBar()) {
          n1 = varsData.min;
          n2 = varsData.max;
        }
        else {
          n1 = 0;
          n2 = varsData.inds.size();
        }

        CQChartsGeom::BBox bbox;

        if      (isOverlay()) {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(bucket - 0.5, n1, bucket + 0.5, n2);
          else
            bbox = CQChartsGeom::BBox(n1, bucket - 0.5, n2, bucket + 0.5);
        }
        else if (! isSkipEmpty()) {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(bucket1 - 0.5, n1, bucket1 + 0.5, n2);
          else
            bbox = CQChartsGeom::BBox(n1, bucket1 - 0.5, n2, bucket1 + 0.5);
        }
        else {
          if (! isHorizontal())
            bbox = CQChartsGeom::BBox(pos - 0.5, n1, pos + 0.5, n2);
          else
            bbox = CQChartsGeom::BBox(n1, pos - 0.5, n2, pos + 0.5);
        }

        CQChartsDistributionBarObj *barObj =
          new CQChartsDistributionBarObj(this, bbox, groupInd, bucket, n1, n2, is, ns, iv, nv);

        addPlotObject(barObj);

        //---

        if      (! isBucketed()) {
          QString groupName = groupIndName(groupInd);

          valueAxis()->setTickLabel(bucket1, groupName);
        }
        else if (isNumeric) {
          if (valueAxis()->tickLabelPlacement() == CQChartsAxis::TickLabelPlacement::MIDDLE) {
            QString bucketStr = bucketValuesStr(bucket, values, BucketValueType::ALL);

            if      (isOverlay())
              valueAxis()->setTickLabel(bucket, bucketStr);
            else if (! isSkipEmpty())
              valueAxis()->setTickLabel(bucket1, bucketStr);
            else
              valueAxis()->setTickLabel(pos, bucketStr);
          }
          else {
            QString bucketStr1 = bucketValuesStr(bucket, values, BucketValueType::START);
            QString bucketStr2 = bucketValuesStr(bucket, values, BucketValueType::END  );

            if      (isOverlay()) {
              valueAxis()->setTickLabel(bucket    , bucketStr1);
              valueAxis()->setTickLabel(bucket + 1, bucketStr2);
            }
            else if (! isSkipEmpty()) {
              valueAxis()->setTickLabel(bucket1    , bucketStr1);
              valueAxis()->setTickLabel(bucket1 + 1, bucketStr2);
            }
            else {
              valueAxis()->setTickLabel(pos    , bucketStr1);
              valueAxis()->setTickLabel(pos + 1, bucketStr2);
            }
          }
        }
        else {
          QString bucketStr = bucketValuesStr(bucket, values, BucketValueType::START);

          if      (isOverlay())
            valueAxis()->setTickLabel(bucket, bucketStr);
          else if (! isSkipEmpty())
            valueAxis()->setTickLabel(bucket1, bucketStr);
          else
            valueAxis()->setTickLabel(pos, bucketStr);
        }

        //---

        ++iv;

        ++pos;
      }

      //---

      if (! isOverlay() && ! isSkipEmpty())
        count += bucketRange.max(0) - bucketRange.min(0) + 1;
    }
    else {
      const CQChartsDensity::Points &opoints = values->densityData.opoints();

      CQChartsGeom::BBox bbox;

      for (const auto &p : opoints)
        bbox.add(p.x(), p.y());

      CQChartsDistributionDensityObj *barObj =
        new CQChartsDistributionDensityObj(this, bbox, groupInd, opoints, values->mean, is, ns);

      addPlotObject(barObj);
    }

    ++is;
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
  if      (isRangeBar())
    countAxis()->setLabel("Range");
  else if (isDensity())
    countAxis()->setLabel("Density");
  else
    countAxis()->setLabel("Count");

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
  auto pg = groupValues_.find(groupInd);
  if (pg == groupValues_.end()) return;

  const Values *values = (*pg).second;

  auto pb = values->bucketValues.find(bucket);
  if (pb == values->bucketValues.end()) return;

  inds = (*pb).second.inds;
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

  int ns = groupValues_.size();

  if      (ns > 1) {
    int is = 0;

    for (const auto &groupValues : groupValues_) {
      int           groupInd = groupValues.first;
    //const Values *values   = groupValues.second;

      QString groupName = groupIndName(groupInd);

      addKeyRow(is, ns, groupName);

      ++is;
    }
  }
  else if (ns == 1) {
    auto pg = groupValues_.begin();

  //int           groupInd = (*pg).first;
    const Values *values   = (*pg).second;

    int nv = values->bucketValues.size();

    int iv = 0;

    for (const auto &bucketValues : values->bucketValues) {
      int                    bucket   = bucketValues.first;
    //const VariantIndsData &varsData = bucketValues.second;

      QString bucketName = bucketValuesStr(bucket, values, BucketValueType::ALL);

      addKeyRow(iv, nv, bucketName);

      ++iv;
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
      return bucketValuesStr(bucket, groupValues.second, type);
  }

  return "";
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int bucket, const Values *values, BucketValueType type) const
{
  if (! isBucketed())
    return "";

  bool isNumeric = values->valueSet->isNumeric();

  if (isNumeric) {
    double value1, value2;

    bucketValues(bucket, value1, value2);

    if      (type == BucketValueType::ALL)
      return bucketer_.bucketName(value1, value2, /*arrow*/true);
    else if (type == BucketValueType::START)
      return QString("%1").arg(value1);
    else
      return QString("%1").arg(value2);
  }
  else {
    if (isAutoBucket())
      return values->valueSet->buckets(bucket);
    else
      return values->valueSet->inds(bucket);
  }
}

void
CQChartsDistributionPlot::
bucketValues(int bucket, double &value1, double &value2) const
{
  value1 = 0.0;
  value2 = 0.0;

  if (! isBucketed())
    return;

  bool isAuto = (! filterStack_.empty() || isAutoBucket());

  if (isAuto)
    bucketer_.autoBucketValues(bucket, value1, value2);
  else
    bucketer_.bucketRValues(bucket, value1, value2);

  if (CQChartsUtil::isZero(value1)) value1 = 0.0;
  if (CQChartsUtil::isZero(value2)) value2 = 0.0;
}

//------

bool
CQChartsDistributionPlot::
addMenuItems(QMenu *menu)
{
  QAction *horizontalAction = new QAction("Horizontal", menu);
  QAction *overlayAction    = new QAction("Overlay"   , menu);
  QAction *skipEmptyAction  = new QAction("Skip Empty", menu);
  QAction *rangeBarAction   = new QAction("Range Bar" , menu);
  QAction *densityAction    = new QAction("Density"   , menu);

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  overlayAction->setCheckable(true);
  overlayAction->setChecked(isOverlay());

  skipEmptyAction->setCheckable(true);
  skipEmptyAction->setChecked(isSkipEmpty());

  rangeBarAction->setCheckable(true);
  rangeBarAction->setChecked(isRangeBar());

  densityAction->setCheckable(true);
  densityAction->setChecked(isDensity());

  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));
  connect(overlayAction   , SIGNAL(triggered(bool)), this, SLOT(setOverlay(bool)));
  connect(skipEmptyAction , SIGNAL(triggered(bool)), this, SLOT(setSkipEmpty(bool)));
  connect(rangeBarAction  , SIGNAL(triggered(bool)), this, SLOT(setRangeBar(bool)));
  connect(densityAction   , SIGNAL(triggered(bool)), this, SLOT(setDensity(bool)));

  //---

  PlotObjs objs;

  selectedObjs(objs);

  QAction *pushAction   = new QAction("Push"   , menu);
  QAction *popAction    = new QAction("Pop"    , menu);
  QAction *popTopAction = new QAction("Pop Top", menu);

  connect(pushAction  , SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction   , SIGNAL(triggered()), this, SLOT(popSlot()));
  connect(popTopAction, SIGNAL(triggered()), this, SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(! filterStack_.empty());
  popTopAction->setEnabled(! filterStack_.empty());

  //---

  menu->addSeparator();

  menu->addAction(horizontalAction);
  menu->addAction(overlayAction);
  menu->addAction(skipEmptyAction);
  menu->addAction(rangeBarAction);
  menu->addAction(densityAction);

  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  return true;
}

//---

void
CQChartsDistributionPlot::
pushSlot()
{
  PlotObjs objs;

  selectedObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w;

    pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

    objsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  Filters filters;

  for (const auto &obj : objs) {
    CQChartsDistributionBarObj *distObj = qobject_cast<CQChartsDistributionBarObj *>(obj);

    double value1, value2;

    bucketValues(distObj->bucket(), value1, value2);

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
  QString groupName;

  if (ns_ > 1)
    groupName = plot_->groupIndName(groupInd_);

  QString bucketStr;

  if (! plot_->isBucketed())
    bucketStr = plot_->groupIndName(groupInd_);
  else
    bucketStr =
      plot_->bucketValuesStr(groupInd_, bucket_, CQChartsDistributionPlot::BucketValueType::ALL);

  CQChartsTableTip tableTip;

  if (groupName.length())
    tableTip.addTableRow("Group", groupName);

  tableTip.addTableRow("Bucket", bucketStr);

  if (plot_->isRangeBar()) {
    tableTip.addTableRow("Min", n1_);
    tableTip.addTableRow("Max", n2_);
  }
  else
    tableTip.addTableRow("Count", n2_);

  return tableTip.str();
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
    ystr = QString("%1-%2").arg(n1_).arg(n2_);
  else
    ystr = QString("%1").arg(n2_);

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

  double s = (! plot_->isHorizontal() ? qrect.width() : qrect.height());

  bool useLine = (s <= 2);

  //---

  // get bar colors
  ColorData colorData;

  if (getBarColoredRects(colorData)) {
    double size = (! plot_->isHorizontal() ? qrect.height() : qrect.width());

    double dsize = size/colorData.nv;

    double pos1 = 0.0, pos2 = 0.0;

    for (auto &p : colorData.colorSet) {
      const CQChartsColor &color = p.first;
      int                  n     = colorData.colorCount[p.second];

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
    ystr = QString("%1-%2").arg(n1_).arg(n2_);
  else
    ystr = QString("%1").arg(n2_);

  plot_->dataLabel().draw(painter, qrect, ystr);
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
  // calc pen (stroke)
  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (bc);
    pen.setWidthF(bw);

    CQChartsUtil::penSetLineDash(pen, plot_->borderDash());
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  // calc brush (fill)
  QBrush barBrush;

  if (plot_->isBarFill()) {
    QColor fc = color.interpColor(plot_, 0, 1);

    fc.setAlphaF(plot_->barAlpha());

    barBrush.setColor(fc);
    barBrush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->barPattern()));

    if (useLine) {
      pen.setColor (fc);
      pen.setWidthF(0);
    }
  }
  else {
    barBrush.setStyle(Qt::NoBrush);

    if (useLine)
      pen.setWidthF(0);
  }

  plot_->updateObjPenBrushState(this, pen, barBrush);

  //---

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

QColor
CQChartsDistributionBarObj::
barColor() const
{
  QColor color;

  if (ns_ > 1)
    color = plot_->interpBarColor(is_, ns_);
  else
    color = plot_->interpBarColor(iv_, nv_);

  return color;
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
                               int groupInd, const Points &points, double mean, int is, int ns) :
 CQChartsPlotObj(plot, rect), plot_(plot), groupInd_(groupInd), points_(points),
 mean_(mean), is_(is), ns_(ns)
{
  const CQChartsGeom::Range &dataRange = plot_->dataRange();

  poly_ << QPointF(dataRange.xmin(), dataRange.ymin());

  int np = points_.size();

  for (int i = 0; i < np; ++i)
    poly_ << points_[i];

  poly_ << QPointF(dataRange.xmax(), dataRange.ymin());
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

  QString groupName = plot_->groupIndName(groupInd_);

  tableTip.addTableRow("Name"   , groupName);
  tableTip.addTableRow("Samples", points_.size());

  return tableTip.str();
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
  QBrush brush;

  QColor fillColor = plot_->interpBarColor(is_, ns_);

  fillColor.setAlphaF(plot_->barAlpha());

  brush.setColor(fillColor);

  brush.setStyle(CQChartsFillPattern::toStyle(
   (CQChartsFillPattern::Type) plot_->barPattern()));

  QPen pen(Qt::NoPen);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  QPolygonF poly;

  int np = poly_.length();

  for (int i = 0; i < np; ++i)
    poly << plot()->windowToPixel(poly_[i]);

  //---

  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawPolygon(poly);

  //---

  if (plot_->isShowMean()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    QPen pen(bc);

    painter->setPen(pen);

    const CQChartsGeom::Range &dataRange = plot_->dataRange();

    QPointF p1 = plot()->windowToPixel(QPointF(mean_, dataRange.ymin()));
    QPointF p2 = plot()->windowToPixel(QPointF(mean_, dataRange.ymax()));

    painter->drawLine(p1, p2);
  }
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
  QColor c = plot_->interpBarColor(i_, n_);

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
