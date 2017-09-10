#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsPiePlot::
CQChartsPiePlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  addKey();

  addTitle();
}

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "donut"          );
  addProperty("", this, "innerRadius"    );
  addProperty("", this, "labelRadius"    );
  addProperty("", this, "explodeSelected");
  addProperty("", this, "xColumn"        );
  addProperty("", this, "yColumn"        );
}

void
CQChartsPiePlot::
updateRange()
{
  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  applyDataRange();

  setEqualScale(true);
}

void
CQChartsPiePlot::
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

  double xc = 0.0;
  double yc = 0.0;
  double r  = 0.90;

  setInnerRadius(0.6*r);

  //---

  double angle1 = 90.0;

  int n = model_->rowCount(QModelIndex());

  //---

  double total = 0.0;

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    QModelIndex yind = model_->index(i, yColumn_);

    bool ok;

    double value = CQChartsUtil::modelReal(model_, yind, ok);

    if (! ok)
      value = i;

    if (CQChartsUtil::isNaN(value))
      continue;

    total += value;
  }

  //---

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    QModelIndex xind = model_->index(i, xColumn_);
    QModelIndex yind = model_->index(i, yColumn_);

    bool ok1, ok2;

    QString name  = CQChartsUtil::modelString(model_, xind, ok1);
    double  value = CQChartsUtil::modelReal  (model_, yind, ok2);

    if (! ok2) value = i;

    if (CQChartsUtil::isNaN(value))
      continue;

    double angle  = 360.0*value/total;
    double angle2 = angle1 - angle;

    //---

    CBBox2D rect(xc - r, yc - r, xc + r, yc + r);

    CQChartsPieObj *obj = new CQChartsPieObj(this, rect, i, n);

    obj->setId(QString("%1:%2").arg(name).arg(value));

    obj->setCenter(CPoint2D(xc, yc));
    obj->setRadius(r);
    obj->setAngle1(angle1);
    obj->setAngle2(angle2);
    obj->setName  (name);
    obj->setValue (value);

    addPlotObject(obj);

    //---

    angle1 = angle2;
  }

  //---

  keyObj_->clearItems();

  addKeyItems(keyObj_);
}

void
CQChartsPiePlot::
addKeyItems(CQChartsKey *key)
{
  int n = model_->rowCount(QModelIndex());

  for (int i = 0; i < n; ++i) {
    QModelIndex xind = model_->index(i, xColumn_);

    bool ok;

    QString name = CQChartsUtil::modelString(model_, xind, ok);

    CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, i, n);
    CQChartsKeyText     *text  = new CQChartsPieKeyText (this, i, name);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsPiePlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  drawObjs(p);

  //---

  drawTitle(p);

  drawKey(p);
}

//------

CQChartsPieObj::
CQChartsPieObj(CQChartsPiePlot *plot, const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), i_(i), n_(n)
{
}

bool
CQChartsPieObj::
inside(const CPoint2D &p) const
{
  double r = p.distanceTo(center());

  double ir = 0.0;

  if (plot_->isDonut())
    ir = plot_->innerRadius()*radius();

  if (r < ir || r > radius())
    return false;

  //---

  // check angle
  double a = CAngle::Rad2Deg(atan2(p.y - center().y, p.x - center().x)); while (a < 0) a += 360.0;

  double a1 = angle1(); while (a1 < 0) a1 += 360.0;
  double a2 = angle2(); while (a2 < 0) a2 += 360.0;

  if (a1 < a2) {
    // crosses zero
    if (a >= 0 && a <= a1)
      return true;

    if (a <= 360 && a >= a2)
      return true;
  }
  else {
    if (a >= a2 && a <= a1)
      return true;
  }

  return false;
}

