#include <CQChartsTreeMapPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>

namespace {

int s_colorId = 0;

int nextColorId() {
  ++s_colorId;

  return s_colorId;
}

}

//------

CQChartsTreeMapPlotType::
CQChartsTreeMapPlotType()
{
  addColumnParameter("name" , "Name" , "nameColumn" , "", 0);
  addColumnParameter("value", "Value", "valueColumn", "", 1);

  addStringParameter("separator", "Separator", "separator", "optional", "/");
}

CQChartsPlot *
CQChartsTreeMapPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsTreeMapPlot(view, model);
}

//------

CQChartsTreeMapPlot::
CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("treemap"), model)
{
  setMargins(1, 1, 1, 1);

  addTitle();
}

void
CQChartsTreeMapPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "separator"   );
  addProperty("", this, "titles"      );
  addProperty("", this, "fontHeight"  );
  addProperty("", this, "headerHeight");
  addProperty("", this, "headerColor" );
  addProperty("", this, "marginWidth" );
}

void
CQChartsTreeMapPlot::
setCurrentRoot(CQChartsTreeMapHierNode *hier)
{
  currentRoot_ = hier;

  updateCurrentRoot();
}

void
CQChartsTreeMapPlot::
updateCurrentRoot()
{
  replaceNodes();

  updateObjs();
}

void
CQChartsTreeMapPlot::
updateRange(bool apply)
{
  dataRange_.reset();

  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  if (apply)
    applyDataRange();
}

void
CQChartsTreeMapPlot::
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
CQChartsTreeMapPlot::
initNodeObjs(CQChartsTreeMapHierNode *hier, CQChartsTreeMapHierObj *parentObj, int depth)
{
  CQChartsTreeMapHierObj *hierObj = 0;

  if (hier != root_) {
    CQChartsGeom::BBox rect(hier->x(), hier->y(), hier->x() + hier->w(), hier->y() + hier->h());

    hierObj = new CQChartsTreeMapHierObj(this, hier, parentObj, rect, hier->depth(), maxDepth());

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

    CQChartsGeom::BBox rect(node->x(), node->y(), node->x() + node->w(), node->y() + node->h());

    CQChartsTreeMapObj *obj =
      new CQChartsTreeMapObj(this, node, parentObj, rect, node->depth(), maxDepth());

    obj->setId(QString("%1:%2").arg(node->name()).arg(node->size()));

    addPlotObject(obj);
  }
}

void
CQChartsTreeMapPlot::
initNodes()
{
  hierInd_ = 0;

  root_ = new CQChartsTreeMapHierNode(this, 0, "<root>");

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
CQChartsTreeMapPlot::
replaceNodes()
{
  windowHeaderHeight_ = pixelToWindowHeight(headerHeight());
  windowMarginWidth_  = pixelToWindowWidth (marginWidth());

  placeNodes(currentRoot_);
}

void
CQChartsTreeMapPlot::
placeNodes(CQChartsTreeMapHierNode *hier)
{
  hier->setPosition(-1, -1, 2, 2);
}

bool
CQChartsTreeMapPlot::
isHier() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return true;

  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    QModelIndex index1 = model->index(r, 0);

    if (model->rowCount(index1) > 0)
      return true;
  }

  return false;
}

void
CQChartsTreeMapPlot::
loadChildren(CQChartsTreeMapHierNode *hier, const QModelIndex &index, int depth, int colorId)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

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
      CQChartsTreeMapHierNode *hier1 =
        new CQChartsTreeMapHierNode(this, hier, name, nameInd1);

      hier1->setDepth(depth);
      hier1->setHierInd(hierInd_++);

      int colorId1 = colorId;

      if (hier != currentRoot_) {
        if (colorId1 < 0)
          colorId1 = nextColorId();
      }

      loadChildren(hier1, nameInd, depth + 1, colorId1);
    }
    else {
      if (colorId < 0)
        colorId = nextColorId();

      bool ok;

      int value = CQChartsUtil::modelInteger(model, valueInd, ok);

      if (! ok) value = 1;

      CQChartsTreeMapNode *node =
        new CQChartsTreeMapNode(hier, name, value, nameInd1);

      node->setDepth(depth);

      node->setColorId(colorId);

      maxColorId_ = std::max(maxColorId_, colorId);

      hier->addNode(node);
    }
  }
}

