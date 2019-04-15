#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsNamePair.h>
#include <CQChartsTip.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QPainter>

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

  addColumnParameter("name", "Name", "nameColumn").
   setString().setTip("Name For Id");

  addColumnParameter("groupId", "Group Id", "groupIdColumn").
   setNumeric().setTip("Group Id for Color");

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsAdjacencyPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws connectivity information between two different sets of data as a "
         "matrix where the color of the cells represents the group and connectivity.</p>\n"
         "<h2>Columns</h2>\n"
         "<p>Connection information can be supplied using:</p>\n"
         "<ul>\n"
         "<li>A list of connections in the <b>Connections</b> column of the form "
         "{{&lt;id&gt; &lt;count&gt;} ...}.</li>\n"
         "<li>A name pair using <b>NamePair</b> column in the form &lt;id1&gt;/&lt;id2&gt; "
         "and a count using the <b>Count</b> column.</li>\n"
         "</ul>\n"
         "<p>The column id is taken from the <b>Id</b> column and an optional "
         "name for the id can be supplied in the <b>Name</b> column.</p>\n"
         "<p>The group is specified using the <b>Group</b> column.</p>";
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

  setBorderColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1.0));
  setBorderAlpha(0.5);

  setEmptyCellFillColor  (CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.1));
  setEmptyCellBorderColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.2));

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
  CQChartsUtil::testAndSet(nodeColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setConnectionsColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setNamePairColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(namePairColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setCountColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(countColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setGroupIdColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupIdColumn_, c, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
setSortType(const SortType &t)
{
  CQChartsUtil::testAndSet(sortType_, t, [&]() { queueUpdateRangeAndObjs(); } );
}

//---

void
CQChartsAdjacencyPlot::
setBgMargin(const CQChartsLength &l)
{
  bgMargin_ = l;

  queueUpdateObjs();
}

//---

void
CQChartsAdjacencyPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nodeColumn"       , "node"       )->setDesc("Node column");
  addProperty("columns", this, "connectionsColumn", "connections")->setDesc("Connections column");
  addProperty("columns", this, "nameColumn"       , "name"       )->setDesc("Name column");

  addProperty("columns", this, "namePairColumn", "namePair")->setDesc("Name/Value column");
  addProperty("columns", this, "countColumn"   , "count"   )->setDesc("Count column");

  addProperty("columns", this, "groupIdColumn", "groupId")->setDesc("Grouping column");

  addProperty("options", this, "sortType", "sort"  )->setDesc("Sort type");
  addProperty("options", this, "bgMargin", "margin")->setDesc("Background margin");

  addFillProperties("background/fill", "backgroundFill");

  addFillProperties("cell/fill"  , "fill"  );
  addLineProperties("cell/stroke", "border");

  addProperty("cell/stroke", this, "cornerSize", "cornerSize")->setDesc("Cell box corner size");

  addFillProperties("emptyCell/fill"  , "emptyCellFill"  );
  addLineProperties("emptyCell/stroke", "emptyCellBorder");

  addProperty("emptyCell/stroke", this, "cornerSize", "cornerSize")->
    setDesc("Empty cell box corner size");

  addTextProperties("text", "text");
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

  NoUpdate noUpdate(const_cast<CQChartsAdjacencyPlot *>(this));

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

        CQChartsAdjacencyObj *obj = new CQChartsAdjacencyObj(this, node1, node2, value, bbox);

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

  int nn = numNodes();

  th->nodeData_.scale = (1.0 - 2*std::max(xb, yb))/(nn + maxLen()*factor_);

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

        CQChartsAdjacencyObj *obj = new CQChartsAdjacencyObj(this, node1, node2, value, bbox);

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

  for (auto &pnode : nodes) {
    CQChartsAdjacencyNode *node = const_cast<CQChartsAdjacencyNode *>(pnode.second);

    sortedNodes.push_back(node);

    nodeData.maxValue = std::max(nodeData.maxValue, node->maxCount());
    nodeData.maxGroup = std::max(nodeData.maxGroup, node->group());
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

    queueUpdateObjs();
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
drawBackground(QPainter *painter) const
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

  painter->setFont(font);

  QFontMetricsF fm(painter->font());

  //---

  // draw text
  QPen tpen;

  QColor tc = interpTextColor(0, 1);

  setPen(tpen, true, tc, textAlpha());

  painter->setPen(tpen);

  //---

  double twMax = 0.0;

  // draw row labels
  double px = po.x + lengthPixelWidth (bgMargin());
  double py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    const QString &str = node->name();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);

    CQChartsDrawUtil::drawSimpleText(painter, px + xts - tw - 2, py + pys - fm.descent(), str);

    py += pys;
  }

  // save draw factor
  CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

  th->nodeData_.drawFactor = twMax/std::min(maxLen()*pxs, maxLen()*pys);

  // draw column labels
  px = po.x + lengthPixelWidth (bgMargin()) + xts;
  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    CQChartsRotatedText::draw(painter, px + pxs/2, py - 2, node->name(), 90,
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

  QColor fc = interpBackgroundFillColor(0, 1);

  setBrush(fillBrush, true, fc, backgroundFillAlpha(), backgroundFillPattern());

  QRectF cellRect(px, py, nn*pxs, nn*pys);

  painter->fillRect(cellRect, fillBrush);

  //---

  // draw empty cells
  QPen   emptyPen;
  QBrush emptyBrush;

  QColor pc = interpEmptyCellBorderColor(0, 1);
  QColor bc = interpEmptyCellFillColor  (0, 1);

  setPen(emptyPen, true, pc, emptyCellBorderAlpha(),
         emptyCellBorderWidth(), emptyCellBorderDash());

  setBrush(emptyBrush, true, bc, emptyCellFillAlpha(), emptyCellFillPattern());

  double cxs = lengthPixelWidth (emptyCellCornerSize());
  double cys = lengthPixelHeight(emptyCellCornerSize());

  py = po.y + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node1 : sortedNodes_) {
    double px = po.x + lengthPixelWidth(bgMargin()) + xts;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && CMathUtil::isZero(value));

      if (empty) {
        painter->setPen  (emptyPen);
        painter->setBrush(emptyBrush);

        QRectF cellRect(px, py, pxs, pys);

        CQChartsRoundedPolygon::draw(painter, cellRect, cxs, cys);
      }

      px += pxs;
    }

    py += pys;
  }

  if (insideObject()) {
    CQChartsAdjacencyPlot *th = const_cast<CQChartsAdjacencyPlot *>(this);

    th->setInsideObj(nullptr);

    th->queueDrawForeground();
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
drawForeground(QPainter *painter) const
{
  if (insideObj())
    insideObj()->draw(painter);
}

QColor
CQChartsAdjacencyPlot::
interpGroupColor(int group) const
{
  return interpPaletteColor((1.0*group)/maxGroup());
}

//------

CQChartsAdjacencyObj::
CQChartsAdjacencyObj(const CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                     CQChartsAdjacencyNode *node2, double value, const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(const_cast<CQChartsAdjacencyPlot *>(plot), rect), plot_(plot),
 node1_(node1), node2_(node2), value_(value)
{
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

  return tableTip.str();
}

void
CQChartsAdjacencyObj::
getSelectIndices(Indices &inds) const
{
  inds.insert(node1_->ind());
  inds.insert(node2_->ind());
}

void
CQChartsAdjacencyObj::
draw(QPainter *painter)
{
  if (isInside()) {
    if (plot_->insideObj() != this) {
      CQChartsAdjacencyPlot *plot = const_cast<CQChartsAdjacencyPlot *>(plot_);

      plot->setInsideObj(this);

      plot->queueDrawForeground();
    }
  }

  //---

  //int nn = plot_->numNodes();

  QColor bc = plot_->interpEmptyCellFillColor(0, 1);

  // node to self (diagonal)
  if (node1_ == node2_) {
    bc = plot_->interpGroupColor(node1_->group());
  }
  // node to other node (scale to connections)
  else {
    QColor c1 = plot_->interpGroupColor(node1_->group());
    QColor c2 = plot_->interpGroupColor(node2_->group());

    double s = (1.0*plot_->maxValue() - value_)/plot_->maxValue();

    double r = (c1.redF  () + c2.redF  () + s*bc.redF  ())/3;
    double g = (c1.greenF() + c2.greenF() + s*bc.greenF())/3;
    double b = (c1.blueF () + c2.blueF () + s*bc.blueF ())/3;

    bc = QColor::fromRgbF(r, g, b);
  }

  //---

  // set pen and brush
  QPen   pen;
  QBrush brush;

  QColor pc = plot_->interpBorderColor(0, 1);

  plot_->setPen(pen, true, pc, plot_->borderAlpha(),
                plot_->borderWidth(), plot_->borderDash());

  plot_->setBrush(brush, true, bc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw box
  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  double cxs = plot_->lengthPixelWidth (plot_->cornerSize());
  double cys = plot_->lengthPixelHeight(plot_->cornerSize());

  CQChartsRoundedPolygon::draw(painter, CQChartsUtil::toQRect(prect), cxs, cys);
}

bool
CQChartsAdjacencyObj::
inside(const CQChartsGeom::Point &p) const
{
  return rect().inside(p);
}
