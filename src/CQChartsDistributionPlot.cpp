#include <CQChartsDistributionPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
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
  borderObj_ = new CQChartsBoxObj;
  fillObj_   = new CQChartsFillObj;

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

const QColor &
CQChartsDistributionPlot::
borderColor() const
{
  return borderObj_->borderColor();
}

void
CQChartsDistributionPlot::
setBorderColor(const QColor &c)
{
  borderObj_->setBorderColor(c); update();
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
barColor(int i, int n) const
{
  if (! fillObj_->isPalette())
    return fillObj_->color();

  return paletteColor(i, n);
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

  xAxis_->clearTickLabels();
  yAxis_->clearTickLabels();

  xAxis()->setIntegral(true);
  yAxis()->setIntegral(true);

  //---

  QChar arrowChar(0x2192);

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

    double value1 = 0.0;
    double value2 = 0.0;

    if (deltaValue() > 0.0) {
      value1 = bucket*deltaValue() + startValue();
      value2 = value1 + deltaValue();
    }

    QString id = QString("%1%2%3 : %4").arg(value1).arg(arrowChar).arg(value2).arg(values.size());

    barObj->setId(id);

    addPlotObject(barObj);

    //---

    QString xId = QString("%1%2%3").arg(value1).arg(arrowChar).arg(value2);

    if (! isHorizontal())
      xAxis_->setTickLabel(bucket, xId);
    else
      yAxis_->setTickLabel(bucket, xId);

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
}

void
CQChartsDistributionPlot::
addKeyItems(CQChartsKey *)
{
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
      pen.setColor(plot_->borderColor());
      pen.setWidth(plot_->borderWidth());
    }
    else {
      pen.setStyle(Qt::NoPen);
    }

    // calc brush (fill)
    QBrush barBrush;

    if (plot_->isBarFill()) {
      QColor barColor = plot_->barColor(i_, n_);

      barColor.setAlpha(plot_->barAlpha()*255);

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
