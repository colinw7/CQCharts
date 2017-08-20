#include <CQChartsPiePlot.h>
#include <CQChartsWindow.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsPiePlot::
CQChartsPiePlot(CQChartsWindow *window, QAbstractItemModel *model) :
 CQChartsPlot(window, model)
{
  addKey();

  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  displayRange_.setEqualScale(true);
}

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty(id_, this, "donut");
}

void
CQChartsPiePlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  double xc = 0.5;
  double yc = 0.5;
  double r  = 0.45;

  //---

  double angle1 = 90.0;

  QModelIndex ind;

  int n = model_->rowCount(ind);

  //---

  double total = 0.0;

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    QModelIndex yind = model_->index(i, yColumn_);

    double value = CQChartsUtil::toReal(model_->data(yind));

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

    QString name  = model_->data(xind).toString();
    double  value = CQChartsUtil::toReal(model_->data(yind));

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

    obj->setInnerRadius(0.6*r);

    obj->setLabelRadius(0.5);

    addPlotObject(obj);

    //---

    angle1 = angle2;
  }

  //---

  key_->clearItems();

  for (int i = 0; i < n; ++i) {
    QModelIndex xind = model_->index(i, xColumn_);

    QString name = model_->data(xind).toString();

    CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, i, n);
    CQChartsKeyText     *text  = new CQChartsKeyText    (this, name);

    key_->addItem(color, i, 0);
    key_->addItem(text , i, 1);
  }
}

void
CQChartsPiePlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(p);

  //---

  drawKey(p);
}

QColor
CQChartsPiePlot::
segmentColor(int i, int n) const
{
  static QColor colors[] = {
    QColor(0x98,0xAB,0xC5),
    QColor(0x8A,0x89,0xA6),
    QColor(0x7B,0x68,0x88),
    QColor(0x6B,0x48,0x6B),
    QColor(0xA0,0x5D,0x56),
    QColor(0xD0,0x74,0x3C),
    QColor(0xFF,0x8C,0x00),
  };

  static uint num_colors = 7;

  //---

  return paletteColor(i, n, colors[i % num_colors]);
}

QColor
CQChartsPiePlot::
textColor(const QColor &bg) const
{
  int g = qGray(bg.red(), bg.green(), bg.blue());

  return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
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

  double ir = innerRadius()*radius();

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

  //if (isSelected() && isExplodeSelected())
  //  exploded = true;

  if (exploded) {
    double angle = CAngle::Deg2Rad((a1 + a2)/2.0);

    double dx = 0.1*radius()*cos(angle);
    double dy = 0.1*radius()*sin(angle);

    c.x += dx;
    c.y += dy;
  }

  //---

  //double ir = innerRadius()*radius();

  QPainterPath path;

  CPoint2D pc;

  plot_->windowToPixel(c, pc);

  //---

  CBBox2D bbox(c.x - r, c.y - r, c.x + r, c.y + r);

  CBBox2D pbbox;

  plot_->windowToPixel(bbox, pbbox);

  if (plot_->isDonut()) {
    double ir = innerRadius();

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
    path.moveTo(QPointF(pc.x, pc.y));

    path.arcTo(CQUtil::toQRect(pbbox), a1, a2 - a1);
  }

  //fillPieSlice  (c, ir, radius(), a1, a2, *fill  );
  //strokePieSlice(c, ir, radius(), a1, a2, isWedge(), *stroke);

  path.closeSubpath();

  //---

  QColor bg = plot_->objectColor(this, i_, n_, plot_->segmentColor(i_, n_));
  QColor fg = plot_->textColor(bg);

  p->setBrush(bg);
  p->setPen  (fg);

  p->drawPath(path);

  //---

  if (name() != "") {
    double ta = (a1 + a2)/2.0;

    double tangle = CAngle::Deg2Rad(ta);

    double lr;

    if (plot_->isDonut())
      lr = innerRadius() + labelRadius()*(radius() - innerRadius());
    else
      lr = labelRadius()*radius();

    if (lr < 0.01)
      lr = 0.01;

    double tc = cos(tangle);
    double ts = sin(tangle);

    double tx = c.x + lr*tc;
    double ty = c.y + lr*ts;

    double ptx, pty;

    plot_->windowToPixel(tx, ty, ptx, pty);

    //---

    QFontMetrics fm(plot_->window()->font());

    int tw = fm.width(name());

    p->setPen(fg);

    p->drawText(ptx - tw/2, pty + fm.ascent(), name());

#if 0
    // aligned ?
    CPoint2D tp(tx, ty);

    CRGBA tc1(0,0,0);

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
    else
      p->drawHAlignedText(tp, HAlignPos(CHALIGN_TYPE_CENTER, 0),
                          VAlignPos(CVALIGN_TYPE_CENTER, 0), name(), tc1);
#endif
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

void
CQChartsPieKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();
}
