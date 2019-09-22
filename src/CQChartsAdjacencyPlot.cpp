#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsNamePair.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

CQChartsAdjacencyPlotType::
CQChartsAdjacencyPlotType()
{
}

void
CQChartsAdjacencyPlotType::
addParameters()
{
  // connections are list of node ids/counts
  startParameterGroup("Connection List");

  addColumnParameter("node", "Node", "nodeColumn").
    setNumeric().setTip("Node Id Column");

  addColumnParameter("connections", "Connections", "connectionsColumn").
   setTip("List of Connection Pairs (Ids from id column and connection count)").setDiscriminator();

  endParameterGroup();

  //---

  // connections are id pairs and counts
  startParameterGroup("Name Pair/Count");

  addColumnParameter("namePair", "Name Pair", "namePairColumn").
   setTip("Connected Name Pairs (<name1>/<name2>)").setDiscriminator();

  addColumnParameter("count", "Count", "countColumn").
   setNumeric().setTip("Connection Count");

  endParameterGroup();

  //---

  startParameterGroup("General");

  addColumnParameter("name", "Name", "nameColumn").
   setString().setTip("Name For Node Id");

  addColumnParameter("groupId", "Group Id", "groupIdColumn").
   setNumeric().setTip("Group Id for Color");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsAdjacencyPlotType::
description() const
{
  auto B    = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto PARM = [](const QString &str) { return CQChartsHtml::Str::angled(str); };
  auto LI   = [](const QString &str) { return CQChartsHtml::Str(str); };
//auto BR   = []() { return CQChartsHtml::Str(CQChartsHtml::Str::Type::BR); };
  auto IMG  = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Adjacency Plot").
    h3("Summary").
     p("Draws connectivity information between two different sets of data as a "
       "matrix where the color of the cells represents the group and connectivity.").
    h3("Columns").
     p("Connection information can be supplied using:").
     ul({ LI("A list of connections in the " + B("Connections") + " column with the "
             "associated node numbers in the " + B("Node") + " column."),
          LI("A name pair using " + B("NamePair") + " column and a count using the " +
             B("Count") + " column.") }).
     p("The connections column is in the form {{" + PARM("id") + " " + PARM("count") + "} ...}.").
     p("The name pair column is in the form " + PARM("id1") + "/" + PARM("id2")).
     p("The column id is taken from the " + B("Id") + " column and an optional "
       "name for the id can be supplied in the " + B("Name") + " column.").
     p("The group is specified using the " + B("Group") + " column.").
    h3("Options").
     p("The nodes can be sorted by group, name or count using the " + B("sortType") + " option").
     p("The maring around the plot can be specified using the " + B("bgMargin") + " option").
    h3("Styling").
     p("The styling (fill, stroke) of the connection cells, empty (no connection) cell "
       "and background can be set").
    h3("Limitations").
     p("The plot does not support axes, key or logarithmic scales").
    h3("Example").
     p(IMG("images/adjacency.png"));
}

bool
CQChartsAdjacencyPlotType::
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
CQChartsAdjacencyPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsAdjacencyPlot(view, model);
}

//---

CQChartsAdjacencyPlot::
CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("adjacency"), model),
 CQChartsObjBackgroundFillData<CQChartsAdjacencyPlot>(this),
 CQChartsObjShapeData         <CQChartsAdjacencyPlot>(this),
 CQChartsObjTextData          <CQChartsAdjacencyPlot>(this),
 CQChartsObjEmptyCellShapeData<CQChartsAdjacencyPlot>(this)
{
  NoUpdate noUpdate(this);

  setBackgroundFillColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.2));

  setStrokeColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1.0));
  setStrokeAlpha(0.5);

  setEmptyCellFillColor  (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.1));
  setEmptyCellStrokeColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.2));

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setOuterMargin(0, 0, 0, 0);

  addTitle();
}

CQChartsAdjacencyPlot::
~CQChartsAdjacencyPlot()
{
  clearNodes();
}

