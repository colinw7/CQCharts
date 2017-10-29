#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsPiePlotType::
CQChartsPiePlotType()
{
  addParameters();
}

void
CQChartsPiePlotType::
addParameters()
{
  // name, desc, propName, attributes, default
  addColumnParameter("label"   , "Label"    , "labelColumn"   , "", 0);
  addColumnParameter("data"    , "Data"     , "dataColumn"    , "", 1);
  addColumnParameter("keyLabel", "Key Label", "keyLabelColumn", "optional");
  addColumnParameter("color"   , "Color"    , "colorColumn"   , "optional");

  CQChartsPlotType::addParameters();
}

CQChartsPlot *
CQChartsPiePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsPiePlot(view, model);
}

//---

CQChartsPiePlot::
CQChartsPiePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("pie"), model), textBox_(this)
{
  setLayerActive(Layer::FG, true);

  addKey();

  addTitle();
}

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "labelColumn"   , "label"   );
  addProperty("columns", this, "dataColumn"    , "data"    );
  addProperty("columns", this, "keyLabelColumn", "keyLabel");
  addProperty("columns", this, "colorColumn"   , "color"   );

  addProperty("", this, "donut"          );
  addProperty("", this, "innerRadius"    );
  addProperty("", this, "labelRadius"    );
  addProperty("", this, "explodeSelected");
  addProperty("", this, "startAngle"     );
  addProperty("", this, "rotatedText"    );

  addProperty("label", &textBox_, "visible");
  addProperty("label", &textBox_, "font"   );
  addProperty("label", &textBox_, "color"  );

  QString labelBoxPath = id() + "/label/box";

  textBox_.CQChartsBoxObj::addProperties(propertyView(), labelBoxPath);

  addProperty("color", this, "colorMapEnabled", "mapEnabled" );
  addProperty("color", this, "colorMapMin"    , "mapMin"     );
  addProperty("color", this, "colorMapMax"    , "mapMax"     );
}

void
CQChartsPiePlot::
updateRange(bool apply)
{
  double r = 1.0;

  r = std::max(r, labelRadius());

  double xr = r;
  double yr = r;

  if (isEqualScale()) {
    double aspect = view()->aspect();

    if (aspect > 1.0)
      xr *= aspect;
    else
      yr *= 1.0/aspect;
  }

  dataRange_.reset();

  dataRange_.updateRange(-xr, -yr);
  dataRange_.updateRange( xr,  yr);

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsPiePlot::
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

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  double xc = 0.0;
  double yc = 0.0;
  double r  = 0.90;

  setInnerRadius(0.6*r);

  //---

  double angle1 = startAngle();

  int n = model->rowCount(QModelIndex());

  //---

  double total = 0.0;

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    bool ok;

    double value = CQChartsUtil::modelReal(model, i, dataColumn(), ok);

    if (! ok)
      value = i;

    if (CQChartsUtil::isNaN(value))
      continue;

    total += value;
  }

  //---

  // init value sets
  if (colorSet_.empty()) {
    if (colorColumn() >= 0) {
      bool ok;

      for (int i = 0; i < n; ++i)
        colorSet_.addValue(CQChartsUtil::modelValue(model, i, colorColumn(), ok));
    }
  }

  //---

  for (int i = 0; i < n; ++i) {
    bool hidden = isSetHidden(i);

    if (hidden)
      continue;

    //---

    bool ok1, ok2;

    QString label = CQChartsUtil::modelString(model, i, labelColumn(), ok1);
    double  value = CQChartsUtil::modelReal  (model, i, dataColumn (), ok2);

    if (! ok2) value = i;

    if (CQChartsUtil::isNaN(value))
      continue;

    double angle  = 360.0*value/total;
    double angle2 = angle1 - angle;

    //---

    // get color label (?)

    OptReal color;

    if (colorColumn() >= 0)
      color = colorSet_.imap(i);

    //---

    CBBox2D rect(xc - r, yc - r, xc + r, yc + r);

    CQChartsPieObj *obj = new CQChartsPieObj(this, rect, i, n);

    obj->setId(QString("%1:%2").arg(label).arg(columnStr(dataColumn(), value)));

    obj->setAngle1(angle1);
    obj->setAngle2(angle2);
    obj->setLabel (label);
    obj->setValue (value);

    if (color)
      obj->setColor(*color);

    addPlotObject(obj);

    //---

    angle1 = angle2;
  }

  //---

  resetKeyItems();
}

void
CQChartsPiePlot::
addKeyItems(CQChartsKey *key)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int labelColumn = keyLabelColumn();

  if (labelColumn < 0)
    labelColumn = this->labelColumn();

  int n = model->rowCount(QModelIndex());

  for (int i = 0; i < n; ++i) {
    bool ok;

    QString label = CQChartsUtil::modelString(model, i, labelColumn, ok);

    CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, i, n);
    CQChartsPieKeyText  *text  = new CQChartsPieKeyText (this, i, label);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsPiePlot::
handleResize()
{
  dataRange_.reset();

  CQChartsPlot::handleResize();
}

