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

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  xAxis_->setIntegral(true);
  xAxis_->setDataLabels(true);

  addProperty("", this, "barColor");
}

void
CQChartsBarChartPlot::
updateRange()
{
  QModelIndex ind;

  int n = model_->rowCount(ind);

  dataRange_.reset();

  dataRange_.updateRange(-0.5, 0);

  if (! yColumns_.empty()) {
    int ny = yColumns_.size();

    for (int i = 0; i < n; ++i) {
      QString name = CQChartsUtil::modelString(model_, i, xColumn_);

      ValueSet *valueSet = getValueSet(name);

      for (int j = 0; j < ny; ++j) {
        double value = CQChartsUtil::modelReal(model_, i, yColumns_[j]);

        valueSet->values.push_back(value);

        dataRange_.updateRange(0, value);
      }
    }
  }
  else {
    for (int i = 0; i < n; ++i) {
      QString name = CQChartsUtil::modelString(model_, i, xColumn_);

      ValueSet *valueSet = getValueSet(name);

      double value = CQChartsUtil::modelReal(model_, i, yColumn_);

      valueSet->values.push_back(value);

      dataRange_.updateRange(0, value);
    }
  }

  dataRange_.updateRange(valueSets_.size() - 0.5, dataRange_.ymin());

  displayRange_.setWindowRange(dataRange_.xmin(), dataRange_.ymin(),
                               dataRange_.xmax(), dataRange_.ymax());

  if (xAxis_) {
    xAxis_->setRange(dataRange_.xmin(), dataRange_.xmax());
    yAxis_->setRange(dataRange_.ymin(), dataRange_.ymax());
  }
}

CQChartsBarChartPlot::ValueSet *
CQChartsBarChartPlot::
getValueSet(const QString &name)
{
  for (size_t j = 0; j < valueSets_.size(); ++j) {
    if (valueSets_[j].name == name)
      return &valueSets_[j];
  }

  valueSets_.emplace_back(name);

  return &valueSets_.back();
}

void
CQChartsBarChartPlot::
initObjs()
{
  if (! plotObjs_.empty())
    return;

  //---

  // calc bar width in pixels
  double px1, py1, px2, py2;

  windowToPixel(0.0, 0.0, px1, py1);
  windowToPixel(1.0, 0.0, px2, py2);

  double bw = px2 - px1; // bar width

  //---

  // start at px1 - bar width
  double bx = px1 - bw/2.0;

  int nv = valueSets_.size();

  for (int j = 0; j < nv; ++j) {
    const ValueSet &valueSet = valueSets_[j];

    QString setName = valueSet.name;

    double bx1 = bx;
    double bw1 = bw/valueSet.values.size();

    double mbw1 = std::max(1.0, bw1 - 2.0);

    int nv1 = valueSet.values.size();

    for (int i = 0; i < nv1; ++i) {
      // calc bar height in pixel
      double value = valueSet.values[i];

      windowToPixel(0, 0.0  , px1, py2);
      windowToPixel(0, value, px2, py1);

      double bh = py2 - py1; // bar height

      //---

      // create bar rect
      CBBox2D prect(CPoint2D(bx1 + (bw1 - mbw1)/2, py1), CSize2D(mbw1, bh));

      CBBox2D wrect;

      pixelToWindow(prect, wrect);

      CQChartsBarChartObj *barObj = new CQChartsBarChartObj(this, wrect, i, j);

      barObj->setId(QString("%1:%2").arg(i).arg(j));

      addPlotObject(barObj);

      //---

      bx1 += bw1;
    }

    bx += bw;
  }
}

int
CQChartsBarChartPlot::
numSets() const
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

  p.fillRect(rect(), QBrush(background()));

  //---

#if 0
  int pxmin = getPixelRange().getXRange().min();
  int pymin = getPixelRange().getYRange().min();
  int pxmax = getPixelRange().getXRange().max();
  int pymax = getPixelRange().getYRange().max();

  p->fillRect(QRect(pxmin, pymin, pxmax - pxmin - 1, pymax - pymin - 1), QBrush(background()));
#endif

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(&p);

  drawAxes(&p);

#if 0
  for (auto legend : legends_)
    legend->draw(this, p);
#endif
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

  p->setPen(stroke());

  QColor barColor = plot_->objectColor(this, iset_, plot_->numSetValues(), plot_->barColor());

  p->setBrush(barColor);

  QRectF qrect = CQUtil::toQRect(prect);

  p->drawRect(qrect);
}