void
CQChartsAdjacencyPlot::
clearNodes()
{
  for (auto &pnode : nodes_)
    delete pnode.second;

  nodes_.clear();

  nameNodeMap_.clear();

  sortedNodes_.clear();
}

//---

void
CQChartsAdjacencyPlot::
setNodeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nodeColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setConnectionsColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setNamePairColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(namePairColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setCountColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(countColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setGroupIdColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupIdColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
setSortType(const SortType &t)
{
  CQChartsUtil::testAndSet(sortType_, t, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
setBgMargin(const CQChartsLength &l)
{
  bgMargin_ = l;

  updateObjs();
}

//---

void
CQChartsAdjacencyPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  // columns
  addProp("columns", "nodeColumn"       , "node"       , "Node column");
  addProp("columns", "connectionsColumn", "connections", "Connections column");
  addProp("columns", "nameColumn"       , "name"       , "Node name column");

  addProp("columns", "namePairColumn", "namePair", "Name/Value column");
  addProp("columns", "countColumn"   , "count"   , "Count column");

  addProp("columns", "groupIdColumn", "groupId", "Grouping column");

  // options
  addProp("options", "sortType", "sort"  , "Sort type");
  addProp("options", "bgMargin", "margin", "Background margin");

  // background
  addFillProperties("background/fill", "backgroundFill", "Background");

  // cell style
  addFillProperties("cell/fill"  , "fill"  , "Cell");
  addLineProperties("cell/stroke", "stroke", "Cell");

  addProp("cell/stroke", "cornerSize", "cornerSize", "Cell box corner size");

  // empty cell style
  addFillProperties("emptyCell/fill"  , "emptyCellFill"  , "Empty cell");
  addLineProperties("emptyCell/stroke", "emptyCellStroke", "Empty cell");

  addProp("emptyCell/stroke", "cornerSize", "cornerSize", "Empty cell box corner size");

  //---

  addTextProperties("text", "text", "");
}

CQChartsGeom::Range
CQChartsAdjacencyPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::calcRange");

  CQChartsGeom::Range dataRange;

  double r = 1.0;

  //---

  dataRange.updateRange(0, 0);
  dataRange.updateRange(r, r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

bool
CQChartsAdjacencyPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->clearNodes();

  //---

  th->setInsideObj(nullptr);

  th->connectionsColumnType_ = columnValueType(connectionsColumn());
  th->namePairColumnType_    = columnValueType(namePairColumn   ());

  //---

  if      (namePairColumn().isValid() && countColumn().isValid())
    return initHierObjs(objs);
  else if (connectionsColumn().isValid())
    return initConnectionObjs(objs);
  else
    return false;
}

bool
CQChartsAdjacencyPlot::
initHierObjs(PlotObjs &objs) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int group = data.row;

      if (plot_->groupIdColumn().isValid()) {
        bool ok1;

        group = plot_->modelInteger(data.row, plot_->groupIdColumn(), data.parent, ok1);

        if (! ok1)
          group = data.row;
      }

      //---

      bool ok2;

      CQChartsNamePair namePair;

      if (plot_->namePairColumnType() == ColumnType::NAME_PAIR) {
        QVariant namePairVar =
          plot_->modelValue(data.row, plot_->namePairColumn(), data.parent, ok2);

        if (! ok2)
          return State::SKIP;

        namePair = namePairVar.value<CQChartsNamePair>();
      }
      else {
        QString namePairStr =
          plot_->modelString(data.row, plot_->namePairColumn(), data.parent, ok2);

        if (! ok2)
          return State::SKIP;

        namePair = CQChartsNamePair(namePairStr);
      }

      if (! namePair.isValid())
        return State::SKIP;

      //---

      bool ok3;

      double count = plot_->modelReal(data.row, plot_->countColumn(), data.parent, ok3);

      if (! ok3)
        return State::SKIP;

      //---

      QString srcStr  = namePair.name1();
      QString destStr = namePair.name2();

      CQChartsAdjacencyNode *srcNode  = plot_->getNodeByName(srcStr );
      CQChartsAdjacencyNode *destNode = plot_->getNodeByName(destStr);

      srcNode->addNode(destNode, count);

      //---

      QModelIndex nameInd  = plot_->modelIndex(data.row, plot_->nameColumn(), data.parent);
      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      srcNode->setGroup(group);
      srcNode->setInd  (nameInd1);

      return State::OK;
    }

   private:
    const CQChartsAdjacencyPlot *plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

  for (const auto &nameNode : nameNodeMap_) {
    CQChartsAdjacencyNode *node = nameNode.second;

    th->nodes_[node->id()] = node;
  }

  //---

  sortNodes(nodes_, th->sortedNodes_, th->nodeData_);

  //---

  double xb = lengthPlotWidth (bgMargin());
  double yb = lengthPlotHeight(bgMargin());

  th->nodeData_.maxLen = 0;

  for (auto &node1 : sortedNodes_) {
    th->nodeData_.maxLen = std::max(th->nodeData_.maxLen, int(node1->name().size()));
  }

  //---

  int nn = numNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CMathUtil::isZero(value)) {
        CQChartsGeom::BBox bbox(x, y - scale(), x + scale(), y);

        ColorInd ig(node1->group(), maxGroup() + 1);

        CQChartsAdjacencyObj *obj =
          new CQChartsAdjacencyObj(this, node1, node2, value, bbox, ig);

        objs.push_back(obj);
      }

      x += scale();
    }

    y -= scale();
  }

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
initConnectionObjs(PlotObjs &objs) const
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ConnectionsData connections;

      if (! plot_->getRowConnections(data, connections))
        return State::SKIP;

      idConnections_[connections.node] = connections;

      return State::OK;
    }

    const IdConnectionsData &idConnections() const { return idConnections_; }

   private:
    const CQChartsAdjacencyPlot* plot_ { nullptr };
    IdConnectionsData            idConnections_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

  const IdConnectionsData &idConnectionsData = visitor.idConnections();

  //---

  for (const auto &idConnections : idConnectionsData) {
    int                id    = idConnections.first;
    const QModelIndex& ind   = idConnections.second.ind;
    const QString&     name  = idConnections.second.name;
    int                group = idConnections.second.group;

    CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, name, group, ind);

    th->nodes_[id] = node;
  }

  //---

  for (const auto &idConnections : idConnectionsData) {
    int                    id          = idConnections.first;
    const ConnectionsData &connections = idConnections.second;

    CQChartsAdjacencyNode *node = th->nodes_[id];

    for (const auto &connection : connections.connections) {
      CQChartsAdjacencyNode *node1 = th->nodes_[connection.node];

      node->addNode(node1, connection.count);
    }
  }

  //---

  sortNodes(nodes_, th->sortedNodes_, th->nodeData_);

  //---

  double xb = lengthPlotWidth (bgMargin());
  double yb = lengthPlotHeight(bgMargin());

  th->nodeData_.maxLen = 0;

  for (auto &node1 : sortedNodes_) {
    th->nodeData_.maxLen = std::max(th->nodeData_.maxLen, int(node1->name().size()));
  }

  //---

  int nn = numNodes();

  if (nn + maxLen()*factor_ > 0)
    th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);
  else
    th->nodeData_.scale = 1.0;

  double tsize = maxLen()*factor_*scale();

  //---

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CMathUtil::isZero(value)) {
        CQChartsGeom::BBox bbox(x, y - scale(), x + scale(), y);

        ColorInd ig(node1->group(), maxGroup() + 1);

        CQChartsAdjacencyObj *obj =
          new CQChartsAdjacencyObj(this, node1, node2, value, bbox, ig);

        objs.push_back(obj);
      }

      x += scale();
    }

    y -= scale();
  }

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connections) const
{
  // get optional group id
  bool ok2;

  int group = modelInteger(data.row, groupIdColumn(), data.parent, ok2);

  if (! ok2) group = data.row;

  //---

  // get optional id
  bool ok1;

  int id = modelInteger(data.row, nodeColumn(), data.parent, ok1);

  if (! ok1) id = data.row;

  //---

  // get connections
  bool ok3;

  if (connectionsColumnType_ == ColumnType::CONNECTION_LIST) {
    QVariant connectionsVar = modelValue(data.row, connectionsColumn(), data.parent, ok3);

    connections.connections = connectionsVar.value<CQChartsConnectionList>().connections();
  }
  else {
    QString connectionsStr = modelString(data.row, connectionsColumn(), data.parent, ok3);

    if (! ok3)
      return false;

    decodeConnections(connectionsStr, connections.connections);
  }

  //----

  // get optional name
  bool ok4;

  QString name = modelString(data.row, nameColumn(), data.parent, ok4);

  if (! name.length())
    name = QString("%1").arg(id);

  //---

  // return connections data
  QModelIndex nodeInd  = modelIndex(data.row, nodeColumn(), data.parent);
  QModelIndex nodeInd1 = normalizeIndex(nodeInd);

  connections.ind   = nodeInd1;
  connections.node  = id;
  connections.name  = name;
  connections.group = group;

  return true;
}

