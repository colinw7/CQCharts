#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsTip.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

//---

CQChartsSunburstPlotType::
CQChartsSunburstPlotType()
{
}

void
CQChartsSunburstPlotType::
addParameters()
{
  CQChartsHierPlotType::addParameters();
}

QString
CQChartsSunburstPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Sunburst Plot").
    h3("Summary").
     p("Draw hierarchical data as segments of concentric circles.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/sunburst.png"));
}

CQChartsPlot *
CQChartsSunburstPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsSunburstPlot(view, model);
}

//---

CQChartsSunburstPlot::
CQChartsSunburstPlot(View *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("sunburst"), model),
 CQChartsObjShapeData<CQChartsSunburstPlot>(this),
 CQChartsObjTextData <CQChartsSunburstPlot>(this)
{
}

CQChartsSunburstPlot::
~CQChartsSunburstPlot()
{
  term();
}

//---

void
CQChartsSunburstPlot::
init()
{
  CQChartsHierPlot::init();

  //---

  NoUpdate noUpdate(this);

  setFillColor(Color(Color::Type::PALETTE));

  setFilled (true);
  setStroked(true);

  setTextFontSize(8.0);

  setTextColor(Color(Color::Type::INTERFACE_VALUE, 1));

  setOuterMargin(PlotMargin(Length("4px"), Length("4px"), Length("4px"), Length("4px")));

  // addKey() // TODO

  addTitle();
}

void
CQChartsSunburstPlot::
term()
{
  resetRoots();
}

//---

void
CQChartsSunburstPlot::
resetRoots()
{
  for (auto &root : roots_)
    delete root;

  roots_.clear();
}

//----

