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
   setTip("List of Connection Pairs (Ids from id column and connection count)").setDiscrimator();

  endParameterGroup();

  //---

  // connections are id pairs and counts
  startParameterGroup("Name Pair/Count");

  addColumnParameter("namePair", "Name Pair", "namePairColumn").
   setTip("Connected Name Pairs (<name1>/<name2>)").setDiscrimator();

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
  for (auto &pnode : nodes_)
    delete pnode.second;
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

  addProperty("columns", this, "nodeColumn"       , "node"      );
  addProperty("columns", this, "connectionsColumn", "connections");
  addProperty("columns", this, "nameColumn"       , "name"       );

  addProperty("columns", this, "namePairColumn", "namePair");
  addProperty("columns", this, "countColumn"   , "count"   );

  addProperty("columns", this, "groupIdColumn", "groupId");

  addProperty("options", this, "sortType", "sort"  );
  addProperty("options", this, "bgMargin", "margin");

  addFillProperties("background/fill", "backgroundFill");

  addFillProperties("cell/fill"  , "fill"  );
  addLineProperties("cell/stroke", "border");

  addProperty("cell/stroke", this, "cornerSize", "cornerSize");

  addFillProperties("emptyCell/fill"  , "emptyCellFill"  );
  addLineProperties("emptyCell/stroke", "emptyCellBorder");

  addProperty("emptyCell/stroke", this, "cornerSize", "cornerSize");

  addTextProperties("text", "text");
}

CQChartsGeom::Range
CQChartsAdjacencyPlot::
calcRange()
{
  CQChartsGeom::Range dataRange;

  double r = 1.0;

  //---

  connectionsColumnType_ = columnValueType(connectionsColumn());
  namePairColumnType_    = columnValueType(namePairColumn   ());

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
createObjs()
{
  CQPerfTrace trace("CQChartsAdjacencyPlot::createObjs");

  for (auto &pnode : nodes_)
    delete pnode.second;

  nodes_.clear();

  //---

  setInsideObj(nullptr);

  if      (namePairColumn().isValid() && countColumn().isValid())
    return initHierObjs();
  else if (connectionsColumn().isValid())
    return initConnectionObjs();
  else
    return false;
}

bool
CQChartsAdjacencyPlot::
initHierObjs()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
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
    CQChartsAdjacencyPlot *plot_ { nullptr };
  };

  nameNodeMap_.clear();

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  for (const auto &nameNode : nameNodeMap_) {
    CQChartsAdjacencyNode *node = nameNode.second;

    nodes_[node->id()] = node;
  }

  //---

  sortNodes();

  //---

  double xb = lengthPlotWidth (bgMargin());
  double yb = lengthPlotHeight(bgMargin());

  maxLen_ = 0;

  for (auto &node1 : sortedNodes_) {
    maxLen_ = std::max(maxLen_, int(node1->name().size()));
  }

  int nn = numNodes();

  if (nn + maxLen_*factor_ > 0)
    scale_ = (1.0 - 2*std::max(xb, yb))/(nn + maxLen_*factor_);
  else
    scale_ = 1.0;

  double tsize = maxLen_*factor_*scale_;

  //---

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CMathUtil::isZero(value)) {
        CQChartsGeom::BBox bbox(x, y - scale_, x + scale_, y);

        CQChartsAdjacencyObj *obj = new CQChartsAdjacencyObj(this, node1, node2, value, bbox);

        addPlotObject(obj);
      }

      x += scale_;
    }

    y -= scale_;
  }

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
initConnectionObjs()
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      ConnectionsData connections;

      if (! plot_->getRowConnections(data, connections))
        return State::SKIP;

      idConnections_[connections.node] = connections;

      return State::OK;
    }

    const IdConnectionsData &idConnections() const { return idConnections_; }

   private:
    CQChartsAdjacencyPlot *plot_ { nullptr };
    IdConnectionsData      idConnections_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  idConnections_ = visitor.idConnections();

  //---

  for (const auto &idConnections : idConnections_) {
    int                id    = idConnections.first;
    const QModelIndex& ind   = idConnections.second.ind;
    const QString&     name  = idConnections.second.name;
    int                group = idConnections.second.group;

    CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, name, group, ind);

    nodes_[id] = node;
  }

  //---

  for (const auto &idConnections : idConnections_) {
    int                    id          = idConnections.first;
    const ConnectionsData &connections = idConnections.second;

    CQChartsAdjacencyNode *node = nodes_[id];

    for (const auto &connection : connections.connections) {
      CQChartsAdjacencyNode *node1 = nodes_[connection.node];

      node->addNode(node1, connection.count);
    }
  }

  //---

  sortNodes();

  //---

  double xb = lengthPlotWidth (bgMargin());
  double yb = lengthPlotHeight(bgMargin());

  maxLen_ = 0;

  for (auto &node1 : sortedNodes_) {
    maxLen_ = std::max(maxLen_, int(node1->name().size()));
  }

  int nn = numNodes();

  scale_ = (1.0 - 2*std::max(xb, yb))/(nn + maxLen_*factor_);

  double tsize = maxLen_*factor_*scale_;

  //---

  double y = 1.0 - tsize - yb;

  for (auto &node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CMathUtil::isZero(value)) {
        CQChartsGeom::BBox bbox(x, y - scale_, x + scale_, y);

        CQChartsAdjacencyObj *obj = new CQChartsAdjacencyObj(this, node1, node2, value, bbox);

        addPlotObject(obj);
      }

      x += scale_;
    }

    y -= scale_;
  }

  //---

  return true;
}