void
CQChartsAdjacencyPlot::
sortNodes(const NodeMap &nodes, NodeArray &sortedNodes, NodeData &nodeData) const
{
  assert(sortedNodes.empty());

  nodeData.maxValue = 0;
  nodeData.maxGroup = 0;
  nodeData.maxNode  = 0;

  for (auto &pnode : nodes) {
    CQChartsAdjacencyNode *node = const_cast<CQChartsAdjacencyNode *>(pnode.second);

    sortedNodes.push_back(node);

    nodeData.maxValue = std::max(nodeData.maxValue, node->maxCount());
    nodeData.maxGroup = std::max(nodeData.maxGroup, node->group());
    nodeData.maxNode  = std::max(nodeData.maxNode , node->id());
  }

  if      (sortType() == SortType::NAME) {
    std::sort(sortedNodes.begin(), sortedNodes.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        return lhs->name() < rhs->name();
      });
  }
  else if (sortType() == SortType::GROUP) {
    std::sort(sortedNodes.begin(), sortedNodes.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        if (lhs->group() != rhs->group())
          return lhs->group() < rhs->group();

        return lhs->name() < rhs->name();
      });
  }
  else if (sortType() == SortType::COUNT) {
    std::sort(sortedNodes.begin(), sortedNodes.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        if (lhs->count() != rhs->count())
          return lhs->count() < rhs->count();

        return lhs->name() < rhs->name();
      });
  }
}

