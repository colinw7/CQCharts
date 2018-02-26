#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRoundedPolygon.h>

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
  addColumnParameter("value", "Value", "valueColumn", "", 0);
  addColumnParameter("color", "Color", "colorColumn", "optional");

  addBoolParameter("horizontal", "Horizontal", "horizontal", "optional");

  addBoolParameter("autoRange", "Auto Range", "autoRange" , "optional", true);
  addRealParameter("start"    , "Start"     , "startValue", "optional");
  addRealParameter("delta"    , "Delta"     , "deltaValue", "optional");
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
  (void) addValueSet("values");
  (void) addColorSet("color");

  setBarFill (true);
  setBarColor(CQChartsColor(CQChartsColor::Type::PALETTE));

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

  addProperty("stroke", this, "border"     , "visible"   );
  addProperty("stroke", this, "borderColor", "color"     );
  addProperty("stroke", this, "borderAlpha", "alpha"     );
  addProperty("stroke", this, "borderWidth", "width"     );
  addProperty("stroke", this, "cornerSize" , "cornerSize");

  addProperty("fill", this, "barFill"   , "visible");
  addProperty("fill", this, "barColor"  , "color"  );
  addProperty("fill", this, "barAlpha"  , "alpha"  );
  addProperty("fill", this, "barPattern", "pattern");

  dataLabel_.addProperties("dataLabel");

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

//---

void
CQChartsDistributionPlot::
setHorizontal(bool b)
{
  if (b != horizontal_) {
    horizontal_ = b;

    dataLabel_.setDirection(horizontal_ ?
      CQChartsDataLabel::Direction::HORIZONTAL : CQChartsDataLabel::Direction::VERTICAL);

    updateRangeAndObjs();
  }
}

//---

bool
CQChartsDistributionPlot::
isBorder() const
{
  return boxData_.shape.border.visible;
}

void
CQChartsDistributionPlot::
setBorder(bool b)
{
  boxData_.shape.border.visible = b;

  update();
}

const CQChartsColor &
CQChartsDistributionPlot::
borderColor() const
{
  return boxData_.shape.border.color;
}

void
CQChartsDistributionPlot::
setBorderColor(const CQChartsColor &c)
{
  boxData_.shape.border.color = c;

  update();
}

QColor
CQChartsDistributionPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsDistributionPlot::
borderAlpha() const
{
  return boxData_.shape.border.alpha;
}

void
CQChartsDistributionPlot::
setBorderAlpha(double a)
{
  boxData_.shape.border.alpha = a;

  update();
}

const CQChartsLength &
CQChartsDistributionPlot::
borderWidth() const
{
  return boxData_.shape.border.width;
}

void
CQChartsDistributionPlot::
setBorderWidth(const CQChartsLength &l)
{
  boxData_.shape.border.width = l;

  update();
}

const CQChartsLength &
CQChartsDistributionPlot::
cornerSize() const
{
  return boxData_.cornerSize;
}

void
CQChartsDistributionPlot::
setCornerSize(const CQChartsLength &s)
{
  boxData_.cornerSize = s;

  update();
}

//---

bool
CQChartsDistributionPlot::
isBarFill() const
{
  return boxData_.shape.background.visible;
}

void
CQChartsDistributionPlot::
setBarFill(bool b)
{
  if (b != boxData_.shape.background.visible) {
    boxData_.shape.background.visible = b;

    update();
  }
}

const CQChartsColor &
CQChartsDistributionPlot::
barColor() const
{
  return boxData_.shape.background.color;
}

void
CQChartsDistributionPlot::
setBarColor(const CQChartsColor &c)
{
  if (c != boxData_.shape.background.color) {
    boxData_.shape.background.color = c;

    update();
  }
}

QColor
CQChartsDistributionPlot::
interpBarColor(int i, int n) const
{
  return barColor().interpColor(this, i, n);
}

double
CQChartsDistributionPlot::
barAlpha() const
{
  return boxData_.shape.background.alpha;
}

void
CQChartsDistributionPlot::
setBarAlpha(double a)
{
  if (a != boxData_.shape.background.alpha) {
    boxData_.shape.background.alpha = a;

    update();
  }
}

CQChartsDistributionPlot::Pattern
CQChartsDistributionPlot::
barPattern() const
{
  return (Pattern) boxData_.shape.background.pattern;
}

