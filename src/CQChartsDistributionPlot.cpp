#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>

#include <QMenu>
#include <QAction>
#include <QPainter>

CQChartsDistributionPlotType::
CQChartsDistributionPlotType()
{
  addParameters();
}

void
CQChartsDistributionPlotType::
addParameters()
{
  addColumnParameter("value", "Value", "valueColumn", "", 0);
  addColumnParameter("color", "Color", "colorColumn", "optional");

  addBoolParameter("autoRange", "Auto Range", "autoRange", "optional", true);

  addRealParameter("start", "Start", "startValue", "optional");
  addRealParameter("delta", "Delta", "deltaValue", "optional");
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
 CQChartsPlot(view, view->charts()->plotType("distribution"), model), dataLabel_(this)
{
  borderObj_ = new CQChartsBoxObj(this);
  fillObj_   = new CQChartsFillObj(this);

  fillObj_->setVisible(true);

  //---

  setBorder(true);

  setLayerActive(Layer::FG, true);

  addAxes();

  addKey();

  addTitle();
}

CQChartsDistributionPlot::
~CQChartsDistributionPlot()
{
  delete borderObj_;
  delete fillObj_;
}

void
CQChartsDistributionPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "colorColumn", "color");

  addProperty("value", this, "autoRange" , "autoRange");
  addProperty("value", this, "startValue", "start"    );
  addProperty("value", this, "deltaValue", "delta"    );
  addProperty("value", this, "numAuto"   , "numAuto"  );

  addProperty("", this, "horizontal");
  addProperty("", this, "margin"    );

  QString strokeStr = "stroke";
  QString fillStr   = "fill";

  addProperty(strokeStr, this, "border"          , "visible"   );
  addProperty(strokeStr, this, "borderColor"     , "color"     );
  addProperty(strokeStr, this, "borderWidth"     , "width"     );
  addProperty(strokeStr, this, "borderCornerSize", "cornerSize");

  addProperty(fillStr, this, "barFill"   , "visible");
  addProperty(fillStr, this, "barColor"  , "color"  );
  addProperty(fillStr, this, "barAlpha"  , "alpha"  );
  addProperty(fillStr, this, "barPattern", "pattern");

  dataLabel_.addProperties("dataLabel");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

//---

bool
CQChartsDistributionPlot::
isBorder() const
{
  return borderObj_->isBorder();
}

void
CQChartsDistributionPlot::
setBorder(bool b)
{
  borderObj_->setBorder(b); update();
}

QString
CQChartsDistributionPlot::
borderColorStr() const
{
  return borderObj_->borderColorStr();
}

void
CQChartsDistributionPlot::
setBorderColorStr(const QString &s)
{
  borderObj_->setBorderColorStr(s); update();
}

QColor
CQChartsDistributionPlot::
interpBorderColor(int i, int n) const
{
  return borderObj_->interpBorderColor(i, n);
}

double
CQChartsDistributionPlot::
borderWidth() const
{
  return borderObj_->borderWidth();
}

void
CQChartsDistributionPlot::
setBorderWidth(double r)
{
  borderObj_->setBorderWidth(r); update();
}

double
CQChartsDistributionPlot::
borderCornerSize() const
{
  return borderObj_->borderCornerSize();
}

void
CQChartsDistributionPlot::
setBorderCornerSize(double r)
{
  borderObj_->setBorderCornerSize(r); update();
}

//---

bool
CQChartsDistributionPlot::
isBarFill() const
{
  return fillObj_->isVisible();
}

void
CQChartsDistributionPlot::
setBarFill(bool b)
{
  fillObj_->setVisible(b); update();
}

QString
CQChartsDistributionPlot::
barColorStr() const
{
  return fillObj_->colorStr();
}

void
CQChartsDistributionPlot::
setBarColorStr(const QString &str)
{
  fillObj_->setColorStr(str);

  update();
}

QColor
CQChartsDistributionPlot::
interpBarColor(int i, int n) const
{
  return fillObj_->interpColor(i, n);
}

double
CQChartsDistributionPlot::
barAlpha() const
{
  return fillObj_->alpha();
}

