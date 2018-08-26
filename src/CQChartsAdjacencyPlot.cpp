#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsTip.h>

#include <CQStrParse.h>
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
  addColumnParameter("connections", "Connections", "connectionsColumn").
   setTip("List of Connection Pairs (Ids from id column and connection count)");
  endParameterGroup();

  // connections are id pairs and counts
  startParameterGroup("Name Pair/Count");
  addColumnParameter("namePair", "NamePair", "namePairColumn").
    setTip("Name Pairs (<name1>/<name2>)");
  addColumnParameter("count", "Count", "countColumn").setTip("Connection Count");
  endParameterGroup();

  addColumnParameter("name" , "Name" , "nameColumn" ).setTip("Name For Id");
  addColumnParameter("group", "Group", "groupColumn").setTip("Group Id for Color");

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
 CQChartsPlotShapeData<CQChartsAdjacencyPlot>(this),
 CQChartsPlotTextData <CQChartsAdjacencyPlot>(this),
 bgBox_(this), emptyCellBox_(this)
{
  CQChartsColor bg         (CQChartsColor::Type::INTERFACE_VALUE, 0.2);
  CQChartsColor border     (CQChartsColor::Type::INTERFACE_VALUE, 1.0);
  CQChartsColor emptyCellBg(CQChartsColor::Type::INTERFACE_VALUE, 0.1);

  bgBox_.setBackgroundColor(bg);

  setBorderColor(border);
  setBorderAlpha(0.5);

  emptyCellBox_.setBackgroundColor(emptyCellBg);

  setMargins(0, 0, 0, 0);

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
setConnectionsColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(connectionsColumn_, c, [&]() { updateRangeAndObjs(); } );
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
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsAdjacencyPlot::
setGroupColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() { updateRangeAndObjs(); } );
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
setBgColor(const CQChartsColor &c)
{
  bgBox_.setBackgroundColor(c);

  invalidateLayers();
}

QColor
CQChartsAdjacencyPlot::
interpBgColor(int i, int n) const
{
  return bgBox_.interpBackgroundColor(i, n);
}

//---

void
CQChartsAdjacencyPlot::
setCornerSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(cornerSize_, s, [&]() { invalidateLayers(); } );
}

void
CQChartsAdjacencyPlot::
setMargin(double r)
{
  bgBox_.setMargin(r);

  updateObjs();
}

//---

void
CQChartsAdjacencyPlot::
setEmptyCellColor(const CQChartsColor &s)
{
  emptyCellBox_.setBackgroundColor(s);

  invalidateLayers();
}

QColor
CQChartsAdjacencyPlot::
interpEmptyCellColor(int i, int n) const
{
  return emptyCellBox_.interpBackgroundColor(i, n);
}

void
CQChartsAdjacencyPlot::
setEmptyCellCornerSize(const CQChartsLength &s)
{
  emptyCellBox_.setCornerSize(s);

  invalidateLayers();
}

//---

void
CQChartsAdjacencyPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "connectionsColumn", "connections");
  addProperty("columns", this, "namePairColumn"   , "namePair"   );
  addProperty("columns", this, "countColumn"      , "count"      );
  addProperty("columns", this, "nameColumn"       , "name"       );
  addProperty("columns", this, "groupColumn"      , "group"      );

  addProperty("options", this, "sortType", "");

  addProperty("background", this, "bgColor", "color");

  addProperty("cell"       , this, "margin"     , "margin"    );
  addProperty("cell/stroke", this, "borderColor", "color"     );
  addProperty("cell/stroke", this, "borderAlpha", "alpha"     );
  addProperty("cell/stroke", this, "cornerSize" , "cornerSize");

  addProperty("emptyCell/fill"  , this, "emptyCellColor"     , "color"     );
  addProperty("emptyCell/stroke", this, "emptyCellCornerSize", "cornerSize");

  addTextProperties("text", "text");
}

