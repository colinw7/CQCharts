#include <CQChartsBubblePlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsTitle.h>
#include <CQCharts.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QPainter>

CQChartsBubblePlotType::
CQChartsBubblePlotType()
{
}

void
CQChartsBubblePlotType::
addParameters()
{
  startParameterGroup("Bubble");

  addColumnParameter("name", "Name", "nameColumn").
   setString().setTip("Name Column");

  addColumnParameter("value", "Value", "valueColumn").
   setNumeric().setRequired().setTip("Value Column");

  endParameterGroup();

  //---

  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsBubblePlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Bubble Plot").
    h3("Summary").
     p("Draws circles represent a set of data values and packs then into the "
       "smallest enclosing circle.").
    h3("Columns").
     p("The values are taken from the value column. The value name can be specified "
       "in the name column.").
    h3("Options").
     p("The value can be displayed at the center of the bubble along with the name.").
     p("Bubbles can be sorted by value or displayed in model order.").
    h3("Customization").
     p("The bubble style (fill and stroke) and text style can be specified.").
    h3("Limitations").
     p("A user defined range cannot be specified, no axes or key are displayed, "
       "logarithmic values are not supported and probing is not allowed.").
    h3("Example").
     p(IMG("images/bubbleplot.png"));
}

CQChartsPlot *
CQChartsBubblePlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsBubblePlot(view, model);
}

//------

CQChartsBubblePlot::
CQChartsBubblePlot(View *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("bubble"), model),
 CQChartsObjShapeData<CQChartsBubblePlot>(this),
 CQChartsObjTextData <CQChartsBubblePlot>(this)
{
}

CQChartsBubblePlot::
~CQChartsBubblePlot()
{
  term();
}

//---

void
CQChartsBubblePlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  setExactValue(false);

  //---

  setFillColor(Color(Color::Type::PALETTE));

  setStroked(true);
  setFilled (true);

  setTextContrast(true);
  setTextFontSize(12.0);

  setTextColor(Color(Color::Type::INTERFACE_VALUE, 1));

  setOuterMargin(PlotMargin(Length("4px"), Length("4px"), Length("4px"), Length("4px")));

  marginSet_ = false;

  addTitle();
}

void
CQChartsBubblePlot::
term()
{
  delete nodeData_.root;
}

//---

void
CQChartsBubblePlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBubblePlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBubblePlot::
setValueLabel(bool b)
{
  CQChartsUtil::testAndSet(valueLabel_, b, [&]() { drawObjs(); } );
}

