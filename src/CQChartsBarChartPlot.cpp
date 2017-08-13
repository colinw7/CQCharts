#include <CQChartsBarChartPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBarChartPlot::
CQChartsBarChartPlot(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
  addAxes();

  addKey();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  xAxis_->setIntegral(true);
  xAxis_->setDataLabels(true);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);

  addProperty("", this, "barColor");
  addProperty("", this, "stacked" );
}

void
CQChartsBarChartPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  dataRange_.updateRange(-0.5, 0);

  //---

  valueSets_.clear();

  int ny = numSets();

  for (int i = 0; i < n; ++i) {
    QString name = CQChartsUtil::modelString(model_, i, xColumn_);

    double sum = 0.0;

    ValueSet *valueSet = getValueSet(name);

    for (int j = 0; j < ny; ++j) {
      int yColumn = getSetColumn(j);

      double value = CQChartsUtil::modelReal(model_, i, yColumn);

      valueSet->values.push_back(value);

      if (isStacked())
        dataRange_.updateRange(0, sum + value);
      else
        dataRange_.updateRange(0, value);

      sum += value;
    }
  }

  //---

  valueNames_.clear();

  for (int j = 0; j < ny; ++j) {
    int yColumn = getSetColumn(j);

    QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

    valueNames_.push_back(name);
  }

  //---

  int nv = numValueSets();

  dataRange_.updateRange(nv - 0.5, dataRange_.ymin());

  applyDataRange();
}

int
CQChartsBarChartPlot::
numSets() const
{
  if (yColumns_.empty())
    return 1;

  return yColumns_.size();
}

int
CQChartsBarChartPlot::
getSetColumn(int i) const
{
  if (! yColumns_.empty())
    return yColumns_[i];
  else
    return yColumn_;
}

CQChartsBarChartPlot::ValueSet *
CQChartsBarChartPlot::
getValueSet(const QString &name)
{
  int nv = numValueSets();

  for (int j = 0; j < nv; ++j) {
    if (valueSets_[j].name == name)
      return &valueSets_[j];
  }

  valueSets_.emplace_back(name);

  return &valueSets_.back();
}

void
CQChartsBarChartPlot::
initObjs(bool force)
{
  if (force) {
    for (auto &plotObj : plotObjs_)
      delete plotObj;

    plotObjs_.clear();

    dataRange_.reset();

    plotObjTree_.reset();
  }

  //--

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  // start at px1 - bar width
  double bx = -0.5;

  int nv = numValueSets();

  for (int j = 0; j < nv; ++j) {
    const ValueSet &valueSet = valueSets_[j];

    QString setName = valueSet.name;

    double bx1 = bx;
    double bw1 = (isStacked() ? 1.0 : 1.0/valueSet.values.size());

    double sum = 0.0;

    int nv1 = valueSet.values.size();

    for (int i = 0; i < nv1; ++i) {
      double value = valueSet.values[i];

      double value1 = value + sum;

      //---

      // create bar rect
      CBBox2D brect;

      if (isStacked())
        brect = CBBox2D(bx, sum, bx + 1.0, value1);
      else
        brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);

      CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, brect, i, j);

      QString valueName = valueNames_[i];

      barObj->setId(QString("%1:%2:%3").arg(setName).arg(valueName).arg(value));

      addPlotObject(barObj);

      //---

      bx1 += bw1;
      sum += value;
    }

    bx += 1.0;
  }

  //----

  key_->clearItems();

  int nv1 = numSetValues();

  if (nv1 > 1) {
    for (int i = 0; i < nv1; ++i) {
      CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, true, i);
      CQChartsKeyText     *text  = new CQChartsKeyText    (this, valueNames_[i]);

      key_->addItem(color, i, 0);
      key_->addItem(text , i, 1);
    }
  }
  else {
    for (int i = 0; i < nv; ++i) {
      const ValueSet &valueSet = valueSets_[i];

      CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, false, i);
      CQChartsKeyText     *text  = new CQChartsKeyText    (this, valueSet.name);

      key_->addItem(color, i, 0);
      key_->addItem(text , i, 1);
    }
  }
}

int
CQChartsBarChartPlot::
numValueSets() const
{
  return valueSets_.size();
}

int
CQChartsBarChartPlot::
numSetValues() const
{
  if (! valueSets_.empty())
    return valueSets_[0].values.size();
  else
    return 0;
}

void
CQChartsBarChartPlot::
paintEvent(QPaintEvent *)
{
  initObjs();

  //---

  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  drawBackground(&p);

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(&p);

  drawAxes(&p);

  //---

  drawKey(&p);
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect, int iset, int ival) :
 CQChartsPlotObj(rect), plot_(plot), iset_(iset), ival_(ival)
{
}

void
CQChartsBarChartObj::
draw(QPainter *p)
{
  CBBox2D prect;

  plot_->windowToPixel(rect(), prect);

  int m = 2;

  if (prect.getWidth() > 3*m) {
    prect.setXMin(prect.getXMin() + m);
    prect.setXMax(prect.getXMax() - m);
  }

  p->setPen(stroke());

  int nv  = plot_->numValueSets();
  int nv1 = plot_->numSetValues();

  QColor barColor = plot_->barColor();

  if (nv1 > 1)
    barColor = plot_->objectColor(this, iset_, nv1, barColor);
  else
    barColor = plot_->objectColor(this, ival_, nv , barColor);

  p->setBrush(barColor);

  QRectF qrect = CQUtil::toQRect(prect);

  p->drawRect(qrect);
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, bool valueColor, int ind) :
 CQChartsKeyItem(plot->key()), plot_(plot), valueColor_(valueColor), ind_(ind)
{
}

QSizeF
CQChartsBarKeyColor::
size() const
{
  QFontMetrics fm(plot_->font());

  double h = fm.height();

  double ww = plot_->pixelToWindowWidth (h + 2);
  double wh = plot_->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

void
CQChartsBarKeyColor::
draw(QPainter *p, const CBBox2D &rect)
{
  p->setPen(Qt::black);

  CBBox2D prect;

  plot_->windowToPixel(rect, prect);
  plot_->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  int nv = (valueColor_ ? plot_->numSetValues() : plot_->numValueSets());

  QColor c = plot_->paletteColor(ind_, nv, plot_->barColor());

  p->setPen  (Qt::black);
  p->setBrush(c);

  p->drawRect(prect1);
}
