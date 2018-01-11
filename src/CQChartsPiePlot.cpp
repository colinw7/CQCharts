#include <CQChartsPiePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
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
 CQChartsPlot(view, view->charts()->plotType("pie"), model)
{
  textBox_ = new CQChartsPieTextObj(this);

  setLayerActive(Layer::FG, true);

  addKey();

  addTitle();
}

CQChartsPiePlot::
~CQChartsPiePlot()
{
  delete textBox_;
}

void
CQChartsPiePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "labelColumn"   , "label"   );
  addProperty("columns", this, "dataColumn"    , "data"    );
  addProperty("columns", this, "keyLabelColumn", "keyLabel");
  addProperty("columns", this, "colorColumn"   , "color"   );

  // general
  addProperty("", this, "donut"          );
  addProperty("", this, "innerRadius"    );
  addProperty("", this, "labelRadius"    );
  addProperty("", this, "explodeSelected");
  addProperty("", this, "explodeRadius"  );
  addProperty("", this, "startAngle"     );
  addProperty("", this, "rotatedText"    );

  // label
  addProperty("label", textBox_, "textVisible", "visible");
  addProperty("label", textBox_, "textFont"   , "font"   );
  addProperty("label", textBox_, "textColor"  , "color"  );
  addProperty("label", textBox_, "textAlpha"  , "alpha"  );

  QString labelBoxPath = id() + "/label/box";

  textBox_->CQChartsBoxObj::addProperties(propertyModel(), labelBoxPath);

  // colormap
  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

