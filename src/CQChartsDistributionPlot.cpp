#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRoundedPolygon.h>

#include <QAbstractItemModel>
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

  addRealParameter("start", "Start", "startValue", "", 0.0);
  addRealParameter("delta", "Delta", "deltaValue", "", 1.0);
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
  addProperty("value"  , this, "startValue" , "start");
  addProperty("value"  , this, "deltaValue" , "delta");

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

  addProperty("color", this, "colorMapEnabled", "mapEnabled" );
  addProperty("color", this, "colorMapMin"    , "mapMin"     );
  addProperty("color", this, "colorMapMax"    , "mapMax"     );
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

void
CQChartsDistributionPlot::
initColorSet()
{
  colorSet_.clear();

  if (colorColumn() < 0)
    return;

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  for (int i = 0; i < nr; ++i) {
    bool ok;

    QVariant value = CQChartsUtil::modelValue(model, i, colorColumn(), ok);

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

  if (! model)
    return;

  int nr = model->rowCount(QModelIndex());

  dataRange_.reset();

  int minBucket = 1;
  int maxBucket = -1;

  ivalues_.clear();

  for (int r = 0; r < nr; ++r) {
    QModelIndex valueInd = model->index(r, valueColumn());

    QModelIndex valueInd1 = normalizeIndex(valueInd);

    //---

    bool ok;

    double value = CQChartsUtil::modelReal(model, valueInd, ok);

    if (! ok)
      continue;

    if (CQChartsUtil::isNaN(value))
      continue;

    //----

    int bucket = 0;

    if (deltaValue() > 0.0)
      bucket = std::floor((value - startValue())/deltaValue());

    ivalues_[bucket].emplace_back(valueInd1);

    //---

    if (minBucket <= maxBucket) {
      minBucket = std::min(minBucket, bucket);
      maxBucket = std::max(maxBucket, bucket);
    }
    else {
      minBucket = bucket;
      maxBucket = bucket;
    }
  }

  int maxValues = 0;

  for (const auto &ivalue : ivalues_)
    maxValues = std::max(maxValues, int(ivalue.second.size()));

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

void
CQChartsDistributionPlot::
updateObjs()
{
  colorSet_.clear();

  CQChartsPlot::updateObjs();
}

void
CQChartsDistributionPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  // init color value set
  if (colorSet_.empty())
    initColorSet();

  //---

  xAxis_->clearTickLabels();
  yAxis_->clearTickLabels();

  xAxis()->setIntegral(true);
  yAxis()->setIntegral(true);

  xAxis()->setRequireTickLabel(! isHorizontal());
  yAxis()->setRequireTickLabel(  isHorizontal());

  //---

  int i = 0;
  int n = ivalues_.size();

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

    QString bucketStr =  bucketValuesStr(bucket);

    if (! isHorizontal())
      xAxis_->setTickLabel(bucket, bucketStr);
    else
      yAxis_->setTickLabel(bucket, bucketStr);

    //---

    ++i;
  }

  QString valueName = model()->headerData(valueColumn(), Qt::Horizontal).toString();

  if (! isHorizontal()) {
    xAxis_->setLabel(valueName);
    yAxis_->setLabel("Count");
  }
  else {
    yAxis_->setLabel(valueName);
    xAxis_->setLabel("Count");
  }

  //---

  resetKeyItems();

  //---

  initObjTree();
}

void
CQChartsDistributionPlot::
addKeyItems(CQChartsKey *key)
{
  int row = key->maxRow();

  int i = 0;
  int n = ivalues_.size();

  for (const auto &ivalue : ivalues_) {
    int bucket = ivalue.first;

    QString bucketStr = bucketValuesStr(bucket);

    CQChartsKeyColorBox *color = new CQChartsKeyColorBox(this, i, n);
    CQChartsKeyText     *text  = new CQChartsKeyText    (this, bucketStr);

    key->addItem(color, row, 0);
    key->addItem(text , row, 1);

    ++row;

    ++i;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

QString
CQChartsDistributionPlot::
bucketValuesStr(int bucket) const
{
  QChar arrowChar(0x2192);

  double value1, value2;

  bucketValues(bucket, value1, value2);

  return QString("%1%2%3").arg(value1).arg(arrowChar).arg(value2);
}

void
CQChartsDistributionPlot::
bucketValues(int bucket, double &value1, double &value2) const
{
  value1 = 0.0;
  value2 = 0.0;

  if (deltaValue() > 0.0) {
    value1 = bucket*deltaValue() + startValue();
    value2 = value1 + deltaValue();
  }
}

void
CQChartsDistributionPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsDistributionPlot::
drawDataLabel(QPainter *p, const QRectF &qrect, const QString &ystr)
{
  dataLabel_.draw(p, qrect, ystr);
}

//------

CQChartsDistributionBarObj::
CQChartsDistributionBarObj(CQChartsDistributionPlot *plot, const CQChartsGeom::BBox &rect,
                           int bucket, const Values &values, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), bucket_(bucket), values_(values), i_(i), n_(n)
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
mousePress(const CQChartsGeom::Point &)
{
  plot_->beginSelect();

  for (const auto &value : values_) {
    plot_->addSelectIndex(value);
  }

  plot_->endSelect();
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
draw(QPainter *p, const CQChartsPlot::Layer &layer)
{
  p->save();

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
    p->setPen(pen);
    p->setBrush(barBrush);

    CQRoundedPolygon::draw(p, qrect, plot_->borderCornerSize());
  }
  else {
    QString ystr = QString("%1").arg(values_.size());

    plot_->drawDataLabel(p, qrect, ystr);
  }

  p->restore();
}