void
CQChartsPiePlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
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
  CPoint2D center(0, 0);

  double r = p.distanceTo(center);

  double ro = plot_->outerRadius();
  double ri = 0.0;

  if (plot_->isDonut())
    ri = plot_->innerRadius()*ro;

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CQChartsUtil::Rad2Deg(atan2(p.y - center.y, p.x - center.x));
  a = CQChartsUtil::normalizeAngle(a);

  double a1 = angle1(); a1 = CQChartsUtil::normalizeAngle(a1);
  double a2 = angle2(); a2 = CQChartsUtil::normalizeAngle(a2);

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
draw(QPainter *p, const CQChartsPlot::Layer &layer)
{
  CPoint2D center(0, 0);

  CPoint2D c  = center;
  double   ro = plot_->outerRadius();
  double   a1 = angle1();
  double   a2 = angle2();

  //---

  bool exploded = isExploded();

  if (isSelected() && plot_->isExplodeSelected())
    exploded = true;

  if (exploded) {
    double angle = CQChartsUtil::Deg2Rad(CQChartsUtil::avg(a1, a2));

    double dx = 0.1*ro*cos(angle);
    double dy = 0.1*ro*sin(angle);

    c.x += dx;
    c.y += dy;
  }

  //---

  //double ri = plot_->innerRadius()*ro;

  CPoint2D pc;

  plot_->windowToPixel(c, pc);

  //---

  CBBox2D bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  CBBox2D pbbox;

  plot_->windowToPixel(bbox, pbbox);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    QPainterPath path;

    if (plot_->isDonut()) {
      double ri = plot_->innerRadius();

      CBBox2D bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

      CBBox2D pbbox1;

      plot_->windowToPixel(bbox1, pbbox1);

      //---

      double ra1 = a1*M_PI/180.0;
      double ra2 = a2*M_PI/180.0;

      double x1 = c.x + ri*cos(ra1);
      double y1 = c.y + ri*sin(ra1);
      double x2 = c.x + ro*cos(ra1);
      double y2 = c.y + ro*sin(ra1);

      double x3 = c.x + ri*cos(ra2);
      double y3 = c.y + ri*sin(ra2);
      double x4 = c.x + ro*cos(ra2);
      double y4 = c.y + ro*sin(ra2);

      double px1, py1, px2, py2, px3, py3, px4, py4;

      plot_->windowToPixel(x1, y1, px1, py1);
      plot_->windowToPixel(x2, y2, px2, py2);
      plot_->windowToPixel(x3, y3, px3, py3);
      plot_->windowToPixel(x4, y4, px4, py4);

      path.moveTo(px1, py1);
      path.lineTo(px2, py2);

      path.arcTo(CQChartsUtil::toQRect(pbbox), a1, a2 - a1);

      path.lineTo(px4, py4);
      path.lineTo(px3, py3);

      path.arcTo(CQChartsUtil::toQRect(pbbox1), a2, a1 - a2);
    }
    else {
      double a21 = a2 - a1;

      if (std::abs(a21) < 360.0) {
        path.moveTo(QPointF(pc.x, pc.y));

        path.arcTo(CQChartsUtil::toQRect(pbbox), a1, a2 - a1);
      }
      else {
        path.addEllipse(CQChartsUtil::toQRect(pbbox));
      }
    }

    path.closeSubpath();

    //---

    QColor bg;

    if (color_)
      bg = plot_->interpPaletteColor(*color_);
    else
      bg = plot_->objectColor(this, i_, n_);

    QColor fg = plot_->textColor(bg);

    p->setBrush(bg);
    p->setPen  (fg);

    p->drawPath(path);
  }

  //---

  if (layer == CQChartsPlot::Layer::FG && label() != "") {
    double a21 = a2 - a1;

    // if full circle always draw text at center
    if (CQChartsUtil::realEq(std::abs(a21), 360.0)) {
      plot_->textBox().draw(p, CQChartsUtil::toQPoint(pc), label(), 0.0);
    }
    // draw on arc center line
    else {
      double ri = plot_->innerRadius();
      double lr = plot_->labelRadius();

      double ta = CQChartsUtil::avg(a1, a2);

      double tangle = CQChartsUtil::Deg2Rad(ta);

      double lr1;

      if (plot_->isDonut())
        lr1 = ri + lr*(ro - ri);
      else
        lr1 = lr*ro;

      if (lr1 < 0.01)
        lr1 = 0.01;

      double tc = cos(tangle);
      double ts = sin(tangle);

      double tx = c.x + lr1*tc;
      double ty = c.y + lr1*ts;

      double ptx, pty;

      plot_->windowToPixel(tx, ty, ptx, pty);

      //---

      double        dx    = 0.0;
      Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

      if (lr1 > ro) {
        double lx1 = c.x + ro*tc;
        double ly1 = c.y + ro*ts;
        double lx2 = c.x + lr1*tc;
        double ly2 = c.y + lr1*ts;

        double lpx1, lpy1, lpx2, lpy2;

        plot_->windowToPixel(lx1, ly1, lpx1, lpy1);
        plot_->windowToPixel(lx2, ly2, lpx2, lpy2);

        int tickSize = 16;

        if (tc >= 0) {
          dx    = tickSize;
          align = Qt::AlignLeft | Qt::AlignVCenter;
        }
        else {
          dx    = -tickSize;
          align = Qt::AlignRight | Qt::AlignVCenter;
        }

        QColor bg = plot_->objectColor(this, i_, n_);

        p->setPen(bg);

        p->drawLine(lpx1, lpy1, lpx2     , lpy2);
        p->drawLine(lpx2, lpy2, lpx2 + dx, lpy2);
      }

      //---

      QPointF pt = QPointF(ptx + dx, pty);

      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      plot_->textBox().draw(p, pt, label(), angle, align);

      CBBox2D tbbox;

      plot_->pixelToWindow(CQChartsUtil::fromQRect(plot_->textBox().rect()), tbbox);
    }
  }
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

  plot->updateObjs();

  return true;
}

QBrush
CQChartsPieKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

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
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}

//------

CQChartsPieTextObj::
CQChartsPieTextObj(CQChartsPiePlot *plot) :
 plot_(plot)
{
}

void
CQChartsPieTextObj::
redrawBoxObj()
{
  plot_->update();
}