void
CQChartsSunburstPlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setOuterRadius(double r)
{
  CQChartsUtil::testAndSet(outerRadius_, r, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setStartAngle(const Angle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setMultiRoot(bool b)
{
  CQChartsUtil::testAndSet(multiRoot_, b, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setSortType(const SortType &t)
{
  CQChartsUtil::testAndSet(sortType_, t, [&]() { resetRoots(); updateObjs(); } );
}

void
CQChartsSunburstPlot::
setClipText(bool b)
{
  CQChartsUtil::testAndSet(clipText_, b, [&]() { drawObjs(); } );
}

//---

bool
CQChartsSunburstPlot::
isRoot(const HierNode *node) const
{
  auto *root = currentRoot();

  if (root) {
    return (node == root);
  }
  else {
    for (const auto &root : roots_) {
      if (node == root)
        return true;
    }

    return false;
  }
}

//----

void
CQChartsSunburstPlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    auto f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

    drawObjs();
  }
}

//---

void
CQChartsSunburstPlot::
setColorById(bool b)
{
  CQChartsUtil::testAndSet(colorById_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsSunburstPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addHierProperties();

  // options
  addProp("options", "innerRadius"     , "", "Inner radius")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "outerRadius"     , "", "Outer radius")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("options", "startAngle"      , "", "Angle for first segment");
  addProp("options", "multiRoot"       , "", "Support multiple roots");
  addProp("options", "followViewExpand", "", "Follow view expand");
  addProp("options", "sortType"        , "", "Sort type");

  // coloring
  addProp("coloring", "colorById", "colorById", "Color by id");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
  addProp("text", "textVisible", "visible", "Text label visible");
  addProp("text", "clipText"   , "clip"   , "Text is clipped");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // color map
  addColorMapProperties();
}

//------

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
currentRoot() const
{
  HierNode *currentRoot = nullptr;

  auto names = currentRootName_.split('@', QString::SkipEmptyParts);

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
CQChartsSunburstPlot::
setCurrentRoot(HierNode *hier, bool update)
{
  if (hier)
    currentRootName_ = hier->hierName('@');
  else
    currentRootName_ = "";

  if (update) {
    replaceRoots();

    updateObjs();
  }
}

CQChartsGeom::Range
CQChartsSunburstPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSunburstPlot::calcRange");

  Range dataRange;

  double r = 1.0;

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
CQChartsSunburstPlot::
clearPlotObjects()
{
  resetRoots();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsSunburstPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsSunburstPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsSunburstPlot *>(this);

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

  if (roots_.empty())
    th->initRoots();

  //---

  th->initColorIds();

  if (currentRoot()) {
    colorNodes(currentRoot());
  }
  else {
    for (auto &root : roots_)
      colorNodes(root);
  }

  //---

  int nr = roots_.size();

  bool isUnnamedRoot = (nr == 1 && roots_[0]->name() == "");

  if (currentRoot()) {
    ColorInd ci(0, 1);

    if (! isUnnamedRoot || roots_[0] != currentRoot())
      addPlotObj(currentRoot(), objs, ci);

    addPlotObjs(currentRoot(), objs, ci);
  }
  else {
    int ir = 0;

    for (auto &root : roots_) {
      ColorInd ci(ir, nr);

      if (! isUnnamedRoot)
        addPlotObj(root, objs, ci);

      addPlotObjs(root, objs, ci);

      ++ir;
    }
  }

  //---

  int in = 0;

  for (auto &obj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(obj);

    if (nodeObj) { nodeObj->setInd(in); ++in; }
  }

  for (auto &obj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(obj);

    if (nodeObj)
      nodeObj->setIv(ColorInd(nodeObj->ind(), in));
  }

  return true;
}

void
CQChartsSunburstPlot::
initRoots()
{
  RootNode *root = nullptr;

  if (! isMultiRoot())
    root = createRootNode();

  if (isHierarchical())
    loadHier(root);
  else
    loadFlat(root);

  //---

  if (isFollowViewExpand()) {
    std::set<QModelIndex> indSet;

    expandedModelIndices(indSet);

    for (const auto &root : roots_) {
      for (auto &hierNode : root->getChildren())
        setNodeExpansion(hierNode, indSet);
    }
  }

  //---

  replaceRoots();
}

void
CQChartsSunburstPlot::
replaceRoots() const
{
  double ri = std::max(innerRadius(), 0.0);
  double ro = CMathUtil::clamp(outerRadius(), ri, 1.0);

  auto a = startAngle();

  if (currentRoot()) {
    Angle da(360.0);

    currentRoot()->setPosition(0.0, a, ri, da);

    currentRoot()->packNodes(currentRoot(), ri, ro, 0.0, a, da,
                             (CQChartsSunburstNode::SortType) sortType());
  }
  else {
    Angle da(! roots_.empty() ? 360.0/roots_.size() : 0.0);

    for (auto &root : roots_) {
      root->setPosition(0.0, a, ri, da);

      root->packNodes(ri, ro, 0.0, a, da, (CQChartsSunburstNode::SortType) sortType());

      a += da;
    }
  }
}

void
CQChartsSunburstPlot::
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
CQChartsSunburstPlot::
colorNode(Node *node) const
{
  if (! node->color().isValid()) {
    auto *th = const_cast<CQChartsSunburstPlot *>(this);

    node->setColorId(th->nextColorId());
  }
}

void
CQChartsSunburstPlot::
loadHier(HierNode *root) const
{
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsSunburstPlot;

   public:
    RowVisitor(const Plot *plot, HierNode *root) :
     plot_(plot) {
      hierStack_.push_back(root);
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
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
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      auto *plot = const_cast<Plot *>(plot_);

      double size = 1.0;

      QModelIndex valueInd;

      if (plot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

        valueInd = plot_->modelIndex(valueModelInd);

        if (! plot_->getValueSize(valueModelInd, size))
          return State::SKIP;

        if (size == 0.0)
          return State::SKIP;
      }

      //---

      (void) plot_->hierAddNode(parentHier(), name, size, nameInd, valueInd);

      return State::OK;
    }

   private:
    HierNode *parentHier() const {
      assert(! hierStack_.empty());

      return hierStack_.back();
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      auto *plot = const_cast<Plot *>(plot_);

      ModelIndex nameModelInd(plot, data.row, plot_->nameColumns().column(), data.parent);

      nameInd = plot_->modelIndex(nameModelInd);

      bool ok;

      name = plot_->modelString(nameModelInd, ok);

      return ok;
    }

   private:
    using HierStack = std::vector<HierNode *>;

    const Plot* plot_ { nullptr };
    HierStack   hierStack_;
  };

  RowVisitor visitor(this, root);

  visitModel(visitor);
}

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
addHierNode(HierNode *hier, const QString &name, const QModelIndex &nameInd) const
{
  auto nameInd1 = normalizeIndex(nameInd);

  auto *hier1 = new HierNode(this, hier, name);

  hier1->setInd(nameInd1);

  return hier1;
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
hierAddNode(HierNode *hier, const QString &name, double size,
            const QModelIndex &nameInd, const QModelIndex &valueInd) const
{
  auto *node = new Node(this, hier, name);

  node->setSize(size);

  if (valueInd.isValid()) {
    auto valueInd1 = normalizeIndex(valueInd);

    node->setInd(valueInd1);
  }
  else {
    auto nameInd1 = normalizeIndex(nameInd);

    node->setInd(nameInd1);
  }

  hier->addNode(node);

  return node;
}

void
CQChartsSunburstPlot::
loadFlat(HierNode *root) const
{
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsSunburstPlot;

   public:
    RowVisitor(const Plot *plot, HierNode *root) :
     plot_(plot), root_(root) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get hier names from name columns
      QStringList   nameStrs;
      QModelIndices nameInds;

      if (! plot_->getHierColumnNames(data.parent, data.row, plot_->nameColumns(),
                                      plot_->separator(), nameStrs, nameInds))
        return State::SKIP;

      auto nameInd1 = plot_->normalizeIndex(nameInds[0]);

      //---

      auto *plot = const_cast<Plot *>(plot_);

      double size = 1.0;

      QModelIndex valueInd;

      if (plot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

        valueInd = plot_->modelIndex(valueModelInd);

        if (! plot_->getValueSize(valueModelInd, size))
          return State::SKIP;

        if (size == 0.0)
          return State::SKIP;
      }

      //---

      auto *node = plot_->flatAddNode(root_, nameStrs, size, nameInd1, valueInd);

      if (node && plot_->colorColumn().isValid()) {
        Color color;

        ModelIndex colorInd(plot, data.row, plot_->colorColumn(), data.parent);

        if (plot_->modelIndexColor(colorInd, color))
          node->setColor(color);
      }

      return State::OK;
    }

   private:
    const Plot* plot_ { nullptr };
    HierNode*   root_ { nullptr };
  };

  RowVisitor visitor(this, root);

  visitModel(visitor);

  //---

  for (auto &root : roots_)
    addExtraNodes(root);
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
flatAddNode(HierNode *root, const QStringList &nameStrs, double size,
            const QModelIndex &nameInd, const QModelIndex &valueInd) const
{
  auto *th = const_cast<CQChartsSunburstPlot *>(this);

  auto *parent = root;

  for (int i = 0; i < nameStrs.length() - 1; ++i) {
    HierNode *child = nullptr;

    if (i == 0 && isMultiRoot()) {
      auto *root = rootNode(nameStrs[i]);

      if (! root) {
        root = th->createRootNode(nameStrs[i]);

        root->setInd(nameInd);
      }

      child = root;
    }
    else {
      child = childHierNode(parent, nameStrs[i]);

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

        child = new HierNode(this, parent, nameStrs[i]);

        child->setSize(size1);

        child->setInd(nameInd1);
      }
    }

    parent = child;
  }

  //---

  auto nodeName = nameStrs[nameStrs.length() - 1];

  auto *node = childNode(parent, nodeName);

  if (! node) {
    // use hier node if already created
    auto *child = childHierNode(parent, nodeName);

    if (child) {
      child->setSize(size);
      return nullptr;
    }

    //---

    node = new Node(this, parent, nodeName);

    node->setSize(size);

    if (valueInd.isValid()) {
      auto valueInd1 = normalizeIndex(valueInd);

      node->setInd(valueInd1);
    }
    else
      node->setInd(nameInd);

    parent->addNode(node);
  }
  else {
    // update size
    node->setSize(node->size() + size);
  }

  return node;
}

void
CQChartsSunburstPlot::
addExtraNodes(HierNode *hier) const
{
  if (hier->size() > 0) {
    auto *node = new Node(this, hier, "");

    auto ind1 = unnormalizeIndex(hier->ind());

    Color color;

    if (colorColumnColor(ind1.row(), ind1.parent(), color))
      node->setColor(color);

    node->setSize(hier->size());
    node->setInd (hier->ind());

    node->setFiller(true);

    hier->addNode(node);

    hier->setSize(0.0);
  }

  for (const auto &child : hier->getChildren())
    addExtraNodes(child);
}

CQChartsSunburstRootNode *
CQChartsSunburstPlot::
createRootNode(const QString &name)
{
  auto *root = new RootNode(this, name);

  roots_.push_back(root);

  return root;
}

CQChartsSunburstRootNode *
CQChartsSunburstPlot::
rootNode(const QString &name) const
{
  for (const auto &root : roots_)
    if (root->name() == name)
      return root;

  return nullptr;
}

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
childHierNode(HierNode *parent, const QString &name) const
{
  if (! parent) {
    for (const auto &root : roots_) {
      auto *hier = childHierNode(root, name);
      if (hier) return hier;
    }

    return nullptr;
  }

  //--

  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
childNode(HierNode *parent, const QString &name) const
{
  assert(parent);

  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

bool
CQChartsSunburstPlot::
getValueSize(const ModelIndex &ind, double &size) const
{
  auto addDataError = [&](const QString &msg) {
    const_cast<CQChartsSunburstPlot *>(this)->addDataError(ind, msg);
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
  else
    ok = false;

  if (! ok)
    return addDataError("Invalid numeric value");

  if (size < 0.0)
    return addDataError("Non-positive value");

  return true;
}

//------

void
CQChartsSunburstPlot::
addPlotObjs(HierNode *hier, PlotObjs &objs, const ColorInd &ir) const
{
  for (auto &node : hier->getNodes()) {
    addPlotObj(node, objs, ir);
  }

  for (auto &hierNode : hier->getChildren()) {
    addPlotObj(hierNode, objs, ir);

    addPlotObjs(hierNode, objs, ir);
  }
}

void
CQChartsSunburstPlot::
addPlotObj(Node *node, PlotObjs &objs, const ColorInd &ir) const
{
  double r1 = node->r();
  double r2 = r1 + node->dr();

  BBox bbox(-r2, -r2, r2, r2);

  auto *obj = createNodeObj(bbox, node);

  obj->setIs(ir);

  node->setObj(obj);

  objs.push_back(obj);
}

//------

bool
CQChartsSunburstPlot::
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
  popAction   ->setEnabled(currentRoot() != nullptr);
  popTopAction->setEnabled(currentRoot() != nullptr);

  return true;
}

void
CQChartsSunburstPlot::
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
    auto *sobj = dynamic_cast<NodeObj *>(obj);
    if (! sobj) continue;

    auto *node = sobj->node();

    auto *hnode = dynamic_cast<HierNode *>(node);

    if (! hnode && node)
      hnode = node->parent();

    if (hnode) {
      setCurrentRoot(hnode, /*update*/true);

      break;
    }
  }
}

void
CQChartsSunburstPlot::
popSlot()
{
  auto *root = currentRoot();

  if (root && root->parent()) {
    setCurrentRoot(root->parent(), /*update*/true);
  }
}

void
CQChartsSunburstPlot::
popTopSlot()
{
  auto *root = currentRoot();

  if (root) {
    setCurrentRoot(nullptr, /*update*/true);
  }
}

//------

void
CQChartsSunburstPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

void
CQChartsSunburstPlot::
followViewExpandChanged()
{
  if (isFollowViewExpand())
    modelViewExpansionChanged();
  else
    resetNodeExpansion();

  drawObjs();
}

void
CQChartsSunburstPlot::
modelViewExpansionChanged()
{
  if (! isFollowViewExpand())
    return;

  resetRoots();

  updateObjs();
}

void
CQChartsSunburstPlot::
setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet)
{
  hierNode->setExpanded(indSet.find(hierNode->ind()) != indSet.end());

  for (auto &hierNode1 : hierNode->getChildren())
    setNodeExpansion(hierNode1, indSet);
}

void
CQChartsSunburstPlot::
resetNodeExpansion()
{
  for (const auto &root : roots_) {
    for (auto &hierNode : root->getChildren())
      resetNodeExpansion(hierNode);
  }
}

void
CQChartsSunburstPlot::
resetNodeExpansion(HierNode *hierNode)
{
  hierNode->setExpanded(true);

  for (auto &hierNode1 : hierNode->getChildren())
    resetNodeExpansion(hierNode1);
}

//------

#if 0
void
CQChartsSunburstPlot::
drawNodes(CQChartsPaintDevice *device, HierNode *hier) const
{
  for (auto &node : hier->getNodes())
    drawNode(device, nullptr, node);

  //------

  for (auto &hierNode : hier->getChildren()) {
    drawNode(device, nullptr, hierNode);

    drawNodes(device, hierNode);
  }
}
#endif

void
CQChartsSunburstPlot::
drawNode(CQChartsPaintDevice *device, NodeObj *nodeObj, Node *node) const
{
  if (! node->placed())
    return;

  auto *root = dynamic_cast<RootNode *>(node);
//auto *hier = dynamic_cast<HierNode *>(node);

  //---

  double xc = 0.0;
  double yc = 0.0;

  double r1, r2;

  if (root) {
    r1 = 0.0;
    r2 = std::max(innerRadius(), 0.0);
  }
  else {
    r1 = node->r();
    r2 = r1 + node->dr();
  }

  auto p11 = Point(xc - r1, yc - r1);
  auto p21 = Point(xc + r1, yc + r1);
  auto p12 = Point(xc - r2, yc - r2);
  auto p22 = Point(xc + r2, yc + r2);

  BBox ibbox(p11, p21);
  BBox obbox(p12, p22);

  double a1 = node->a().value();
  double da = node->da().value();
  double a2 = a1 + da;

  //---

  // calc stroke and brush
  auto colorInd = nodeObj->calcColorInd();

  PenBrush penBrush;

  auto bc = interpStrokeColor(colorInd);
  auto fc = node->interpColor(this, fillColor(), colorInd, numColorIds());

  setPenBrush(penBrush,
    PenData  (isStroked(), bc, strokeAlpha(), strokeWidth(), strokeDash()),
    BrushData(isFilled(), fc, fillAlpha(), fillPattern()));

  if (nodeObj)
    updateObjPenBrushState(nodeObj, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  QPainterPath path;

  bool isCircle = (std::abs(da) > 360.0 || CMathUtil::realEq(std::abs(da), 360.0));

  if (isCircle) {
    if (ibbox.getWidth() > 0.0)
      device->drawEllipse(ibbox);

    if (obbox.getWidth() > 0.0)
      device->drawEllipse(obbox);
  }
  else {
    // if has non-zero inner radius draw arc segment
    if      (ibbox.getWidth() > 0.0) {
      CQChartsDrawUtil::arcSegmentPath(path, ibbox, obbox, Angle(a1), Angle(da));

      device->drawPath(path);
    }
    // draw pie slice
    else if (obbox.getWidth() > 0.0) {
      CQChartsDrawUtil::arcPath(path, obbox, Angle(a1), Angle(da));

      device->drawPath(path);
    }
  }

  charts()->setContrastColor(fc);

  //---

  if (isTextVisible()) {
    // calc text pen
    PenBrush tPenBrush;

    auto tc = interpTextColor(colorInd);

    setPen(tPenBrush, PenData(true, tc, textAlpha()));

    if (nodeObj)
      updateObjPenBrushState(nodeObj, tPenBrush);

    //---

    // set font
    view()->setPlotPainterFont(this, device, textFont());

    //---

    // check if text visible (see treemap)
    if (! isCircle) {
      double c1 = std::cos(CMathUtil::Deg2Rad(a1));
      double s1 = std::sin(CMathUtil::Deg2Rad(a1));
      double c2 = std::cos(CMathUtil::Deg2Rad(a2));
      double s2 = std::sin(CMathUtil::Deg2Rad(a2));

      Point pw1(r2*c1, r2*s1);
      Point pw2(r2*c2, r2*s2);

      auto pp1 = windowToPixel(pw1);
      auto pp2 = windowToPixel(pw2);

      double d = std::hypot(pp2.x - pp1.x, pp2.y - pp1.y);

      if (d < 1.5) // length less than 1.5 pixels
        return;
    }

    //---

    if (! isCircle) {
      device->save();

      if (isClipText())
        device->setClipPath(path);
    }

    //---

    // draw node label
    double ta, c, s;

    if (isCircle) {
      ta = 0.0;
      c  = 1.0;
      s  = 0.0;
    }
    else {
      ta = a1 + da/2.0;
      c  = std::cos(CMathUtil::Deg2Rad(ta));
      s  = std::sin(CMathUtil::Deg2Rad(ta));
    }

    double tx, ty;

    if (isCircle && CMathUtil::isZero(r1)) {
      tx = 0.0;
      ty = 0.0;
    }
    else {
      double r3 = CMathUtil::avg(r1, r2);

      tx = r3*c;
      ty = r3*s;
    }

    device->setPen(tPenBrush.pen);

    Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

    Point pt(tx, ty);

    auto name = (! node->isFiller() ? node->name() : node->parent()->name());

    double ta1 = (c >= 0 ? ta : ta - 180);

    // only contrast support (custom align and angle)
    CQChartsTextOptions options;

    options.angle         = Angle(ta1);
    options.align         = align;
    options.contrast      = isTextContrast();
    options.contrastAlpha = textContrastAlpha();
    options.clipLength    = lengthPixelWidth(textClipLength());
    options.clipElide     = textClipElide();

    CQChartsDrawUtil::drawTextAtPoint(device, pt, name, options, /*centered*/true);

  //CQChartsRotatedText::draw(device, pt, name, options);

    //---

    if (! isCircle)
      device->restore();
  }
}

//---

CQChartsSunburstNodeObj *
CQChartsSunburstPlot::
createNodeObj(const BBox &rect, Node *node) const
{
  return new NodeObj(this, rect, node);
}

//------

CQChartsSunburstNodeObj::
CQChartsSunburstNodeObj(const Plot *plot, const BBox &rect, Node *node) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect), plot_(plot), node_(node)
{
  if (node_->ind().isValid())
    setModelInd(node_->ind());
}

QString
CQChartsSunburstNodeObj::
calcId() const
{
  auto name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  return QString("%1:%2:%3").arg(typeName()).arg(name).arg(node_->hierSize());
}

QString
CQChartsSunburstNodeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  auto name = (! node_->isFiller() ? node_->hierName() : node_->parent()->hierName());

  //return QString("%1:%2").arg(name).arg(node_->hierSize());

  tableTip.addTableRow("Name", name);
  tableTip.addTableRow("Size", node_->hierSize());

  if (plot_->colorColumn().isValid()) {
    auto ind1 = plot_->unnormalizeIndex(node_->ind());

    ModelIndex colorModelInd(const_cast<CQChartsSunburstPlot *>(plot_),
                             ind1.row(), plot_->colorColumn(), ind1.parent());

    bool ok;

    auto colorStr = plot_->modelString(colorModelInd, ok);

    tableTip.addTableRow("Color", colorStr);
  }

  //---

  plot()->addTipColumns(tableTip, node_->ind());

  //---

  return tableTip.str();
}

