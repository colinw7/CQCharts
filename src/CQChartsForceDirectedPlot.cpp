#include <CQChartsForceDirectedPlot.h>
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

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

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
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsForceDirectedPlotType::
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsForceDirectedPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsForceDirectedPlot(view, model);
}

//---

CQChartsForceDirectedPlot::
CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("forcedirected"), model),
 CQChartsObjNodeShapeData<CQChartsForceDirectedPlot>(this),
 CQChartsObjEdgeLineData <CQChartsForceDirectedPlot>(this)
{
  NoUpdate noUpdate(this);

  forceDirected_ = new CQChartsForceDirected;

  setOuterMargin(0, 0, 0, 0);

  setNodeStrokeAlpha(CQChartsAlpha(0.5));
}

CQChartsForceDirectedPlot::
~CQChartsForceDirectedPlot()
{
  delete forceDirected_;
}

//---

void
CQChartsForceDirectedPlot::
setRunning(bool b)
{
  running_ = b;
}

void
CQChartsForceDirectedPlot::
setNodeRadius(double r)
{
  CQChartsUtil::testAndSet(nodeRadius_, r, [&]() { drawObjs(); } );
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsConnectionPlot::addProperties();

  // options
  addProp("options", "running", "", "Is running");

  // node/edge
  addProp("node", "nodeRadius", "radius", "Node radius in pixels")->setMinValue(0.0);

  addFillProperties("node/fill"  , "nodeFill"  , "Node");
  addLineProperties("node/stroke", "nodeStroke", "Node");

  addProp("edge", "edgeLinesValueWidth", "linesValueWidth", "Line width from value");

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

  //---

  // check columns
  if (! checkColumns())
    return CQChartsGeom::Range(0.0, 0.0, 1.0, 1.0);

  //---

  // TODO: calculate good range size from data or auto scale/fit ?
  CQChartsGeom::Range dataRange;

  double rangeSize = this->rangeSize();

  dataRange.updateRange(-rangeSize, -rangeSize);
  dataRange.updateRange( rangeSize,  rangeSize);

  return dataRange;
}

bool
CQChartsForceDirectedPlot::
createObjs(PlotObjs &) const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  delete th->forceDirected_;

  th->forceDirected_ = new CQChartsForceDirected;

  th->idConnections_.clear();
  th->nameNodeMap_  .clear();

  //---

  bool rc = true;

  if (isHierarchical())
    rc = initHierObjs();
  else {
    if      (linkColumn().isValid() && valueColumn().isValid())
      rc = initLinkConnectionObjs();
    else if (connectionsColumn().isValid())
      rc = initLinkConnectionObjs();
    else
      rc = initTableObjs();
  }

  if (! rc)
    return false;

  //---

  addIdConnections();

  //---

  for (int i = 0; i < initSteps_; ++i)
    forceDirected_->step(stepSize_);

  //---

  return true;
}

void
CQChartsForceDirectedPlot::
addIdConnections() const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  th->maxGroup_ = 0;
  th->maxValue_ = 0.0;

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    th->maxGroup_ = std::max(th->maxGroup_, connectionsData.group);

    for (const auto &connection : connectionsData.connections) {
      th->maxValue_ = std::max(th->maxValue_, connection.value);
    }
  }

  //---

  th->nodes_          .clear();
  th->connectionNodes_.clear();

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (connectionsData.connections.empty())
      continue;

    int id = idConnections.first;

    const QString &name  = connectionsData.name;
    int            group = connectionsData.group;

    auto *node = forceDirected_->newNode();

    QString label = QString("%1:%2").arg(name).arg(group);

    node->setLabel(label);
    node->setMass (nodeMass_);
    node->setGroup(group);
    node->setValue((1.0*group)/maxGroup_);

    th->nodes_          [id        ] = node;
    th->connectionNodes_[node->id()] = id;
  }

  th->widthScale_ = (maxValue_ > 0.0 ? 1.0/maxValue_ : 1.0);

  //---

  for (const auto &idConnections : idConnections_) {
    const auto &connectionsData = idConnections.second;

    if (connectionsData.connections.empty())
      continue;

    int id = idConnections.first;

    auto pn = nodes_.find(id);
    assert(pn != nodes_.end());

    auto *node = (*pn).second;
    assert(node);

    for (const auto &connection : connectionsData.connections) {
      auto pn1 = nodes_.find(connection.node);
      assert(pn1 != nodes_.end());

      auto *node1 = (*pn1).second;
      assert(node1);

      assert(connection.value > 0.0);

      auto *edge = forceDirected_->newEdge(node, node1);

      edge->setLength(1.0/connection.value);
      edge->setValue(connection.value);
    }
  }
}

