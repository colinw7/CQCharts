#include <CQChartsForceDirectedPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsArrow.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsDisplayRange.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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
  term();
}

//---

void
CQChartsForceDirectedPlot::
init()
{
  CQChartsConnectionPlot::init();

  setSymmetric(false);

  //---

  NoUpdate noUpdate(this);

  forceDirected_ = std::make_unique<CQChartsForceDirected>();

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
}

void
CQChartsForceDirectedPlot::
term()
{
}

//---

void
CQChartsForceDirectedPlot::
setRunning(bool b)
{
  if (b != running_) {
    running_ = b;

    if (! isRunning())
      stopAnimateTimer();
    else
      startAnimateTimer();

    emit customDataChanged();
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
setNodeMouseColoring(bool b)
{
  CQChartsUtil::testAndSet(nodeDrawData_.mouseColoring, b, [&]() { drawObjs(); } );
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
setEdgeArrow(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.arrow, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeDrawData_.width, l, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setArrowWidth(double w)
{
  CQChartsUtil::testAndSet(edgeDrawData_.arrowWidth, w, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeValueColored(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.valueColored, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeMouseColoring(bool b)
{
  CQChartsUtil::testAndSet(edgeDrawData_.mouseColoring, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsForceDirectedPlot::
setRangeSize(double r)
{
  CQChartsUtil::testAndSet(rangeSize_, r, [&]() { updateRange(); } );
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

  // animation data
  addProp("options", "running"     , "", "Is running");
  addProp("options", "initSteps"   , "", "Initial steps");
  addProp("options", "animateSteps", "", "Animate steps");
  addProp("options", "numSteps"    , "", "Number of steps");
  addProp("options", "stepSize"    , "", "Step size");
  addProp("options", "rangeSize"   , "", "Range size");

  // node
  addProp("node", "nodeShape"        , "shapeType"    , "Node shape type");
  addProp("node", "nodeScaled"       , "scaled"       , "Node scaled by value");
  addProp("node", "nodeSize"         , "size"         , "Node size (ignore if <= 0)");
  addProp("node", "minNodeSize"      , "minSize"      , "Node min size (ignore if <= 0)");
  addProp("node", "nodeValueColored" , "valueColored" , "Node colored by value");
  addProp("node", "nodeMouseColoring", "mouseColoring", "Color node edges on mouse over");

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
  addProp("edge", "edgeMouseColoring", "mouseColoring", "Color edge nodes on mouse over");

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

  // info
  addProp("stats", "numNodes", "", "Number of nodes");
  addProp("stats", "numEdges", "", "Number of edges");
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

  th->forceDirected_ = std::make_unique<CQChartsForceDirected>();

  //th->forceDirected_->reset();

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

  th->filterObjs();

  addIdConnections();

  //---

  numSteps_ = 0;

  th->execInitSteps();

  //---

  return true;
}

void
CQChartsForceDirectedPlot::
execInitSteps()
{
  while (numSteps_ < initSteps()) {
    forceDirected_->step(stepSize());

    ++numSteps_;
  }

  doAutoFit();

  drawObjs();
}

void
CQChartsForceDirectedPlot::
addIdConnections() const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  th->maxGroup_     = 0;
  th->maxEdgeValue_ = 0.0;

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    th->maxGroup_ = std::max(th->maxGroup_, connectionsData.group);

    for (const auto &connection : connectionsData.connections) {
      if (connection.value.isSet())
        th->maxEdgeValue_ = std::max(th->maxEdgeValue_, connection.value.real());
    }
  }

  if (th->maxGroup_ <= 0)
    th->maxGroup_ = 1;

  //---

  th->nodes_          .clear();
  th->connectionNodes_.clear();

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    //if (connectionsData.connections.empty()) continue;

    int id = idConnections.first;

    const auto &name  = connectionsData.name;

    int group = connectionsData.group;

    if (group < 0)
      group = 0;

    auto node = forceDirected_->newNode();

    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (! forceDirected_->getNode(node->id()))
      forceDirected_->addNode(node);

    //auto id = QString("%1:%2").arg(name).arg(group);

    if (name.length())
      snode->setLabel(name.toStdString());

    snode->setMass (nodeMass_);
    snode->setGroup(group);
    snode->setInd  (connectionsData.ind);

    if (connectionsData.value.isSet())
      snode->setNodeValue(connectionsData.value);

    if (connectionsData.shapeType != NodeShape::NONE)
      snode->setShape(static_cast<Node::Shape>(connectionsData.shapeType));

    th->nodes_          [id         ] = node;
    th->connectionNodes_[snode->id()] = id;
  }

  th->edgeScale_ = (maxEdgeValue() > 0.0 ? 1.0/maxEdgeValue() : 1.0);

  //---

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    if (connectionsData.connections.empty())
      continue;

    int id = idConnections.first;

    auto pn = nodes_.find(id);
    assert(pn != nodes_.end());

    auto node = (*pn).second;
    assert(node);

    for (const auto &connection : connectionsData.connections) {
      auto pn1 = nodes_.find(connection.destNode);
      if (pn1 == nodes_.end()) continue;

      auto node1 = (*pn1).second;
      assert(node1);

      double value = 0.0;

      if (connection.value.isSet())
        value = connection.value.real();

      auto edge = forceDirected_->newEdge(node, node1);

      auto *sedge = dynamic_cast<Edge *>(edge.get());
      assert(sedge);

      sedge->setLength(value > 0.0 ? 1.0/value : 1.0);
      sedge->setValue(value);

      if (connection.label.length())
        sedge->setLabel(connection.label.toStdString());

      if (connection.shapeType != EdgeShape::NONE)
        sedge->setShape(static_cast<Edge::Shape>(connection.shapeType));
    }
  }

  //---

  th->maxNodeValue_ = 0.0;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    auto value = calcNodeValue(snode);

    if (value.isSet())
      th->maxNodeValue_ = std::max(th->maxNodeValue_, value.real());
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
      srcConnectionsData.fillColor = c;

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
#if 0
    auto addModelInd = [&](const ModelIndex &modelInd) {
      if (modelInd.isValid())
        connection->addModelInd(modelInd);
    };

    auto fromModelIndex  = modelIndex(fromToData.fromModelInd);
    auto fromModelIndex1 = normalizeIndex(fromModelIndex);

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

    //---

    // set edge color (if color column specified)
    // Note: from and to are same row so we can use either
    Color c;

    if (colorColumnColor(fromToData.fromModelInd.row(), fromToData.fromModelInd.parent(), c))
      connection->fillColor = c;

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

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsForceDirectedPlot *plot) :
     plot_(plot) {
      separator_ = plot_->calcSeparator();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // Get group value
      long group = -1;

      if (plot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(plot_, data.row, plot_->groupColumn(), data.parent);

        bool ok1;
        group = plot_->modelInteger(groupModelInd, ok1);

        if (! ok1)
          return addDataError(groupModelInd, "Non-integer group value");
      }

      int igroup = static_cast<int>(group);

      //---

      // link objs
      if      (plot_->linkColumn().isValid() && plot_->valueColumn().isValid()) {
        int    srcId  { -1 };
        int    destId { -1 };
        double value  { 0.0 };

        if (! plot_->getNameConnections(igroup, data, srcId, destId, value, separator_))
          return State::SKIP;

        addConnection(srcId, destId, value);
      }
      // connection objs
      else if (plot_->connectionsColumn().isValid()) {
        if (! plot_->getRowConnections(igroup, data))
          return State::SKIP;
      }
      else {
        assert(false);
      }

      return State::OK;
    }

    void addConnection(int srcId, int destId, double value) {
      auto &srcConnectionsData = const_cast<ConnectionsData &>(plot_->getConnections(srcId));

      Connection connection;

      connection.srcNode  = srcId;
      connection.destNode = destId;
      connection.value    = OptReal(value);
      connection.ind      = srcConnectionsData.ind;

      srcConnectionsData.connections.push_back(std::move(connection));
    }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsForceDirectedPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsForceDirectedPlot* plot_      { nullptr };
    QString                          separator_ { "/" };
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
  // shape
  if      (name == "shape") {
    NodeShape shapeType;

    stringToShapeType(valueStr, shapeType);

    connectionsData.shapeType = shapeType;
  }
  // shape num sides
  else if (name == "label") {
    connectionsData.label = valueStr;
  }
  else if (name == "value") {
    bool ok;

    auto r = CQChartsUtil::toReal(valueStr, ok);

    if (ok)
      connectionsData.value = OptReal(r);
  }
  else if (name == "fill_color" || name == "color") {
    connectionsData.fillColor = Color(valueStr);
  }
  else if (name == "fill_alpha" || name == "alpha") {
    //connectionsData.fillAlpha = CQChartsUtil::toReal(valueStr, ok);
  }
  else if (name == "stroke_color") {
    //connectionsData.strokeColor = Color(valueStr);
  }
  else if (name == "stroke_alpha") {
    //connectionsData.strokeAlpha = CQChartsUtil::toReal(valueStr, ok);
  }
#if 0
  else if (name == "stroke_width" || name == "width") {
    node->setStrokeWidth(CQChartsLength(valueStr));
  }
  else if (name == "stroke_dash" || name == "dash") {
    node->setStrokeDash(CQChartsLineDash(valueStr));
  }
#endif
  else {
    auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

    th->addDataError(ModelIndex(), QString("Unhandled name '%1'").arg(name));
  }
}

void
CQChartsForceDirectedPlot::
processEdgeNameValues(Connection *connection, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name     = nv.first;
    auto        valueStr = nv.second.toString();

    if      (name == "shape") {
      EdgeShape shapeType;

      stringToShapeType(valueStr, shapeType);

      connection->shapeType = shapeType;
    }
    if      (name == "label") {
      connection->label = valueStr;
    }
    else if (name == "color") {
      connection->fillColor = CQChartsColor(valueStr);
    }
    else if (name.left(4) == "src_") {
      auto &srcConnections =
        const_cast<ConnectionsData &>(getConnections(connection->srcNode));

      processNodeNameValue(srcConnections, name.mid(4), valueStr);
    }
    else if (name.left(5) == "dest_") {
      auto &destConnections =
        const_cast<ConnectionsData &>(getConnections(connection->destNode));

      processNodeNameValue(destConnections, name.mid(5), valueStr);
    }
  }
}

void
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
  }
}

void
CQChartsForceDirectedPlot::
stringToShapeType(const QString &str, EdgeShape &shapeType)
{
  if      (str == "arc" )        shapeType = EdgeShape::ARC;
  else if (str == "line")        shapeType = EdgeShape::LINE;
  else if (str == "rectilinear") shapeType = EdgeShape::RECTILINEAR;
  else                           shapeType = EdgeShape::NONE;
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
addMenuItems(QMenu *menu)
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
  if (isRunning())
    startAnimateTimer();
}

void
CQChartsForceDirectedPlot::
animateStep()
{
  execInitSteps();

  //---

  if (pressed_ || ! isRunning())
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
  for (int i = 0; i < animateSteps(); ++i) {
    forceDirected_->step(stepSize());

    ++numSteps_;
  }

  doAutoFit();

  drawObjs();
}

void
CQChartsForceDirectedPlot::
doAutoFit()
{
  if (isAutoFit()) {
    double xmin { 0.0 }, ymin { 0.0 }, xmax { 0.0 }, ymax { 0.0 };

    double s = lengthPlotWidth(nodeSize());

    auto minSize = pixelToWindowWidth(3.0);

    double xm = std::max(s, minSize);
    double ym = xm;

    forceDirected_->calcRange(xmin, ymin, xmax, ymax);

    calcDataRange_ = Range();

    calcDataRange_.updateRange(xmin - xm, ymin - ym);
    calcDataRange_.updateRange(xmax + xm, ymax + ym);

    unequalDataRange_ = adjustDataRange(calcDataRange_);

    dataRange_ = unequalDataRange_;

    applyEqualScale(dataRange_);

    outerDataRange_ = dataRange_;

    //---

    auto bbox = CQChartsUtil::rangeBBox(dataRange_);

    auto adjustedBBox = adjustDataRangeBBox(bbox);

    setWindowRange(bbox, adjustedBBox);
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
handleSelectPress(const Point &p, SelMod /*selMod*/)
{
  setCurrentNode(p);

  pressed_ = true;

  selectAt(p);

  drawObjs();

  return true;
}

bool
CQChartsForceDirectedPlot::
handleSelectMove(const Point &p, Constraints, bool)
{
  setCurrentNode(p);

  if (! running_)
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
    if (forceDirected_->currentPoint())
      forceDirected_->currentPoint()->setP(Springy::Vector(p.x, p.y));

    drawObjs();

    return true;
  }
  else {
    setCurrentNode(p);

    //---

    if (! running_)
      updateInside(p);
  }

  return true;
}

bool
CQChartsForceDirectedPlot::
handleEditRelease(const Point &, const Point &p)
{
  if (forceDirected_->currentPoint())
    forceDirected_->currentPoint()->setP(Springy::Vector(p.x, p.y));

  resetCurrentNode();

  pressed_ = false;

  drawObjs();

  return true;
}

//---

void
CQChartsForceDirectedPlot::
setCurrentNode(const Point &p)
{
  auto nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

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
selectAt(const Point &p)
{
  using NodeSet = std::set<Node *>;
  using EdgeSet = std::set<Edge *>;

  NodeSet selectedNodes;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (snode->isSelected())
      selectedNodes.insert(snode);
  }

  EdgeSet selectedEdges;

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    if (sedge->isSelected())
      selectedEdges.insert(sedge);
  }

  //---

  Node *selectedNode = nullptr;
  Edge *selectedEdge = nullptr;

  nearestNodeEdge(p, selectedNode, selectedEdge);

  if (selectedNode) {
    if (selectedNodes.size() == 1 && *selectedNodes.begin() == selectedNode)
      return false;
  }
  else if (selectedEdge) {
    if (selectedEdges.size() == 1 && *selectedEdges.begin() == selectedEdge)
      return false;
  }
  else {
    if (selectedNodes.empty() && selectedEdges.empty())
      return false;
  }

  for (auto *node : selectedNodes)
    node->setSelected(false);

  for (auto *edge : selectedEdges)
    edge->setSelected(false);

  if      (selectedNode)
    selectedNode->setSelected(true);
  else if (selectedEdge)
    selectedEdge->setSelected(true);

  updateSelText();

  drawObjs();

  //---

  if      (selectedNode)
    emit objIdPressed(selectedNode->stringId());
  else if (selectedEdge)
    emit objIdPressed(selectedEdge->stringId());

  return true;
}

bool
CQChartsForceDirectedPlot::
updateInside(const Point &p)
{
  using NodeSet = std::set<Node *>;
  using EdgeSet = std::set<Edge *>;

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
updateSelText()
{
  Node *selectedNode = nullptr;
  Edge *selectedEdge = nullptr;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    if (snode->isSelected()) {
      selectedNode = snode;
      break;
    }
  }

  if (! selectedNode) {
    for (auto &edge : forceDirected_->edges()) {
      auto *sedge = dynamic_cast<Edge *>(edge.get());
      assert(sedge);

      if (sedge->isSelected()) {
        selectedEdge = sedge;
        break;
      }
    }
  }

  //---

  QString selText;

  if      (selectedNode)
    selText = calcNodeLabel(selectedNode);
  else if (selectedEdge) {
    auto *snode = dynamic_cast<Node *>(selectedEdge->source().get());
    auto *tnode = dynamic_cast<Node *>(selectedEdge->target().get());

    if (snode && tnode)
      selText = QString("%1 -> %2").arg(calcNodeLabel(snode)).arg(calcNodeLabel(tnode));
  }

  view()->setSelText(selText);
}

//---

bool
CQChartsForceDirectedPlot::
keyPress(int key, int modifier)
{
  if (key == Qt::Key_S)
    setRunning(! isRunning());
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
  if (! isRunning()) {
    CQChartsTableTip tableTip;

#if 0
    auto nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

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

  tableTip.addTableRow("Id", node->id());

  if (pc != connectionNodes_.end()) {
    auto &connectionsData = getConnections((*pc).second);

    if (connectionsData.name.length())
      tableTip.addTableRow("Label", connectionsData.name);
    else
      tableTip.addTableRow("Label", calcNodeLabel(node));

    if (connectionsData.group >= 0)
      tableTip.addTableRow("Group", connectionsData.group);

    if (connectionsData.total.isSet())
      tableTip.addTableRow("Total", connectionsData.total.real());

    tableTip.addTableRow("Connections", connectionsData.connections.size());
  }
  else
    tableTip.addTableRow("Label", calcNodeLabel(node));

  auto value = calcNodeValue(node);

  if (value.isSet())
    tableTip.addTableRow("Value", value.real());
}

void
CQChartsForceDirectedPlot::
edgeTipText(Edge *edge, CQChartsTableTip &tableTip) const
{
  auto edgeStr = QString::fromStdString(edge->label());

  tableTip.addTableRow("Label", edgeStr);

  if (edge->value())
    tableTip.addTableRow("Value", edge->value().value());

  auto *snode = dynamic_cast<Node *>(edge->source().get());
  auto *tnode = dynamic_cast<Node *>(edge->target().get());

  if (snode) tableTip.addTableRow("From", calcNodeLabel(snode));
  if (tnode) tableTip.addTableRow("To"  , calcNodeLabel(tnode));
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
      if (! insideSolidEdge && sedge->curvePath().contains(p.qpoint()))
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

  CQChartsPlotPaintDevice device(th, painter);

  drawDeviceParts(&device);
}

void
CQChartsForceDirectedPlot::
drawDeviceParts(PaintDevice *device) const
{
  device->save();

  setClipRect(device);

  //--

  // reset node slots
  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    snode->clearOccupiedSlots();
  }

  //---

  // draw edges
  int numEdges = int(forceDirected_->edges().size());
  int edgeNum  = 0;

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    ColorInd colorInd(edgeNum++, numEdges);

    drawEdge(device, edge, sedge, colorInd);
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

    drawNode(device, node, snode, colorInd);
  }

  //---

  if (hasTitle())
    drawTitle(device);

  //---

  device->restore();
}

