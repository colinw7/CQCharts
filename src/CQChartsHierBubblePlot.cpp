#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

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

QString
CQChartsHierBubblePlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Hierarchical Bubble Plot").
    h3("Summary").
     p("Draws circles represent a set of data values and packs then into the "
       "smallest enclosing circle.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/hierbubble.png"));
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
 CQChartsHierPlot(view, view->charts()->plotType("hierbubble"), model),
 CQChartsObjShapeData<CQChartsHierBubblePlot>(this),
 CQChartsObjTextData <CQChartsHierBubblePlot>(this)
{
  NoUpdate noUpdate(this);

//setExactValue(false);

  //---

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setTextContrast(true);
  setTextFontSize(12.0);

  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  setOuterMargin(CQChartsLength("4px"), CQChartsLength("4px"),
                 CQChartsLength("4px"), CQChartsLength("4px"));

  addTitle();
}

CQChartsHierBubblePlot::
~CQChartsHierBubblePlot()
{
  delete nodeData_.root;
}

//----

void
CQChartsHierBubblePlot::
setValueLabel(bool b)
{
  CQChartsUtil::testAndSet(valueLabel_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsHierBubblePlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    CQChartsFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

    drawObjs();
  }
}

//---

void
CQChartsHierBubblePlot::
setColorById(bool b)
{
  CQChartsUtil::testAndSet(colorById_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsHierBubblePlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsHierPlot::addProperties();

  // options
  addProp("options", "valueLabel", "", "Show value label");
  addProp("options", "sorted"    , "", "Sort values by size");

  // coloring
  addProp("coloring", "colorById", "colorById", "Color by id");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
  addProp("text", "textVisible", "visible", "Text visible");

  addAllTextProperties("text", "text", "");
}

//------

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
currentRoot() const
{
  CQChartsHierBubbleHierNode *currentRoot = nodeData_.root;

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

CQChartsGeom::Range
CQChartsHierBubblePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsHierBubblePlot::calcRange");

  double r = 1.0;

  CQChartsGeom::Range dataRange;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

//------

void
CQChartsHierBubblePlot::
clearPlotObjects()
{
  resetNodes();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsHierBubblePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsHierBubblePlot::createObjs");

  NoUpdate noUpdate(this);

  CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  // init value sets
//initValueSets();

  //---

  if (! nodeData_.root)
    initNodes();

  //---

  th->initColorIds();

  colorNodes(nodeData_.root);

  //---

  initNodeObjs(currentRoot(), nullptr, 0, objs);

  //---

  int ig = 0, in = 0;

  for (auto &obj : objs) {
    CQChartsHierBubbleHierObj *hierObj = dynamic_cast<CQChartsHierBubbleHierObj *>(obj);
    CQChartsHierBubbleNodeObj *nodeObj = dynamic_cast<CQChartsHierBubbleNodeObj *>(obj);

    if      (hierObj) { hierObj->setInd(ig); ++ig; }
    else if (nodeObj) { nodeObj->setInd(in); ++in; }
  }

  for (auto &obj : objs) {
    CQChartsHierBubbleHierObj *hierObj = dynamic_cast<CQChartsHierBubbleHierObj *>(obj);
    CQChartsHierBubbleNodeObj *nodeObj = dynamic_cast<CQChartsHierBubbleNodeObj *>(obj);

    if      (hierObj) {
      if (hierObj->parent())
        hierObj->setIg(ColorInd(hierObj->parent()->ind(), ig));

      hierObj->setIv(ColorInd(hierObj->ind(), ig));
    }
    else if (nodeObj) {
      if (nodeObj->parent())
        nodeObj->setIg(ColorInd(nodeObj->parent()->ind(), ig));

      nodeObj->setIv(ColorInd(nodeObj->ind(), in));
    }
  }

  return true;
}

void
CQChartsHierBubblePlot::
initNodeObjs(CQChartsHierBubbleHierNode *hier, CQChartsHierBubbleHierObj *parentObj,
             int depth, PlotObjs &objs) const
{
  CQChartsHierBubbleHierObj *hierObj = nullptr;

  if (hier != nodeData_.root) {
    double r = hier->radius();

    CQChartsGeom::BBox rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    ColorInd is(hier->depth(), maxDepth() + 1);

    hierObj = new CQChartsHierBubbleHierObj(this, hier, parentObj, rect, is);

    objs.push_back(hierObj);
  }

  //---

  for (auto &hierNode : hier->getChildren()) {
    initNodeObjs(hierNode, hierObj, depth + 1, objs);
  }

  //---

  for (auto &node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    CQChartsGeom::BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    ColorInd is(node->depth(), maxDepth() + 1);

    CQChartsHierBubbleNodeObj *obj = new CQChartsHierBubbleNodeObj(this, node, parentObj, rect, is);

    objs.push_back(obj);
  }
}

void
CQChartsHierBubblePlot::
resetNodes()
{
  delete nodeData_.root;

  nodeData_.root = nullptr;
}

void
CQChartsHierBubblePlot::
initNodes() const
{
  CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  th->nodeData_.hierInd = 0;

  th->nodeData_.root = new CQChartsHierBubbleHierNode(this, 0, "<root>");

  th->nodeData_.root->setDepth(0);
  th->nodeData_.root->setHierInd(th->nodeData_.hierInd++);

  //---

  if (isHierarchical())
    loadHier();
  else
    loadFlat();

  //---

  th->nodeData_.firstHier = nodeData_.root;

  while (th->nodeData_.firstHier && th->nodeData_.firstHier->getChildren().size() == 1)
    th->nodeData_.firstHier = th->nodeData_.firstHier->getChildren()[0];

  //---

  replaceNodes();
}

void
CQChartsHierBubblePlot::
replaceNodes() const
{
  placeNodes(currentRoot());
}

void
CQChartsHierBubblePlot::
placeNodes(CQChartsHierBubbleHierNode *hier) const
{
  CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  initNodes(hier);

  //---

  hier->packNodes();

  th->placeData_.offset = CQChartsGeom::Point(hier->x(), hier->y());
  th->placeData_.scale  = (hier->radius() > 0.0 ? 1.0/hier->radius() : 1.0);

  //---

  hier->setX((hier->x() - offset().x)*scale());
  hier->setY((hier->y() - offset().y)*scale());

  hier->setRadius(1.0);

  transformNodes(hier);
}

void
CQChartsHierBubblePlot::
initNodes(CQChartsHierBubbleHierNode *hier) const
{
  for (auto &hierNode : hier->getChildren()) {
    hierNode->initRadius();

    initNodes(hierNode);
  }

  //---

  for (auto &node : hier->getNodes())
    node->initRadius();
}

void
CQChartsHierBubblePlot::
transformNodes(CQChartsHierBubbleHierNode *hier) const
{
  for (auto &hierNode : hier->getChildren()) {
    hierNode->setX((hierNode->x() - offset().x)*scale());
    hierNode->setY((hierNode->y() - offset().y)*scale());

    hierNode->setRadius(hierNode->radius()*scale());

    transformNodes(hierNode);
  }

  //---

  for (auto &node : hier->getNodes()) {
    node->setX((node->x() - offset().x)*scale());
    node->setY((node->y() - offset().y)*scale());

    node->setRadius(node->radius()*scale());
  }
}

void
CQChartsHierBubblePlot::
colorNodes(CQChartsHierBubbleHierNode *hier) const
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
colorNode(CQChartsHierBubbleNode *node) const
{
  if (! node->color().isValid()) {
    CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

    node->setColorId(th->nextColorId());
  }
}

void
CQChartsHierBubblePlot::
loadHier() const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *root) :
     plot_(plot) {
      hierStack_.push_back(root);

      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      CQChartsHierBubbleHierNode *hier = plot_->addHierNode(parentHier(), name, nameInd);

      //---

      hierStack_.push_back(hier);

      return State::OK;
    }

    State hierPostVisit(const QAbstractItemModel *, const VisitData &) override {
      hierStack_.pop_back();

      assert(! hierStack_.empty());

      return State::OK;
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      //---

      CQChartsHierBubbleNode *node = plot_->addNode(parentHier(), name, size, nameInd);

      if (node) {
        CQChartsColor color;

        if (plot_->columnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    CQChartsHierBubbleHierNode *parentHier() const {
      assert(! hierStack_.empty());

      return hierStack_.back();
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      if (plot_->nameColumns().column().isValid())
        nameInd = plot_->modelIndex(data.row, plot_->nameColumns().column(), data.parent);
      else
        nameInd = plot_->modelIndex(data.row, plot_->idColumn(), data.parent);

      bool ok;

      if (plot_->nameColumns().column().isValid())
        name = plot_->modelString(data.row, plot_->nameColumns().column(), data.parent, ok);
      else
        name = plot_->modelString(data.row, plot_->idColumn(), data.parent, ok);

      return ok;
    }

    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      if      (valueColumnType_ == ColumnType::REAL)
        size = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok);
      else if (valueColumnType_ == ColumnType::INTEGER)
        size = plot_->modelInteger(data.row, plot_->valueColumn(), data.parent, ok);
      else if (valueColumnType_ == ColumnType::STRING)
        size = 1.0;
      else
        ok = false;

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    using HierStack = std::vector<CQChartsHierBubbleHierNode *>;

    const CQChartsHierBubblePlot* plot_            { nullptr };
    ColumnType                    valueColumnType_ { ColumnType::NONE };
    HierStack                     hierStack_;
  };

  RowVisitor visitor(this, nodeData_.root);

  visitModel(visitor);
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
addHierNode(CQChartsHierBubbleHierNode *hier, const QString &name,
            const QModelIndex &nameInd) const
{
  CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsHierBubbleHierNode *hier1 = new CQChartsHierBubbleHierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(th->nodeData_.hierInd++);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return hier1;
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
addNode(CQChartsHierBubbleHierNode *hier, const QString &name, double size,
        const QModelIndex &nameInd) const
{
  CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsHierBubbleNode *node = new CQChartsHierBubbleNode(this, hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return node;
}

void
CQChartsHierBubblePlot::
loadFlat() const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsHierBubblePlot *plot) :
     plot_(plot) {
      valueColumnType_ = plot_->columnValueType(plot_->valueColumn());
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QStringList  nameStrs;
      ModelIndices nameInds;

      if (! plot_->getHierColumnNames(data.parent, data.row, plot_->nameColumns(),
                                      plot_->separator(), nameStrs, nameInds))
        return State::SKIP;

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      //---

      CQChartsHierBubbleNode *node = plot_->addNode(nameStrs, size, nameInds[0]);

      if (node) {
        CQChartsColor color;

        if (plot_->columnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      bool ok = true;

      if      (valueColumnType_ == ColumnType::REAL)
        size = plot_->modelReal(data.row, plot_->valueColumn(), data.parent, ok);
      else if (valueColumnType_ == ColumnType::INTEGER)
        size = plot_->modelInteger(data.row, plot_->valueColumn(), data.parent, ok);
      else
        ok = false;

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    const CQChartsHierBubblePlot* plot_            { nullptr };
    ColumnType                    valueColumnType_ { ColumnType::NONE };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  addExtraNodes(nodeData_.root);
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
addNode(const QStringList &nameStrs, double size, const QModelIndex &nameInd) const
{
  CQChartsHierBubblePlot *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  int depth = nameStrs.length();

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth + 1);

  //---

  CQChartsHierBubbleHierNode *parent = nodeData_.root;

  for (int i = 0; i < nameStrs.length() - 1; ++i) {
    CQChartsHierBubbleHierNode *child = childHierNode(parent, nameStrs[i]);

    if (! child) {
      // remove any existing leaf node (save size to use in new hier node)
      QModelIndex nameInd1;
      double      size1 = 0.0;

      CQChartsHierBubbleNode *node = childNode(parent, nameStrs[i]);

      if (node) {
        nameInd1 = node->ind();
        size1    = node->size();

        parent->removeNode(node);

        delete node;
      }

      //---

      child = new CQChartsHierBubbleHierNode(this, parent, nameStrs[i], nameInd1);

      child->setSize(size1);

      child->setDepth(depth);
      child->setHierInd(th->nodeData_.hierInd++);
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
      return nullptr;
    }

    //---

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    node = new CQChartsHierBubbleNode(this, parent, name, size, nameInd1);

    node->setDepth(depth);

    parent->addNode(node);
  }

  return node;
}

void
CQChartsHierBubblePlot::
addExtraNodes(CQChartsHierBubbleHierNode *hier) const
{
  if (hier->size() > 0) {
    CQChartsHierBubbleNode *node =
      new CQChartsHierBubbleNode(this, hier, "", hier->size(), hier->ind());

    QModelIndex ind1 = unnormalizeIndex(hier->ind());

    CQChartsColor color;

    if (columnColor(ind1.row(), ind1.parent(), color))
      node->setColor(color);

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
  auto addMenuAction = [&](QMenu *menu, const QString &name, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  PlotObjs objs;

  selectedPlotObjs(objs);

  menu->addSeparator();

  QAction *pushAction   = addMenuAction(menu, "Push"   , SLOT(pushSlot()));
  QAction *popAction    = addMenuAction(menu, "Pop"    , SLOT(popSlot()));
  QAction *popTopAction = addMenuAction(menu, "Pop Top", SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot() != nodeData_.root);
  popTopAction->setEnabled(currentRoot() != nodeData_.root);

  return true;
}

void
CQChartsHierBubblePlot::
pushSlot()
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  if (objs.empty()) {
    QPointF gpos = view()->menuPos();

    QPointF pos = view()->mapFromGlobal(QPoint(gpos.x(), gpos.y()));

    CQChartsGeom::Point w = pixelToWindow(CQChartsGeom::Point(pos));

    plotObjsAtPoint(w, objs);
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

    CQChartsHierBubbleNodeObj *nodeObj = dynamic_cast<CQChartsHierBubbleNodeObj *>(obj);

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

  if (root != nodeData_.root) {
    setCurrentRoot(nodeData_.root, /*update*/true);
  }
}

//------

void
CQChartsHierBubblePlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

bool
CQChartsHierBubblePlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsHierBubblePlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  drawBounds(device, currentRoot());
}

void
CQChartsHierBubblePlot::
drawBounds(CQChartsPaintDevice *device, CQChartsHierBubbleHierNode *hier) const
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  //---

  CQChartsGeom::Point p1(xc - r, yc - r);
  CQChartsGeom::Point p2(xc + r, yc + r);

  QRectF qrect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

  //---

  // draw bubble
  QColor bc = interpStrokeColor(ColorInd());

  device->setPen  (bc);
  device->setBrush(Qt::NoBrush);

  QPainterPath path;

  path.addEllipse(qrect);

  device->drawPath(path);
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                          CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                          const ColorInd &is) :
 CQChartsHierBubbleNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
  setModelInd(hier_->ind());
}

QString
CQChartsHierBubbleHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsHierBubbleNodeObj::calcId();
}

QString
CQChartsHierBubbleHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hier_->hierName()).arg(hier_->hierSize());
  return CQChartsHierBubbleNodeObj::calcTipId();
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
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsHierBubbleHierObj::
draw(CQChartsPaintDevice *device)
{
  CQChartsHierBubbleHierNode *root = hier_->parent();

  if (! root)
    root = hier_;

  //---

  double r = hier_->radius();

  CQChartsGeom::Point p1(hier_->x() - r, hier_->y() - r);
  CQChartsGeom::Point p2(hier_->x() + r, hier_->y() + r);

  QRectF qrect = CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y).qrect();

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpStrokeColor(colorInd);
  QColor fc = hier_->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  plot_->setPenBrush(pen, brush,
    plot_->isStroked(), bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
    plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw bubble
  QPainterPath path;

  path.addEllipse(qrect);

  device->drawPath(path);
}

//------

CQChartsHierBubbleNodeObj::
CQChartsHierBubbleNodeObj(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                          CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                          const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsHierBubblePlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(node_->ind());
}

QString
CQChartsHierBubbleNodeObj::
calcId() const
{
  if (node_->isFiller())
    return hierObj_->calcId();

  return QString("%1:%2:%3").arg(typeName()).arg(node_->name()).arg(node_->hierSize());
}

QString
CQChartsHierBubbleNodeObj::
calcTipId() const
{
  if (node_->isFiller())
    return hierObj_->calcTipId();

  CQChartsTableTip tableTip;

  //return QString("%1:%2").arg(name).arg(node_->hierSize());

  tableTip.addTableRow("Name", node_->hierName());
  tableTip.addTableRow("Size", node_->hierSize());

  if (plot_->colorColumn().isValid()) {
    QModelIndex ind1 = plot_->unnormalizeIndex(node_->ind());

    bool ok;

    QString colorStr = plot_->modelString(ind1.row(), plot_->colorColumn(), ind1.parent(), ok);

    tableTip.addTableRow("Color", colorStr);
  }

  //---

  plot()->addTipColumns(tableTip, node_->ind());

  //---

  return tableTip.str();
}

bool
CQChartsHierBubbleNodeObj::
inside(const CQChartsGeom::Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p,
        CQChartsGeom::Point(node_->x(), node_->y())) < node_->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleNodeObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsHierBubbleNodeObj::
draw(CQChartsPaintDevice *device)
{
  double r = node_->radius();

  CQChartsGeom::Point p1(node_->x() - r, node_->y() - r);
  CQChartsGeom::Point p2(node_->x() + r, node_->y() + r);

  QRectF  qrect = CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y).qrect();

  //---

  // check if small enough to draw as point
  QPointF pp1 = device->windowToPixel(p1.qpoint());
  QPointF pp2 = device->windowToPixel(p2.qpoint());

  double pw = std::abs(pp2.x() - pp1.x()) - 2;
  double ph = std::abs(pp2.y() - pp1.y()) - 2;

  QPointF qpoint;

  bool isPoint = (pw <= 1.5 || ph <= 1.5);

  if (isPoint)
    qpoint = QPointF((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0);

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpStrokeColor(colorInd);
  QColor fc = node_->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  if (isPoint) {
    if      (plot_->isFilled())
      plot_->setPenBrush(pen, brush,
        true, fc, plot_->fillAlpha(), 0.0, CQChartsLineDash(),
        true, fc, plot_->fillAlpha(), plot_->fillPattern());
    else if (plot_->isStroked())
      plot_->setPenBrush(pen, brush,
        true, bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
        true, bc, plot_->strokeAlpha(), CQChartsFillPattern());
  }
  else {
    plot_->setPenBrush(pen, brush,
      plot_->isStroked(), bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
      plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern());
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  // draw bubble
  device->setPen  (pen);
  device->setBrush(brush);

  if (isPoint)
    device->drawPoint(qpoint);
  else {
    QPainterPath path;

    path.addEllipse(qrect);

    device->drawPath(path);
  }

  //---

  if (isPoint)
    return;

  if (! plot_->isTextVisible())
    return;

  //---

  // get labels (name and optional size)
  QStringList strs;

  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel() && ! node_->isFiller()) {
    strs.push_back(QString("%1").arg(node_->size()));
  }

  //---

  // calc text pen
  QPen tpen;

  QColor tc = plot_->interpTextColor(colorInd);

  plot_->setPen(tpen, true, tc, plot_->textAlpha());

  plot_->updateObjPenBrushState(this, tpen, brush);

  //---

  device->save();

  //---

  // set font
  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  if (plot_->isTextScaled()) {
    // calc text size
    QFontMetricsF fm(device->font());

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i)
      tw = std::max(tw, fm.width(strs[i]));

    double th = strs.size()*fm.height();

    //---

    // calc scale factor
    QRectF prect = device->windowToPixel(qrect);

    double sx = (tw > 0 ? prect.width ()/tw : 1.0);
    double sy = (th > 0 ? prect.height()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    double fs = device->font().pointSizeF()*s;

    QFont font1 = device->font();

    font1.setPointSizeF(fs);

    device->setFont(font1);
  }

  //---

  // calc text position
  CQChartsGeom::Point pc = plot_->windowToPixel(CQChartsGeom::Point(node_->x(), node_->y()));

  //---

  // draw label
  device->setClipRect(qrect);

  CQChartsTextOptions textOptions;

  textOptions.contrast  = plot_->isTextContrast ();
  textOptions.formatted = plot_->isTextFormatted();
  textOptions.html      = plot_->isTextHtml();
//textOptions.align     = plot_->textAlign();

  textOptions = plot_->adjustTextOptions(textOptions);

  device->setPen(tpen);

  QPointF tp = pc.qpoint();

  if      (strs.size() == 1) {
    CQChartsDrawUtil::drawTextAtPoint(device, device->pixelToWindow(tp), name, textOptions);
  }
  else if (strs.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    QPointF tp1 = device->pixelToWindow(QPointF(pc.x, pc.y - th/2));
    QPointF tp2 = device->pixelToWindow(QPointF(pc.x, pc.y + th/2));

    CQChartsDrawUtil::drawTextAtPoint(device, tp1, strs[0], textOptions);
    CQChartsDrawUtil::drawTextAtPoint(device, tp2, strs[1], textOptions);
  }
  else {
    assert(false);
  }

  //---

  device->restore();
}

//------

CQChartsHierBubbleHierNode::
CQChartsHierBubbleHierNode(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
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
  if (plot_->isSorted())
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
interpColor(const CQChartsHierBubblePlot *plot, const CQChartsColor &c,
            const ColorInd &colorInd, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(plot, c, colorInd, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(plot, c, colorInd, n));

  if (colors.empty())
    return plot->interpColor(c, colorInd);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsHierBubbleNode::
CQChartsHierBubbleNode(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
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
  // area = PI*r*r; r = sqrt(area/PI)
  r_ = sqrt(hierSize()/M_PI);
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
interpColor(const CQChartsHierBubblePlot *plot, const CQChartsColor &c,
            const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpColor(c, ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}