bool
CQChartsForceDirectedPlot::
initHierObjs() const
{
  CQPerfTrace trace("CQChartsForceDirectedPlot::initHierObjs");

  //---

  CQChartsConnectionPlot::initHierObjs();

  return true;
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
initHierObjsAddConnection(const QString &srcStr, const CQChartsModelIndex &srcLinkInd,
                          double srcTotal,
                          const QString &destStr, const CQChartsModelIndex &destLinkInd,
                          double destTotal, int depth) const
{
  assert(destTotal > 0.0);

  auto srcId  = getStringId(srcStr);
  auto destId = getStringId(destStr);

  assert(srcId != destId);

  auto &srcConnectionsData  = const_cast<ConnectionsData &>(getConnections(srcId));
  auto &destConnectionsData = const_cast<ConnectionsData &>(getConnections(destId));

  //---

  if (srcLinkInd.isValid()) {
    QModelIndex srcLinkIndex  = modelIndex(srcLinkInd);
    QModelIndex srcLinkIndex1 = normalizeIndex(srcLinkIndex);

    //---

    srcConnectionsData.ind   = srcLinkIndex1;
    srcConnectionsData.name  = srcStr;
    srcConnectionsData.group = depth;
    srcConnectionsData.total = srcTotal;

    //---

    if (destLinkInd.isValid()) {
      CQChartsConnectionList::Connection connection;

      connection.node  = srcId;
      connection.value = destTotal;

      destConnectionsData.connections.push_back(connection);
    }
  }

  //---

  if (destLinkInd.isValid()) {
    QModelIndex destLinkIndex  = modelIndex(destLinkInd);
    QModelIndex destLinkIndex1 = normalizeIndex(destLinkIndex);

    //---

    destConnectionsData.ind   = destLinkIndex1;
    destConnectionsData.name  = destStr;
    destConnectionsData.group = depth + 1;
    destConnectionsData.total = destTotal;

    //---

    if (srcLinkInd.isValid()) {
      CQChartsConnectionList::Connection connection;

      connection.node  = destId;
      connection.value = destTotal;

      srcConnectionsData.connections.push_back(connection);
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
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // Get group value
      int group = data.row;

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

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

      CQChartsConnectionList::Connection connection;

      connection.node  = destId;
      connection.value = value;

      srcConnectionsData.connections.push_back(connection);

      (void) plot_->getConnections(destId);
    }

   private:
    State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsForceDirectedPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsForceDirectedPlot* plot_      { nullptr };
    QChar                            separator_ { '/' };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  return true;
}

bool
CQChartsForceDirectedPlot::
getNameConnections(int group, const ModelVisitor::VisitData &data, int &srcId, int &destId,
                   double &value, const QChar &separator) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  //---

  // Get link value
  CQChartsModelIndex linkModelInd(data.row, linkColumn(), data.parent);

  CQChartsNamePair namePair;

  if (linkColumnType() == ColumnType::NAME_PAIR) {
    bool ok;
    QVariant linkVar = modelValue(linkModelInd, ok);
    if (! ok) return th->addDataError(linkModelInd, "Invalid Link");

    namePair = linkVar.value<CQChartsNamePair>();
  }
  else {
    bool ok;
    QString linkStr = modelString(linkModelInd, ok);
    if (! ok) return th->addDataError(linkModelInd, "Invalid Link");

    namePair = CQChartsNamePair(linkStr, separator);
  }

  if (! namePair.isValid())
    return th->addDataError(linkModelInd, "Invalid Link");

  //---

  // Get value value
  CQChartsModelIndex valueModelInd(data.row, valueColumn(), data.parent);

  bool ok1;
  value = modelReal(valueModelInd, ok1);
  if (! ok1) return th->addDataError(valueModelInd, "Invalid value");

  //---

  QString srcStr  = namePair.name1();
  QString destStr = namePair.name2();

  srcId  = getStringId(srcStr);
  destId = getStringId(destStr);

  //---

  // return connections data
  auto &connectionsData = const_cast<ConnectionsData &>(getConnections(srcId));

  QModelIndex nameInd  = modelIndex(linkModelInd);
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  connectionsData.ind   = nameInd1;
  connectionsData.name  = srcStr;
  connectionsData.group = group;
  connectionsData.total = value;

  return true;
}

bool
CQChartsForceDirectedPlot::
getRowConnections(int group, const ModelVisitor::VisitData &data) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  // get optional node id (default to row)
  CQChartsModelIndex nodeModelInd;

  int id = data.row;

  if (nodeColumn().isValid()) {
    nodeModelInd = CQChartsModelIndex(data.row, nodeColumn(), data.parent);

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
  CQChartsModelIndex connectionsModelInd(data.row, connectionsColumn(), data.parent);

  if (connectionsColumnType() == ColumnType::CONNECTION_LIST) {
    bool ok3;
    QVariant connectionsVar = modelValue(connectionsModelInd, ok3);

    connectionsData.connections = connectionsVar.value<CQChartsConnectionList>().connections();
  }
  else {
    bool ok3;
    QString connectionsStr = modelString(connectionsModelInd, ok3);
    if (! ok3) return false;

    CQChartsConnectionList::stringToConnections(connectionsStr, connectionsData.connections);
  }

  //---

  // get optional name
  QString name = QString("%1").arg(id);

  if (nameColumn().isValid()) {
    CQChartsModelIndex nameModelInd(data.row, nameColumn(), data.parent);

    bool ok4;
    name = modelString(nameModelInd, ok4);
    if (! ok4) return th->addDataError(nameModelInd, "Invalid name string");
  }

  connectionsData.name = name;

  //---

  // set total
  double total = 0.0;

  for (const auto &connection : connectionsData.connections)
    total += connection.value;

  connectionsData.total = total;

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

    connectionsData.ind   = tableConnectionData.linkInd();
    connectionsData.name  = tableConnectionData.name();
    connectionsData.group = tableConnectionData.group().i;

    // add connections
    for (const auto &value : tableConnectionData.values()) {
      CQChartsConnectionList::Connection connection;

      connection.node  = value.to;
      connection.value = value.toValue;

      connectionsData.connections.push_back(connection);
    }
  }

  return true;
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
  startAnimateTimer();
}