void
CQChartsForceDirectedPlot::
drawEdge(PaintDevice *device, const CForceDirected::EdgeP &edge, Edge *sedge,
         const ColorInd &colorInd) const
{
  // calc pen and brush
  PenBrush penBrush;

  QColor fillColor;

  if (isEdgeValueColored()) {
    auto value = calcNormalizedEdgeValue(sedge);

    double rvalue = (value.isSet() ? value.real() : 0.0);

    fillColor = interpColor(edgeFillColor(), ColorInd(rvalue));
  }
  else
    fillColor = interpEdgeFillColor(colorInd);

  auto strokeColor = interpEdgeStrokeColor(colorInd);

  if (sedge->isInside())
    fillColor = insideColor(fillColor);

  auto penData   = edgePenData(strokeColor);
  auto brushData = edgeBrushData(fillColor);

  setPenBrush(penBrush, penData, brushData);

  //---

  // get connection rect of source and destination object
  auto *snode = dynamic_cast<Node *>(sedge->source().get());
  auto *tnode = dynamic_cast<Node *>(sedge->target().get());

  auto sbbox = nodeBBox(sedge->source(), snode);
  auto tbbox = nodeBBox(sedge->target(), tnode);

  // get default connection line (no path)
  CQChartsDrawUtil::ConnectPos        ep1, ep2;
  CQChartsDrawUtil::RectConnectData   rectConnectData;
  CQChartsDrawUtil::CircleConnectData circleConnectData;

  auto sshape = calcNodeShape(snode);
  auto tshape = calcNodeShape(tnode);

  auto sc = sbbox.getCenter(); auto sr = sbbox.getWidth()/2.0;
  auto tc = tbbox.getCenter(); auto tr = tbbox.getWidth()/2.0;

  if (sshape == Node::Shape::CIRCLE || sshape == Node::Shape::DOUBLE_CIRCLE) {
    circleConnectData.numSlots      = 16;
    circleConnectData.occupiedSlots = snode->occupiedSlots();

    CQChartsDrawUtil::circleConnectionPoint(sc, sr, tc, tr, ep1, circleConnectData);

    snode->addOccupiedSlot(ep1.slot);
  }
  else
    CQChartsDrawUtil::rectConnectionPoint(sbbox, tbbox, ep1, rectConnectData);

  if (tshape == Node::Shape::CIRCLE || tshape == Node::Shape::DOUBLE_CIRCLE) {
    circleConnectData.numSlots      = 16;
    circleConnectData.occupiedSlots = tnode->occupiedSlots();

    CQChartsDrawUtil::circleConnectionPoint(tc, tr, ep1.p, 0.0, ep2, circleConnectData);

    tnode->addOccupiedSlot(ep2.slot);
  }
  else
    CQChartsDrawUtil::rectConnectionPoint(tbbox, sbbox, ep2, rectConnectData);

  //---

  auto edgeType = calcEdgeShape(sedge);

  // calc edge paths
  bool isArrow = isEdgeArrow();
  bool isLine  = false;

  QPainterPath edgePath, curvePath;

  double edgeWidth = lengthPixelWidth(this->edgeWidth());

  double lw = edgeWidth;

  if (isEdgeScaled() && sedge->value()) {
    auto value = calcNormalizedEdgeValue(sedge);

    if (value.isSet())
      lw = edgeWidth*value.real();
  }

  if (lw > 0.5) {
    double lww = pixelToWindowWidth(lw);

    CQChartsDrawUtil::curvePath(edgePath, ep1.p, ep2.p, edgeType, ep1.angle, ep2.angle);

    if (isArrow) {
      CQChartsArrowData arrowData;

      arrowData.setFHeadType(CQChartsArrowData::HeadType::NONE);  // directional
      arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

      CQChartsArrow::pathAddArrows(edgePath, arrowData, lww,
                                   arrowWidth(), arrowWidth(), curvePath);
    }
    else {
      CQChartsDrawUtil::edgePath(curvePath, ep1.p, ep2.p, lww, edgeType, ep1.angle, ep2.angle);
    }
  }
  else {
    CQChartsDrawUtil::linePath(edgePath, ep1.p, ep2.p);

    isLine = true;
  }

  //---

  // draw path
  QPainterPath edgePath1;

  if (isLine)
    edgePath1 = edgePath;
  else {
    if (edgeType == CQChartsDrawUtil::EdgeType::RECTILINEAR)
      edgePath1 = curvePath.simplified();
    else
      edgePath1 = curvePath;
  }

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawPath(edgePath1);

  edgePaths_[sedge->id()] = edgePath1;

  //---

  // set edge draw geometry
  sedge->setIsLine(isLine);

  sedge->setStartPoint(ep1.p);
  sedge->setEndPoint  (ep2.p);

  sedge->setCurvePath(curvePath);
  sedge->setEdgePath (edgePath);

  //---

  // draw text
  auto edgeStr = QString::fromStdString(sedge->label());

  if (isEdgeTextVisible() && edgeStr.length()) {
    // set font
    setPainterFont(device, edgeTextFont());

    //---

    // set text pen
    auto c = interpEdgeTextColor(colorInd);

    setPen(penBrush, PenData(true, c, edgeTextAlpha()));

    device->setPen(penBrush.pen);

    //---

    auto pt = Point(CQChartsDrawUtil::pathMidPoint(edgePath));

    auto textOptions = nodeTextOptions(device);

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignCenter;

    CQChartsDrawUtil::drawTextAtPoint(device, pt, edgeStr, textOptions);
  }

  //---

  if (sedge->isInside()) {
    if (isEdgeMouseColoring()) {
      penData  .setAlpha(Alpha(1.0));
      brushData.setAlpha(Alpha(1.0));

      setPenBrush(penBrush, penData, brushData);

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      //view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

      drawEdgeNodes(device, edge);

      //view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
    }
  }
}

