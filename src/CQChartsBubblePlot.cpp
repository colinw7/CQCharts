#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
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

//---

CQChartsBubblePlot::
CQChartsBubblePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("bubble"), model)
{
  setMargins(1, 1, 1, 1);

  addTitle();
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  for (auto &node : nodes_)
    delete node;
}

void
CQChartsBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "fontHeight");
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

void
CQChartsBubblePlot::
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

    obj->setId(QString("%1:%2").arg(node->name().c_str()).arg(node->size()));

    addPlotObject(obj);

    ++i;
  }
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

  int colorId = -1;

  int nr = model->rowCount(index);

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
      if (colorId < 0)
        colorId = nextColorId();

      bool ok2;

      int size = CQChartsUtil::modelInteger(model, valueInd, ok2);

      if (! ok2) size = 1;

      CQChartsBubbleNode *node =
        new CQChartsBubbleNode(name.toStdString(), size, colorId, nameInd1);

      pack_.addNode(node);

      nodes_.push_back(node);
    }
  }
}

void
CQChartsBubblePlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsBubblePlot::
drawForeground(QPainter *p)
{
  double xc = 0.0, yc = 0.0, r = 1.0;

  pack_.boundingCircle(xc, yc, r);

  double px1, py1, px2, py2;

  windowToPixel(xc - r, yc + r, px1, py1);
  windowToPixel(xc + r, yc - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // draw bubble
  p->setPen  (QColor(0,0,0));
  p->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  p->drawPath(path);
}

QColor
CQChartsBubblePlot::
nodeColor(CQChartsBubbleNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/numColors_, c);
}

//------

CQChartsBubbleObj::
CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                  const CQChartsGeom::BBox &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), i_(i), n_(n)
{
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
mousePress(const CQChartsGeom::Point &)
{
  const QModelIndex &ind = node_->ind();

  plot_->beginSelect();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());

  plot_->endSelect();
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
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  QColor c = plot_->nodeColor(node_);

  QBrush brush(c);

  QColor bc = Qt::black;
  QColor tc = plot_->textColor(c);

  QPen bpen(bc);
  QPen tpen(tc);

  plot_->updateObjPenBrushState(this, bpen, brush);
  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  p->save();

  //---

  // draw bubble
  p->setPen  (bpen);
  p->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  p->drawPath(path);

  //---

  // set font size
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  //---

  // calc text size and position
  p->setFont(font);

  QString name = node_->name().c_str();

  QFontMetricsF fm(p->font());

  double tw = fm.width(name);

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  //---

  // draw label
  p->setClipRect(qrect, Qt::ReplaceClip);

  plot_->drawContrastText(p, px1 - tw/2, py1 + fm.descent(), name, tpen);

  //---

  p->restore();
}
