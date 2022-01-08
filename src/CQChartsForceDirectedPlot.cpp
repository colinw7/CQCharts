#include <CQChartsForceDirectedPlot.h>
#include <CQChartsConnectionList.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsValueSet.h>
#include <CQChartsVariant.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QCheckBox>

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
 CQChartsObjEdgeLineData <CQChartsForceDirectedPlot>(this)
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

  forceDirected_ = new CQChartsForceDirected;

  setOuterMargin(PlotMargin(Length::plot(0), Length::plot(0), Length::plot(0), Length::plot(0)));

  setNodeFillColor  (Color());
  setNodeStrokeAlpha(Alpha(0.5));

  setAutoFit(true);
}

void
CQChartsForceDirectedPlot::
term()
{
  delete forceDirected_;
}

//---

void
CQChartsForceDirectedPlot::
setRunning(bool b)
{
  running_ = b;

  if (! isRunning())
    stopAnimateTimer();
  else
    startAnimateTimer();
}

void
CQChartsForceDirectedPlot::
setNodeRadius(double r)
{
  CQChartsUtil::testAndSet(nodeRadius_, r, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeScaled(bool b)
{
  CQChartsUtil::testAndSet(nodeScaled_, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeLinesValueWidth(bool b)
{
  CQChartsUtil::testAndSet(edgeLinesValueWidth_, b, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setRangeSize(double r)
{
  CQChartsUtil::testAndSet(rangeSize_, r, [&]() { updateRange(); } );
}

void
CQChartsForceDirectedPlot::
setMaxLineWidth(double r)
{
  CQChartsUtil::testAndSet(maxLineWidth_, r, [&]() { drawObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNodeLabel(bool b)
{
  CQChartsUtil::testAndSet(nodeLabel_, b, [&]() { drawObjs(); } );
}

//---

int
CQChartsForceDirectedPlot::
numNodes() const
{
  return (forceDirected_ ? forceDirected_->nodes().size() : 0);
}

int
CQChartsForceDirectedPlot::
numEdges() const
{
  return (forceDirected_ ? forceDirected_->edges().size() : 0);
}

//---

void
CQChartsForceDirectedPlot::
addProperties()
{
  CQChartsConnectionPlot::addProperties();

  // options
  addProp("options", "running"  , "", "Is running");
  addProp("options", "rangeSize", "", "Range size");

  // node/edge
  addProp("node", "nodeRadius", "radius"     , "Node radius in pixels")->setMinValue(0.0);
  addProp("node", "nodeScaled", "scaleRadius", "Node radius scaled from value")->setMinValue(0.0);
  addProp("node", "nodeLabel" , "label"      , "Show node label");

  addFillProperties("node/fill"  , "nodeFill"  , "Node");
  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("edge", "edgeLinesValueWidth", "scaleWidth", "Line width scaled from value");
  addProp("edge", "maxLineWidth"       , "maxWidth"  , "Max line width in pixels");

  addLineProperties("edge/stroke", "edgeLines" , "Edge");

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
  static bool inside;

  if (inside) return false; // TODO: assert

  inside = true;

  CQPerfTrace trace("CQChartsForceDirectedPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //th->stopAnimateTimer();

  //---

  delete th->forceDirected_;

  th->forceDirected_ = new CQChartsForceDirected;

  //th->forceDirected_->reset();

  th->idConnections_  .clear();
  th->nameNodeMap_    .clear();
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

  if (! rc) {
    inside = false;
    return false;
  }

  //---

  th->filterObjs();

  addIdConnections();

  //---

  stepInit_ = false;

  if (isRunning())
    th->initSteps();

  //---

  inside = false;

  return true;
}

void
CQChartsForceDirectedPlot::
initSteps()
{
  if (! stepInit_) {
    for (int i = 0; i < initSteps_; ++i)
      forceDirected_->step(stepSize_);

    stepInit_ = true;
  }
}

void
CQChartsForceDirectedPlot::
addIdConnections() const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  th->maxGroup_     = 0;
  th->maxValue_     = 0.0;
  th->maxDataValue_ = 0.0;

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (! connectionsData.visible) continue;

    th->maxGroup_ = std::max(th->maxGroup_, connectionsData.group);

    if (connectionsData.value.isSet())
      th->maxDataValue_ = std::max(th->maxDataValue_, connectionsData.value.real());

    for (const auto &connection : connectionsData.connections) {
      if (connection.value.isSet())
        th->maxValue_ = std::max(th->maxValue_, connection.value.real());
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

    auto *pnode = dynamic_cast<CQChartsSpringyNode *>(node.get());
    assert(pnode);

    forceDirected_->addNode(node);

    //auto id = QString("%1:%2").arg(name).arg(group);

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

  th->widthScale_ = (maxValue() > 0.0 ? 1.0/maxValue() : 1.0);

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
      auto pn1 = nodes_.find(connection.node);
      if (pn1 == nodes_.end()) continue;

      auto node1 = (*pn1).second;
      assert(node1);

      if (! connection.value.isSet())
        continue;

      double value = connection.value.real();

      assert(value > 0.0);

      auto edge = forceDirected_->newEdge(node, node1);

      auto *pedge = dynamic_cast<CQChartsSpringyEdge *>(edge.get());
      assert(pedge);

      pedge->setLength(1.0/value);
      pedge->setValue(value);
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

    if (destLinkInd.isValid()) {
      Connection connection;

      connection.node  = srcId;
      connection.value = OptReal(destTotal);
      connection.ind   = srcConnectionsData.ind;

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

    if (srcLinkInd.isValid()) {
      Connection connection;

      connection.node  = destId;
      connection.value = OptReal(destTotal);
      connection.ind   = destConnectionsData.ind;

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
        if (connection.node == destConnectionsData.node) {
          hasConnection = true;
          break;
        }
      }

      if (! hasConnection) {
        Connection connection;

        connection.node = destConnectionsData.node;
        connection.ind  = destConnectionsData.ind;

        srcConnectionsData.connections.push_back(std::move(connection));

        destConnectionsData.parentId = srcConnectionsData.node;
      }
    }
    else {
      Connection connection;

      connection.node  = destConnectionsData.node;
      connection.value = OptReal(pathData.value);
      connection.ind   = srcConnectionsData.ind;

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
            if (parentConnection.node == id)
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

  auto &srcConnectionsData = th->getConnections(fromToData.fromStr);

//if (depth > 0)
//  srcConnectionsData.setDepth(depth);

  // Just node
  if (fromToData.toStr == "") {
    for (const auto &nv : fromToData.nameValues.nameValues()) {
      auto value = nv.second.toString();

      if      (nv.first == "label") {
        srcConnectionsData.label = value;
      }
      else if (nv.first == "color") {
        //srcConnectionsData.color = QColor(value);
      }
    }
  }
  else {
    if (fromToData.fromStr == fromToData.toStr)
      return;

    auto &destConnectionsData = th->getConnections(fromToData.toStr);

//  if (depth > 0)
//    destConnectionsData.setDepth(depth + 1);

    addEdge(srcConnectionsData, destConnectionsData, fromToData.value.realOr(1.0));

    for (const auto &nv : fromToData.nameValues.nameValues()) {
      auto value = nv.second.toString();

      if      (nv.first == "label") {
      }
      else if (nv.first == "color") {
      }
    }
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
      int group = data.row;

      if (plot_->groupColumn().isValid()) {
        ModelIndex groupModelInd(plot_, data.row, plot_->groupColumn(), data.parent);

        bool ok1;
        group = (int) plot_->modelInteger(groupModelInd, ok1);

        if (! ok1)
          return addDataError(groupModelInd, "Non-integer group value");
      }

      //---

      // link objs
      if      (plot_->linkColumn().isValid() && plot_->valueColumn().isValid()) {
        int    srcId  { -1 };
        int    destId { -1 };
        double value  { 0.0 };

        if (! plot_->getNameConnections(group, data, srcId, destId, value, separator_))
          return State::SKIP;

        addConnection(srcId, destId, value);
      }
      // connection objs
      else if (plot_->connectionsColumn().isValid()) {
        if (! plot_->getRowConnections(group, data))
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

      connection.node  = destId;
      connection.value = OptReal(value);
      connection.ind   = srcConnectionsData.ind;

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

    namePair = linkVar.value<CQChartsNamePair>();
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

  int id = data.row;

  if (nodeColumn().isValid()) {
    nodeModelInd = ModelIndex(th, data.row, nodeColumn(), data.parent);

    bool ok2;
    id = (int) modelInteger(nodeModelInd, ok2);
    if (! ok2) return th->addDataError(nodeModelInd, "Non-integer node value");
  }

  //--

  // get connections data for id
  auto &connectionsData = const_cast<ConnectionsData &>(getConnections(id));

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

    connections = connectionsVar.value<CQChartsConnectionList>().connections();
  }
  else {
    bool ok3;
    auto connectionsStr = modelString(connectionsModelInd, ok3);
    if (! ok3) return false;

    CQChartsConnectionList::stringToConnections(connectionsStr, connections);
  }

  for (auto &connection : connections) {
    Connection connection1;

    connection1.node  = connection.node;
    connection1.value = OptReal(connection.value);
    connection1.ind   = connectionsData.ind;

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

  int nv = tableConnectionDatas.size();

  for (int row = 0; row < nv; ++row) {
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

      connection.node  = value.to;
      connection.value = OptReal(value.value);
      connection.ind   = connectionsData.ind;

      connectionsData.connections.push_back(std::move(connection));
    }
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

CQChartsForceDirectedPlot::ConnectionsData &
CQChartsForceDirectedPlot::
getConnections(const QString &str)
{
  auto id = getStringId(str);

  return getConnections(id);
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
  auto p = idConnections_.find(id);

  if (p != idConnections_.end())
    return (*p).second;

  //---

  ConnectionsData data;

  data.node = id;

  p = idConnections_.insert(p, IdConnectionsData::value_type(id, data));

  return (*p).second;
}

//---

void
CQChartsForceDirectedPlot::
addEdge(ConnectionsData &srcConnectionsData,
        ConnectionsData &destConnectionsData, double value) const
{
  Connection connection;

  connection.node  = destConnectionsData.node;
  connection.value = OptReal(value);
  connection.ind   = srcConnectionsData.ind;

  srcConnectionsData.connections.push_back(std::move(connection));
}

//---

int
CQChartsForceDirectedPlot::
getStringId(const QString &str) const
{
  //assert(str.length());

  auto p = nameNodeMap_.find(str);

  if (p != nameNodeMap_.end())
    return (*p).second;

  //---

  int id = nameNodeMap_.size();

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), StringIndMap::value_type(str, id));

  return (*p1).second;
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
  if (pressed_ || ! isRunning())
    return;

  //---

  initSteps();

  forceDirected_->step(stepSize_);

  if (isAutoFit()) {
    double xmin { 0.0 }, ymin { 0.0 }, xmax { 0.0 }, ymax { 0.0 };

    double r = std::max(nodeRadius(), 0.0);

    double xm = pixelToWindowWidth (2*r);
    double ym = pixelToWindowHeight(2*r);

    forceDirected_->calcRange(xmin, ymin, xmax, ymax);

    dataRange_.updateRange(xmin - xm, ymin - ym);
    dataRange_.updateRange(xmax + xm, ymax + ym);

    //applyDataRange();
  }

  drawObjs();

  //---

  if (! isUpdatesEnabled()) {
    setUpdatesEnabled(true);

    setUpdatesEnabled(false);
  }
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

    auto *node = dynamic_cast<CQChartsSpringyNode *>(nodePoint.first.get());
    if (! node) return false;

    CQChartsTableTip tableTip;

    auto pc = connectionNodes_.find(node->id());

    tableTip.addTableRow("Id", node->id());

    if (pc != connectionNodes_.end()) {
      auto &connectionsData = getConnections((*pc).second);

      tableTip.addTableRow("Label", connectionsData.name);
      tableTip.addTableRow("Group", connectionsData.group);

      if (connectionsData.total.isSet())
        tableTip.addTableRow("Total", connectionsData.total.real());

      tableTip.addTableRow("Connections", connectionsData.connections.size());
    }
    else
      tableTip.addTableRow("Label", QString::fromStdString(node->label()));

    if (node->nodeValue().isSet())
      tableTip.addTableRow("Value", node->nodeValue().real());
    else
      tableTip.addTableRow("Value", node->value());

    tip = tableTip.str();

    return true;
  }

  return false;
}

#if 0
void
CQChartsForceDirectedPlot::
draw(QPainter *painter)
{
  drawParts(painter);

  //---

  {
  LockMutex lock(this, "draw");

  auto updateState = this->updateState();

  if (updateState == UpdateState::READY)
    setGroupedUpdateState(UpdateState::DRAWN);
  }
}
#endif

void
CQChartsForceDirectedPlot::
drawPlotParts(QPainter *painter) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  if (! hasLockId()) {
    LockMutex lock(th, "drawParts");

    CQChartsPlotPaintDevice device(th, painter);

    drawDeviceParts(&device);
  }
}

void
CQChartsForceDirectedPlot::
drawDeviceParts(PaintDevice *device) const
{
  device->save();

  setClipRect(device);

  // draw edges
  PenBrush edgePenBrush;

  setEdgeLineDataPen(edgePenBrush.pen, ColorInd());

  for (auto &edge : forceDirected_->edges()) {
#if 0
    bool isTemp = false;

    auto *spring = forceDirected_->spring(edge, isTemp);

    const auto &p1 = spring->point1()->p();
    const auto &p2 = spring->point2()->p();
#else
    auto p1 = forceDirected_->point(edge->source())->p();
    auto p2 = forceDirected_->point(edge->target())->p();
#endif

    if (isEdgeLinesValueWidth()) {
      auto edgePenBrush1 = edgePenBrush;

      double w = maxLineWidth()*(widthScale_*edge->value());

      if (w > 0.5) {
        edgePenBrush1.pen.setWidthF(w);

        device->setPen(edgePenBrush1.pen);

        double ww = pixelToWindowWidth(w);

        device->drawRoundedLine(Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), ww);
      }
      else {
        device->setPen(edgePenBrush.pen);

        device->drawLine(Point(p1.x(), p1.y()), Point(p2.x(), p2.y()));
      }
    }
    else {
      device->setPen(edgePenBrush.pen);

      device->drawLine(Point(p1.x(), p1.y()), Point(p2.x(), p2.y()));
    }

#if 0
    if (isTemp)
      delete spring;
#endif
  }

  // draw nodes
  double r = std::max(nodeRadius(), 0.0);

  double xm = pixelToWindowWidth (2*r);
  double ym = pixelToWindowHeight(2*r);

  for (auto &node : forceDirected_->nodes()) {
    auto *snode = dynamic_cast<CQChartsSpringyNode *>(node.get());

    double rn  = r;
    double xmn = xm;
    double ymn = ym;

    if (isNodeScaled() && snode->nodeValue().isSet()) {
      rn = nodeRadius()*(snode->nodeValue().real()/maxDataValue());

      xmn = pixelToWindowWidth (2*rn);
      ymn = pixelToWindowHeight(2*rn);
    }

    auto point = forceDirected_->point(node);

    const auto &p1 = point->p();

    //---

    PenBrush penBrush;

    auto pc = interpNodeStrokeColor(ColorInd());

    auto fc = calcPointFillColor(snode);

    if (node == forceDirected_->currentNode())
      fc = insideColor(fc);

    setPenBrush(penBrush, nodePenData(pc), nodeBrushData(fc));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    BBox ebbox(p1.x() - xmn/2.0, p1.y() - ymn/2.0, p1.x() + xmn/2.0, p1.y() + ymn/2.0);

    device->drawEllipse(ebbox);

    if (isNodeLabel()) {
      CQChartsTextOptions textOptions;

      textOptions.scaled    = true;
      textOptions.formatted = true;

      CQChartsDrawUtil::drawTextInBox(device, ebbox, QString::fromStdString(node->label()),
                                      textOptions);
    }
  }

  //---

  device->restore();
}

QColor
CQChartsForceDirectedPlot::
calcPointFillColor(Node *node) const
{
  ColorInd colorInd;

  if      (colorType() == ColorType::GROUP)
    colorInd = ColorInd(node->group(), maxGroup_);
  else if (colorType() == ColorType::INDEX)
    colorInd = ColorInd(node->id(), nodes_.size());
  else
    colorInd = ColorInd(node->value());

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
  plot_ = dynamic_cast<CQChartsForceDirectedPlot *>(plot);

  CQChartsConnectionPlotCustomControls::setPlot(plot);
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
  plot_->setRunning(state);
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
