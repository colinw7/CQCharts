#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRotatedText.h>
#include <CQStrParse.h>

#include <QAbstractItemModel>
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
 CQChartsPlot(view, view->charts()->plotType("adjacency"), model)
{
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
  addProperty(""       , this, "textColor"        , ""          );
  addProperty(""       , this, "emptyCellColor"   , ""          );
  addProperty(""       , this, "font"             , ""          );
}

void
CQChartsAdjacencyPlot::
updateRange()
{
  dataRange_.updateRange(0, 0);
  dataRange_.updateRange(1, 1);

  //setEqualScale(true);
}

void
CQChartsAdjacencyPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //--

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int n = model->rowCount(QModelIndex());

  for (int i = 0; i < n; ++i) {
    bool ok1, ok2;

    int id    = CQChartsUtil::modelInteger(model, i, nodeColumn_ , ok1);
    int group = CQChartsUtil::modelInteger(model, i, groupColumn_, ok2);

    if (! ok1) id    = i;
    if (! ok2) group = i;

    bool ok3;

    QString connectionsStr = CQChartsUtil::modelString(model, i, connectionsColumn_, ok3);

    bool ok4;

    QString name = CQChartsUtil::modelString(model, i, nameColumn_, ok4);

    if (! name.length())
      name = QString("%1").arg(id);

    ConnectionsData connections;

    connections.node  = id;
    connections.name  = name;
    connections.group = group;

    decodeConnections(connectionsStr, connections.connections);

    idConnections_[id] = connections;
  }

  //---

  for (const auto &idConnections : idConnections_) {
    int            id    = idConnections.first;
    const QString &name  = idConnections.second.name;
    int            group = idConnections.second.group;

    CQChartsAdjacencyNode *node = new CQChartsAdjacencyNode(id, name.toStdString(), group);

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

  maxLen_ = 0;

  for (auto node1 : sortedNodes_) {
    maxLen_ = std::max(maxLen_, int(node1->name().size()));
  }

  int nn = numNodes();

  scale_ = 1.0/(nn + maxLen_*factor_);

  double tsize = maxLen_*factor_*scale_;

  //---

  double y = 1.0 - tsize;

  for (auto node1 : sortedNodes_) {
    double x = tsize;

    for (auto node2 : sortedNodes_) {
      int value = node1->nodeValue(node2);

      // skip unconnected
      if (node1 == node2 || value) {
        CBBox2D bbox(x, y - scale_, x + scale_, y);

        CQChartsAdjacencyObj *obj = new CQChartsAdjacencyObj(this, node1, node2, value, bbox);

        obj->setId(QString("%1(%2):%3(%4):%5").
          arg(node1->name().c_str()).arg(node1->group()).
          arg(node2->name().c_str()).arg(node2->group()).arg(value));

        addPlotObject(obj);
      }

      x += scale_;
    }

    y -= scale_;
  }
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

    initObjs(/*force*/true);

    update();
  }
}

void
CQChartsAdjacencyPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsAdjacencyPlot::
drawBackground(QPainter *p)
{
  CQChartsPlot::drawBackground(p);

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

  p->setFont(f);

  QFontMetricsF fm(f);

  //---

  double twMax = 0.0;

  // draw row labels
  p->setPen(textColor());

  double px = pxo;
  double py = pyo + yts;

  for (auto node : sortedNodes_) {
    QString str = node->name().c_str();

    double tw = fm.width(str) + 4;

    twMax = std::max(twMax, tw);

    p->drawText(px + xts - tw - 2, py + pys - fm.descent(), str);

    py += pys;
  }

  drawFactor_ = twMax/std::min(maxLen_*pxs, maxLen_*pys);

  // draw column labels
  px = pxo + xts;
  py = pyo + yts;

  for (auto node : sortedNodes_) {
    CQRotatedText::drawRotatedText(p, px + pxs/2, py - 2, node->name().c_str(), 90,
                                   Qt::AlignHCenter | Qt::AlignBottom, /*alignBox*/true);

    px += pxs;
  }

  //---

  int nn = numNodes();

  px = pxo + xts;
  py = pyo + yts;

  p->fillRect(QRectF(px, py, nn*pxs, nn*pys), bgColor());

  //---

  QColor bc = emptyCellColor();

  py = pyo + yts;

  for (auto node1 : sortedNodes_) {
    double px = pxo + xts;

    for (auto node2 : sortedNodes_) {
      int value = node1->nodeValue(node2);

      bool empty = (node1 != node2 && ! value);

      if (empty) {
        QColor pc = bc.lighter(120);

        p->setPen  (pc);
        p->setBrush(bc);

        p->drawRect(QRectF(px, py, pxs, pys));
      }

      px += pxs;
    }

    py += pys;
  }

  setInsideObj(nullptr);
}

void
CQChartsAdjacencyPlot::
drawForeground(QPainter *p)
{
  if (insideObj())
    insideObj()->draw(p, CQChartsPlot::Layer::FG);
}

QColor
CQChartsAdjacencyPlot::
groupColor(int group) const
{
  return interpPaletteColor((1.0*group)/maxGroup_, Qt::white);
}

//------

CQChartsAdjacencyObj::
CQChartsAdjacencyObj(CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                     CQChartsAdjacencyNode *node2, int value, const CBBox2D &rect) :
 CQChartsPlotObj(rect), plot_(plot), node1_(node1), node2_(node2), value_(value)
{
}

void
CQChartsAdjacencyObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  //int nn = plot_->numNodes();

  QColor bc = plot_->emptyCellColor();

  if (node1_ == node2_) {
    bc = plot_->groupColor(node1_->group());
  }
  else {
    QColor c1 = plot_->groupColor(node1_->group());
    QColor c2 = plot_->groupColor(node2_->group());

    double s = (1.0*plot_->maxValue() - value_)/plot_->maxValue();

    int r = (c1.red  () + c2.red  () + s*bc.red  ())/3;
    int g = (c1.green() + c2.green() + s*bc.green())/3;
    int b = (c1.blue () + c2.blue () + s*bc.blue ())/3;

    bc = QColor(r, g, b);
  }

  QColor pc = bc.lighter(120);

  if (isInside()) {
    bc = plot_->insideColor(bc);
    pc = CQChartsUtil::bwColor(bc);

    plot_->setInsideObj(this);
  }

  p->setPen  (pc);
  p->setBrush(bc);

  CBBox2D prect;

  plot_->windowToPixel(rect(), prect);

  p->drawRect(CQChartsUtil::toQRect(prect));
}

bool
CQChartsAdjacencyObj::
inside(const CPoint2D &p) const
{
  return rect().inside(p);
}
