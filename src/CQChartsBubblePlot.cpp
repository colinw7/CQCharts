#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRenderer.h>
#include <CQChartsBoxObj.h>
#include <CGradientPalette.h>

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
  bubbleObj_ = new CQChartsBoxObj(this);

  bubbleObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setBorder(true);

  textFont_.setPointSizeF(8.0);

  textColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  delete bubbleObj_;

  for (auto &node : nodes_)
    delete node;
}

QString
CQChartsBubblePlot::
fillColorStr() const
{
  return bubbleObj_->backgroundColorStr();
}

void
CQChartsBubblePlot::
setFillColorStr(const QString &s)
{
  bubbleObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsBubblePlot::
interpFillColor(int i, int n) const
{
  return bubbleObj_->interpBackgroundColor(i, n);
}

double
CQChartsBubblePlot::
fillAlpha() const
{
  return bubbleObj_->backgroundAlpha();
}

void
CQChartsBubblePlot::
setFillAlpha(double a)
{
  bubbleObj_->setBackgroundAlpha(a);

  update();
}

bool
CQChartsBubblePlot::
isBorder() const
{
  return bubbleObj_->isBorder();
}

void
CQChartsBubblePlot::
setBorder(bool b)
{
  bubbleObj_->setBorder(b);

  update();
}

QString
CQChartsBubblePlot::
borderColorStr() const
{
  return bubbleObj_->borderColorStr();
}

void
CQChartsBubblePlot::
setBorderColorStr(const QString &str)
{
  bubbleObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsBubblePlot::
interpBorderColor(int i, int n) const
{
  return bubbleObj_->interpBorderColor(i, n);
}

double
CQChartsBubblePlot::
borderAlpha() const
{
  return bubbleObj_->borderAlpha();
}

void
CQChartsBubblePlot::
setBorderAlpha(double a)
{
  bubbleObj_->setBorderAlpha(a);

  update();
}

double
CQChartsBubblePlot::
borderWidth() const
{
  return bubbleObj_->borderWidth();
}

void
CQChartsBubblePlot::
setBorderWidth(double r)
{
  bubbleObj_->setBorderWidth(r);

  update();
}

QColor
CQChartsBubblePlot::
interpTextColor(int i, int n) const
{
  return textColor_.interpColor(this, i, n);
}

void
CQChartsBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("fill"  , this, "fillColor"  , "color"    );
  addProperty("fill"  , this, "fillAlpha"  , "alpha"    );
  addProperty("border", this, "border"     , "displayed");
  addProperty("border", this, "borderColor", "color"    );
  addProperty("border", this, "borderAlpha", "alpha"    );
  addProperty("border", this, "borderWidth", "width"    );
  addProperty("text"  , this, "textFont"   , "font"     );
  addProperty("text"  , this, "textColor"  , "color"    );
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
draw(CQChartsRenderer *renderer)
{
  initPlotObjs();

  //---

  drawParts(renderer);
}

void
CQChartsBubblePlot::
drawForeground(CQChartsRenderer *renderer)
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

  renderer->setPen  (bc);
  renderer->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  renderer->drawPath(path);
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
draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &)
{
  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // calc stroke and brush

  QColor c = plot_->interpFillColor(node_->colorId(), plot_->numColors());

  c.setAlphaF(plot_->fillAlpha());

  QBrush brush(c);

  QPen bpen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    bpen = QPen(bc);

    bpen.setWidthF(plot_->borderWidth());
  }
  else
    bpen = QPen(Qt::NoPen);

  QColor tc = plot_->interpTextColor(0, 1);

  QPen tpen(tc);

  plot_->updateObjPenBrushState(this, bpen, brush);
  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  renderer->save();

  //---

  // draw bubble
  renderer->setPen  (bpen);
  renderer->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  renderer->drawPath(path);

  //---

  // set font size
  QFont font = plot_->textFont();

  //---

  // calc text size and position
  renderer->setFont(font);

  const QString &name = node_->name();

  QFontMetricsF fm(renderer->font());

  double tw = fm.width(name);

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  //---

  // draw label
  renderer->setClipRect(qrect);

  plot_->drawContrastText(renderer, px1 - tw/2, py1 + fm.descent(), name, tpen);

  //---

  renderer->restore();
}
