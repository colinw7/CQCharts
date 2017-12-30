#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsFillObj.h>
#include <CGradientPalette.h>

#include <QPainter>

namespace {

int s_colorId = -1;

int nextColorId() {
  ++s_colorId;

  return s_colorId;
}

}

//------

CQChartsBubblePlotType::
CQChartsBubblePlotType()
{
  addParameters();
}

void
CQChartsBubblePlotType::
addParameters()
{
  addColumnParameter("name" , "Name" , "nameColumn" , "", 0);
  addColumnParameter("value", "Value", "valueColumn", "", 1);
}

CQChartsPlot *
CQChartsBubblePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsBubblePlot(view, model);
}

//------

CQChartsBubblePlot::
CQChartsBubblePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("bubble"), model)
{
  textBoxObj_ = new CQChartsTextBoxObj(this);

  textBoxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  setTextContrast(true);

  textBoxObj_->setTextFontSize(8.0);

  CQChartsPaletteColor textColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  textBoxObj_->setTextColor(textColor);

  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  delete textBoxObj_;

  for (auto &node : nodes_)
    delete node;
}

//------

bool
CQChartsBubblePlot::
isFilled() const
{
  return textBoxObj_->isBackground();
}

void
CQChartsBubblePlot::
setFilled(bool b)
{
  textBoxObj_->setBackground(b);

  update();
}

QString
CQChartsBubblePlot::
fillColorStr() const
{
  return textBoxObj_->backgroundColorStr();
}