void
CQChartsDistributionPlot::
setBarPattern(Pattern pattern)
{
  if (pattern != (Pattern) boxData_.shape.background.pattern) {
    boxData_.shape.background.pattern = (CQChartsFillPattern::Type) pattern;

    update();
  }
}

//---

void
CQChartsDistributionPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  dataRange_.reset();

  //---

  // calc category type
  initValueSets();

  //---

  CQChartsValueSet *valueSet = getValueSet("values");

  // calc value range
  bool hasRange = valueSet->isNumeric();

  if (hasRange) {
    class ColumnDetails : public CQChartsUtil::ModelColumnDetails {
     public:
      ColumnDetails(CQChartsDistributionPlot *plot, QAbstractItemModel *model,
                    const CQChartsColumn &column) :
       CQChartsUtil::ModelColumnDetails(plot->charts(), model, column), plot_(plot) {
      }

      bool checkRow(const QVariant &var) override {
        return plot_->checkFilter(var.toReal());
      }

     private:
      CQChartsDistributionPlot *plot_ { nullptr };
    };

    //---

    ColumnDetails columnDetails(this, model, valueColumn());

    categoryRange_.minValue = columnDetails.minValue().toReal();
    categoryRange_.maxValue = columnDetails.maxValue().toReal();

    //---

    calcCategoryRange();
  }

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsDistributionPlot *plot, CQChartsValueSet *valueSet) :
     plot_(plot), valueSet_(valueSet) {
      hasRange_ = valueSet->isNumeric();
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      QModelIndex valueInd  = model->index(row, plot_->valueColumn().column(), parent);
      QModelIndex valueInd1 = plot_->normalizeIndex(valueInd);

      //---

      int bucket = -1;

      if (hasRange_) {
        bool ok;

        double value = CQChartsUtil::modelReal(model, row, plot_->valueColumn(), parent, ok);

        if (! ok)
          return State::SKIP;

        if (CQChartsUtil::isNaN(value))
          return State::SKIP;

        if (! plot_->checkFilter(value))
          return State::SKIP;

        //----

        bucket = plot_->calcBucket(value);

        ivalues_[bucket].emplace_back(valueInd1);
      }
      else {
        bool ok;

        QString value = CQChartsUtil::modelString(model, row, plot_->valueColumn(), parent, ok);

        if (! ok)
          return State::SKIP;

        bucket = valueSet_->sbucket(value);

        ivalues_[bucket].emplace_back(valueInd1);
      }

      //---

      if (bucket >= 0) {
        if (minBucket_ <= maxBucket_) {
          minBucket_ = std::min(minBucket_, bucket);
          maxBucket_ = std::max(maxBucket_, bucket);
        }
        else {
          minBucket_ = bucket;
          maxBucket_ = bucket;
        }
      }

      return State::OK;
    }

    const IValues &ivalues() const { return ivalues_; }

    int minBucket() const { return minBucket_; }
    int maxBucket() const { return maxBucket_; }

   private:
    CQChartsDistributionPlot *plot_      { nullptr };
    CQChartsValueSet         *valueSet_  { nullptr };
    bool                      hasRange_  { false };
    IValues                   ivalues_;
    int                       minBucket_ {  1 };
    int                       maxBucket_ { -1 };
  };

  RowVisitor visitor(this, valueSet);

  visitModel(visitor);

  ivalues_ = visitor.ivalues();

  int minBucket = visitor.minBucket();
  int maxBucket = visitor.maxBucket();

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
checkFilter(double value) const
{
  if (filterStack_.empty())
    return true;

  const Filters &filters = filterStack_.back();

  for (const auto &filter : filters) {
    if (value >= filter.minValue && value < filter.maxValue)
      return true;
  }

  return false;
}

