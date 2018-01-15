#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsRoundedPolygon.h>
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
  addColumnParameter("node"       , "Node"       , "nodeColumn"       , "optional");
  addColumnParameter("connections", "Connections", "connectionsColumn", "optional");
  addColumnParameter("value"      , "Value"      , "valueColumn"      , "optional");
  addColumnParameter("group"      , "Group"      , "groupColumn"      , "optional");
  addColumnParameter("name"       , "Name"       , "nameColumn"       , "optional");
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
 bgBox_(this), cellBox_(this), emptyCellBox_(this)
{
  CQChartsPaletteColor bg         (CQChartsPaletteColor::Type::THEME_VALUE, 0.2);
  CQChartsPaletteColor border     (CQChartsPaletteColor::Type::THEME_VALUE, 1.0);
  CQChartsPaletteColor emptyCellBg(CQChartsPaletteColor::Type::THEME_VALUE, 0.1);

  bgBox_       .setBackgroundColor(bg);
  cellBox_     .setBorderColor    (border);
  cellBox_     .setBorderAlpha    (0.5);
  emptyCellBox_.setBackgroundColor(emptyCellBg);

  textColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setMargins(0, 0, 0, 0);

  addTitle();
}

CQChartsAdjacencyPlot::
~CQChartsAdjacencyPlot()
{
  for (auto pnode : nodes_)
    delete pnode.second;
}

void
CQChartsAdjacencyPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nodeColumn"       , "node"      );
  addProperty("columns", this, "connectionsColumn", "connection");
  addProperty("columns", this, "valueColumn"      , "value"     );
  addProperty("columns", this, "groupColumn"      , "group"     );
  addProperty("columns", this, "nameColumn"       , "name"      );

  addProperty("", this, "sortType"      , "");
  addProperty("", this, "bgColor"       , "");
  addProperty("", this, "emptyCellColor", "");
  addProperty("", this, "margin"        , "");

  addProperty("border", this, "borderColor", "color");
  addProperty("border", this, "borderAlpha", "alpha");
  addProperty("border", this, "cornerSize" , "cornerSize");

  addProperty("text", this, "textColor", "color");
  addProperty("text", this, "font"     , "font" );
}

void
CQChartsAdjacencyPlot::
updateRange(bool apply)
{
  double xr = 1.0;
  double yr = 1.0;

  if (isEqualScale()) {
    double aspect = this->aspect();

    if (aspect > 1.0)
      xr *= aspect;
    else
      yr *= 1.0/aspect;
  }

  dataRange_.reset();

  dataRange_.updateRange( 0,  0);
  dataRange_.updateRange(xr, yr);

  //---

  if (apply)
    applyDataRange();
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

  for (auto pnode : nodes_)
    delete pnode.second;

  nodes_.clear();

  //---

  if      (nameColumn() >= 0 && valueColumn() >= 0)
    return initHierObjs();
  else if (connectionsColumn() >= 0)
    return initConnectionObjs();
  else
    return false;
}