void
CQChartsTreeMapPlot::
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

    CQChartsTreeMapHierNode *parent = root_;

    for (int j = 0; j < strs.length() - 1; ++j) {
      CQChartsTreeMapHierNode *child = childHierNode(parent, strs[j]);

      if (! child) {
        child = new CQChartsTreeMapHierNode(this, parent, strs[j], nameInd1);

        child->setDepth(depth);
        child->setHierInd(hierInd_++);
      }

      parent = child;
    }

    CQChartsTreeMapNode *node = childNode(parent, strs[strs.length() - 1]);

    if (! node) {
      node = new CQChartsTreeMapNode(parent, strs[strs.length() - 1], size, nameInd1);

      node->setDepth(depth);

      parent->addNode(node);
    }
  }
}

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
childHierNode(CQChartsTreeMapHierNode *parent, const QString &name) const
{
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
childNode(CQChartsTreeMapHierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

void
CQChartsTreeMapPlot::
zoomFull()
{
  setCurrentRoot(root_);
}

void
CQChartsTreeMapPlot::
handleResize()
{
  replaceNodes();

  CQChartsPlot::handleResize();
}

void
CQChartsTreeMapPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

QColor
CQChartsTreeMapPlot::
nodeColor(CQChartsTreeMapNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/(maxColorId() + 1), c);
}

//------

CQChartsTreeMapHierObj::
CQChartsTreeMapHierObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                       CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                       int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), hier_(hier), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsTreeMapHierObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::BBox bbox(hier_->x(), hier_->y(), hier_->x() + hier_->w(), hier_->y() + hier_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

bool
CQChartsTreeMapHierObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = hier_->ind();

  return (ind == nind);
}

void
CQChartsTreeMapHierObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x()             , hier_->y()             , px1, py1);
  plot_->windowToPixel(hier_->x() + hier_->w(), hier_->y() + hier_->h(), px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc stroke and brush

//QColor c = plot_->interpPaletteColor((1.0*(i_ + 1))/(n_ + 1));
//QColor c = plot_->hierColor(hier_);
//QColor c = plot_->interpPaletteColor((1.0*(hier_->hierInd() + 1))/(plot_->maxHierInd() + 1));
  QColor c = plot_->headerColor();

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

  // draw rectangle
  p->setPen  (bpen);
  p->setBrush(brush);

  p->drawRect(qrect);

  //---

  // set font size
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  //---

  // calc text size and position
  p->setFont(font);

  QString name = hier_->name();

  QFontMetricsF fm(p->font());

  plot_->windowToPixel(hier_->x(), hier_->y() + hier_->h(), px1, py1);

  //---

  // draw label
  p->setClipRect(qrect, Qt::ReplaceClip);

  plot_->drawContrastText(p, px1 + 2, py1 + fm.ascent(), name, tpen);

  //---

  p->restore();
}

//------

CQChartsTreeMapObj::
CQChartsTreeMapObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                   CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsTreeMapObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::BBox bbox(node_->x(), node_->y(), node_->x() + node_->w(), node_->y() + node_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

void
CQChartsTreeMapObj::
clickZoom(const CQChartsGeom::Point &)
{
  CQChartsTreeMapHierNode *parent1 = node_->parent();
  CQChartsTreeMapHierNode *parent2 = (parent1 ? parent1->parent() : nullptr);

  while (parent2 && parent2 != plot_->currentRoot()) {
    parent1 = parent2;
    parent2 = parent1->parent();
  }

  if (parent1)
    plot_->setCurrentRoot(parent1);
}

void
CQChartsTreeMapObj::
mousePress(const CQChartsGeom::Point &)
{
  const QModelIndex &ind = node_->ind();

  plot_->beginSelect();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());

  plot_->endSelect();
}

bool
CQChartsTreeMapObj::
isIndex(const QModelIndex &ind) const
{
  const QModelIndex &nind = node_->ind();

  return (ind == nind);
}

void
CQChartsTreeMapObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  //CQChartsTreeMapHierNode *root = node_->rootNode(plot_->firstHier());
  CQChartsTreeMapHierNode *root = node_->parent();

  //---

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x()             , node_->y()             , px1, py1);
  plot_->windowToPixel(node_->x() + node_->w(), node_->y() + node_->h(), px2, py2);

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1 + 1, py2 + 1, px2 - 1, py1 - 1));

  //---

  // calc stroke and brush

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

  // draw rectangle
  p->setPen  (bpen);
  p->setBrush(brush);

  p->drawRect(qrect);

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

  plot_->windowToPixel(node_->x() + node_->w()/2, node_->y() + node_->h()/2, px1, py1);

  double fdy = (fm.ascent() - fm.descent())/2;

  //---

  // draw label
  p->setClipRect(qrect, Qt::ReplaceClip);

  plot_->drawContrastText(p, px1 - tw/2, py1 + fdy, name, tpen);

  //---

  p->restore();
}

