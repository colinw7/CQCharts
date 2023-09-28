#include <CQChartsForceDirectedPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsAnnotation.h>
#include <CQChartsArrow.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsDisplayRange.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQBusyButton.h>

#include <QCheckBox>
#include <QPushButton>

CQChartsForceDirectedPlotType::
CQChartsForceDirectedPlotType()
{
}

void
CQChartsForceDirectedPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

void
CQChartsForceDirectedPlotType::
addGeneralParameters()
{
  CQChartsConnectionPlotType::addGeneralParameters();

  addColumnParameter("edgeWidth", "Edge Width", "edgeWidthColumn").setBasic().
    setNumericColumn().setPropPath("columns.edgeWidth").setTip("Edge Width");
}

QString
CQChartsForceDirectedPlotType::
description() const
{
  auto B    = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto PARM = [](const QString &str) { return CQChartsHtml::Str::angled(str); };
  auto LI   = [](const QString &str) { return CQChartsHtml::Str(str); };
//auto BR   = []() { return CQChartsHtml::Str(CQChartsHtml::Str::Type::BR); };
  auto IMG  = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Force Directed Plot").
     h3("Summary").
      p("Draws connected data using animated nodes connected by springs.").
     h3("Columns").
      p("Connection information can be supplied using:").
      ul({ LI("A list of connections in the " + B("Connections") + " column with the "
              "associated node numbers in the " + B("Node") + " column."),
           LI("A link using " + B("Link") + " column and a value using the " +
              B("Value") + " column.") }).
      p("The connections column is in the form {{" + PARM("id") + " " + PARM("value") + "} ...}.").
      p("The link column is in the form " + PARM("id1") + "/" + PARM("id2")).
      p("The column id is taken from the " + B("Id") + " column and an optional "
        "name for the id can be supplied in the " + B("Name") + " column.").
      p("The group is specified using the " + B("Group") + " column.").
     h3("Styling").
      p("The styling (fill, stroke) of the node shape and edge shape can be set").
      p("The stlying (font, size, ...) of the node and edge text can be set").
     h3("Limitations").
      p("The plot does not support axes, key or logarithmic scales.").
     h3("Example").
      p(IMG("images/forcedirected.png"));
}

bool
CQChartsForceDirectedPlotType::
isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsForceDirectedPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsForceDirectedPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsForceDirectedPlot(view, model);
}

//---

CQChartsForceDirectedPlot::
CQChartsForceDirectedPlot(View *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("forcedirected"), model),
 CQChartsObjNodeShapeData<CQChartsForceDirectedPlot>(this),
 CQChartsObjNodeTextData <CQChartsForceDirectedPlot>(this),
 CQChartsObjEdgeShapeData<CQChartsForceDirectedPlot>(this),
 CQChartsObjEdgeTextData <CQChartsForceDirectedPlot>(this)
{
}

CQChartsForceDirectedPlot::
~CQChartsForceDirectedPlot()
{
  CQChartsForceDirectedPlot::term();
}

//---

void
CQChartsForceDirectedPlot::
init()
{
  CQChartsConnectionPlot::init();

  setSymmetric(false);

  //---

  CQChartsObjNodeTextData<CQChartsForceDirectedPlot>::setNodeReloadObj(false);
  CQChartsObjEdgeTextData<CQChartsForceDirectedPlot>::setEdgeReloadObj(false);

  //---

  NoUpdate noUpdate(this);

  //---

  forceDirected_ = std::make_unique<CQChartsForceDirected>();

  forceDirected_->setStiffness(stiffness());
  forceDirected_->setRepulsion(repulsion());
  forceDirected_->setDamping(damping());
  forceDirected_->setCenterAttract(centerAttract());

  //---

  auto bg = Color::makePalette();

  setNodeFilled(true);
  setNodeFillColor(bg);

  setNodeStroked(true);
  setNodeStrokeAlpha(Alpha(0.2));

  setNodeTextScaled(true);
  setNodeTextColor(Color::makeContrast());

  //---

  setEdgeFilled(true);
  setEdgeFillColor(bg);
  setEdgeFillAlpha(Alpha(0.25));

  setEdgeStroked(true);
  setEdgeStrokeAlpha(Alpha(0.2));

  //---

  addTitle();

  //---

  addColorMapKey();

  //---

  setAutoFit(true);

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));

  setEqualScale(true);

  setAnimating(true);
}

void
CQChartsForceDirectedPlot::
term()
{
}

//---