bool
CQChartsAdjacencyPlot::
decodeConnections(const QString &str, Connections &connections) const
{
  return CQChartsConnectionList::stringToConnections(str, connections);
}

CQChartsAdjacencyNode *
CQChartsAdjacencyPlot::
getNodeByName(const QString &str) const
{
  auto p = nameNodeMap_.find(str);

  if (p != nameNodeMap_.end())
    return (*p).second;

  //---

  int id = nameNodeMap_.size();

  CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, str, 0, QModelIndex());

  CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

  auto p1 = th->nameNodeMap_.insert(th->nameNodeMap_.end(), NameNodeMap::value_type(str, node));

  return (*p1).second;
}

//---

void
CQChartsAdjacencyPlot::
autoFit()
{
  int tries = 3;

  for (int i = 0; i < tries; ++i) {
    factor_ = drawFactor();

    updateObjs();
  }
}

void
CQChartsAdjacencyPlot::
postResize()
{
  CQChartsPlot::postResize();

  setInsideObj(nullptr);

  clearRangeAndObjs();
}

bool
CQChartsAdjacencyPlot::
hasBackground() const
{
  return true;
}

void
CQChartsAdjacencyPlot::
execDrawBackground(CQChartsPaintDevice *device) const
{
  // calc text size
  CQChartsGeom::Point po = windowToPixel(CQChartsGeom::Point(0.0, 1.0));

  double pxs = windowToPixelWidth (scale());
  double pys = windowToPixelHeight(scale());

  double xts = maxLen()*factor_*pxs;
  double yts = maxLen()*factor_*pys;

  //---

  // set font
  double ts = std::min(pxs, pys);

  QFont font = this->textFont().calcFont();

  font.setPixelSize(ts >= 1 ? ts : 1.0);

  device->setFont(font);

  QFontMetricsF fm(device->font());

  //---

  // draw text
  QPen tpen;

  QColor tc = interpTextColor(ColorInd());

  setPen(tpen, true, tc, textAlpha());

  device->setPen(tpen);

  //---

  double twMax = 0.0;

  // draw row labels
  double px = po.x + lengthPixelWidth (bgMargin());
  double py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    const QString &str = node->name();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);

    QPointF pt(px + xts - tw - 2, py + pys - fm.descent());

    CQChartsDrawUtil::drawSimpleText(device, device->pixelToWindow(pt), str);

    py += pys;
  }

  // save draw factor
  CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->nodeData_.drawFactor = twMax/std::min(maxLen()*pxs, maxLen()*pys);

  // draw column labels
  px = po.x + lengthPixelWidth (bgMargin()) + xts;
  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    QPointF p1(px + pxs/2, py - 2);

    CQChartsRotatedText::draw(device, device->pixelToWindow(p1), node->name(), 90,
                              Qt::AlignHCenter | Qt::AlignBottom, /*alignBox*/true,
                              isTextContrast());

    px += pxs;
  }

  //---

  int nn = numNodes();

  px = po.x + lengthPixelWidth (bgMargin()) + xts;
  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  //---

  QBrush fillBrush;

  QColor fc = interpBackgroundFillColor(ColorInd());

  setBrush(fillBrush, true, fc, backgroundFillAlpha(), backgroundFillPattern());

  QRectF cellRect(px, py, nn*pxs, nn*pys);

  device->fillRect(device->pixelToWindow(cellRect), fillBrush);

  //---

  // draw empty cells
  CQChartsPenBrush emptyPenBrush;

  QColor pc = interpEmptyCellStrokeColor(ColorInd());
  QColor bc = interpEmptyCellFillColor  (ColorInd());

  setPen(emptyPenBrush.pen, true, pc, emptyCellStrokeAlpha(),
         emptyCellStrokeWidth(), emptyCellStrokeDash());

  setBrush(emptyPenBrush.brush, true, bc, emptyCellFillAlpha(), emptyCellFillPattern());

  CQChartsLength cornerSize = emptyCellCornerSize();

  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node1 : sortedNodes_) {
    double px = po.x + lengthPixelWidth(bgMargin()) + xts;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && CMathUtil::isZero(value));

      if (empty) {
        QRectF cellRect = device->pixelToWindow(QRectF(px, py, pxs, pys));

        CQChartsDrawUtil::setPenBrush(device, emptyPenBrush);

        CQChartsDrawUtil::drawRoundedPolygon(device, cellRect, cornerSize, cornerSize);
      }

      px += pxs;
    }

    py += pys;
  }

  if (insideObject()) {
    CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

    th->setInsideObj(nullptr);

    th->drawForeground();
  }
}

