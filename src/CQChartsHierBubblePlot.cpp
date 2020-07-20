#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
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
create(View *view, const ModelP &model) const
{
  return new CQChartsHierBubblePlot(view, model);
}

//------

CQChartsHierBubblePlot::
CQChartsHierBubblePlot(View *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("hierbubble"), model),
 CQChartsObjShapeData<CQChartsHierBubblePlot>(this),
 CQChartsObjTextData <CQChartsHierBubblePlot>(this)
{
  NoUpdate noUpdate(this);

//setExactValue(false);

  //---

  setFillColor(Color(Color::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setTextContrast(true);
  setTextFontSize(12.0);

  setTextColor(Color(Color::Type::INTERFACE_VALUE, 1));

  setOuterMargin(PlotMargin(Length("4px"), Length("4px"), Length("4px"), Length("4px")));

  addTitle();
}

CQChartsHierBubblePlot::
~CQChartsHierBubblePlot()
{
  delete nodeData_.root;
}

//---

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
setMinSize(const OptReal &r)
{
  CQChartsUtil::testAndSet(minSize_, r, [&]() { updateRangeAndObjs(); } );
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

  addHierProperties();

  // options
  addProp("options", "valueLabel"      , "", "Show value label");
  addProp("options", "sorted"          , "", "Sort values by size");
  addProp("options", "followViewExpand", "", "Follow view expand");

  addProp("filter", "minSize", "minSize", "Min size");

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

  addTextProperties("text", "text", "",
    CQChartsTextOptions::ValueType::CONTRAST | CQChartsTextOptions::ValueType::SCALED);
}

//---

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
currentRoot() const
{
  auto *currentRoot = nodeData_.root;

  auto names = currentRootName_.split(separator(), QString::SkipEmptyParts);

  if (names.empty())
    return currentRoot;

  for (int i = 0; i < names.size(); ++i) {
    auto *hier = childHierNode(currentRoot, names[i]);

    if (! hier)
      return currentRoot;

    currentRoot = hier;
  }

  return currentRoot;
}

void
CQChartsHierBubblePlot::
setCurrentRoot(HierNode *hier, bool update)
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

  Range dataRange;

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

  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  th->clearErrors();

  //---

  // init value sets
//initValueSets();

  //---

  // check columns
  bool columnsValid = true;

  // value column required
  // name, id, color columns optional

  if (! checkColumns(nameColumns(), "Name", /*required*/true))
    columnsValid = false;

  if (! checkColumn(valueColumn(), "Value", th->valueColumnType_))
    columnsValid = false;

  if (! checkColumn(colorColumn(), "Color")) columnsValid = false;

  if (! columnsValid)
    return false;

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
    auto *hierObj = dynamic_cast<HierObj *>(obj);
    auto *nodeObj = dynamic_cast<NodeObj *>(obj);

    if      (hierObj) { hierObj->setInd(ig); ++ig; }
    else if (nodeObj) { nodeObj->setInd(in); ++in; }
  }

  for (auto &obj : objs) {
    auto *hierObj = dynamic_cast<HierObj *>(obj);
    auto *nodeObj = dynamic_cast<NodeObj *>(obj);

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
initNodeObjs(HierNode *hier, HierObj *parentObj, int depth, PlotObjs &objs) const
{
  HierObj *hierObj = nullptr;

  if (hier != nodeData_.root) {
    double r = hier->radius();

    BBox rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    ColorInd is(hier->depth(), maxDepth() + 1);

    hierObj = createHierObj(hier, parentObj, rect, is);

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

    BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    ColorInd is(node->depth(), maxDepth() + 1);

    auto *obj = createNodeObj(node, parentObj, rect, is);

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
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  th->nodeData_.hierInd = 0;

  th->nodeData_.root = new HierNode(this, 0, "<root>");

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
placeNodes(HierNode *hier) const
{
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  initNodes(hier);

  //---

  hier->packNodes();

  th->placeData_.offset = Point(hier->x(), hier->y());
  th->placeData_.scale  = (hier->radius() > 0.0 ? 1.0/hier->radius() : 1.0);

  //---

  hier->setX((hier->x() - offset().x)*scale());
  hier->setY((hier->y() - offset().y)*scale());

  hier->setRadius(1.0);

  transformNodes(hier);
}

void
CQChartsHierBubblePlot::
initNodes(HierNode *hier) const
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
transformNodes(HierNode *hier) const
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
colorNodes(HierNode *hier) const
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
    auto *th = const_cast<CQChartsHierBubblePlot *>(this);

    node->setColorId(th->nextColorId());
  }
}

void
CQChartsHierBubblePlot::
loadHier() const
{
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsHierBubblePlot;

   public:
    RowVisitor(const Plot *plot, HierNode *root) :
     plot_(plot) {
      hierStack_.push_back(root);
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      // get name and associated model index for row
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      auto *hier = plot_->addHierNode(parentHier(), name, nameInd);

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
      // get name and associated model index for row
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      if (plot_->minSize().isSet()) {
        if (size < plot_->minSize().real())
          return State::SKIP;
      }

      //---

      auto *node = plot_->addNode(parentHier(), name, size, nameInd);

      if (node) {
        Color color;

        if (plot_->colorColumnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    HierNode *parentHier() const {
      assert(! hierStack_.empty());

      return hierStack_.back();
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      auto *plot = const_cast<Plot *>(plot_);

      bool ok;

      if (plot_->nameColumns().column().isValid()) {
        ModelIndex nameColumnInd(plot, data.row, plot_->nameColumns().column(), data.parent);

        nameInd = plot_->modelIndex(nameColumnInd);

        name = plot_->modelString(nameColumnInd, ok);
      }
      else {
        ModelIndex idColumnInd(plot, data.row, plot_->idColumn(), data.parent);

        nameInd = plot_->modelIndex(idColumnInd);

        name = plot_->modelString(idColumnInd, ok);
      }

      return ok;
    }

    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      auto *plot = const_cast<Plot *>(plot_);

      ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

      if (! plot_->getValueSize(valueModelInd, size))
        return false;

      if (size <= 0.0)
        return false;

      return true;
    }

   private:
    using HierStack = std::vector<HierNode *>;

    const Plot* plot_ { nullptr };
    HierStack   hierStack_;
  };

  RowVisitor visitor(this, nodeData_.root);

  visitModel(visitor);
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
addHierNode(HierNode *hier, const QString &name, const QModelIndex &nameInd) const
{
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  auto *hier1 = new HierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(th->nodeData_.hierInd++);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return hier1;
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
addNode(HierNode *hier, const QString &name, double size, const QModelIndex &nameInd) const
{
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  auto *node = new CQChartsHierBubbleNode(this, hier, name, size, nameInd1);

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
    using Plot = CQChartsHierBubblePlot;

   public:
    RowVisitor(const Plot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get names and associated model indices for row
      QStringList   nameStrs;
      QModelIndices nameInds;

      if (! plot_->getHierColumnNames(data.parent, data.row, plot_->nameColumns(),
                                      plot_->separator(), nameStrs, nameInds))
        return State::SKIP;

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      if (plot_->minSize().isSet()) {
        if (size < plot_->minSize().real())
          return State::SKIP;
      }

      //---

      auto *node = plot_->addNode(nameStrs, size, nameInds[0]);

      if (node) {
        Color color;

        if (plot_->colorColumnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      auto *plot = const_cast<Plot *>(plot_);

      ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

      if (! plot_->getValueSize(valueModelInd, size))
        return false;

      if (size <= 0.0)
        return false;

      return true;
    }

   private:
    using ModelInds = std::vector<ModelIndex>;
    using GroupInds = std::map<int,ModelInds>;

    const Plot*       plot_ { nullptr };
    mutable GroupInds groupInds_;
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
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  int depth = nameStrs.length();

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth + 1);

  //---

  auto *parent = nodeData_.root;

  for (int i = 0; i < nameStrs.length() - 1; ++i) {
    auto *child = childHierNode(parent, nameStrs[i]);

    if (! child) {
      // remove any existing leaf node (save size to use in new hier node)
      QModelIndex nameInd1;
      double      size1 = 0.0;

      auto *node = childNode(parent, nameStrs[i]);

      if (node) {
        nameInd1 = node->ind();
        size1    = node->size();

        parent->removeNode(node);

        delete node;
      }

      //---

      child = new HierNode(this, parent, nameStrs[i], nameInd1);

      child->setSize(size1);

      child->setDepth(depth);
      child->setHierInd(th->nodeData_.hierInd++);
    }

    parent = child;
  }

  //---

  QString name = nameStrs[nameStrs.length() - 1];

  auto *node = childNode(parent, name);

  if (! node) {
    // use hier node if already created
    auto *child = childHierNode(parent, name);

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
addExtraNodes(HierNode *hier) const
{
  if (hier->size() > 0) {
    auto *node = new CQChartsHierBubbleNode(this, hier, "", hier->size(), hier->ind());

    QModelIndex ind1 = unnormalizeIndex(hier->ind());

    Color color;

    if (colorColumnColor(ind1.row(), ind1.parent(), color))
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
childHierNode(HierNode *parent, const QString &name) const
{
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
childNode(HierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

bool
CQChartsHierBubblePlot::
getValueSize(const ModelIndex &ind, double &size) const
{
  auto addDataError = [&](const QString &msg) {
    const_cast<CQChartsHierBubblePlot *>(this)->addDataError(ind, msg);
    return false;
  };

  size = 1.0;

  if (! ind.isValid())
    return false;

  bool ok = true;

  if      (valueColumnType() == ColumnType::REAL)
    size = modelReal(ind, ok);
  else if (valueColumnType() == ColumnType::INTEGER)
    size = (double) modelInteger(ind, ok);
  else if (valueColumnType() == ColumnType::STRING)
    size = 1.0;
  else
    ok = false;

  if (! ok)
    return addDataError("Invalid numeric value");

  if (size < 0.0)
    return addDataError("Non-positive value");

  return true;
}

//------

bool
CQChartsHierBubblePlot::
addMenuItems(QMenu *menu)
{
  auto addMenuAction = [&](QMenu *menu, const QString &name, const char *slot) {
    auto *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slot);

    menu->addAction(action);

    return action;
  };

  //---

  PlotObjs objs;

  selectedPlotObjs(objs);

  menu->addSeparator();

  auto *pushAction   = addMenuAction(menu, "Push"   , SLOT(pushSlot()));
  auto *popAction    = addMenuAction(menu, "Pop"    , SLOT(popSlot()));
  auto *popTopAction = addMenuAction(menu, "Pop Top", SLOT(popTopSlot()));

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
    auto gpos = view()->menuPos();
    auto pos  = view()->mapFromGlobal(QPoint(gpos.x, gpos.y));

    auto w = pixelToWindow(Point(pos));

    plotObjsAtPoint(w, objs);
  }

  if (objs.empty())
    return;

  for (const auto &obj : objs) {
    auto *hierObj = dynamic_cast<HierObj *>(obj);

    if (hierObj) {
      auto *hnode = hierObj->hierNode();

      setCurrentRoot(hnode, /*update*/true);

      break;
    }

    auto *nodeObj = dynamic_cast<CQChartsHierBubbleNodeObj *>(obj);

    if (nodeObj) {
      auto *node = nodeObj->node();

      auto *hnode = node->parent();

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
  auto *root = currentRoot();

  if (root && root->parent()) {
    setCurrentRoot(root->parent(), /*update*/true);
  }
}

void
CQChartsHierBubblePlot::
popTopSlot()
{
  auto *root = currentRoot();

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

void
CQChartsHierBubblePlot::
followViewExpandChanged()
{
  if (isFollowViewExpand())
    modelViewExpansionChanged();
  else
    resetNodeExpansion();

  drawObjs();
}

void
CQChartsHierBubblePlot::
modelViewExpansionChanged()
{
  if (! isFollowViewExpand())
    return;

  std::set<QModelIndex> indSet;

  expandedModelIndices(indSet);

  for (auto &hierNode : root()->getChildren())
    setNodeExpansion(hierNode, indSet);

  drawObjs();
}

void
CQChartsHierBubblePlot::
setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet)
{
  hierNode->setExpanded(indSet.find(hierNode->ind()) != indSet.end());

  for (auto &hierNode1 : hierNode->getChildren())
    setNodeExpansion(hierNode1, indSet);
}

void
CQChartsHierBubblePlot::
resetNodeExpansion()
{
  for (auto &hierNode : root()->getChildren())
    resetNodeExpansion(hierNode);
}

void
CQChartsHierBubblePlot::
resetNodeExpansion(HierNode *hierNode)
{
  hierNode->setExpanded(true);

  for (auto &hierNode1 : hierNode->getChildren())
    resetNodeExpansion(hierNode1);
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
drawBounds(PaintDevice *device, HierNode *hier) const
{
  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  //---

  Point p1(xc - r, yc - r);
  Point p2(xc + r, yc + r);

  BBox bbox(p1, p2);

  //---

  // draw bubble
  CQChartsPenBrush penBrush;

  QColor bc = interpStrokeColor(ColorInd());

  setPenBrush(penBrush, CQChartsPenData(true, bc), CQChartsBrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawEllipse(bbox);
}

//---

CQChartsHierBubbleHierObj *
CQChartsHierBubblePlot::
createHierObj(HierNode *hier, HierObj *hierObj, const BBox &rect, const ColorInd &is) const
{
  return new HierObj(this, hier, hierObj, rect, is);
}

CQChartsHierBubbleNodeObj *
CQChartsHierBubblePlot::
createNodeObj(Node *node, HierObj *hierObj, const BBox &rect, const ColorInd &is) const
{
  return new NodeObj(this, node, hierObj, rect, is);
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                          const BBox &rect, const ColorInd &is) :
 CQChartsHierBubbleNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
  setModelInd(hier_->ind());
}

QString
CQChartsHierBubbleHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hierNode()->name()).arg(hierNode()->hierSize());
  return CQChartsHierBubbleNodeObj::calcId();
}

QString
CQChartsHierBubbleHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hierNode()->hierName()).arg(hierNode()->hierSize());
  return CQChartsHierBubbleNodeObj::calcTipId();
}

bool
CQChartsHierBubbleHierObj::
inside(const Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p, Point(hierNode()->x(), hierNode()->y())) < this->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleHierObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsHierBubbleHierObj::
draw(PaintDevice *device)
{
  auto *pnode = node()->parent();

  if (pnode && ! pnode->isHierExpanded())
    return;

  //---

  if (! hierNode()->isExpanded())
    return CQChartsHierBubbleNodeObj::draw(device);

  //---

  auto *root = hierNode()->parent();

  if (! root)
    root = hierNode();

  //---

  double r = this->radius();

  Point p1(hierNode()->x() - r, hierNode()->y() - r);
  Point p2(hierNode()->x() + r, hierNode()->y() + r);

  BBox bbox(p1, p2);

  //---

  // calc stroke and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw bubble
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawEllipse(bbox);

  device->resetColorNames();
}

void
CQChartsHierBubbleHierObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  if (! hierNode()->isExpanded())
    return CQChartsHierBubbleNodeObj::calcPenBrush(penBrush, updateState);

  //---

  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = hierNode()->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (plot_->isStroked(), bc, plot_->strokeAlpha(),
                      plot_->strokeWidth(), plot_->strokeDash()),
    CQChartsBrushData(plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsHierBubbleHierObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  CQChartsHierBubbleNodeObj::writeScriptData(device);
}

//------

CQChartsHierBubbleNodeObj::
CQChartsHierBubbleNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                          const BBox &rect, const ColorInd &is) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(node_->ind());
}

QString
CQChartsHierBubbleNodeObj::
calcId() const
{
  if (node()->isFiller())
    return hierObj_->calcId();

  return QString("%1:%2:%3").arg(typeName()).arg(node()->name()).arg(node()->hierSize());
}

QString
CQChartsHierBubbleNodeObj::
calcTipId() const
{
  if (node()->isFiller())
    return hierObj_->calcTipId();

  CQChartsTableTip tableTip;

  //return QString("%1:%2").arg(name).arg(node()->hierSize());

  tableTip.addTableRow("Name", node()->hierName());
  tableTip.addTableRow("Size", node()->hierSize());

  if (plot_->colorColumn().isValid()) {
    QModelIndex ind1 = plot_->unnormalizeIndex(node()->ind());

    ModelIndex colorColumnInd(plot(), ind1.row(), plot_->colorColumn(), ind1.parent());

    bool ok;

    QString colorStr = plot_->modelString(colorColumnInd, ok);

    tableTip.addTableRow("Color", colorStr);
  }

  //---

  const auto &ind = node_->ind();

  plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

bool
CQChartsHierBubbleNodeObj::
inside(const Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p, Point(node()->x(), node()->y())) < this->radius())
    return true;

  return false;
}

void
CQChartsHierBubbleNodeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsHierBubbleNodeObj::
draw(PaintDevice *device)
{
  auto *pnode = node()->parent();

  if (pnode && ! pnode->isHierExpanded())
    return;

  //---

  double r = this->radius();

  Point p1(node()->x() - r, node()->y() - r);
  Point p2(node()->x() + r, node()->y() + r);

  BBox bbox(p1, p2);

  //---

  bool isPoint = this->isPoint();

  Point point;

  if (isPoint)
    point = Point((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0);

  //---

  // calc stroke and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw bubble
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if (isPoint)
    device->drawPoint(point);
  else
    device->drawEllipse(bbox);

  device->resetColorNames();

  //---

  if (isPoint)
    return;

  //---

  if (plot_->isTextVisible())
    drawText(device, bbox);
}

void
CQChartsHierBubbleNodeObj::
drawText(PaintDevice *device, const BBox &bbox)
{
  // get labels (name and optional size)
  QStringList strs;

  QString name = (! node()->isFiller() ? node()->name() : node()->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel() && ! node()->isFiller()) {
    strs.push_back(QString("%1").arg(node()->size()));
  }

  //---

  // calc text pen
  ColorInd colorInd = calcColorInd();

  CQChartsPenBrush tPenBrush;

  auto tc = plot_->interpTextColor(colorInd);

  plot_->setPen(tPenBrush, CQChartsPenData(true, tc, plot_->textAlpha()));

  plot_->updateObjPenBrushState(this, tPenBrush);

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
    auto pbbox = device->windowToPixel(bbox);

    double sx = (tw > 0 ? pbbox.getWidth ()/tw : 1.0);
    double sy = (th > 0 ? pbbox.getHeight()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    device->setFont(CQChartsUtil::scaleFontSize(device->font(), s));
  }

  //---

  // calc text position
  auto pc = plot_->windowToPixel(Point(node()->x(), node()->y()));

  //---

  // draw label
  device->setClipRect(bbox);

  // angle and align not supported (always 0 and centered)
  // text is pre-scaled if needed (formatted and html not suppoted as changes scale calc)
  CQChartsTextOptions textOptions;

  textOptions.contrast      = plot_->isTextContrast ();
  textOptions.contrastAlpha = plot_->textContrastAlpha();
  textOptions.clipLength    = plot_->textClipLength();

  textOptions = plot_->adjustTextOptions(textOptions);

  device->setPen(tPenBrush.pen);

  auto tp = pc;

  if      (strs.size() == 1) {
    CQChartsDrawUtil::drawTextAtPoint(device, device->pixelToWindow(tp), name, textOptions);
  }
  else if (strs.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    auto tp1 = device->pixelToWindow(Point(pc.x, pc.y - th/2));
    auto tp2 = device->pixelToWindow(Point(pc.x, pc.y + th/2));

    CQChartsDrawUtil::drawTextAtPoint(device, tp1, strs[0], textOptions);
    CQChartsDrawUtil::drawTextAtPoint(device, tp2, strs[1], textOptions);
  }
  else {
    assert(false);
  }

  //---

  device->restore();
}

bool
CQChartsHierBubbleNodeObj::
isPoint() const
{
  // check if small enough to draw as point
  double r = this->radius();

  double pw = plot()->windowToPixelWidth (2*r) - 2;
  double ph = plot()->windowToPixelHeight(2*r) - 2;

  return (pw <= 1.5 || ph <= 1.5);
}

void
CQChartsHierBubbleNodeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = node()->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  bool isPoint = this->isPoint();

  if (isPoint) {
    if      (plot_->isFilled())
      plot_->setPenBrush(penBrush,
        CQChartsPenData  (true, fc, plot_->fillAlpha()),
        CQChartsBrushData(true, fc, plot_->fillAlpha(), plot_->fillPattern()));
    else if (plot_->isStroked())
      plot_->setPenBrush(penBrush,
        CQChartsPenData  (true, bc, plot_->strokeAlpha(),
                          plot_->strokeWidth(), plot_->strokeDash()),
        CQChartsBrushData(true, bc, plot_->strokeAlpha()));
  }
  else {
    plot_->setPenBrush(penBrush,
      CQChartsPenData  (plot_->isStroked(), bc, plot_->strokeAlpha(),
                        plot_->strokeWidth(), plot_->strokeDash()),
      CQChartsBrushData(plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern()));
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsHierBubbleNodeObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.name = \"" << node()->hierName().toStdString() << "\";\n";
  os << "  this.size = " << node()->hierSize() << ";\n";
}

//------

CQChartsHierBubbleHierNode::
CQChartsHierBubbleHierNode(const Plot *plot, HierNode *parent, const QString &name,
                           const QModelIndex &ind) :
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

bool
CQChartsHierBubbleHierNode::
isHierExpanded() const
{
  if (plot_->currentRoot() == this)
    return true;

  if (! isExpanded())
    return false;

  if (parent() && ! parent()->isHierExpanded())
    return false;

  return true;
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
interpColor(const Plot *plot, const CQChartsColor &c, const ColorInd &colorInd, int n) const
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
CQChartsHierBubbleNode(const Plot *plot, HierNode *parent, const QString &name,
                       double size, const QModelIndex &ind) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind)
{
  r_ = CQChartsUtil::areaToRadius(size_);
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
  r_ = CQChartsUtil::areaToRadius(hierSize());
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
interpColor(const Plot *plot, const CQChartsColor &c, const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpColor(c, ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}