bool
CQChartsAdjacencyPlot::
getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connections)
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
sortNodes()
{
  maxValue_ = 0;
  maxGroup_ = 0;

  sortedNodes_.clear();

  for (auto &pnode : nodes_) {
    CQChartsAdjacencyNode *node = pnode.second;

    sortedNodes_.push_back(node);

    maxValue_ = std::max(maxValue_, node->maxCount());
    maxGroup_ = std::max(maxGroup_, node->group());
  }

  if      (sortType() == SortType::NAME)
    std::sort(sortedNodes_.begin(), sortedNodes_.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        return lhs->name() < rhs->name();
      });
  else if (sortType() == SortType::GROUP)
    std::sort(sortedNodes_.begin(), sortedNodes_.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        if (lhs->group() != rhs->group())
          return lhs->group() < rhs->group();

        return lhs->name() < rhs->name();
      });
  else if (sortType() == SortType::COUNT)
    std::sort(sortedNodes_.begin(), sortedNodes_.end(),
      [](CQChartsAdjacencyNode *lhs, CQChartsAdjacencyNode *rhs) {
        if (lhs->count() != rhs->count())
          return lhs->count() < rhs->count();

        return lhs->name() < rhs->name();
      });
}

bool
CQChartsAdjacencyPlot::
decodeConnections(const QString &str, Connections &connections)
{
  return CQChartsConnectionList::stringToConnections(str, connections);
}

CQChartsAdjacencyNode *
CQChartsAdjacencyPlot::
getNodeByName(const QString &str)
{
  auto p = nameNodeMap_.find(str);

  if (p == nameNodeMap_.end()) {
    int id = nameNodeMap_.size();

    CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, str, 0, QModelIndex());

    p = nameNodeMap_.insert(p, NameNodeMap::value_type(str, node));
  }

  return (*p).second;
}

//---

void
CQChartsAdjacencyPlot::
autoFit()
{
  int tries = 3;

  for (int i = 0; i < tries; ++i) {
    factor_ = drawFactor_;

    updateObjs();
  }
}

void
CQChartsAdjacencyPlot::
handleResize()
{
  CQChartsPlot::handleResize();

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
drawBackground(QPainter *painter)
{
  double pxo, pyo;

  windowToPixel(0.0, 1.0, pxo, pyo);

  double pxs = windowToPixelWidth (scale_);
  double pys = windowToPixelHeight(scale_);

  double xts = maxLen_*factor_*pxs;
  double yts = maxLen_*factor_*pys;

  //---

  double ts = std::min(pxs, pys);

  QFont font = this->textFont();

  font.setPixelSize(ts >= 1 ? ts : 1.0);

  painter->setFont(font);

  QFontMetricsF fm(painter->font());

  //---

  QPen tpen;

  QColor tc = interpTextColor(0, 1);

  setPen(tpen, true, tc, textAlpha(), CQChartsLength("0px"));

  painter->setPen(tpen);

  //---

  double twMax = 0.0;

  // draw row labels
  double px = pxo + lengthPixelWidth (bgMargin());
  double py = pyo + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    const QString &str = node->name();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);

    painter->drawText(QPointF(px + xts - tw - 2, py + pys - fm.descent()), str);

    py += pys;
  }

  drawFactor_ = twMax/std::min(maxLen_*pxs, maxLen_*pys);

  // draw column labels
  px = pxo + lengthPixelWidth (bgMargin()) + xts;
  py = pyo + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node : sortedNodes_) {
    CQChartsRotatedText::drawRotatedText(painter, px + pxs/2, py - 2, node->name(), 90,
                                         Qt::AlignHCenter | Qt::AlignBottom, /*alignBox*/true);

    px += pxs;
  }

  //---

  int nn = numNodes();

  px = pxo + lengthPixelWidth (bgMargin()) + xts;
  py = pyo + lengthPixelHeight(bgMargin()) + yts;

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

  py = pyo + lengthPixelHeight(bgMargin()) + yts;

  for (auto &node1 : sortedNodes_) {
    double px = pxo + lengthPixelWidth(bgMargin()) + xts;

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
    setInsideObj(nullptr);

    invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
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
drawForeground(QPainter *painter)
{
  if (insideObj())
    insideObj()->draw(painter);
}

QColor
CQChartsAdjacencyPlot::
interpGroupColor(int group) const
{
  return interpPaletteColor((1.0*group)/maxGroup_);
}

//------

CQChartsAdjacencyObj::
CQChartsAdjacencyObj(CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                     CQChartsAdjacencyNode *node2, double value, const CQChartsGeom::BBox &rect) :
 CQChartsPlotObj(plot, rect), plot_(plot), node1_(node1), node2_(node2), value_(value)
{
}

QString
CQChartsAdjacencyObj::
calcId() const
{
  QString groupStr1 = QString("(%1)").arg(node1_->group());
  QString groupStr2 = QString("(%1)").arg(node2_->group());

  return QString("%1%2:%3%4:%5").arg(node1_->name()).arg(groupStr1).
                                 arg(node2_->name()).arg(groupStr2).arg(value_);
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
      plot_->setInsideObj(this);

      plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
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

  QPen   pen;
  QBrush brush;

  QColor pc = plot_->interpBorderColor(0, 1);

  plot_->setPen(pen, true, pc, plot_->borderAlpha(),
                plot_->borderWidth(), plot_->borderDash());

  plot_->setBrush(brush, true, bc, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  painter->setPen  (pen);
  painter->setBrush(brush);

  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

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
