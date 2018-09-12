#include <CQChartsForceDirectedPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>

#include <QPainter>
#include <QTimer>

CQChartsForceDirectedPlotType::
CQChartsForceDirectedPlotType()
{
}

void
CQChartsForceDirectedPlotType::
addParameters()
{
  // connections are list of node ids/counts
  startParameterGroup("Connection List");

  addColumnParameter("node", "Node", "nodeColumn").
    setTip("Node Id Column");

  addColumnParameter("connections", "Connections", "connectionsColumn").
    setTip("List of Connection Pairs (Ids from id column and connection count)");

  addColumnParameter("name", "Name", "nameColumn").
    setTip("Optional node name");

  endParameterGroup();

  //---

  // connections are id pairs and counts
  startParameterGroup("Name Pair/Count");

  addColumnParameter("namePair", "NamePair", "namePairColumn").
    setTip("Connected Name Pairs (<name1>/<name2>)");

  addColumnParameter("count", "Count", "countColumn").
    setTip("Connection Count");

  endParameterGroup();

  //---

  addColumnParameter("groupId", "GroupId", "groupIdColumn");

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsForceDirectedPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draw connected data using animated nodes connected by springs.</p>\n";
}

bool
CQChartsForceDirectedPlotType::
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  if      (parameter->name() == "connections") {
    if (columnDetails->type() == CQChartsPlot::ColumnType::CONNECTION_LIST)
      return true;

    return false;
  }
  else if (parameter->name() == "namePair") {
    if (columnDetails->type() == CQChartsPlot::ColumnType::NAME_PAIR)
      return true;

    return false;
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
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
 CQChartsPlot(view, view->charts()->plotType("adjacency"), model),
 CQChartsPlotNodeShapeData<CQChartsForceDirectedPlot>(this),
 CQChartsPlotEdgeLineData <CQChartsForceDirectedPlot>(this)
{
  setMargins(0, 0, 0, 0);

  setNodeBorderAlpha(0.5);

  //---

  startAnimateTimer();
}

CQChartsForceDirectedPlot::
~CQChartsForceDirectedPlot()
{
}

//---

void
CQChartsForceDirectedPlot::
setNodeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nodeColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsForceDirectedPlot::
setConnectionsColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsForceDirectedPlot::
setNamePairColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(namePairColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsForceDirectedPlot::
setCountColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(countColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsForceDirectedPlot::
setGroupIdColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupIdColumn_, c, [&]() { updateRangeAndObjs(); } );
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
  CQChartsUtil::testAndSet(nodeRadius_, r, [&]() { invalidateLayers(); } );
}

void
CQChartsForceDirectedPlot::
setEdgeLinesValueWidth(bool b)
{
  CQChartsUtil::testAndSet(edgeLinesValueWidth_, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsForceDirectedPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nodeColumn"       , "node"      );
  addProperty("columns", this, "connectionsColumn", "connection");
  addProperty("columns", this, "nameColumn"       , "name"      );

  addProperty("columns", this, "namePairColumn", "namePair");
  addProperty("columns", this, "countColumn"   , "count"   );

  addProperty("columns", this, "groupIdColumn", "groupId");

  addProperty("options", this, "running");

  addProperty("node", this, "nodeRadius", "radius");

  addLineProperties("node/stroke", "nodeBorder");
  addFillProperties("node/fill"  , "nodeFill"  );
  addLineProperties("edge/stroke", "edgeLines" );
}

void
CQChartsForceDirectedPlot::
calcRange()
{
  dataRange_.reset();

  //---

  connectionsColumnType_ = columnValueType(connectionsColumn());
  namePairColumnType_    = columnValueType(namePairColumn   ());

  //---

  // TODO: calculate good range size from data or auto scale/fit ?
  dataRange_.updateRange(-rangeSize_, -rangeSize_);
  dataRange_.updateRange( rangeSize_,  rangeSize_);
}

bool
CQChartsForceDirectedPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  if (! idConnections_.empty())
    return false;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsForceDirectedPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      bool ok1;

      int group = plot_->modelInteger(data.row, plot_->groupIdColumn(), data.parent, ok1);

      if (! ok1) group = data.row;

      //---

      if (plot_->connectionsColumn().isValid()) {
        ConnectionsData connectionsData;

        if (! plot_->getRowConnections(group, data, connectionsData))
          return State::SKIP;

        plot_->addConnections(connectionsData.node, connectionsData);
      }
      else {
        ConnectionsData connectionsData;
        int             destId, count;

        if (! plot_->getNameConnections(group, data, connectionsData, destId, count))
          return State::SKIP;

        ConnectionsData &srcConnectionsData = plot_->getConnections(connectionsData.node);

        srcConnectionsData = connectionsData;

        CQChartsConnectionList::Connection connection;

        connection.node  = destId;
        connection.count = count;

        (void) plot_->getConnections(connection.node);

        srcConnectionsData.connections.push_back(connection);
      }

      //---

      maxGroup_ = std::max(maxGroup_, group);

      return State::OK;
    }

    int maxGroup() const { return maxGroup_; }

   private:
    CQChartsForceDirectedPlot *plot_     { nullptr };
    int                        maxGroup_ { 0 };
  };

  nameNodeMap_.clear();

  RowVisitor visitor(this);

  visitModel(visitor);

  int maxGroup = visitor.maxGroup();

  //---

  for (const auto &idConnections : idConnections_) {
    int            id    = idConnections.first;
    const QString &name  = idConnections.second.name;
    int            group = idConnections.second.group;

    Springy::Node *node = forceDirected_.newNode();

    QString label = QString("%1:%2").arg(name).arg(group);

    node->setLabel(label);
    node->setMass (nodeMass_);
    node->setValue((1.0*group)/maxGroup);

    nodes_[id] = node;
  }

  //---

  for (const auto &idConnections : idConnections_) {
    int                    id          = idConnections.first;
    const ConnectionsData &connections = idConnections.second;

    Springy::Node *node = nodes_[id];

    for (const auto &connection : connections.connections) {
      Springy::Node *node1 = nodes_[connection.node];

      Springy::Edge *edge = forceDirected_.newEdge(node, node1);

      edge->setLength(1.0/connection.count);
      edge->setValue(connection.count);
    }
  }

  //---

  for (int i = 0; i < initSteps_; ++i)
    forceDirected_.step(stepSize_);

  //---

  return true;
}