bool
CQChartsSunburstNodeObj::
inside(const Point &p) const
{
  double r1 = node_->r();
  double r2 = r1 + node_->dr();

  Point c(0, 0);

  double r = p.distanceTo(c);

  if (r < r1 || r > r2)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(CQChartsGeom::pointAngle(c, p)); while (a < 0) a += 360.0;

  double a1 = node_->a().value();
  double a2 = a1 + node_->da().value();

  while (a1 < 0) a1 += 360.0;
  while (a2 < 0) a2 += 360.0;

  if (a1 > a2) {
    // crosses zero
    if (a >= 0 && a <= a2)
      return true;

    if (a <= 360 && a >= a1)
      return true;
  }
  else {
    if (a >= a1 && a <= a2)
      return true;
  }

  return false;
}

void
CQChartsSunburstNodeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsSunburstNodeObj::
draw(CQChartsPaintDevice *device) const
{
  plot_->drawNode(device, const_cast<CQChartsSunburstNodeObj *>(this), node_);
}

//------

CQChartsSunburstHierNode::
CQChartsSunburstHierNode(const Plot *plot, HierNode *parent, const QString &name) :
 CQChartsSunburstNode(plot, parent, name)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsSunburstHierNode::
~CQChartsSunburstHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

bool
CQChartsSunburstHierNode::
isHierExpanded() const
{
  if (plot_->isRoot(this))
    return true;

  if (! isExpanded())
    return false;

  if (parent() && ! parent()->isHierExpanded())
    return false;

  return true;
}

