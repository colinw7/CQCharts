#include <CQChartsForceDirectedPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRotatedText.h>
#include <CQStrParse.h>

#include <QAbstractItemModel>
#include <QPainter>
#include <QTimer>

CQChartsForceDirectedPlotType::
CQChartsForceDirectedPlotType()
{
  addParameters();
}

void
CQChartsForceDirectedPlotType::
addParameters()
{
  addColumnParameter("node"       , "Node"       , "nodeColumn"       , "", 0);
  addColumnParameter("connections", "Connections", "connectionsColumn", "", 1);

  addColumnParameter("group", "Group", "groupColumn", "optional");
  addColumnParameter("name" , "Name" , "nameColumn" , "optional");
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
  CQChartsPaletteColor themeFg(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setMargins(0, 0, 0, 0);

  nodeBorderColor_ = themeFg;
  edgeColor_       = themeFg;

  //---

  timer_ = new QTimer;

  connect(timer_, SIGNAL(timeout()), this, SLOT(animateSlot()));

  timer_->start(tickLen_);
}

void
CQChartsForceDirectedPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nodeColumn"       , "node"       );
  addProperty("columns", this, "connectionsColumn", "connection" );
  addProperty("columns", this, "groupColumn"      , "group"      );
  addProperty("columns", this, "nameColumn"       , "name"       );
  addProperty(""       , this, "autoFit"                         );
  addProperty(""       , this, "running"                         );
  addProperty("node"   , this, "nodeRadius"       , "radius"     );
  addProperty("node"   , this, "nodeBorderColor"  , "borderColor");
  addProperty("edge"   , this, "edgeColor"        , "color"      );
  addProperty("edge"   , this, "edgeAlpha"        , "alpha"      );
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

void
CQChartsForceDirectedPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  if (! idConnections_.empty())
    return;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int maxGroup = 0;

  int nr = model->rowCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    QModelIndex nodeInd  = model->index(r, nodeColumn ());
    QModelIndex groupInd = model->index(r, groupColumn());

    //---

    bool ok1, ok2;

    int id    = CQChartsUtil::modelInteger(model, nodeInd , ok1);
    int group = CQChartsUtil::modelInteger(model, groupInd, ok2);

    if (! ok1) id    = r;
    if (! ok2) group = r;

    //---

    QModelIndex connectionsInd = model->index(r, connectionsColumn());

    bool ok3;

    QString connectionsStr = CQChartsUtil::modelString(model, connectionsInd, ok3);

    //---

    QModelIndex nameInd = model->index(r, nameColumn());

    bool ok4;

    QString name = CQChartsUtil::modelString(model, nameInd, ok4);

    if (! name.length())
      name = QString("%1").arg(id);

    //---

    ConnectionsData connections;

    connections.node  = id;
    connections.name  = name;
    connections.group = group;
    connections.ind   = normalizeIndex(nodeInd);

    decodeConnections(connectionsStr, connections.connections);

    idConnections_[id] = connections;

    //---

    maxGroup = std::max(maxGroup, group);
  }

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

  initObjTree();
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

    if (! parse.readBracedString(str1, /*includeBraces*/false))
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
animateSlot()
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

  update();
}

bool
CQChartsForceDirectedPlot::
mousePress(const CQChartsGeom::Point &p)
{
  Springy::NodePoint nodePoint = forceDirected_.nearest(Springy::Vector(p.x, p.y));

  forceDirected_.setCurrentNode (nodePoint.first );
  forceDirected_.setCurrentPoint(nodePoint.second);

  pressed_ = true;

  update();

  return true;
}

bool
CQChartsForceDirectedPlot::
mouseMove(const CQChartsGeom::Point &p, bool first)
{
  if (pressed_) {
    if (forceDirected_.currentPoint())
      forceDirected_.currentPoint()->setP(Springy::Vector(p.x, p.y));

    update();

    return true;
  }
  else {
    Springy::NodePoint nodePoint = forceDirected_.nearest(Springy::Vector(p.x, p.y));

    forceDirected_.setCurrentNode (nodePoint.first );
    forceDirected_.setCurrentPoint(nodePoint.second);
  }

  return CQChartsPlot::mouseMove(p, first);
}

void
CQChartsForceDirectedPlot::
mouseRelease(const CQChartsGeom::Point &p)
{
  if (forceDirected_.currentPoint())
    forceDirected_.currentPoint()->setP(Springy::Vector(p.x, p.y));

  forceDirected_.setCurrentNode (0);
  forceDirected_.setCurrentPoint(0);

  pressed_ = false;

  update();
}

void
CQChartsForceDirectedPlot::
keyPress(int key)
{
  if (key == Qt::Key_S)
    setRunning(! isRunning());
  else
    CQChartsPlot::keyPress(key);
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
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  // draw edges
  QColor edgeColor = this->interpEdgeColor(0, 1);

  edgeColor.setAlphaF(edgeAlpha());

  for (auto edge : forceDirected_.edges()) {
    auto spring = forceDirected_.spring(edge);

    const Springy::Vector &p1 = spring->point1()->p();
    const Springy::Vector &p2 = spring->point2()->p();

    double px1, py1, px2, py2;

    windowToPixel(p1.x(), p1.y(), px1, py1);
    windowToPixel(p2.x(), p2.y(), px2, py2);

    double w = sqrt(edge->value());

    p->setPen(QPen(edgeColor, w));

    p->drawLine(px1, py1, px2, py2);
  }

  // draw nodes
  for (auto node : forceDirected_.nodes()) {
    auto point = forceDirected_.point(node);

    const Springy::Vector &p1 = point->p();

    double px, py;

    windowToPixel(p1.x(), p1.y(), px, py);

    p->setPen(interpNodeBorderColor(0, 1));

    QColor c = interpPaletteColor(node->value(), /*scale*/false);

    if (node == forceDirected_.currentNode()) {
      p->setBrush(insideColor(c));
    }
    else {
      p->setBrush(c);
    }

    double r = nodeRadius();

    p->drawEllipse(QRectF(px - r, py - r, 2*r, 2*r));
  }
}