void
CQChartsBubblePlot::
setFillColorStr(const QString &s)
{
  textBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsBubblePlot::
interpFillColor(int i, int n) const
{
  return textBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsBubblePlot::
fillAlpha() const
{
  return textBoxObj_->backgroundAlpha();
}

void
CQChartsBubblePlot::
setFillAlpha(double a)
{
  textBoxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsBubblePlot::Pattern
CQChartsBubblePlot::
fillPattern() const
{
  return (Pattern) textBoxObj_->backgroundPattern();
}

void
CQChartsBubblePlot::
setFillPattern(Pattern pattern)
{
  textBoxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
}

//---

bool
CQChartsBubblePlot::
isBorder() const
{
  return textBoxObj_->isBorder();
}

void
CQChartsBubblePlot::
setBorder(bool b)
{
  textBoxObj_->setBorder(b);

  update();
}

QString
CQChartsBubblePlot::
borderColorStr() const
{
  return textBoxObj_->borderColorStr();
}

void
CQChartsBubblePlot::
setBorderColorStr(const QString &str)
{
  textBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsBubblePlot::
interpBorderColor(int i, int n) const
{
  return textBoxObj_->interpBorderColor(i, n);
}

double
CQChartsBubblePlot::
borderAlpha() const
{
  return textBoxObj_->borderAlpha();
}

void
CQChartsBubblePlot::
setBorderAlpha(double a)
{
  textBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsBubblePlot::
borderWidth() const
{
  return textBoxObj_->borderWidth();
}

void
CQChartsBubblePlot::
setBorderWidth(double r)
{
  textBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsBubblePlot::
textFont() const
{
  return textBoxObj_->textFont();
}

void
CQChartsBubblePlot::
setTextFont(const QFont &f)
{
  textBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsBubblePlot::
textColorStr() const
{
  return textBoxObj_->textColorStr();
}

void
CQChartsBubblePlot::
setTextColorStr(const QString &s)
{
  textBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsBubblePlot::
interpTextColor(int i, int n) const
{
  return textBoxObj_->interpTextColor(i, n);
}

bool
CQChartsBubblePlot::
isTextContrast() const
{
  return textBoxObj_->isTextContrast();
}

void
CQChartsBubblePlot::
setTextContrast(bool b)
{
  textBoxObj_->setTextContrast(b);

  update();
}

//---

void
CQChartsBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("stroke", this, "border"     , "visible");
  addProperty("stroke", this, "borderColor", "color"  );
  addProperty("stroke", this, "borderAlpha", "alpha"  );
  addProperty("stroke", this, "borderWidth", "width"  );

  addProperty("fill", this, "filled"     , "visible");
  addProperty("fill", this, "fillColor"  , "color"  );
  addProperty("fill", this, "fillAlpha"  , "alpha"  );
  addProperty("fill", this, "fillPattern", "pattern");

  addProperty("text", this, "textFont"    , "font"    );
  addProperty("text", this, "textColor"   , "color"   );
  addProperty("text", this, "textContrast", "contrast");
}

void
CQChartsBubblePlot::
updateRange(bool apply)
{
  double radius = 1.0;

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

bool
CQChartsBubblePlot::
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

  if (nodes_.empty())
    initNodes();

  //---

  int i = 0;
  int n = nodes_.size();

  for (auto node : nodes_) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CQChartsGeom::BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    CQChartsBubbleObj *obj = new CQChartsBubbleObj(this, node, rect, i, n);

    addPlotObject(obj);

    ++i;
  }

  //---

  return true;
}

void
CQChartsBubblePlot::
initNodes()
{
  loadChildren();

  numColors_ = s_colorId;

  //---

  double xc, yc, r;

  pack_.boundingCircle(xc, yc, r);

  offset_ = CQChartsGeom::Point(xc, yc);
  scale_  = (r > 0.0 ? 1.0/r : 1.0);

  //---

  for (auto node : nodes_) {
    node->setX((node->x() - offset_.x)*scale_);
    node->setY((node->y() - offset_.y)*scale_);

    node->setRadius(node->radius()*scale_);
  }
}

void
CQChartsBubblePlot::
loadChildren(const QModelIndex &index)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  int nr = model->rowCount(index);

  bool hierarchical = false;

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd = model->index(r, nameColumn(), index);

    if (model->rowCount(nameInd) > 0) {
      hierarchical = true;
      break;
    }
  }

  //---

  int colorId = -1;

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd  = model->index(r, nameColumn (), index);
    QModelIndex valueInd = model->index(r, valueColumn(), index);

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    //---

    bool ok1;

    QString name = CQChartsUtil::modelString(model, nameInd, ok1);

    //---

    if (model->rowCount(nameInd) > 0) {
      loadChildren(nameInd);
    }
    else {
      if (hierarchical) {
        if (colorId < 0)
          colorId = nextColorId();
      }
      else {
        colorId = nextColorId();
      }

      //---

      bool ok2;

      double size = CQChartsUtil::modelReal(model, valueInd, ok2);

      if (! ok2 || size <= 0.0)
        continue;

      CQChartsBubbleNode *node =
        new CQChartsBubbleNode(name, size, colorId, nameInd1);

      pack_.addNode(node);

      nodes_.push_back(node);
    }
  }
}

void
CQChartsBubblePlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsBubblePlot::
drawForeground(QPainter *painter)
{
  double xc = 0.0, yc = 0.0, r = 1.0;

  pack_.boundingCircle(xc, yc, r);

  double px1, py1, px2, py2;

  windowToPixel(xc - r, yc + r, px1, py1);
  windowToPixel(xc + r, yc - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // draw bubble
  QColor bc = interpBorderColor(0, 1);

  painter->setPen  (bc);
  painter->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);
}

//------

CQChartsBubbleObj::
CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                  const CQChartsGeom::BBox &rect, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node), i_(i), n_(n)
{
}

QString
CQChartsBubbleObj::
calcId() const
{
  return QString("%1:%2").arg(node_->name()).arg(node_->size());
}

bool
CQChartsBubbleObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}

void
CQChartsBubbleObj::
addSelectIndex()
{
  const QModelIndex &ind = node_->ind();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());
}

bool
CQChartsBubbleObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = node_->ind();

  return (ind == nind);
}

void
CQChartsBubbleObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = plot_->interpFillColor(node_->colorId(), plot_->numColors());

    c.setAlphaF(plot_->fillAlpha());

    brush.setColor(c);
    brush.setStyle(CQChartsFillObj::patternToStyle(
      (CQChartsFillObj::Pattern) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen bpen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    bpen.setColor (bc);
    bpen.setWidthF(plot_->borderWidth());
  }
  else {
    bpen.setStyle(Qt::NoPen);
  }

  QColor tc = plot_->interpTextColor(0, 1);

  QPen tpen(tc);

  plot_->updateObjPenBrushState(this, bpen, brush);
  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  painter->save();

  //---

  // draw bubble
  painter->setPen  (bpen);
  painter->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);

  //---

  // set font size
  QFont font = plot_->textFont();

  //---

  // calc text size and position
  painter->setFont(font);

  const QString &name = node_->name();

  QFontMetricsF fm(painter->font());

  double tw = fm.width(name);

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  //---

  // draw label
  painter->setClipRect(qrect);

  if (plot_->isTextContrast())
    plot_->drawContrastText(painter, px1 - tw/2, py1 + fm.descent(), name, tpen);
  else {
    painter->setPen(tpen);

    painter->drawText(px1 - tw/2, py1 + fm.descent(), name);
  }

  //---

  painter->restore();
}