void
CQChartsForceDirectedPlot::
setEdgeWidthColumn(const Column &c)
{
  CQChartsUtil::testAndSet(edgeWidthColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsForceDirectedPlot::
initNodeColumns()
{
  CQChartsConnectionPlot::initNodeColumns();

  nodeShapeColumn_  .setModelInd(nodeModel_.modelInd());
  nodeLabelColumn_  .setModelInd(nodeModel_.modelInd());
  nodeValueColumn_  .setModelInd(nodeModel_.modelInd());
  nodeInitPosColumn_.setModelInd(nodeModel_.modelInd());
  nodeColorColumn_  .setModelInd(nodeModel_.modelInd());
}

void
CQChartsForceDirectedPlot::
setNodeShapeColumn(const CQChartsModelColumn &c)
{
  CQChartsUtil::testAndSet(nodeShapeColumn_, c, [&]() {
    nodeShapeColumn_.setCharts(charts());

    updateRangeAndObjs();
  } );
}

void
CQChartsForceDirectedPlot::
setNodeLabelColumn(const CQChartsModelColumn &c)
{
  CQChartsUtil::testAndSet(nodeLabelColumn_, c, [&]() {
    nodeLabelColumn_.setCharts(charts());

    updateRangeAndObjs();
  } );
}

void
CQChartsForceDirectedPlot::
setNodeValueColumn(const CQChartsModelColumn &c)
{
  CQChartsUtil::testAndSet(nodeValueColumn_, c, [&]() {
    nodeValueColumn_.setCharts(charts());

    updateRangeAndObjs();
  } );
}

void
CQChartsForceDirectedPlot::
setNodeInitPosColumn(const CQChartsModelColumn &c)
{
  CQChartsUtil::testAndSet(nodeInitPosColumn_, c, [&]() {
    nodeInitPosColumn_.setCharts(charts());

    updateRangeAndObjs();
  } );
}

void
CQChartsForceDirectedPlot::
setNodeColorColumn(const CQChartsModelColumn &c)
{
  CQChartsUtil::testAndSet(nodeColorColumn_, c, [&]() {
    nodeColorColumn_.setCharts(charts());

    updateRangeAndObjs();
  } );
}

//---

CQChartsColumn
CQChartsForceDirectedPlot::
getNamedColumn(const QString &name) const
{
  if (name == "edgeWidth")
    return this->edgeWidthColumn();

  return CQChartsConnectionPlot::getNamedColumn(name);
}

void
CQChartsForceDirectedPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if (name == "edgeWidth")
    return this->setEdgeWidthColumn(c);

  CQChartsConnectionPlot::setNamedColumn(name, c);
}

//---

void
CQChartsForceDirectedPlot::
setAnimating(bool b)
{
  if (b != isAnimating()) {
    setAnimating1(b);

    addPlotObjects();

    updateBusyButton();

    Q_EMIT animateStateChanged(isAnimating());

    Q_EMIT customDataChanged();
  }
}

void
CQChartsForceDirectedPlot::
setInitSteps(int i)
{
  CQChartsUtil::testAndSet(initSteps_, i, [&]() { } );
}

void
CQChartsForceDirectedPlot::
setAnimateSteps(int i)
{
  CQChartsUtil::testAndSet(animateSteps_, i, [&]() { } );
}

void
CQChartsForceDirectedPlot::
setStepSize(double s)
{
  CQChartsUtil::testAndSet(stepSize_, s, [&]() { } );
}

void
CQChartsForceDirectedPlot::
setShowBusyButton(bool b)
{
  showBusyButton_ = b;

  updateBusyButton();
}

void
CQChartsForceDirectedPlot::
setAutoHideBusyButton(bool b)
{
  autoHideBusyButton_ = b;

  updateBusyButton();
}

void
CQChartsForceDirectedPlot::
updateBusyButton()
{
  if (isShowBusyButton()) {
    if (isAnimating() && ! busyButton_) {
      busyButton_ = new CQBusyButton(view());

      busyButton_->setToolTip("Busy calculating placement : Click to stop");

      connect(busyButton_, SIGNAL(busyStateChanged(bool)), this, SLOT(busyButtonSlot(bool)));
    }

    if (busyButton_) {
      busyButton_->setVisible(isAutoHideBusyButton() ? isAnimating() : true);
      busyButton_->setBusy(isAnimating());
    }

    placeBusyButton();
  }
  else {
    if (busyButton_)
      busyButton_->setVisible(false);
  }
}

void
CQChartsForceDirectedPlot::
placeBusyButton()
{
  if (busyButton_) {
    busyButton_->setVisible(isAutoHideBusyButton() ? isAnimating() : true);

    if (busyButton_->isVisible()) {
      auto bbox = view()->windowToPixel(calcViewBBox());

      busyButton_->move(bbox.getXMin() + 4, bbox.getYMin() + 4);
    }
  }
}

void
CQChartsForceDirectedPlot::
busyButtonSlot(bool b)
{
  setAnimating(b);
}

//---

void
CQChartsForceDirectedPlot::
setNodeShape(const NodeShape &s)
{
  CQChartsUtil::testAndSet(nodeDrawData_.shape, s, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.scaled, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeSize(const Length &s)
{
  CQChartsUtil::testAndSet(nodeDrawData_.size, s, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setMinNodeSize(const Length &s)
{
  CQChartsUtil::testAndSet(nodeDrawData_.minSize, s, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeValueColored(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.valueColored, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeValueLabel(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.valueLabel, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeEdgeValueType(const NodeEdgeType &t)
{
  CQChartsUtil::testAndSet(nodeEdgeValueType_, t, [&]() { updateMaxNodeValue(); drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeUseEdgeValue(bool b)
{
  CQChartsUtil::testAndSet(nodeUseEdgeValue_, b, [&]() { updateMaxNodeValue(); drawObjs(); } );
}

//---

void
CQChartsForceDirectedPlot::
setNodeMouseColoring(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.mouseColoring, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeMouseColorType(const NodeEdgeType &type)
{
  CQChartsUtil::testAndSet(nodeDrawData_.mouseColorType, type, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeMouseValue(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.mouseValue, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeMouseEdgeColor(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.mouseEdgeColor, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsForceDirectedPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeDrawData_.shape, s, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.scaled, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeArrow(const EdgeArrow &arrow)
{
  CQChartsUtil::testAndSet(edgeDrawData_.arrow, arrow, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeDrawData_.width, l, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setArrowWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeDrawData_.arrowWidth, l, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeValueColored(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.valueColored, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeValueLabel(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.valueLabel, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeMouseColoring(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.mouseColoring, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeMouseValue(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.mouseValue, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsForceDirectedPlot::
setRangeSize(double r)
{
  CQChartsUtil::testAndSet(rangeSize_, r, [&]() { updateRange(); } );
}

void
CQChartsForceDirectedPlot::
setMinDelta(double d)
{
  minDelta_ = d;
}

void
CQChartsForceDirectedPlot::
setMaxSteps(int n)
{
  maxSteps_ = n;
}

//---

void
CQChartsForceDirectedPlot::
setStiffness(double r)
{
  // TODO: better range
  stiffness_ = std::min(std::max(r, 0.0), 10000.0);

  if (forceDirected_)
    forceDirected_->setStiffness(stiffness_);
}

void
CQChartsForceDirectedPlot::
setRepulsion(double r)
{
  // TODO: better range
  repulsion_ = std::min(std::max(r, 0.0), 100000.0);

  if (forceDirected_)
    forceDirected_->setRepulsion(repulsion_);
}

void
CQChartsForceDirectedPlot::
setDamping(double r)
{
  // TODO: better range
  damping_ = std::min(std::max(r, 0.1), 0.9);

  if (forceDirected_)
    forceDirected_->setDamping(damping_);
}

void
CQChartsForceDirectedPlot::
setCenterAttract(double r)
{
  // TODO: better range
  centerAttract_ = std::min(std::max(r, 0.0), 1000.0);

  if (forceDirected_)
    forceDirected_->setCenterAttract(centerAttract_);
}

void
CQChartsForceDirectedPlot::
setReset(bool b)
{
  if (forceDirected_ && b) {
    forceDirected_->resetPlacement();

    initNodePos();

    drawObjs();

    updatePlotObjs();
  }
}

void
CQChartsForceDirectedPlot::
initNodePos()
{
  for (const auto &pn : nodes_) {
    const auto &node = pn.second;

    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (snode->initPos()) {
      auto p  = snode->initPos().value();
      auto p1 = plotToForcePoint(p);

      auto fp = forceDirected_->point(node);

      fp->setP(SpringVec(p1.x, p1.y));
    }
  }
}

void
CQChartsForceDirectedPlot::
setMinSpringLength(double r)
{
  if (r != minSpringLength_) {
    minSpringLength_ = r;

    updateRangeAndObjs();
  }
}

void
CQChartsForceDirectedPlot::
setMaxSpringLength(double r)
{
  if (r != maxSpringLength_) {
    maxSpringLength_ = r;

    updateRangeAndObjs();
  }
}

//---

int
CQChartsForceDirectedPlot::
numNodes() const
{
  return (forceDirected_ ? int(forceDirected_->nodes().size()) : 0);
}

int
CQChartsForceDirectedPlot::
numEdges() const
{
  return (forceDirected_ ? int(forceDirected_->edges().size()) : 0);
}

//---

void
CQChartsForceDirectedPlot::
addProperties()
{
  CQChartsConnectionPlot::addProperties();

  //---

  // columns
  addProp("columns", "edgeWidthColumn", "edgeWidth", "Edge width column");

  // node columns
  addPropI("nodeModel", "nodeShapeColumn"  , "shapeColumn"  , "Node shape column");
  addPropI("nodeModel", "nodeLabelColumn"  , "labelColumn"  , "Node label column");
  addPropI("nodeModel", "nodeValueColumn"  , "valueColumn"  , "Node value column");
  addPropI("nodeModel", "nodeInitPosColumn", "initPosColumn", "Node init pos column");
  addPropI("nodeModel", "nodeColorColumn"  , "colorColumn"  , "Node color column");

  // animation data
  addProp("animation", "initSteps"         , "", "Initial steps");
  addProp("animation", "animateSteps"      , "", "Animate steps");
  addProp("animation", "numSteps"          , "", "Number of steps");
  addProp("animation", "stepSize"          , "", "Step size");
  addProp("animation", "rangeSize"         , "", "Range size");
  addProp("animation", "minDelta"          , "", "Min delta change");
  addProp("animation", "maxSteps"          , "", "Max animation steps");
  addProp("animation", "showBusyButton"    , "", "Show busy button");
  addProp("animation", "autoHideBusyButton", "", "Auto hide busy button");

  // node
  addProp("node", "nodeShape"         , "shapeType"     , "Node shape type");
  addProp("node", "nodeScaled"        , "scaled"        , "Node scaled by value");
  addProp("node", "nodeSize"          , "size"          , "Node size (ignore if <= 0)");
  addProp("node", "minNodeSize"       , "minSize"       , "Node min size (ignore if <= 0)");
  addProp("node", "nodeValueColored"  , "valueColored"  , "Node colored by value");
  addProp("node", "nodeValueLabel"    , "valueLabel"    , "Draw node value as label");
  addProp("node", "nodeEdgeValueType" , "edgeValueType" , "Type of edges used for node value");
  addProp("node", "nodeUseEdgeValue"  , "useEdgeValue"  , "Use edge value sum for node value");

  addProp("node", "nodeMouseColoring" , "mouseColoring" , "Color node edges on mouse over");
  addProp("node", "nodeMouseColorType", "mouseColorType", "Node mouse edge color type");
  addProp("node", "nodeMouseValue"    , "mouseValue"    , "Show node value on mouse over");
  addProp("node", "nodeMouseEdgeColor", "mouseEdgeColor", "Show mouse over edges in node color");
  addProp("node", "nodeTipNameLabel"  , "tipNameLabel"  , "Label for node name tip");
  addProp("node", "nodeTipValueLabel" , "tipValueLabel" , "Label for node value tip");

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeShape"        , "shapeType"    , "Edge shape type");
  addProp("edge", "edgeArrow"        , "arrow"        , "Edge arrow");
  addProp("edge", "edgeScaled"       , "scaled"       , "Edge width scaled by value");
  addProp("edge", "edgeWidth"        , "width"        , "Max edge width");
  addProp("edge", "arrowWidth"       , "arrowWidth"   , "Directed edge arrow width factor");
  addProp("edge", "edgeValueColored" , "valueColored" , "Edge colored by value");
  addProp("edge", "edgeValueLabel"   , "valueLabel"   , "Draw edge value as label");
  addProp("edge", "edgeMouseColoring", "mouseColoring", "Color edge nodes on mouse over");
  addProp("edge", "edgeMouseValue"   , "mouseValue"   , "Show edge value on mouse over");
  addProp("edge", "edgeMinWidth"     , "minWidth"     , "Min width for non-line edge");

  // edge style
  addProp("edge/stroke", "edgeStroked", "visible", "Edge stroke visible");

  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edit fill visible");

  addFillProperties("edge/fill", "edgeFill", "Edge");

  //---

  // text
  addProp("node/text", "nodeTextVisible", "visible", "Node text label visible");

  addTextProperties("node/text", "nodeText", "Node Text",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::FORMATTED |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

//addProp("node/text", "nodeTextSingleScale", "singleScale",
//        "Node text single scale (when scaled)");

  //--

  addProp("edge/text", "edgeTextVisible", "visible", "Edge text label visible");

  addTextProperties("edge/text", "edgeText", "Edge Text",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::FORMATTED |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // text
  addProp("text/inside"  , "insideTextVisible"  , "visible", "Inside text label visible");
  addProp("text/selected", "selectedTextVisible", "visible", "Selected text label visible");

  addProp("text/inside"  , "insideTextNoElide"  , "noElide", "Inside text label no clip");
  addProp("text/selected", "selectedTextNoElide", "noElide", "Selected text label no clip");

  addProp("text/inside"  , "insideTextNoScale"  , "noScale", "Inside text label no scale");
  addProp("text/selected", "selectedTextNoScale", "noScale", "Selected text label no scale");

  //---

  addProp("placement", "stiffness"    , "", "Force directed stiffness");
  addProp("placement", "repulsion"    , "", "Force directed repulsion");
  addProp("placement", "damping"      , "", "Force directed damping");
  addProp("placement", "centerAttract", "", "Force directed center attraction");
  addProp("placement", "reset"        , "", "Reset placement");

  addProp("placement", "minSpringLength", "", "Min spring length");
  addProp("placement", "maxSpringLength", "", "Max spring length");

  addProp("placement", "unitRange", "", "Use unit range");

  //---

  // info
  addProp("stats", "numNodes", "", "Number of nodes");
  addProp("stats", "numEdges", "", "Number of edges");
}

//---

void
CQChartsForceDirectedPlot::
checkExtraColumns(bool &columnsValid) const
{
  // edge width optional
  if (checkColumn(edgeWidthColumn(), "Edge Width"))
    modelColumns_.push_back(edgeWidthColumn());
  else
    columnsValid = false;
}

//---

CQChartsGeom::Range
CQChartsForceDirectedPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  if (! checkColumns())
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  // TODO: calculate good range size from data or auto scale/fit ?
  Range dataRange;

  double rangeSize = this->rangeSize();

  dataRange.updateRange(-rangeSize, -rangeSize);
  dataRange.updateRange( rangeSize,  rangeSize);

  //---

  return dataRange;
}

//------

void
CQChartsForceDirectedPlot::
clearPlotObjList()
{
  std::unique_lock<std::mutex> lock(createMutex_);

  CQChartsPlot::clearPlotObjList();
}

bool
CQChartsForceDirectedPlot::
createObjs(PlotObjs &) const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::createObjs");

  std::unique_lock<std::mutex> lock(createMutex_);

  NoUpdate noUpdate(this);

  //---

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //th->stopAnimateTimer();

  //---

  if (! isAnimating())
    th->removePlotObjects();

  //---

  th->forceDirected_ = std::make_unique<CQChartsForceDirected>();

  th->forceDirected_->setStiffness(stiffness());
  th->forceDirected_->setRepulsion(repulsion());
  th->forceDirected_->setDamping(damping());
  th->forceDirected_->setCenterAttract(centerAttract());

  //th->forceDirected_->reset();

  //---

  th->idConnections_  .clear();
  th->nameIdMap_      .clear();
  th->idNameMap_      .clear();
  th->nodes_          .clear();
  th->connectionNodes_.clear();

  //---

  // create objects
  auto columnDataType = calcColumnDataType();

  bool rc = false;

  if      (columnDataType == ColumnDataType::HIER)
    rc = initHierObjs();
  else if (columnDataType == ColumnDataType::LINK)
    rc = initLinkObjs();
  else if (columnDataType == ColumnDataType::CONNECTIONS)
    rc = initConnectionObjs();
  else if (columnDataType == ColumnDataType::PATH)
    rc = initPathObjs();
  else if (columnDataType == ColumnDataType::FROM_TO)
    rc = initFromToObjs();
  else if (columnDataType == ColumnDataType::TABLE)
    rc = initTableObjs();

  if (! rc)
    return false;

  //---

  processMetaData();

  //---

  if (nodeModel().isValid() && nodeIdColumn().isValid()) {
    auto *nodeModelData = nodeModel().modelData();

    class NodeVisitor : public CQChartsModelVisitor {
     public:
      NodeVisitor(const CQChartsForceDirectedPlot *plot) :
       plot_(plot) {
        modelInd_ = plot_->nodeModel().modelInd();
        idColumn_ = plot_->nodeIdColumn().column();

        if (plot_->nodeShapeColumn().isValid())
          shapeColumn_ = plot_->nodeShapeColumn().column();

        if (plot_->nodeLabelColumn().isValid())
          labelColumn_ = plot_->nodeLabelColumn().column();

        if (plot_->nodeValueColumn().isValid())
          valueColumn_ = plot_->nodeValueColumn().column();

        if (plot_->nodeInitPosColumn().isValid())
          initPosColumn_ = plot_->nodeInitPosColumn().column();

        if (plot_->nodeColorColumn().isValid())
          colorColumn_ = plot_->nodeColorColumn().column();
      }

      // visit row
      State visit(const QAbstractItemModel *model, const VisitData &data) override {
        bool ok;

        auto var = CQChartsModelUtil::modelValue(
          plot_->charts(), model, data.row, idColumn_, data.parent, ok);

        if (! var.isValid())
          return State::SKIP;

        auto id = var.toString();

        auto *plot = const_cast<CQChartsForceDirectedPlot *>(plot_);

        auto &connectionsData = plot->getConnections(id);

        if (shapeColumn_.isValid()) {
          auto shapeVar = CQChartsModelUtil::modelValue(
            plot_->charts(), model, data.row, shapeColumn_, data.parent, ok);

          if (shapeVar.isValid()) {
            NodeShape shapeType;

            if (! stringToShapeType(shapeVar.toString(), shapeType))
              return State::SKIP;

            connectionsData.shapeType = shapeType;
          }
        }

        if (labelColumn_.isValid()) {
          auto labelVar = CQChartsModelUtil::modelValue(
            plot_->charts(), model, data.row, labelColumn_, data.parent, ok);

          if (labelVar.isValid())
            connectionsData.label = labelVar.toString();
        }

        if (valueColumn_.isValid()) {
          auto valueVar = CQChartsModelUtil::modelValue(
            plot_->charts(), model, data.row, valueColumn_, data.parent, ok);

          if (valueVar.isValid()) {
            auto r = CQChartsVariant::toReal(valueVar, ok);
            if (! ok) return State::SKIP;

            connectionsData.value = OptReal(r);
          }
        }

        if (initPosColumn_.isValid()) {
          auto initPosVar = CQChartsModelUtil::modelValue(
            plot_->charts(), model, data.row, initPosColumn_, data.parent, ok);

          if (initPosVar.isValid()) {
            auto pos = CQChartsVariant::toPoint(initPosVar, ok);
            if (! ok) return State::SKIP;

            connectionsData.pos = OptPoint(pos);
          }
        }

        if (colorColumn_.isValid()) {
          auto colorVar = CQChartsModelUtil::modelValue(
            plot_->charts(), model, data.row, colorColumn_, data.parent, ok);

          if (colorVar.isValid()) {
            connectionsData.fillData.color = CQChartsVariant::toColor(colorVar, ok);
            if (! ok) return State::SKIP;
          }
        }

        return State::OK;
      }

     private:
      const CQChartsForceDirectedPlot *plot_ { nullptr };
      int                              modelInd_ { - 1};
      Column                           idColumn_;
      Column                           shapeColumn_;
      Column                           labelColumn_;
      Column                           valueColumn_;
      Column                           initPosColumn_;
      Column                           colorColumn_;
    };

    NodeVisitor nodeVisitor(this);

    CQChartsModelVisit::exec(charts(), nodeModelData->model().data(), nodeVisitor);
  }

  //---

  th->filterObjs();

  addIdConnections();

  //---

  numSteps_ = 0;

  th->execInitSteps();

  return true;
}

void
CQChartsForceDirectedPlot::
addPlotObjects()
{
  if (isAnimating())
    return;

  auto no = forceDirected_->nodes().size() + forceDirected_->edges().size();

  if (plotObjs_.size() >= no)
    return;

  //---

  removePlotObjects();

  if (! currentModelData())
    return;

  //---

  // add node objects
  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    auto bbox = nodeBBox(node, snode);

    auto *obj = createNodeObj(node, bbox);

    plotObjs_.push_back(obj);
  }

  // add edge objects
  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    auto bbox = edgeBBox(edge, sedge);

    auto *obj = createEdgeObj(edge, bbox);

    plotObjs_.push_back(obj);
  }

  Q_EMIT plotObjsAdded();
}

void
CQChartsForceDirectedPlot::
removePlotObjects()
{
  PlotObjs plotObjs;

  std::swap(plotObjs, plotObjs_);

  for (const auto &plotObj : plotObjs)
    delete plotObj;
}

void
CQChartsForceDirectedPlot::
execInitSteps()
{
  while (numSteps_ < initSteps()) {
    (void) forceDirected_->step(stepSize());

    ++numSteps_;
  }

  doAutoFit();

  updateBusyButton();

  drawObjs();
}

void
CQChartsForceDirectedPlot::
addIdConnections() const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  // calc max group and max edge value for all connections
  th->maxGroup_          = 0;
  th->maxEdgeValue_      = 0.0;
  th->maxEdgeWidthValue_ = 0.0;

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    th->maxGroup_ = std::max(th->maxGroup_, connectionsData.group);

    for (const auto &connection : connectionsData.connections) {
      if (connection.value.isSet())
        th->maxEdgeValue_ = std::max(th->maxEdgeValue_, connection.value.real());

      if (connection.edgeWidth.isSet())
        th->maxEdgeWidthValue_ = std::max(th->maxEdgeWidthValue_, connection.edgeWidth.real());
    }
  }

  if (th->maxGroup_ <= 0)
    th->maxGroup_ = 1;

  th->edgeScale_      = (maxEdgeValue     () > 0.0 ? 1.0/maxEdgeValue     () : 1.0);
  th->edgeWidthScale_ = (maxEdgeWidthValue() > 0.0 ? 1.0/maxEdgeWidthValue() : 1.0);

  //---

  // update nodes for edges
  th->nodes_          .clear();
  th->connectionNodes_.clear();

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    //if (connectionsData.connections.empty()) continue;

    // get source node details
    int         id    = idConnections.first;
    const auto &name  = connectionsData.name;
    int         group = std::max(connectionsData.group, 0);

    // get source node (add if needed)
    auto node = forceDirected_->newNode();

    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (! forceDirected_->getNode(node->id()))
      forceDirected_->addNode(node);

    //auto id = QString("%1:%2").arg(name).arg(group);

    // update node values
    // TODO: check consistent if duplicate ?
    if (name.length())
      snode->setLabel(name.toStdString());

    snode->setMass (nodeMass_);
    snode->setGroup(group);
    snode->setInd  (connectionsData.ind);

    if (connectionsData.value.isSet())
      snode->setNodeValue(connectionsData.value);

    if (connectionsData.shapeType != NodeShape::NONE)
      snode->setShape(static_cast<Node::Shape>(connectionsData.shapeType));

    if (connectionsData.pos)
      snode->setInitPos(connectionsData.pos);

    if (connectionsData.fillData.color.isValid())
      snode->setFillColor(connectionsData.fillData.color);

    th->nodes_          [id         ] = node;
    th->connectionNodes_[snode->id()] = id;
  }

  //---

  // create node edges
  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    if (connectionsData.connections.empty())
      continue;

    // get source node
    int id = idConnections.first;

    auto pn = nodes_.find(id);
    assert(pn != nodes_.end());

    auto srcNode = (*pn).second;
    assert(srcNode);

    for (const auto &connectionData : connectionsData.connections) {
      // get dest node
      auto pn1 = nodes_.find(connectionData.destNode);
      if (pn1 == nodes_.end()) continue;

      auto dstNode = (*pn1).second;
      assert(dstNode);

      //---

      // get edge
      auto edge = forceDirected_->newEdge(srcNode, dstNode);

      auto *sedge = dynamic_cast<Edge *>(edge.get());
      assert(sedge);

      //---

      // set edge value
      OptReal value;

      if (connectionData.value.isSet())
        value = OptReal(connectionData.value.real());

      if (value.isSet()) {
        auto length = CMathUtil::map(value.real(), 0.0, maxEdgeValue(),
                                     maxSpringLength(), minSpringLength());

        sedge->setLength(length);
      }

      if (value.isSet())
        sedge->setValue(value.value());

      //---

      // set edge attributes (label, shape, fill color)
      if (connectionData.label.length())
        sedge->setLabel(connectionData.label.toStdString());

      if (connectionData.shapeType != EdgeShape::NONE)
        sedge->setShape(static_cast<Edge::Shape>(connectionData.shapeType));

      if (connectionData.edgeWidth.isSet())
        sedge->setEdgeWidth(connectionData.edgeWidth);

      if (connectionData.fillData.color.isValid())
        sedge->setFillColor(connectionData.fillData.color);

      //---

      sedge->setInd(connectionData.ind);

      //---

      // update in/out edge count
      auto *srcNode1 = dynamic_cast<Node *>(srcNode.get());
      assert(srcNode1);
      auto *dstNode1 = dynamic_cast<Node *>(dstNode.get());
      assert(dstNode1);

      srcNode1->addOutEdge(sedge);
      dstNode1->addInEdge (sedge);
    }
  }

  //---

  th->updateMaxNodeValue();

  th->initNodePos();
}

void
CQChartsForceDirectedPlot::
updateMaxNodeValue()
{
  maxNodeValue_ = 0.0;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    auto value = calcNodeValue(snode);

    if (value.isSet())
      maxNodeValue_ = std::max(maxNodeValue_, value.real());
  }
}

//------

bool
CQChartsForceDirectedPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::initHierObjs");

  return CQChartsConnectionPlot::initHierObjs();
}

void
CQChartsForceDirectedPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsAddConnection(srcHierData, destHierData);
}

void
CQChartsForceDirectedPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsAddConnection(srcHierData, destHierData);
}

void
CQChartsForceDirectedPlot::
initHierObjsAddConnection(const HierConnectionData &srcHierData,
                          const HierConnectionData &destHierData) const
{
  int depth = srcHierData.linkStrs.size();

  auto        srcStr     = srcHierData.parentStr;
  const auto &srcLinkInd = srcHierData.parentLinkInd;
  double      srcTotal   = srcHierData.total;

  auto        destStr     = destHierData.parentStr;
  const auto &destLinkInd = destHierData.parentLinkInd;
  double      destTotal   = destHierData.total;

  if (! srcHierData.linkStrs.empty())
    srcStr = srcHierData.linkStrs.back();

  if (! destHierData.linkStrs.empty())
    destStr = destHierData.linkStrs.back();

  //---

  assert(destTotal > 0.0);

  auto srcId  = getStringId(srcStr);
  auto destId = getStringId(destStr);

  assert(srcId != destId);

  auto &srcConnectionsData  = const_cast<ConnectionsData &>(getConnections(srcId ));
  auto &destConnectionsData = const_cast<ConnectionsData &>(getConnections(destId));

  //---

  if (isSymmetric() && srcLinkInd.isValid()) {
    auto srcLinkIndex  = modelIndex(srcLinkInd);
    auto srcLinkIndex1 = normalizeIndex(srcLinkIndex);

    //---

    srcConnectionsData.ind   = srcLinkIndex1;
    srcConnectionsData.name  = srcStr;
    srcConnectionsData.group = depth;
    srcConnectionsData.total = OptReal(srcTotal);

    //---

    // dest -> src connections
    if (destLinkInd.isValid()) {
      Connection connection;

      connection.srcNode  = destId;
      connection.destNode = srcId;
      connection.value    = OptReal(destTotal);
      connection.ind      = srcConnectionsData.ind;

      destConnectionsData.connections.push_back(std::move(connection));
    }
  }

  //---

  if (destLinkInd.isValid()) {
    auto destLinkIndex  = modelIndex(destLinkInd);
    auto destLinkIndex1 = normalizeIndex(destLinkIndex);

    //---

    destConnectionsData.ind   = destLinkIndex1;
    destConnectionsData.name  = destStr;
    destConnectionsData.group = depth + 1;
    destConnectionsData.total = OptReal(destTotal);

    //---

    // src -> dest connections
    if (srcLinkInd.isValid()) {
      Connection connection;

      connection.srcNode  = srcId;
      connection.destNode = destId;
      connection.value    = OptReal(destTotal);
      connection.ind      = destConnectionsData.ind;

      srcConnectionsData.connections.push_back(std::move(connection));
    }
  }
}

//---

bool
CQChartsForceDirectedPlot::
initPathObjs() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->maxNodeDepth_ = 0;

  //---

  if (! CQChartsConnectionPlot::initPathObjs())
    return false;

  //---

  if (isPropagate())
    th->propagatePathValues();

  return true;
}

void
CQChartsForceDirectedPlot::
addPathValue(const PathData &pathData) const
{
  int n = pathData.pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  auto separator = calcSeparator();

  auto path1 = pathData.pathStrs[0];

  for (int i = 1; i < n; ++i) {
    auto path2 = path1 + separator + pathData.pathStrs[i];

    auto &srcConnectionsData  = th->getConnections(path1);
    auto &destConnectionsData = th->getConnections(path2);

    srcConnectionsData.name  = path1;
    srcConnectionsData.label = pathData.pathStrs[i - 1];
    srcConnectionsData.depth = i - 1;
    srcConnectionsData.group = srcConnectionsData.depth;

    destConnectionsData.name  = path2;
    destConnectionsData.label = pathData.pathStrs[i];
    destConnectionsData.depth = i;
    destConnectionsData.group = destConnectionsData.depth;

    if (i < n - 1) {
      bool hasEdge = false;

      for (auto &connection : srcConnectionsData.connections) {
        if (connection.destNode == destConnectionsData.node) {
          hasEdge = true;
          break;
        }
      }

      if (! hasEdge) {
        // create edge
        Connection connection;

        connection.srcNode  = srcConnectionsData.node;
        connection.destNode = destConnectionsData.node;
        connection.ind      = destConnectionsData.ind;

        srcConnectionsData.connections.push_back(std::move(connection));

        destConnectionsData.parentId = srcConnectionsData.node;
      }
    }
    else {
      // create edge
      Connection connection;

      connection.srcNode  = srcConnectionsData.node;
      connection.destNode = destConnectionsData.node;
      connection.value    = OptReal(pathData.value);
      connection.ind      = srcConnectionsData.ind;

      srcConnectionsData.connections.push_back(std::move(connection));

      destConnectionsData.parentId = srcConnectionsData.node;
      destConnectionsData.value    = OptReal(pathData.value);
    }

    path1 = path2;
  }
}

void
CQChartsForceDirectedPlot::
propagatePathValues()
{
  // propagate node value up through edges and parent nodes
  for (int depth = maxNodeDepth_; depth >= 0; --depth) {
    for (auto &idConnections : idConnections_) {
      auto &connectionsData = idConnections.second;

      if (connectionsData.depth != depth) continue;

      int id = idConnections.first;

      // set node value from sum of dest values
      if (! connectionsData.value.isSet()) {
        if (! connectionsData.connections.empty()) {
          OptReal sum;

          for (auto &connection : connectionsData.connections) {
            if (connection.value.isSet()) {
              double value = connection.value.real();

              if (sum.isSet())
                sum = OptReal(sum.real() + value);
              else
                sum = OptReal(value);
            }
          }

          if (sum.isSet())
            connectionsData.value = sum;
        }
      }

      // propagate set node value up to source nodes
      if (connectionsData.value.isSet()) {
        if (connectionsData.parentId >= 0) {
          auto pp = idConnections_.find(connectionsData.parentId);
          assert(pp != idConnections_.end());

          auto &parentConnectionsData = (*pp).second;

          for (auto &parentConnection : parentConnectionsData.connections) {
            if (parentConnection.destNode == id)
              parentConnection.value = connectionsData.value;
          }
        }
      }
    }
  }
}

//---

bool
CQChartsForceDirectedPlot::
initFromToObjs() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::initFromToObjs");

  //---

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->initEdgeValueName(valueColumn());

  //---

  CQChartsConnectionPlot::initFromToObjs();

  return true;
}

void
CQChartsForceDirectedPlot::
addFromToValue(const FromToData &fromToData) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  // get src data
  auto &srcConnectionsData = th->getConnections(fromToData.fromStr);

  if (fromToData.depth > 0)
    srcConnectionsData.depth = fromToData.depth;

  //---

  // set group
  if (fromToData.groupData.isValid() && ! fromToData.groupData.isNull())
    srcConnectionsData.group = fromToData.groupData.ig;
  else
    srcConnectionsData.group = -1;

  //---

  // Just node
  if (fromToData.toStr == "") {
    // set node color (if color column specified)
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      srcConnectionsData.fillData.color = c;

    //---

    // set node name values (attribute column)
    processNodeNameValues(srcConnectionsData, fromToData.nameValues);

    //----

    // get graph id ?
  }
  else {
    // No self connect (allow for draw only ?)
    if (fromToData.fromStr == fromToData.toStr)
      return;

    // get dest node
    auto &destConnectionsData = th->getConnections(fromToData.toStr);

    if (fromToData.depth > 0)
      destConnectionsData.depth = fromToData.depth + 1;

    //---

    // create edge
    auto *connection = addEdge(srcConnectionsData, destConnectionsData, fromToData.value);

    //---

    // add model indices ?
    auto fromModelIndex  = modelIndex(fromToData.fromModelInd);
    auto fromModelIndex1 = normalizeIndex(fromModelIndex);

#if 0
    auto addModelInd = [&](const ModelIndex &modelInd) {
      if (modelInd.isValid())
        connection->addModelInd(modelInd);
    };

    connection->setModelInd(fromModelIndex1);

    addModelInd(fromToData.fromModelInd  );
    addModelInd(fromToData.toModelInd    );
    addModelInd(fromToData.valueModelInd );
    addModelInd(fromToData.depthModelInd );

    connection->setNamedColumn("From" , fromToData.fromModelInd  .column());
    connection->setNamedColumn("To"   , fromToData.toModelInd    .column());
    connection->setNamedColumn("Value", fromToData.valueModelInd .column());
    connection->setNamedColumn("Depth", fromToData.depthModelInd .column());
#endif

    connection->ind = fromModelIndex1;

    //---

    // set edge color (if color column specified)
    // Note: from and to are same row so we can use either
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      connection->fillData.color = c;

    //---

    if (edgeWidthColumn().isValid()) {
      ModelIndex edgeWidthModelInd(this, fromToData.fromModelInd.row(),
                                   edgeWidthColumn(), fromToData.fromModelInd.parent());

      bool ok1;
      auto edgeWidth = modelReal(edgeWidthModelInd, ok1);
      if (ok1)
        connection->edgeWidth = OptReal(edgeWidth);
    }

    //---

    // set edge name values (attribute column)
    processEdgeNameValues(connection, fromToData.nameValues);
  }
}

//---

bool
CQChartsForceDirectedPlot::
initLinkObjs() const
{
  return initLinkConnectionObjs();
}

bool
CQChartsForceDirectedPlot::
initConnectionObjs() const
{
  return initLinkConnectionObjs();
}

bool
CQChartsForceDirectedPlot::
initLinkConnectionObjs() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::initLinkConnectionObjs");

  //---

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->initEdgeValueName(valueColumn());

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsForceDirectedPlot *forceDirectedPlot) :
     forceDirectedPlot_(forceDirectedPlot) {
      separator_ = forceDirectedPlot_->calcSeparator();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // Get group value
      long group = -1;

      if (forceDirectedPlot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(forceDirectedPlot_, data.row,
                                 forceDirectedPlot_->groupColumn(), data.parent);

        bool ok1;
        group = forceDirectedPlot_->modelInteger(groupModelInd, ok1);

        if (! ok1)
          return addDataError(groupModelInd, "Non-integer group value");
      }

      int igroup = static_cast<int>(group);

      //---

      // link objs
      if      (forceDirectedPlot_->linkColumn().isValid() &&
               forceDirectedPlot_->valueColumn().isValid()) {
        int    srcId  { -1 };
        int    destId { -1 };
        double value  { 0.0 };

        if (! forceDirectedPlot_->getNameConnections(igroup, data, srcId, destId,
                                                     value, separator_))
          return State::SKIP;

        addConnection(srcId, destId, value);
      }
      // connection objs
      else if (forceDirectedPlot_->connectionsColumn().isValid()) {
        if (! forceDirectedPlot_->getRowConnections(igroup, data))
          return State::SKIP;
      }
      else {
        //assert(false);
        return State::SKIP;
      }

      return State::OK;
    }

    void addConnection(int srcId, int destId, double value) {
      auto &srcConnectionsData =
        const_cast<ConnectionsData &>(forceDirectedPlot_->getConnections(srcId));

      Connection connection;

      connection.srcNode  = srcId;
      connection.destNode = destId;
      connection.value    = OptReal(value);
      connection.ind      = srcConnectionsData.ind;

      srcConnectionsData.connections.push_back(std::move(connection));
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsForceDirectedPlot *>(forceDirectedPlot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsForceDirectedPlot* forceDirectedPlot_ { nullptr };
    QString                          separator_         { "/" };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

bool
CQChartsForceDirectedPlot::
getNameConnections(int group, const ModelVisitor::VisitData &data, int &srcId, int &destId,
                   double &value, const QString &separator) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  // Get link value
  ModelIndex linkModelInd(th, data.row, linkColumn(), data.parent);

  CQChartsNamePair namePair;

  if (linkColumnType() == ColumnType::NAME_PAIR) {
    bool ok;
    auto linkVar = modelValue(linkModelInd, ok);
    if (! ok) return th->addDataError(linkModelInd, "Invalid Link");

    namePair = CQChartsNamePair::fromVariant(linkVar);
  }
  else {
    bool ok;
    auto linkStr = modelString(linkModelInd, ok);
    if (! ok) return th->addDataError(linkModelInd, "Invalid Link");

    namePair = CQChartsNamePair(linkStr, separator);
  }

  if (! namePair.isValid())
    return th->addDataError(linkModelInd, "Invalid Link");

  //---

  // Get value value
  ModelIndex valueModelInd(th, data.row, valueColumn(), data.parent);

  bool ok1;
  value = modelReal(valueModelInd, ok1);
  if (! ok1) return th->addDataError(valueModelInd, "Invalid value");

  //---

  auto srcStr  = namePair.name1();
  auto destStr = namePair.name2();

  srcId  = getStringId(srcStr);
  destId = getStringId(destStr);

  //---

  auto nameInd  = modelIndex(linkModelInd);
  auto nameInd1 = normalizeIndex(nameInd);

  // init src connections data
  auto &srcConnectionsData = const_cast<ConnectionsData &>(getConnections(srcId));

  srcConnectionsData.ind   = nameInd1;
  srcConnectionsData.name  = srcStr;
  srcConnectionsData.group = group;
  srcConnectionsData.total = OptReal(value);

  //---

  // init dest connections data
  auto &destConnectionsData = const_cast<ConnectionsData &>(getConnections(destId));

  if (destConnectionsData.name == "") {
    destConnectionsData.ind   = nameInd1;
    destConnectionsData.name  = destStr;
    destConnectionsData.group = group;
    destConnectionsData.total = OptReal(value);
  }

  return true;
}

bool
CQChartsForceDirectedPlot::
getRowConnections(int group, const ModelVisitor::VisitData &data) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  // get optional node id (default to row)
  ModelIndex nodeModelInd;

  long id = data.row;

  if (nodeColumn().isValid()) {
    nodeModelInd = ModelIndex(th, data.row, nodeColumn(), data.parent);

    bool ok2;
    id = modelInteger(nodeModelInd, ok2);
    if (! ok2) return th->addDataError(nodeModelInd, "Non-integer node value");
  }

  //--

  int iid = static_cast<int>(id);

  // get connections data for id
  auto &connectionsData = const_cast<ConnectionsData &>(getConnections(iid));

  connectionsData.group = group;

  //--

  if (nodeModelInd.isValid()) {
    auto nodeInd  = modelIndex(nodeModelInd);
    auto nodeInd1 = normalizeIndex(nodeInd);

    connectionsData.ind = nodeInd1;
  }

  //---

  // get connections
  CQChartsConnectionList::Connections connections;

  ModelIndex connectionsModelInd(th, data.row, connectionsColumn(), data.parent);

  if (connectionsColumnType() == ColumnType::CONNECTION_LIST) {
    bool ok3;
    auto connectionsVar = modelValue(connectionsModelInd, ok3);

    connections = CQChartsConnectionList::fromVariant(connectionsVar).connections();
  }
  else {
    bool ok3;
    auto connectionsStr = modelString(connectionsModelInd, ok3);
    if (! ok3) return false;

    CQChartsConnectionList::stringToConnections(connectionsStr, connections);
  }

  for (auto &connection : connections) {
    Connection connection1;

    connection1.srcNode  = connectionsData.node;
    connection1.destNode = connection.node;
    connection1.value    = OptReal(connection.value);
    connection1.ind      = connectionsData.ind;

    connectionsData.connections.push_back(std::move(connection1));
  }

  //---

  // get optional name
  auto name = QString::number(id);

  if (nameColumn().isValid()) {
    ModelIndex nameModelInd(th, data.row, nameColumn(), data.parent);

    bool ok4;
    name = modelString(nameModelInd, ok4);
    if (! ok4) return th->addDataError(nameModelInd, "Invalid name string");
  }

  connectionsData.name = name;

  //---

  // set total
  double total = 0.0;

  for (const auto &connection : connectionsData.connections) {
    if (! connection.value.isSet())
      continue;

    total += connection.value.real();
  }

  connectionsData.total = OptReal(total);

  return true;
}

//---

bool
CQChartsForceDirectedPlot::
initTableObjs() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  //---

  auto nv = tableConnectionDatas.size();

  for (size_t row = 0; row < nv; ++row) {
    const auto &tableConnectionData = tableConnectionDatas[row];

    if (tableConnectionData.values().empty())
      continue;

    // create connection data for connection id
    auto &connectionsData =
      const_cast<ConnectionsData &>(getConnections(tableConnectionData.from()));

    connectionsData.ind  = tableConnectionData.nameInd();
    connectionsData.name = tableConnectionData.name();

    if (tableConnectionData.group().isValid() && ! tableConnectionData.group().isNull())
      connectionsData.group = tableConnectionData.group().ig;
    else
      connectionsData.group = -1;

    // add connections
    for (const auto &value : tableConnectionData.values()) {
      Connection connection;

      connection.srcNode  = connectionsData.node;
      connection.destNode = value.to;
      connection.value    = OptReal(value.value);
      connection.ind      = connectionsData.ind;

      connectionsData.connections.push_back(std::move(connection));
    }
  }

  return true;
}

//---

void
CQChartsForceDirectedPlot::
initEdgeValueName(const Column &c)
{
  edgeValueName_ = "";

  if (c.isValid()) {
    bool ok;
    edgeValueName_ = modelHHeaderString(c, ok);
  }
}

//---

bool
CQChartsForceDirectedPlot::
processMetaNodeValue(const QString &name, const QString &key, const QVariant &value)
{
  auto &connectionsData = getConnections(name);

  return processNodeNameVar(connectionsData, key, value);
}

void
CQChartsForceDirectedPlot::
processNodeNameValues(ConnectionsData &connectionsData, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    processNodeNameValue(connectionsData, name, value);
  }
}

void
CQChartsForceDirectedPlot::
processNodeNameValue(ConnectionsData &connectionsData, const QString &name,
                     const QString &valueStr) const
{
  if (! processNodeNameVar(connectionsData, name, valueStr)) {
    auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

    th->addDataError(ModelIndex(), QString("Unhandled name '%1'").arg(name));
  }
}

bool
CQChartsForceDirectedPlot::
processNodeNameVar(ConnectionsData &connectionsData, const QString &name,
                   const QVariant &var) const
{
  // custom shape
  if      (name == "shape") {
    NodeShape shapeType;

    if (! stringToShapeType(var.toString(), shapeType))
      return false;

    connectionsData.shapeType = shapeType;
  }
  // custom label
  else if (name == "label") {
    connectionsData.label = var.toString();
  }
  // custom value
  else if (name == "value") {
    bool ok;
    auto r = CQChartsVariant::toReal(var, ok);
    if (! ok) return false;

    connectionsData.value = OptReal(r);
  }
  // custom init pos
  else if (name == "pos" || name == "init_pos") {
    bool ok;
    auto pos = CQChartsVariant::toPoint(var, ok);
    if (! ok) return false;

    connectionsData.pos = OptPoint(pos);
  }
  // custom fill color
  else if (name == "fill_color" || name == "color") {
    bool ok;
    connectionsData.fillData.color = CQChartsVariant::toColor(var, ok);
    if (! ok) return false;
  }
  // custom fill alpha
  else if (name == "fill_alpha" || name == "alpha") {
    //bool ok;
    //connectionsData.fillAlpha = CQChartsVariant::toReal(var, ok);
    //if (! ok) return false;
  }
  // custom stroke color
  else if (name == "stroke_color") {
    //bool ok;
    //connectionsData.strokeColor = CQChartsVariant::toColor(var, ok);
    //if (! ok) return false;
  }
  // custom stroke alpha
  else if (name == "stroke_alpha") {
    //bool ok;
    //connectionsData.strokeAlpha = CQChartsVariant::toReal(var, ok);
    //if (! ok) return false;
  }
#if 0
  // custom stroke width
  else if (name == "stroke_width" || name == "width") {
    //bool ok;
    node->setStrokeWidth(CQChartsVariant::toLength(var, ok));
    //if (! ok) return false;
  }
  // custom stroke dash
  else if (name == "stroke_dash" || name == "dash") {
    node->setStrokeDash(CQChartsLineDash(var.toString()));
  }
#endif
  else
    return false;

  return true;
}

void
CQChartsForceDirectedPlot::
processEdgeNameValues(Connection *connection, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name     = nv.first;
    auto        valueStr = nv.second.toString();

    // custom shape
    if      (name == "shape") {
      EdgeShape shapeType;

      (void) stringToShapeType(valueStr, shapeType);

      connection->shapeType = shapeType;
    }
    // custom label
    else if (name == "label") {
      connection->label = valueStr;
    }
    // custom fill color
    else if (name == "fill_color" || name == "color") {
      connection->fillData.color = CQChartsColor(valueStr);
    }
    // handle custom value for source node
    else if (name.left(4) == "src_") {
      auto &srcConnections =
        const_cast<ConnectionsData &>(getConnections(connection->srcNode));

      processNodeNameValue(srcConnections, name.mid(4), valueStr);
    }
    // handle custom value for destination node
    else if (name.left(5) == "dest_") {
      auto &destConnections =
        const_cast<ConnectionsData &>(getConnections(connection->destNode));

      processNodeNameValue(destConnections, name.mid(5), valueStr);
    }
  }
}

bool
CQChartsForceDirectedPlot::
stringToShapeType(const QString &str, NodeShape &shapeType)
{
  if      (str == "box"         ) shapeType = NodeShape::BOX;
  else if (str == "circle"      ) shapeType = NodeShape::CIRCLE;
  else if (str == "ellipse"     ) shapeType = NodeShape::CIRCLE;
  else if (str == "doublecircle") shapeType = NodeShape::DOUBLE_CIRCLE;
  else {
    //charts()->errorMsg("Unhandled shape type " + str);
    shapeType = NodeShape::BOX;
    return false;
  }

  return true;
}

bool
CQChartsForceDirectedPlot::
stringToShapeType(const QString &str, EdgeShape &shapeType)
{
  if      (str == "arc" )        shapeType = EdgeShape::ARC;
  else if (str == "line")        shapeType = EdgeShape::LINE;
  else if (str == "rectilinear") shapeType = EdgeShape::RECTILINEAR;
  else {
    //charts()->errorMsg("Unhandled shape type " + str);
    shapeType = EdgeShape::NONE;
    return false;
  }

  return true;
}

//---

void
CQChartsForceDirectedPlot::
filterObjs()
{
  // hide nodes below depth
  if (maxDepth() > 0) {
    for (auto &idConnections : idConnections_) {
      auto &connectionsData = idConnections.second;

      if (connectionsData.depth > maxDepth())
        connectionsData.visible = false;
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (auto &idConnections : idConnections_) {
      auto &connectionsData = idConnections.second;

      if (! connectionsData.value.isSet() || connectionsData.value.real() < minValue())
        connectionsData.visible = false;
    }
  }
}

//---

bool
CQChartsForceDirectedPlot::
addMenuItems(QMenu *menu, const Point &)
{
  if (canDrawColorMapKey()) {
    menu->addSeparator();

    addColorMapKeyItems(menu);
  }

  return true;
}

//---

CQChartsForceDirectedPlot::ConnectionsData &
CQChartsForceDirectedPlot::
getConnections(const QString &str)
{
  auto id = getStringId(str);

  return getConnections1(id, str);
}

const CQChartsForceDirectedPlot::ConnectionsData &
CQChartsForceDirectedPlot::
getConnections(int id) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  return th->getConnections(id);
}

CQChartsForceDirectedPlot::ConnectionsData &
CQChartsForceDirectedPlot::
getConnections(int id)
{
  auto str = getIdString(id);

  return getConnections1(id, str);
}

CQChartsForceDirectedPlot::ConnectionsData &
CQChartsForceDirectedPlot::
getConnections1(int id, const QString &str)
{
  auto p = idConnections_.find(id);

  if (p != idConnections_.end())
    return (*p).second;

  //---

  ConnectionsData data;

  data.node = id;
  data.name = str;

  p = idConnections_.insert(p, IdConnectionsData::value_type(id, data));

  return (*p).second;
}

//---

CQChartsForceDirectedPlot::Connection *
CQChartsForceDirectedPlot::
addEdge(ConnectionsData &srcConnectionsData,
        ConnectionsData &destConnectionsData, const OptReal &value) const
{
  Connection connection;

  connection.srcNode  = srcConnectionsData.node;
  connection.destNode = destConnectionsData.node;
  connection.value    = value;
  connection.ind      = srcConnectionsData.ind;

  srcConnectionsData.connections.push_back(std::move(connection));

  return &srcConnectionsData.connections[srcConnectionsData.connections.size() - 1];
}

//---

int
CQChartsForceDirectedPlot::
getStringId(const QString &str) const
{
  //assert(str.length());

  auto p = nameIdMap_.find(str);

  if (p != nameIdMap_.end())
    return (*p).second;

  //---

  int id = int(nameIdMap_.size());

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->nameIdMap_[str] = id;
  th->idNameMap_[id ] = str;

  return id;
}

QString
CQChartsForceDirectedPlot::
getIdString(int id) const
{
  auto p = idNameMap_.find(id);

  return (p != idNameMap_.end() ? (*p).second : "");
}

//---

void
CQChartsForceDirectedPlot::
postUpdateObjs()
{
  if (isAnimating())
    startAnimateTimer();
}

void
CQChartsForceDirectedPlot::
animateStep()
{
  execInitSteps();

  //---

  if (pressed_ || ! isAnimating())
    return;

  //---

  execAnimateStep();

  //---

  if (! isUpdatesEnabled()) {
    LockMutex lock(this, "setUpdatesEnabled");

    setUpdatesEnabled(true );
    setUpdatesEnabled(false);
  }
}

void
CQChartsForceDirectedPlot::
execAnimateStep()
{
  double delta = 0.0;

  for (int i = 0; i < animateSteps(); ++i) {
    delta = forceDirected_->step(stepSize());

    ++numSteps_;
  }

  if (minDelta() > 0.0 && delta < minDelta())
    setAnimating(false);

  if (maxSteps() > 0 && numSteps_ > maxSteps())
    setAnimating(false);

  doAutoFit();

  drawObjs();

  updatePlotObjs();
}

void
CQChartsForceDirectedPlot::
doAutoFit()
{
  if (isAutoFit()) {
    // set margin to max of node radius and 3 pixels
    double s = lengthPlotWidth(nodeSize())/2.0;

    auto minSize = pixelToWindowWidth(3.0);

    double xm = std::max(s, minSize);
    double ym = xm;

    //---

    // calc force directed range
    double xmin { -1.0 }, ymin { -1.0 }, xmax { 1.0 }, ymax { 1.0 };
    forceDirected_->calcRange(xmin, ymin, xmax, ymax);

    forceRange_  = Range(xmin, ymin, xmax, ymax);
    forceAspect_ = forceRange_.aspect();

    calcDataRange_ = Range();

    if (isUnitRange()) {
      double w = (forceAspect_ > 1.0 ? 2.0*forceAspect_ : 2.0             );
      double h = (forceAspect_ > 1.0 ? 2.0              : 2.0/forceAspect_);

      calcDataRange_.updateRange(-w/2.0 - xm, -h/2.0 - ym);
      calcDataRange_.updateRange( w/2.0 + xm,  h/2.0 + ym);
    }
    else {
      calcDataRange_.updateRange(xmin - xm, ymin - ym);
      calcDataRange_.updateRange(xmax + xm, ymax + ym);
    }

    unequalDataRange_ = adjustDataRange(calcDataRange_);

    dataRange_ = unequalDataRange_;

    applyEqualScale(dataRange_);

    outerDataRange_ = dataRange_;

    //---

    auto bbox = CQChartsUtil::rangeBBox(dataRange_);

    auto adjustedBBox = adjustDataRangeBBox(bbox);

    setWindowRange(bbox, adjustedBBox);

    //---

    updatePlotObjs();
  }
}

void
CQChartsForceDirectedPlot::
autoFitUpdate()
{
  doAutoFit();
}

//---

bool
CQChartsForceDirectedPlot::
handleSelectPress(const Point &p, SelMod selMod)
{
  setCurrentNode(p);

  pressed_ = true;

  selectPoint(p, selMod);

  drawObjs();

  return true;
}

bool
CQChartsForceDirectedPlot::
handleSelectMove(const Point &p, Constraints, bool)
{
  setCurrentNode(p);

  if (! isAnimating())
    updateInside(p);

  return true;
}

bool
CQChartsForceDirectedPlot::
handleSelectRelease(const Point & /*p*/)
{
  resetCurrentNode();

  pressed_ = false;

  drawObjs();

  return true;
}

//---

bool
CQChartsForceDirectedPlot::
handleEditPress(const Point &, const Point &p, bool)
{
  setCurrentNode(p);

  pressed_ = true;

  drawObjs();

  return true;
}

bool
CQChartsForceDirectedPlot::
handleEditMove(const Point &, const Point &p, bool)
{
  if (pressed_) {
    if (forceDirected_->currentPoint()) {
      auto p1 = plotToForcePoint(p);

      forceDirected_->currentPoint()->setP(SpringVec(p1.x, p1.y));
    }

    drawObjs();

    updatePlotObjs();

    return true;
  }
  else {
    setCurrentNode(p);

    //---

    if (! isAnimating())
      updateInside(p);
  }

  return true;
}

bool
CQChartsForceDirectedPlot::
handleEditRelease(const Point &, const Point &p)
{
  if (forceDirected_->currentPoint()) {
    auto p1 = plotToForcePoint(p);

    forceDirected_->currentPoint()->setP(SpringVec(p1.x, p1.y));
  }

  resetCurrentNode();

  pressed_ = false;

  drawObjs();

  updatePlotObjs();

  return true;
}

//---

void
CQChartsForceDirectedPlot::
updatePlotObjs()
{
  for (auto *plotObj : plotObjs_) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    auto *edgeObj = dynamic_cast<EdgeObj *>(plotObj);

    if      (nodeObj) {
      auto *snode = dynamic_cast<Node *>(nodeObj->node().get());
      assert(snode);

      if (isUnitRange()) {
        auto bbox = nodeBBox(nodeObj->node(), snode);

        nodeObj->setRect(bbox);
      }
      else
        nodeObj->setRect(snode->bbox());
    }
    else if (edgeObj) {
      auto *sedge = dynamic_cast<Edge *>(edgeObj->edge().get());
      assert(sedge);

      if (isUnitRange()) {
        auto bbox = edgeBBox(edgeObj->edge(), sedge);

        edgeObj->setRect(bbox);
      }
    }
  }
}

//---

bool
CQChartsForceDirectedPlot::
pointSelect(const Point &p, SelMod selMod)
{
  setCurrentNode(p);

  selectPoint(p, selMod);

  drawObjs();

  return true;
}

bool
CQChartsForceDirectedPlot::
rectSelect(const BBox &r, SelMod selMod)
{
  auto p = r.getCenter();

  setCurrentNode(p);

  selectRect(r, selMod, view()->isSelectInside());

  drawObjs();

  return true;
}

//---

void
CQChartsForceDirectedPlot::
setCurrentNode(const Point &p)
{
  auto p1 = plotToForcePoint(p);

  auto nodePoint = forceDirected_->nearest(SpringVec(p1.x, p1.y));

  forceDirected_->setCurrentNode (nodePoint.first );
  forceDirected_->setCurrentPoint(nodePoint.second);
}

void
CQChartsForceDirectedPlot::
resetCurrentNode()
{
  forceDirected_->setCurrentNode (nullptr);
  forceDirected_->setCurrentPoint(nullptr);
}

bool
CQChartsForceDirectedPlot::
selectRect(const BBox &r, SelMod selMod, bool inside)
{
  return selectGeom(r, r.getCenter(), selMod, inside, true);
}

bool
CQChartsForceDirectedPlot::
selectPoint(const Point &p, SelMod selMod)
{
  return selectGeom(BBox(), p, selMod, false, false);
}

bool
CQChartsForceDirectedPlot::
selectGeom(const BBox &r, const Point &p, SelMod selMod, bool inside, bool isRect)
{
  using AnnotationSet = std::set<Annotation *>;

  // get currently selected nodes and edges
  NodeSet selectedNodes;
  EdgeSet selectedEdges;

  selectedNodesAndEdges(selectedNodes, selectedEdges);

  AnnotationSet selectedAnnotations;

  for (const auto &annotation : annotations())
    if (annotation->isSelected())
      selectedAnnotations.insert(annotation);

  //---

  // get new selection
  NodeSet newSelectedNodes, pressedNodes;
  EdgeSet newSelectedEdges, pressedEdges;

  AnnotationSet newSelectedAnnotations, pressedAnnotations;

  if (selMod == SelMod::ADD || selMod == SelMod::REMOVE) {
    newSelectedNodes = selectedNodes;
    newSelectedEdges = selectedEdges;

    newSelectedAnnotations = selectedAnnotations;
  }

  //---

  auto addSelectedNode = [&](Node *node) {
    if (selMod == SelMod::REMOVE)
      newSelectedNodes.erase(node);
    else
      newSelectedNodes.insert(node);
  };

  auto addSelectedEdge = [&](Edge *edge) {
    if (selMod == SelMod::REMOVE)
      newSelectedEdges.erase(edge);
    else
      newSelectedEdges.insert(edge);
  };

  auto addSelectedAnnotation = [&](Annotation *annotation) {
    if (! annotation->isSelectable()) return;

    if (selMod == SelMod::REMOVE)
      newSelectedAnnotations.erase(annotation);
    else
      newSelectedAnnotations.insert(annotation);
  };

  //---

  if (! isRect) {
    Node *selectedNode = nullptr;
    Edge *selectedEdge = nullptr;

    nearestNodeEdge(p, selectedNode, selectedEdge);

    if      (selectedNode) {
      pressedNodes.insert(selectedNode);

      addSelectedNode(selectedNode);
    }
    else if (selectedEdge) {
      pressedEdges.insert(selectedEdge);

      addSelectedEdge(selectedEdge);
    }
    else {
      for (const auto &annotation : annotations()) {
        if (annotation->contains(p)) {
          pressedAnnotations.insert(annotation);

          addSelectedAnnotation(annotation);
        }
      }
    }
  }
  else {
    NodeSet insideNodes;
    EdgeSet insideEdges;

    insideNodesAndEdges(r, insideNodes, insideEdges, inside);

    pressedNodes = insideNodes;
    pressedEdges = insideEdges;

    for (auto *node : insideNodes)
      addSelectedNode(node);

    for (auto *edge : insideEdges)
      addSelectedEdge(edge);
  }

  //---

  // check if match original selection
  bool match = false;

  if (selectedNodes      .size() == newSelectedNodes      .size() &&
      selectedEdges      .size() == newSelectedEdges      .size() &&
      selectedAnnotations.size() == newSelectedAnnotations.size()) {
    match = true;

    auto pn1 = selectedNodes   .begin();
    auto pn2 = newSelectedNodes.begin();

    while (match && pn1 != selectedNodes.end()) {
      if (*pn1 != *pn2)
        match = false;

      ++pn1;
      ++pn2;
    }

    auto pe1 = selectedEdges   .begin();
    auto pe2 = newSelectedEdges.begin();

    while (match && pe1 != selectedEdges.end()) {
      if (*pe1 != *pe2)
        match = false;

      ++pe1;
      ++pe2;
    }

    auto pa1 = selectedAnnotations   .begin();
    auto pa2 = newSelectedAnnotations.begin();

    while (match && pa1 != selectedAnnotations.end()) {
      if (*pa1 != *pa2)
        match = false;

      ++pa1;
      ++pa2;
    }
  }

  //---

  if (! match) {
    startSelection();

    // deselect old nodes and edges
    for (auto *node : selectedNodes)
      node->setSelected(false);

    for (auto *edge : selectedEdges)
      edge->setSelected(false);

    for (auto *annotation : selectedAnnotations)
      annotation->setSelected(false);

    //---

    // select new nodes and edges
    for (auto *node : newSelectedNodes)
      node->setSelected(true);

    for (auto *edge : newSelectedEdges)
      edge->setSelected(true);

    for (auto *annotation : newSelectedAnnotations)
      annotation->setSelected(true);

    endSelection();
  }

  //---

  updateSelText();

  drawObjs();

  //---

  // send pressed signals
  for (auto *node : pressedNodes)
    Q_EMIT objIdPressed(node->stringId());

  for (auto *edge : pressedEdges)
    Q_EMIT objIdPressed(edge->stringId());

  for (auto *annotation : pressedAnnotations) {
    Q_EMIT annotationPressed  (annotation);
    Q_EMIT annotationIdPressed(annotation->id());
  }

  return true;
}

bool
CQChartsForceDirectedPlot::
updateInside(const Point &p)
{
  NodeSet insideNodes;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (snode->isInside())
      insideNodes.insert(snode);
  }

  EdgeSet insideEdges;

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    if (sedge->isInside())
      insideEdges.insert(sedge);
  }

  //---

  Node *insideNode = nullptr;
  Edge *insideEdge = nullptr;

  nearestNodeEdge(p, insideNode, insideEdge);

  if (insideNode) {
    if (insideNodes.size() == 1 && *insideNodes.begin() == insideNode)
      return false;
  }
  else if (insideEdge) {
    if (insideEdges.size() == 1 && *insideEdges.begin() == insideEdge)
      return false;
  }
  else {
    if (insideNodes.empty() && insideEdges.empty())
      return false;
  }

  for (auto *node : insideNodes)
    node->setInside(false);

  for (auto *edge : insideEdges)
    edge->setInside(false);

  if      (insideNode)
    insideNode->setInside(true);
  else if (insideEdge)
    insideEdge->setInside(true);

  drawObjs();

  return true;
}

void
CQChartsForceDirectedPlot::
selectedNodesAndEdges(NodeSet &selectedNodes, EdgeSet &selectedEdges) const
{
  // get selected nodes and edges
  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (snode->isSelected())
      selectedNodes.insert(snode);
  }

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    if (sedge->isSelected())
      selectedEdges.insert(sedge);
  }
}

void
CQChartsForceDirectedPlot::
updateSelText()
{
  // get selected nodes and edges
  NodeSet selectedNodes;
  EdgeSet selectedEdges;

  selectedNodesAndEdges(selectedNodes, selectedEdges);

  //---

  QString selText;

  auto addText = [&](const QString &text) {
    if (selText != "")
      selText += ", ";

    selText += text;
  };

  for (auto *node : selectedNodes)
    addText(calcNodeLabel(node));

  for (auto *edge : selectedEdges) {
    auto *snode = dynamic_cast<Node *>(edge->source().get());
    auto *tnode = dynamic_cast<Node *>(edge->target().get());

    if (snode && tnode)
      addText(QString("%1 -> %2").arg(calcNodeLabel(snode)).arg(calcNodeLabel(tnode)));
  }

  //---

  view()->setSelText(selText);
}

//---

bool
CQChartsForceDirectedPlot::
keyPress(int key, int modifier)
{
  if (key == Qt::Key_S)
    setAnimating(! isAnimating());
  else {
    return CQChartsPlot::keyPress(key, modifier);
  }

  return true;
}

//---

bool
CQChartsForceDirectedPlot::
plotTipText(const Point &p, QString &tip, bool /*single*/) const
{
  if (! isAnimating()) {
    CQChartsTableTip tableTip;

#if 0
    auto p1 = plotToForcePoint(p);

    auto nodePoint = forceDirected_->nearest(SpringVec(p1.x, p1.y));

    auto *node = dynamic_cast<Node *>(nodePoint.first.get());
    if (! node) return false;

    nodeTipText(node, tableTip);
#else
    Node *insideNode = nullptr;
    Edge *insideEdge = nullptr;

    nearestNodeEdge(p, insideNode, insideEdge);

    if      (insideNode)
      nodeTipText(insideNode, tableTip);
    else if (insideEdge)
      edgeTipText(insideEdge, tableTip);
    else
      return false;
#endif

    tip = tableTip.str();

    return true;
  }

  return false;
}

void
CQChartsForceDirectedPlot::
nodeTipText(Node *node, CQChartsTableTip &tableTip) const
{
  auto pc = connectionNodes_.find(node->id());

  //tableTip.addTableRow("Id", node->id());

  if (pc != connectionNodes_.end()) {
    auto &connectionsData = getConnections((*pc).second);

    auto label = (connectionsData.label.length() ? connectionsData.label : connectionsData.name);

    if (! label.length())
      label = calcNodeLabel(node);

    if (label.length()) {
      auto nameLabel = (nodeTipNameLabel() != "" ? nodeTipNameLabel() : "Label");

      tableTip.addTableRow(nameLabel, label);
    }

    if (groupColumn().isValid() && connectionsData.group >= 0)
      tableTip.addTableRow("Group", connectionsData.group);

    if (node->totalValue().isSet())
      tableTip.addTableRow("Total", node->totalValue().real());

    // connections
    //tableTip.addTableRow("Outputs", connectionsData.connections.size());
    tableTip.addTableRow("Inputs" , node->numInEdges ());
    tableTip.addTableRow("Outputs", node->numOutEdges());
  }
  else {
    tableTip.addTableRow("Label", calcNodeLabel(node));
  }

  auto value = calcNodeValue(node);

  if (value.isSet()) {
    auto valueLabel = (nodeTipValueLabel() != "" ? nodeTipValueLabel() : "Value");

    tableTip.addTableRow(valueLabel, value.real());
  }

  addTipColumns(tableTip, node->ind());
}

void
CQChartsForceDirectedPlot::
edgeTipText(Edge *edge, CQChartsTableTip &tableTip) const
{
  auto edgeStr = QString::fromStdString(edge->label());

  if (edgeStr.length())
    tableTip.addTableRow("Label", edgeStr);

  if (edge->value())
    tableTip.addTableRow(edgeValueName_ != "" ? edgeValueName_ : "Value", edge->value().value());

  auto *snode = dynamic_cast<Node *>(edge->source().get());
  auto *tnode = dynamic_cast<Node *>(edge->target().get());

  if (snode) tableTip.addTableRow("From", calcNodeLabel(snode));
  if (tnode) tableTip.addTableRow("To"  , calcNodeLabel(tnode));

  addTipColumns(tableTip, edge->ind());
}

//---

void
CQChartsForceDirectedPlot::
nearestNodeEdge(const Point &p, Node* &insideNode, Edge* &insideEdge) const
{
  insideNode = nullptr;
  insideEdge = nullptr;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (! insideNode && snode->bbox().inside(p))
      insideNode = snode;
  }

  double minDist = 9999;

  Edge *insideLineEdge  = nullptr;
  Edge *insideSolidEdge = nullptr;

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    if (sedge->getIsLine()) {
      double d;

      if (CQChartsUtil::PointLineDistance(p, sedge->startPoint(), sedge->endPoint(), &d)) {
        if (! insideLineEdge || d < minDist) {
          insideLineEdge = sedge;
          minDist        = d;
        }
      }
    }
    else {
      if (! insideSolidEdge && sedge->selectPath().contains(p.qpoint()))
        insideSolidEdge = sedge;
    }
  }

  if (insideSolidEdge)
    insideEdge = insideSolidEdge;
  else {
    auto pd = windowToPixelWidth(minDist);

    if (pd < 4)
      insideEdge = insideLineEdge;
  }
}

