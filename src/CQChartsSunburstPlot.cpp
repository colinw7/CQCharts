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
  CQChartsHierPlotType::addHierParameters("Sunburst");

  //---

  CQChartsPlotType::addParameters();
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
  CQChartsSunburstPlot::term();
}

//---

void
CQChartsSunburstPlot::
init()
{
  CQChartsHierPlot::init();

  //---

  NoUpdate noUpdate(this);

  setFillColor(Color::makePalette());

  setFilled (true);
  setStroked(true);

  setTextFontSize(8.0);

  setTextColor(Color::makeInterfaceValue(1.0));

  setOuterMargin(PlotMargin::pixel(4, 4, 4, 4));

  //---

  // addKey() // TODO

  addTitle();

  //---

  addColorMapKey();
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
  for (auto &pg : groupSunburstData_) {
    auto &sunburstData = pg.second;

    for (auto &root : sunburstData.roots)
      delete root;

    sunburstData.roots.clear();
  }

  //---

  for (auto &root : sunburstData_.roots)
    delete root;

  sunburstData_.roots.clear();
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

void
CQChartsSunburstPlot::
setSplitGroups(bool b)
{
  CQChartsUtil::testAndSet(splitGroups_, b, [&]() { updateObjs(); } );
}

//---

const CQChartsSunburstPlot::RootNodes &
CQChartsSunburstPlot::
roots(const QString &groupName) const
{
  auto &sunburstData = getSunburstData(groupName);

  return sunburstData.roots;
}

bool
CQChartsSunburstPlot::
isRoot(const QString &groupName, const HierNode *node) const
{
  auto *root = currentRoot(groupName);

  if (root) {
    return (node == root);
  }
  else {
    auto &sunburstData = getSunburstData(groupName);

    for (const auto &root : sunburstData.roots) {
      if (node == root)
        return true;
    }

    return false;
  }
}

bool
CQChartsSunburstPlot::
hasFalseRoot(const QString &groupName, HierNode **newRoot) const
{
  *newRoot = nullptr;

  auto *root = currentRoot(groupName);

  if (! root) {
    auto &sunburstData = getSunburstData(groupName);

    if (sunburstData.roots.size() != 1)
      return false;

    root = sunburstData.roots[0];
  }

  if (root->name() != "" || root->getChildren().size() != 1)
    return false;

  *newRoot = root->getChildren()[0];

  return true;
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
  addProp("options", "splitGroups"     , "", "Show grouped data in separated sunbursts");

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

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//------

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
currentRoot(const QString &groupName) const
{
  auto &sunburstData = getSunburstData(groupName);

  auto names = sunburstData.currentRootName.split('@', Qt::SkipEmptyParts);

  if (names.empty())
    return nullptr;

  HierNode *currentRoot = nullptr;

  for (int i = 0; i < names.size(); ++i) {
    auto *hier = childHierNode(groupName, currentRoot, names[i]);

    if (! hier)
      return currentRoot;

    currentRoot = hier;
  }

  return currentRoot;
}

void
CQChartsSunburstPlot::
setCurrentRoot(const QString &groupName, HierNode *hier, bool update)
{
  auto &sunburstData = getSunburstData(groupName);

  if (hier)
    sunburstData.currentRootName = hier->hierName("@");
  else
    sunburstData.currentRootName.clear();

  if (update) {
    replaceGroups();

    updateObjs();
  }
}

//---

int
CQChartsSunburstPlot::
colorId(const QString &groupName) const
{
  auto &sunburstData = getSunburstData(groupName);

  return sunburstData.colorData.colorId;
}

int
CQChartsSunburstPlot::
numColorIds(const QString &groupName) const
{
  auto &sunburstData = getSunburstData(groupName);

  return sunburstData.colorData.numColorIds;
}

void
CQChartsSunburstPlot::
initColorIds(const QString &groupName)
{
  auto &sunburstData = getSunburstData(groupName);

  sunburstData.colorData.reset();
}

int
CQChartsSunburstPlot::
nextColorId(const QString &groupName)
{
  auto &sunburstData = getSunburstData(groupName);

  ++sunburstData.colorData.colorId;

  if (sunburstData.colorData.colorId >= sunburstData.colorData.numColorIds)
    sunburstData.colorData.numColorIds = sunburstData.colorData.colorId + 1;

  return sunburstData.colorData.colorId;
}

//---

CQChartsGeom::Range
CQChartsSunburstPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsSunburstPlot::calcRange");

  // fixed range (-1,-1 to 1,1)
  Range dataRange;

  double r = 1.0;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

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

  // name columns required
  // value, id, color columns optional

  if (! checkColumns(nameColumns(), "Name", /*required*/true))
    columnsValid = false;

  if (! checkNumericColumn(valueColumn(), "Value", th->valueColumnType_))
    columnsValid = false;

  if (! checkColumn(colorColumn(), "Color")) columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  if (sunburstData_.roots.empty())
    th->initRoots();

  //---

  for (const auto &groupName : groupNameSet_)
    colorGroupNodes(groupName);

  colorGroupNodes("");

  //---

  for (const auto &groupName : groupNameSet_)
    addGroupPlotObjs(groupName, objs);

  addGroupPlotObjs("", objs);

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
addGroupPlotObjs(const QString &groupName, PlotObjs &objs) const
{
  auto &sunburstData = getSunburstData(groupName);

  int nr = int(sunburstData.roots.size());

  bool isUnnamedRoot = (nr == 1 && sunburstData.roots[0]->name() == "");

  auto *currentRoot = this->currentRoot(groupName);

  if (currentRoot) {
    ColorInd ci(0, 1);

    if (! isUnnamedRoot || sunburstData.roots[0] != currentRoot)
      addPlotObj(currentRoot, objs, ci);

    addPlotObjs(currentRoot, objs, ci);
  }
  else {
    int ir = 0;

    for (auto &root : sunburstData.roots) {
      ColorInd ci(ir, nr);

      if (! isUnnamedRoot)
        addPlotObj(root, objs, ci);

      addPlotObjs(root, objs, ci);

      ++ir;
    }
  }
}

void
CQChartsSunburstPlot::
initRoots()
{
  RootNode *root = nullptr;

  if (! isMultiRoot()) {
    // group roots created on demand
    root = createRootNode("");
  }

  if (isHierarchical())
    loadHier(root);
  else
    loadFlat(root);

  //---

  if (isHierarchical() && isFollowViewExpand()) {
    std::set<QModelIndex> indSet;

    expandedModelIndices(indSet);

    //---

    for (auto &pg : groupSunburstData_) {
      auto &sunburstData = pg.second;

      for (const auto &root : sunburstData.roots) {
        for (auto &hierNode : root->getChildren())
          setNodeExpansion(hierNode, indSet);
      }
    }

    for (const auto &root : sunburstData_.roots) {
      for (auto &hierNode : root->getChildren())
        setNodeExpansion(hierNode, indSet);
    }
  }

  //---

  replaceGroups();
}

void
CQChartsSunburstPlot::
replaceGroups()
{
  int ng = int(groupNameSet_.size());

  if (isSplitGroups() && ng > 0) {
    int nx, ny;
    CQChartsUtil::countToSquareGrid(ng, nx, ny);

    double m  = 0.01;
    double dx = 2.0/nx - m;
    double dy = 2.0/ny - m;

    int ig = 0;

    for (const auto &groupName : groupNameSet_) {
      int ix = ig % nx;
      int iy = ig / nx;

      double x1 = ix*(dx + m) - 1.0 + m/2.0;
      double y1 = iy*(dy + m) - 1.0 + m/2.0;
      double x2 = x1 + dx;
      double y2 = y1 + dy;

      replaceRoots(groupName, BBox(Point(x1, y1), Point(x2, y2)));

      ++ig;
    }
  }
  else {
    replaceRoots("", BBox(-1, -1, 1, 1));
  }
}

void
CQChartsSunburstPlot::
replaceRoots(const QString &groupName, const BBox &rect) const
{
  double rs = rect.getWidth()/2.0;

  double ri = std::max(rs*innerRadius(), 0.0);
  double ro = CMathUtil::clamp(rs*outerRadius(), ri, rs);

  auto sortType = static_cast<CQChartsSunburstNode::SortType>(this->sortType());

  auto a = startAngle();

  auto *currentRoot = this->currentRoot(groupName);

  auto &sunburstData = getSunburstData(groupName);

  sunburstData.rect = rect;

  if (currentRoot) {
    Angle da(360.0);

    currentRoot->setPosition(0.0, a, ri, da);

    currentRoot->packNodes(currentRoot, ri, ro, 0.0, a, da, sortType);
  }
  else {
    HierNode *newRoot;

    if (hasFalseRoot(groupName, &newRoot)) {
      Angle da(360.0);

      newRoot->setPosition(0.0, a, ri, da);

      newRoot->packNodes(sunburstData.roots[0], ri, ro, 0.0, a, da, sortType);
    }
    else {
      Angle da(! sunburstData.roots.empty() ? 360.0/double(sunburstData.roots.size()) : 0.0);

      for (auto &root : sunburstData.roots) {
        root->setPosition(0.0, a, ri, da);

        root->packNodes(ri, ro, 0.0, a, da, sortType);

        a += da;
      }
    }
  }
}

//---

void
CQChartsSunburstPlot::
colorGroupNodes(const QString &groupName) const
{
  auto *th = const_cast<CQChartsSunburstPlot *>(this);

  th->initColorIds(groupName);

  auto *currentRoot = this->currentRoot(groupName);

  if (currentRoot) {
    colorNodes(groupName, currentRoot);
  }
  else {
    auto &sunburstData = getSunburstData(groupName);

    for (auto &root : sunburstData.roots)
      colorNodes(groupName, root);
  }
}

void
CQChartsSunburstPlot::
colorNodes(const QString &groupName, HierNode *hier) const
{
  if (! hier->hasNodes() && ! hier->hasChildren()) {
    colorNode(groupName, hier);
  }
  else {
    for (const auto &node : hier->getNodes())
      colorNode(groupName, node);

    for (const auto &child : hier->getChildren())
      colorNodes(groupName, child);
  }
}

void
CQChartsSunburstPlot::
colorNode(const QString &groupName, Node *node) const
{
  if (! node->color().isValid()) {
    auto *th = const_cast<CQChartsSunburstPlot *>(this);

    node->setColorId(th->nextColorId(groupName));
  }
}

void
CQChartsSunburstPlot::
loadHier(HierNode *root) const
{
  class RowVisitor : public ModelVisitor {
   public:
    using SunburstPlot = CQChartsSunburstPlot;

   public:
    RowVisitor(const SunburstPlot *sunburstPlot, HierNode *root) :
     sunburstPlot_(sunburstPlot) {
      hierStack_.push_back(root);
    }

    State hierVisit(const QAbstractItemModel *, const VisitData &data) override {
      QString     name;
      QModelIndex nameInd;

      (void) getName(data, name, nameInd);

      //---

      auto *hier = sunburstPlot_->addHierNode(parentHier(), name, nameInd);

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

      QModelIndex valueInd;

      if (sunburstPlot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(sunburstPlot_, data.row, sunburstPlot_->valueColumn(),
                                 data.parent);

        valueInd = sunburstPlot_->modelIndex(valueModelInd);

        if (! sunburstPlot_->getValueSize(valueModelInd, size))
          return State::SKIP;

        if (size == 0.0)
          return State::SKIP;
      }

      //---

      (void) sunburstPlot_->hierAddNode(parentHier(), name, size, nameInd, valueInd);

      return State::OK;
    }

   private:
    HierNode *parentHier() const {
      assert(! hierStack_.empty());

      return hierStack_.back();
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      ModelIndex nameModelInd(sunburstPlot_, data.row, sunburstPlot_->nameColumns().column(),
                              data.parent);

      nameInd = sunburstPlot_->modelIndex(nameModelInd);

      bool ok;

      name = sunburstPlot_->modelString(nameModelInd, ok);

      return ok;
    }

   private:
    using HierStack = std::vector<HierNode *>;

    const SunburstPlot* sunburstPlot_ { nullptr };
    HierStack           hierStack_;
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
    using SunburstPlot = CQChartsSunburstPlot;

   public:
    RowVisitor(const SunburstPlot *sunburstPlot, HierNode *root) :
     sunburstPlot_(sunburstPlot), root_(root) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get hier names from name columns
      QStringList   nameStrs;
      QModelIndices nameInds;

      if (! sunburstPlot_->getHierColumnNames(data.parent, data.row, sunburstPlot_->nameColumns(),
                                              sunburstPlot_->calcSeparator(), nameStrs, nameInds))
        return State::SKIP;

      //---

      // add group name at top of hier if specified
      QString groupName;

      if (sunburstPlot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(sunburstPlot_, data.row, sunburstPlot_->groupColumn(),
                                 data.parent);

        bool ok;

        groupName = sunburstPlot_->modelString(groupModelInd, ok); // hier ?

        if (groupName == "")
          groupName = "<none>";
      }

      //---

      auto nameInd1 = sunburstPlot_->normalizeIndex(nameInds[0]);

      //---

      // get size from value column
      double      size = 1.0;
      QModelIndex valueInd;

      if (! getSize(data, size, valueInd))
        return State::SKIP;

      // TODO: min size (see hier bubble plot)

      //---

      auto *root1 = root_;

      if (sunburstPlot_->isSplitGroups()) {
        // init root for group if not yet created
        auto *sunburstPlot = const_cast<CQChartsSunburstPlot *>(sunburstPlot_);

        auto pg = sunburstPlot->groupSunburstData_.find(groupName);

        if (pg == sunburstPlot->groupSunburstData_.end()) {
          pg = sunburstPlot->groupSunburstData_.insert(pg,
                 GroupSunburstData::value_type(groupName, SunburstData()));

          auto &sunburstData = (*pg).second;

          auto *root = new RootNode(sunburstPlot, groupName);

          root->setGroupName(groupName);

          sunburstData.roots.push_back(root);
        }

        //---

        root1 = sunburstPlot_->rootNode(groupName, groupName);

        if (! root1) {
          root1 = sunburstPlot->createRootNode(groupName, groupName);

          root1->setInd(nameInd1);
        }
      }

      //---

      // create node
      auto *node = sunburstPlot_->flatAddNode(groupName, root1, nameStrs, size, nameInd1, valueInd);

      if (groupName != "")
        groupNameSet_.insert(groupName);

      //---

      // set color from color column
      if (node && sunburstPlot_->colorColumn().isValid()) {
        Color color;

        if (sunburstPlot_->colorColumnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

    bool getSize(const VisitData &data, double &size, QModelIndex &valueInd) const {
      size = 1.0;

      if (! sunburstPlot_->valueColumn().isValid())
        return true;

      ModelIndex valueModelInd(sunburstPlot_, data.row, sunburstPlot_->valueColumn(), data.parent);

      if (! sunburstPlot_->getValueSize(valueModelInd, size))
        return false;

      if (size == 0.0) // allow negative ?
        return false;

      valueInd = sunburstPlot_->modelIndex(valueModelInd);

      return true;
    }

    const GroupNameSet &groupNameSet() const { return groupNameSet_; }

   private:
    const SunburstPlot* sunburstPlot_ { nullptr };
    HierNode*           root_         { nullptr };
    GroupNameSet        groupNameSet_;
  };

  //---

  RowVisitor visitor(this, root);

  visitModel(visitor);

  auto *th = const_cast<CQChartsSunburstPlot *>(this);

  if (isSplitGroups())
    th->groupNameSet_ = visitor.groupNameSet();
  else
    th->groupNameSet_.clear();

  //---

  for (auto &pg : groupSunburstData_) {
    auto &sunburstData = pg.second;

    for (auto &root : sunburstData.roots)
      addExtraNodes(root);
  }

  for (auto &root : sunburstData_.roots)
    addExtraNodes(root);
}

CQChartsSunburstNode *
CQChartsSunburstPlot::
flatAddNode(const QString &groupName, HierNode *root, const QStringList &nameStrs, double size,
            const QModelIndex &nameInd, const QModelIndex &valueInd) const
{
  auto groupName1 = groupName;

  auto nameStrs1 = nameStrs;

  if (! isSplitGroups()) {
    if (groupName1 != "") {
      nameStrs1.push_front(groupName1);

      groupName1 = "";
    }
  }
  else {
    if (groupName1 == "")
      groupName1 = "<none>";
  }

  //---

  auto *th = const_cast<CQChartsSunburstPlot *>(this);

  //---

  // create parent nodes
  auto *parent = root;

  for (int i = 0; i < nameStrs1.length() - 1; ++i) {
    HierNode *child = nullptr;

    if (i == 0 && isMultiRoot()) {
      auto *root1 = rootNode(groupName1, nameStrs1[i]);

      if (! root1) {
        root1 = th->createRootNode(groupName1, nameStrs1[i]);

        root1->setInd(nameInd);
      }

      child = root1;
    }
    else {
      child = childHierNode(groupName, parent, nameStrs1[i]);

      if (! child) {
        // remove any existing leaf node (save size to use in new hier node)
        QModelIndex nameInd1;
        double      size1 = 0.0;

        auto *node = childNode(parent, nameStrs1[i]);

        if (node) {
          nameInd1 = node->ind();
          size1    = node->size();

          parent->removeNode(node);

          delete node;
        }

        //---

        child = new HierNode(this, parent, nameStrs1[i]);

        child->setSize(size1);
        child->setInd(nameInd1);
        child->setGroupName(groupName1);
      }
    }

    parent = child;
  }

  //---

  // create leaf node
  auto nodeName = nameStrs1[nameStrs1.length() - 1];

  auto *node = childNode(parent, nodeName);

  if (! node) {
    // use hier node if already created
    auto *child = childHierNode(groupName, parent, nodeName);

    if (child) {
      child->setSize(size);
      return nullptr;
    }

    //---

    node = new Node(this, parent, nodeName);

    node->setSize(size);
    node->setGroupName(groupName1);

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

//---

void
CQChartsSunburstPlot::
addExtraNodes(HierNode *hier) const
{
  // create child nodes for hier nodes with explicit size (no children ?)

  if (hier->size() > 0) {
    auto *node = new Node(this, hier, "");

    auto ind1 = unnormalizeIndex(hier->ind());

    if (colorColumn().isValid()) {
      Color color;

      if (colorColumnColor(ind1.row(), ind1.parent(), color))
        node->setColor(color);
    }

    node->setSize(hier->size());
    node->setInd (hier->ind());

    node->setGroupName(hier->groupName());

    node->setFiller(true);

    hier->addNode(node);

    hier->setSize(0.0);
  }

  for (const auto &child : hier->getChildren())
    addExtraNodes(child);
}

//---

CQChartsGeom::BBox
CQChartsSunburstPlot::
getGroupRect(const QString &groupName) const
{
  auto &sunburstData = getSunburstData(groupName);

  return sunburstData.rect;
}

CQChartsSunburstPlot::SunburstData &
CQChartsSunburstPlot::
getSunburstData(const QString &groupName) const
{
  if (groupName != "") {
    auto pg = groupSunburstData_.find(groupName);
    assert(pg != groupSunburstData_.end());

    return const_cast<SunburstData &>((*pg).second);
  }

  return const_cast<SunburstData &>(sunburstData_);
}

//---

CQChartsSunburstRootNode *
CQChartsSunburstPlot::
createRootNode(const QString &groupName, const QString &name)
{
  auto &sunburstData = getSunburstData(groupName);

  auto *root = new RootNode(this, name);

  root->setGroupName(groupName);

  sunburstData.roots.push_back(root);

  return root;
}

CQChartsSunburstRootNode *
CQChartsSunburstPlot::
rootNode(const QString &groupName, const QString &name) const
{
  auto &sunburstData = getSunburstData(groupName);

  for (const auto &root : sunburstData.roots)
    if (root->name() == name)
      return root;

  return nullptr;
}

CQChartsSunburstHierNode *
CQChartsSunburstPlot::
childHierNode(const QString &groupName, HierNode *parent, const QString &name) const
{
  if (! parent) {
    auto &sunburstData = getSunburstData(groupName);

    for (const auto &root : sunburstData.roots) {
      auto *hier = childHierNode(groupName, root, name);
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

  if (valueColumnType() == ColumnType::REAL || valueColumnType() == ColumnType::INTEGER)
    size = modelReal(ind, ok);
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

  auto rc = getGroupRect(node->groupName()).getCenter();

  double xc = rc.x;
  double yc = rc.y;

  BBox bbox(xc - r2, yc - r2, xc + r2, yc + r2);

  auto *obj = createNodeObj(bbox, node);

  obj->connectDataChanged(this, SLOT(updateSlot()));

  obj->setIs(ir);

  node->setObj(obj);

  objs.push_back(obj);
}

//------

bool
CQChartsSunburstPlot::
addMenuItems(QMenu *menu, const Point &pv)
{
  auto pp = viewToWindow(pv);

  PlotObj *plotObj = nullptr;

  (void) objNearestPoint(pp, plotObj);

  auto *nodeObj = dynamic_cast<CQChartsSunburstNodeObj *>(plotObj);

  menuGroupName_ = (nodeObj ? nodeObj->node()->groupName() : "");

  //---

  PlotObjs objs;

  selectedPlotObjs(objs);

  auto *currentRoot = this->currentRoot(menuGroupName_);

  menu->addSeparator();

  auto *pushAction   = addMenuAction(menu, "Push"   , SLOT(pushSlot()));
  auto *popAction    = addMenuAction(menu, "Pop"    , SLOT(popSlot()));
  auto *popTopAction = addMenuAction(menu, "Pop Top", SLOT(popTopSlot()));

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot != nullptr);
  popTopAction->setEnabled(currentRoot != nullptr);

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

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
    auto pos  = view()->mapFromGlobal(QPointF(gpos.x, gpos.y).toPoint());

    auto w = pixelToWindow(Point(pos));

    plotObjsAtPoint(w, objs, Constraints::SELECTABLE);
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
      setCurrentRoot(menuGroupName_, hnode, /*update*/true);

      break;
    }
  }
}

void
CQChartsSunburstPlot::
popSlot()
{
  auto *root = currentRoot(menuGroupName_);

  if (root && root->parent()) {
    setCurrentRoot(menuGroupName_, root->parent(), /*update*/true);
  }
}

void
CQChartsSunburstPlot::
popTopSlot()
{
  auto *root = currentRoot(menuGroupName_);

  if (root) {
    setCurrentRoot(menuGroupName_, nullptr, /*update*/true);
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
resetNodeExpansion()
{
  for (auto &pg : groupSunburstData_) {
    auto &sunburstData = pg.second;

    for (const auto &root : sunburstData.roots) {
      for (auto &hierNode : root->getChildren())
        resetNodeExpansion(hierNode);
    }
  }

  for (const auto &root : sunburstData_.roots) {
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

void
CQChartsSunburstPlot::
setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet)
{
  hierNode->setExpanded(indSet.find(hierNode->ind()) != indSet.end());

  for (auto &hierNode1 : hierNode->getChildren())
    setNodeExpansion(hierNode1, indSet);
}

//------

void
CQChartsSunburstPlot::
drawNode(PaintDevice *device, Node *node, const PenBrush &penBrush,
         const PenBrush &tPenBrush) const
{
  if (! node->placed())
    return;

  // get if root node (if single root and single child of root then child is also root)
  auto *root = dynamic_cast<RootNode *>(node);
//auto *hier = dynamic_cast<HierNode *>(node);

  bool isRoot = root;

  if (! isRoot) {
    auto *parentRoot = dynamic_cast<RootNode *>(node->parent());

    HierNode *newRoot;

    if (parentRoot && hasFalseRoot(node->groupName(), &newRoot))
      isRoot = true;
  }

  //---

  auto a1 = node->a();
  auto da = node->da();
  auto a2 = a1 + da;

  bool isCircle = Angle::isCircle(Angle(), da);

  //---

  auto rr = getGroupRect(node->groupName());

  auto rs = rr.getWidth()/2.0;
  auto rc = rr.getCenter();

  double xc = rc.x;
  double yc = rc.y;

  double r1, r2;

  if (isRoot) {
    r1 = 0.0;

    if (root)
      r2 = std::max(rs*innerRadius(), 0.0);
    else
      r2 = node->r() + node->dr();
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

  //---

  // draw path
  CQChartsDrawUtil::setPenBrush(device, penBrush);

  QPainterPath path;

  if (isCircle) {
    if (ibbox.getWidth() > 0.0)
      device->drawEllipse(ibbox);

    if (obbox.getWidth() > 0.0)
      device->drawEllipse(obbox);
  }
  else {
    // if has non-zero inner radius draw arc segment
    if      (ibbox.getWidth() > 0.0) {
      CQChartsDrawUtil::arcSegmentPath(path, ibbox, obbox, a1, da);

      device->drawPath(path);
    }
    // draw pie slice
    else if (obbox.getWidth() > 0.0) {
      CQChartsDrawUtil::arcPath(path, obbox, a1, da);

      device->drawPath(path);
    }
  }

  charts()->setContrastColor(penBrush.brush.color());

  //---

  // draw text
  if (isTextVisible()) {
    // set font
    setPainterFont(device, textFont());

    //---

    // check if text visible (see treemap)
    if (! isCircle) {
      double c1 = a1.cos();
      double s1 = a1.sin();
      double c2 = a2.cos();
      double s2 = a2.sin();

      Point pw1(xc + r2*c1, yc + r2*s1);
      Point pw2(xc + r2*c2, yc + r2*s2);

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
    Angle  ta;
    double c, s;

    if (isCircle) {
      c = 1.0;
      s = 0.0;
    }
    else {
      ta = Angle(a1.value() + da.value()/2.0);
      c  = ta.cos();
      s  = ta.sin();
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

    Point pt(xc + tx, yc + ty);

    auto name = (! node->isFiller() ? node->name() : node->parent()->name());

    auto ta1 = Angle(c >= 0 ? ta.value() : ta.value() - 180);

    // only contrast support (custom align and angle)
    auto textOptions = this->textOptions();

    textOptions.angle = ta1;
    textOptions.align = align;

    bool centered = ! isCircle;

    CQChartsDrawUtil::drawTextAtPoint(device, pt, name, textOptions, centered);

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

//---

bool
CQChartsSunburstPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsSunburstPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsSunburstPlot::
createCustomControls()
{
  auto *controls = new CQChartsSunburstPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsSunburstNodeObj::
CQChartsSunburstNodeObj(const SunburstPlot *sunburstPlot, const BBox &rect, Node *node) :
 CQChartsPlotObj(const_cast<SunburstPlot *>(sunburstPlot), rect),
 sunburstPlot_(sunburstPlot), node_(node)
{
  if (node_->ind().isValid())
    setModelInd(node_->ind());
}

//---

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

  plot()->addNoTipColumns(tableTip);

  //---

  auto name = (! node_->isFiller() ? node_->hierName() : node_->parent()->hierName());

  tableTip.addTableRow("Name" , name);
  tableTip.addTableRow("Value", node_->hierSize());

  auto ind1 = sunburstPlot_->unnormalizeIndex(node_->ind());

  if (sunburstPlot_->colorColumn().isValid()) {
    ModelIndex colorInd1(sunburstPlot_, ind1.row(), sunburstPlot_->colorColumn(), ind1.parent());

    bool ok;

    auto colorStr = sunburstPlot_->modelString(colorInd1, ok);

    tableTip.addTableRow("Color", colorStr);
  }

  sunburstPlot_->addTipColumn(tableTip, sunburstPlot_->groupColumn(), ind1);

  //---

  const auto &ind = node_->ind();

  plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

//---

bool
CQChartsSunburstNodeObj::
inside(const Point &p) const
{
  double r1 = node_->r();
  double r2 = r1 + node_->dr();

  auto c = sunburstPlot_->getGroupRect(node_->groupName()).getCenter();

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
  for (const auto &c : sunburstPlot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, sunburstPlot_->valueColumn());
}

void
CQChartsSunburstNodeObj::
draw(PaintDevice *device) const
{
  bool updateState = device->isInteractive();

  // calc stroke and brush
  PenBrush penBrush;

  calcPenBrush(penBrush, updateState);

  // calc text pen
  PenBrush tPenBrush;

  calcTextPenBrush(tPenBrush, updateState);

  //---

  sunburstPlot_->drawNode(device, node_, penBrush, tPenBrush);
}

void
CQChartsSunburstNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto bc = sunburstPlot_->interpStrokeColor(colorInd);
  auto fc = node_->interpColor(sunburstPlot_, sunburstPlot_->fillColor(), colorInd,
                               sunburstPlot_->numColorIds(node_->groupName()));

  sunburstPlot_->setPenBrush(penBrush, sunburstPlot_->penData(bc), sunburstPlot_->brushData(fc));

  if (updateState)
    sunburstPlot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsSunburstNodeObj::
calcTextPenBrush(PenBrush &tPenBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto tc = sunburstPlot_->interpTextColor(colorInd);

  sunburstPlot_->setPen(tPenBrush, PenData(true, tc, sunburstPlot_->textAlpha()));

  if (updateState)
    sunburstPlot_->updateObjPenBrushState(this, tPenBrush);
}

//------

CQChartsSunburstHierNode::
CQChartsSunburstHierNode(const SunburstPlot *plot, HierNode *parent, const QString &name) :
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
  if (sunburstPlot_->isRoot(groupName(), this))
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
  int num = int(nodes_.size());

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

  if      (root->sunburstPlot()->sortType() == CQChartsSunburstPlot::SortType::SIZE)
    std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeSizeCmp());
  else if (root->sunburstPlot()->sortType() == CQChartsSunburstPlot::SortType::COUNT)
    std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeCountCmp());
  else if (root->sunburstPlot()->sortType() == CQChartsSunburstPlot::SortType::NAME)
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
  auto n = nodes_.size();

  size_t i = 0;

  for ( ; i < n; ++i) {
    if (nodes_[size_t(i)] == node)
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
interpColor(const SunburstPlot *sunburstPlot, const Color &c,
            const ColorInd &colorInd, int n) const
{
  using Colors = std::vector<QColor>;

  Colors colors;

  for (auto &child : children_)
    colors.push_back(child->interpColor(sunburstPlot, c, colorInd, n));

  for (auto &node : nodes_)
    colors.push_back(node->interpColor(sunburstPlot, c, colorInd, n));

  if (colors.empty())
    return sunburstPlot->interpColor(c, colorInd);

  return CQChartsUtil::blendColors(colors);
}

//------

CQChartsSunburstNode::
CQChartsSunburstNode(const SunburstPlot *sunburstPlot, HierNode *parent, const QString &name) :
 sunburstPlot_(sunburstPlot), parent_(parent), id_(nextId()), name_(name)
{
}

QString
CQChartsSunburstNode::
hierName(const QString &separator) const
{
  if (parent() && (sunburstPlot()->isMultiRoot() ||
      parent() != sunburstPlot()->roots(groupName())[0]))
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

#if 0
bool
CQChartsSunburstNode::
pointInside(double x, double y)
{
  if (! placed_) return false;

  double r = std::hypoy(x, y);

  if (r < r_ || r > r_ + dr_) return false;

  double a = CMathUtil::normalizeAngle(CMathUtil::Rad2Deg(std::atan2(y, x)));

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
#endif

QColor
CQChartsSunburstNode::
interpColor(const SunburstPlot *sunburstPlot, const Color &c,
            const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return sunburstPlot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && sunburstPlot_->isColorById())
    return sunburstPlot->interpFillColor(ColorInd(colorId(), n));
  else
    return sunburstPlot->interpColor(c, colorInd);
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

//------

CQChartsSunburstPlotCustomControls::
CQChartsSunburstPlotCustomControls(CQCharts *charts) :
 CQChartsHierPlotCustomControls(charts, "sunburst")
{
}

void
CQChartsSunburstPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsSunburstPlotCustomControls::
addWidgets()
{
  addHierColumnWidgets();

  addColorColumnWidgets();
}

void
CQChartsSunburstPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  sunburstPlot_ = dynamic_cast<CQChartsSunburstPlot *>(plot);

  CQChartsHierPlotCustomControls::setPlot(plot);
}

void
CQChartsSunburstPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsHierPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsSunburstPlotCustomControls::
connectSlots(bool b)
{
  CQChartsHierPlotCustomControls::connectSlots(b);
}

CQChartsColor
CQChartsSunburstPlotCustomControls::
getColorValue()
{
  return sunburstPlot_->fillColor();
}

void
CQChartsSunburstPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  sunburstPlot_->setFillColor(c);
}