bool
CQChartsAdjacencyPlot::
hasForeground() const
{
  if (! insideObj())
    return true;

  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsAdjacencyPlot::
execDrawForeground(CQChartsPaintDevice *device) const
{
  if (insideObj())
    insideObj()->draw(device);
}

QColor
CQChartsAdjacencyPlot::
interpGroupColor(int group) const
{
  ColorInd ig(group, maxGroup() + 1);

  return interpPaletteColor(ig);
}

//------

CQChartsAdjacencyObj::
CQChartsAdjacencyObj(const CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                     CQChartsAdjacencyNode *node2, double value, const CQChartsGeom::BBox &rect,
                     const ColorInd &ig) :
 CQChartsPlotObj(const_cast<CQChartsAdjacencyPlot *>(plot), rect, ColorInd(), ig, ColorInd()),
 plot_(plot), node1_(node1), node2_(node2), value_(value)
{
  setDetailHint(DetailHint::MAJOR);

  addModelInd(node1->ind());
  addModelInd(node2->ind());
}

QString
CQChartsAdjacencyObj::
calcId() const
{
  QString groupStr1 = QString("(%1)").arg(node1_->group());
  QString groupStr2 = QString("(%1)").arg(node2_->group());

  return QString("%1:%2%3:%4%5:%6").arg(typeName()).
           arg(node1_->name()).arg(groupStr1).arg(node2_->name()).arg(groupStr2).arg(value_);
}

QString
CQChartsAdjacencyObj::
calcTipId() const
{
  QString groupStr1 = QString("(%1)").arg(node1_->group());
  QString groupStr2 = QString("(%1)").arg(node2_->group());

  CQChartsTableTip tableTip;

  tableTip.addTableRow("From" , node1_->name(), groupStr1);
  tableTip.addTableRow("To"   , node2_->name(), groupStr2);
  tableTip.addTableRow("Value", value_);

  //---

  //plot()->addTipColumns(tableTip, node1_->ind());

  //---

  return tableTip.str();
}

void
CQChartsAdjacencyObj::
getSelectIndices(Indices &inds) const
{
  for (auto &ind : modelInds())
    inds.insert(ind);
}

void
CQChartsAdjacencyObj::
draw(CQChartsPaintDevice *device)
{
  if (isInside()) {
    if (plot_->insideObj() != this) {
      CQChartsAdjacencyPlot *plot = const_cast<CQChartsAdjacencyPlot *>(plot_);

      plot->setInsideObj(const_cast<CQChartsAdjacencyObj *>(this));

      plot->drawForeground();
    }
  }

  //---

  ColorInd colorInd = calcColorInd();

  //---

  auto interpGroupColor = [&](CQChartsAdjacencyNode *node) {
    if (plot_->colorType() == CQChartsPlot::ColorType::AUTO)
      return plot_->interpGroupColor(node->group());
    else {
      return plot_->interpFillColor(colorInd);
    }
  };

  //---

  // calc fill color
  QColor bc = plot_->interpEmptyCellFillColor(ColorInd());

  // node to self (diagonal)
  if (node1_ == node2_) {
    bc = interpGroupColor(node1_);
  }
  // node to other node (scale to connections)
  else {
    QColor c1 = interpGroupColor(node1_);
    QColor c2 = interpGroupColor(node2_);

    double s = CMathUtil::map(value_, 0.0, plot_->maxValue(), 0.0, 1.0);

    double r = (c1.redF  () + c2.redF  () + s*bc.redF  ())/3;
    double g = (c1.greenF() + c2.greenF() + s*bc.greenF())/3;
    double b = (c1.blueF () + c2.blueF () + s*bc.blueF ())/3;

    bc = QColor::fromRgbF(r, g, b);
  }

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  QColor pc = plot_->interpStrokeColor(colorInd);

  plot_->setPenBrush(penBrush.pen, penBrush.brush,
    true, pc, plot_->strokeAlpha(), plot_->strokeWidth(), plot_->strokeDash(),
    true, bc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, penBrush.pen, penBrush.brush);

  //---

  // draw box
  drawRoundedPolygon(device, penBrush, rect(), plot_->cornerSize());
}

bool
CQChartsAdjacencyObj::
inside(const CQChartsGeom::Point &p) const
{
  return rect().inside(p);
}

double
CQChartsAdjacencyObj::
xColorValue(bool relative) const
{
  if (! relative)
    return node1_->id();
  else
    return CMathUtil::map(node1_->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}

double
CQChartsAdjacencyObj::
yColorValue(bool relative) const
{
  if (! relative)
    return node2_->id();
  else
    return CMathUtil::map(node2_->id(), 0.0, plot_->maxNode(), 0.0, 1.0);
}
