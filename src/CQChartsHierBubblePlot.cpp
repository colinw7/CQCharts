#include <CQChartsHierBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsPlacer.h>

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
}

CQChartsHierBubblePlot::
~CQChartsHierBubblePlot()
{
  CQChartsHierBubblePlot::term();
}

//---

void
CQChartsHierBubblePlot::
init()
{
  CQChartsHierPlot::init();

  //---

  NoUpdate noUpdate(this);

//setExactBucketValue(false);

  //---

  setFillColor(Color::makePalette());

  setFilled (true);
  setStroked(true);

  setTextContrast(true);
  setTextFontSize(12.0);

  setTextColor(Color::makeInterfaceValue(1.0));

  setOuterMargin(PlotMargin::pixel(4, 4, 4, 4));

 //marginSet_ = false;

  //---

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsHierBubblePlot::
term()
{
  for (auto &pg : groupHierBubbleData_)
    delete pg.second.root;

  delete hierBubbleData_.root;
}

//---

void
CQChartsHierBubblePlot::
setValueLabel(bool b)
{
  CQChartsUtil::testAndSet(valueLabel_, b, [&]() { drawObjs(); } );
}

void
CQChartsHierBubblePlot::
setSorted(bool b)
{
  CQChartsUtil::testAndSet(sortData_.enabled, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierBubblePlot::
setSortReverse(bool b)
{
  CQChartsUtil::testAndSet(sortData_.reverse, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsHierBubblePlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    auto f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

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

void
CQChartsHierBubblePlot::
setMinArea(const Area &a)
{
  CQChartsUtil::testAndSet(minArea_, a, [&]() { drawObjs(); } );
}

void
CQChartsHierBubblePlot::
setSplitGroups(bool b)
{
  CQChartsUtil::testAndSet(splitGroups_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsHierBubblePlot::
addProperties()
{
  addHierProperties();

  // options
  addProp("options", "valueLabel"      , "", "Show value label");
  addProp("options", "sorted"          , "", "Sort values by size");
  addProp("options", "sortReverse"     , "", "Sort values large to small");
  addProp("options", "followViewExpand", "", "Follow view expand");
  addProp("options", "splitGroups"     , "", "Show grouped data in separated treemaps");

  addProp("filter", "minSize", "", "Min size value");
  addProp("filter", "minArea", "", "Min circle area");

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
    CQChartsTextOptions::ValueType::CONTRAST | CQChartsTextOptions::ValueType::SCALED |
    CQChartsTextOptions::ValueType::CLIP_LENGTH |
    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
root(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.root;
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
firstHier(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.firstHier;
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
currentRoot(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  auto *currentRoot = hierBubbleData.root;

  auto names = hierBubbleData.currentRootName.split(calcSeparator(), Qt::SkipEmptyParts);

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
setCurrentRoot(const QString &groupName, HierNode *hier, bool update)
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  if (hier)
    hierBubbleData.currentRootName = hier->hierName();
  else
    hierBubbleData.currentRootName.clear();

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

  return dataRange;
}

//------

int
CQChartsHierBubblePlot::
colorId(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.colorData.colorId;
}

int
CQChartsHierBubblePlot::
numColorIds(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.colorData.numColorIds;
}

void
CQChartsHierBubblePlot::
initColorIds(const QString &groupName)
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  hierBubbleData.colorData.reset();
}

int
CQChartsHierBubblePlot::
nextColorId(const QString &groupName)
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  ++hierBubbleData.colorData.colorId;

  if (hierBubbleData.colorData.colorId >= hierBubbleData.colorData.numColorIds)
    hierBubbleData.colorData.numColorIds = hierBubbleData.colorData.colorId + 1;

  return hierBubbleData.colorData.colorId;
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

#if 0
  if (! marginSet_) {
    if (title()->textStr().length() > 0) {
      th->setOuterMargin(PlotMargin::pixel(4, 4, 4, 4));

      marginSet_ = true;
    }
  }
#endif

  // init value sets
  //initValueSets();

  //---

  // check columns
  bool columnsValid = true;

  // name column required
  // value, color columns optional

  if (! checkColumns(nameColumns(), "Name", /*required*/true))
    columnsValid = false;

  if (! checkNumericColumn(valueColumn(), "Value", th->valueColumnType_))
    columnsValid = false;

  if (! checkColumn(colorColumn(), "Color")) columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  if (! root(""))
    initNodes();

  //---

  for (const auto &groupName : groupNameSet_)
    colorGroupNodes(groupName);

  colorGroupNodes("");

  //---

  for (const auto &groupName : groupNameSet_) {
    auto *currentRoot = this->currentRoot(groupName);

    if (currentRoot)
      initNodeObjs(currentRoot, groupName, nullptr, 0, objs);
  }

  auto *currentRoot = this->currentRoot("");

  if (currentRoot)
    initNodeObjs(currentRoot, "", nullptr, 0, objs);

  //---

  for (auto &obj : objs) {
    auto *hierObj = dynamic_cast<HierObj *>(obj);
    auto *nodeObj = dynamic_cast<NodeObj *>(obj);

    if (hierObj) {
      auto groupName = hierObj->calcGroupName();

      int ig = groupInd(groupName);

      if (hierObj->parent())
        hierObj->setIg(ColorInd(hierObj->parent()->ind(), ig));

      hierObj->setIv(ColorInd(hierObj->ind(), ig));
    }
    else if (nodeObj) {
      auto groupName = nodeObj->calcGroupName();

      int ig = groupInd(groupName);
      int in = valueInd(groupName);

      if (nodeObj->parent())
        nodeObj->setIg(ColorInd(nodeObj->parent()->ind(), ig));

      nodeObj->setIv(ColorInd(nodeObj->ind(), in));
    }
  }

  //---

  return true;
}

void
CQChartsHierBubblePlot::
initNodeObjs(HierNode *hier, const QString &groupName, HierObj *parentObj,
             int depth, PlotObjs &objs) const
{
  HierObj *hierObj = nullptr;

  if (hier != root(groupName)) {
    double r = hier->radius();

    BBox rect(hier->x() - r, hier->y() - r, hier->x() + r, hier->y() + r);

    ColorInd is(hier->depth(), maxDepth(groupName) + 1);

    hierObj = createHierObj(hier, parentObj, rect, is);

    connect(hierObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    objs.push_back(hierObj);
  }

  //---

  for (auto &hierNode : hier->getChildren())
    initNodeObjs(hierNode, groupName, hierObj, depth + 1, objs);

  //---

  for (auto &node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    double r = node->radius();

    BBox rect(node->x() - r, node->y() - r, node->x() + r, node->y() + r);

    ColorInd is(node->depth(), maxDepth(groupName) + 1);

    auto *obj = createNodeObj(node, parentObj, rect, is);

    connect(obj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    objs.push_back(obj);
  }
}

void
CQChartsHierBubblePlot::
resetNodes()
{
  for (auto &pg : groupHierBubbleData_) {
    delete pg.second.root;

    pg.second.reset();
  }

  groupHierBubbleData_.clear();

  delete hierBubbleData_.root;

  hierBubbleData_.reset();
}

void
CQChartsHierBubblePlot::
initNodes() const
{
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  //---

  th->hierBubbleData_.reset();

  th->hierBubbleData_.root = new HierNode(this, nullptr, "<root>");

  th->hierBubbleData_.root->setDepth(0);
  th->hierBubbleData_.root->setHierInd(nextHierInd(""));

  //---

  if (isHierarchical())
    loadHier();
  else
    loadFlat();

  //---

  for (auto &pg : th->groupHierBubbleData_) {
    pg.second.firstHier = pg.second.root;

    while (pg.second.firstHier && pg.second.firstHier->getChildren().size() == 1)
      pg.second.firstHier = pg.second.firstHier->getChildren()[0];
  }

  th->hierBubbleData_.firstHier = hierBubbleData_.root;

  while (th->hierBubbleData_.firstHier && th->hierBubbleData_.firstHier->getChildren().size() == 1)
    th->hierBubbleData_.firstHier = th->hierBubbleData_.firstHier->getChildren()[0];

  //---

  replaceNodes();
}

void
CQChartsHierBubblePlot::
replaceNodes() const
{
  int ng = int(groupNameSet_.size());

  if (isSplitGroups() && ng > 0) {
#if 0
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

      auto rect = BBox(x1, y1, x2, y2);

      auto *hier = currentRoot(groupName);

      if (hier)
        placeNodes(hier, rect);

      auto &hierBubbleData = getHierBubbleData(groupName);

      hierBubbleData.rect = rect;

      ++ig;
    }
#else
    CQChartsGridPlacer placer;

    placer.place(ng);

    size_t ig = 0;

    for (const auto &groupName : groupNameSet_) {
      const auto &bbox = placer.bbox(ig);

      auto *hier = currentRoot(groupName);

      if (hier)
        placeNodes(hier, bbox);

      auto &hierBubbleData = getHierBubbleData(groupName);

      hierBubbleData.rect = bbox;

      ++ig;
    }
#endif
  }
  else {
    auto rect = BBox(-1, -1, 1, 1);

    auto *hier = currentRoot("");

    if (hier)
      placeNodes(hier, rect);

    auto &hierBubbleData = getHierBubbleData("");

    hierBubbleData.rect = rect;
  }
}

void
CQChartsHierBubblePlot::
placeNodes(HierNode *hier, const BBox &rect) const
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

  transformNodes(hier, rect);
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
transformNodes(HierNode *hier, const BBox &rect) const
{
  auto rs = rect.getWidth()/2.0;
  auto rc = rect.getCenter();

  auto scale = this->scale()*rs;

  for (auto &hierNode : hier->getChildren()) {
    hierNode->setX(rc.x + (hierNode->x() - offset().x)*scale);
    hierNode->setY(rc.y + (hierNode->y() - offset().y)*scale);

    hierNode->setRadius(hierNode->radius()*scale);

    transformNodes(hierNode, rect);
  }

  //---

  for (auto &node : hier->getNodes()) {
    node->setX(rc.x + (node->x() - offset().x)*scale);
    node->setY(rc.y + (node->y() - offset().y)*scale);

    node->setRadius(node->radius()*scale);
  }
}

//---

void
CQChartsHierBubblePlot::
colorGroupNodes(const QString &groupName) const
{
  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  th->initColorIds(groupName);

  colorNodes(groupName, root(groupName));
}

void
CQChartsHierBubblePlot::
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
CQChartsHierBubblePlot::
colorNode(const QString &groupName, Node *node) const
{
  if (! node->color().isValid()) {
    auto *th = const_cast<CQChartsHierBubblePlot *>(this);

    node->setColorId(th->nextColorId(groupName));
  }
}

//---

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

      auto *hier = plot_->addHierNode("", parentHier(), name, nameInd);

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

      auto *node = plot_->addNode("", parentHier(), name, size, nameInd);

      if (node && plot_->colorColumn().isValid()) {
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
      bool ok;

      if (plot_->nameColumns().column().isValid()) {
        ModelIndex nameColumnInd(plot_, data.row, plot_->nameColumns().column(), data.parent);

        nameInd = plot_->modelIndex(nameColumnInd);

        name = plot_->modelString(nameColumnInd, ok);
      }
      else {
        ModelIndex idColumnInd(plot_, data.row, plot_->idColumn(), data.parent);

        nameInd = plot_->modelIndex(idColumnInd);

        name = plot_->modelString(idColumnInd, ok);
      }

      return ok;
    }

    bool getSize(const VisitData &data, double &size) const {
      if (! plot_->valueColumn().isValid())
        return true;

      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

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

  RowVisitor visitor(this, root(""));

  visitModel(visitor);
}

CQChartsHierBubbleHierNode *
CQChartsHierBubblePlot::
addHierNode(const QString &groupName, HierNode *hier, const QString &name,
            const QModelIndex &nameInd) const
{
  int depth1 = hier->depth() + 1;

  auto nameInd1 = normalizeIndex(nameInd);

  auto *hier1 = new HierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(nextHierInd(groupName));

  updateMaxDepth(groupName, depth1);

  return hier1;
}

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
addNode(const QString &groupName, HierNode *hier, const QString &name,
        double size, const QModelIndex &nameInd) const
{
  int depth1 = hier->depth() + 1;

  auto nameInd1 = normalizeIndex(nameInd);

  auto *node = new CQChartsHierBubbleNode(this, hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  updateMaxDepth(groupName, depth1);

  return node;
}

//----

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
      // get hier names and associated model indices from name columns
      QStringList   nameStrs;
      QModelIndices nameInds;

      if (! plot_->getHierColumnNames(data.parent, data.row, plot_->nameColumns(),
                                      plot_->calcSeparator(), nameStrs, nameInds))
        return State::SKIP;

      //---

      // add group name at top of hier if specified
      QString groupName;

      if (plot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(plot_, data.row, plot_->groupColumn(), data.parent);

        bool ok;

        groupName = plot_->modelString(groupModelInd, ok); // hier ?

        if (groupName == "")
          groupName = "<none>";
      }

      //---

      // get size from value column
      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      if (plot_->minSize().isSet()) {
        if (size < plot_->minSize().real())
          return State::SKIP;
      }

      //---

      // create node
      auto *node = plot_->addFlatNode(groupName, nameStrs, size, nameInds[0]);

      if (groupName != "")
        groupNameSet_.insert(groupName);

      //---

      // set color from color column
      if (node && plot_->colorColumn().isValid()) {
        Color color;

        if (plot_->colorColumnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

    const GroupNameSet &groupNameSet() const { return groupNameSet_; }

   private:
    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

      if (! plot_->getValueSize(valueModelInd, size))
        return false;

      if (size <= 0.0) // alow negative ?
        return false;

      return true;
    }

   private:
    const Plot*  plot_ { nullptr };
    GroupNameSet groupNameSet_;
  };

  //---

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  if (isSplitGroups())
    th->groupNameSet_ = visitor.groupNameSet();
  else
    th->groupNameSet_.clear();

  //---

  for (const auto &groupName : groupNameSet_)
    addExtraNodes(root(groupName));

  addExtraNodes(root(""));
}

//---

CQChartsHierBubbleNode *
CQChartsHierBubblePlot::
addFlatNode(const QString &groupName, const QStringList &nameStrs, double size,
            const QModelIndex &nameInd) const
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

  auto *th = const_cast<CQChartsHierBubblePlot *>(this);

  if (groupName1 != "" && isSplitGroups()) {
    // init root for group if not yet created
    auto pg = th->groupHierBubbleData_.find(groupName1);

    if (pg == th->groupHierBubbleData_.end()) {
      pg = th->groupHierBubbleData_.insert(pg,
             GroupHierBubbleData::value_type(groupName1, HierBubbleData()));

      auto &hierBubbleData = (*pg).second;

      hierBubbleData.root = new HierNode(this, nullptr, groupName1);

      hierBubbleData.root->setDepth(0);
      hierBubbleData.root->setHierInd(nextHierInd(groupName1));
      hierBubbleData.root->setGroupName(groupName1);
    }
  }

  //---

  int depth = nameStrs1.length();

  updateMaxDepth(groupName1, depth + 1);

  //---

  // create parent nodes
  auto *parent = root(groupName1);

  for (int i = 0; i < nameStrs1.length() - 1; ++i) {
    auto *child = childHierNode(parent, nameStrs1[i]);

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

      child = new HierNode(this, parent, nameStrs1[i], nameInd1);

      child->setSize(size1);

      child->setDepth(depth);
      child->setHierInd(nextHierInd(""));
    }

    parent = child;
  }

  //---

  // create leaf node
  auto name = nameStrs1.back();

  auto *node = childNode(parent, name);

  if (! node) {
    // use hier node if already created
    auto *child = childHierNode(parent, name);

    if (child) {
      child->setSize(size);
      return nullptr;
    }

    //---

    auto nameInd1 = normalizeIndex(nameInd);

    node = new CQChartsHierBubbleNode(this, parent, name, size, nameInd1);

    node->setDepth(depth);

    parent->addNode(node);
  }

  return node;
}

//---

void
CQChartsHierBubblePlot::
addExtraNodes(HierNode *hier) const
{
  // create child nodes for hier nodes with explicit size (no children ?)

  if (hier->size() > 0) {
    auto *node = new CQChartsHierBubbleNode(this, hier, "", hier->size(), hier->ind());

    auto ind1 = unnormalizeIndex(hier->ind());

    if (colorColumn().isValid()) {
      Color color;

      if (colorColumnColor(ind1.row(), ind1.parent(), color))
        node->setColor(color);
    }

    node->setDepth (hier->depth() + 1);
    node->setFiller(true);

    hier->addNode(node);

    hier->setSize(0.0);
  }

  for (const auto &child : hier->getChildren())
    addExtraNodes(child);
}

//---

int
CQChartsHierBubblePlot::
nextHierInd(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.hierInd++;
}

int
CQChartsHierBubblePlot::
groupInd(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.ig;
}

int
CQChartsHierBubblePlot::
nextGroupInd(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.ig++;
}

int
CQChartsHierBubblePlot::
valueInd(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.in;
}

int
CQChartsHierBubblePlot::
nextValueInd(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.in++;
}

int
CQChartsHierBubblePlot::
maxDepth(const QString &groupName) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  return hierBubbleData.maxDepth;
}

void
CQChartsHierBubblePlot::
updateMaxDepth(const QString &groupName, int depth) const
{
  auto &hierBubbleData = getHierBubbleData(groupName);

  hierBubbleData.maxDepth = std::max(hierBubbleData.maxDepth, depth);
}

//---

CQChartsHierBubblePlot::HierBubbleData &
CQChartsHierBubblePlot::
getHierBubbleData(const QString &groupName) const
{
  if (groupName != "") {
    auto pg = groupHierBubbleData_.find(groupName);
    assert(pg != groupHierBubbleData_.end());

    return const_cast<HierBubbleData &>((*pg).second);
  }

  return const_cast<HierBubbleData &>(hierBubbleData_);
}

//---

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

//---

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

  if      (valueColumnType() == ColumnType::REAL || valueColumnType() == ColumnType::INTEGER)
    size = modelReal(ind, ok);
  else if (valueColumnType() == ColumnType::STRING)
    size = 1.0; // TODO: error
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
execDrawForeground(PaintDevice *device) const
{
  int ng = int(groupNameSet_.size());

  if (isSplitGroups() && ng > 0) {
    for (const auto &groupName : groupNameSet_)
      drawBounds(device, groupName, currentRoot(groupName));
  }
  else {
    drawBounds(device, "", currentRoot(""));
  }

  if (isColorMapKey())
    drawColorMapKey(device);
}

void
CQChartsHierBubblePlot::
drawBounds(PaintDevice *device, const QString &groupName, HierNode *hier) const
{
  if (! hier)
    return;

  auto &hierBubbleData = getHierBubbleData(groupName);

  auto rs = hierBubbleData.rect.getWidth()/2.0;
  auto rc = hierBubbleData.rect.getCenter();

  double xc = rc.x + hier->x();
  double yc = rc.y + hier->y();
  double r  = hier->radius()*rs;

  //---

  Point p1(xc - r, yc - r);
  Point p2(xc + r, yc + r);

  BBox bbox(p1, p2);

  //---

  // draw bubble
  PenBrush penBrush;

  auto bc = interpStrokeColor(ColorInd());

  setPenBrush(penBrush, penData(bc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawEllipse(bbox);
}

//------

bool
CQChartsHierBubblePlot::
addMenuItems(QMenu *menu, const Point &pv)
{
  auto pp = viewToWindow(pv);

  PlotObj *plotObj = nullptr;

  (void) objNearestPoint(pp, plotObj);

  auto *nodeObj = dynamic_cast<CQChartsHierBubbleNodeObj *>(plotObj);

  menuGroupName_ = (nodeObj ? nodeObj->calcGroupName() : "");

  //---

  PlotObjs objs;

  selectedPlotObjs(objs);

  menu->addSeparator();

  auto *pushAction   = addMenuAction(menu, "Push"   , SLOT(pushSlot()));
  auto *popAction    = addMenuAction(menu, "Pop"    , SLOT(popSlot()));
  auto *popTopAction = addMenuAction(menu, "Pop Top", SLOT(popTopSlot()));

  auto *currentRoot = this->currentRoot(menuGroupName_);
  auto *root        = this->root(menuGroupName_);

  pushAction  ->setEnabled(! objs.empty());
  popAction   ->setEnabled(currentRoot != root);
  popTopAction->setEnabled(currentRoot != root);

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
}

//---

void
CQChartsHierBubblePlot::
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
    auto *hierObj = dynamic_cast<HierObj *>(obj);

    if (hierObj) {
      auto *hnode = hierObj->hierNode();

      setCurrentRoot("", hnode, /*update*/true);

      break;
    }

    auto *nodeObj = dynamic_cast<CQChartsHierBubbleNodeObj *>(obj);

    if (nodeObj) {
      auto *node = nodeObj->node();

      auto *hnode = node->parent();

      if (hnode) {
        setCurrentRoot("", hnode, /*update*/true);

        break;
      }
    }
  }
}

void
CQChartsHierBubblePlot::
popSlot()
{
  auto *root = currentRoot("");

  if (root && root->parent()) {
    setCurrentRoot("", root->parent(), /*update*/true);
  }
}

void
CQChartsHierBubblePlot::
popTopSlot()
{
  auto *currentRoot = this->currentRoot(menuGroupName_);
  auto *root        = this->root(menuGroupName_);

  if (currentRoot != root) {
    setCurrentRoot("", root, /*update*/true);
  }
}

//----

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

  for (const auto &groupName : groupNameSet_) {
    for (auto &hierNode : root(groupName)->getChildren())
      setNodeExpansion(hierNode, indSet);
  }

  for (auto &hierNode : root("")->getChildren())
    setNodeExpansion(hierNode, indSet);

  drawObjs();
}

void
CQChartsHierBubblePlot::
setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet)
{
  bool expanded = (indSet.find(hierNode->ind()) != indSet.end());

  if (hierNode->isExpanded() != expanded)
    hierNode->setExpanded(expanded);

  for (auto &hierNode1 : hierNode->getChildren())
    setNodeExpansion(hierNode1, indSet);
}

void
CQChartsHierBubblePlot::
resetNodeExpansion()
{
  for (const auto &groupName : groupNameSet_) {
    for (auto &hierNode : root(groupName)->getChildren())
      resetNodeExpansion(hierNode);
  }

  for (auto &hierNode : root("")->getChildren())
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

//---

CQChartsPlotCustomControls *
CQChartsHierBubblePlot::
createCustomControls()
{
  auto *controls = new CQChartsHierBubblePlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsHierBubbleHierObj::
CQChartsHierBubbleHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                          const BBox &rect, const ColorInd &is) :
 CQChartsHierBubbleNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
  if (hier_->ind().isValid())
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

QString
CQChartsHierBubbleHierObj::
calcGroupName() const
{
  return (hierNode() ? hierNode()->calcGroupName() : "");
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
draw(PaintDevice *device) const
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
  PenBrush penBrush;

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
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  if (! hierNode()->isExpanded())
    return CQChartsHierBubbleNodeObj::calcPenBrush(penBrush, updateState);

  //---

  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = hierNode()->interpColor(plot_, plot_->fillColor(), colorInd,
                                    plot_->numColorIds(calcGroupName()));

  plot_->setPenBrush(penBrush, plot_->penData(bc), plot_->brushData(fc));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsHierBubbleNodeObj::
CQChartsHierBubbleNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                          const BBox &rect, const ColorInd &is) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
  setDetailHint(DetailHint::MAJOR);

  if (node_->ind().isValid())
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

  tableTip.addTableRow("Name" , node()->hierName());
  tableTip.addTableRow("Value", node()->hierSize());

  auto ind1 = plot_->unnormalizeIndex(node()->ind());

  if (plot_->colorColumn().isValid()) {
    ModelIndex colorInd1(plot(), ind1.row(), plot_->colorColumn(), ind1.parent());

    bool ok;

    auto colorStr = plot_->modelString(colorInd1, ok);

    tableTip.addTableRow("Color", colorStr);
  }

  plot_->addTipColumn(tableTip, plot_->groupColumn(), ind1);

  //---

  const auto &ind = node_->ind();

  plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

QString
CQChartsHierBubbleNodeObj::
calcGroupName() const
{
  return (node() ? node()->calcGroupName() : "");
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
draw(PaintDevice *device) const
{
  auto *pnode = node()->parent();

  if (pnode && ! pnode->isHierExpanded())
    return;

  //---

  if (this->isMinArea())
    return;

  double r = this->radius();

  Point p1(node()->x() - r, node()->y() - r);
  Point p2(node()->x() + r, node()->y() + r);

  BBox bbox(p1, p2);

  //---

  bool isCirclePoint = this->isCirclePoint();

  Point point;

  if (isCirclePoint)
    point = Point((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0);

  //---

  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw bubble
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if (isCirclePoint)
    device->drawPoint(point);
  else
    device->drawEllipse(bbox);

  device->resetColorNames();

  //---

  if (isCirclePoint)
    return;

  if (plot_->isTextVisible())
    drawText(device, bbox, penBrush.brush.color(), updateState);
}

void
CQChartsHierBubbleNodeObj::
drawText(PaintDevice *device, const BBox &bbox, const QColor &brushColor, bool updateState) const
{
  // get labels (name and optional size)
  QStringList strs;

  auto name = (! node()->isFiller() ? node()->name() : node()->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel() && ! node()->isFiller()) {
    strs.push_back(QString::number(node()->size()));
  }

  //---

  // calc text pen
  plot_->charts()->setContrastColor(brushColor);

  auto colorInd = calcColorInd();

  PenBrush tPenBrush;

  auto tc = plot_->interpTextColor(colorInd);

  plot_->setPen(tPenBrush, PenData(true, tc, plot_->textAlpha()));

  if (updateState)
    plot_->updateObjPenBrushState(this, tPenBrush);

  //---

  device->save();

  //---

  // set font
  auto clipLength = plot_->lengthPixelWidth(plot_->textClipLength());
  auto clipElide  = plot_->textClipElide();

  plot_->setPainterFont(device, plot_->textFont());

  QStringList strs1;

  if (plot_->isTextScaled()) {
    // calc text size
    QFontMetricsF fm(device->font());

    double tw = 0;

    for (int i = 0; i < strs.size(); ++i) {
      auto str1 = CQChartsDrawUtil::clipTextToLength(strs[i], device->font(),
                                                     clipLength, clipElide);

      tw = std::max(tw, fm.horizontalAdvance(str1));

      strs1.push_back(str1);
    }

    double th = strs.size()*fm.height();

    //---

    // calc scale factor
    auto pbbox = plot_->windowToPixel(bbox);

    double sx = (tw > 0 ? pbbox.getWidth ()/tw : 1.0);
    double sy = (th > 0 ? pbbox.getHeight()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    device->setFont(CQChartsUtil::scaleFontSize(device->font(), s), /*scale*/false);
  }
  else {
    for (int i = 0; i < strs.size(); ++i) {
      auto str1 = CQChartsDrawUtil::clipTextToLength(strs[i], device->font(),
                                                     clipLength, clipElide);

      strs1.push_back(str1);
    }
  }

  //---

  // calc text position
  auto pc = plot_->windowToPixel(Point(node()->x(), node()->y()));

  //---

  // draw label
  double xm = plot_->pixelToWindowWidth (3);
  double ym = plot_->pixelToWindowHeight(3);

  device->setClipRect(bbox.adjusted(xm, ym, -xm, -ym));

  // angle and align not supported (always 0 and centered)
  // text is pre-scaled if needed (formatted and html not suppoted as changes scale calc)
  auto textOptions = plot_->textOptions();

  textOptions.angle      = Angle();
  textOptions.align      = Qt::AlignHCenter | Qt::AlignVCenter;
  textOptions.clipLength = 0.0;
  textOptions.scaled     = false;

  textOptions = plot_->adjustTextOptions(textOptions);

  device->setPen(tPenBrush.pen);

  CQChartsDrawUtil::drawTextsAtPoint(device, plot_->pixelToWindow(pc), strs1, textOptions);

#if 0
  if      (strs1.size() == 1) {
    CQChartsDrawUtil::drawTextAtPoint(device, plot_->pixelToWindow(pc), strs1[0], textOptions);
  }
  else if (strs1.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    Point tp1(pc.x, pc.y - th/2);
    Point tp2(pc.x, pc.y + th/2);

    CQChartsDrawUtil::drawTextAtPoint(device, plot_->pixelToWindow(tp1), strs1[0], textOptions);
    CQChartsDrawUtil::drawTextAtPoint(device, plot_->pixelToWindow(tp2), strs1[1], textOptions);
  }
  else {
    assert(false);
  }
#endif

  plot_->charts()->resetContrastColor();

  //---

  device->restore();
}

bool
CQChartsHierBubbleNodeObj::
isMinArea() const
{
  auto &minArea = plot_->minArea();

  if (! minArea.isValid())
    return false;

  double r = this->radius();

  if      (minArea.units() == Units::PLOT) {
    return (minArea.value() > CQChartsUtil::circleArea(r));
  }
  else if (minArea.units() == Units::PIXEL) {
    auto pw = plot_->windowToPixelWidth (r);
    auto ph = plot_->windowToPixelHeight(r);

    return (minArea.value() > CQChartsUtil::ellipseArea(pw, ph));
  }
  else
    return false;
}

bool
CQChartsHierBubbleNodeObj::
isCirclePoint() const
{
  // check if small enough to draw as point
  double r = this->radius();

  auto pw = plot()->windowToPixelWidth (2*r) - 2;
  auto ph = plot()->windowToPixelHeight(2*r) - 2;

  return (pw <= 1.5 || ph <= 1.5);
}

void
CQChartsHierBubbleNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = node()->interpColor(plot_, plot_->fillColor(), colorInd,
                                plot_->numColorIds(calcGroupName()));

  bool isCirclePoint = this->isCirclePoint();

  if (isCirclePoint) {
    if      (plot_->isFilled())
      plot_->setPenBrush(penBrush, PenData(true, fc, plot_->fillAlpha()), plot_->brushData(fc));
    else if (plot_->isStroked())
      plot_->setPenBrush(penBrush, plot_->penData(bc), BrushData(true, bc, plot_->strokeAlpha()));
  }
  else {
    plot_->setPenBrush(penBrush, plot_->penData(bc), plot_->brushData(fc));
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
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
  if (plot_->currentRoot(calcGroupName()) == this)
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
    std::sort(packNodes.begin(), packNodes.end(),
              CQChartsHierBubbleNodeCmp(plot_->isSortReverse()));

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
  int n = int(nodes_.size());

  int i = 0;

  for ( ; i < n; ++i) {
    if (nodes_[size_t(i)] == node)
      break;
  }

  assert(i < n);

  ++i;

  for ( ; i < n; ++i)
    nodes_[size_t(i - 1)] = nodes_[size_t(i)];

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

QString
CQChartsHierBubbleHierNode::
calcGroupName() const
{
  if (groupName_ != "")
    return groupName_;

  if (parent())
    return parent()->calcGroupName();

  return "";
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
  r_ = CQChartsUtil::areaToRadius(hierSize());
}

QString
CQChartsHierBubbleNode::
hierName() const
{
  if (parent() && parent() != plot()->root(calcGroupName()))
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
interpColor(const Plot *plot, const Color &c, const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpColor(c, ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}

QString
CQChartsHierBubbleNode::
calcGroupName() const
{
  if (parent())
    return parent()->calcGroupName();

  return "";
}

//------

CQChartsHierBubblePlotCustomControls::
CQChartsHierBubblePlotCustomControls(CQCharts *charts) :
 CQChartsHierPlotCustomControls(charts, "hierbubble")
{
}

void
CQChartsHierBubblePlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsHierBubblePlotCustomControls::
addWidgets()
{
  addHierColumnWidgets();

  addColorColumnWidgets();
}

void
CQChartsHierBubblePlotCustomControls::
connectSlots(bool b)
{
  CQChartsHierPlotCustomControls::connectSlots(b);
}

void
CQChartsHierBubblePlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsHierBubblePlot *>(plot);

  CQChartsHierPlotCustomControls::setPlot(plot);
}

void
CQChartsHierBubblePlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsHierPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsHierBubblePlotCustomControls::
getColorValue()
{
  return plot_->fillColor();
}

void
CQChartsHierBubblePlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setFillColor(c);
}