void
CQChartsDistributionPlot::
setBarAlpha(double a)
{
  fillObj_->setAlpha(a); update();
}

CQChartsDistributionPlot::Pattern
CQChartsDistributionPlot::
barPattern() const
{
  return (Pattern) fillObj_->pattern();
}

void
CQChartsDistributionPlot::
setBarPattern(Pattern pattern)
{
  fillObj_->setPattern((CQChartsFillObj::Pattern) pattern); update();
}

//---

int
CQChartsDistributionPlot::
rowCount(QAbstractItemModel *model) const
{
  return (model ? model->rowCount(QModelIndex()) : 0);
}

QVariant
CQChartsDistributionPlot::
rowValue(QAbstractItemModel *model, int r, int c, bool &ok) const
{
  ok = false;

  return (model ? CQChartsUtil::modelValue(model, r, c, ok) : QVariant());
}

QVariant
CQChartsDistributionPlot::
headerValue(QAbstractItemModel *model, int c, bool &ok) const
{
  ok = false;

  return (model ? CQChartsUtil::modelHeaderString(model, c, ok) : QVariant());
}

//---

void
CQChartsDistributionPlot::
initColorSet()
{
  QAbstractItemModel *model = this->model();

  //---

  colorSet_.clear();

  if (colorColumn() < 0)
    return;

  int nr = rowCount(model);

  for (int r = 0; r < nr; ++r) {
    bool ok;

    QVariant value = rowValue(model, r, colorColumn(), ok);

    colorSet_.addValue(value); // always add some value
  }
}

bool
CQChartsDistributionPlot::
colorSetColor(int i, OptColor &color)
{
  if (colorSet_.empty())
    return false;

  // color can be actual color value (string) or value used to map into palette
  // (map enabled or disabled)
  if (colorSet_.type() == CQChartsValueSet::Type::STRING) {
    QVariant colorVar = colorSet_.value(i);

    QColor c(colorVar.toString());

    if (c.isValid()) {
      color = c;

      return true;
    }
  }

  double value = colorSet_.imap(i);

  color = CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE, value);

  return true;
}

//---

void
CQChartsDistributionPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  //---

  int nr = rowCount(model);

  dataRange_.reset();

  //---

  // calc category type
  valueSet_.clear();

  for (int r = 0; r < nr; ++r) {
    bool ok;

    QVariant value = rowValue(model, r, valueColumn(), ok);

    if (! ok)
      continue;

    valueSet_.addValue(value);
  }

  //---

  // calc value range
  bool hasRange = valueSet_.isNumeric();

  categoryRange_.numValues = 0;
  categoryRange_.minValue  = 0.0;
  categoryRange_.maxValue  = 0.0;

  if (hasRange) {
    for (int r = 0; r < nr; ++r) {
      bool ok;

      double value = rowValue(model, r, valueColumn(), ok).toReal();

      if (! ok)
        continue;

      if (CQChartsUtil::isNaN(value))
        continue;

      if (! checkFilter(value))
        continue;

      if (categoryRange_.numValues == 0) {
        categoryRange_.minValue = value;
        categoryRange_.maxValue = value;
      }
      else {
        categoryRange_.minValue = std::min(categoryRange_.minValue, value);
        categoryRange_.maxValue = std::max(categoryRange_.maxValue, value);
      }

      ++categoryRange_.numValues;
    }

    calcCategoryRange();
  }

  //---

  int minBucket = 1;
  int maxBucket = -1;

  ivalues_.clear();

  for (int r = 0; r < nr; ++r) {
    QModelIndex valueInd, valueInd1;

    if (model) {
      valueInd  = model->index(r, valueColumn());
      valueInd1 = normalizeIndex(valueInd);
    }

    //---

    int bucket = -1;

    if (hasRange) {
      bool ok;

      double value = rowValue(model, r, valueColumn(), ok).toReal();

      if (! ok)
        continue;

      if (CQChartsUtil::isNaN(value))
        continue;

      if (! checkFilter(value))
        continue;

      //----

      bucket = calcBucket(value);

      ivalues_[bucket].emplace_back(valueInd1);
    }
    else {
      bool ok;

      QString value = rowValue(model, r, valueColumn(), ok).toString();

      if (! ok)
        continue;

      bucket = valueSet_.sind(value);

      ivalues_[bucket].emplace_back(valueInd1);
    }

    //---

    if (bucket >= 0) {
      if (minBucket <= maxBucket) {
        minBucket = std::min(minBucket, bucket);
        maxBucket = std::max(maxBucket, bucket);
      }
      else {
        minBucket = bucket;
        maxBucket = bucket;
      }
    }
  }

  //---

  int maxValues = 0;

  for (const auto &ivalue : ivalues_)
    maxValues = std::max(maxValues, int(ivalue.second.size()));

  if (maxValues == 0)
    maxValues = 1;

  if (maxBucket < minBucket) // unset
    maxBucket = minBucket;

  //---

  if (! isHorizontal()) {
    dataRange_.updateRange(minBucket - 1.0, 0        );
    dataRange_.updateRange(maxBucket + 1.0, maxValues);
  }
  else {
    dataRange_.updateRange(0        , minBucket - 1.0);
    dataRange_.updateRange(maxValues, maxBucket + 1.0);
  }

  //---

  if (apply)
    applyDataRange();
}