void
CQChartsForceDirectedPlot::
insideNodesAndEdges(const BBox &r, NodeSet &insideNodes, EdgeSet &insideEdges, bool inside) const
{
  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (inside) {
      if (r.inside(snode->bbox()))
        insideNodes.insert(snode);
    }
    else {
      if (r.overlaps(snode->bbox()))
        insideNodes.insert(snode);
    }
  }

  QPainterPath path1;

  path1.addRect(r.qrect());

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    QPainterPath path;

    if (sedge->getIsLine())
      path = sedge->edgePath();
    else
      path = sedge->selectPath();

    if (inside) {
      if (path1.contains(path))
        insideEdges.insert(sedge);
    }
    else {
      if (path.intersects(r.qrect()))
        insideEdges.insert(sedge);
    }
  }
}

//---

QString
CQChartsForceDirectedPlot::
calcNodeLabel(Node *node) const
{
  auto label = QString::fromStdString(node->label());

  if (label == "")
    label = getIdString(node->id());

  return label;
}

//---

void
CQChartsForceDirectedPlot::
drawParts(QPainter *painter) const
{
  if (numSteps_ < initSteps())
    return;

  std::unique_lock<std::mutex> lock(createMutex_);

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  CQChartsBuffer buffer(view());

  auto *painter1 = buffer.beginPaint(painter, calcPlotPixelRect().qrect());

  CQChartsPlotPaintDevice device(th, painter1);

  BackgroundParts bgParts;

  bgParts.rects       = hasBackgroundRects();
  bgParts.annotations = hasGroupedAnnotations(Layer::Type::BG_ANNOTATION);

  drawBackgroundDeviceParts(&device, bgParts);

  drawDeviceParts(&device);

  ForegroundParts fgParts;

  fgParts.annotations = hasGroupedAnnotations(Layer::Type::FG_ANNOTATION);

  drawForegroundDeviceParts(&device, fgParts);

  //---

  buffer.endPaint();

  //---

  if (zoomData_.isFullScreen()) {
    auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

    th->saveOverview(buffer.image());
  }
}

