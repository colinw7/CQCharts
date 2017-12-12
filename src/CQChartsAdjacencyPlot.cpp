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
  addParameters();
}

void
CQChartsAdjacencyPlotType::
addParameters()
{
  addColumnParameter("node"       , "Node"       , "nodeColumn"       , "", 0);
  addColumnParameter("connections", "Connections", "connectionsColumn", "", 1);

  addColumnParameter("group", "Group", "groupColumn", "optional");
  addColumnParameter("name" , "Name" , "nameColumn" , "optional");
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

void
CQChartsAdjacencyPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nodeColumn"       , "node"      );
  addProperty("columns", this, "connectionsColumn", "connection");
  addProperty("columns", this, "groupColumn"      , "group"     );
  addProperty("columns", this, "nameColumn"       , "name"      );
  addProperty(""       , this, "bgColor"          , ""          );
  addProperty(""       , this, "borderColor"      , ""          );
  addProperty(""       , this, "borderAlpha"      , ""          );
  addProperty(""       , this, "emptyCellColor"   , ""          );
  addProperty(""       , this, "textColor"        , ""          );
  addProperty(""       , this, "cornerSize"       , ""          );
  addProperty(""       , this, "font"             , ""          );
  addProperty(""       , this, "margin"           , ""          );
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

  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  int nr = model->rowCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    QModelIndex nodeInd        = model->index(r, nodeColumn       ());
    QModelIndex groupInd       = model->index(r, groupColumn      ());
    QModelIndex connectionsInd = model->index(r, connectionsColumn());
    QModelIndex nameInd        = model->index(r, nameColumn       ());

    QModelIndex nodeInd1 = normalizeIndex(nodeInd);

    //---

    bool ok1, ok2;

    int id    = CQChartsUtil::modelInteger(model, nodeInd , ok1);
    int group = CQChartsUtil::modelInteger(model, groupInd, ok2);

    if (! ok1) id    = r;
    if (! ok2) group = r;

    bool ok3;

    QString connectionsStr = CQChartsUtil::modelString(model, connectionsInd, ok3);

    bool ok4;

    QString name = CQChartsUtil::modelString(model, nameInd, ok4);

    if (! name.length())
      name = QString("%1").arg(id);

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
      int value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || value) {
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

#if 0
  if      (sort_ == SortType::NAME)
    std::sort(sortedNodes_.begin(), sortedNodes_.end(), NodeNameCmp());
  else if (sort_ == SortType::GROUP)
    std::sort(sortedNodes_.begin(), sortedNodes_.end(), NodeGroupCmp());
  else if (sort_ == SortType::COUNT)
    std::sort(sortedNodes_.begin(), sortedNodes_.end(), NodeCountCmp());
#endif
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
      int value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && ! value);

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
                     CQChartsAdjacencyNode *node2, int value, const CQChartsGeom::BBox &rect) :
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