double
CQChartsSunburstHierNode::
hierSize() const
{
  double s = size();

  for (const auto &child : children_)
    s += child->hierSize();

  for (const auto &node : nodes_)
    s += node->hierSize();

  return s;
}

int
CQChartsSunburstHierNode::
depth() const
{
  int depth = 1;

  for (const auto &child : children_)
    depth = std::max(depth, child->depth() + 1);

  return depth;
}

int
CQChartsSunburstHierNode::
numNodes() const
{
  int num = nodes_.size();

  for (const auto &child : children_)
    num += child->numNodes();

  return std::max(num, 1);
}

void
CQChartsSunburstHierNode::
unplace()
{
  unplaceNodes();
}

void
CQChartsSunburstHierNode::
unplaceNodes()
{
  Node::unplace();

  for (auto &child : children_)
    child->unplaceNodes();

  for (auto &node : nodes_)
    node->unplace();
}

void
CQChartsSunburstHierNode::
packNodes(HierNode *root, double ri, double ro, double dr,
          const Angle &a, const Angle &da, const SortType &sortType)
{
  int d = depth();

  if (dr <= 0.0)
    dr = (ro - ri)/d;

  double s = (sortType == SortType::SIZE ? hierSize() : numNodes());

  double da1 = da.value()/s;

  packSubNodes(root, ri, dr, a, Angle(da1), sortType);
}

