#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>
#include <QTimer>

namespace {

int s_colorId = -1;

int nextColorId() {
  ++s_colorId;

  return s_colorId;
}

}

//------

CQChartsHierBubblePlotType::
CQChartsHierBubblePlotType()
{
  addParameters();
}

void
CQChartsHierBubblePlotType::
addParameters()
{
  addColumnParameter("name" , "Name"  , "nameColumn" , "", 0);
  addColumnParameter("value", "Value" , "valueColumn", "", 1);

  addStringParameter("separator", "Separator", "separator", "optional", "/");
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
 CQChartsPlot(view, view->charts()->plotType("hierbubble"), model)
{
  setMargins(1, 1, 1, 1);

  addTitle();
}

void
CQChartsHierBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "separator" );
  addProperty("", this, "fontHeight");
}

void
CQChartsHierBubblePlot::
setCurrentRoot(CQChartsHierBubbleHierNode *hier)
{
  currentRoot_ = hier;
}

void
CQChartsHierBubblePlot::
updateCurrentRoot()
{
  replaceNodes();

  updateObjs();
}

void
CQChartsHierBubblePlot::
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
CQChartsHierBubblePlot::
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

  if (! root_)
    initNodes();

  //---

  initNodeObjs(currentRoot_, nullptr, 0);
}

void
CQChartsHierBubblePlot::
initNodeObjs(CQChartsHierBubbleHierNode *hier, CQChartsHierBubbleHierObj *parentObj, int depth)
{
  CQChartsHierBubbleHierObj *hierObj = 0;

  if (hier != root_) {
    double r = hier->radius();

    CQChartsGeom::BBox rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    hierObj = new CQChartsHierBubbleHierObj(this, hier, parentObj, rect, hier->depth(), maxDepth());

    hierObj->setId(QString("%1:%2").arg(hier->name()).arg(hier->size()));

    addPlotObject(hierObj);
  }

  //---

  for (auto hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1);
  }

  //---

  for (auto node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CQChartsGeom::BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    CQChartsHierBubbleObj *obj =
      new CQChartsHierBubbleObj(this, node, parentObj, rect, node->depth(), maxDepth());

    obj->setId(QString("%1:%2").arg(node->name()).arg(node->size()));

    addPlotObject(obj);
  }
}

void
CQChartsHierBubblePlot::
initNodes()
{
  hierInd_ = 0;

  root_ = new CQChartsHierBubbleHierNode(this, 0, "<root>");

  root_->setDepth(0);
  root_->setHierInd(hierInd_++);

  currentRoot_ = root_;

  //---

  if (isHier())
    loadChildren(root_);
  else
    loadFlat();

  maxHierInd_ = hierInd_;

  //---

  firstHier_ = root_;

  while (firstHier_ && firstHier_->getChildren().size() == 1)
    firstHier_ = firstHier_->getChildren()[0];

  //---

  replaceNodes();
}

void
CQChartsHierBubblePlot::
replaceNodes()
{
  placeNodes(currentRoot_);
}

void
CQChartsHierBubblePlot::
placeNodes(CQChartsHierBubbleHierNode *hier)
{
  initNodes(hier);

  //---

  hier->packNodes();

  offset_ = CQChartsGeom::Point(hier->x(), hier->y());
  scale_  = (hier->radius() > 0.0 ? 1.0/hier->radius() : 1.0);

  //---

  hier->setX((hier->x() - offset_.x)*scale_);
  hier->setY((hier->y() - offset_.y)*scale_);

  hier->setRadius(1.0);

  transformNodes(hier);
}

void
CQChartsHierBubblePlot::
initNodes(CQChartsHierBubbleHierNode *hier)
{
  for (auto hierNode : hier->getChildren()) {
    hierNode->initRadius();

    initNodes(hierNode);
  }

  //---

  for (auto node : hier->getNodes())
    node->initRadius();
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

bool
CQChartsHierBubblePlot::
isHier() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return true;

  int nc = model->rowCount();

  for (int i = 0; i < nc; ++i) {
    QModelIndex index1 = model->index(i, 0);

    if (model->rowCount(index1) > 0)
      return true;
  }

  return false;
}

