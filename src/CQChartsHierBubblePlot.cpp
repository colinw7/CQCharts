#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsFillObj.h>
#include <CGradientPalette.h>

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

CQChartsHierBubblePlot::
~CQChartsHierBubblePlot()
{
  delete textBoxObj_;

  delete root_;
}

//----

bool
CQChartsHierBubblePlot::
isFilled() const
{
  return textBoxObj_->isBackground();
}

void
CQChartsHierBubblePlot::
setFilled(bool b)
{
  textBoxObj_->setBackground(b);

  update();
}

QString
CQChartsHierBubblePlot::
fillColorStr() const
{
  return textBoxObj_->backgroundColorStr();
}

void
CQChartsHierBubblePlot::
setFillColorStr(const QString &s)
{
  textBoxObj_->setBackgroundColorStr(s);

  update();
}

QColor
CQChartsHierBubblePlot::
interpFillColor(int i, int n) const
{
  return textBoxObj_->interpBackgroundColor(i, n);
}

double
CQChartsHierBubblePlot::
fillAlpha() const
{
  return textBoxObj_->backgroundAlpha();
}

void
CQChartsHierBubblePlot::
setFillAlpha(double a)
{
  textBoxObj_->setBackgroundAlpha(a);

  update();
}

CQChartsHierBubblePlot::Pattern
CQChartsHierBubblePlot::
fillPattern() const
{
  return (Pattern) textBoxObj_->backgroundPattern();
}

void
CQChartsHierBubblePlot::
setFillPattern(Pattern pattern)
{
  textBoxObj_->setBackgroundPattern((CQChartsBoxObj::Pattern) pattern);

  update();
}

//---

bool
CQChartsHierBubblePlot::
isBorder() const
{
  return textBoxObj_->isBorder();
}

void
CQChartsHierBubblePlot::
setBorder(bool b)
{
  textBoxObj_->setBorder(b);

  update();
}

QString
CQChartsHierBubblePlot::
borderColorStr() const
{
  return textBoxObj_->borderColorStr();
}

void
CQChartsHierBubblePlot::
setBorderColorStr(const QString &str)
{
  textBoxObj_->setBorderColorStr(str);

  update();
}

QColor
CQChartsHierBubblePlot::
interpBorderColor(int i, int n) const
{
  return textBoxObj_->interpBorderColor(i, n);
}

double
CQChartsHierBubblePlot::
borderAlpha() const
{
  return textBoxObj_->borderAlpha();
}

void
CQChartsHierBubblePlot::
setBorderAlpha(double a)
{
  textBoxObj_->setBorderAlpha(a);

  update();
}

double
CQChartsHierBubblePlot::
borderWidth() const
{
  return textBoxObj_->borderWidth();
}

void
CQChartsHierBubblePlot::
setBorderWidth(double r)
{
  textBoxObj_->setBorderWidth(r);

  update();
}

//---

const QFont &
CQChartsHierBubblePlot::
textFont() const
{
  return textBoxObj_->textFont();
}

void
CQChartsHierBubblePlot::
setTextFont(const QFont &f)
{
  textBoxObj_->setTextFont(f);

  update();
}

QString
CQChartsHierBubblePlot::
textColorStr() const
{
  return textBoxObj_->textColorStr();
}

void
CQChartsHierBubblePlot::
setTextColorStr(const QString &s)
{
  textBoxObj_->setTextColorStr(s);

  update();
}

QColor
CQChartsHierBubblePlot::
interpTextColor(int i, int n) const
{
  return textBoxObj_->interpTextColor(i, n);
}

bool
CQChartsHierBubblePlot::
isTextContrast() const
{
  return textBoxObj_->isTextContrast();
}

void
CQChartsHierBubblePlot::
setTextContrast(bool b)
{
  textBoxObj_->setTextContrast(b);

  update();
}

//---

void
CQChartsHierBubblePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "separator");

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

bool
CQChartsHierBubblePlot::
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

  if (! root_)
    initNodes();

  //---

  initNodeObjs(currentRoot_, nullptr, 0);

  //---

  return true;
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
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsHierBubblePlot::
drawForeground(QPainter *painter)
{
  drawBounds(painter, currentRoot_);
}

void
CQChartsHierBubblePlot::
drawBounds(QPainter *painter, CQChartsHierBubbleHierNode *hier)
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
  QColor bc = interpBorderColor(0, 1);

  painter->setPen  (bc);
  painter->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);

  //---

#if 0
  for (auto hierNode : hier->getChildren()) {
    drawBounds(painter, hierNode);
  }
#endif
}

QColor
CQChartsHierBubblePlot::
interpNodeColor(CQChartsHierBubbleNode *node) const
{
  return interpPaletteColor((1.0*node->colorId())/(maxColorId() + 1), /*scale*/false);
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                          CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                          int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), hier_(hier), hierObj_(hierObj), i_(i), n_(n)
{
}

QString
CQChartsHierBubbleHierObj::
calcId() const
{
  return QString("%1:%2").arg(hier_->name()).arg(hier_->size());
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
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
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
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = plot_->interpFillColor(root->hierInd(), plot_->maxHierInd());

    c.setAlphaF(plot_->fillAlpha());

    brush.setColor(c);
    brush.setStyle(CQChartsFillObj::patternToStyle(
      (CQChartsFillObj::Pattern) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    pen.setColor (bc);
    pen.setWidthF(plot_->borderWidth());
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  // draw bubble
  painter->setPen  (pen);
  painter->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);
}

//------

CQChartsHierBubbleObj::
CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                      CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                      int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), node_(node), hierObj_(hierObj), i_(i), n_(n)
{
}

QString
CQChartsHierBubbleObj::
calcId() const
{
  return QString("%1:%2").arg(node_->name()).arg(node_->size());
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
addSelectIndex()
{
  const QModelIndex &ind = node_->ind();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->valueColumn(), ind.parent());
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
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double r = node_->radius();

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x() - r, node_->y() + r, px1, py1);
  plot_->windowToPixel(node_->x() + r, node_->y() - r, px2, py2);

  QRectF qrect(px1, py1, px2 - px1, py2 - py1);

  //---

  //CQChartsHierBubbleHierNode *root = node_->rootNode(plot_->firstHier());
  CQChartsHierBubbleHierNode *root = node_->parent();

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = plot_->interpFillColor(root->hierInd(), plot_->maxHierInd());

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

//------

CQChartsHierBubbleHierNode::
CQChartsHierBubbleHierNode(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                           const QString &name, const QModelIndex &ind) :
 CQChartsHierBubbleNode(parent, name, 0.0, -1, ind), plot_(plot)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsHierBubbleHierNode::
~CQChartsHierBubbleHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

void
CQChartsHierBubbleHierNode::
packNodes()
{
  pack_.reset();

  for (auto &node : nodes_)
    node->resetPosition();

  //---

  // pack child hier nodes first
  for (auto &child : children_)
    child->packNodes();

  //---

  // make single list of nodes to pack
  Nodes packNodes;

  for (auto &child : children_)
    packNodes.push_back(child);

  for (auto &node : nodes_)
    packNodes.push_back(node);

  // sort nodes
  std::sort(packNodes.begin(), packNodes.end(), CQChartsHierBubbleNodeCmp());

  // pack nodes
  for (auto &packNode : packNodes)
    pack_.addNode(packNode);

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

  for (auto &node : nodes_)
    node->setPosition(node->x() + dx, node->y() + dy);

  for (auto &child : children_)
    child->setPosition(child->x() + dx, child->y() + dy);
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
