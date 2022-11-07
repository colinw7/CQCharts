#include <CQChartsTreeMapPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsHtml.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsVariant.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>
#include <QCheckBox>

CQChartsTreeMapPlotType::
CQChartsTreeMapPlotType()
{
}

void
CQChartsTreeMapPlotType::
addParameters()
{
  CQChartsHierPlotType::addParameters();
}

QString
CQChartsTreeMapPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Tree Map Plot").
    h3("Summary").
     p("Draw hierarchical data values using sized boxes.").
     p("Each level of the tree map can have an optional title (if large enough) "
       "and can be separately colored.").
     p("The user can push into, or pop out of, a level of hierarchy by selecting the node "
       "and using the Push and Pop operations on the context menu.").
    h3("Columns").
     p("The hierarchical data comes from the " + B("Name") + " columns and " +
       B("Value") + " column.").
     p("If the name columns is a hierarchical path then the separator can be specified width "
       "the " + B("Separator") + " option (default '/').").
    h3("Options").
     p("The " + B("valueLabel") + " option determines if box label includes the value").
    h3("Limitations").
     p("This plot does not support a user specified range, axes, logarithm scales, "
       "or probing.").
     p("The plot does not support an X/Y axis or Key.").
    h3("Example").
     p(IMG("images/treemap.png"));
}

CQChartsPlot *
CQChartsTreeMapPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsTreeMapPlot(view, model);
}

//------

CQChartsTreeMapPlot::
CQChartsTreeMapPlot(View *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("treemap"), model),
 CQChartsObjHierShapeData  <CQChartsTreeMapPlot>(this),
 CQChartsObjHeaderShapeData<CQChartsTreeMapPlot>(this),
 CQChartsObjHeaderTextData <CQChartsTreeMapPlot>(this),
 CQChartsObjShapeData      <CQChartsTreeMapPlot>(this),
 CQChartsObjTextData       <CQChartsTreeMapPlot>(this)
{
}

CQChartsTreeMapPlot::
~CQChartsTreeMapPlot()
{
  CQChartsTreeMapPlot::term();
}

//---

void
CQChartsTreeMapPlot::
init()
{
  CQChartsHierPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  // hier node style
  setHierFilled(true);
  setHierStroked(true);

  setHierFillColor(Color::makeInterfaceValue(0.4));
  setHierStrokeAlpha(Alpha(0.5));


  //---

  // hier header style
  setHeaderFilled(true);
  setHeaderStroked(false);

  setHeaderFillColor(Color::makeInterfaceValue(0.2));
  setHeaderStrokeColor(Color::makeInterfaceValue(1.0));

  setHeaderTextColor(Color::makeInterfaceValue(1.0));
  setHeaderTextFontSize(12.0);
  setHeaderTextAlign(Qt::AlignLeft | Qt::AlignVCenter);

  //---

  // node style
  setFilled (true);
  setStroked(true);

  setFillColor(Color::makePalette());
  setTextColor(Color::makeContrast());

  setTextFontSize(14.0);
  setTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);
  setTextScaled(true);
  setTextFormatted(true);

  //---

  setOuterMargin(PlotMargin::pixel(4, 4, 4, 4));

  //---

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsTreeMapPlot::
term()
{
  for (auto &pg : groupTreeMapData_)
    delete pg.second.root;

  delete treeMapData_.root;
}

//---