void
CQChartsForceDirectedPlot::
drawDeviceParts(PaintDevice *device) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->addPlotObjects();

  //--

  device->save();

  setClipRect(device);

  //---

  // reset node slots
  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    snode->clearOccupiedSlots();
  }

  drawTextDatas_.clear();

  insideDrawEdges_.clear();
  insideDrawNodes_.clear();

  insideEdgeData_.clear();
  insideNodeData_.clear();

  //---

  // draw edges
  int numEdges = int(forceDirected_->edges().size());
  int edgeNum  = 0;

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    ColorInd colorInd(edgeNum++, numEdges);

    sedge->setColorInd(colorInd);

    drawEdge(device, edge, sedge);
  }

  //--

  // draw nodes
  int numNodes = int(forceDirected_->nodes().size());
  int nodeNum  = 0;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    auto point = forceDirected_->point(node);

    auto colorInd = ColorInd(nodeNum++, numNodes);

    snode->setColorInd(colorInd);

    drawNode(device, node, snode);
  }

  //--

  // draw texts
  for (const auto &textData : drawTextDatas_)
    drawTextData(device, textData, MouseOver::NONE);

  //---

  if (hasTitle())
    drawTitle(device);

  //--

  // draw inside
  for (const auto &pe : insideDrawEdges_)
    drawEdgeInside(device, pe.first, pe.second);

  for (const auto &pn : insideDrawNodes_)
    drawNodeInside(device, pn.first, pn.second);

  //---

  device->restore();
}