void
CQChartsPieObj::
draw(QPainter *p)
{
  CPoint2D c  = center();
  double   r  = radius();
  double   a1 = angle1();
  double   a2 = angle2();

  bool exploded = isExploded();

  if (isSelected() && plot_->isExplodeSelected())
    exploded = true;

  if (exploded) {
    double angle = CAngle::Deg2Rad((a1 + a2)/2.0);

    double dx = 0.1*radius()*cos(angle);
    double dy = 0.1*radius()*sin(angle);

    c.x += dx;
    c.y += dy;
  }

  //---

  //double ir = plot_->innerRadius()*radius();

  QPainterPath path;

  CPoint2D pc;

  plot_->windowToPixel(c, pc);

  //---

  CBBox2D bbox(c.x - r, c.y - r, c.x + r, c.y + r);

  CBBox2D pbbox;

  plot_->windowToPixel(bbox, pbbox);

  if (plot_->isDonut()) {
    double ir = plot_->innerRadius();

    CBBox2D bbox1(c.x - ir, c.y - ir, c.x + ir, c.y + ir);

    CBBox2D pbbox1;

    plot_->windowToPixel(bbox1, pbbox1);

    //---

    double ra1 = a1*M_PI/180.0;
    double ra2 = a2*M_PI/180.0;

    double x1 = c.x + ir*cos(ra1);
    double y1 = c.y + ir*sin(ra1);
    double x2 = c.x +  r*cos(ra1);
    double y2 = c.y +  r*sin(ra1);

    double x3 = c.x + ir*cos(ra2);
    double y3 = c.y + ir*sin(ra2);
    double x4 = c.x +  r*cos(ra2);
    double y4 = c.y +  r*sin(ra2);

    double px1, py1, px2, py2, px3, py3, px4, py4;

    plot_->windowToPixel(x1, y1, px1, py1);
    plot_->windowToPixel(x2, y2, px2, py2);
    plot_->windowToPixel(x3, y3, px3, py3);
    plot_->windowToPixel(x4, y4, px4, py4);

    path.moveTo(px1, py1);
    path.lineTo(px2, py2);

    path.arcTo(CQUtil::toQRect(pbbox), a1, a2 - a1);

    path.lineTo(px4, py4);
    path.lineTo(px3, py3);

    path.arcTo(CQUtil::toQRect(pbbox1), a2, a1 - a2);
  }
  else {
    double a21 = a2 - a1;

    if (std::abs(a21) < 360.0) {
      path.moveTo(QPointF(pc.x, pc.y));

      path.arcTo(CQUtil::toQRect(pbbox), a1, a2 - a1);
    }
    else {
      path.addEllipse(CQUtil::toQRect(pbbox));
    }
  }

  //fillPieSlice  (c, ir, radius(), a1, a2, *fill  );
  //strokePieSlice(c, ir, radius(), a1, a2, isWedge(), *stroke);

  path.closeSubpath();

  //---

  QColor bg = plot_->objectColor(this, i_, n_);
  QColor fg = plot_->textColor(bg);

  p->setBrush(bg);
  p->setPen  (fg);

  p->drawPath(path);

  //---

  if (name() != "") {
    double a21 = a2 - a1;

    if (std::abs(a21) < 360.0) {
      double ta = (a1 + a2)/2.0;

      double tangle = CAngle::Deg2Rad(ta);

      double lr;

      if (plot_->isDonut())
        lr = plot_->innerRadius() + plot_->labelRadius()*(radius() - plot_->innerRadius());
      else
        lr = plot_->labelRadius()*radius();

      if (lr < 0.01)
        lr = 0.01;

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = c.x + lr*tc;
      double ty = c.y + lr*ts;

      double ptx, pty;

      plot_->windowToPixel(tx, ty, ptx, pty);

      //---

      QFontMetrics fm(plot_->view()->font());

      int tw = fm.width(name());

      p->setPen(fg);

      p->drawText(ptx - tw/2, pty + fm.ascent(), name());

#if 0
      // aligned ?
      CPoint2D tp(tx, ty);

      QColor tc1;

      if (fill->type() == CGnuPlotTypes::FillType::SOLID)
        tc1 = fill->color().bwContrast();

      if (isRotatedText()) {
        if (tc >= 0)
          p->drawRotatedText(tp, name(), ta, HAlignPos(CHALIGN_TYPE_LEFT, 0),
                             VAlignPos(CVALIGN_TYPE_CENTER, 0), tc1);
        else
          p->drawRotatedText(tp, name(), 180.0 + ta, HAlignPos(CHALIGN_TYPE_RIGHT, 0),
                             VAlignPos(CVALIGN_TYPE_CENTER, 0), tc1);
      }
      else {
        p->drawHAlignedText(tp, HAlignPos(CHALIGN_TYPE_CENTER, 0),
                            VAlignPos(CVALIGN_TYPE_CENTER, 0), name(), tc1);
      }
#endif
    }
    else {
      QFontMetrics fm(plot_->view()->font());

      int tw = fm.width(name());

      p->setPen(fg);

      p->drawText(pc.x - tw/2, pc.y + (fm.ascent() - fm.descent())/2, name());
    }
  }

  //---

  //CPoint2D pr(radius(), radius());

  //bbox_ = CBBox2D(c - pr, c + pr);
}

//------

CQChartsPieKeyColor::
CQChartsPieKeyColor(CQChartsPiePlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsPieKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

QColor
CQChartsPieKeyColor::
fillColor() const
{
  QColor c = CQChartsKeyColorBox::fillColor();

  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQUtil::blendColors(c, Qt::white, 0.5);

  return c;
}

//------

CQChartsPieKeyText::
CQChartsPieKeyText(CQChartsPiePlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsPieKeyText::
textColor() const
{
  QColor c = CQChartsKeyText::textColor();

  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQUtil::blendColors(c, Qt::white, 0.5);

  return c;
}
