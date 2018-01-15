#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsTip.h>
#include <CGradientPalette.h>

#include <QMenu>
#include <QPainter>

CQChartsHierBubblePlotType::
CQChartsHierBubblePlotType()
{
}

void
CQChartsHierBubblePlotType::
addParameters()
{
  CQChartsHierPlotType::addParameters();
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
 CQChartsHierPlot(view, view->charts()->plotType("hierbubble"), model)
{
  textBoxObj_ = new CQChartsTextBoxObj(this);

  textBoxObj_->setBackgroundColor(CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE));

  setBorder(true);
  setFilled(true);

  setTextContrast(true);

  textBoxObj_->setTextFontSize(12.0);

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
  CQChartsHierPlot::addProperties();

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

  addProperty("color", this, "colorMapEnabled", "mapEnabled");
  addProperty("color", this, "colorMapMin"    , "mapMin"    );
  addProperty("color", this, "colorMapMax"    , "mapMax"    );
}

//------

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
currentRoot() const
{
  CQChartsHierBubbleHierNode *currentRoot = root_;

  QStringList names = currentRootName_.split(separator(), QString::SkipEmptyParts);

  if (names.empty())
    return currentRoot;

  for (int i = 0; i < names.size(); ++i) {
    CQChartsHierBubbleHierNode *hier = childHierNode(currentRoot, names[i]);

    if (! hier)
      return currentRoot;

    currentRoot = hier;
  }

  return currentRoot;
}

void
CQChartsHierBubblePlot::
setCurrentRoot(CQChartsHierBubbleHierNode *hier, bool update)
{
  if (hier)
    currentRootName_ = hier->hierName();
  else
    currentRootName_ = "";

  if (update)
    updateCurrentRoot();
}

void
CQChartsHierBubblePlot::
updateCurrentRoot()
{
  replaceNodes();

  updateObjs();
}

//---

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
CQChartsHierBubblePlot::
colorSetColor(int i, OptColor &color)
{
  return colorSet_.icolor(i, color);
}

//------

void
CQChartsHierBubblePlot::
updateObjs()
{
  colorSet_.clear();

  resetNodes();

  CQChartsPlot::updateObjs();
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

  // init value sets
  if (colorSet_.empty())
    initColorSet();

  //---

  if (! root_)
    initNodes();

  //---

  initColorIds();

  colorNodes(root_);

  //---

  initNodeObjs(currentRoot(), nullptr, 0);

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
resetNodes()
{
  delete root_;

  root_ = nullptr;
}

void
CQChartsHierBubblePlot::
initNodes()
{
  hierInd_ = 0;

  root_ = new CQChartsHierBubbleHierNode(this, 0, "<root>");

  root_->setDepth(0);
  root_->setHierInd(hierInd_++);

  //---

  if (isHierarchical())
    loadChildren(root_);
  else
    loadFlat();

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
  placeNodes(currentRoot());
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

void
CQChartsHierBubblePlot::
colorNodes(CQChartsHierBubbleHierNode *hier)
{
  if (! hier->hasNodes() && ! hier->hasChildren()) {
    colorNode(hier);
  }
  else {
    for (const auto &node : hier->getNodes())
      colorNode(node);

    for (const auto &child : hier->getChildren())
      colorNodes(child);
  }
}

void
CQChartsHierBubblePlot::
colorNode(CQChartsHierBubbleNode *node)
{
  if (! node->color().isValid())
    node->setColorId(nextColorId());
}

void
CQChartsHierBubblePlot::
loadChildren(CQChartsHierBubbleHierNode *hier, const QModelIndex &index, int depth)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //CQBaseModel::Type valueColumnType = columnValueType(model, valueColumn());

  //---

  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  int nr = model->rowCount(index);

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd = model->index(r, nameColumn(), index);

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
      QModelIndex valueInd = model->index(r, valueColumn(), index);

      double size = 1.0;

      if (valueInd.isValid()) {
        bool ok;

        size = CQChartsUtil::modelReal(model, valueInd, ok);

        if (ok && size <= 0.0)
          ok = false;

        if (! ok)
          continue;
      }

      //---

      CQChartsHierBubbleNode *node =
        new CQChartsHierBubbleNode(this, hier, name, size, nameInd1);

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

  CQBaseModel::Type valueColumnType = columnValueType(model, valueColumn());

  //---

  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    QStringList  nameStrs;
    ModelIndices nameInds;

    if (! getHierColumnNames(r, nameColumns(), separator(), nameStrs, nameInds))
      continue;

    QModelIndex nameInd1 = normalizeIndex(nameInds[0]);

    //---

    double size = 1.0;

    QModelIndex valueInd = model->index(r, valueColumn());

    if (valueInd.isValid()) {
      bool ok2 = true;

      if      (valueColumnType == CQBaseModel::Type::REAL)
        size = CQChartsUtil::modelReal(model, valueInd, ok2);
      else if (valueColumnType == CQBaseModel::Type::INTEGER)
        size = CQChartsUtil::modelInteger(model, valueInd, ok2);
      else
        ok2 = false;

      if (ok2 && size <= 0.0)
        ok2 = false;

      if (! ok2)
        continue;
    }

    //---

    int depth = nameStrs.length();

    maxDepth_ = std::max(maxDepth_, depth + 1);

    //---

    CQChartsHierBubbleHierNode *parent = root_;

    for (int j = 0; j < nameStrs.length() - 1; ++j) {
      CQChartsHierBubbleHierNode *child = childHierNode(parent, nameStrs[j]);

      if (! child) {
        // remove any existing leaf node (save size to use in new hier node)
        double size = 0.0;

        CQChartsHierBubbleNode *node = childNode(parent, nameStrs[j]);

        if (node) {
          nameInd1 = node->ind();
          size     = node->size();

          parent->removeNode(node);

          delete node;
        }

        //---

        child = new CQChartsHierBubbleHierNode(this, parent, nameStrs[j], nameInd1);

        child->setSize(size);

        child->setDepth(depth);
        child->setHierInd(hierInd_++);
      }

      parent = child;
    }

    //---

    QString name = nameStrs[nameStrs.length() - 1];

    CQChartsHierBubbleNode *node = childNode(parent, name);

    if (! node) {
      // use hier node if already created
      CQChartsHierBubbleHierNode *child = childHierNode(parent, name);

      if (child) {
        child->setSize(size);

        continue;
      }

      //---

      node = new CQChartsHierBubbleNode(this, parent, name, size, nameInd1);

      node->setDepth(depth);

      OptColor color;

      if (colorSetColor(r, color))
        node->setColor(*color);

      parent->addNode(node);
    }
  }

  //----

  addExtraNodes(root_);
}