void
CQChartsForceDirectedPlot::
drawNode(PaintDevice *device, const CForceDirected::NodeP &node, Node *snode,
         const ColorInd &colorInd) const
{
  // calc pen and brush
  PenBrush penBrush;

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

  if      (snode->isInside()) {
    if (snode->isSelected()) {
      view()->updateSelectedObjPenBrushState(colorInd, penBrush);
      view()->updateInsideObjPenBrushState  (colorInd, penBrush);
    }
    else
      view()->updateInsideObjPenBrushState(colorInd, penBrush);
  }
  else if (snode->isSelected())
    view()->updateSelectedObjPenBrushState(colorInd, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw node
  auto shape = calcNodeShape(snode);
  auto ebbox = nodeBBox(node, snode);

  NodeShapeBBox nodeShape;

  nodeShape.shape = shape;
  nodeShape.bbox  = ebbox;

  drawNodeShape(device, nodeShape);

  nodeShapes_[snode->id()] = nodeShape;

  //---

  // draw text
  if (brushData.isVisible())
    charts()->setContrastColor(fc);

  if (isNodeTextVisible()) {
    // set font
    setPainterFont(device, nodeTextFont());

    //---

    // set text pen
    auto c = interpNodeTextColor(colorInd);

    setPen(penBrush, PenData(true, c, nodeTextAlpha()));

    device->setPen(penBrush.pen);

    //---

    auto textOptions = nodeTextOptions(device);

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignCenter;

    if (shape != Node::Shape::BOX)
      CQChartsDrawUtil::drawTextInCircle(device, ebbox, calcNodeLabel(snode), textOptions);
    else
      CQChartsDrawUtil::drawTextInBox(device, ebbox, calcNodeLabel(snode), textOptions);
  }

  charts()->resetContrastColor();

  //---

  snode->setBBox(ebbox);

  //---

  if (snode->isInside()) {
    if (isNodeMouseColoring()) {
      penData  .setAlpha(Alpha(1.0));
      brushData.setAlpha(Alpha(1.0));

      setPenBrush(penBrush, penData, brushData);

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      //view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER_EXTRA);

      drawNodeEdges(device, node);

      //view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
    }
  }
}

void
CQChartsForceDirectedPlot::
drawNodeShape(PaintDevice *device, const NodeShapeBBox &nodeShape) const
{
  if      (nodeShape.shape == Node::Shape::DOUBLE_CIRCLE)
    CQChartsDrawUtil::drawDoubleEllipse(device, nodeShape.bbox);
  else if (nodeShape.shape == Node::Shape::CIRCLE)
    device->drawEllipse(nodeShape.bbox);
  else
    device->drawRect(nodeShape.bbox);
}

//---

void
CQChartsForceDirectedPlot::
drawNodeEdges(PaintDevice *device, const NodeP &node) const
{
  if (! forceDirected_)
    return;

  auto edges = forceDirected_->getEdges(node);

  for (const auto &edge : edges) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    device->drawPath(edgePaths_[sedge->id()]);
  }
}