void
CQChartsBubblePlot::
setSorted(bool b)
{
  CQChartsUtil::testAndSet(sortData_.enabled, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsBubblePlot::
setSortReverse(bool b)
{
  CQChartsUtil::testAndSet(sortData_.reverse, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsBubblePlot::
setTextFontSize(double s)
{
  if (s != textData_.font().pointSizeF()) {
    auto f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f);

    drawObjs();
  }
}

//---

void
CQChartsBubblePlot::
setColorById(bool b)
{
  CQChartsUtil::testAndSet(colorById_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsBubblePlot::
setMinSize(const OptReal &r)
{
  CQChartsUtil::testAndSet(minSize_, r, [&]() { updateRangeAndObjs(); } );
}

//----

void
CQChartsBubblePlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "valueColumn", "value", "Value column");

  addGroupingProperties();

  // options
  addProp("options", "valueLabel" , "", "Show value label");
  addProp("options", "sorted"     , "", "Sort values by size (default small to large)");
  addProp("options", "sortReverse", "", "Sort values large to small");

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
    CQChartsTextOptions::ValueType::CONTRAST | CQChartsTextOptions::ValueType::SCALED |
    CQChartsTextOptions::ValueType::CLIP_LENGTH |
    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // color map
  addColorMapProperties();
}

//---

CQChartsBubbleHierNode *
CQChartsBubblePlot::
currentRoot() const
{
  return nodeData_.root;
}

//---

CQChartsGeom::Range
CQChartsBubblePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsBubblePlot::calcRange");

  double r = 1.0;

  Range dataRange;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  //---

  initGroupData(Columns(), nameColumn(), /*hier*/true);

  //---

  return dataRange;
}

//------

void
CQChartsBubblePlot::
clearPlotObjects()
{
  resetNodes();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsBubblePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsBubblePlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsBubblePlot *>(this);

  th->clearErrors();

  //---

  if (! marginSet_) {
    if (title()->textStr().length() > 0) {
      th->setOuterMargin(PlotMargin(Length("4px"), Length("5%"), Length("4px"), Length("4px")));

      marginSet_ = true;
    }
  }

  // init value sets
//initValueSets();

  //---

  // check columns
  bool columnsValid = true;

  // value column required
  // name, id, color columns optional

  if (! checkColumn(valueColumn(), "Value", th->valueColumnType_, /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(idColumn   (), "Id"   )) columnsValid = false;
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
CQChartsBubblePlot::
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
CQChartsBubblePlot::
resetNodes()
{
  delete nodeData_.root;

  nodeData_.root = nullptr;

  groupHierNodes_.clear();
}

void
CQChartsBubblePlot::
initNodes() const
{
  auto *th = const_cast<CQChartsBubblePlot *>(this);

  th->nodeData_.hierInd = 0;

  th->nodeData_.root = new HierNode(this, nullptr, "<root>");

  th->nodeData_.root->setDepth(0);
  th->nodeData_.root->setHierInd(th->nodeData_.hierInd++);

  //---

  loadModel();

  //---

  replaceNodes();
}

void
CQChartsBubblePlot::
replaceNodes() const
{
  placeNodes(nodeData_.root);
}

void
CQChartsBubblePlot::
placeNodes(HierNode *hier) const
{
  auto *th = const_cast<CQChartsBubblePlot *>(this);

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
CQChartsBubblePlot::
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
CQChartsBubblePlot::
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
CQChartsBubblePlot::
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
CQChartsBubblePlot::
colorNode(Node *node) const
{
  if (! node->color().isValid()) {
    auto *th = const_cast<CQChartsBubblePlot *>(this);

    node->setColorId(th->nextColorId());
  }
}

void
CQChartsBubblePlot::
loadModel() const
{
  class RowVisitor : public ModelVisitor {
   public:
    using Plot = CQChartsBubblePlot;

   public:
    RowVisitor(const Plot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get name and associated model index for row
      QString     name;
      QModelIndex nameInd;

      bool hasName = getName(data, name, nameInd);

      //---

      double size = 1.0;

      if (! getSize(data, size))
        return State::SKIP;

      if (plot_->minSize().isSet()) {
        if (size < plot_->minSize().real())
          return State::SKIP;
      }

      //---

      auto nameInd1 = plot_->normalizeIndex(nameInd);

      auto *pnode = parentHier(data);

      if (! hasName) {
        name = pnode->hierName();

        QStringList names = name.split("/", QString::KeepEmptyParts);

        if (names.size() > 0)
          name = names.back();
      }

      auto *node = plot_->addNode(pnode, name, size, nameInd1);

      if (node) {
        Color color;

        if (plot_->colorColumnColor(data.row, data.parent, color))
          node->setColor(color);
      }

      return State::OK;
    }

    void termVisit() override {
      for (const auto &pg : groupInds_) {
        int         groupInd  = pg.first;
        const auto &modelInds = pg.second;

        int size = modelInds.size();

        if (! size)
          continue;

        if (plot_->minSize().isSet()) {
          if (size < plot_->minSize().real())
            continue;
        }

        VisitData data;

        data.row    = modelInds[0].row();
        data.parent = modelInds[0].parent();

        QString     name;
        QModelIndex nameInd;

        (void) getName(data, name, nameInd);

        QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

        QString groupName = plot_->groupIndName(groupInd, /*hier*/false);

        auto *pnode = parentHier(data);
        auto *node  = plot_->addNode(pnode, groupName, size, nameInd1);

        for (const auto &ind : modelInds) {
          ModelIndex nameModelInd;

          auto *plot = const_cast<Plot *>(plot_);

          if (plot->nameColumn().isValid())
            nameModelInd = ModelIndex(plot, ind.row(), plot_->nameColumn(), ind.parent());
          else
            nameModelInd = ModelIndex(plot, ind.row(), plot_->idColumn(), ind.parent());

          auto nameInd  = plot_->modelIndex(nameModelInd);
          auto nameInd1 = plot_->normalizeIndex(nameInd);

          node->addInd(nameInd1);
        }
      }
    }

   private:
    HierNode *parentHier(const VisitData &data) const {
      if (plot_->valueColumn().isGroup())
        return plot_->currentRoot();

      auto *plot = const_cast<Plot *>(plot_);

      ModelIndex ind(plot, data.row, plot_->valueColumn(), data.parent);

      std::vector<int> groupInds = plot_->rowHierGroupInds(ind);

      auto *hierNode = plot_->currentRoot();

      for (std::size_t i = 0; i < groupInds.size(); ++i) {
        int groupInd = groupInds[i];

        hierNode = plot_->groupHierNode(hierNode, groupInd);
      }

      return hierNode;
    }

    bool getName(const VisitData &data, QString &name, QModelIndex &nameInd) const {
      if (! plot_->nameColumn().isValid() && ! plot_->idColumn().isValid())
        return false;

      auto *plot = const_cast<Plot *>(plot_);

      ModelIndex nameModelInd;

      if (plot_->nameColumn().isValid())
        nameModelInd = ModelIndex(plot, data.row, plot_->nameColumn(), data.parent);
      else
        nameModelInd = ModelIndex(plot, data.row, plot_->idColumn(), data.parent);

      nameInd = plot_->modelIndex(nameModelInd);

      bool ok;
      name = plot_->modelString(nameModelInd, ok);

      return ok;
    }

    bool getSize(const VisitData &data, double &size) const {
      size = 1.0;

      if (! plot_->valueColumn().isValid())
        return true;

      auto *plot = const_cast<Plot *>(plot_);

      ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);

      if (plot_->valueColumn().isGroup()) {
        int groupInd = plot_->rowGroupInd(valueModelInd);

        groupInds_[groupInd].push_back(valueModelInd);

        return false;
      }

      bool ok = true;

      if      (plot_->valueColumnType() == ColumnType::REAL)
        size = plot_->modelReal(valueModelInd, ok);
      else if (plot_->valueColumnType() == ColumnType::INTEGER)
        size = (double) plot_->modelInteger(valueModelInd, ok);
      else if (plot_->valueColumnType() == ColumnType::STRING)
        size = 1.0;
      else
        ok = false;

      if (ok && size <= 0.0)
        ok = false;

      return ok;
    }

   private:
    using ModelInds = std::vector<ModelIndex>;
    using GroupInds = std::map<int, ModelInds>;

    const Plot*       plot_ { nullptr };
    mutable GroupInds groupInds_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

CQChartsBubbleHierNode *
CQChartsBubblePlot::
groupHierNode(HierNode *parent, int groupInd) const
{
  if (groupInd < 0)
    return parent;

  auto p = groupHierNodes_.find(groupInd);

  if (p != groupHierNodes_.end())
    return (*p).second;

  auto *th = const_cast<CQChartsBubblePlot *>(this);

  QString name = groupIndName(groupInd, /*hier*/true);

  QString name1 = name;

  QStringList names = name.split("/", QString::KeepEmptyParts);

  if (names.size() > 0)
    name1 = names.back();

  QModelIndex ind;

  auto *hierNode = th->addHierNode(parent, name1, ind);

  auto p1 = th->groupHierNodes_.insert(th->groupHierNodes_.end(),
              GroupHierNodes::value_type(groupInd, hierNode));

  return (*p1).second;
}

CQChartsBubbleHierNode *
CQChartsBubblePlot::
addHierNode(HierNode *hier, const QString &name, const QModelIndex &nameInd) const
{
  auto *th = const_cast<CQChartsBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  auto *hier1 = new HierNode(this, hier, name, nameInd1);

  hier1->setDepth(depth1);

  hier1->setHierInd(th->nodeData_.hierInd++);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return hier1;
}

CQChartsBubbleNode *
CQChartsBubblePlot::
addNode(HierNode *hier, const QString &name, double size, const QModelIndex &nameInd) const
{
  auto *th = const_cast<CQChartsBubblePlot *>(this);

  int depth1 = hier->depth() + 1;

  QModelIndex nameInd1 = normalizeIndex(nameInd);

  auto *node = new Node(this, hier, name, size, nameInd1);

  node->setDepth(depth1);

  hier->addNode(node);

  th->nodeData_.maxDepth = std::max(nodeData_.maxDepth, depth1);

  return node;
}

//------

void
CQChartsBubblePlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//------

bool
CQChartsBubblePlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsBubblePlot::
execDrawForeground(PaintDevice *device) const
{
  drawBounds(device, currentRoot());
}

void
CQChartsBubblePlot::
drawBounds(PaintDevice *device, HierNode *hier) const
{
  if (! hier)
    return;

  double xc = hier->x();
  double yc = hier->y();
  double r  = hier->radius();

  //---

  Point p1(xc - r, yc - r);
  Point p2(xc + r, yc + r);

  BBox bbox(p1, p2);

  //---

  // draw bubble
  PenBrush penBrush;

  auto bc = interpStrokeColor(ColorInd());

  setPenBrush(penBrush, PenData(true, bc), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawEllipse(bbox);
}

//---

CQChartsBubbleHierObj *
CQChartsBubblePlot::
createHierObj(HierNode *hier, HierObj *hierObj, const BBox &rect, const ColorInd &is) const
{
  return new HierObj(this, hier, hierObj, rect, is);
}

CQChartsBubbleNodeObj *
CQChartsBubblePlot::
createNodeObj(Node *node, HierObj *hierObj, const BBox &rect, const ColorInd &is) const
{
  return new NodeObj(this, node, hierObj, rect, is);
}

//------

CQChartsBubbleHierObj::
CQChartsBubbleHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                      const BBox &rect, const ColorInd &is) :
 CQChartsBubbleNodeObj(plot, hier, hierObj, rect, is), hier_(hier)
{
  if (hier_->ind().isValid())
    setModelInd(hier_->ind());
}

QString
CQChartsBubbleHierObj::
calcId() const
{
  //return QString("%1:%2").arg(hier_->name()).arg(hier_->hierSize());
  return CQChartsBubbleNodeObj::calcId();
}

QString
CQChartsBubbleHierObj::
calcTipId() const
{
  //return QString("%1:%2").arg(hier_->hierName()).arg(hier_->hierSize());
  return CQChartsBubbleNodeObj::calcTipId();
}

bool
CQChartsBubbleHierObj::
inside(const Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p, Point(hier_->x(), hier_->y())) < this->radius())
    return true;

  return false;
}

void
CQChartsBubbleHierObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsBubbleHierObj::
draw(PaintDevice *device)
{
  double r = this->radius();

  Point p1(hier_->x() - r, hier_->y() - r);
  Point p2(hier_->x() + r, hier_->y() + r);

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
CQChartsBubbleHierObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = hier_->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  plot_->setPenBrush(penBrush,
    PenData  (plot_->isStroked(), bc, plot_->strokeAlpha(),
              plot_->strokeWidth(), plot_->strokeDash()),
    BrushData(plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsBubbleHierObj::
writeScriptData(ScriptPaintDevice *device) const
{
  CQChartsBubbleNodeObj::writeScriptData(device);
}

//------

CQChartsBubbleNodeObj::
CQChartsBubbleNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                      const BBox &rect, const ColorInd &is) :
 CQChartsPlotObj(const_cast<Plot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), node_(node), hierObj_(hierObj)
{
  setDetailHint(DetailHint::MAJOR);

  for (const auto &ind : node_->inds()) {
    if (ind.isValid())
      addModelInd(ind);
  }
}

QString
CQChartsBubbleNodeObj::
calcId() const
{
  if (node_->isFiller())
    return hierObj_->calcId();

  return QString("%1:%2:%3").arg(typeName()).arg(node_->name()).arg(node_->hierSize());
}

QString
CQChartsBubbleNodeObj::
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

    ModelIndex colorModelInd(plot(), ind1.row(), plot_->colorColumn(), ind1.parent());

    bool ok;

    QString colorStr = plot_->modelString(colorModelInd, ok);

    tableTip.addTableRow("Color", colorStr);
  }

  //---

  const auto &ind = node_->ind();

  plot()->addTipColumns(tableTip, ind);

  //---

  return tableTip.str();
}

bool
CQChartsBubbleNodeObj::
inside(const Point &p) const
{
  if (CQChartsUtil::PointPointDistance(p, Point(node_->x(), node_->y())) < this->radius())
    return true;

  return false;
}

void
CQChartsBubbleNodeObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->valueColumn());
}

void
CQChartsBubbleNodeObj::
draw(PaintDevice *device)
{
  double r = this->radius();

  Point p1(node_->x() - r, node_->y() - r);
  Point p2(node_->x() + r, node_->y() + r);

  BBox bbox(p1, p2);

  //---

  bool isPoint = this->isPoint();

  Point point;

  if (isPoint)
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

  if (isPoint)
    device->drawPoint(point);
  else
    device->drawEllipse(bbox);

  device->resetColorNames();

  //---

  if (isPoint)
    return;

  if (plot_->isTextVisible())
    drawText(device, bbox, penBrush.brush.color());
}

void
CQChartsBubbleNodeObj::
drawText(PaintDevice *device, const BBox &bbox, const QColor &brushColor)
{
  // get labels (name and optional size)
  QStringList strs;

  QString name = (! node_->isFiller() ? node_->name() : node_->parent()->name());

  strs.push_back(name);

  if (plot_->isValueLabel() && ! node_->isFiller()) {
    strs.push_back(QString("%1").arg(node_->size()));
  }

  //---

  // calc text pen
  plot_->charts()->setContrastColor(brushColor);

  auto colorInd = calcColorInd();

  PenBrush tPenBrush;

  auto tc = plot_->interpTextColor(colorInd);

  plot_->setPen(tPenBrush, PenData(true, tc, plot_->textAlpha()));

  plot_->updateObjPenBrushState(this, tPenBrush);

  //---

  device->save();

  //---

  // set font
  auto clipLength = plot_->lengthPixelWidth(plot_->textClipLength());
  auto clipElide  = plot_->textClipElide();

  plot_->view()->setPlotPainterFont(plot_, device, plot_->textFont());

  QStringList strs1;

  if (plot_->isTextScaled()) {
    // calc text size
    QFontMetricsF fm(device->font());

    double tw = 0.0;

    for (int i = 0; i < strs.size(); ++i) {
      auto str1 = CQChartsDrawUtil::clipTextToLength(strs[i], device->font(),
                                                     clipLength, clipElide);

      tw = std::max(tw, fm.width(str1));

      strs1.push_back(str1);
    }

    double th = strs1.size()*fm.height();

    //---

    // calc scale factor
    auto pbbox = plot_->windowToPixel(bbox);

    double sx = (tw > 0 ? pbbox.getWidth ()/tw : 1.0);
    double sy = (th > 0 ? pbbox.getHeight()/th : 1.0);

    double s = std::min(sx, sy);

    //---

    // scale font
    device->setFont(CQChartsUtil::scaleFontSize(device->font(), s));
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
  auto pc = plot_->windowToPixel(Point(node_->x(), node_->y()));

  //---

  // draw label
  double xm = plot_->pixelToWindowWidth (3);
  double ym = plot_->pixelToWindowHeight(3);

  device->setClipRect(bbox.adjusted(xm, ym, -xm, -ym));

  // angle and align not supported (always 0 and centered)
  // text is pre-scaled if needed (formatted and html not supported as changes scale calc)
  CQChartsTextOptions textOptions;

  textOptions.contrast      = plot_->isTextContrast();
  textOptions.contrastAlpha = plot_->textContrastAlpha();
//textOptions.clipLength    = clipLength;
//textOptions.clipElide     = clipElide;

  textOptions = plot_->adjustTextOptions(textOptions);

  device->setPen(tPenBrush.pen);

  auto tp = pc;

  if      (strs1.size() == 1) {
    CQChartsDrawUtil::drawTextAtPoint(device, plot_->pixelToWindow(tp), strs1[0], textOptions);
  }
  else if (strs1.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    auto tp1 = plot_->pixelToWindow(Point(tp.x, tp.y - th/2));
    auto tp2 = plot_->pixelToWindow(Point(tp.x, tp.y + th/2));

    CQChartsDrawUtil::drawTextAtPoint(device, tp1, strs1[0], textOptions);
    CQChartsDrawUtil::drawTextAtPoint(device, tp2, strs1[1], textOptions);
  }
  else {
    assert(false);
  }

  plot_->charts()->resetContrastColor();

  //---

  device->restore();
}

bool
CQChartsBubbleNodeObj::
isPoint() const
{
  // check if small enough to draw as point
  double r = this->radius();

  double pw = plot()->windowToPixelWidth (2*r) - 2;
  double ph = plot()->windowToPixelHeight(2*r) - 2;

  return (pw <= 1.5 || ph <= 1.5);
}

void
CQChartsBubbleNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  // calc stroke and brush
  auto colorInd = calcColorInd();

  auto bc = plot_->interpStrokeColor(colorInd);
  auto fc = node_->interpColor(plot_, plot_->fillColor(), colorInd, plot_->numColorIds());

  bool isPoint = this->isPoint();

  if (isPoint) {
    if      (plot_->isFilled())
      plot_->setPenBrush(penBrush,
        PenData  (true, fc, plot_->fillAlpha()),
        BrushData(true, fc, plot_->fillAlpha(), plot_->fillPattern()));
    else if (plot_->isStroked())
      plot_->setPenBrush(penBrush,
        PenData  (true, bc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash()),
        BrushData(true, bc, plot_->strokeAlpha()));
  }
  else {
    plot_->setPenBrush(penBrush,
      PenData  (plot_->isStroked(), bc, plot_->strokeAlpha(),
                plot_->strokeWidth(), plot_->strokeDash()),
      BrushData(plot_->isFilled(), fc, plot_->fillAlpha(), plot_->fillPattern()));
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsBubbleNodeObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.name = \"" << node_->hierName().toStdString() << "\";\n";
  os << "  this.size = " << node_->hierSize() << ";\n";
}

//------

CQChartsBubbleHierNode::
CQChartsBubbleHierNode(const Plot *plot, HierNode *parent, const QString &name,
                       const QModelIndex &ind) :
 CQChartsBubbleNode(plot, parent, name, 0.0, ind)
{
  if (parent_)
    parent_->children_.push_back(this);
}

CQChartsBubbleHierNode::
~CQChartsBubbleHierNode()
{
  for (auto &child : children_)
    delete child;

  for (auto &node : nodes_)
    delete node;
}

double
CQChartsBubbleHierNode::
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
CQChartsBubbleHierNode::
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
              CQChartsBubbleNodeCmp(plot_->isSortReverse()));

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
CQChartsBubbleHierNode::
addNode(Node *node)
{
  nodes_.push_back(node);
}

void
CQChartsBubbleHierNode::
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

void
CQChartsBubbleHierNode::
setPosition(double x, double y)
{
  double dx = x - this->x();
  double dy = y - this->y();

  CQChartsBubbleNode::setPosition(x, y);

  for (auto &node : nodes_)
    node->setPosition(node->x() + dx, node->y() + dy);

  for (auto &child : children_)
    child->setPosition(child->x() + dx, child->y() + dy);
}

QColor
CQChartsBubbleHierNode::
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

CQChartsBubbleNode::
CQChartsBubbleNode(const Plot *plot, HierNode *parent, const QString &name,
                   double size, const QModelIndex &ind) :
 plot_(plot), parent_(parent), id_(nextId()), name_(name), size_(size)
{
  inds_.push_back(ind);

  r_ = CQChartsUtil::areaToRadius(size_);
}

CQChartsBubbleNode::
~CQChartsBubbleNode()
{
}

void
CQChartsBubbleNode::
initRadius()
{
  r_ = CQChartsUtil::areaToRadius(hierSize());
}

QString
CQChartsBubbleNode::
hierName() const
{
  if (parent() && parent() != plot()->root())
    return parent()->hierName() + "/" + name();
  else
    return name();
}

void
CQChartsBubbleNode::
setPosition(double x, double y)
{
  CQChartsCircleNode::setPosition(x, y);

  placed_ = true;
}

QColor
CQChartsBubbleNode::
interpColor(const Plot *plot, const Color &c, const ColorInd &colorInd, int n) const
{
  if      (color().isValid())
    return plot->interpColor(color(), ColorInd());
  else if (colorId() >= 0 && plot_->isColorById())
    return plot->interpColor(c, ColorInd(colorId(), n));
  else
    return plot->interpColor(c, colorInd);
}