void
CQChartsSunburstHierNode::
packSubNodes(HierNode *root, double ri, double dr,
             const Angle &a, const Angle &da, const SortType &sortType)
{
  if (! isExpanded())
    return;

  // make single list of nodes to pack
  Nodes nodes;

  for (auto &child : children_)
    nodes.push_back(child);

  for (auto &node : nodes_)
    nodes.push_back(node);

  if      (root->plot()->sortType() == CQChartsSunburstPlot::SortType::SIZE)
    std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeSizeCmp());
  else if (root->plot()->sortType() == CQChartsSunburstPlot::SortType::COUNT)
    std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeCountCmp());
  else if (root->plot()->sortType() == CQChartsSunburstPlot::SortType::NAME)
    std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeNameCmp());

  //---

  placed_ = true;

  // place each node
  double a1 = a.value();

  for (auto &node : nodes) {
    double s = (sortType == SortType::SIZE ? node->hierSize() : node->numNodes());

    node->setPosition(ri, Angle(a1), dr, Angle(s*da.value()));

    auto *hierNode = dynamic_cast<HierNode *>(node);

    if (hierNode)
      hierNode->packSubNodes(root, ri + dr, dr, Angle(a1), da, sortType);

    a1 += s*da.value();
  }
}

void
CQChartsSunburstHierNode::
addNode(Node *node)
{
  nodes_.push_back(node);
}