void
CQChartsHierBubblePlot::
loadChildren(CQChartsHierBubbleHierNode *hier, const QModelIndex &index, int depth)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  int colorId = -1;

  int nr = model->rowCount(index);

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd  = model->index(r, nameColumn (), index);
    QModelIndex valueInd = model->index(r, valueColumn(), index);

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    //---

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    //---

    if (model->rowCount(nameInd) > 0) {
      CQChartsHierBubbleHierNode *hier1 =
        new CQChartsHierBubbleHierNode(this, hier, name, nameInd1);

      hier1->setDepth(depth);
      hier1->setHierInd(hierInd_++);

      loadChildren(hier1, nameInd, depth + 1);
    }
    else {
      if (colorId < 0)
        colorId = nextColorId();

      bool ok;

      int size = CQChartsUtil::modelInteger(model, valueInd, ok);

      if (! ok) size = 1;

      CQChartsHierBubbleNode *node =
        new CQChartsHierBubbleNode(hier, name, size, colorId, nameInd1);

      node->setDepth(depth);

      hier->addNode(node);
    }
  }
}

void
CQChartsHierBubblePlot::
loadFlat()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd  = model->index(r, nameColumn ());
    QModelIndex valueInd = model->index(r, valueColumn());

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    //---

    bool ok1, ok2;

    QString name = CQChartsUtil::modelString (model, nameInd , ok1);
    int     size = CQChartsUtil::modelInteger(model, valueInd, ok2);

    QStringList strs;

    if (separator().length())
      strs = name.split(separator(), QString::SkipEmptyParts);
    else
      strs << name;

    if (! strs.length())
      continue;

    //--

    int depth = strs.length();

    maxDepth_ = std::max(maxDepth_, depth + 1);

    CQChartsHierBubbleHierNode *parent = root_;

    for (int j = 0; j < strs.length() - 1; ++j) {
      CQChartsHierBubbleHierNode *child = childHierNode(parent, strs[j]);

      if (! child) {
        child = new CQChartsHierBubbleHierNode(this, parent, strs[j], nameInd1);

        child->setDepth(depth);
        child->setHierInd(hierInd_++);
      }

      parent = child;
    }

    CQChartsHierBubbleNode *node = childNode(parent, strs[strs.length() - 1]);

    if (! node) {
      node = new CQChartsHierBubbleNode(parent, strs[strs.length() - 1], size, depth, nameInd1);

      node->setDepth(depth);

      parent->addNode(node);
    }
  }
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
childHierNode(CQChartsHierBubbleHierNode *parent, const QString &name) const
{
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
childNode(CQChartsHierBubbleHierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

void
CQChartsHierBubblePlot::
zoomFull()
{
  setCurrentRoot(root_);

  updateCurrentRoot();
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
  drawBounds(p, currentRoot_);
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

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // draw bubble
  p->setPen  (QColor(0,0,0));
  p->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  p->drawPath(path);

  //---

#if 0
  for (auto hierNode : hier->getChildren()) {
    drawBounds(p, hierNode);
  }
#endif
}

QColor
CQChartsHierBubblePlot::
nodeColor(CQChartsHierBubbleNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/(maxColorId() + 1), c);
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                          CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                          int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), hier_(hier), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsHierBubbleHierObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(hier_->x(), hier_->y())) < hier_->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleHierObj::
clickZoom(const CQChartsGeom::Point &)
{
#if 0
  CQChartsHierBubbleHierNode *parent1 = hier_->parent();
  CQChartsHierBubbleHierNode *parent2 = (parent1 ? parent1->parent() : nullptr);

  while (parent2 && parent2 != plot_->currentRoot()) {
    parent1 = parent2;
    parent2 = parent1->parent();
  }

  if (parent1) {
    plot_->setCurrentRoot(parent1);

    QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
  }
#else
  plot_->setCurrentRoot(hier_);

  QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
#endif
}

bool
CQChartsHierBubbleHierObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = hier_->ind();

  return (ind == nind);
}

void
CQChartsHierBubbleHierObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
//double f = (1.0*(i_ + 1))/(n_ + 1);

  CQChartsHierBubbleHierNode *root = hier_->parent();

  if (! root)
    root = hier_;

  //---

  double r = hier_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x() - r, hier_->y() + r, px1, py1);
  plot_->windowToPixel(hier_->x() + r, hier_->y() - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  // calc stroke and brush

//QColor c = plot_->hierColor(hier_);
//QColor c = plot_->interpPaletteColor(f);
  QColor c = plot_->interpPaletteColor((1.0*(root->hierInd() + 1))/(plot_->maxHierInd() + 1));

  QBrush brush(c);

//QColor bc = plot_->textColor(c);
  QColor bc = Qt::black;

  QPen pen(bc);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  // draw bubble
  p->setPen  (pen);
  p->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  p->drawPath(path);
}