//------

bool
CQChartsDistributionPlot::
checkFilter(double value) const
{
  if (filters_.empty())
    return true;

  const Filter &filter = filters_.back();

  return (value >= filter.minValue && value < filter.maxValue);
}

void
CQChartsDistributionPlot::
calcCategoryRange()
{
  double length = categoryRange_.maxValue - categoryRange_.minValue;

  double length1 = length/categoryRange_.numAuto;

  // Calculate nearest Power of Ten to Length
  int power = (length1 > 0 ? CQChartsUtil::Round(log10(length1)) : 1);

  categoryRange_.increment = 0.1;

  if      (power < 0) {
    for (int i = 0; i < -power; i++)
      categoryRange_.increment /= 10.0;
  }
  else if (power > 0) {
    for (int i = 0; i <  power; i++)
      categoryRange_.increment *= 10.0;
  }

  // round min value to increment
  if (length1 > 0) {
    categoryRange_.increment =
      categoryRange_.increment*CQChartsUtil::Round(length1/categoryRange_.increment);

    categoryRange_.calcMinValue = categoryRange_.increment*
      CQChartsUtil::RoundDown(categoryRange_.minValue/categoryRange_.increment);
  }
  else {
    categoryRange_.calcMinValue = categoryRange_.minValue;
  }
}

int
CQChartsDistributionPlot::
calcBucket(double value) const
{
  int bucket = 0;

  bool isAuto = (! filters_.empty() || categoryRange_.type == CategoryRange::Type::AUTO);

  if (isAuto) {
    if (categoryRange_.increment > 0.0)
      bucket = std::floor((value - categoryRange_.calcMinValue)/categoryRange_.increment);
  }
  else {
    if (deltaValue() > 0.0)
      bucket = std::floor((value - calcStartValue())/deltaValue());
  }

  return bucket;
}

double
CQChartsDistributionPlot::
calcStartValue() const
{
  double startVal = std::min(categoryRange_.minValue, startValue());

  if (deltaValue() > 0.0)
    startVal = deltaValue()*CQChartsUtil::RoundDown(startVal/deltaValue());

  return startVal;
}

//------