void
CQChartsHierBubblePlot::
addExtraNodes(CQChartsHierBubbleHierNode *hier)
{
  if (hier->size() > 0) {
    CQChartsHierBubbleNode *node =
      new CQChartsHierBubbleNode(this, hier, "", hier->size(), hier->ind());

    int r = unnormalizeIndex(hier->ind()).row();

    OptColor color;

    if (colorSetColor(r, color))
      node->setColor(*color);

    node->setDepth (hier->depth() + 1);
    node->setFiller(true);

    hier->addNode(node);

    hier->setSize(0.0);
  }

  for (const auto &child : hier->getChildren())
    addExtraNodes(child);
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

//------

bool
CQChartsHierBubblePlot::
addMenuItems(QMenu *menu)
{
  PlotObjs objs;

  selectedObjs(objs);

  QAction *pushAction   = new QAction("Push"   , menu);
  QAction *popAction    = new QAction("Pop"    , menu);
  QAction *popTopAction = new QAction("Pop Top", menu);

  connect(pushAction  , SIGNAL(triggered()), this, SLOT(pushSlot()));
  connect(popAction   , SIGNAL(triggered()), this, SLOT(popSlot()));
  connect(popTopAction, SIGNAL(triggered()), this, SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot() != root_);
  popTopAction->setEnabled(currentRoot() != root_);

  menu->addSeparator();

  menu->addAction(pushAction  );
  menu->addAction(popAction   );
  menu->addAction(popTopAction);

  menu->addSeparator();

  return true;
}

void
CQChartsHierBubblePlot::
pushSlot()
{
  PlotObjs objs;

  selectedObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w;

    pixelToWindow(CQChartsUtil::fromQPoint(pos), w);

    objsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  for (const auto &obj : objs) {
    CQChartsHierBubbleHierObj *hierObj = dynamic_cast<CQChartsHierBubbleHierObj *>(obj);

    if (hierObj) {
      CQChartsHierBubbleHierNode *hnode = hierObj->hierNode();

      setCurrentRoot(hnode, /*update*/true);

      break;
    }

    CQChartsHierBubbleObj *nodeObj = dynamic_cast<CQChartsHierBubbleObj *>(obj);

    if (nodeObj) {
      CQChartsHierBubbleNode *node = nodeObj->node();

      CQChartsHierBubbleHierNode *hnode = node->parent();

      if (hnode) {
        setCurrentRoot(hnode, /*update*/true);

        break;
      }
    }
  }
}

void
CQChartsHierBubblePlot::
popSlot()
{
  CQChartsHierBubbleHierNode *root = currentRoot();

  if (root && root->parent()) {
    setCurrentRoot(root->parent(), /*update*/true);
  }
}

void
CQChartsHierBubblePlot::
popTopSlot()
{
  CQChartsHierBubbleHierNode *root = currentRoot();

  if (root != root_) {
    setCurrentRoot(root_, /*update*/true);
  }
}

//------

void
CQChartsHierBubblePlot::
handleResize()
{
  dataRange_.reset();

  CQChartsPlot::handleResize();
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
  drawBounds(painter, currentRoot());
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
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                          CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                          int i, int n) :
 CQChartsHierBubbleObj(plot, hier, hierObj, rect, i, n), hier_(hier)
{
}

QString
CQChartsHierBubbleHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsHierBubbleObj::calcId();
}