void
CQChartsForceDirectedPlot::
drawEdge(PaintDevice *device, const ForceEdgeP &edge, Edge *sedge) const
{
  bool colorInside = (sedge->isInside() && (isEdgeMouseColoring() || isEdgeMouseValue()));

  //---

  auto colorInd = sedge->colorInd();

  bool   isLine = false;
  double lw     = 1.0;

  auto penBrush = calcEdgePenBrush(sedge, colorInd, isLine, lw);

  auto drawType = (isLine ? DrawType::LINE : DrawType::ARC);

  auto insidePenBrush = penBrush;

  view()->updatePenBrushState(colorInd, insidePenBrush, false, true, drawType);
  view()->updatePenBrushState(colorInd, penBrush, sedge->isSelected(), sedge->isInside(), drawType);

  auto lww = pixelToWindowWidth(lw);

  //---

  // get edge type
  auto edgeType      = calcEdgeShape(sedge);
  bool isRectilinear = (edgeType == CQChartsDrawUtil::EdgeType::RECTILINEAR);

  auto *snode = dynamic_cast<Node *>(sedge->source().get());
  auto *tnode = dynamic_cast<Node *>(sedge->target().get());
  assert(snode && tnode);

  auto sshape = calcNodeShape(snode);
  auto tshape = calcNodeShape(tnode);

  bool isCircleS = (sshape == Node::Shape::CIRCLE || sshape == Node::Shape::DOUBLE_CIRCLE);
  bool isCircleT = (tshape == Node::Shape::CIRCLE || tshape == Node::Shape::DOUBLE_CIRCLE);

  //--

  // get connection rect of source and destination object
  auto sbbox = nodeBBox(sedge->source(), snode);
  auto tbbox = nodeBBox(sedge->target(), tnode);

  // get default connection line (no path)
  CQChartsDrawUtil::ConnectPos        ep1, ep2;
  CQChartsDrawUtil::RectConnectData   rectConnectData;
  CQChartsDrawUtil::CircleConnectData circleConnectData;

  auto sc = sbbox.getCenter(); auto sr = sbbox.getWidth()/2.0;
  auto tc = tbbox.getCenter(); auto tr = tbbox.getWidth()/2.0;

  const int NodeSlots = 16; // balance between available and angle separation

  if (isCircleS) {
    circleConnectData.numSlots = NodeSlots;

    for (const auto &ps : snode->occupiedSlots())
      circleConnectData.occupiedSlots.insert(ps.first);

    CQChartsDrawUtil::circleConnectionPoint(sc, sr, tc, tr, ep1, circleConnectData);

    snode->addOccupiedSlot(ep1.slot, sedge);
  }
  else
    CQChartsDrawUtil::rectConnectionPoint(sbbox, tbbox, ep1, rectConnectData);

  if (isCircleT) {
    circleConnectData.numSlots = NodeSlots;

    for (const auto &ps : tnode->occupiedSlots())
      circleConnectData.occupiedSlots.insert(ps.first);

    CQChartsDrawUtil::circleConnectionPoint(tc, tr, ep1.p, 0.0, ep2, circleConnectData);

    //---

    int saveSlot = ep2.slot;

    if (isCircleS) {
      while (true) {
        bool intersect = false;

        QPainterPath edgePath;

        CQChartsDrawUtil::curvePath(edgePath, ep1.p, ep2.p, edgeType, ep1.angle, ep2.angle);

        for (const auto &ps : tnode->occupiedSlots()) {
          for (auto *oedge : ps.second) {
            const auto &opath = edgePaths_[oedge->id()];

            if (edgePath.intersects(opath)) {
              intersect = true;
              break;
            }
          }

          if (intersect)
            break;
        }

        if (! intersect)
          break;

        //std::cerr << "Path intersect\n";

        circleConnectData.occupiedSlots.insert(ep2.slot);

        CQChartsDrawUtil::circleConnectionPoint(tc, tr, ep1.p, 0.0, ep2, circleConnectData);

        if (ep2.slot == -1) {
          //std::cerr << "No non-intersect found\n";
          ep2.slot = saveSlot;
          break;
        }
      }
    }

    //---

    tnode->addOccupiedSlot(ep2.slot, sedge);
  }
  else
    CQChartsDrawUtil::rectConnectionPoint(tbbox, sbbox, ep2, rectConnectData);

  //---

  // calc edge paths
  bool isArrow = (edgeArrow() != EdgeArrow::NONE);

  QPainterPath edgePath, curvePath, selectPath;

  if (! isLine) {
    CQChartsDrawUtil::curvePath(edgePath, ep1.p, ep2.p, edgeType, ep1.angle, ep2.angle);

    if (isArrow) {
      CQChartsArrowData arrowData;

      if (edgeArrow() == EdgeArrow::END) {
        arrowData.setFHeadType(CQChartsArrowData::HeadType::NONE);  // directional
        arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);
      }
      else {
        arrowData.setMidHeadType(CQChartsArrowData::HeadType::ARROW);
      }

      CQChartsArrow::pathAddArrows(device, edgePath, arrowData, lww, arrowWidth(), curvePath);
    }
    else {
      CQChartsDrawUtil::edgePath(curvePath, ep1.p, ep2.p, lww, edgeType, ep1.angle, ep2.angle);
    }

    if (lw < 3) {
      auto lww1 = pixelToWindowWidth(3);

      CQChartsDrawUtil::edgePath(selectPath, ep1.p, ep2.p, lww1, edgeType, ep1.angle, ep2.angle);
    }
    else
      selectPath = curvePath;
  }
  else {
    CQChartsDrawUtil::linePath(edgePath, ep1.p, ep2.p);

    selectPath = edgePath;
  }

  //---

  // set edge draw geometry
  sedge->setIsLine(isLine);

  sedge->setStartPoint(ep1.p);
  sedge->setEndPoint  (ep2.p);

  sedge->setCurvePath (curvePath);
  sedge->setEdgePath  (edgePath);
  sedge->setSelectPath(selectPath);

  //---

  // calc edge draw path
  QPainterPath edgePath1;

  if (sedge->getIsLine())
    edgePath1 = edgePath;
  else {
    if (isRectilinear)
      edgePath1 = curvePath.simplified();
    else
      edgePath1 = curvePath;
  }

  edgePaths_[sedge->id()] = edgePath1;

  //---

  // draw edge path
  if (! colorInside) {
    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawPath(edgePath1);
  }

  //---

  drawEdgeText(device, sedge, colorInd, MouseOver::NONE);

  //---

  if (colorInside)
    insideDrawEdges_[edge] = insidePenBrush;

  insideEdgeData_[edge] = insidePenBrush;
}