//------

CQChartsTreeMapHierNode::
CQChartsTreeMapHierNode(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent,
                        const QString &name, const QModelIndex &ind) :
 CQChartsTreeMapNode(parent, name, 0.0, ind), plot_(plot)
{
  if (parent_)
    parent_->children_.push_back(this);
}

double
CQChartsTreeMapHierNode::
size() const
{
  double s = 0.0;

  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
    s += (*p)->size();

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
   s += (*pn)->size();

  return s;
}

void
CQChartsTreeMapHierNode::
packNodes(double x, double y, double w, double h)
{
  double whh = plot_->windowHeaderHeight();
  double wmw = plot_->windowMarginWidth();

  double dh = (plot_->isTitles() ? (h > whh ? whh : 0.0) : 0.0);
  double m  = (w > wmw ? wmw : 0.0);

  // make single list of nodes to pack
  Nodes nodes;

  for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
    nodes.push_back(*p);

  for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
    nodes.push_back(*pn);

  // sort nodes by size
  std::sort(nodes.begin(), nodes.end(), CQChartsTreeMapNodeCmp());

  //std::cerr << name() << "\n";
  //for (uint i = 0; i < nodes.size(); ++i)
  //  std::cerr << " " << nodes[i]->name() << ":" << nodes[i]->size() << "\n";

  packSubNodes(x + m/2, y + m/2, w - m, h - dh - m, nodes);
}

void
CQChartsTreeMapHierNode::
packSubNodes(double x, double y, double w, double h, const Nodes &nodes)
{
  // place nodes
  int n = nodes.size();
  if (n == 0) return;

  if (n >= 2) {
    int n1 = n/2;

    Nodes  nodes1, nodes2;
    double size1 = 0.0, size2 = 0.0;

    for (int i = 0; i < n1; ++i) {
      size1 += nodes[i]->size();

      nodes1.push_back(nodes[i]);
    }

    for (int i = n1; i <  n; ++i) {
      size2 += nodes[i]->size();

      nodes2.push_back(nodes[i]);
    }

    // split area = (w*h) if largest direction
    // e.g. split at w1. area1 = w1*h; area2 = (w - w1)*h;
    // area1/area2 = w1/(w - w1) = size1/size2;
    // w1*size2 = w*size1 - w1*size1;
    // w1 = (w*size1)/(size1 + size2);

    double size12 = size1 + size2;

    if (size12 == 0.0)
      return;

    double f = size1/size12;

    if (w >= h) {
      double w1 = f*w;

      packSubNodes(x     , y,     w1, h, nodes1);
      packSubNodes(x + w1, y, w - w1, h, nodes2);
    }
    else {
      double h1 = f*h;

      packSubNodes(x, y     , w, h1    , nodes1);
      packSubNodes(x, y + h1, w, h - h1, nodes2);
    }
  }
  else {
    CQChartsTreeMapNode *node = nodes[0];

    node->setPosition(x, y, w, h);
  }
}

void
CQChartsTreeMapHierNode::
setPosition(double x, double y, double w, double h)
{
  CQChartsTreeMapNode::setPosition(x, y, w, h);

  packNodes(x, y, w, h);
}

void
CQChartsTreeMapHierNode::
addNode(CQChartsTreeMapNode *node)
{
  nodes_.push_back(node);
}

//------

CQChartsTreeMapNode::
CQChartsTreeMapNode(CQChartsTreeMapHierNode *parent, const QString &name, double size,
                    const QModelIndex &ind) :
 parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind)
{
}

void
CQChartsTreeMapNode::
setPosition(double x, double y, double w, double h)
{
  assert(! COSNaN::is_nan(x) && ! COSNaN::is_nan(y) &&
         ! COSNaN::is_nan(w) && ! COSNaN::is_nan(h));

  x_ = x; y_ = y;
  w_ = w; h_ = h;

  //std::cerr << "Node: " << name() << " @ ( " << x_ << "," << y_ << ")" <<
  //             " [" << w_ << "," << h_ << "]" << "\n";

  placed_ = true;
}

bool
CQChartsTreeMapNode::
contains(double x, double y) const
{
  return (x >= x_ && x <= (x_ + w_) && y >= y_ && y <= (y_ + h_));
}

CQChartsTreeMapHierNode *
CQChartsTreeMapNode::
rootNode(CQChartsTreeMapHierNode *root) const
{
  CQChartsTreeMapHierNode *parent = this->parent();

  while (parent && parent->parent() && parent->parent() != root)
    parent = parent->parent();

  return parent;
}