void
CQChartsForceDirectedPlot::
drawEdgeNodes(PaintDevice *device, const EdgeP &edge) const
{
  if (! forceDirected_)
    return;

  auto *snode1 = dynamic_cast<Node *>(edge->source().get());
  auto *snode2 = dynamic_cast<Node *>(edge->target().get());
  assert(snode1 && snode2);

  drawNodeShape(device, nodeShapes_[snode1->id()]);
  drawNodeShape(device, nodeShapes_[snode2->id()]);
}

//---

void
CQChartsForceDirectedPlot::
postResize()
{
  CQChartsPlot::postResize();

  setNeedsAutoFit(true);

  drawObjs();
}

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
nodeBBox(const CForceDirected::NodeP &node, Node *snode) const
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

  return BBox(p1.x() - xmn/2.0, p1.y() - ymn/2.0, p1.x() + xmn/2.0, p1.y() + ymn/2.0);
}

CQChartsForceDirectedPlot::OptReal
CQChartsForceDirectedPlot::
calcNodeValue(Node *node) const
{
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
    colorInd = ColorInd(node->group(), maxGroup_);
  else if (colorType() == ColorType::INDEX)
    colorInd = ColorInd(node->id(), int(nodes_.size()));
  else {
    auto value = calcNormalizedNodeValue(node);

    if (value.isSet())
      colorInd = ColorInd(value.real());
  }

  //---

  if (colorType() == ColorType::AUTO) {
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

CQChartsForceDirectedPlot::OptReal
CQChartsForceDirectedPlot::
calcNormalizedEdgeValue(Edge *edge) const
{
  if (edge->value())
    return OptReal(edgeScale_*edge->value().value());
  else
    return OptReal();
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

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsForceDirectedPlotCustomControls::
addWidgets()
{
  addConnectionColumnWidgets();

  addColorColumnWidgets("Point Color");

  addOptionsWidgets();

  addRunWidgets();
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
  CQChartsWidgetUtil::optConnectDisconnect(b,
    runningCheck_, SIGNAL(stateChanged(int)), this, SLOT(runningSlot(int)));

  CQChartsConnectionPlotCustomControls::connectSlots(b);
}

void
CQChartsForceDirectedPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsForceDirectedPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsForceDirectedPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  runningCheck_->setChecked(plot_->isRunning());

  CQChartsConnectionPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

void
CQChartsForceDirectedPlotCustomControls::
runningSlot(int state)
{
  if (plot_)
    plot_->setRunning(state);
}

void
CQChartsForceDirectedPlotCustomControls::
stepSlot()
{
  if (plot_)
    plot_->execAnimateStep();
}

CQChartsColor
CQChartsForceDirectedPlotCustomControls::
getColorValue()
{
  return plot_->nodeFillColor();
}

void
CQChartsForceDirectedPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setNodeFillColor(c);
}