bool
CQChartsForceDirectedPlot::
isEdgeLine(Edge *sedge, double &lw, double tol) const
{
  useRawRange_ = true;
  auto pedgeWidth = lengthPlotWidth(this->edgeWidth());
  useRawRange_ = false;

  double edgeWidth = lengthPixelWidth(Length::plot(pedgeWidth));

  lw = edgeWidth;

  if (isEdgeScaled() && hasEdgeWidth(sedge)) {
    auto value = calcNormalizedEdgeWidth(sedge);

    if (value.isSet())
      lw = edgeWidth*value.real();
  }

  if (lw < tol)
    return true;

  return false;
}

CQChartsPenBrush
CQChartsForceDirectedPlot::
calcEdgePenBrush(Edge *sedge, const ColorInd &colorInd, bool &isLine, double &lw) const
{
  // calc pen and brush
  PenBrush penBrush;

  QColor fillColor;
  auto   fillAlpha = edgeFillAlpha();

  if (isEdgeValueColored()) {
    auto value = calcNormalizedEdgeWidth(sedge);

    double rvalue = (value.isSet() ? value.real() : 0.0);

    fillColor = interpColor(edgeFillColor(), ColorInd(rvalue));
  }
  else {
    if (sedge->fillColor().isValid())
      fillColor = interpColor(sedge->fillColor(), colorInd);
    else
      fillColor = interpEdgeFillColor(colorInd);
  }

  auto strokeColor = interpEdgeStrokeColor(colorInd);
  auto strokeAlpha = edgeStrokeAlpha();

  isLine = isEdgeLine(sedge, lw, edgeMinWidth());

  if (sedge->isInside()) {
    fillColor = insideColor(fillColor);

    if (isLine) {
      strokeColor = fillColor;
      strokeAlpha = fillAlpha;
    }
  }

  auto penData   = edgePenData(strokeColor, strokeAlpha);
  auto brushData = edgeBrushData(fillColor, fillAlpha);

  setPenBrush(penBrush, penData, brushData);

  return penBrush;
}

