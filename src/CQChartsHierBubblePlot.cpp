#include <CQChartsHierBubblePlot.h>
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

CQChartsHierBubblePlotType::
CQChartsHierBubblePlotType()
{
  addColumnParameter("name" , "Name"  , "nameColumn" , "", 0);
  addColumnParameter("value", "Value" , "valueColumn", "", 1);
}

CQChartsPlot *
CQChartsHierBubblePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsHierBubblePlot(view, model);
}

//------

CQChartsHierBubblePlot::
CQChartsHierBubblePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("bubble"), model)
{
  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  applyDataRange();

  setMargins(1, 1, 1, 1);
}

void
CQChartsHierBubblePlot::
updateRange()
{
  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  applyDataRange();
}

void
CQChartsHierBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "fontHeight");
}

void
CQChartsHierBubblePlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //--

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  if (! root_)
    initNodes();

  //---

  initObjs(root_, 0);
}

void
CQChartsHierBubblePlot::
initObjs(CQChartsHierBubbleHierNode *hier, int depth)
{
  if (hier != root_) {
    double r = hier->radius();

    CBBox2D rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    CQChartsHierBubbleHierObj *hierObj =
      new CQChartsHierBubbleHierObj(this, hier, rect, depth, maxDepth());

    hierObj->setId(QString("%1:%2").arg(hier->name().c_str()).arg(hier->size()));

    addPlotObject(hierObj);
  }

  //---

  for (auto hierNode : hier->getChildren()) {
    initObjs(hierNode, depth + 1);
  }

  //---

  for (auto node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CBBox2D rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    CQChartsHierBubbleObj *obj = new CQChartsHierBubbleObj(this, node, rect, depth, maxDepth());

    obj->setId(QString("%1:%2").arg(node->name().c_str()).arg(node->size()));

    addPlotObject(obj);
  }
}

void
CQChartsHierBubblePlot::
initNodes()
{
  root_ = new CQChartsHierBubbleHierNode(0, "<root>");

  //---

  QModelIndex index;

  loadChildren(root_, index, 0);

  //---

  root_->packNodes();

  offset_ = CPoint2D(root_->x(), root_->y());
  scale_  = 1.0/root_->radius();

  //---

  transformNodes(root_);
}

void
CQChartsHierBubblePlot::
transformNodes(CQChartsHierBubbleHierNode *hier)
{
  for (auto hierNode : hier->getChildren()) {
    hierNode->setX((hierNode->x() - offset_.x)*scale_);
    hierNode->setY((hierNode->y() - offset_.y)*scale_);

    hierNode->setRadius(hierNode->radius()*scale_);

    transformNodes(hierNode);
  }

  //---

  for (auto node : hier->getNodes()) {
    node->setX((node->x() - offset_.x)*scale_);
    node->setY((node->y() - offset_.y)*scale_);

    node->setRadius(node->radius()*scale_);
  }
}

void
CQChartsHierBubblePlot::
loadChildren(CQChartsHierBubbleHierNode *hier, const QModelIndex &index, int depth)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  Nodes nodes;

  int colorId = -1;

  uint nc = model->rowCount(index);

  for (uint i = 0; i < nc; ++i) {
    QModelIndex index1 = model->index(i, nameColumn_, index);

    bool ok;

    QString name = CQChartsUtil::modelString(model, index1, ok);

    //---

    if (model->rowCount(index1) > 0) {
      CQChartsHierBubbleHierNode *hier1 = new CQChartsHierBubbleHierNode(hier, name.toStdString());

      loadChildren(hier1, index1, depth + 1);
    }
    else {
      if (colorId < 0)
        colorId = nextColorId();

      QModelIndex index2 = model->index(i, valueColumn_, index);

      bool ok;

      int size = CQChartsUtil::modelInteger(model, index2, ok);

      if (! ok) size = 1;

      CQChartsHierBubbleNode *node = new CQChartsHierBubbleNode(name.toStdString(), size, colorId);

      hier->addNode(node);
    }
  }
}

void
CQChartsHierBubblePlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsHierBubblePlot::
drawForeground(QPainter *p)
{
  drawBounds(p, root_);
}

void
CQChartsHierBubblePlot::
drawBounds(QPainter *p, CQChartsHierBubbleHierNode *hier)
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  double px1, py1, px2, py2;

  windowToPixel(xc - r, yc + r, px1, py1);
  windowToPixel(xc + r, yc - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen(QColor(0,0,0));
  p->setBrush(QColor(0,0,0,0));

  p->drawPath(path);

  //---

  for (auto hierNode : hier->getChildren()) {
    drawBounds(p, hierNode);
  }
}

QColor
CQChartsHierBubblePlot::
nodeColor(CQChartsHierBubbleNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/numColors, c);
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                          const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), hier_(hier), i_(i), n_(n)
{
}

void
CQChartsHierBubbleHierObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFontMetrics fm(p->font());

  //QColor c = plot_->objectStateColor(this, plot_->hierColor(hier_));
  QColor c = plot_->interpPaletteColor((1.0*i_)/n_);

  QColor c1 = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.8);

  QColor tc = plot_->textColor(c1);

  double r = hier_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x() - r, hier_->y() + r, px1, py1);
  plot_->windowToPixel(hier_->x() + r, hier_->y() - r, px2, py2);

  QPainterPath path;

  path.addEllipse(QRectF(px1, py1, px2 - px1, py2 - py1));

  p->setPen  (tc);
  p->setBrush(c1);

  p->drawPath(path);
}

bool
CQChartsHierBubbleHierObj::
inside(const CPoint2D &p) const
{
  if (CQChartsUtil::PointPointDistance(p, CPoint2D(hier_->x(), hier_->y())) < hier_->radius())
    return true;

  return false;
}

//------

CQChartsHierBubbleObj::
CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                      const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), i_(i), n_(n)
{
}

void
CQChartsHierBubbleObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  p->setFont(font);

  //---

  QFontMetrics fm(p->font());

  QColor c = plot_->interpPaletteColor((1.0*i_)/n_);
  //QColor c = plot_->objectStateColor(this, plot_->nodeColor(node_));

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

    int tw = fm.width(name1.c_str());

    if (tw > 2*(px2 - px1)) continue;

    p->drawText(px1 - tw/2, py1 + fm.descent(), name1.c_str());

    break;
  }
}

bool
CQChartsHierBubbleObj::
inside(const CPoint2D &p) const
{
  if (CQChartsUtil::PointPointDistance(p, CPoint2D(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}