bool
CQChartsAdjacencyPlot::
initHierObjs()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  using NameNodeMap = std::map<QString,CQChartsAdjacencyNode *>;

  NameNodeMap nameNodeMap;

  //---

  int nr = model->rowCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    QModelIndex nameInd = model->index(r, nameColumn ());

    QModelIndex nameInd1 = normalizeIndex(nameInd);

    bool ok1;

    QString linkStr = CQChartsUtil::modelString(model, nameInd, ok1);

    if (! ok1)
      continue;

    //---

    QModelIndex valueInd = model->index(r, valueColumn());

    bool ok2;

    double value = CQChartsUtil::modelReal(model, valueInd, ok2);

    if (! ok2)
      continue;

    //---

    int group = r;

    if (groupColumn() >= 0) {
      QModelIndex groupInd = model->index(r, groupColumn());

      bool ok3;

      group = CQChartsUtil::modelInteger(model, groupInd, ok3);

      if (! ok3)
        group = r;
    }

    //---

    int pos = linkStr.indexOf("/");

    if (pos == -1)
      continue;

    QString srcStr  = linkStr.mid(0, pos ).simplified();
    QString destStr = linkStr.mid(pos + 1).simplified();

    auto ps = nameNodeMap.find(srcStr);

    if (ps == nameNodeMap.end()) {
      int id = nameNodeMap.size();

      CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, srcStr, group, nameInd1);

      ps = nameNodeMap.insert(ps, NameNodeMap::value_type(srcStr, node));
    }

    auto pd = nameNodeMap.find(destStr);

    if (pd == nameNodeMap.end()) {
      int id = nameNodeMap.size();

      CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, destStr, 0, QModelIndex());

      pd = nameNodeMap.insert(pd, NameNodeMap::value_type(destStr, node));
    }

    //---

    CQChartsAdjacencyNode *srcNode  = (*ps).second;
    CQChartsAdjacencyNode *destNode = (*pd).second;

    srcNode->setGroup(group);
    srcNode->setInd  (nameInd1);

    srcNode->addNode(destNode, value);
  }

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

  for (auto node1 : sortedNodes_) {
    maxLen_ = std::max(maxLen_, int(node1->name().size()));
  }

  int nn = numNodes();

  scale_ = (1.0 - 2*std::max(xb, yb))/(nn + maxLen_*factor_);

  double tsize = maxLen_*factor_*scale_;

  //---

  double y = 1.0 - tsize - yb;

  for (auto node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CQChartsUtil::isZero(value)) {
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
  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  int nr = model->rowCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    QModelIndex nodeInd        = model->index(r, nodeColumn       ());
    QModelIndex connectionsInd = model->index(r, connectionsColumn());

    QModelIndex nodeInd1 = normalizeIndex(nodeInd);

    //---

    bool ok1;

    QModelIndex nameInd = model->index(r, nameColumn());

    int id = CQChartsUtil::modelInteger(model, nodeInd , ok1);

    if (! ok1) id = r;

    //---

    bool ok2;

    QModelIndex groupInd = model->index(r, groupColumn());

    int group = CQChartsUtil::modelInteger(model, groupInd, ok2);

    if (! ok2) group = r;

    //---

    bool ok3;

    QString connectionsStr = CQChartsUtil::modelString(model, connectionsInd, ok3);

    if (! ok3)
      continue;

    //----

    bool ok4;

    QString name = CQChartsUtil::modelString(model, nameInd, ok4);

    if (! name.length())
      name = QString("%1").arg(id);

    //---

    ConnectionsData connections;

    connections.ind   = nodeInd1;
    connections.node  = id;
    connections.name  = name;
    connections.group = group;

    decodeConnections(connectionsStr, connections.connections);

    idConnections_[id] = connections;
  }

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

  for (auto node1 : sortedNodes_) {
    maxLen_ = std::max(maxLen_, int(node1->name().size()));
  }

  int nn = numNodes();

  scale_ = (1.0 - 2*std::max(xb, yb))/(nn + maxLen_*factor_);

  double tsize = maxLen_*factor_*scale_;

  //---

  double y = 1.0 - tsize - yb;

  for (auto node1 : sortedNodes_) {
    double x = tsize + xb;

    for (auto node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || ! CQChartsUtil::isZero(value)) {
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

  for (auto pnode : nodes_) {
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
CQChartsAdjacencyPlot::
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
  dataRange_.reset();

  clearPlotObjects();

  CQChartsPlot::handleResize();
}

void
CQChartsAdjacencyPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
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

  QFont f = this->font();

  f.setPixelSize(ts);

  painter->setFont(f);

  QFontMetricsF fm(f);

  //---

  double twMax = 0.0;

  // draw row labels
  painter->setPen(interpTextColor(0, 1));

  double px = pxo + margin();
  double py = pyo + margin() + yts;

  for (auto node : sortedNodes_) {
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

  for (auto node : sortedNodes_) {
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

  QColor bc = interpEmptyCellColor(0, 1);

  py = pyo + margin() + yts;

  for (auto node1 : sortedNodes_) {
    double px = pxo + margin() + xts;

    for (auto node2 : sortedNodes_) {
      double value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && CQChartsUtil::isZero(value));

      if (empty) {
        QColor pc = bc.lighter(120);

        painter->setPen  (pc);
        painter->setBrush(bc);

        QRectF cellRect(px, py, pxs, pys);

        double cs = 0; // cornerSize()

        CQChartsRoundedPolygon::draw(painter, cellRect, cs);
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
    insideObj()->draw(painter, CQChartsPlot::Layer::FG);
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
  return QString("%1(%2):%3(%4):%5").arg(node1_->name()).arg(node1_->group()).
                                     arg(node2_->name()).arg(node2_->group()).arg(value_);
}

void
CQChartsAdjacencyObj::
addSelectIndex()
{
  plot_->addSelectIndex(node1_->ind());
  plot_->addSelectIndex(node2_->ind());
}

bool
CQChartsAdjacencyObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == node1_->ind() || ind == node2_->ind());
}

void
CQChartsAdjacencyObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
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

    int r = (c1.red  () + c2.red  () + s*bc.red  ())/3;
    int g = (c1.green() + c2.green() + s*bc.green())/3;
    int b = (c1.blue () + c2.blue () + s*bc.blue ())/3;

    bc = QColor(r, g, b);
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

  CQChartsRoundedPolygon::draw(painter, CQChartsUtil::toQRect(prect), plot_->cornerSize());
}

bool
CQChartsAdjacencyObj::
inside(const CQChartsGeom::Point &p) const
{
  return rect().inside(p);
}