bool
CQChartsForceDirectedPlot::
getRowConnections(int group, const ModelVisitor::VisitData &data, ConnectionsData &connectionsData)
{
  // get node
  bool ok2;

  int id = modelInteger(data.row, nodeColumn(), data.parent, ok2);

  if (! ok2) id = data.row;

  //---

  // get connections
  bool ok3;

  if (connectionsColumnType_ == ColumnType::CONNECTION_LIST) {
    QVariant connectionsVar = modelValue(data.row, connectionsColumn(), data.parent, ok3);

    connectionsData.connections = connectionsVar.value<CQChartsConnectionList>().connections();
  }
  else {
    QString connectionsStr = modelString(data.row, connectionsColumn(), data.parent, ok3);

    if (! ok3)
      return false;

    decodeConnections(connectionsStr, connectionsData.connections);
  }

  //---

  // get name
  bool ok4;

  QString name = modelString(data.row, nameColumn(), data.parent, ok4);

  if (! name.length())
    name = QString("%1").arg(id);

  //---

  // return connections data
  QModelIndex nodeInd  = modelIndex(data.row, nodeColumn(), data.parent);
  QModelIndex nodeInd1 = normalizeIndex(nodeInd);

  connectionsData.ind   = nodeInd1;
  connectionsData.node  = id;
  connectionsData.name  = name;
  connectionsData.group = group;

  return true;
}

bool
CQChartsForceDirectedPlot::
getNameConnections(int group, const ModelVisitor::VisitData &data,
                   ConnectionsData &connections, int &destId, int &count)
{
  bool ok2;

  CQChartsNamePair namePair;

  if (namePairColumnType_ == ColumnType::NAME_PAIR) {
    QVariant namePairVar = modelValue(data.row, namePairColumn(), data.parent, ok2);

    if (! ok2)
      return false;

    namePair = namePairVar.value<CQChartsNamePair>();
  }
  else {
    QString namePairStr = modelString(data.row, namePairColumn(), data.parent, ok2);

    if (! ok2)
      return false;

    namePair = CQChartsNamePair(namePairStr);
  }

  if (! namePair.isValid())
    return false;

  //---

  bool ok3;

  count = modelInteger(data.row, countColumn(), data.parent, ok3);

  if (! ok3)
    count = 0;

  //---

  QString srcStr  = namePair.name1();
  QString destStr = namePair.name2();

  int srcId  = getStringId(srcStr);
      destId = getStringId(destStr);

  //---

  QModelIndex nameInd  = modelIndex(data.row, namePairColumn(), data.parent);
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  connections.ind   = nameInd1;
  connections.node  = srcId;
  connections.name  = srcStr;
  connections.group = group;

  return true;
}

CQChartsForceDirectedPlot::ConnectionsData &
CQChartsForceDirectedPlot::
getConnections(int id)
{
  auto p = idConnections_.find(id);

  if (p == idConnections_.end()) {
    ConnectionsData data;

    data.node = id;

    p = idConnections_.insert(p, IdConnectionsData::value_type(id, data));
  }

  return (*p).second;
}

void
CQChartsForceDirectedPlot::
addConnections(int id, const ConnectionsData &connections)
{
  idConnections_[id] = connections;
}

bool
CQChartsForceDirectedPlot::
decodeConnections(const QString &str, Connections &connections)
{
  return CQChartsConnectionList::stringToConnections(str, connections);
}