void
CQChartsForceDirectedPlot::
animateStep()
{
  if (pressed_ || ! isRunning())
    return;

  forceDirected_->step(stepSize_);

  if (isAutoFit()) {
    double xmin { 0.0 }, ymin { 0.0 }, xmax { 0.0 }, ymax { 0.0 };

    double r = std::max(nodeRadius(), 0.0);

    double xm = pixelToWindowWidth (2*r);
    double ym = pixelToWindowHeight(2*r);

    forceDirected_->calcRange(xmin, ymin, xmax, ymax);

    dataRange_.updateRange(xmin - xm, ymin - ym);
    dataRange_.updateRange(xmax + xm, ymax + ym);

    applyDataRange();
  }

  drawObjs();
}

bool
CQChartsForceDirectedPlot::
selectPress(const CQChartsGeom::Point &p, SelMod /*selMod*/)
{
  Springy::NodePoint nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

  forceDirected_->setCurrentNode (nodePoint.first );
  forceDirected_->setCurrentPoint(nodePoint.second);

  pressed_ = true;

  drawObjs();

  return true;
}

bool
CQChartsForceDirectedPlot::
selectMove(const CQChartsGeom::Point &p, bool first)
{
  if (pressed_) {
    if (forceDirected_->currentPoint())
      forceDirected_->currentPoint()->setP(Springy::Vector(p.x, p.y));

    drawObjs();

    return true;
  }
  else {
    Springy::NodePoint nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

    forceDirected_->setCurrentNode (nodePoint.first );
    forceDirected_->setCurrentPoint(nodePoint.second);
  }

  return CQChartsPlot::selectMove(p, first);
}

bool
CQChartsForceDirectedPlot::
selectRelease(const CQChartsGeom::Point &p)
{
  if (forceDirected_->currentPoint())
    forceDirected_->currentPoint()->setP(Springy::Vector(p.x, p.y));

  forceDirected_->setCurrentNode (0);
  forceDirected_->setCurrentPoint(0);

  pressed_ = false;

  drawObjs();

  return true;
}