void
CQChartsAdjacencyPlot::
calcRange()
{
  double r = 1.0;

  dataRange_.reset();

  dataRange_.updateRange(0, 0);
  dataRange_.updateRange(r, r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }
}

bool
CQChartsAdjacencyPlot::
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

  for (auto &pnode : nodes_)
    delete pnode.second;

  nodes_.clear();

  //---

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
  using NameNodeMap = std::map<QString,CQChartsAdjacencyNode *>;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsAdjacencyPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      bool ok1;

      QString linkStr = plot_->modelString(row, plot_->namePairColumn(), parent, ok1);

      if (! ok1)
        return State::SKIP;

      //---

      bool ok2;

      double value = plot_->modelReal(row, plot_->countColumn(), parent, ok2);

      if (! ok2)
        return State::SKIP;

      //---

      int group = row;

      if (plot_->groupColumn().isValid()) {
        bool ok3;

        group = plot_->modelInteger(row, plot_->groupColumn(), parent, ok3);

        if (! ok3)
          group = row;
      }

      //---

      QModelIndex nameInd  = plot_->modelIndex(row, plot_->nameColumn(), parent);
      QModelIndex nameInd1 = plot_->normalizeIndex(nameInd);

      int pos = linkStr.indexOf("/");

      if (pos == -1)
        return State::SKIP;

      QString srcStr  = linkStr.mid(0, pos ).simplified();
      QString destStr = linkStr.mid(pos + 1).simplified();

      auto ps = nameNodeMap_.find(srcStr);

      if (ps == nameNodeMap_.end()) {
        int id = nameNodeMap_.size();

        CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, srcStr, group, nameInd1);

        ps = nameNodeMap_.insert(ps, NameNodeMap::value_type(srcStr, node));
      }

      auto pd = nameNodeMap_.find(destStr);

      if (pd == nameNodeMap_.end()) {
        int id = nameNodeMap_.size();

        CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, destStr, 0, QModelIndex());

        pd = nameNodeMap_.insert(pd, NameNodeMap::value_type(destStr, node));
      }

      //---

      CQChartsAdjacencyNode *srcNode  = (*ps).second;
      CQChartsAdjacencyNode *destNode = (*pd).second;

      srcNode->setGroup(group);
      srcNode->setInd  (nameInd1);

      srcNode->addNode(destNode, value);

      return State::OK;
    }

    const NameNodeMap &nameNodeMap() const { return nameNodeMap_; }

   private:
    CQChartsAdjacencyPlot *plot_ { nullptr };
    NameNodeMap            nameNodeMap_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  NameNodeMap nameNodeMap = visitor.nameNodeMap();

  //---

  for (const auto &nameNode : nameNodeMap) {
    CQChartsAdjacencyNode *node = nameNode.second;

    nodes_[node->id()] = node;
  }

  //---

  sortNodes();

  //---

  double xb = pixelToWindowWidth (margin());
  double yb = pixelToWindowHeight(margin());

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

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      bool ok1;

      int id = plot_->modelInteger(row, plot_->idColumn(), parent , ok1);

      if (! ok1) id = row;

      //---

      bool ok2;

      int group = plot_->modelInteger(row, plot_->groupColumn(), parent, ok2);

      if (! ok2) group = row;

      //---

      bool ok3;

      QString connectionsStr = plot_->modelString(row, plot_->connectionsColumn(), parent, ok3);

      if (! ok3)
        return State::SKIP;

      //----

      bool ok4;

      QString name = plot_->modelString(row, plot_->nameColumn(), parent, ok4);

      if (! name.length())
        name = QString("%1").arg(id);

      //---

      QModelIndex nodeInd  = plot_->modelIndex(row, plot_->idColumn(), parent);
      QModelIndex nodeInd1 = plot_->normalizeIndex(nodeInd);

      ConnectionsData connections;

      connections.ind   = nodeInd1;
      connections.node  = id;
      connections.name  = name;
      connections.group = group;

      plot_->decodeConnections(connectionsStr, connections.connections);

      idConnections_[id] = connections;

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

  double xb = pixelToWindowWidth (margin());
  double yb = pixelToWindowHeight(margin());

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
decodeConnections(const QString &str, ConnectionDataArray &connections)
{
  // connections are { <connection> <connection> ... }
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
CQChartsAdjacencyPlot::
decodeConnection(const QString &str, ConnectionData &connection)
{
  // connection is { <node> <count> }
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

  clearRangeAndObjs();
}

void
CQChartsAdjacencyPlot::
drawBackground(QPainter *painter)
{
  CQChartsPlot::drawBackground(painter);

  //---

  double pxo, pyo;

  windowToPixel(0.0, 1.0, pxo, pyo);

  double pxs = windowToPixelWidth (scale_);
  double pys = windowToPixelHeight(scale_);

  double xts = maxLen_*factor_*pxs;
  double yts = maxLen_*factor_*pys;

  //---

  double ts = std::min(pxs, pys);

  QFont f = this->textFont();

  f.setPixelSize(ts);

  painter->setFont(f);

  QFontMetricsF fm(f);

  //---

  QColor tc = interpTextColor(0, 1);

  tc.setAlphaF(textAlpha());

  QPen tpen(tc);

  painter->setPen(tpen);

  //---

  double twMax = 0.0;

  // draw row labels
  double px = pxo + margin();
  double py = pyo + margin() + yts;

  for (auto &node : sortedNodes_) {
    const QString &str = node->name();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);

    painter->drawText(QPointF(px + xts - tw - 2, py + pys - fm.descent()), str);

    py += pys;
  }

  drawFactor_ = twMax/std::min(maxLen_*pxs, maxLen_*pys);

  // draw column labels
  px = pxo + margin() + xts;
  py = pyo + margin() + yts;

  for (auto &node : sortedNodes_) {
    CQChartsRotatedText::drawRotatedText(painter, px + pxs/2, py - 2, node->name(), 90,
                                         Qt::AlignHCenter | Qt::AlignBottom, /*alignBox*/true);

    px += pxs;
  }

  //---

  int nn = numNodes();

  px = pxo + margin() + xts;
  py = pyo + margin() + yts;

  QRectF cellRect(px, py, nn*pxs, nn*pys);

  painter->fillRect(cellRect, interpBgColor(0, 1));

  //---

  // draw empty cell
  QColor bc = interpEmptyCellColor(0, 1);

  py = pyo + margin() + yts;

  for (auto &node1 : sortedNodes_) {
    double px = pxo + margin() + xts;

    for (auto &node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && CMathUtil::isZero(value));

      if (empty) {
        QColor pc = bc.lighter(120);

        painter->setPen  (pc);
        painter->setBrush(bc);

        QRectF cellRect(px, py, pxs, pys);

        double cxs = lengthPixelWidth (emptyCellCornerSize());
        double cys = lengthPixelHeight(emptyCellCornerSize());

        CQChartsRoundedPolygon::draw(painter, cellRect, cxs, cys);
      }

      px += pxs;
    }

    py += pys;
  }

  setInsideObj(nullptr);
}

void
CQChartsAdjacencyPlot::
drawForeground(QPainter *painter)
{
  if (insideObj())
    insideObj()->draw(painter);

  //---

  CQChartsPlot::drawForeground(painter);
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
  if (isInside())
    plot_->setInsideObj(this);

  //---

  //int nn = plot_->numNodes();

  QColor bc = plot_->interpEmptyCellColor(0, 1);

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

  //QColor pc = bc.lighter(120);
  QColor pc = plot_->interpBorderColor(0, 1);

  pc.setAlphaF(plot_->borderAlpha());

  QBrush brush(bc);
  QPen   pen  (pc);

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
