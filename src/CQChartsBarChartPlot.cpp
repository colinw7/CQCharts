#include <CQChartsBarChartPlot.h>
#include <CQChartsWindow.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsBarChartPlot::
CQChartsBarChartPlot(CQChartsWindow *window, QAbstractItemModel *model) :
 CQChartsPlot(window, model)
{
  addAxes();

  addKey();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  xAxis_->setIntegral(true);
  xAxis_->setDataLabels(true);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);
}

void
CQChartsBarChartPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty(id_, this, "stacked"    );
  addProperty(id_, this, "barColor"   );
  addProperty(id_, this, "borderColor");
}

QString
CQChartsBarChartPlot::
barColorStr() const
{
  if (barColorPalette_)
    return "palette";

  return barColor_.name();
}

void
CQChartsBarChartPlot::
setBarColorStr(const QString &str)
{
  if (str == "palette") {
    barColorPalette_ = true;
  }
  else {
    barColorPalette_ = false;
    barColor_        = QColor(str);
  }
}

QColor
CQChartsBarChartPlot::
barColor(int i, int n) const
{
  if (barColorPalette_)
    return paletteColor(i, n);

  return barColor_;
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
    clearPlotObjects();

    dataRange_.reset();
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

    //---

    int nv1 = valueSet.values.size();

    int numVisible = 0;

    for (int i = 0; i < nv1; ++i) {
      bool hidden = (nv1 > 1 ? isSetHidden(i) : isSetHidden(j));

      if (hidden)
        continue;

      ++numVisible;
    }

    if (! numVisible)
      continue;

    //---

    double bx1 = bx;
    double bw1 = (isStacked() ? 1.0 : 1.0/numVisible);

    double sum = 0.0;

    for (int i = 0; i < nv1; ++i) {
      bool hidden = (nv1 > 1 ? isSetHidden(i) : isSetHidden(j));

      if (hidden)
        continue;

      //---

      double value = valueSet.values[i];

      double value1 = value + sum;

      //---

      // create bar rect
      CBBox2D brect;

      if (isStacked())
        brect = CBBox2D(bx, sum, bx + 1.0, value1);
      else
        brect = CBBox2D(bx1, 0.0, bx1 + bw1, value);

      CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, brect, i, nv1, j, nv);

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
      CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, nv1);
      CQChartsKeyText     *text  = new CQChartsKeyText    (this, valueNames_[i]);

      key_->addItem(color, i, 0);
      key_->addItem(text , i, 1);
    }
  }
  else {
    for (int i = 0; i < nv; ++i) {
      const ValueSet &valueSet = valueSets_[i];

      CQChartsBarKeyColor *color = new CQChartsBarKeyColor(this, i, nv);
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
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(p);

  drawAxes(p);

  //---

  drawKey(p);
}

//------

CQChartsBarChartObj::
CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect,
                    int iset, int nset, int ival, int nval) :
 CQChartsPlotObj(rect), plot_(plot), iset_(iset), nset_(nset), ival_(ival), nval_(nval)
{
}

void
CQChartsBarChartObj::
draw(QPainter *p)
{
  bool hidden = (nset_ > 1 ? plot_->isSetHidden(iset_) : plot_->isSetHidden(ival_));

  if (hidden)
    return;

  //---

  CBBox2D prect;

  plot_->windowToPixel(rect(), prect);

  int m = 2;

  if (prect.getWidth() > 3*m) {
    prect.setXMin(prect.getXMin() + m);
    prect.setXMax(prect.getXMax() - m);
  }

  p->setPen(plot_->borderColor());

  QColor barColor;

  if (nset_ > 1)
    barColor = plot_->objectStateColor(this, plot_->barColor(iset_, nset_));
  else
    barColor = plot_->objectStateColor(this, plot_->barColor(ival_, nval_));

  p->setBrush(barColor);

  QRectF qrect = CQUtil::toQRect(prect);

  p->drawRect(qrect);
}

//------

CQChartsBarKeyColor::
CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

void
CQChartsBarKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsBarChartPlot *plot = qobject_cast<CQChartsBarChartPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();
}
