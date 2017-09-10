#include <CQChartsAdjacencyPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQRotatedText.h>
#include <CQUtil.h>
#include <CGradientPalette.h>
#include <CStrParse.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsAdjacencyPlot::
CQChartsAdjacencyPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  setMargins(0, 0, 0, 0);
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

  int n = model_->rowCount(QModelIndex());

  for (int i = 0; i < n; ++i) {
    bool ok1, ok2;

    int id    = CQChartsUtil::modelInteger(model_, i, nodeColumn_ , ok1);
    int group = CQChartsUtil::modelInteger(model_, i, groupColumn_, ok2);

    if (! ok1) id    = i;
    if (! ok2) group = i;

    bool ok3;

    QString connectionsStr = CQChartsUtil::modelString(model_, i, connectionsColumn_, ok3);

    bool ok4;

    QString name = CQChartsUtil::modelString(model_, i, nameColumn_, ok4);

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

  scale_ = 1.0/(nn + maxLen_);

  double tsize = maxLen_*scale_;

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
  CStrParse parse(str.toStdString());

  parse.skipSpace();

  if (! parse.isChar('{'))
    return false;

  parse.skipChar();

  while (! parse.isChar('}')) {
    parse.skipSpace();

    std::string str1;

    if (! parse.readBracedString(str1, /*includeBraces*/false))
      return false;

    ConnectionData connection;

    if (! decodeConnection(str1.c_str(), connection))
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
  CStrParse parse(str.toStdString());

  parse.skipSpace();

  std::string str1;

  if (! parse.readNonSpace(str1))
    return false;

  parse.skipSpace();

  std::string str2;

  if (! parse.readNonSpace(str2))
    return false;

  long node;

  if (! CQChartsUtil::toInt(str1.c_str(), node))
    return false;

  long count;

  if (! CQChartsUtil::toInt(str2.c_str(), count))
    return false;

  connection = ConnectionData(node, count);

  return true;
}

void
CQChartsAdjacencyPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  drawNodes(p);

  //---

  drawObjs(p);
}

void
CQChartsAdjacencyPlot::
drawNodes(QPainter *p)
{
  double pxo, pyo;

  windowToPixel(0.0, 1.0, pxo, pyo);

  double pxs = windowToPixelWidth (scale_);
  double pys = windowToPixelHeight(scale_);

  double xts = maxLen_*pxs;
  double yts = maxLen_*pys;

  //---

  int ts = std::min(pxs, pys);

  QFont f = view()->font();

  f.setPixelSize(ts);

  p->setFont(f);

  QFontMetricsF fm(f);

  //---

  // draw row labels
  p->setPen(textColor());

  double px = pxo;
  double py = pyo + yts;

  for (auto node : sortedNodes_) {
    p->drawText(px, py + pys - fm.descent(), node->name().c_str());

    py += pys;
  }

  // draw column labels
  px = pxo + xts;
  py = pyo + yts;

  for (auto node : sortedNodes_) {
    CQRotatedText::drawRotatedText(p, px + pxs/2 + (fm.ascent() - fm.descent())/2, py - 2,
                                   node->name().c_str(), -90, Qt::AlignLeft);

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
draw(QPainter *p)
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

  p->setPen  (pc);
  p->setBrush(bc);

  CBBox2D prect;

  plot_->windowToPixel(rect(), prect);

  p->drawRect(CQUtil::toQRect(prect));
}

bool
CQChartsAdjacencyObj::
inside(const CPoint2D &p) const
{
  return rect().inside(p);
}