void
CQChartsPiePlot::
updateRange(bool apply)
{
  double radius = 1.0;

  radius = std::max(radius, labelRadius());

  double xr = radius;
  double yr = radius;

  if (isEqualScale()) {
    double aspect = this->aspect();

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
CQChartsPiePlot::
colorSetColor(int i, OptColor &color)
{
  return colorSet_.icolor(i, color);
}

//------

void
CQChartsPiePlot::
updateObjs()
{
  colorSet_.clear();

  CQChartsPlot::updateObjs();
}

bool
CQChartsPiePlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  double xc     = 0.0;
  double yc     = 0.0;
  double radius = outerRadius();

  setInnerRadius1(innerRadius()*radius);

  //---

  double angle1 = startAngle();

  int nr = model->rowCount(QModelIndex());

  //---

  double total = 0.0;

  for (int r = 0; r < nr; ++r) {
    bool hidden = isSetHidden(r);

    if (hidden)
      continue;

    //---

    QModelIndex dataInd = model->index(r, dataColumn());

    bool ok;

    double value = CQChartsUtil::modelReal(model, dataInd, ok);

    if (! ok)
      value = r;

    if (CQChartsUtil::isNaN(value))
      continue;

    total += value;
  }

  //---

  // init value sets
  if (colorSet_.empty())
    initColorSet();

  //---

  for (int r = 0; r < nr; ++r) {
    bool hidden = isSetHidden(r);

    if (hidden)
      continue;

    //---

    QModelIndex labelInd = model->index(r, labelColumn());
    QModelIndex dataInd  = model->index(r, dataColumn ());

    QModelIndex dataInd1 = normalizeIndex(dataInd);

    //---

    bool ok1, ok2;

    QString label = CQChartsUtil::modelString(model, labelInd, ok1);
    double  value = CQChartsUtil::modelReal  (model, dataInd , ok2);

    if (! ok2) value = r;

    if (CQChartsUtil::isNaN(value))
      continue;

    double angle  = 360.0*value/total;
    double angle2 = angle1 - angle;

    //---

    CQChartsGeom::BBox rect(xc - radius, yc - radius, xc + radius, yc + radius);

    CQChartsPieObj *obj = new CQChartsPieObj(this, rect, dataInd1, r, nr);

    obj->setAngle1(angle1);
    obj->setAngle2(angle2);
    obj->setLabel (label);
    obj->setValue (value);

    OptColor color;

    if (colorSetColor(r, color))
      obj->setColor(*color);

    addPlotObject(obj);

    //---

    angle1 = angle2;
  }

  //---

  resetKeyItems();

  //---

  return true;
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

  int nr = model->rowCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    QModelIndex labelInd = model->index(r, labelColumn);

    //---

    bool ok;

    QString label = CQChartsUtil::modelString(model, labelInd, ok);

    CQChartsPieKeyColor *color = new CQChartsPieKeyColor(this, r, nr);
    CQChartsPieKeyText  *text  = new CQChartsPieKeyText (this, r, label);

    key->addItem(color, r, 0);
    key->addItem(text , r, 1);
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
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsPieObj::
CQChartsPieObj(CQChartsPiePlot *plot, const CQChartsGeom::BBox &rect, const QModelIndex &ind,
               int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsPieObj::
calcId() const
{
  QModelIndex labelInd = plot_->model()->index(i_, plot_->labelColumn());

  bool ok;

  QString label = CQChartsUtil::modelString(plot_->model(), labelInd, ok);

  return QString("%1:%2").arg(label).arg(plot_->columnStr(plot_->dataColumn(), value_));
}

bool
CQChartsPieObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::Point center(0, 0);

  double r = p.distanceTo(center);

  double ro = plot_->outerRadius();
  double ri = 0.0;

  if (plot_->isDonut())
    ri = plot_->innerRadius1()*ro;

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
addSelectIndex()
{
  plot_->addSelectIndex(ind_.row(), plot_->labelColumn(), ind_.parent());
  plot_->addSelectIndex(ind_.row(), plot_->dataColumn (), ind_.parent());
}

bool
CQChartsPieObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsPieObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  CQChartsGeom::Point center(0, 0);

  CQChartsGeom::Point c  = center;
  double              ro = plot_->outerRadius();
  double              a1 = angle1();
  double              a2 = angle2();

  //---

  bool exploded = isExploded();

  if (isSelected() && plot_->isExplodeSelected())
    exploded = true;

  if (exploded) {
    double angle = CQChartsUtil::Deg2Rad(CQChartsUtil::avg(a1, a2));

    double dx = plot_->explodeRadius()*ro*cos(angle);
    double dy = plot_->explodeRadius()*ro*sin(angle);

    c.x += dx;
    c.y += dy;
  }

  //---

  //double ri = plot_->innerRadius1()*ro;

  CQChartsGeom::Point pc;

  plot_->windowToPixel(c, pc);

  //---

  CQChartsGeom::BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  CQChartsGeom::BBox pbbox;

  plot_->windowToPixel(bbox, pbbox);

  //---

  if (layer == CQChartsPlot::Layer::MID) {
    QPainterPath path;

    if (plot_->isDonut()) {
      double ri = plot_->innerRadius1();

      CQChartsGeom::BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

      CQChartsGeom::BBox pbbox1;

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
      bg = (*color_).interpColor(plot_, i_, n_);
    else
      bg = plot_->interpPaletteColor(i_, n_);

    QColor fg = plot_->textColor(bg);

    QPen   pen  (fg);
    QBrush brush(bg);

    plot_->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);
  }

  //---

  if (layer == CQChartsPlot::Layer::FG && label() != "") {
    double a21 = a2 - a1;

    // if full circle always draw text at center
    if (CQChartsUtil::realEq(std::abs(a21), 360.0)) {
      plot_->textBox()->draw(painter, CQChartsUtil::toQPoint(pc), label(), 0.0);
    }
    // draw on arc center line
    else {
      double ri = plot_->innerRadius1();
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

        QColor bg = plot_->interpPaletteColor(i_, n_);

        painter->setPen(bg);

        painter->drawLine(QPointF(lpx1, lpy1), QPointF(lpx2     , lpy2));
        painter->drawLine(QPointF(lpx2, lpy2), QPointF(lpx2 + dx, lpy2));
      }

      //---

      QPointF pt = QPointF(ptx + dx, pty);

      double angle = 0.0;

      if (plot_->isRotatedText())
        angle = (tc >= 0 ? ta : 180.0 + ta);

      plot_->textBox()->draw(painter, pt, label(), angle, align);

      CQChartsGeom::BBox tbbox;

      plot_->pixelToWindow(CQChartsUtil::fromQRect(plot_->textBox()->rect()), tbbox);
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
mousePress(const CQChartsGeom::Point &)
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
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

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
interpTextColor(int i, int n) const
{
  CQChartsPiePlot *plot = qobject_cast<CQChartsPiePlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}

//------

CQChartsPieTextObj::
CQChartsPieTextObj(CQChartsPiePlot *plot) :
 CQChartsRotatedTextBoxObj(plot), plot_(plot)
{
}