void
CQChartsDistributionPlot::
calcCategoryRange()
{
  double length = categoryRange_.maxValue - categoryRange_.minValue;

  double length1 = length/categoryRange_.numAuto;

  // Calculate nearest Power of Ten to Length
  int power = (length1 > 0 ? CQChartsUtil::RoundDown(log10(length1)) : 1);

  //if (isIntegral()) {
  //  if (power < 0)
  //    power = 1;
  //}

  categoryRange_.increment = 1;

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

  bool isAuto = (! filterStack_.empty() || isAutoRange());

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
  clearValueSets();

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
  initValueSets();

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

  CQChartsValueSet *valueSet = getValueSet("values");

  bool hasRange = valueSet->isNumeric();

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

  QString valueName = CQChartsUtil::modelHeaderString(model, valueColumn(), ok);

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
addKeyItems(CQChartsPlotKey *key)
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
  CQChartsValueSet *valueSet = getValueSet("values");

  bool hasRange = valueSet->isNumeric();

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
    return valueSet->buckets(bucket);
  }
}

void
CQChartsDistributionPlot::
bucketValues(int bucket, double &value1, double &value2) const
{
  value1 = 0.0;
  value2 = 0.0;

  bool isAuto = (! filterStack_.empty() || isAutoRange());

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

//---

bool
CQChartsDistributionPlot::
probe(ProbeData &probeData) const
{
  if (! isHorizontal()) {
    probeData.direction = ProbeData::Direction::VERTICAL;

    if (probeData.x < dataRange_.xmin() + 0.5)
      probeData.x = dataRange_.xmin() + 0.5;

    if (probeData.x > dataRange_.xmax() - 0.5)
      probeData.x = dataRange_.xmax() - 0.5;

    probeData.x = std::round(probeData.x);
  }
  else {
    probeData.direction = ProbeData::Direction::HORIZONTAL;

    if (probeData.y < dataRange_.ymin() + 0.5)
      probeData.y = dataRange_.ymin() + 0.5;

    if (probeData.y > dataRange_.ymax() - 0.5)
      probeData.y = dataRange_.ymax() - 0.5;

    probeData.y = std::round(probeData.y);
  }

  return true;
}

//---

bool
CQChartsDistributionPlot::
addMenuItems(QMenu *menu)
{
  QAction *horizontalAction = new QAction("Horizontal", menu);

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));

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
  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  return true;
}

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

    filters.emplace_back(value1, value2);
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

void
CQChartsDistributionPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
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

CQChartsGeom::BBox
CQChartsDistributionBarObj::
dataLabelRect() const
{
  if (! plot_->dataLabel().isVisible())
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox rect = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(rect);

  QString ystr = QString("%1").arg(values_.size());

  return plot_->dataLabel().calcRect(qrect, ystr);
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

  CQChartsGeom::BBox pbbox = calcRect();

  QRectF qrect = CQChartsUtil::toQRect(pbbox);

  double s = (! plot_->isHorizontal() ? qrect.width() : qrect.height());

  bool useLine = (s <= 2);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    // calc pen (stroke)
    QPen pen;

    if (plot_->isBorder()) {
      QColor c = plot_->interpBorderColor(0, 1);

      c.setAlphaF(plot_->borderAlpha());

      double bw = plot_->lengthPixelWidth(plot_->borderWidth());

      pen.setColor (c);
      pen.setWidthF(bw);
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

        (void) plot_->colorSetColor("color", colorInd.row(), color);
      }

      if (color)
        barColor = color->interpColor(plot_, 0, 1);

      barColor.setAlphaF(plot_->barAlpha());

      barBrush.setColor(barColor);
      barBrush.setStyle(CQChartsFillPattern::toStyle(
       (CQChartsFillPattern::Type) plot_->barPattern()));

      if (useLine) {
        pen.setColor (barColor);
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
      if (! plot_->isHorizontal())
        painter->drawLine(qrect.center().x(), qrect.bottom(), qrect.center().x(), qrect.top());
      else
        painter->drawLine(qrect.left(), qrect.center().y(), qrect.right(), qrect.center().y());
    }
  }
  else {
    QString ystr = QString("%1").arg(values_.size());

    plot_->dataLabel().draw(painter, qrect, ystr);
  }

  painter->restore();
}

CQChartsGeom::BBox
CQChartsDistributionBarObj::
calcRect() const
{
  CQChartsGeom::BBox bbox;

  double m;

  if (! plot_->isHorizontal())
    m = plot_->pixelToWindowWidth (plot_->margin());
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

  return pbbox;
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

    (void) plot->colorSetColor("color", colorInd.row(), color);

    if (color)
      barColor = color->interpColor(plot, 0, 1);
  }

  return barColor;
}