void
CQChartsTreeMapPlot::
setTitles(bool b)
{
  CQChartsUtil::testAndSet(titleData_.visible, b, [&]() {
    updateCurrentRoot(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsTreeMapPlot::
setTitleAutoHide(bool b)
{
  CQChartsUtil::testAndSet(titleData_.autoHide, b, [&]() { updateCurrentRoot(); } );
}

void
CQChartsTreeMapPlot::
setTitleMaxExtent(const OptReal &r)
{
  CQChartsUtil::testAndSet(titleData_.maxExtent, r, [&]() { updateCurrentRoot(); } );
}

void
CQChartsTreeMapPlot::
setTitleHeight(const OptLength &l)
{
  CQChartsUtil::testAndSet(titleData_.height, l, [&]() { updateCurrentRoot(); } );
}

void
CQChartsTreeMapPlot::
setTitleHierName(bool b)
{
  CQChartsUtil::testAndSet(titleData_.hierName, b, [&]() { drawObjs(); } );
}

void
CQChartsTreeMapPlot::
setTitleTextClipped(bool b)
{
  CQChartsUtil::testAndSet(titleData_.textClipped, b, [&]() { drawObjs(); } );
}

void
CQChartsTreeMapPlot::
setTitleMargin(double m)
{
  CQChartsUtil::testAndSet(titleData_.margin, m, [&]() { updateCurrentRoot(); } );
}

void
CQChartsTreeMapPlot::
setTitleDepth(int d)
{
  CQChartsUtil::testAndSet(titleData_.depth, d, [&]() { updateCurrentRoot(); } );
}

//--

double
CQChartsTreeMapPlot::
calcTitleHeight() const
{
  auto font = view()->plotFont(this, headerTextFont());

  QFontMetricsF fm(font);

  if (titleHeight().isSet()) {
    double hh = lengthPixelHeight(*titleHeight().value());

    return std::max(hh, 4.0);
  }

  return fm.height() + 4.0;
}

//----

void
CQChartsTreeMapPlot::
setHierName(bool b)
{
  CQChartsUtil::testAndSet(nodeData_.hierName, b, [&]() { drawObjs(); } );
}

void
CQChartsTreeMapPlot::
setNumSkipHier(int n)
{
  CQChartsUtil::testAndSet(nodeData_.numSkipHier, n, [&]() { drawObjs(); } );
}

void
CQChartsTreeMapPlot::
setTextClipped(bool b)
{
  CQChartsUtil::testAndSet(nodeData_.textClipped, b, [&]() { drawObjs(); } );
}

void
CQChartsTreeMapPlot::
setValueLabel(bool b)
{
  CQChartsUtil::testAndSet(nodeData_.valueLabel, b, [&]() {
    drawObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsTreeMapPlot::
setValueCombine(const ValueCombine &combine)
{
  CQChartsUtil::testAndSet(nodeData_.valueCombine, combine, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsTreeMapPlot::
setMarginWidth(const Length &l)
{
  CQChartsUtil::testAndSet(nodeData_.marginWidth, l, [&]() { updateCurrentRoot(); } );
}

//---

void
CQChartsTreeMapPlot::
setMinArea(const Area &a)
{
  CQChartsUtil::testAndSet(treeData_.minArea, a, [&]() { drawObjs(); } );
}

//----

void
CQChartsTreeMapPlot::
setSplitGroups(bool b)
{
  CQChartsUtil::testAndSet(treeData_.splitGroups, b, [&]() {
    menuGroupName_ = ""; popTop(/*update*/false); updateRangeAndObjs(); } );
}

void
CQChartsTreeMapPlot::
setGroupPalette(bool b)
{
  CQChartsUtil::testAndSet(treeData_.groupPalette, b, [&]() { updateRangeAndObjs(); } );
}

//----

void
CQChartsTreeMapPlot::
setNodeColorType(const NodeColorType &type)
{
  CQChartsUtil::testAndSet(nodeColorType_, type, [&]() { drawObjs(); } );
}

void
CQChartsTreeMapPlot::
setHierColorType(const HierColorType &type)
{
  CQChartsUtil::testAndSet(hierColorType_, type, [&]() { drawObjs(); } );
}

//----

void
CQChartsTreeMapPlot::
setHeaderTextFontSize(double s)
{
  if (s != headerTextData_.font().pointSizeF()) {
    auto f = headerTextData_.font(); f.setPointSizeF(s); headerTextData_.setFont(f);

    drawObjs();
  }
}

//----

void
CQChartsTreeMapPlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    auto f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

    drawObjs();
  }
}

//---

void
CQChartsTreeMapPlot::
addProperties()
{
  addHierProperties();

  // options
  addProp("options", "valueLabel"      , "", "Show value label");
  addProp("options", "valueCombine"    , "", "How duplicate path values are handled");
  addProp("options", "followViewExpand", "", "Follow view expand");
  addProp("options", "hierSelect"      , "", "Allow hier select");

  // grouping
  addProp("grouping", "splitGroups" , "", "Show grouped data in separate treemaps");
  addProp("grouping", "groupPalette", "", "Use separate palette per group");

  // filter
  addProp("filter", "minArea", "", "Min box area");

  // margins
  addProp("margins", "marginWidth", "box", "Margin size for tree map boxes");

  // coloring
  addProp("coloring", "nodeColorType", "nodeColorType", "Node color type");
  addProp("coloring", "hierColorType", "hierColorType", "Hier node color type");

  //---

  // header
  addProp("header", "titles"        , "visible"  ,
          "Header title visible for each hierarchical group");
  addProp("header", "titleAutoHide", "autoHide",
          "Auto hide header if extent too large");
  addProp("header", "titleMaxExtent", "maxExtent",
          "Max extent of hierarchical group header (0.0 - 1.0)")->
    setMinValue(0.0).setMaxValue(1.0);
  addProp("header", "titleHeight"  , "height"   ,
          "Explicit hierarchical group header height");
  addProp("header", "titleMargin"  , "margin"   ,
          "Hierarchical group header margin in pixels")->setMinValue(0.0);
  addProp("header", "titleDepth"   , "depth"   ,
          "Maximum depth to show header (-1 is unset)")->setMinValue(-1);

  // header text
  addProp("header/text", "titleHierName"   , "hierName", "Show hierarchical name on title");
  addProp("header/text", "titleTextClipped", "clipped" , "Clip text to header");

  addTextProperties("header/text", "headerText", "Header", CQChartsTextOptions::ValueType::ALL);

  // header fill
  addProp("header/fill", "headerFilled", "visible", "Header fill visible");

  addFillProperties("header/fill", "headerFill", "Header");

  // header stroke
  addProp("header/stroke", "headerStroked", "visible", "Header stroke visible");

  addLineProperties("header/stroke", "headerStroke", "Header");

  //---

  // hier node fill
  addProp("hier/fill", "hierFilled", "visible", "Hier node fill visible");

  addFillProperties("hier/fill", "hierFill", "Hier node");

  // hier node stroke
  addProp("hier/stroke", "hierStroked", "visible", "Hier node stroke visible");

  addLineProperties("hier/stroke", "hierStroke", "Hier node");

  //---

  // node fill
  addProp("node/fill", "filled", "visible", "Node Fill visible");

  addFillProperties("node/fill", "fill", "");

  // node stroke
  addProp("node/stroke", "stroked", "visible", "Node Stroke visible");

  addLineProperties("node/stroke", "stroke", "");

  // node text
  addProp("node/text", "textVisible", "visible", "Node Text visible");

  addProp("node/text", "hierName"   , "hierName"   , "Show hierarchical name in box");
  addProp("node/text", "numSkipHier", "numSkipHier", "Number of hierarchical name levels to skip");
  addProp("node/text", "textClipped", "clipped"    , "Clip text to box");

  addTextProperties("node/text", "text", "", CQChartsTextOptions::ValueType::ALL);

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
root(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.root;
}

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
firstHier(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.firstHier;
}

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
currentRoot(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  auto sep = calcSeparator();

  auto names = treeMapData.currentRootName.split(sep, Qt::SkipEmptyParts);

  if (names.empty())
    return treeMapData.firstHier;

  auto *currentRoot = treeMapData.root;

  if (! currentRoot)
    return nullptr;

  for (int i = 0; i < names.size(); ++i) {
    auto *hier = childHierNode(currentRoot, names[i]);

    if (! hier)
      return currentRoot;

    currentRoot = hier;
  }

  return currentRoot;
}

void
CQChartsTreeMapPlot::
setCurrentRoot(const QString &groupName, HierNode *hier, bool update)
{
  auto sep = calcSeparator();

  treeData_.currentGroupName =
    (hier && ! hier->parent() && hier != treeMapData_.root ? hier->hierName(sep) : "");

  auto &treeMapData = getTreeMapData(groupName);

  if (hier) {
    treeMapData.currentRootName = hier->hierName(sep);
  }
  else
    treeMapData.currentRootName.clear();

  if (update)
    updateCurrentRoot();
}

void
CQChartsTreeMapPlot::
updateCurrentRoot()
{
  //replaceNodes();

  updateObjs();
}

//---

CQChartsGeom::Range
CQChartsTreeMapPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsTreeMapPlot::calcRange");

  double r = 1.0;

  Range dataRange;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

  return dataRange;
}

//------

#if 0
int
CQChartsTreeMapPlot::
colorId(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.colorData.colorId;
}
#endif

int
CQChartsTreeMapPlot::
numColorIds(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.colorData.numColorIds;
}

void
CQChartsTreeMapPlot::
initColorIds(const QString &groupName)
{
  auto &treeMapData = getTreeMapData(groupName);

  treeMapData.colorData.reset();
}

int
CQChartsTreeMapPlot::
nextColorId(const QString &groupName)
{
  auto &treeMapData = getTreeMapData(groupName);

  ++treeMapData.colorData.colorId;

  if (treeMapData.colorData.colorId >= treeMapData.colorData.numColorIds)
    treeMapData.colorData.numColorIds = treeMapData.colorData.colorId + 1;

  return treeMapData.colorData.colorId;
}

//------

void
CQChartsTreeMapPlot::
clearPlotObjects()
{
  resetNodes();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsTreeMapPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsTreeMapPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsTreeMapPlot *>(this);

  th->clearErrors();

  //---

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

  if (! checkColumn(colorColumn(), "Color"))
    columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  auto *modelData     = getModelData();
  auto *details       = (modelData ? modelData->details() : nullptr);
  auto *columnDetails = (details ? details->columnDetails(valueColumn()) : nullptr);

  bool ok1 = false, ok2 = false;
  if (columnDetails) {
    th->minValue_ = CQChartsVariant::toReal(columnDetails->minValue(), ok1);
    th->maxValue_ = CQChartsVariant::toReal(columnDetails->maxValue(), ok2);
  }

  th->hasValueRange_ = (ok1 && ok2);

  //---

  if (! root(""))
    initNodes();
  else
    replaceNodes();

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

    if      (hierObj) {
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

  th->restoreSelection(objs);

  th->modelViewExpansionChanged();

  return true;
}

void
CQChartsTreeMapPlot::
initNodeObjs(HierNode *hier, const QString &groupName, HierObj *parentObj,
             int depth, PlotObjs &objs) const
{
  HierObj *hierObj = nullptr;

  bool isSplitGroup = false;

  if (isSplitGroups() && groupName != "")
    isSplitGroup = true;

  if (isSplitGroup || hier != root(groupName)) {
    BBox rect(hier->x(), hier->y(), hier->x() + hier->w(), hier->y() + hier->h());

    ColorInd is(hier->depth(), maxDepth(groupName) + 1);

    hierObj = createHierObj(hier, parentObj, rect, is);

    connect(hierObj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    hierObj->setInd(nextGroupInd(groupName));

    if (parentObj)
      parentObj->addChild(hierObj);

    objs.push_back(hierObj);
  }

  //---

  for (auto &hierNode : hier->getChildren())
    initNodeObjs(hierNode, groupName, hierObj, depth + 1, objs);

  //---

  for (auto &node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    BBox rect(node->x(), node->y(), node->x() + node->w(), node->y() + node->h());

    ColorInd is(node->depth(), maxDepth(groupName) + 1);

    auto *obj = createNodeObj(node, hierObj, rect, is);

    connect(obj, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

    obj->setInd(nextValueInd(groupName));

    if (parentObj)
      parentObj->addChild(obj);

    objs.push_back(obj);
  }
}

void
CQChartsTreeMapPlot::
resetNodes()
{
  for (auto &pg : groupTreeMapData_) {
    delete pg.second.root;

    pg.second.reset();
  }

  groupTreeMapData_.clear();

  delete treeMapData_.root;

  treeMapData_.reset();
}

void
CQChartsTreeMapPlot::
initNodes() const
{
  auto *th = const_cast<CQChartsTreeMapPlot *>(this);

  //---

  th->treeMapData_.reset();

  th->treeMapData_.maxDepth = 0;

  th->treeMapData_.root = createHierNode(nullptr, "<root>", QModelIndex());

  th->treeMapData_.root->setDepth(0);
  th->treeMapData_.root->setHierInd(nextHierInd(""));

  //---

  if (isHierarchical())
    loadHier();
  else
    loadFlat();

  //---

  for (auto &pg : th->groupTreeMapData_) {
    pg.second.firstHier = pg.second.root;

    while (pg.second.firstHier && pg.second.firstHier->numChildren() == 1)
      pg.second.firstHier = pg.second.firstHier->childAt(0);
  }

  th->treeMapData_.firstHier = th->treeMapData_.root;

  while (th->treeMapData_.firstHier && th->treeMapData_.firstHier->numChildren() == 1)
    th->treeMapData_.firstHier = th->treeMapData_.firstHier->childAt(0);

  //---

  replaceNodes();
}

void
CQChartsTreeMapPlot::
replaceNodes() const
{
  auto *th = const_cast<CQChartsTreeMapPlot *>(this);

  th->windowHeaderHeight_ = pixelToWindowHeight(calcTitleHeight());
//th->windowMarginWidth_  = lengthPixelWidth   (marginWidth());
  th->windowMarginWidth_  = lengthPlotWidth    (marginWidth());

  int ng = numGroups();

  if (isSplitGroups() && ng > 0) {
    int nx = 1, ny = 1;

    if (treeData_.currentGroupName == "")
      CQChartsUtil::countToSquareGrid(ng, nx, ny);

    double m  = 0.01;
    double dx = 2.0/nx - m;
    double dy = 2.0/ny - m;

    int ig = 0;

    for (const auto &groupName : groupNameSet_) {
      bool visible = (treeData_.currentGroupName == "" || groupName == treeData_.currentGroupName);

      auto *hier = currentRoot(groupName);
      assert(hier);

      hier->setVisible(visible);

      if (! visible)
        continue;

      int ix = ig % nx;
      int iy = ig / nx;

      double x1 = ix*(dx + m) - 1.0 + m/2.0;
      double y1 = iy*(dy + m) - 1.0 + m/2.0;
      double x2 = x1 + dx;
      double y2 = y1 + dy;

      placeNodes(hier, x1, y1, x2, y2);

      ++ig;
    }
  }
  else {
    auto *hier = currentRoot("");

    if (hier)
      placeNodes(hier, -1, -1, 1, 1);
  }
}

void
CQChartsTreeMapPlot::
placeNodes(HierNode *hier, double x1, double y1, double x2, double y2) const
{
  hier->setPosition(x1, y1, x2 - x1, y2 - y1); // x, y, w, h
}

//---

void
CQChartsTreeMapPlot::
colorGroupNodes(const QString &groupName) const
{
  auto *th = const_cast<CQChartsTreeMapPlot *>(this);

  th->initColorIds(groupName);

  colorNodes(groupName, firstHier(groupName));
}

void
CQChartsTreeMapPlot::
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
CQChartsTreeMapPlot::
colorNode(const QString &groupName, Node *node) const
{
  if (! node->color().isValid()) {
    auto *th = const_cast<CQChartsTreeMapPlot *>(this);

    node->setColorId(th->nextColorId(groupName));
  }
}

//---

void
CQChartsTreeMapPlot::
loadHier() const
{
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsTreeMapPlot;

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
      auto *hier = hierStack_.back();

      hierStack_.pop_back();

      assert(! hierStack_.empty());

      if (hier->hierSize() == 0) {
        auto *plot = const_cast<Plot *>(plot_);

        plot->removeHierNode(hier);
      }

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

      //---

      auto *node = plot_->hierAddNode("", parentHier(), name, size, nameInd);

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

      ModelIndex nameModelInd(plot_, data.row, plot_->nameColumns().column(), data.parent);

      nameInd = plot_->modelIndex(nameModelInd);

      name = plot_->modelString(nameModelInd, ok);

      return ok;
    }

    bool getSize(const VisitData &data, double &size) {
      if (! plot_->valueColumn().isValid())
        return false;

      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

      if (! plot_->getValueSize(valueModelInd, size))
        return false;

      if (size == 0.0)
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

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
addHierNode(const QString &groupName, HierNode *hier, const QString &name,
            const QModelIndex &nameInd) const
{
  int depth1 = hier->depth() + 1;

  auto nameInd1 = normalizeIndex(nameInd);

  auto *hier1 = createHierNode(hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(nextHierInd(groupName));

  updateMaxDepth(groupName, depth1);

  //---

  auto sep = calcSeparator();

  bool expanded = isPathExpanded(hier1->hierName(sep));

  hier1->setExpanded(expanded);

  return hier1;
}

void
CQChartsTreeMapPlot::
removeHierNode(HierNode *hier)
{
  if (hier->parent())
    hier->parent()->removeChild(hier);
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
hierAddNode(const QString &groupName, HierNode *hier, const QString &name,
            double size, const QModelIndex &nameInd) const
{
  int depth1 = hier->depth() + 1;

  auto nameInd1 = normalizeIndex(nameInd);

  auto *node = createNode(hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  updateMaxDepth(groupName, depth1);

  return node;
}

//---

void
CQChartsTreeMapPlot::
loadFlat() const
{
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsTreeMapPlot;

   public:
    RowVisitor(const Plot *plot) :
     plot_(plot) {
      groupColumn_ = plot_->groupColumn();

      if (groupColumn_.isValid()) {
        for (const auto &c : plot_->nameColumns())
          if (c != groupColumn_)
            nameColumns_.addColumn(c);

        if (nameColumns_.count() == 0)
          nameColumns_ = plot_->nameColumns();
      }
      else
        nameColumns_ = plot_->nameColumns();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get hier names and associated model indices from name columns
      QStringList   nameStrs;
      QModelIndices nameInds;

      auto sep = plot_->calcSeparator();

      if (! plot_->getHierColumnNames(data.parent, data.row, nameColumns_,
                                      sep, nameStrs, nameInds))
        return State::SKIP;

      //---

      // add group name at top of hier if specified
      QString groupName;

      if (groupColumn_.isValid()) {
        ModelIndex groupModelInd(plot_, data.row, groupColumn_, data.parent);

        bool ok;

        groupName = plot_->modelString(groupModelInd, ok); // hier ?

        if (groupName == "")
          groupName = "<none>";
      }

      //---

      // get value name (last name columns name or from id column)
      QString     name;
      QModelIndex nameInd;

      if (plot_->idColumn().isValid()) {
        ModelIndex idModelInd(plot_, data.row, plot_->idColumn(), data.parent);

        bool ok;

        name = plot_->modelString(idModelInd, ok);

        if (! ok)
          name = nameStrs.back();

        nameInd = plot_->modelIndex(idModelInd);
      }
      else {
        name    = nameStrs.back();
        nameInd = nameInds[0];
      }

      auto nameInd1 = plot_->normalizeIndex(nameInd);

      //---

      // get size from value column
      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      // TODO: min size (see hier bubble plot)

      //---

      // create node
      auto *node = plot_->flatAddNode(groupName, nameStrs, size, nameInd1, name);

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
      if (! plot_->valueColumn().isValid())
        return true;

      ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);

      if (! plot_->getValueSize(valueModelInd, size))
        return false;

      if (size <= 0.0) // allow negative ?
        return false;

      return true;
    }

   private:
    const Plot*  plot_ { nullptr };
    Column       groupColumn_;
    Columns      nameColumns_;
    GroupNameSet groupNameSet_;
  };

  //---

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  auto *th = const_cast<CQChartsTreeMapPlot *>(this);

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

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
flatAddNode(const QString &groupName, const QStringList &nameStrs, double size,
            const QModelIndex &nameInd, const QString &name) const
{
  auto groupName1 = groupName;

  auto nameStrs1 = nameStrs;

  if (groupName1 != "") {
    if (! isSplitGroups()) {
      nameStrs1.push_front(groupName1);

      groupName1 = "";
    }
  }
  else {
    if (groupName1 == "")
      groupName1 = "<none>";
  }

  //---

  auto *th = const_cast<CQChartsTreeMapPlot *>(this);

  if (groupName1 != "" && isSplitGroups()) {
    // init root for group if not yet created
    auto pg = th->groupTreeMapData_.find(groupName1);

    if (pg == th->groupTreeMapData_.end()) {
      pg = th->groupTreeMapData_.insert(pg,
             GroupTreeMapData::value_type(groupName1, TreeMapData()));

      auto &treeMapData = (*pg).second;

      treeMapData.maxDepth = 0;

      treeMapData.root = createHierNode(nullptr, groupName1, QModelIndex());

      treeMapData.root->setDepth(0);
      treeMapData.root->setHierInd(nextHierInd(groupName1));
      treeMapData.root->setGroupName(groupName1);
    }
  }

  //---

  int depth = nameStrs1.length();

  updateMaxDepth(groupName1, depth + 1);

  //---

  // create parent nodes
  auto *parent = root(groupName1);

  for (int j = 0; j < nameStrs1.length() - 1; ++j) {
    auto *child = childHierNode(parent, nameStrs1[j]);

    if (! child) {
      // remove any existing leaf node (save size to use in new hier node)
      auto   nameInd1 = nameInd;
      double size1    = 0.0;

      auto *node = childNode(parent, nameStrs1[j]);

      if (node) {
        nameInd1 = node->ind();
        size1    = node->size();

        parent->removeNode(node);

        delete node;
      }

      //---

      child = createHierNode(parent, nameStrs1[j], nameInd1);

      child->setSize(size1);

      child->setDepth(j + 1);
      child->setHierInd(nextHierInd(groupName1));

      //---

      auto sep = calcSeparator();

      bool expanded = isPathExpanded(child->hierName(sep));

      child->setExpanded(expanded);
    }

    parent = child;
  }

  //---

  // create leaf node
  auto nodeName = nameStrs1.back();

  auto *node = childNode(parent, nodeName);

  if (! node) {
    // use hier node if already created
    auto *child = childHierNode(parent, nodeName);

    if (child) {
      child->setSize(size);
      return nullptr;
    }

    //---

    node = createNode(parent, name, size, nameInd);

    node->setDepth(depth);

    parent->addNode(node);
  }
  else {
    // node already exists so we may have accidental duplicate or value to combine
    if      (valueCombine() == ValueCombine::REPLACE)
      node->setSize(size);
    else if (valueCombine() == ValueCombine::SUM)
      node->setSize(node->size() + size);
    else if (valueCombine() == ValueCombine::WARN) {
      ModelIndex modelInd(this, nameInd.row(), valueColumn(), nameInd.parent());
      auto sep = calcSeparator();
      (void) th->addDataError(modelInd, "Duplicate node value for '" + nameStrs1.join(sep) + "'");
    }
  }

  return node;
}

//---

void
CQChartsTreeMapPlot::
addExtraNodes(HierNode *hier) const
{
  // create child nodes for hier nodes with explicit size (no children ?)

  if (hier->size() > 0) {
    auto *node = createNode(hier, "", hier->size(), hier->ind());

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
CQChartsTreeMapPlot::
nextHierInd(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.hierInd++;
}

int
CQChartsTreeMapPlot::
groupInd(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.ig;
}

int
CQChartsTreeMapPlot::
nextGroupInd(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.ig++;
}

int
CQChartsTreeMapPlot::
valueInd(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.in;
}

int
CQChartsTreeMapPlot::
nextValueInd(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.in++;
}

int
CQChartsTreeMapPlot::
maxDepth(const QString &groupName) const
{
  auto &treeMapData = getTreeMapData(groupName);

  return treeMapData.maxDepth;
}

void
CQChartsTreeMapPlot::
updateMaxDepth(const QString &groupName, int depth) const
{
  auto &treeMapData = getTreeMapData(groupName);

  treeMapData.maxDepth = std::max(treeMapData.maxDepth, depth);
}

//---

CQChartsTreeMapPlot::TreeMapData &
CQChartsTreeMapPlot::
getTreeMapData(const QString &groupName) const
{
  auto pg = groupTreeMapData_.find(groupName);

  if (pg != groupTreeMapData_.end())
    return const_cast<TreeMapData &>((*pg).second);

  if (groupName != "" && groupName != "<none>")
    assert(false);

  return const_cast<TreeMapData &>(treeMapData_);
}

//---

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
childHierNode(HierNode *parent, const QString &name) const
{
  for (const auto &child : parent->getChildren())
    if (child->name() == name)
      return child;

  return nullptr;
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
childNode(HierNode *parent, const QString &name) const
{
  for (const auto &node : parent->getNodes())
    if (node->name() == name)
      return node;

  return nullptr;
}

//---

bool
CQChartsTreeMapPlot::
getValueSize(const ModelIndex &ind, double &size) const
{
  auto addDataError = [&](const QString &msg) {
    const_cast<CQChartsTreeMapPlot *>(this)->addDataError(ind, msg);
    return false;
  };

  size = 1.0;

  if (! ind.isValid())
    return false;

  bool ok = true;

  if      (valueColumnType() == ColumnType::REAL || valueColumnType() == ColumnType::INTEGER)
    size = modelReal(ind, ok);
//else if (valueColumnType() == ColumnType::STRING)
//  size = 1.0; // TODO: error
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
CQChartsTreeMapPlot::
addMenuItems(QMenu *menu, const Point &pv)
{
  auto pp = viewToWindow(pv);

  PlotObj *plotObj = nullptr;

  (void) objNearestPoint(pp, plotObj);

  auto *nodeObj = dynamic_cast<CQChartsTreeMapNodeObj *>(plotObj);

  menuGroupName_ = (nodeObj ? nodeObj->calcGroupName() : "");

  //---

  PlotObjs objs;

  menuPlotObjs(objs);

  int ng = numGroups();

  //---

  menu->addSeparator();

  auto *pushAction   = addMenuAction(menu, "Push"   , SLOT(pushSlot()));
  auto *popAction    = addMenuAction(menu, "Pop"    , SLOT(popSlot()));
  auto *popTopAction = addMenuAction(menu, "Pop Top", SLOT(popTopSlot()));

  auto *currentRoot = this->currentRoot(menuGroupName_);
  auto *firstHier   = this->firstHier  (menuGroupName_);

  pushAction  ->setEnabled(ng > 1 || ! objs.empty());
  popAction   ->setEnabled(currentRoot != firstHier || treeData_.currentGroupName != "");
  popTopAction->setEnabled(currentRoot != firstHier || treeData_.currentGroupName != "");

  //---

  auto *obj = (! objs.empty() ? *objs.begin() : nullptr);

  if (obj) {
    auto *expandAction   = addMenuAction(menu, "Expand"  , SLOT(expandSlot()));
    auto *collapseAction = addMenuAction(menu, "Collapse", SLOT(collapseSlot()));

    const auto *hierObj = dynamic_cast<CQChartsTreeMapHierObj *>(obj);

    expandAction  ->setEnabled(hierObj);
    collapseAction->setEnabled(true);
  }

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
}

//---

void
CQChartsTreeMapPlot::
pushSlot()
{
  PlotObjs objs;

  menuPlotObjs(objs);
  if (objs.empty()) return;

  //---

  // get hier node with max depth
  HierNode *hnode = nullptr;

  for (const auto &obj : objs) {
    auto *hierObj1 = dynamic_cast<HierObj *>(obj);

    HierNode *hnode1 = nullptr;

    if (hierObj1)
      hnode1 = hierObj1->hierNode();
    else {
      auto *nodeObj1 = dynamic_cast<NodeObj *>(obj);

      if (nodeObj1) {
        auto *node1 = nodeObj1->node();

        hnode1 = node1->parent();
      }
    }

    if (hnode1) {
      if (! hnode || hnode1->depth() > hnode->depth())
        hnode = hnode1;
    }
  }

  if (! hnode)
    return;

  saveSelection();

  setCurrentRoot(menuGroupName_, hnode, /*update*/true);
}

void
CQChartsTreeMapPlot::
popSlot()
{
  treeData_.currentGroupName = "";

  auto *root = currentRoot(menuGroupName_);

  saveSelection();

  if (root && root->parent())
    setCurrentRoot(menuGroupName_, root->parent(), /*update*/true);
  else
    updateObjs();
}

void
CQChartsTreeMapPlot::
popTopSlot()
{
  popTop(/*update*/true);
}

void
CQChartsTreeMapPlot::
popTop(bool update)
{
  treeData_.currentGroupName = "";

  auto *root      = currentRoot(menuGroupName_);
  auto *firstHier = this->firstHier(menuGroupName_);

  saveSelection();

  if (root != firstHier)
    setCurrentRoot(menuGroupName_, firstHier, update);
  else {
    if (update)
      updateObjs();
  }
}

//---

void
CQChartsTreeMapPlot::
expandSlot()
{
  PlotObjs objs;

  menuPlotObjs(objs);
  if (objs.empty()) return;

  auto *obj = (! objs.empty() ? *objs.begin() : nullptr);

  const auto *hierObj = dynamic_cast<CQChartsTreeMapHierObj *>(obj);
  if (! hierObj) return;

  if (isFollowViewExpand()) {
    if (hierObj->modelInd().isValid())
      expandModelIndex(hierObj->modelInd(), true);
  }
  else {
    hierObj->hierNode()->setExpanded(true);
  }

  drawObjs();
}

void
CQChartsTreeMapPlot::
collapseSlot()
{
  PlotObjs objs;

  menuPlotObjs(objs);
  if (objs.empty()) return;

  auto *obj = (! objs.empty() ? *objs.begin() : nullptr);

  auto *nodeObj = dynamic_cast<CQChartsTreeMapNodeObj *>(obj);

  if (isFollowViewExpand()) {
    auto *hierObj = (nodeObj ? nodeObj->parent() : nullptr);
    if (! hierObj) return;

    if (hierObj->modelInd().isValid())
      expandModelIndex(hierObj->modelInd(), false);
  }
  else {
    auto *hierNode = nodeObj->node()->parent();
    if (! hierNode) return;

    hierNode->setExpanded(false);
  }

  drawObjs();
}

void
CQChartsTreeMapPlot::
menuPlotObjs(PlotObjs &objs) const
{
  selectedPlotObjs(objs);

  if (objs.empty()) {
    auto gpos = view()->menuPos();
    auto pos  = view()->mapFromGlobal(QPointF(gpos.x, gpos.y).toPoint());

    auto w = pixelToWindow(Point(pos));

    plotObjsAtPoint(w, objs, Constraints::SELECTABLE);
  }
}

void
CQChartsTreeMapPlot::
saveSelection()
{
  PlotObjs objs;

  selectedPlotObjs(objs);

  selInds_.clear();

  for (const auto &obj : objs) {
    auto ind = obj->modelInd();

    selInds_.insert(ind);
  }
}

void
CQChartsTreeMapPlot::
restoreSelection(const PlotObjs &objs)
{
  if (! selInds_.empty()) {
    startSelection();

    for (const auto &obj : objs) {
      auto ind = obj->modelInd();

      if (selInds_.find(ind) != selInds_.end())
        obj->setSelected(true);
    }

    endSelection();

    selInds_.clear();
  }
}

//------

void
CQChartsTreeMapPlot::
postResize()
{
  CQChartsPlot::postResize();

  //replaceNodes();

  updateObjs();
}

//----

void
CQChartsTreeMapPlot::
followViewExpandChanged()
{
  if (isFollowViewExpand())
    modelViewExpansionChanged();
  else {
    resetNodeExpansion(true);

    updateRangeAndObjs();
  }
}

void
CQChartsTreeMapPlot::
modelViewExpansionChanged()
{
  if (! isFollowViewExpand())
    return;

  resetNodeExpansion(false);

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
CQChartsTreeMapPlot::
setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet)
{
  bool expanded = (indSet.find(hierNode->ind()) != indSet.end());

  if (hierNode->isExpanded() != expanded)
    hierNode->setExpanded(expanded);

  for (auto &hierNode1 : hierNode->getChildren())
    setNodeExpansion(hierNode1, indSet);
}

void
CQChartsTreeMapPlot::
resetNodeExpansion(bool expanded)
{
  for (const auto &groupName : groupNameSet_) {
    for (auto &hierNode : root(groupName)->getChildren())
      resetNodeExpansion(hierNode, expanded);
  }

  for (auto &hierNode : root("")->getChildren())
    resetNodeExpansion(hierNode, expanded);
}

void
CQChartsTreeMapPlot::
resetNodeExpansion(HierNode *hierNode, bool expanded)
{
  if (hierNode->isExpanded() != expanded)
    hierNode->setExpanded(expanded);

  for (auto &hierNode1 : hierNode->getChildren())
    resetNodeExpansion(hierNode1, expanded);
}

//---

int
CQChartsTreeMapPlot::
numGroups() const
{
  return int(groupNameSet_.size());
}

int
CQChartsTreeMapPlot::
groupNum(const QString &groupName) const
{
  int i = 0;

  for (const auto &groupName1 : groupNameSet_) {
    if (groupName == groupName1)
      return i;

    ++i;
  }

  return -1;
}

//---

bool
CQChartsTreeMapPlot::
isPathExpanded(const QString &path) const
{
  auto p = pathExpanded_.find(path);
  if (p == pathExpanded_.end()) return true;

  return (*p).second;
}

void
CQChartsTreeMapPlot::
setPathExpanded(const QString &path, bool expanded)
{
  pathExpanded_[path] = expanded;
}

void
CQChartsTreeMapPlot::
resetPathExpanded()
{
  pathExpanded_.clear();
}

//---

CQChartsTreeMapHierNode *
CQChartsTreeMapPlot::
createHierNode(HierNode *parent, const QString &name, const QModelIndex &nameInd) const
{
  return new HierNode(this, parent, name, nameInd);
}

CQChartsTreeMapNode *
CQChartsTreeMapPlot::
createNode(HierNode *parent, const QString &name, double size, const QModelIndex &nameInd) const
{
  return new Node(this, parent, name, size, nameInd);
}

CQChartsTreeMapHierObj *
CQChartsTreeMapPlot::
createHierObj(HierNode *hier, HierObj *hierObj, const BBox &rect, const ColorInd &is) const
{
  return new HierObj(this, hier, hierObj, rect, is);
}

CQChartsTreeMapNodeObj *
CQChartsTreeMapPlot::
createNodeObj(Node *node, HierObj *hierObj, const BBox &rect, const ColorInd &is) const
{
  return new NodeObj(this, node, hierObj, rect, is);
}

//---

bool
CQChartsTreeMapPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsTreeMapPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsTreeMapPlot::
createCustomControls()
{
  auto *controls = new CQChartsTreeMapPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsTreeMapHierObj::
CQChartsTreeMapHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                       const BBox &rect, const ColorInd &is) :
 CQChartsTreeMapNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
  if (hier_->ind().isValid())
    setModelInd(hier_->ind());
}

QString
CQChartsTreeMapHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsTreeMapNodeObj::calcId();
}

QString
CQChartsTreeMapHierObj::
calcTipId() const
{
  // auto sep = plot_->calcSeparator();
  //return QString("%1:%2").arg(hier_->hierName(sep)).arg(hier_->hierSize());
  return CQChartsTreeMapNodeObj::calcTipId();
}

QString
CQChartsTreeMapHierObj::
calcGroupName() const
{
  return (hierNode() ? hierNode()->calcGroupName() : "");
}

bool
CQChartsTreeMapHierObj::
inside(const Point &p) const
{
  if (! hier_->isHierVisible())
    return false;

  //---

  auto *pnode = hier_->parent();

  if (pnode && ! pnode->isHierExpanded())
    return false;

  BBox bbox(hier_->x(), hier_->y(), hier_->x() + hier_->w(), hier_->y() + hier_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

bool
CQChartsTreeMapHierObj::
isSelectable() const
{
  if (! hier_->isHierVisible())
    return false;

  //---

  auto *pnode = hier_->parent();

  if (pnode && ! pnode->isHierExpanded())
    return false;

  //---

  if (hier_->isHierExpanded())
    return plot_->isHierSelect();

  return true;
}

bool
CQChartsTreeMapHierObj::
isClickable() const
{
  return true;
}

void
CQChartsTreeMapHierObj::
getObjSelectIndices(Indices &inds) const
{
  return addColumnSelectIndex(inds, plot_->valueColumn());
}

bool
CQChartsTreeMapHierObj::
selectDoubleClick(const Point &, SelMod)
{
  auto *plot = const_cast<CQChartsTreeMapPlot *>(plot_);

  if (plot->isFollowViewExpand()) {
    if (modelInd().isValid())
      plot->expandModelIndex(modelInd(), true);

    plot->drawObjs();
  }
  else {
    hierNode()->setExpanded(! hierNode()->isExpanded());

    plot->drawObjs();
  }

  return true;
}

void
CQChartsTreeMapHierObj::
draw(PaintDevice *device) const
{
  if (! hier_->isHierVisible())
    return;

  //---

  auto *pnode = node()->parent();

  if (pnode && ! pnode->isHierExpanded())
    return;

  //---

  if (! hierNode()->isExpanded())
    return CQChartsTreeMapNodeObj::draw(device);

  //---

  Point p1(hier_->x()             , hier_->y()             );
  Point p2(hier_->x() + hier_->w(), hier_->y() + hier_->h());

  BBox bbox(p1.x, p2.y, p2.x, p1.y);

  //---

  // calc hier node stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw rectangle
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawRect(bbox);

  device->resetColorNames();

  //---

  if (hier_->isShowTitle())
    drawHeader(device, bbox, updateState);
}

void
CQChartsTreeMapHierObj::
drawHeader(PaintDevice *device, const BBox &bbox, bool updateState) const
{
  // get label (name)
  auto sep = plot_->calcSeparator();

  auto name = (plot_->isTitleHierName() ? hier_->hierName(sep) : hier_->name());

  //---

  PenBrush penBrush;

  calcHeaderPenBrush(penBrush, updateState);

  //---

  // calc text pen
  auto tPenBrush = penBrush;

  auto tc = plot_->interpHeaderTextColor(ColorInd());

  plot_->setPen(tPenBrush, PenData(true, tc, plot_->headerTextAlpha()));

  if (updateState)
    plot_->updateObjPenBrushState(this, tPenBrush);

  //---

  device->save();

  //---

  // set font
  plot_->setPainterFont(device, plot_->headerTextFont());

  //---

  // check if text visible (font dependent)
  auto pbbox = plot_->windowToPixel(bbox);

  bool visible = true;

  if (plot_->isTitleTextClipped()) {
    QFontMetricsF fm(device->font());

    double minTextWidth  = fm.horizontalAdvance("X") + 4;
    double minTextHeight = fm.height() + 4;

    visible = (pbbox.getWidth() >= minTextWidth && pbbox.getHeight() >= minTextHeight);
  }

  //---

  if (visible) {
    // get header bbox
    double m = plot_->titleMargin(); // margin in pixels

    double hh = plot_->calcTitleHeight(); // title height in pixels

    BBox pbbox1(pbbox.getXMin(), pbbox.getYMin(),
                pbbox.getXMax(), pbbox.getYMin() + hh);
    BBox pbbox2(pbbox.getXMin() + m, pbbox.getYMin(),
                pbbox.getXMax() - m, pbbox.getYMin() + hh);

    auto bbox1 = plot_->pixelToWindow(pbbox1);
    auto bbox2 = plot_->pixelToWindow(pbbox2);

    //---

    // draw header rectangle
    device->setColorNames();

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawRect(bbox1);

    device->resetColorNames();

    //---

    // draw header label
    plot_->charts()->setContrastColor(penBrush.brush.color());

    auto textOptions = plot_->headerTextOptions(device);

    textOptions.clipped = plot_->isTitleTextClipped();

    textOptions = plot_->adjustTextOptions(textOptions);

    device->setPen(tPenBrush.pen);

    CQChartsDrawUtil::drawTextInBox(device, bbox2, name, textOptions);

    plot_->charts()->resetContrastColor();
  }

  //---

  device->restore();
}

void
CQChartsTreeMapHierObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  if (numColorIds_ < 0)
    numColorIds_ = plot_->numColorIds(calcGroupName());

  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpHierStrokeColor(colorInd);

  if (isChildSelected())
    bc.setAlphaF(1.0);

  auto hierColor = hier_->interpColor(plot_, plot_->hierFillColor(), colorInd, numColorIds_);

  QColor fc;

  if (hierNode()->isExpanded()) {
    if (plot_->hierColorType() != Plot::HierColorType::PARENT_VALUE &&
        plot_->hierColorType() != Plot::HierColorType::GLOBAL_VALUE) {
      auto c = plot_->interpHierFillColor(colorInd);

      fc = CQChartsUtil::blendColors(c, hierColor, 0.8);
    }
    else
      fc = hierColor;
  }
  else
    fc = hierColor;

  plot_->setPenBrush(penBrush, plot_->hierPenData(bc), plot_->hierBrushData(fc));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsTreeMapHierObj::
calcHeaderPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto bc = plot_->interpHeaderStrokeColor(colorInd);
  auto fc = plot_->interpHeaderFillColor(colorInd);

  plot_->setPenBrush(penBrush, plot_->headerPenData(bc), plot_->headerBrushData(fc));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsTreeMapNodeObj::
CQChartsTreeMapNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                       const BBox &rect, const ColorInd &is) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
  setDetailHint(DetailHint::MAJOR);

  if (node_->ind().isValid())
    setModelInd(node_->ind());
}

QString
CQChartsTreeMapNodeObj::
calcId() const
{
//if (node_->isFiller())
//  return hierObj_->calcId();

  auto ind1 = plot_->unnormalizeIndex(node_->ind());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3").arg(typeName()).
    arg(node_->isFiller() ? node_->parent()->name() : node_->name()).arg(node_->hierSize());
}

QString
CQChartsTreeMapNodeObj::
calcTipId() const
{
//if (node_->isFiller())
//  return hierObj_->calcTipId();

  CQChartsTableTip tableTip;

  auto sep = plot_->calcSeparator();

  if (plot_->idColumn().isValid())
    tableTip.addTableRow("Name", (node_->isFiller() ?
      node_->parent()->name() : node_->name()));
  else
    tableTip.addTableRow("Name", (node_->isFiller() ?
      node_->parent()->hierName(sep) : node_->hierName(sep)));

  auto ind1 = plot_->unnormalizeIndex(node_->isFiller() ? node_->parent()->ind() : node_->ind());

  QString valueName;

  if (plot_->valueColumn().isValid())
    valueName = plot_->columnHeaderName(plot_->valueColumn(), /*tip*/true);

  if (valueName == "")
    valueName = "Value";

  tableTip.addTableRow(valueName, node_->hierSize());

  if (plot_->colorColumn().isValid()) {
    ModelIndex colorInd1(plot_, ind1.row(), plot_->colorColumn(), ind1.parent());

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
CQChartsTreeMapNodeObj::
calcGroupName() const
{
  return (node() ? node()->calcGroupName() : "");
}

bool
CQChartsTreeMapNodeObj::
inside(const Point &p) const
{
  auto *pnode = node()->parent();

  if (pnode && ! pnode->isHierVisible())
    return false;

  if (pnode && ! pnode->isHierExpanded())
    return false;

  BBox bbox(node()->x(), node()->y(), node()->x() + node()->w(), node()->y() + node()->h());

  if (bbox.inside(p))
    return true;

  return false;
}

void
CQChartsTreeMapNodeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->nameColumns())
    addColumnSelectIndex(inds, c);

  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsTreeMapNodeObj::
draw(PaintDevice *device) const
{
  auto *pnode = node()->parent();

  if (pnode && ! pnode->isHierVisible())
    return;

  if (pnode && ! pnode->isHierExpanded())
    return;

  //---

  if (this->isMinArea())
    return;

  auto p1 = plot_->windowToPixel(Point(node_->x()             , node_->y()             ));
  auto p2 = plot_->windowToPixel(Point(node_->x() + node_->w(), node_->y() + node_->h()));

  bool isNodePoint = this->isNodePoint();

  BBox  pbbox;
  Point point;

  if (isNodePoint)
    point = Point(CMathUtil::avg(p1.x, p2.x), CMathUtil::avg(p1.y, p2.y));
  else
    pbbox = BBox(p1.x + 1, p2.y + 1, p2.x - 1, p1.y - 1);

  //---

  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw rectangle
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if (isNodePoint)
    device->drawPoint(plot_->pixelToWindow(point));
  else
    device->drawRect(plot_->pixelToWindow(pbbox));

  device->resetColorNames();

  //---

  if (isNodePoint)
    return;

  //---

  if (plot_->isTextVisible())
    drawText(device, pbbox, updateState);
}

void
CQChartsTreeMapNodeObj::
drawText(PaintDevice *device, const BBox &pbbox, bool updateState) const
{
  // get labels (name and optional size)
  QStringList strs;

  QString name;

  if (plot_->isHierName()) {
    auto sep = plot_->calcSeparator();

    name = (! node_->isFiller() ? node_->hierName(sep) : node_->parent()->hierName(sep));

    int nh = plot_->numSkipHier();

    if (nh > 0) {
      auto strs = name.split(sep);

      int ns = strs.size();

      QStringList strs1;

      for (int i = nh; i < ns; ++i)
        strs1 += strs[i];

      name = strs1.join(sep);
    }
  }
  else
    name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel()) {
    if (node_->isHier())
      strs.push_back(QString::number(node_->hierSize()));
    else
      strs.push_back(QString::number(node_->size()));
  }

  //---

  PenBrush penBrush;

  calcPenBrushNodePoint(penBrush, /*isNodePoint*/false, updateState);

  plot_->charts()->setContrastColor(penBrush.brush.color());

  //---

  // calc text pen
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
  plot_->setPainterFont(device, plot_->textFont());

  //---

  // check if text visible (font dependent)
  bool visible = true;

  if (plot_->isTextClipped()) {
    QFontMetricsF fm(device->font());

    double minTextWidth  = fm.horizontalAdvance("X") + 4;
    double minTextHeight = fm.height() + 4;

    visible = (pbbox.getWidth() >= minTextWidth && pbbox.getHeight() >= minTextHeight);
  }

  //---

  // draw label
  if (visible) {
    auto textOptions = plot_->textOptions();

    textOptions.clipped    = plot_->isTextClipped();
    textOptions.formatSeps = plot_->calcSeparator();

    textOptions = plot_->adjustTextOptions(textOptions);

    device->setPen(tPenBrush.pen);

    auto ibbox = pbbox.adjusted(3, 3, -3, -3);

    auto bbox1 = plot_->pixelToWindow(ibbox);

    if (! plot_->isValueLabel()) {
      CQChartsDrawUtil::drawTextInBox(device, bbox1, name, textOptions);
    }
    else {
      if (plot_->isTextClipped())
        device->setClipRect(bbox1);

      if (textOptions.formatted) {
        auto str1 = strs.join('\n');

        CQChartsDrawUtil::drawTextInBox(device, bbox1, str1, textOptions);
      }
      else
        CQChartsDrawUtil::drawStringsInBox(device, bbox1, strs, textOptions);
    }
  }

  //---

  plot_->charts()->resetContrastColor();

  device->restore();
}

void
CQChartsTreeMapNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  bool isNodePoint = this->isNodePoint();

  calcPenBrushNodePoint(penBrush, isNodePoint, updateState);
}

void
CQChartsTreeMapNodeObj::
calcPenBrushNodePoint(PenBrush &penBrush, bool isNodePoint, bool updateState) const
{
  if (numColorIds_ < 0)
    numColorIds_ = plot_->numColorIds(calcGroupName());

  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = node_->interpColor(plot_, plot_->fillColor(), colorInd, numColorIds_);

  // is node single pixel point
  if (isNodePoint) {
    if      (plot_->isFilled())
      plot_->setPenBrush(penBrush, PenData(true, fc, plot_->fillAlpha()), plot_->brushData(fc));
    else if (plot_->isStroked())
      plot_->setPenBrush(penBrush, plot_->penData(bc), BrushData(true, bc, plot_->strokeAlpha()));
  }
  // is normal node
  else {
    plot_->setPenBrush(penBrush, plot_->penData(bc), plot_->brushData(fc));
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

bool
CQChartsTreeMapNodeObj::
isMinArea() const
{
  auto &minArea = plot_->minArea();

  if (! minArea.isValid())
    return false;

  if      (minArea.units() == Units::PLOT) {
    return (minArea.value() > (node_->w()*node_->h()));
  }
  else if (minArea.units() == Units::PIXEL) {
    auto pw = plot_->windowToPixelWidth (node_->w());
    auto ph = plot_->windowToPixelHeight(node_->h());

    return (minArea.value() > (pw*ph));
  }
  else
    return false;
}

bool
CQChartsTreeMapNodeObj::
isNodePoint() const
{
  auto p1 = plot_->windowToPixel(Point(node_->x()             , node_->y()             ));
  auto p2 = plot_->windowToPixel(Point(node_->x() + node_->w(), node_->y() + node_->h()));

  double pw = std::abs(p2.x - p1.x) - 2;
  double ph = std::abs(p2.y - p1.y) - 2;

  return (pw <= 1.5 || ph <= 1.5);
}

bool
CQChartsTreeMapNodeObj::
isChildSelected() const
{
  for (const auto &child : children_) {
    if (child->isSelected())
      return true;
  }

  for (const auto &child : children_) {
    if (child->isChildSelected())
      return true;
  }

  return false;
}

//------

CQChartsTreeMapHierNode::
CQChartsTreeMapHierNode(const Plot *plot, HierNode *parent, const QString &name,
                        const QModelIndex &ind) :
 CQChartsTreeMapNode(plot, parent, name, 0.0, ind)
{
  if (parent_)
    parent_->addChild(this);
}

CQChartsTreeMapHierNode::
~CQChartsTreeMapHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

void
CQChartsTreeMapHierNode::
setExpanded(bool b)
{
  expanded_ = b;

  auto sep = plot_->calcSeparator();

  const_cast<Plot *>(plot_)->setPathExpanded(hierName(sep), expanded_);
}

bool
CQChartsTreeMapHierNode::
isHierVisible() const
{
  if (! isVisible())
    return false;

  if (parent() && ! parent()->isHierVisible())
    return false;

  return true;
}

bool
CQChartsTreeMapHierNode::
isHierExpanded() const
{
  auto groupName = calcGroupName();

  if (plot_->currentRoot(groupName) == this)
    return true;

  if (! isExpanded())
    return false;

  if (parent() && ! parent()->isHierExpanded())
    return false;

  return true;
}

double
CQChartsTreeMapHierNode::
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
CQChartsTreeMapHierNode::
addChild(HierNode *child)
{
  children_.push_back(child);
}

void
CQChartsTreeMapHierNode::
removeChild(HierNode *child)
{
  auto nc = children_.size();

  size_t i = 0;

  for ( ; i < nc; ++i)
    if (children_[i] == child)
      break;

  assert(i < nc);

  delete children_[i];

  ++i;

  for ( ; i < nc; ++i)
    children_[i - 1] = children_[i];

  children_.pop_back();
}

void
CQChartsTreeMapHierNode::
packNodes(double x, double y, double w, double h)
{
  double whh = plot()->windowHeaderHeight();
  double wmw = plot()->windowMarginWidth();

  //---

  // check if title bar should be shown
  showTitle_ = plot()->isTitles();

  if (plot()->isTitleAutoHide()) {
    double maxExtent = 0.2;

    if (plot()->titleMaxExtent().isSet())
      maxExtent = CMathUtil::clamp(*plot()->titleMaxExtent().value(), 0.0, 1.0);

    if (whh > h*maxExtent)
      showTitle_ = false;
  }

  bool isSplitGroup = false;

  if (plot()->isSplitGroups() && groupName() != "")
    isSplitGroup = true;

  if (! isSplitGroup && ! parent())
    showTitle_ = false;

  if (plot()->titleDepth() > 0 && depth() > plot()->titleDepth())
    showTitle_ = false;

  //---

  double dh = (showTitle_ ? whh : 0.0);
  double m  = (w > wmw ? wmw : 0.0);

  // make single list of nodes to pack
  Nodes nodes;

  for (const auto &child : children_)
    nodes.push_back(child);

  for (const auto &node : nodes_)
    nodes.push_back(node);

  // sort nodes by size (largest to smallest)
  std::sort(nodes.begin(), nodes.end(), CQChartsTreeMapNodeCmp());

  //std::cerr << name() << "\n";
  //for (uint i = 0; i < nodes.size(); ++i)
  //  std::cerr << " " << nodes[i]->name() << ":" << nodes[i]->hierSize() << "\n";

  packSubNodes(x + m/2, y + m/2, w - m, h - dh - m, nodes);
}

void
CQChartsTreeMapHierNode::
packSubNodes(double x, double y, double w, double h, const Nodes &nodes)
{
  // place nodes
  auto n = nodes.size();
  if (n == 0) return;

  if (n >= 2) {
    Nodes  nodes1, nodes2;
    double size1 = 0.0, size2 = 0.0;

#if 1
    double size12 = 0.0;

    for (size_t i = 0; i < n; ++i)
      size12 += nodes[i]->hierSize();

    double hsize = size12/2;

    size_t i = 0;

    for ( ; i < n - 1; ++i) {
      if (size1 > hsize)
        break;

      size1 += nodes[i]->hierSize();

      nodes1.push_back(nodes[i]);
    }

    for ( ; i < n; ++i) {
      size2 += nodes[i]->hierSize();

      nodes2.push_back(nodes[i]);
    }
#else
    int n1 = n/2;

    for (int i = 0; i < n1; ++i) {
      size1 += nodes[i]->hierSize();

      nodes1.push_back(nodes[i]);
    }

    for (int i = n1; i <  n; ++i) {
      size2 += nodes[i]->hierSize();

      nodes2.push_back(nodes[i]);
    }

    double size12 = size1 + size2;
#endif

    // split area = (w*h) if largest direction
    // e.g. split at w1. area1 = w1*h; area2 = (w - w1)*h;
    // area1/area2 = w1/(w - w1) = size1/size2;
    // w1*size2 = w*size1 - w1*size1;
    // w1 = (w*size1)/(size1 + size2);

    if (size12 == 0.0)
      return;

    assert(nodes1.size() > 0 && nodes2.size() > 0);

    assert(size2); // ?

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
    auto *node = nodes[0];

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
addNode(Node *node)
{
  nodes_.push_back(node);
}

void
CQChartsTreeMapHierNode::
removeNode(Node *node)
{
  auto n = nodes_.size();

  size_t i = 0;

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
CQChartsTreeMapHierNode::
interpColor(const Plot *plot, const Color &c, const ColorInd &colorInd, int n) const
{
  if (plot->hierColorType() == Plot::HierColorType::BLEND) {
    // color is blended from child
    using Colors = std::vector<QColor>;

    Colors colors;

    for (auto &child : children_)
      colors.push_back(child->interpColor(plot, c, colorInd, n));

    for (auto &node : nodes_)
      colors.push_back(node->interpColor(plot, c, colorInd, n));

    if (! colors.empty())
      return CQChartsUtil::blendColors(colors);
  }

  //---

  ColorInd colorInd1 = colorInd;

  if      (plot->hierColorType() == Plot::HierColorType::PARENT_VALUE) {
    if (parent())
      colorInd1 = ColorInd(hierSize()/parent()->hierSize());
  }
  else if (plot->hierColorType() == Plot::HierColorType::GLOBAL_VALUE) {
    auto groupName = calcGroupName();

    auto *root = plot_->root(groupName);

    colorInd1 = ColorInd(CMathUtil::norm(hierSize(), 0.0, root->hierSize()));
  }

  //----

  // color by group palette index
  if (plot->isSplitGroups() && plot->isGroupPalette()) {
    if (ng_ < 0) {
      auto groupName = calcGroupName();

      ig_ = plot_->groupNum(groupName);
      ng_ = plot_->numGroups();
    }

    //---

    return plot->interpColor(c, ig_, ng_, colorInd1);
  }
  else
    return plot->interpColor(c, colorInd1);
}

QString
CQChartsTreeMapHierNode::
calcGroupName() const
{
  if (groupName_ != "")
    return groupName_;

  if (parent())
    return parent()->calcGroupName();

  return "";
}

//------

CQChartsTreeMapNode::
CQChartsTreeMapNode(const Plot *plot, HierNode *parent, const QString &name, double size,
                    const QModelIndex &ind) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind)
{
}

QString
CQChartsTreeMapNode::
hierName(const QString &sep) const
{
  if (hierName_ == "" || sep != hierNameSep_) {
    auto groupName = calcGroupName();

    if (parent() && parent() != plot_->root(groupName))
      hierName_ = parent()->hierName(sep) + sep + name();
    else
      hierName_ = name();
  }

  return hierName_;
}

void
CQChartsTreeMapNode::
setPosition(double x, double y, double w, double h)
{
  assert(! CMathUtil::isNaN(x) && ! CMathUtil::isNaN(y) &&
         ! CMathUtil::isNaN(w) && ! CMathUtil::isNaN(h));

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
rootNode(HierNode *root) const
{
  auto *parent = this->parent();

  while (parent && parent->parent() && parent->parent() != root)
    parent = parent->parent();

  return parent;
}

QColor
CQChartsTreeMapNode::
interpColor(const Plot *plot, const Color &c, const ColorInd &colorInd, int n) const
{
  Color    color1    = c;
  ColorInd colorInd1 = colorInd;

  if      (color().isValid())
    color1 = color();
  else if (plot_->nodeColorType() == Plot::NodeColorType::ID)
    colorInd1 = ColorInd(colorId(), n);
  else if (plot_->nodeColorType() == Plot::NodeColorType::PARENT_VALUE)
    colorInd1 = ColorInd(size()/parent()->hierSize());
  else if (plot_->nodeColorType() == Plot::NodeColorType::GLOBAL_VALUE) {
    if (plot->hasValueRange())
      colorInd1 = ColorInd(CMathUtil::norm(size(), plot->minValue(), plot->maxValue()));
    else
      colorInd1 = ColorInd(1.0);
  }

  if (plot->isSplitGroups() && plot->isGroupPalette()) {
    if (ng_ < 0) {
      auto groupName = calcGroupName();

      ig_ = plot_->groupNum(groupName);
      ng_ = plot_->numGroups();
    }

    //---

    return plot->interpColor(color1, ig_, ng_, colorInd1);
  }
  else
    return plot->interpColor(color1, colorInd1);
}

QString
CQChartsTreeMapNode::
calcGroupName() const
{
  if (parent())
    return parent()->calcGroupName();

  return "";
}

//------

CQChartsTreeMapPlotCustomControls::
CQChartsTreeMapPlotCustomControls(CQCharts *charts) :
 CQChartsHierPlotCustomControls(charts, "treemap")
{
}

void
CQChartsTreeMapPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsTreeMapPlotCustomControls::
addWidgets()
{
  addHierColumnWidgets();

  addColorColumnWidgets();

  addOptionsWidgets();
}

void
CQChartsTreeMapPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  headerCheck_     = CQUtil::makeLabelWidget<QCheckBox>("Show Header", "headerCheck");
  valueCheck_      = CQUtil::makeLabelWidget<QCheckBox>("Show Value" , "valueCheck");
  followViewCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Follow View", "followViewCheck_");

  addFrameColWidget(optionsFrame_, headerCheck_);
  addFrameColWidget(optionsFrame_, valueCheck_);
  addFrameColWidget(optionsFrame_, followViewCheck_);
}

void
CQChartsTreeMapPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::optConnectDisconnect(b,
    headerCheck_, SIGNAL(stateChanged(int)), this, SLOT(headerSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    valueCheck_, SIGNAL(stateChanged(int)), this, SLOT(valueSlot()));
  CQChartsWidgetUtil::optConnectDisconnect(b,
    followViewCheck_, SIGNAL(stateChanged(int)), this, SLOT(followViewSlot()));

  CQChartsHierPlotCustomControls::connectSlots(b);
}

void
CQChartsTreeMapPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsTreeMapPlot *>(plot);

  CQChartsHierPlotCustomControls::setPlot(plot);
}

void
CQChartsTreeMapPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  headerCheck_    ->setChecked(plot_->isTitles());
  valueCheck_     ->setChecked(plot_->isValueLabel());
  followViewCheck_->setChecked(plot_->isFollowViewExpand());

  CQChartsHierPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsTreeMapPlotCustomControls::
headerSlot()
{
  plot_->setTitles(headerCheck_->isChecked());
}

void
CQChartsTreeMapPlotCustomControls::
valueSlot()
{
  plot_->setValueLabel(valueCheck_->isChecked());
}

void
CQChartsTreeMapPlotCustomControls::
followViewSlot()
{
  plot_->setFollowViewExpand(followViewCheck_->isChecked());
}

CQChartsColor
CQChartsTreeMapPlotCustomControls::
getColorValue()
{
  return plot_->fillColor();
}

void
CQChartsTreeMapPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setFillColor(c);
}
