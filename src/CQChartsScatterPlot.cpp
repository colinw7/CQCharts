#include <CQChartsScatterPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsScatterPlot::
CQChartsScatterPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  addAxes();

  addKey();

  addTitle();
}

void
CQChartsScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn", "name");
  addProperty("columns", this, "xColumn"   , "x"   );
  addProperty("columns", this, "yColumn"   , "y"   );
  addProperty(""       , this, "symbolSize"        );
}

void
CQChartsScatterPlot::
updateRange()
{
  int n = numRows();

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);
    double y = CQChartsUtil::modelReal(model_, i, yColumn_, ok2);

    if (! ok1) x = i;
    if (! ok2) y = i;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
      continue;

    dataRange_.updateRange(x, y);
  }

  //---

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();
  QString yname = model_->headerData(yColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  applyDataRange();
}

int
CQChartsScatterPlot::
numRows() const
{
  return model_->rowCount(QModelIndex());
}

int
CQChartsScatterPlot::
nameIndex(const QString &name) const
{
  int i = 0;

  for (const auto &nv : nameValues_) {
    if (nv.first == name)
      return i;

    ++i;
  }

  return 0;
}

void
CQChartsScatterPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //---

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  // init name values
  if (nameValues_.empty()) {
    int n = numRows();

    for (int i = 0; i < n; ++i) {
      bool ok;

      QString name = CQChartsUtil::modelString(model_, i, nameColumn_, ok);

      bool ok1, ok2;

      double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);
      double y = CQChartsUtil::modelReal(model_, i, yColumn_, ok2);

      if (! ok1) x = i;
      if (! ok2) y = i;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        continue;

      nameValues_[name].push_back(QPointF(x, y));
    }
  }

  //---

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int nv = nameValues_.size();

  int i = 0;

  for (const auto &nameValues : nameValues_) {
    bool hidden = isSetHidden(i);

    if (! hidden) {
      const QString &name   = nameValues.first;
      const Values  &values = nameValues.second;

      int nv1 = values.size();

      for (int j = 0; j < nv1; ++j) {
        const QPointF &p = values[j];

        CBBox2D bbox(p.x() - sw/2, p.y() - sh/2, p.x() + sw/2, p.y() + sh/2);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, bbox, p, i, nv);

        pointObj->setId(QString("%1:%2:%3").arg(name).arg(p.x()).arg(p.y()));

        addPlotObject(pointObj);
      }
    }

    ++i;
  }

  //---

  keyObj_->clearItems();

  addKeyItems(keyObj_);
}

void
CQChartsScatterPlot::
addKeyItems(CQChartsKey *key)
{
  int nv = nameValues_.size();

  int i = 0;

  for (const auto &nameValue: nameValues_) {
    const QString &name = nameValue.first;

    CQChartsScatterKeyColor *color = new CQChartsScatterKeyColor(this, i, nv);
    CQChartsKeyText         *text  = new CQChartsKeyText        (this, name);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);

    ++i;
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, const QPointF &p,
                        int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), p_(p), i_(i), n_(n)
{
}

bool
CQChartsScatterPointObj::
inside(const CPoint2D &p) const
{
  int s = plot_->symbolSize();

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CBBox2D pbbox(px - s, py - s, px + s, py + s);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
draw(QPainter *p)
{
  int s = plot_->symbolSize();

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  QColor color = plot_->objectColor(this, i_, n_, Qt::blue);

  p->setPen  (Qt::black);
  p->setBrush(color);

  QRectF erect(px - s, py - s, 2*s, 2*s);

  p->drawEllipse(erect);
}

//------

CQChartsScatterKeyColor::
CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsScatterKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

QColor
CQChartsScatterKeyColor::
fillColor() const
{
  QColor c = CQChartsKeyColorBox::fillColor();

  CQChartsScatterPlot *plot = qobject_cast<CQChartsScatterPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}