void
CQChartsForceDirectedPlot::
drawEdgeText(PaintDevice *device, Edge *sedge, const ColorInd &colorInd,
             const MouseOver &mouseOver) const
{
  auto edgePath = sedge->edgePath();

  // draw text
  auto edgeStr = QString::fromStdString(sedge->label());

  bool visible = false;

  if (mouseOver == MouseOver::NONE)
    visible = (isEdgeTextVisible() ||
               (sedge->isInside  () && isInsideTextVisible()) ||
               (sedge->isSelected() && isSelectedTextVisible()));
  else
    visible = true;

  if (! visible)
    return;

  //---

  DrawTextData textData;

  //---

  // add edge label text
  if (edgeStr.length())
    textData.strs << edgeStr;

  // add edge value text
  bool valueVisible = false;

  if (sedge->value()) {
    valueVisible = isEdgeValueLabel();

    if (mouseOver == MouseOver::EDGE && ! valueVisible)
      valueVisible = isEdgeMouseValue();
  }

  if (valueVisible)
    textData.strs << QString("%1").arg(*sedge->value());

  //---

  if (! textData.strs.length())
    return;

  // set font
  textData.font = edgeTextFont();

  // set text pen
  auto c = interpEdgeTextColor(colorInd);

  if (mouseOver == MouseOver::NONE)
    setPen(textData.penBrush, PenData(true, c, edgeTextAlpha()));

  // set position
  textData.point = Point(CQChartsDrawUtil::pathMidPoint(edgePath));

  // set text options
  textData.textOptions = edgeTextOptions(device);

  textData.textOptions.angle = Angle();
  textData.textOptions.align = Qt::AlignCenter;

  if ((sedge->isInside  () && isInsideTextNoElide()) ||
      (sedge->isSelected() && isSelectedTextNoElide()))
    textData.textOptions.clipLength = -1;

  if ((sedge->isInside  () && isInsideTextNoScale()) ||
      (sedge->isSelected() && isSelectedTextNoScale()))
    textData.textOptions.scaled = false;

  if (mouseOver != MouseOver::NONE) {
    drawTextData(device, textData, mouseOver);
  }
  else {
    // add data to draw list
    drawTextDatas_.push_back(textData);
  }
}

void
CQChartsForceDirectedPlot::
drawNode(PaintDevice *device, const ForceNodeP &node, Node *snode) const
{
  bool colorInside = (snode->isInside() && (isNodeMouseColoring() || isNodeMouseValue()));

  //---

  auto colorInd = snode->colorInd();

  // calc pen and brush
  PenBrush penBrush;

  calcNodePenBrush(snode, colorInd, penBrush);

  auto insidePenBrush = penBrush;

  view()->updatePenBrushState(colorInd, insidePenBrush, false, true);
  view()->updatePenBrushState(colorInd, penBrush, snode->isSelected(), snode->isInside());

  //---

  // set node shape
  auto shape = calcNodeShape(snode);
  auto ebbox = nodeBBox(node, snode);

  NodeShapeBBox nodeShape;

  nodeShape.shape = shape;
  nodeShape.bbox  = ebbox;

  nodeShapes_[snode->id()] = nodeShape;

  snode->setBBox(ebbox);

  //---

  if (! colorInside) {
    CQChartsDrawUtil::setPenBrush(device, penBrush);

    drawNodeShape(device, snode);
  }

  //---

  // draw text
  auto contrastColor = penBrush.brush.color();

  charts()->setContrastColor(contrastColor);

  drawNodeText(device, snode, colorInd, MouseOver::NONE);

  charts()->resetContrastColor();

  //---

  if (colorInside)
    insideDrawNodes_[node] = insidePenBrush;

  insideNodeData_[node] = insidePenBrush;
}

void
CQChartsForceDirectedPlot::
calcNodePenBrush(Node *snode, const ColorInd &colorInd, PenBrush &penBrush) const
{
  // calc pen and brush
  auto pc = interpNodeStrokeColor(colorInd);

  QColor fc;

  if (isNodeValueColored()) {
    auto value = calcNormalizedNodeValue(snode);

    double rvalue = (value.isSet() ? value.real() : 0.0);

    fc = interpColor(nodeFillColor(), ColorInd(rvalue));
  }
  else
    fc = calcNodeFillColor(snode);

  auto brushData = nodeBrushData(fc);
  auto penData   = nodePenData(pc);

  setPenBrush(penBrush, penData, brushData);
}

void
CQChartsForceDirectedPlot::
drawNodeText(PaintDevice *device, Node *snode, const ColorInd &colorInd,
             const MouseOver &mouseOver) const
{
  bool visible = false;

  if (mouseOver == MouseOver::NONE)
    visible = (isNodeTextVisible() ||
               (snode->isInside  () && isInsideTextVisible()) ||
               (snode->isSelected() && isSelectedTextVisible()));
  else
    visible = true;

  if (! visible)
    return;

  //---

  DrawTextData textData;

  // set text
  textData.strs << calcNodeLabel(snode);

  bool valueVisible = false;

  auto value = calcNodeValue(snode);

  if (value.isSet()) {
    valueVisible = isNodeValueLabel();

    if (mouseOver == MouseOver::NODE && ! valueVisible)
      valueVisible = isNodeMouseValue();
  }

  if (valueVisible)
    textData.strs << QString("%1").arg(value.real());

  //---

  // set font
  textData.font = nodeTextFont();

  // set text pen
  auto c = interpNodeTextColor(colorInd);

  //if (mouseOver == MouseOver::NONE)
    setPen(textData.penBrush, PenData(true, c, nodeTextAlpha()));

  // set shape and bbox
  auto shape = calcNodeShape(snode);

  textData.shape = static_cast<CQChartsForceDirectedPlot::NodeShape>(shape);
  textData.bbox  = snode->bbox();

  // set text options
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  th->setRefLength(OptReal(textData.bbox.getWidth()));

  textData.textOptions = nodeTextOptions(device);

  th->resetRefLength();

  textData.textOptions.angle = Angle();
  textData.textOptions.align = Qt::AlignCenter;

  textData.contrastColor = charts()->contrastColor();

  if ((snode->isInside  () && isInsideTextNoElide()) ||
      (snode->isSelected() && isSelectedTextNoElide()))
    textData.textOptions.clipLength = -1;

  if ((snode->isInside  () && isInsideTextNoScale()) ||
      (snode->isSelected() && isSelectedTextNoScale()))
    textData.textOptions.scaled = false;

  if (mouseOver != MouseOver::NONE) {
    drawTextData(device, textData, mouseOver);
  }
  else {
    // node name and value will be drawn later
    if (snode->isInside() && isNodeMouseValue())
      return;

    // add to draw list
    drawTextDatas_.push_back(textData);
  }
}

void
CQChartsForceDirectedPlot::
drawNodeShape(PaintDevice *device, Node *snode) const
{
  const auto &nodeShape = nodeShapes_[snode->id()];

  if      (nodeShape.shape == Node::Shape::DOUBLE_CIRCLE)
    CQChartsDrawUtil::drawDoubleEllipse(device, nodeShape.bbox);
  else if (nodeShape.shape == Node::Shape::CIRCLE)
    device->drawEllipse(nodeShape.bbox);
  else
    device->drawRect(nodeShape.bbox);
}

void
CQChartsForceDirectedPlot::
drawTextData(PaintDevice *device, const DrawTextData &textData,
             const MouseOver & /*mouseOver*/) const
{
  device->save();

  device->setContrastColor(textData.contrastColor);

  //---

  // set font
  setPainterFont(device, textData.font);

  //---

  // clip text
  auto strs1 = clipTextsToLength(device, textData.strs, textData.bbox,
                                 textData.textOptions.clipLength, textData.textOptions.clipElide,
                                 textData.textOptions.scaled);

  //---

  //if (mouseOver == MouseOver::NONE)
    device->setPen(textData.penBrush.pen);

  auto textOptions = textData.textOptions;

  textOptions.scaled = false;

  if      (textData.shape == NodeShape::NONE)
    CQChartsDrawUtil::drawTextsAtPoint(device, textData.point, strs1, textOptions);
  else if (textData.shape == NodeShape::CIRCLE)
    CQChartsDrawUtil::drawTextsInCircle(device, textData.bbox, strs1, textOptions);
  else
    CQChartsDrawUtil::drawTextsInBox(device, textData.bbox, strs1, textOptions);

  //---

  device->resetContrastColor();

  device->restore();
}

//---

void
CQChartsForceDirectedPlot::
drawNodeInside(PaintDevice *device, const ForceNodeP &node, const InsideDrawData &drawData) const
{
  if (! forceDirected_)
    return;

  CQChartsDrawUtil::setPenBrush(device, drawData.penBrush);

  auto *snode = dynamic_cast<Node *>(node.get());
  assert(snode);

  // draw node shape
  drawNodeShape(device, snode);

  //---

  // draw connected edges
#if 0
  auto edges = forceDirected_->getEdges(node);

  for (const auto &edge : edges) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    device->drawPath(edgePaths_[sedge->id()]);
  }
#endif

  device->save();

  //---

  // adjust pen for edge highlight
  if (isNodeMouseEdgeColor()) {
    auto pen   = device->pen();
    auto brush = device->brush();

    pen.setColor(brush.color());
    pen.setWidth(4);

    device->setPen(pen);
  }

  //---

  auto getEdgeInsidePenBrush = [&](CQChartsForceDirectedEdge *sedge) {
    for (const auto &pe : insideEdgeData_) {
      if (pe.first.get() == sedge)
        return pe.second.penBrush;
    }
    assert(false);
    return drawData.penBrush;
  };

  if (nodeMouseColorType() == NodeEdgeType::SRC ||
      nodeMouseColorType() == NodeEdgeType::SRC_DEST) {
    for (auto *sedge : snode->inEdges()) {
      if (! isNodeMouseEdgeColor())
        CQChartsDrawUtil::setPenBrush(device, getEdgeInsidePenBrush(sedge));

      device->drawPath(edgePaths_[sedge->id()]);
    }
  }

  if (nodeMouseColorType() == NodeEdgeType::DEST ||
      nodeMouseColorType() == NodeEdgeType::SRC_DEST) {
    for (auto *sedge : snode->outEdges()) {
      if (! isNodeMouseEdgeColor())
        CQChartsDrawUtil::setPenBrush(device, getEdgeInsidePenBrush(sedge));

      device->drawPath(edgePaths_[sedge->id()]);
    }
  }

  device->restore();

  //---

  // draw node text
  CQChartsDrawUtil::setPenBrush(device, drawData.penBrush);

  drawNodeText(device, snode, snode->colorInd(), MouseOver::NODE);
}

void
CQChartsForceDirectedPlot::
drawEdgeInside(PaintDevice *device, const ForceEdgeP &edge, const InsideDrawData &drawData) const
{
  if (! forceDirected_)
    return;

  CQChartsDrawUtil::setPenBrush(device, drawData.penBrush);

  auto *sedge = dynamic_cast<Edge *>(edge.get());
  assert(sedge);

  // draw edge path
  device->drawPath(edgePaths_[sedge->id()]);

  //---

  // draw connected nodes if edge coloring
  if (isEdgeMouseColoring()) {
    auto getNodeInsidePenBrush = [&](CQChartsForceDirectedNode *snode) {
      for (const auto &pn : insideNodeData_) {
        if (pn.first.get() == snode)
          return pn.second.penBrush;
      }
      assert(false);
      return drawData.penBrush;
    };

    auto *snode1 = dynamic_cast<Node *>(edge->source().get());
    auto *snode2 = dynamic_cast<Node *>(edge->target().get());
    assert(snode1 && snode2);

    // draw node shapes
    PenBrush insidePenBrush1 = getNodeInsidePenBrush(snode1);
    PenBrush insidePenBrush2 = getNodeInsidePenBrush(snode2);

    CQChartsDrawUtil::setPenBrush(device, insidePenBrush1);

    drawNodeShape(device, snode1);

    CQChartsDrawUtil::setPenBrush(device, insidePenBrush2);

    drawNodeShape(device, snode2);

    //---

    // draw node texts
    CQChartsDrawUtil::setPenBrush(device, insidePenBrush1);

    drawNodeText(device, snode1, snode1->colorInd(), MouseOver::EDGE);

    CQChartsDrawUtil::setPenBrush(device, insidePenBrush2);

    drawNodeText(device, snode2, snode2->colorInd(), MouseOver::EDGE);
  }

  // draw edge value if mouse value and not already displayed
  if (isEdgeMouseValue()) {
    CQChartsDrawUtil::setPenBrush(device, drawData.penBrush);

    drawEdgeText(device, sedge, sedge->colorInd(), MouseOver::EDGE);
  }
}

//---

void
CQChartsForceDirectedPlot::
postResize()
{
  CQChartsPlot::postResize();

  setNeedsAutoFit(true);

  drawObjs();

  placeBusyButton();
}

bool
CQChartsForceDirectedPlot::
postResizeUpdateRange()
{
  return false;
}

void
CQChartsForceDirectedPlot::
visibleChanged(bool)
{
  placeBusyButton();
}

//---

CQChartsForceDirectedPlot::Node::Shape
CQChartsForceDirectedPlot::
calcNodeShape(Node *snode) const
{
  auto shape = snode->shape();

  return (shape != Node::Shape::NONE ? shape : static_cast<Node::Shape>(nodeShape()));
}

