#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>

namespace {

int numColors = 20;

int colorId = -1;

int nextColorId() {
  ++colorId;

  if (colorId >= numColors)
    colorId = 0;

  return colorId;
}

}

//------

CQChartsBubblePlotType::
CQChartsBubblePlotType()
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

void
CQChartsBubblePlot::
updateRange(bool apply)
{
  dataRange_.reset();

  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  if (apply)
    applyDataRange();
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

    double r = node->radius();

    //---

    CBBox2D rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

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

  //---

  double xc, yc, r;

  pack_.boundingCircle(xc, yc, r);

  offset_ = CPoint2D(xc, yc);
  scale_  = 1.0/r;

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
  double xc, yc, r;

  pack_.boundingCircle(xc, yc, r);

  double px1, py1, px2, py2;

  windowToPixel(xc - r, yc + r, px1, py1);
  windowToPixel(xc + r, yc - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen(QColor(0,0,0));
  p->setBrush(QColor(0,0,0,0));

  p->drawPath(path);
}

QColor
CQChartsBubblePlot::
nodeColor(CQChartsBubbleNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/numColors, c);
}

//------

CQChartsBubbleObj::
CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                  const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), i_(i), n_(n)
{
}

void
CQChartsBubbleObj::
mousePress(const CPoint2D &)
{
  plot_->beginSelect();

  const QModelIndex &ind = node_->ind();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());

  plot_->endSelect();
}

void
CQChartsBubbleObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  p->setFont(font);

  //---

  QFontMetricsF fm(p->font());

  QColor c = plot_->objectStateColor(this, plot_->nodeColor(node_));

  QColor tc = plot_->textColor(c);

  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen  (tc);
  p->setBrush(c);

  p->drawPath(path);

  //---

  p->setPen(tc);

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  int len = node_->name().size();

  for (int i = len; i >= 1; --i) {
    std::string name1 = node_->name().substr(0, i);

    double tw = fm.width(name1.c_str());

    if (tw > 2*(px2 - px1)) continue;

    p->drawText(px1 - tw/2, py1 + fm.descent(), name1.c_str());

    break;
  }
}

bool
CQChartsBubbleObj::
inside(const CPoint2D &p) const
{
  if (CQChartsUtil::PointPointDistance(p, CPoint2D(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}
