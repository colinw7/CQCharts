#include <CQChartsForceDirectedPlot.h>
#include <CQChartsConnectionList.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsArrow.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQChartsDisplayRange.h>

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
     p("The styling (fill, stroke) of the nodes and edges can be set").
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

  //---

  NoUpdate noUpdate(this);

  forceDirected_ = std::make_unique<CQChartsForceDirected>();

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));

  setEqualScale(true);

  //---

  auto bg = Color::makePalette();

  setNodeFilled(true);
  setNodeFillColor(bg);

  setNodeStroked(true);
  setNodeStrokeAlpha(Alpha(0.2));

  //---

  setEdgeFilled(true);
  setEdgeFillColor(bg);
  setEdgeFillAlpha(Alpha(0.25));

  setEdgeStroked(true);
  setEdgeStrokeAlpha(Alpha(0.2));

  //---

  setAutoFit(true);
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
  CQChartsUtil::testAndSet(nodeShape_, s, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeScaled_, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeRadius(double r)
{
  CQChartsUtil::testAndSet(nodeRadius_, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsForceDirectedPlot::
setEdgeShape(const EdgeShape &s)
{
  CQChartsUtil::testAndSet(edgeShape_, s, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeScaled(bool b)
{
  CQChartsUtil::testAndSet(edgeScaled_, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeArrow(bool b)
{
  CQChartsUtil::testAndSet(edgeArrow_, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeWidth(double r)
{
  CQChartsUtil::testAndSet(edgeWidth_, r, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setArrowWidth(double w)
{
  CQChartsUtil::testAndSet(arrowWidth_, w, [&]() { drawObjs(); } );
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
  addProp("node", "nodeShape" , "shapeType", "Node shape type");
  addProp("node", "nodeScaled", "scaled"   , "Node radius scaled from value");
  addProp("node", "nodeRadius", "radius"   , "Node radius in pixels")->setMinValue(0.0);

  // node style
  addProp("node/stroke", "nodeStroked", "visible", "Node stroke visible");

  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("node/fill", "nodeFilled", "visible", "Node fill visible");

  addFillProperties("node/fill", "nodeFill", "Node");

  //---

  // edge
  addProp("edge", "edgeShape" , "shapeType" , "Edge shape type");
  addProp("edge", "edgeArrow" , "arrow"     , "Edge arrow");
  addProp("edge", "edgeScaled", "scaled"    , "Line width scaled from value");
  addProp("edge", "edgeWidth" , "width"     , "Max edge width in pixels");
  addProp("edge", "arrowWidth", "arrowWidth", "Directed edge arrow width factor");

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

  return dataRange;
}

bool
CQChartsForceDirectedPlot::
createObjs(PlotObjs &) const
{
  std::unique_lock<std::mutex> lock(createMutex_);

  CQPerfTrace trace("CQChartsForceDirectedPlot::createObjs");

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
    rc = initLinkConnectionObjs();
  else if (columnDataType == ColumnDataType::CONNECTIONS)
    rc = initLinkConnectionObjs();
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
}

void
CQChartsForceDirectedPlot::
addIdConnections() const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  th->maxGroup_     = 0;
  th->maxNodeValue_ = 0.0;
  th->maxEdgeValue_ = 0.0;

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    th->maxGroup_ = std::max(th->maxGroup_, connectionsData.group);

    if (connectionsData.value.isSet())
      th->maxNodeValue_ = std::max(th->maxNodeValue_, connectionsData.value.real());

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
    int         group = connectionsData.group;

    auto node = forceDirected_->newNode();

    auto *pnode = dynamic_cast<Node *>(node.get());
    assert(pnode);

    if (! forceDirected_->getNode(node->id()))
      forceDirected_->addNode(node);

    //auto id = QString("%1:%2").arg(name).arg(group);

    if (name.length())
      pnode->setLabel(name.toStdString());

    pnode->setMass (nodeMass_);
    pnode->setValue((1.0*group)/maxGroup_);
    pnode->setInd  (connectionsData.ind);

    pnode->setGroup(group);

    if (connectionsData.value.isSet())
      pnode->setNodeValue(connectionsData.value);

    th->nodes_          [id         ] = node;
    th->connectionNodes_[pnode->id()] = id;
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

      if (! connection.value.isSet())
        continue;

      double value = connection.value.real();

      assert(value > 0.0);

      auto edge = forceDirected_->newEdge(node, node1);

      auto *sedge = dynamic_cast<Edge *>(edge.get());
      assert(sedge);

      sedge->setLength(1.0/value);
      sedge->setValue(value);
    }
  }
}

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
  int srcDepth = srcHierData.linkStrs.size();

  initHierObjsAddConnection(srcHierData .parentStr, srcHierData .parentLinkInd, srcHierData .total,
                            destHierData.parentStr, destHierData.parentLinkInd, destHierData.total,
                            srcDepth);
}

void
CQChartsForceDirectedPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  int srcDepth = srcHierData.linkStrs.size();

  initHierObjsAddConnection(srcHierData .parentStr, srcHierData .parentLinkInd, srcHierData .total,
                            destHierData.parentStr, destHierData.parentLinkInd, destHierData.total,
                            srcDepth);
}

void
CQChartsForceDirectedPlot::
initHierObjsAddConnection(const QString &srcStr, const ModelIndex &srcLinkInd, double srcTotal,
                          const QString &destStr, const ModelIndex &destLinkInd, double destTotal,
                          int depth) const
{
  assert(destTotal > 0.0);

  auto srcId  = getStringId(srcStr);
  auto destId = getStringId(destStr);

  assert(srcId != destId);

  auto &srcConnectionsData  = const_cast<ConnectionsData &>(getConnections(srcId ));
  auto &destConnectionsData = const_cast<ConnectionsData &>(getConnections(destId));

  //---

  if (srcLinkInd.isValid()) {
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

    // src ->dest connections
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

  CQChartsConnectionPlot::initPathObjs();

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
      bool hasConnection = false;

      for (auto &connection : srcConnectionsData.connections) {
        if (connection.destNode == destConnectionsData.node) {
          hasConnection = true;
          break;
        }
      }

      if (! hasConnection) {
        Connection connection;

        connection.srcNode  = srcConnectionsData.node;
        connection.destNode = destConnectionsData.node;
        connection.ind      = destConnectionsData.ind;

        srcConnectionsData.connections.push_back(std::move(connection));

        destConnectionsData.parentId = srcConnectionsData.node;
      }
    }
    else {
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

//if (depth > 0)
//  srcConnectionsData.setDepth(depth);

  //---

  // TODO: group ?

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
  }
  else {
    // No self connect (allow ?)
    if (fromToData.fromStr == fromToData.toStr)
      return;

    // get dest node
    auto &destConnectionsData = th->getConnections(fromToData.toStr);

//  if (depth > 0)
//    destConnectionsData.setDepth(depth + 1);

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
      long group = data.row;

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

    // create connection data for connection id
    auto &connectionsData =
      const_cast<ConnectionsData &>(getConnections(tableConnectionData.from()));

    connectionsData.ind   = tableConnectionData.nameInd();
    connectionsData.name  = tableConnectionData.name();
    connectionsData.group = tableConnectionData.group().ig;

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
  if      (name == "label") {
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
}

void
CQChartsForceDirectedPlot::
processEdgeNameValues(Connection *connection, const NameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name     = nv.first;
    auto        valueStr = nv.second.toString();

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

    double r = std::max(nodeRadius(), 0.0);

    double xm = pixelToWindowWidth (2*r);
    double ym = pixelToWindowHeight(2*r);

    forceDirected_->calcRange(xmin, ymin, xmax, ymax);

    calcDataRange_ = Range();

    calcDataRange_.updateRange(xmin - xm, ymin - ym);
    calcDataRange_.updateRange(xmax + xm, ymax + ym);

    unequalDataRange_ = adjustDataRange(calcDataRange_);

    dataRange_ = unequalDataRange_;

    applyEqualScale(dataRange_);

    outerDataRange_ = dataRange_;

    //---

    displayRange_->setWindowRange(dataRange_.xmin(), dataRange_.ymin(),
                                  dataRange_.xmax(), dataRange_.ymax());
  }
}

void
CQChartsForceDirectedPlot::
autoFitUpdate()
{
}

bool
CQChartsForceDirectedPlot::
handleSelectPress(const Point &p, SelMod /*selMod*/)
{
  auto nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

  forceDirected_->setCurrentNode (nodePoint.first );
  forceDirected_->setCurrentPoint(nodePoint.second);

  pressed_ = true;

  drawObjs();

  return true;
}

bool
CQChartsForceDirectedPlot::
handleSelectMove(const Point &p, Constraints constraints, bool first)
{
  if (pressed_) {
    if (forceDirected_->currentPoint())
      forceDirected_->currentPoint()->setP(Springy::Vector(p.x, p.y));

    drawObjs();

    return true;
  }
  else {
    auto nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

    forceDirected_->setCurrentNode (nodePoint.first );
    forceDirected_->setCurrentPoint(nodePoint.second);

    //---

    if (! running_) {
      Node *insideNode = nullptr;
      Edge *insideEdge = nullptr;

      for (auto &node : forceDirected_->nodes()) {
        auto *snode = dynamic_cast<Node *>(node.get());
        assert(snode);

        snode->setInside(false);

        if (! insideNode && snode->bbox().inside(p))
          insideNode = snode;
      }

      double minDist = 9999;

      for (auto &edge : forceDirected_->edges()) {
        auto *sedge = dynamic_cast<Edge *>(edge.get());
        assert(sedge);

        sedge->setInside(false);

        if (sedge->getIsLine()) {
          double d;

          if (CQChartsUtil::PointLineDistance(p, sedge->startPoint(), sedge->endPoint(), &d)) {
            if (! insideEdge || d < minDist) {
              insideEdge = sedge;
              minDist    = d;
            }
          }
        }
        else {
          if (! insideEdge && sedge->curvePath().contains(p.qpoint()))
            insideEdge = sedge;
        }
      }

      if      (insideNode)
        insideNode->setInside(true);
      else if (insideEdge)
        insideEdge->setInside(true);

      drawObjs();
    }
  }

  return CQChartsPlot::handleSelectMove(p, constraints, first);
}

bool
CQChartsForceDirectedPlot::
handleSelectRelease(const Point &p)
{
  if (forceDirected_->currentPoint())
    forceDirected_->currentPoint()->setP(Springy::Vector(p.x, p.y));

  forceDirected_->setCurrentNode (nullptr);
  forceDirected_->setCurrentPoint(nullptr);

  pressed_ = false;

  drawObjs();

  return true;
}

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

bool
CQChartsForceDirectedPlot::
plotTipText(const Point &p, QString &tip, bool /*single*/) const
{
  if (! isRunning()) {
    auto nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

    auto *node = dynamic_cast<Node *>(nodePoint.first.get());
    if (! node) return false;

    CQChartsTableTip tableTip;

    auto pc = connectionNodes_.find(node->id());

    tableTip.addTableRow("Id", node->id());

    if (pc != connectionNodes_.end()) {
      auto &connectionsData = getConnections((*pc).second);

      if (connectionsData.name.length())
        tableTip.addTableRow("Label", connectionsData.name);
      else
        tableTip.addTableRow("Label", calcNodeLabel(node));

      tableTip.addTableRow("Group", connectionsData.group);

      if (connectionsData.total.isSet())
        tableTip.addTableRow("Total", connectionsData.total.real());

      tableTip.addTableRow("Connections", connectionsData.connections.size());
    }
    else
      tableTip.addTableRow("Label", calcNodeLabel(node));

    tableTip.addTableRow("Value", calcNodeValue(node));

    tip = tableTip.str();

    return true;
  }

  return false;
}

QString
CQChartsForceDirectedPlot::
calcNodeLabel(Node *node) const
{
  auto label = QString::fromStdString(node->label());

  if (label == "")
    label = getIdString(node->id());

  return label;
}

void
CQChartsForceDirectedPlot::
drawParts(QPainter *painter) const
{
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

  // draw edges
  int numEdges = int(forceDirected_->edges().size());
  int edgeNum  = 0;

  auto edgeType = static_cast<CQChartsDrawUtil::EdgeType>(edgeShape());

  for (auto &edge : forceDirected_->edges()) {
    auto *sedge = dynamic_cast<Edge *>(edge.get());
    assert(sedge);

    //---

    // set edge brush
    PenBrush edgePenBrush;

    ColorInd colorInd(edgeNum++, numEdges);

    auto fc = interpEdgeFillColor(colorInd);
    auto sc = interpEdgeStrokeColor(colorInd);

    if (sedge->isInside())
      fc = insideColor(fc);

    setPenBrush(edgePenBrush, edgePenData(sc), edgeBrushData(fc));

    //---

    auto *snode = dynamic_cast<Node *>(edge->source().get());
    auto *tnode = dynamic_cast<Node *>(edge->target().get());

    auto sbbox = nodeBBox(edge->source(), snode);
    auto tbbox = nodeBBox(edge->target(), tnode);

    // get default connection line (no path)
    Point           ep1, ep2;
    Qt::Orientation orient1 = Qt::Horizontal, orient2 = Qt::Horizontal;

    CQChartsDrawUtil::rectConnectionPoints(sbbox, tbbox, ep1, ep2, orient1, orient2,
                                            /*cornerPoints*/false);

    //---

    // calc edge paths
    bool isArrow = isEdgeArrow();
    bool isLine  = false;

    QPainterPath edgePath, curvePath;

    double lw = 0.0;

    if (isEdgeScaled())
      lw = edgeWidth()*(edgeScale_*edge->value());
    else
      lw = edgeWidth();

    if (lw > 0.5) {
      double lww = pixelToWindowWidth(lw);

      CQChartsDrawUtil::curvePath(edgePath, ep1, ep2, edgeType, orient1, orient2);

      if (isArrow) {
        CQChartsArrowData arrowData;

        arrowData.setFHeadType(CQChartsArrowData::HeadType::NONE);  // directional
        arrowData.setTHeadType(CQChartsArrowData::HeadType::ARROW);

        CQChartsArrow::pathAddArrows(edgePath, arrowData, lww,
                                     arrowWidth(), arrowWidth(), curvePath);
      }
      else {
        CQChartsDrawUtil::edgePath(curvePath, ep1, ep2, lww, edgeType, orient1, orient2);
      }
    }
    else {
      CQChartsDrawUtil::linePath(edgePath, ep1, ep2);

      isLine = true;
    }

    //---

    // draw path
    CQChartsDrawUtil::setPenBrush(device, edgePenBrush);

    if (isLine)
      device->drawPath(edgePath);
    else {
      if (edgeType == CQChartsDrawUtil::EdgeType::RECTILINEAR)
        device->drawPath(curvePath.simplified());
      else
        device->drawPath(curvePath);
    }

    //---

    // set edge draw geometry
    sedge->setIsLine(isLine);

    sedge->setStartPoint(ep1);
    sedge->setEndPoint  (ep2);

    sedge->setCurvePath(curvePath);
    sedge->setEdgePath (edgePath);
  }

  // draw nodes
  int numNodes = int(forceDirected_->nodes().size());
  int nodeNum  = 0;

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<Node *>(node.get());
    assert(snode);

    //---

    auto colorInd = ColorInd(nodeNum++, numNodes);

    PenBrush penBrush;

    auto pc = interpNodeStrokeColor(colorInd);
    auto fc = calcNodeFillColor(snode);

    if (snode->isInside())
      fc = insideColor(fc);

    setPenBrush(penBrush, nodePenData(pc), nodeBrushData(fc));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    auto ebbox = nodeBBox(node, snode);

    if (nodeShape() == NodeShape::CIRCLE)
      device->drawEllipse(ebbox);
    else
      device->drawRect(ebbox);

    if (isNodeTextVisible()) {
      auto c = interpNodeTextColor(colorInd);

      setPen(penBrush, PenData(true, c, nodeTextAlpha()));

      device->setPen(penBrush.pen);

      //---

      auto textOptions = nodeTextOptions(device);

      textOptions.angle = Angle();
      textOptions.align = Qt::AlignCenter;

      CQChartsDrawUtil::drawTextInBox(device, ebbox, calcNodeLabel(snode), textOptions);
    }

    //---

    snode->setBBox(ebbox);
  }

  //---

  device->restore();
}

CQChartsGeom::BBox
CQChartsForceDirectedPlot::
nodeBBox(const CForceDirected::NodeP &node, Node *snode) const
{
  double xmn, ymn;

  if (isNodeScaled()) {
    auto rn = calcScaledNodeValue(snode);

    xmn = pixelToWindowWidth (2*rn);
    ymn = pixelToWindowHeight(2*rn);
  }
  else {
    double r = std::max(nodeRadius(), 0.0);

    xmn = pixelToWindowWidth (2*r);
    ymn = pixelToWindowHeight(2*r);
  }

  auto point = forceDirected_->point(node);

  const auto &p1 = point->p();

  return BBox(p1.x() - xmn/2.0, p1.y() - ymn/2.0, p1.x() + xmn/2.0, p1.y() + ymn/2.0);
}

double
CQChartsForceDirectedPlot::
calcNodeValue(Node *node) const
{
  if (node->nodeValue().isSet())
    return node->nodeValue().real();
  else
    return node->value();
}

double
CQChartsForceDirectedPlot::
calcNormalizedNodeValue(Node *node) const
{
  auto value = calcNodeValue(node);

  return (value > 0.0 ? value/maxNodeValue() : 0.0);
}

double
CQChartsForceDirectedPlot::
calcScaledNodeValue(Node *node) const
{
  return nodeRadius()*calcNormalizedNodeValue(node);
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

    colorInd = ColorInd(value);
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

  //---

  // options group
  auto optionsFrame = createGroupFrame("Options", "optionsFrame");

  runningCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Running", "runningCheck");

  addFrameColWidget(optionsFrame, runningCheck_);

  //---

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
  CQChartsWidgetUtil::connectDisconnect(b,
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