CQChartsForceDirectedPlot::Edge::Shape
CQChartsForceDirectedPlot::
calcEdgeShape(Edge *sedge) const
{
  auto shape = sedge->shape();

  return (shape != Edge::Shape::NONE ? shape : static_cast<Edge::Shape>(edgeShape()));
}

CQChartsGeom::BBox
CQChartsForceDirectedPlot::
nodeBBox(const ForceNodeP &node, Node *snode) const
{
  OptReal optValue;

  if (isNodeScaled())
    optValue = calcScaledNodeValue(snode);

  double s;

  if (optValue.isSet())
    s = optValue.real();
  else
    s = lengthPlotWidth(nodeSize());

  auto minSize = pixelToWindowWidth(3.0);

  auto xmn = std::max(s, minSize);
  auto ymn = xmn;

  auto point = forceDirected_->point(node);

  const auto &p1 = point->p();

  auto p2 = forcePointToPlot(Point(p1.x(), p1.y()));

  auto bbox = BBox(p2.x - xmn/2.0, p2.y - ymn/2.0, p2.x + xmn/2.0, p2.y + ymn/2.0);

  return bbox;
}

CQChartsGeom::BBox
CQChartsForceDirectedPlot::
edgeBBox(const ForceEdgeP &, Edge *sedge) const
{
  auto *snode = dynamic_cast<Node *>(sedge->source().get());
  auto *tnode = dynamic_cast<Node *>(sedge->target().get());
  assert(snode && tnode);

  auto sbbox = nodeBBox(sedge->source(), snode);
  auto tbbox = nodeBBox(sedge->target(), tnode);

  return sbbox + tbbox;
}

CQChartsForceDirectedPlot::OptReal
CQChartsForceDirectedPlot::
calcNodeValue(Node *node) const
{
  auto totalValueType = nodeEdgeValueType();

  if (node->totalValueType() != static_cast<CQChartsForceDirectedEdgeValueType>(totalValueType)) {
    node->setTotalValueType(static_cast<CQChartsForceDirectedEdgeValueType>(totalValueType));

    if (totalValueType != NodeEdgeType::NONE) {
      double value = 0.0;

      if (totalValueType == NodeEdgeType::SRC || totalValueType == NodeEdgeType::SRC_DEST) {
        for (auto *sedge : node->inEdges())
          if (sedge->value())
            value += sedge->value().value();
      }

      if (totalValueType == NodeEdgeType::DEST || totalValueType == NodeEdgeType::SRC_DEST) {
        for (auto *sedge : node->outEdges())
          if (sedge->value())
            value += sedge->value().value();
      }

      node->setTotalValue(OptReal(value));
    }
    else
      node->setTotalValue(OptReal());
  }

  if (isNodeUseEdgeValue())
    return node->totalValue();

  if      (node->nodeValue().isSet())
    return node->nodeValue();
  else if (node->value())
    return OptReal(node->value().value());
  else
    return OptReal();
}

CQChartsForceDirectedPlot::OptReal
CQChartsForceDirectedPlot::
calcNormalizedNodeValue(Node *node) const
{
  auto value = calcNodeValue(node);
  if (! value.isSet()) return value;

  auto r    = value.real();
  auto rmax = maxNodeValue();

  return OptReal(r > 0.0 && rmax > 0.0 ? r/rmax : 0.0);
}

CQChartsForceDirectedPlot::OptReal
CQChartsForceDirectedPlot::
calcScaledNodeValue(Node *node) const
{
  auto value = calcNormalizedNodeValue(node);
  if (! value.isSet()) return value;

  auto r = value.real();

  double s1 = lengthPlotWidth(minNodeSize());
  double s2 = lengthPlotWidth(nodeSize());

  return OptReal((s2 - s1)*r + s1);
}

QColor
CQChartsForceDirectedPlot::
calcNodeFillColor(Node *node) const
{
  ColorInd colorInd;

  if      (colorType() == ColorType::GROUP)
    colorInd = ColorInd(node->group(), maxGroup_ + 1);
  else if (colorType() == ColorType::INDEX)
    colorInd = ColorInd(node->id(), int(nodes_.size()));
  else {
    auto value = calcNormalizedNodeValue(node);

    if (value.isSet())
      colorInd = ColorInd(value.real());
  }

  //---

  if (colorType() == ColorType::AUTO) {
    if (node->fillColor().isValid())
      return interpColor(node->fillColor(), colorInd);

    if (node->ind().isValid() && colorColumn().isValid()) {
      Color color;

      if (colorColumnColor(node->ind().row(), node->ind().parent(), color))
        return interpColor(color, colorInd);
    }

    if (nodeFillColor().isValid())
      return interpColor(nodeFillColor(), colorInd);
  }

  //---

  QColor fc;

  if      (colorType() == ColorType::GROUP)
    fc = interpPaletteColor(colorInd);
  else if (colorType() == ColorType::INDEX)
    fc = interpPaletteColor(colorInd);
  else
    fc = interpPaletteColor(colorInd, /*scale*/false);

  return fc;
}

//---

CQChartsGeom::Point
CQChartsForceDirectedPlot::
forcePointToPlot(const Point &p) const
{
  if (! isUnitRange())
    return p;

  auto x = p.x;
  auto y = p.y;

  if (forceRange_.xsize() > 0.0)
    x = 2.0*(x - forceRange_.xmid())/forceRange_.xsize();

  if (forceRange_.ysize() > 0.0)
    y = 2.0*(y - forceRange_.ymid())/forceRange_.ysize();

  return Point(x, y);
}

CQChartsGeom::Point
CQChartsForceDirectedPlot::
plotToForcePoint(const Point &p) const
{
  if (! isUnitRange() || ! forceRange_.isSet())
    return p;

  auto x = p.x;
  auto y = p.y;

  if (forceRange_.xsize() > 0.0)
    x = x*forceRange_.xsize()/2.0 + forceRange_.xmid();

  if (forceRange_.ysize() > 0.0)
    y = y*forceRange_.ysize()/2.0 + forceRange_.ymid();

  return Point(x, y);
}

//---

bool
CQChartsForceDirectedPlot::
hasEdgeWidth(Edge *edge) const
{
  if (edge->value())
    return true;

  return false;
}

CQChartsForceDirectedPlot::OptReal
CQChartsForceDirectedPlot::
calcNormalizedEdgeWidth(Edge *edge) const
{
  if      (edge->edgeWidth().isSet())
    return OptReal(edgeWidthScale_*edge->edgeWidth().real());
  else if (edge->value())
    return OptReal(edgeScale_*edge->value().value());
  else
    return OptReal();
}

//---

CQChartsForceDirectedNodeObj *
CQChartsForceDirectedPlot::
createNodeObj(ForceNodeP node, const BBox &bbox) const
{
  return new NodeObj(this, node, bbox);
}

CQChartsForceDirectedEdgeObj *
CQChartsForceDirectedPlot::
createEdgeObj(ForceEdgeP edge, const BBox &bbox) const
{
  return new EdgeObj(this, edge, bbox);
}

//---

CQChartsPlotCustomControls *
CQChartsForceDirectedPlot::
createCustomControls()
{
  auto *controls = new CQChartsForceDirectedPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsForceDirectedNodeObj::
CQChartsForceDirectedNodeObj(const ForceDirectedPlot *forceDirectedPlot,
                             ForceNodeP node, const BBox &bbox) :
 CQChartsPlotObj(const_cast<ForceDirectedPlot *>(forceDirectedPlot), bbox,
                 ColorInd(), ColorInd(), ColorInd()),
 forceDirectedPlot_(forceDirectedPlot), node_(node)
{
  setDetailHint(DetailHint::MAJOR);

  auto *snode = dynamic_cast<Node *>(node_.get());

  if (snode) {
#if 0
    if (snode->ind().isValid())
      addModelInd(snode->ind());
    else {
#endif
      for (auto *sedge : snode->inEdges()) {
        if (sedge->ind().isValid())
          addModelInd(sedge->ind());
      }

      for (auto *sedge : snode->outEdges()) {
        if (sedge->ind().isValid())
          addModelInd(sedge->ind());
      }
#if 0
    }
#endif
  }
}

QString
CQChartsForceDirectedNodeObj::
label() const
{
  auto *snode = dynamic_cast<Node *>(node_.get());

  return (snode ? forceDirectedPlot_->calcNodeLabel(snode) : "");
}

QString
CQChartsForceDirectedNodeObj::
calcId() const
{
  auto *snode = dynamic_cast<Node *>(node_.get());

  return (snode ? snode->stringId() : "null_node");
}

QString
CQChartsForceDirectedNodeObj::
calcTipId() const
{
  return calcId();
}

bool
CQChartsForceDirectedNodeObj::
isSelected() const
{
  auto *snode = dynamic_cast<Node *>(node_.get());

  return (snode && snode->isSelected());
}

void
CQChartsForceDirectedNodeObj::
setSelected(bool b)
{
  CQChartsPlotObj::setSelected(b);

  auto *snode = dynamic_cast<Node *>(node_.get());

  if (snode)
    snode->setSelected(b);
}

void
CQChartsForceDirectedNodeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : forceDirectedPlot_->modelColumns())
    addColumnSelectIndex(inds, c);
}

CQChartsObj::ObjShapeType
CQChartsForceDirectedNodeObj::
objShapeType() const
{
  auto *snode = dynamic_cast<Node *>(node_.get());

  auto sshape = forceDirectedPlot_->calcNodeShape(snode);

  if (sshape == Node::Shape::CIRCLE || sshape == Node::Shape::DOUBLE_CIRCLE)
    return ObjShapeType::CIRCLE;

  return ObjShapeType::RECT;
}

CQChartsGeom::BBox
CQChartsForceDirectedNodeObj::
intersectRect() const
{
  auto *snode = dynamic_cast<Node *>(node_.get());

  return forceDirectedPlot_->nodeBBox(node_, snode);
}

//------

CQChartsForceDirectedEdgeObj::
CQChartsForceDirectedEdgeObj(const ForceDirectedPlot *forceDirectedPlot,
                             ForceEdgeP edge, const BBox &bbox) :
 CQChartsPlotObj(const_cast<ForceDirectedPlot *>(forceDirectedPlot), bbox,
                 ColorInd(), ColorInd(), ColorInd()),
 forceDirectedPlot_(forceDirectedPlot), edge_(edge)
{
  setDetailHint(DetailHint::MAJOR);

  auto *sedge = dynamic_cast<Edge *>(edge_.get());

  if (sedge && sedge->ind().isValid())
    addModelInd(sedge->ind());
}

QString
CQChartsForceDirectedEdgeObj::
label() const
{
  auto *sedge = dynamic_cast<Edge *>(edge_.get());

  return (sedge ? QString::fromStdString(sedge->label()) : "no_edge");
}

QString
CQChartsForceDirectedEdgeObj::
calcId() const
{
  auto *sedge = dynamic_cast<Edge *>(edge_.get());

  return (sedge ? sedge->stringId() : "no_edge");
}

QString
CQChartsForceDirectedEdgeObj::
calcTipId() const
{
  return calcId();
}

bool
CQChartsForceDirectedEdgeObj::
isSelected() const
{
  auto *sedge = dynamic_cast<Edge *>(edge_.get());

  return (sedge ? sedge->isSelected() : false);
}

void
CQChartsForceDirectedEdgeObj::
setSelected(bool b)
{
  CQChartsPlotObj::setSelected(b);

  auto *sedge = dynamic_cast<Edge *>(edge_.get());

  if (sedge)
    sedge->setSelected(b);
}

void
CQChartsForceDirectedEdgeObj::
getObjSelectIndices(Indices &inds) const
{
  for (const auto &c : forceDirectedPlot_->modelColumns())
    addColumnSelectIndex(inds, c);
}

//------

CQChartsForceDirectedPlotCustomControls::
CQChartsForceDirectedPlotCustomControls(CQCharts *charts) :
 CQChartsConnectionPlotCustomControls(charts, "forcedirected")
{
}

void
CQChartsForceDirectedPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsForceDirectedPlotCustomControls::
addWidgets()
{
  addConnectionColumnWidgets();

  addOptionsWidgets();

  addRunWidgets();
}

void
CQChartsForceDirectedPlotCustomControls::
addExtraColumnNames(QStringList &names)
{
  names << "edgeWidth";
}

void
CQChartsForceDirectedPlotCustomControls::
addExtraShowColumns(QStringList &names)
{
  names << "edgeWidth";
}

void
CQChartsForceDirectedPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  runningCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Running", "runningCheck");

  addFrameColWidget(optionsFrame_, runningCheck_);
}

void
CQChartsForceDirectedPlotCustomControls::
addRunWidgets()
{
  auto *buttonFrame  = CQUtil::makeWidget<QFrame>("buttonFrame");
  auto *buttonLayout = CQUtil::makeLayout<QHBoxLayout>(buttonFrame, 2, 2);

  layout_->addWidget(buttonFrame);

  auto *stepButton = CQUtil::makeLabelWidget<QPushButton>("Step", "step");

  connect(stepButton, SIGNAL(clicked()), this, SLOT(stepSlot()));

  buttonLayout->addWidget(stepButton);
  buttonLayout->addStretch(1);
}

void
CQChartsForceDirectedPlotCustomControls::
connectSlots(bool b)
{
  CQUtil::optConnectDisconnect(b,
    runningCheck_, SIGNAL(stateChanged(int)), this, SLOT(runningSlot(int)));

  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsForceDirectedPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  forceDirectedPlot_ = dynamic_cast<CQChartsForceDirectedPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
}

void
CQChartsForceDirectedPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  runningCheck_->setChecked(forceDirectedPlot_->isAnimating());

  //---

  connectSlots(true);

  CQChartsConnectionPlotCustomControls::updateWidgets();
}

void
CQChartsForceDirectedPlotCustomControls::
runningSlot(int state)
{
  if (forceDirectedPlot_)
    forceDirectedPlot_->setAnimating(state);
}

void
CQChartsForceDirectedPlotCustomControls::
stepSlot()
{
  if (forceDirectedPlot_)
    forceDirectedPlot_->execAnimateStep();
}

CQChartsColor
CQChartsForceDirectedPlotCustomControls::
getColorValue()
{
  return forceDirectedPlot_->nodeFillColor();
}

void
CQChartsForceDirectedPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  forceDirectedPlot_->setNodeFillColor(c);
}