//------

CQChartsHierBubbleObj::
CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                      CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsHierBubbleObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleObj::
clickZoom(const CQChartsGeom::Point &)
{
#if 0
  CQChartsHierBubbleHierNode *parent1 = node_->parent();
  CQChartsHierBubbleHierNode *parent2 = (parent1 ? parent1->parent() : nullptr);

  while (parent2 && parent2 != plot_->currentRoot()) {
    parent1 = parent2;
    parent2 = parent1->parent();
  }

  if (parent1) {
    plot_->setCurrentRoot(parent1);

    QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
  }
#else
  CQChartsHierBubbleHierNode *parent = node_->parent();

  plot_->setCurrentRoot(parent);

  QTimer::singleShot(10, plot_, SLOT(updateCurrentRoot()));
#endif
}

void
CQChartsHierBubbleObj::
mousePress(const CQChartsGeom::Point &)
{
  const QModelIndex &ind = node_->ind();

  plot_->beginSelect();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());

  plot_->endSelect();
}

bool
CQChartsHierBubbleObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = node_->ind();

  return (ind == nind);
}

void
CQChartsHierBubbleObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  //CQChartsHierBubbleHierNode *root = node_->rootNode(plot_->firstHier());
  CQChartsHierBubbleHierNode *root = node_->parent();

//QColor c = plot_->interpPaletteColor((1.0*(i_ + 1))/(n_ + 1));
//QColor c = plot_->nodeColor(node_);
  QColor c = plot_->interpPaletteColor((1.0*(root->hierInd() + 1))/(plot_->maxHierInd() + 1));

  QBrush brush(c);

  QColor bc = Qt::black;
  QColor tc = CQChartsUtil::bwColor(c);

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

  QString name = node_->name();

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

//------

CQChartsHierBubbleHierNode::
CQChartsHierBubbleHierNode(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                           const QString &name, const QModelIndex &ind) :
 CQChartsHierBubbleNode(parent, name, 0.0, -1, ind), plot_(plot)
{
  if (parent_)
    parent_->children_.push_back(this);
}

void
CQChartsHierBubbleHierNode::
packNodes()
{
  pack_.reset();

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
    (*pn)->resetPosition();

  //---

  // pack child hier nodes first
  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
    (*p)->packNodes();

  //---

  // make single list of nodes to pack
  Nodes packNodes;

  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
    packNodes.push_back(*p);

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
    packNodes.push_back(*pn);

  // sort nodes
  std::sort(packNodes.begin(), packNodes.end(), CQChartsHierBubbleNodeCmp());

  // pack nodes
  for (Nodes::const_iterator pp = packNodes.begin(); pp != packNodes.end(); ++pp)
    pack_.addNode(*pp);

  //---

  // get bounding circle
  double xc { 0.0 }, yc { 0.0 }, r { 1.0 };

  pack_.boundingCircle(xc, yc, r);

  // set center and radius
  x_ = xc;
  y_ = yc;

  setRadius(r);

  //setRadius(std::max(std::max(fabs(xmin), xmax), std::max(fabs(ymin), ymax)));
}

void
CQChartsHierBubbleHierNode::
addNode(CQChartsHierBubbleNode *node)
{
  nodes_.push_back(node);

  size_ += node->size();

  CQChartsHierBubbleHierNode *parent = parent_;

  while (parent) {
    parent->size_ += node->size();

    parent = parent->parent_;
  }
}

void
CQChartsHierBubbleHierNode::
setPosition(double x, double y)
{
  double dx = x - this->x();
  double dy = y - this->y();

  CQChartsHierBubbleNode::setPosition(x, y);

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
    CQChartsHierBubbleNode *node = *pn;

    node->setPosition(node->x() + dx, node->y() + dy);
  }

  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p) {
    CQChartsHierBubbleHierNode *hierNode = *p;

    hierNode->setPosition(hierNode->x() + dx, hierNode->y() + dy);
  }
}

//------

CQChartsHierBubbleNode::
CQChartsHierBubbleNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                       double size, int colorId, const QModelIndex &ind) :
 parent_(parent), id_(nextId()), name_(name), size_(size), colorId_(colorId), ind_(ind)
{
  initRadius();
}

void
CQChartsHierBubbleNode::
setPosition(double x, double y)
{
  CQChartsCircleNode::setPosition(x, y);

  placed_ = true;
}