void
CQChartsDistributionPlot::
updateObjs()
{
  //valueSet_.clear();

  colorSet_.clear();

  CQChartsPlot::updateObjs();
}

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
  if (colorSet_.empty())
    initColorSet();

  //---

  xAxis_->clearTickLabels();
  yAxis_->clearTickLabels();

  xAxis()->setIntegral(true);
  yAxis()->setIntegral(true);

  valueAxis()->setMajorIncrement(1);
  countAxis()->setMajorIncrement(0);

  valueAxis()->setRequireTickLabel(true );
  countAxis()->setRequireTickLabel(false);

  //---

  bool hasRange = valueSet_.isNumeric();

  int i = 0;
  int n = numValues();

  for (const auto &ivalue : ivalues_) {
    int           bucket = ivalue.first;
    const Values &values = ivalue.second;

    CQChartsGeom::BBox bbox;

    if (! isHorizontal())
      bbox = CQChartsGeom::BBox(bucket - 0.5, 0, bucket + 0.5, values.size());
    else
      bbox = CQChartsGeom::BBox(0, bucket - 0.5, values.size(), bucket + 0.5);

    CQChartsDistributionBarObj *barObj =
      new CQChartsDistributionBarObj(this, bbox, bucket, values, i, n);

    addPlotObject(barObj);

    //---

    QString bucketStr1 = bucketValuesStr(bucket, BucketValueType::START);
    QString bucketStr2 = bucketValuesStr(bucket, BucketValueType::END  );

    if (hasRange) {
      valueAxis()->setTickLabel(bucket    , bucketStr1);
      valueAxis()->setTickLabel(bucket + 1, bucketStr2);
    }
    else
      valueAxis()->setTickLabel(bucket, bucketStr1);

    //---

    ++i;
  }

  //---

  QAbstractItemModel *model = this->model();

  bool ok;

  QString valueName = headerValue(model, valueColumn(), ok).toString();

  valueAxis()->setLabel(valueName);
  countAxis()->setLabel("Count");

  //---

  resetKeyItems();

  //---

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
addKeyItems(CQChartsKey *key)
{
  int row = key->maxRow();

  int n = numValues();

  for (const auto &ivalue : ivalues_) {
    int bucket = ivalue.first;

    QString bucketStr = bucketValuesStr(bucket);

    CQChartsDistKeyColorBox *color = new CQChartsDistKeyColorBox(this, bucket, n);
    CQChartsKeyText         *text  = new CQChartsKeyText        (this, bucketStr);

    key->addItem(color, row, 0);
    key->addItem(text , row, 1);

    ++row;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int bucket, BucketValueType type) const
{
  bool hasRange = valueSet_.isNumeric();

  if (hasRange) {
    double value1, value2;

    bucketValues(bucket, value1, value2);

    if      (type == BucketValueType::ALL) {
      QChar arrowChar(0x2192);

      return QString("%1%2%3").arg(value1).arg(arrowChar).arg(value2);
    }
    else if (type == BucketValueType::START)
      return QString("%1").arg(value1);
    else
      return QString("%1").arg(value2);
  }
  else {
    return valueSet_.inds(bucket);
  }
}

void
CQChartsDistributionPlot::
bucketValues(int bucket, double &value1, double &value2) const
{
  value1 = 0.0;
  value2 = 0.0;

  bool isAuto = (! filters_.empty() || categoryRange_.type == CategoryRange::Type::AUTO);

  if (isAuto) {
    if (categoryRange_.increment > 0.0) {
      value1 = bucket*categoryRange_.increment + categoryRange_.calcMinValue;
      value2 = value1 + categoryRange_.increment;
    }
  }
  else {
    if (deltaValue() > 0.0) {
      value1 = bucket*deltaValue() + calcStartValue();
      value2 = value1 + deltaValue();
    }
  }

  if (CQChartsUtil::isZero(value1)) value1 = 0.0;
  if (CQChartsUtil::isZero(value2)) value2 = 0.0;
}

//------

bool
CQChartsDistributionPlot::
addMenuItems(QMenu *menu)
{
  QAction *pushAction = new QAction("Push", menu);
  QAction *popAction  = new QAction("Pop" , menu);

  connect(pushAction, SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction , SIGNAL(triggered()), this, SLOT(popSlot()));

  menu->addAction(pushAction);
  menu->addAction(popAction );

  return true;
}

void
CQChartsDistributionPlot::
pushSlot()
{
  QPointF gpos = view()->menuPos();

  QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

  CQChartsGeom::Point w;

  pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

  PlotObjs objs;

  objsAtPoint(w, objs);

  if (objs.empty())
    return;

  CQChartsDistributionBarObj *obj = qobject_cast<CQChartsDistributionBarObj *>(objs[0]);

  double value1, value2;

  bucketValues(obj->bucket(), value1, value2);

  filters_.emplace_back(value1, value2);

  updateRangeAndObjs();
}

void
CQChartsDistributionPlot::
popSlot()
{
  if (filters_.empty())
    return;

  filters_.pop_back();

  updateRangeAndObjs();
}

//------

void
CQChartsDistributionPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsDistributionPlot::
drawDataLabel(QPainter *painter, const QRectF &qrect, const QString &ystr)
{
  dataLabel_.draw(painter, qrect, ystr);
}

//------

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                           int bucket, const Values &values, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), bucket_(bucket), values_(values), i_(i), n_(n)
{
}

