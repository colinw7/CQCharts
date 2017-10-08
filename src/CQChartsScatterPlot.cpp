#include <CQChartsScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsScatterPlotType::
CQChartsScatterPlotType()
{
  addColumnParameter("x", "X", "xColumn", "", 0);
  addColumnParameter("y", "Y", "yColumn", "", 1);

  addColumnParameter("name" , "Name" , "nameColumn" , "optional");
  addColumnParameter("size" , "Size" , "sizeColumn" , "optional");
  addColumnParameter("color", "Color", "colorColumn", "optional");
}

CQChartsPlot *
CQChartsScatterPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsScatterPlot(view, model);
}

//---

CQChartsScatterPlot::
CQChartsScatterPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("scatter"), model)
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

  addProperty("columns", this, "nameColumn"       , "name"       );
  addProperty("columns", this, "xColumn"          , "x"          );
  addProperty("columns", this, "yColumn"          , "y"          );
  addProperty("columns", this, "sizeColumn"       , "size"       );
  addProperty("columns", this, "colorColumn"      , "color"      );
  addProperty("symbol" , this, "symbolSize"       , "size"       );
  addProperty("symbol" , this, "symbolBorderColor", "borderColor");
  addProperty("symbol" , this, "symbolSizeMin"    , "sizeMin"    );
  addProperty("symbol" , this, "symbolSizeMax"    , "sizeMax"    );
}

void
CQChartsScatterPlot::
updateRange()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int n = numRows();

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    bool ok1, ok2;

    double x = CQChartsUtil::modelReal(model, i, xColumn(), ok1);
    double y = CQChartsUtil::modelReal(model, i, yColumn(), ok2);

    if (! ok1) x = i;
    if (! ok2) y = i;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
      continue;

    dataRange_.updateRange(x, y);
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  QString xname = model->headerData(xColumn(), Qt::Horizontal).toString();
  QString yname = model->headerData(yColumn(), Qt::Horizontal).toString();

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  applyDataRange();
}

int
CQChartsScatterPlot::
numRows() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return 0;

  return model->rowCount(QModelIndex());
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

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  // init name values
  if (nameValues_.empty()) {
    int n = numRows();

    if (sizeColumn() >= 0) {
      bool ok;

      for (int i = 0; i < n; ++i)
        sizeSet_.addValue(CQChartsUtil::modelValue(model, i, sizeColumn(), ok));
    }

    if (colorColumn() >= 0) {
      bool ok;

      for (int i = 0; i < n; ++i)
        colorSet_.addValue(CQChartsUtil::modelValue(model, i, colorColumn(), ok));
    }

    for (int i = 0; i < n; ++i) {
      bool ok;

      QString name = CQChartsUtil::modelString(model, i, nameColumn(), ok);

      bool ok1, ok2;

      double x = CQChartsUtil::modelReal(model, i, xColumn(), ok1);
      double y = CQChartsUtil::modelReal(model, i, yColumn(), ok2);

      if (! ok1) x = i;
      if (! ok2) y = i;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        continue;

      bool ok3;

      QString sizeStr = CQChartsUtil::modelString(model, i, sizeColumn(), ok3);
      double  rsize   = (sizeColumn () >= 0 ? sizeSet_ .imap(i) : -1);

      QString colorStr = CQChartsUtil::modelString(model, i, colorColumn(), ok3);
      double  rcolor   = (colorColumn() >= 0 ? colorSet_.imap(i) : -1);

      nameValues_[name].emplace_back(x, y, sizeStr, rsize, colorStr, rcolor);
    }
  }

  //---

  QString xname     = model->headerData(xColumn    (), Qt::Horizontal).toString();
  QString yname     = model->headerData(yColumn    (), Qt::Horizontal).toString();
  QString sizeName  = model->headerData(sizeColumn (), Qt::Horizontal).toString();
  QString colorName = model->headerData(colorColumn(), Qt::Horizontal).toString();

  if (xname     == "") xname     = "x";
  if (yname     == "") yname     = "x";
  if (sizeName  == "") sizeName  = "size";
  if (colorName == "") colorName = "color";

  //double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  //double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int nv = nameValues_.size();

  int i = 0;

  for (const auto &nameValues : nameValues_) {
    bool hidden = isSetHidden(i);

    if (! hidden) {
      const QString &name   = nameValues.first;
      const Values  &values = nameValues.second;

      int nv1 = values.size();

      for (int j = 0; j < nv1; ++j) {
        const QPointF &p = values[j].p;

        double ps = mapSymbolSize(values[j].size);

        double sw = pixelToWindowWidth (ps);
        double sh = pixelToWindowHeight(ps);

        CBBox2D bbox(p.x() - sw, p.y() - sh, p.x() + sw, p.y() + sh);

        CQChartsScatterPointObj *pointObj =
          new CQChartsScatterPointObj(this, bbox, p, values[j].size, values[j].color, i, nv);

        QString id = name;

        id += QString("\n  %1\t%2").arg(xname).arg(p.x());
        id += QString("\n  %1\t%2").arg(yname).arg(p.y());

        if (values[j].sizeStr != "")
          id += QString("\n  %1\t%2").arg(sizeName).arg(values[j].sizeStr);

        if (values[j].colorStr != "")
          id += QString("\n  %1\t%2").arg(colorName).arg(values[j].colorStr);

        pointObj->setId(id);

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

double
CQChartsScatterPlot::
mapSymbolSize(double s) const
{
  if (s < 0)
    return symbolSize();

  return s*(symbolSizeMax() - symbolSizeMin()) + symbolSizeMin();
}

//------

CQChartsScatterPointObj::
CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, const QPointF &p,
                        double size, double color, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), p_(p), size_(size), color_(color), i_(i), n_(n)
{
}

bool
CQChartsScatterPointObj::
inside(const CPoint2D &p) const
{
  double s = plot_->mapSymbolSize(size_);

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CBBox2D pbbox(px - s, py - s, px + s, py + s);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsScatterPointObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  double s = plot_->mapSymbolSize(size_);

  QColor color;

  if (color_ >= 0)
    color = plot_->objectStateColor(this, plot_->interpPaletteColor(color_, Qt::blue));
  else
    color = plot_->objectColor(this, i_, n_, Qt::blue);

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  p->setPen  (plot_->symbolBorderColor());
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
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}