int
CQChartsForceDirectedPlot::
getStringId(const QString &str)
{
  auto p = nameNodeMap_.find(str);

  if (p == nameNodeMap_.end()) {
    int id = nameNodeMap_.size();

    p = nameNodeMap_.insert(p, StringIndMap::value_type(str, id));
  }

  return (*p).second;
}

//---

void
CQChartsForceDirectedPlot::
animateStep()
{
  if (pressed_ || ! isRunning())
    return;

  forceDirected_.step(stepSize_);

  if (isAutoFit()) {
    double xmin { 0.0 }, ymin { 0.0 }, xmax { 0.0 }, ymax { 0.0 };

    double r = nodeRadius();

    double xm = pixelToWindowWidth (2*r);
    double ym = pixelToWindowHeight(2*r);

    forceDirected_.calcRange(xmin, ymin, xmax, ymax);

    dataRange_.updateRange(xmin - xm, ymin - ym);
    dataRange_.updateRange(xmax + xm, ymax + ym);

    applyDataRange();
  }

  invalidateLayers();
}

bool
CQChartsForceDirectedPlot::
selectPress(const CQChartsGeom::Point &p, SelMod /*selMod*/)
{
  Springy::NodePoint nodePoint = forceDirected_.nearest(Springy::Vector(p.x, p.y));

  forceDirected_.setCurrentNode (nodePoint.first );
  forceDirected_.setCurrentPoint(nodePoint.second);

  pressed_ = true;

  invalidateLayers();

  return true;
}

bool
CQChartsForceDirectedPlot::
selectMove(const CQChartsGeom::Point &p, bool first)
{
  if (pressed_) {
    if (forceDirected_.currentPoint())
      forceDirected_.currentPoint()->setP(Springy::Vector(p.x, p.y));

    invalidateLayers();

    return true;
  }
  else {
    Springy::NodePoint nodePoint = forceDirected_.nearest(Springy::Vector(p.x, p.y));

    forceDirected_.setCurrentNode (nodePoint.first );
    forceDirected_.setCurrentPoint(nodePoint.second);
  }

  return CQChartsPlot::selectMove(p, first);
}

bool
CQChartsForceDirectedPlot::
selectRelease(const CQChartsGeom::Point &p)
{
  if (forceDirected_.currentPoint())
    forceDirected_.currentPoint()->setP(Springy::Vector(p.x, p.y));

  forceDirected_.setCurrentNode (0);
  forceDirected_.setCurrentPoint(0);

  pressed_ = false;

  invalidateLayers();

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
    Springy::NodePoint nodePoint = forceDirected_.nearest(Springy::Vector(p.x, p.y));

    if (! nodePoint.first)
      return false;

    tip = nodePoint.first->label();

    return true;
  }

  return false;
}

void
CQChartsForceDirectedPlot::
drawParts(QPainter *painter)
{
  painter->save();

  setClipRect(painter);

  // draw edges
  QPen edgePen;

  QColor edgeColor = this->interpEdgeLinesColor(0, 1);

  setPen(edgePen, true, edgeColor, edgeLinesAlpha(), edgeLinesWidth(), edgeLinesDash());

  for (auto &edge : forceDirected_.edges()) {
    bool isTemp = false;

    auto spring = forceDirected_.spring(edge, isTemp);

    const Springy::Vector &p1 = spring->point1()->p();
    const Springy::Vector &p2 = spring->point2()->p();

    double px1, py1, px2, py2;

    windowToPixel(p1.x(), p1.y(), px1, py1);
    windowToPixel(p2.x(), p2.y(), px2, py2);

    if (isEdgeLinesValueWidth()) {
      QPen edgePen1 = edgePen;

      double w = sqrt(edge->value());

      edgePen1.setWidthF(w);

      painter->setPen(edgePen1);
    }
    else
      painter->setPen(edgePen);

    painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));

    if (isTemp)
      delete spring;
  }

  // draw nodes
  for (auto &node : forceDirected_.nodes()) {
    auto point = forceDirected_.point(node);

    const Springy::Vector &p1 = point->p();

    double px, py;

    windowToPixel(p1.x(), p1.y(), px, py);

    //---

    QPen   pen;
    QBrush brush;

    QColor pc = interpNodeBorderColor(0, 1);
    QColor fc = interpPaletteColor(node->value(), /*scale*/false);

    if (node == forceDirected_.currentNode())
      fc = insideColor(fc);

    setPen(pen, true, pc, nodeBorderAlpha(), nodeBorderWidth(), nodeBorderDash());

    setBrush(brush, true, fc, nodeFillAlpha(), nodeFillPattern());

    painter->setPen  (pen);
    painter->setBrush(brush);

    //---

    double r = nodeRadius();

    painter->drawEllipse(QRectF(px - r, py - r, 2*r, 2*r));
  }

  //---

  painter->restore();
}