QString
CQChartsHierBubbleHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hier_->hierName()).arg(hier_->hierSize());
  return CQChartsHierBubbleObj::calcTipId();
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

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = hier_->interpColor(plot_, plot_->numColorIds());

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
  if (node_->isFiller())
    return hierObj_->calcId();

  return QString("%1:%2").arg(node_->name()).arg(node_->hierSize());
}

QString
CQChartsHierBubbleObj::
calcTipId() const
{
  if (node_->isFiller())
    return hierObj_->calcTipId();

  CQChartsTableTip tableTip;

  //return QString("%1:%2").arg(name).arg(node_->hierSize());

  tableTip.addTableRow("Name", node_->hierName());
  tableTip.addTableRow("Size", node_->hierSize());

  if (plot_->colorColumn() >= 0) {
    QAbstractItemModel *model = plot_->model();

    int r = plot_->unnormalizeIndex(node_->ind()).row();

    bool ok;

    QString colorStr = CQChartsUtil::modelString(model, r, plot_->colorColumn(), ok);

    tableTip.addTableRow("Color", colorStr);
  }

  return tableTip.str();
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
addSelectIndex()
{
  const QModelIndex &ind = node_->ind();

  QModelIndex nameInd  = plot_->selectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  QModelIndex valueInd = plot_->selectIndex(ind.row(), plot_->valueColumn(), ind.parent());
  QModelIndex colorInd = plot_->selectIndex(ind.row(), plot_->colorColumn(), ind.parent());

  plot_->addSelectIndex(nameInd);

  if (valueInd.isValid())
    plot_->addSelectIndex(valueInd);

  if (colorInd.isValid())
    plot_->addSelectIndex(colorInd);
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

  QRectF qrect = CQChartsUtil::toQRect(CQChartsGeom::BBox(px1, py2, px2, py1));

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = node_->interpColor(plot_, plot_->numColorIds());

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

  QColor tc = plot_->interpTextColor(0, 1);

  QPen tpen(tc);

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  painter->save();

  //---

  // draw bubble
  painter->setPen  (pen);
  painter->setBrush(brush);

  QPainterPath path;

  path.addEllipse(qrect);

  painter->drawPath(path);

  //---

  // set font
  QFont font = plot_->textFont();

  painter->setFont(font);

  QFontMetricsF fm(painter->font());

  //---

  // calc text size and position
  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  plot_->windowToPixel(node_->x(), node_->y(), px1, py1);

  //---

  double tw = fm.width(name);

  //double fdy = (fm.ascent() - fm.descent())/2;
  double fdy = fm.descent();

  //---

  // draw label
  painter->setClipRect(qrect);

  if (plot_->isTextContrast())
    plot_->drawContrastText(painter, px1 - tw/2, py1 + fdy, name, tpen);
  else {
    painter->setPen(tpen);

    painter->drawText(px1 - tw/2, py1 + fdy, name);
  }

  //---

  painter->restore();
}

//------

CQChartsHierBubbleHierNode::
CQChartsHierBubbleHierNode(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                           const QString &name, const QModelIndex &ind) :
 CQChartsHierBubbleNode(plot, parent, name, 0.0, ind)
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

double
CQChartsHierBubbleHierNode::
hierSize() const
{
  double s = size();

  for (auto &child : children_)
    s += child->hierSize();

  for (auto &node : nodes_)
    s += node->hierSize();

  return s;
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
}

void
CQChartsHierBubbleHierNode::
removeNode(CQChartsHierBubbleNode *node)
{
  int n = nodes_.size();

  int i = 0;

  for ( ; i < n; ++i) {
    if (nodes_[i] == node)
      break;
  }

  assert(i < n);

  ++i;

  for ( ; i < n; ++i)
    nodes_[i - 1] = nodes_[i];

  nodes_.pop_back();
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

QColor
CQChartsHierBubbleHierNode::
interpColor(CQChartsHierBubblePlot *plot, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, n));

  if (colors.empty())
    return plot->interpPaletteColor(0, 1);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsHierBubbleNode::
CQChartsHierBubbleNode(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                       const QString &name, double size, const QModelIndex &ind) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind)
{
  initRadius();
}

CQChartsHierBubbleNode::
~CQChartsHierBubbleNode()
{
}

void
CQChartsHierBubbleNode::
initRadius()
{
  r_ = sqrt(hierSize()/(2*M_PI));
}

QString
CQChartsHierBubbleNode::
hierName() const
{
  if (parent() && parent() != plot()->root())
    return parent()->hierName() + "/" + name();
  else
    return name();
}

void
CQChartsHierBubbleNode::
setPosition(double x, double y)
{
  CQChartsCircleNode::setPosition(x, y);

  placed_ = true;
}

QColor
CQChartsHierBubbleNode::
interpColor(CQChartsHierBubblePlot *plot, int n) const
{
  if      (colorId() >= 0)
    return plot->interpPaletteColor(colorId(), n);
  else if (color().isValid())
    return color().interpColor(plot, 0, 1);
  else
    return plot->interpPaletteColor(0, 1);
}