void
CQChartsSunburstHierNode::
removeNode(Node *node)
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

QColor
CQChartsSunburstHierNode::
interpColor(const Plot *plot, const Color &c, const ColorInd &colorInd, int n) const
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

CQChartsSunburstNode::
CQChartsSunburstNode(const Plot *plot, HierNode *parent, const QString &name) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name)
{
}

QString
CQChartsSunburstNode::
hierName(const QChar &separator) const
{
  if (parent() && (plot()->isMultiRoot() || parent() != plot()->roots()[0]))
    return parent()->hierName(separator) + separator + name();
  else
    return name();
}

void
CQChartsSunburstNode::
setPosition(double r, const Angle &a, double dr, const Angle &da)
{
  r_  = r ; a_  = a ;
  dr_ = dr; da_ = da;

  placed_ = true;
}

bool
CQChartsSunburstNode::
pointInside(double x, double y)
{
  if (! placed_) return false;

  double r = sqrt(x*x + y*y);

  if (r < r_ || r > r_ + dr_) return false;

  double a = CMathUtil::normalizeAngle(CMathUtil::Rad2Deg(atan2(y, x)));

  double a1 = CMathUtil::normalizeAngle(a_.value());
  double a2 = a1 + da_.value();

  if (a2 > a1) {
    if (a2 >= 360.0) {
      double da = a2 - 360.0; a -= da; a1 -= da; a2 = 360.0;
      a = CMathUtil::normalizeAngle(a);
    }

    if (a < a1 || a > a2)
      return false;
  }
  else {
    if (a2 < 0.0) {
      double da = -a2; a += da; a1 += da; a2 = 0.0;

      a = CMathUtil::normalizeAngle(a);
    }

    if (a < a2 || a > a1)
      return false;
  }

  return true;
}

QColor
CQChartsSunburstNode::
interpColor(const Plot *plot, const Color &c, const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpFillColor(ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}

//------

// sort reverse alphabetic no case
bool
CQChartsSunburstNodeNameCmp::
operator()(const Node *n1, const Node *n2)
{
  const auto &name1 = n1->name();
  const auto &name2 = n2->name();

  int l1 = name1.size();
  int l2 = name2.size();

  for (int i = 0; i < std::max(l1, l2); ++i) {
    int c1 = (i < l1 ? tolower(name1[i].toLatin1()) : '\0');
    int c2 = (i < l2 ? tolower(name2[i].toLatin1()) : '\0');

    if (c1 > c2) return true;
    if (c1 < c2) return false;
  }

  return false;
}

// sort size
bool
CQChartsSunburstNodeSizeCmp::
operator()(const Node *n1, const Node *n2)
{
  return n1->size() < n2->size();
}

// sort node count
bool
CQChartsSunburstNodeCountCmp::
operator()(const Node *n1, const Node *n2)
{
  return n1->numNodes() < n2->numNodes();
}