void
CQChartsForceDirectedPlot::
keyPress(int key, int modifier)
{
  if (key == Qt::Key_S)
    setRunning(! isRunning());
  else
    CQChartsPlot::keyPress(key, modifier);
}

bool
CQChartsForceDirectedPlot::
tipText(const CQChartsGeom::Point &p, QString &tip) const
{
  if (! isRunning()) {
    Springy::NodePoint nodePoint = forceDirected_->nearest(Springy::Vector(p.x, p.y));

    auto *node = nodePoint.first;
    if (! node) return false;

    CQChartsTableTip tableTip;

    auto pc = connectionNodes_.find(node->id());

    tableTip.addTableRow("Id", node->id());

    if (pc != connectionNodes_.end()) {
      auto &connectionsData = getConnections((*pc).second);

      tableTip.addTableRow("Label"      , connectionsData.name);
      tableTip.addTableRow("Group"      , connectionsData.group);
      tableTip.addTableRow("Total"      , connectionsData.total);
      tableTip.addTableRow("Connections", connectionsData.connections.size());
    }
    else
      tableTip.addTableRow("Label", node->label());

    tableTip.addTableRow("Value", node->value());

    tip = tableTip.str();

    return true;
  }

  return false;
}

void
CQChartsForceDirectedPlot::
draw(QPainter *painter)
{
  drawParts(painter);

  //---

  {
  LockMutex lock(this, "draw");

  UpdateState updateState = this->updateState();

  if (updateState == UpdateState::READY) {
    setGroupedUpdateState(UpdateState::DRAWN);
  }
  }
}

void
CQChartsForceDirectedPlot::
drawParts(QPainter *painter) const
{
  auto *th = const_cast<CQChartsForceDirectedPlot *>(this);

  CQChartsPlotPaintDevice device(th, painter);

  drawDeviceParts(&device);
}

void
CQChartsForceDirectedPlot::
drawDeviceParts(CQChartsPaintDevice *device) const
{
  device->save();

  setClipRect(device);

  // draw edges
  QPen edgePen;

  QColor edgeColor = this->interpEdgeLinesColor(ColorInd());

  setPen(edgePen, true, edgeColor, edgeLinesAlpha(), edgeLinesWidth(), edgeLinesDash());

  double maxLineWidth = 8.0;

  for (auto &edge : forceDirected_->edges()) {
    bool isTemp = false;

    auto spring = forceDirected_->spring(edge, isTemp);

    const Springy::Vector &p1 = spring->point1()->p();
    const Springy::Vector &p2 = spring->point2()->p();

    if (isEdgeLinesValueWidth()) {
      QPen edgePen1 = edgePen;

      double w = maxLineWidth*(widthScale_*edge->value());

      edgePen1.setWidthF(w);

      device->setPen(edgePen1);
    }
    else
      device->setPen(edgePen);

    device->drawLine(CQChartsGeom::Point(p1.x(), p1.y()),
                     CQChartsGeom::Point(p2.x(), p2.y()));

    if (isTemp)
      delete spring;
  }

  // draw nodes
  double r = std::max(nodeRadius(), 0.0);

  double xm = pixelToWindowWidth (2*r);
  double ym = pixelToWindowHeight(2*r);

  for (auto &node : forceDirected_->nodes()) {
    auto point = forceDirected_->point(node);

    const Springy::Vector &p1 = point->p();

    //---

    CQChartsPenBrush penBrush;

    QColor pc = interpNodeStrokeColor(ColorInd());

    QColor fc;

    if      (colorType() == ColorType::GROUP)
      fc = interpPaletteColor(ColorInd(node->group(), maxGroup_));
    else if (colorType() == ColorType::INDEX)
      fc = interpPaletteColor(ColorInd(node->id(), nodes_.size()));
    else
      fc = interpPaletteColor(ColorInd(node->value()), /*scale*/false);

    if (node == forceDirected_->currentNode())
      fc = insideColor(fc);

    setPenBrush(penBrush,
      CQChartsPenData  (true, pc, nodeStrokeAlpha(), nodeStrokeWidth(), nodeStrokeDash()),
      CQChartsBrushData(true, fc, nodeFillAlpha(), nodeFillPattern()));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    //---

    CQChartsGeom::BBox ebbox(p1.x() - xm/2.0, p1.y() - ym/2.0, p1.x() + xm/2, p1.y() + ym/2.0);

    device->drawEllipse(ebbox);
  }

  //---

  device->restore();
}
