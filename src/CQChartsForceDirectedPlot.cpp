#include <CQChartsForceDirectedPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQStrParse.h>

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
  startParameterGroup("Force Directed");

  addColumnParameter("node"       , "Node"       , "nodeColumn"       );
  addColumnParameter("connections", "Connections", "connectionsColumn");
  addColumnParameter("value"      , "Value"      , "valueColumn"      );
  addColumnParameter("group"      , "Group"      , "groupColumn"      );
  addColumnParameter("name"       , "Name"       , "nameColumn"       );

  endParameterGroup();

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

CQChartsPlot *
CQChartsForceDirectedPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsForceDirectedPlot(view, model);
}

//---

CQChartsForceDirectedPlot::
CQChartsForceDirectedPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("adjacency"), model)
{
  setMargins(0, 0, 0, 0);

  edgeStroke_.alpha = 0.5;

  //---

  startAnimateTimer();
}

CQChartsForceDirectedPlot::
~CQChartsForceDirectedPlot()
{
}

void
CQChartsForceDirectedPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nodeColumn"       , "node"      );
  addProperty("columns", this, "connectionsColumn", "connection");
  addProperty("columns", this, "valueColumn"      , "value"     );
  addProperty("columns", this, "groupColumn"      , "group"     );
  addProperty("columns", this, "nameColumn"       , "name"      );

  addProperty("options", this, "running");

  addProperty("node", this, "nodeRadius"     , "radius"     );
  addProperty("node", this, "nodeBorderColor", "borderColor");

  addProperty("edge", this, "edgeColor", "color");
  addProperty("edge", this, "edgeAlpha", "alpha");
}

void
CQChartsForceDirectedPlot::
updateRange(bool apply)
{
  // TODO: calculate good range size from data or auto fit ?
  dataRange_.reset();

  dataRange_.updateRange(-rangeSize_, -rangeSize_);
  dataRange_.updateRange( rangeSize_,  rangeSize_);

  if (apply)
    applyDataRange();
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

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      bool ok1;

      int group = plot_->modelInteger(row, plot_->groupColumn(), parent, ok1);

      if (! ok1) group = row;

      //---

      if (plot_->connectionsColumn().isValid()) {
        //---

        bool ok2;

        int id = plot_->modelInteger(row, plot_->nodeColumn(), parent, ok2);

        if (! ok2) id = row;

        //---

        bool ok3;

        QString connectionsStr = plot_->modelString(row, plot_->connectionsColumn(), parent, ok3);

        ConnectionDataArray connectionDataArray;

        plot_->decodeConnections(connectionsStr, connectionDataArray);

        //---

        bool ok4;

        QString name = plot_->modelString(row, plot_->nameColumn(), parent, ok4);

        if (! name.length())
          name = QString("%1").arg(id);

        //---

        QModelIndex nodeInd  = plot_->modelIndex(row, plot_->nodeColumn(), parent);
        QModelIndex nodeInd1 = plot_->normalizeIndex(nodeInd);

        ConnectionsData connectionsData;

        connectionsData.ind         = nodeInd1;
        connectionsData.node        = id;
        connectionsData.name        = name;
        connectionsData.group       = group;
        connectionsData.connections = connectionDataArray;

        plot_->addConnections(id, connectionsData);
      }
      else {
        bool ok2;

        QString linkStr = plot_->modelString(row, plot_->nameColumn(), parent, ok2);

        if (! ok2)
          return State::SKIP;

        int pos = linkStr.indexOf("/");

        if (pos == -1)
          return State::SKIP;

        QString srcStr  = linkStr.mid(0, pos ).simplified();
        QString destStr = linkStr.mid(pos + 1).simplified();

        int srcId  = getStringId(srcStr);
        int destId = getStringId(destStr);

        //---

        bool ok3;

        int value = plot_->modelInteger(row, plot_->valueColumn(), parent, ok3);

        if (! ok3)
          value = 0;

        //---

        QModelIndex nameInd  = plot_->modelIndex(row, plot_->nameColumn(), parent);
        QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

        ConnectionsData &srcConnectionsData = plot_->getConnections(srcId);

        (void) plot_->getConnections(destId);

        srcConnectionsData.ind   = nameInd1;
        srcConnectionsData.node  = srcId;
        srcConnectionsData.name  = srcStr;
        srcConnectionsData.group = group;

        ConnectionData connectionData;

        connectionData.node  = destId;
        connectionData.count = value;

        srcConnectionsData.connections.push_back(connectionData);
      }

      //---

      maxGroup_ = std::max(maxGroup_, group);

      return State::OK;
    }

    int maxGroup() const { return maxGroup_; }

   private:
    int getStringId(const QString &str) {
      auto p = stringIndMap_.find(str);

      if (p == stringIndMap_.end()) {
        int id = stringIndMap_.size();

        p = stringIndMap_.insert(p, StringIndMap::value_type(str, id));
      }

      return (*p).second;
    }

   private:
    using StringIndMap = std::map<QString,int>;

    CQChartsForceDirectedPlot *plot_     { nullptr };
    int                        maxGroup_ { 0 };
    StringIndMap               stringIndMap_;
  };

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
decodeConnections(const QString &str, ConnectionDataArray &connections)
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    QString str1;

    if (! parse.readBracedString(str1))
      return false;

    ConnectionData connection;

    if (! decodeConnection(str1, connection))
      return false;

    connections.push_back(connection);

    parse.skipSpace();
  }

  if (parse.isChar('}'))
    parse.skipChar();

  return true;
}

bool
CQChartsForceDirectedPlot::
decodeConnection(const QString &str, ConnectionData &connection)
{
  CQStrParse parse(str);

  parse.skipSpace();

  QString str1;

  if (! parse.readNonSpace(str1))
    return false;

  parse.skipSpace();

  QString str2;

  if (! parse.readNonSpace(str2))
    return false;

  long node;

  if (! CQChartsUtil::toInt(str1, node))
    return false;

  long count;

  if (! CQChartsUtil::toInt(str2, count))
    return false;

  connection = ConnectionData(node, count);

  return true;
}

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
selectPress(const CQChartsGeom::Point &p, ModSelect /*modSelect*/)
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
std::cerr << p.x << " " << p.y << "\n";
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
  // draw edges
  QPen edgePen;

  QColor edgeColor = this->interpEdgeColor(0, 1);

  edgeColor.setAlphaF(edgeAlpha());

  edgePen.setColor(edgeColor);

  for (auto &edge : forceDirected_.edges()) {
    bool isTemp = false;

    auto spring = forceDirected_.spring(edge, isTemp);

    const Springy::Vector &p1 = spring->point1()->p();
    const Springy::Vector &p2 = spring->point2()->p();

    double px1, py1, px2, py2;

    windowToPixel(p1.x(), p1.y(), px1, py1);
    windowToPixel(p2.x(), p2.y(), px2, py2);

    double w = sqrt(edge->value());

    edgePen.setWidthF(w);

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

    painter->setPen(interpNodeBorderColor(0, 1));

    QColor c = interpPaletteColor(node->value(), /*scale*/false);

    if (node == forceDirected_.currentNode()) {
      painter->setBrush(insideColor(c));
    }
    else {
      painter->setBrush(c);
    }

    double r = nodeRadius();

    painter->drawEllipse(QRectF(px - r, py - r, 2*r, 2*r));
  }
}
