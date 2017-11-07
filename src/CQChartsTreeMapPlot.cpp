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

  addProperty("", this, "separator" );
  addProperty("", this, "fontHeight");
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
  placeNodes(currentRoot_);

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
    CBBox2D rect(hier->x(), hier->y(), hier->x() + hier->w(), hier->y() + hier->h());

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

    CBBox2D rect(node->x(), node->y(), node->x() + node->w(), node->y() + node->h());

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
  root_ = new CQChartsTreeMapHierNode(0, "<root>");

  root_->setDepth(0);

  currentRoot_ = root_;

  //---

  if (isHier())
    loadChildren(root_);
  else
    loadFlat();

  //---

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

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  uint nr = model->rowCount(index);

  for (uint r = 0; r < nr; ++r) {
    QModelIndex nameInd  = model->index(r, nameColumn (), index);
    QModelIndex valueInd = model->index(r, valueColumn(), index);

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    //---

    bool ok;

    QString name = CQChartsUtil::modelString(model, nameInd, ok);

    //---

    if (model->rowCount(nameInd) > 0) {
      CQChartsTreeMapHierNode *hier1 = new CQChartsTreeMapHierNode(hier, name);

      hier1->setDepth(depth);

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

      CQChartsTreeMapNode *node = new CQChartsTreeMapNode(hier, name, value, nameInd1);

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
        child = new CQChartsTreeMapHierNode(parent, strs[j]);

        child->setDepth(depth);
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
                       CQChartsTreeMapHierObj *hierObj, const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), hier_(hier), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsTreeMapHierObj::
inside(const CPoint2D &p) const
{
  CBBox2D bbox(hier_->x(), hier_->y(), hier_->x() + hier_->w(), hier_->y() + hier_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

void
CQChartsTreeMapHierObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  p->setFont(font);

  //---

  QColor c = plot_->interpPaletteColor((1.0*(i_ + 1))/(n_ + 1));
//QColor c = plot_->objectStateColor(this, plot_->hierColor(hier_));

  QColor c1 = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.8);

  QColor tc = plot_->textColor(c1);

  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x()             , hier_->y()             , px1, py1);
  plot_->windowToPixel(hier_->x() + hier_->w(), hier_->y() + hier_->h(), px2, py2);

  p->setPen(tc);

  if (isInside())
    p->setBrush(plot_->insideColor(c1));
  else
    p->setBrush(c1);

  p->drawRect(CQChartsUtil::toQRect(CBBox2D(px1, py1, px2, py2)));
}

//------

CQChartsTreeMapObj::
CQChartsTreeMapObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                   CQChartsTreeMapHierObj *hierObj, const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

bool
CQChartsTreeMapObj::
inside(const CPoint2D &p) const
{
  CBBox2D bbox(node_->x(), node_->y(), node_->x() + node_->w(), node_->y() + node_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

void
CQChartsTreeMapObj::
clickZoom(const CPoint2D &)
{
  CQChartsTreeMapHierNode *parent1 = node_->parent();
  CQChartsTreeMapHierNode *parent2 = (parent1 ? parent1->parent() : nullptr);

  while (parent2 && parent2 != plot_->currentRoot()) {
    parent1 = parent2;
    parent2 = (parent1 ? parent1->parent() : nullptr);
  }

  if (parent1)
    plot_->setCurrentRoot(parent1);
}

void
CQChartsTreeMapObj::
mousePress(const CPoint2D &)
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
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  p->setFont(font);

  //---

  QFontMetricsF fm(p->font());

  QColor c = plot_->interpPaletteColor((1.0*(i_ + 1))/(n_ + 1));
//QColor c = plot_->objectStateColor(this, plot_->nodeColor(node_));

  //QColor tc = plot_->textColor(c);
  QColor tc = Qt::black;

  if (isSelected())
    tc = Qt::white;

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x()             , node_->y()             , px1, py1);
  plot_->windowToPixel(node_->x() + node_->w(), node_->y() + node_->h(), px2, py2);

  p->setPen(tc);

  if (isInside())
    p->setBrush(plot_->insideColor(c));
  else
    p->setBrush(c);

  p->drawRect(CQChartsUtil::toQRect(CBBox2D(px1, py1, px2, py2)));

  //---

  p->setPen(tc);

  plot_->windowToPixel(node_->x() + node_->w()/2, node_->y() + node_->h()/2, px1, py1);

  int len = node_->name().size();

  for (int i = len; i >= 1; --i) {
    QString name1 = node_->name().mid(0, i);

    double tw = fm.width(name1);

    if (tw > 2*(px2 - px1)) continue;

    p->drawText(px1 - tw/2, py1 + fm.descent(), name1);

    break;
  }
}