QString
CQChartsDistributionBarObj::
calcId() const
{
  QString bucketStr = plot_->bucketValuesStr(bucket_);

  return QString("%1 : %2").arg(bucketStr).arg(values_.size());
}

void
CQChartsDistributionBarObj::
addSelectIndex()
{
  for (const auto &value : values_) {
    plot_->addSelectIndex(value);
  }
}

bool
CQChartsDistributionBarObj::
isIndex(const QModelIndex &ind) const
{
  for (const auto &value : values_) {
    if (ind == value)
      return true;
  }

  return false;
}

void
CQChartsDistributionBarObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  painter->save();

  //---

  CQChartsGeom::BBox bbox;

  double m;

  if (! plot_->isHorizontal())
    m = plot_->pixelToWindowWidth(plot_->margin());
  else
    m = plot_->pixelToWindowHeight(plot_->margin());

  if (2*m > 1)
    m = m/2;

  if (! plot_->isHorizontal())
    bbox = CQChartsGeom::BBox(bucket_ - 0.5 + m, 0, bucket_ + 0.5 - m, values_.size());
  else
    bbox = CQChartsGeom::BBox(0, bucket_ - 0.5 + m, values_.size(), bucket_ + 0.5 - m);

  CQChartsGeom::BBox pbbox;

  plot_->windowToPixel(bbox, pbbox);

  QRectF qrect = CQChartsUtil::toQRect(pbbox);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    // calc pen (stroke)
    QPen pen;

    if (plot_->isBorder()) {
      pen.setColor (plot_->interpBorderColor(0, 1));
      pen.setWidthF(plot_->borderWidth());
    }
    else {
      pen.setStyle(Qt::NoPen);
    }

    // calc brush (fill)
    QBrush barBrush;

    if (plot_->isBarFill()) {
      QColor barColor = plot_->interpBarColor(i_, n_);

      OptColor color;

      if (! values_.empty()) {
        QModelIndex colorInd = plot_->unnormalizeIndex(values_[0]);

        (void) plot_->colorSetColor(colorInd.row(), color);
      }

      if (color)
        barColor = color->interpColor(plot_, 0, 1);

      barColor.setAlphaF(plot_->barAlpha());

      barBrush.setColor(barColor);
      barBrush.setStyle(CQChartsFillObj::patternToStyle(
        (CQChartsFillObj::Pattern) plot_->barPattern()));
    }
    else {
      barBrush.setStyle(Qt::NoBrush);
    }

    plot_->updateObjPenBrushState(this, pen, barBrush);

    //---

    // draw rect
    painter->setPen(pen);
    painter->setBrush(barBrush);

    CQChartsRoundedPolygon::draw(painter, qrect, plot_->borderCornerSize());
  }
  else {
    QString ystr = QString("%1").arg(values_.size());

    plot_->drawDataLabel(painter, qrect, ystr);
  }

  painter->restore();
}

//------

CQChartsDistKeyColorBox::
CQChartsDistKeyColorBox(CQChartsDistributionPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

QBrush
CQChartsDistKeyColorBox::
fillBrush() const
{
  CQChartsDistributionPlot *plot = qobject_cast<CQChartsDistributionPlot *>(plot_);

  QColor barColor = plot->interpBarColor(i_, n_);

  CQChartsDistributionPlot::OptColor color;

  const CQChartsDistributionPlot::Values &values = plot->ivalues(i_);

  if (! values.empty()) {
    QModelIndex colorInd = plot->unnormalizeIndex(values[0]);

    (void) plot->colorSetColor(colorInd.row(), color);

    if (color)
      barColor = color->interpColor(plot, 0, 1);
  }

  return barColor;
}
