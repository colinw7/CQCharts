#include <CQChartsAdjacencyPlot.h>
#include <CQChartsWindow.h>
#include <CQChartsUtil.h>
#include <CQRotatedText.h>
#include <CQUtil.h>
#include <CGradientPalette.h>
#include <CStrParse.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsAdjacencyPlot::
CQChartsAdjacencyPlot(CQChartsWindow *window, QAbstractItemModel *model) :
 CQChartsPlot(window, model)
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
}

void
CQChartsAdjacencyPlot::
updateRange()
{
  dataRange_.updateRange(0, 0);
  dataRange_.updateRange(1, 1);
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
    int id = CQChartsUtil::modelInteger(model_, i, nodeColumn_);

    int group = CQChartsUtil::modelInteger(model_, i, groupColumn_);

    QString connectionsStr = CQChartsUtil::modelString(model_, i, connectionsColumn_);

    QString name = CQChartsUtil::modelString(model_, i, nameColumn_);

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

    Node *node = new Node(id, name.toStdString(), group);

    nodes_[id] = node;
  }

  //---

  for (const auto &idConnections : idConnections_) {
    int                    id          = idConnections.first;
    const ConnectionsData &connections = idConnections.second;

    Node *node = nodes_[id];

    for (const auto &connection : connections.connections) {
      Node *node1 = nodes_[connection.node];

      node->addNode(node1, connection.count);
    }
  }

  //---

  sortNodes();
}

void
CQChartsAdjacencyPlot::
sortNodes()
{
  maxValue_ = 0;
  maxGroup_ = 0;

  sortedNodes_.clear();

  for (auto pnode : nodes_) {
    Node *node = pnode.second;

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

  int node;

  if (! CStrUtil::toInteger(str1, &node))
    return false;

  int count;

  if (! CStrUtil::toInteger(str2, &count))
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

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(p);

  //---

  drawNodes(p);
}

void
CQChartsAdjacencyPlot::
drawNodes(QPainter *p)
{
  double pw = windowToPixelWidth (1);
  double ph = windowToPixelHeight(1);

  int nn = sortedNodes_.size();

  int th = std::max(int(nn > 0 ? std::min(pw, ph)/nn : 0.0), 8);

  //---

  QFont f = window()->font();

  f.setPixelSize(th);

  p->setFont(f);

  QFontMetrics fm(f);

  ts_ = 0;

  for (auto node : sortedNodes_)
    ts_ = std::max(ts_, fm.width(node->name().c_str()));

  ts_ += 2;

  //---

  cs_ = (nn > 0 ? std::max(1.0*std::min(pw, ph) - ts_, 0.0)/nn : 0.0);

  //---

  double pxo, pyo;

  windowToPixel(0.0, 1.0, pxo, pyo);

  //---

  p->setPen(QColor(0,0,0));

  double py = pyo + ts_;

  for (auto node : sortedNodes_) {
    p->drawText(0, py + cs_ - fm.descent(), node->name().c_str());

    py += cs_;
  }

  double px = pxo + ts_;

  py = pyo + ts_;

  for (auto node : sortedNodes_) {
    CQRotatedText::drawRotatedText(p, px + cs_, py - 2, node->name().c_str(), -90, Qt::AlignLeft);

    px += cs_;
  }

  //---

  px = pxo + ts_;
  py = pyo + ts_;

  p->fillRect(QRectF(px, py, nn*cs_, nn*cs_), QColor(200,200,200));

  //---

  py = pyo + ts_;

  for (auto node1 : sortedNodes_) {
    double px = pxo + ts_;

    for (auto node2 : sortedNodes_) {
      int value = node1->nodeValue(node2);

      int grey = 0xEE;

      QColor bc = QColor(grey,grey,grey);

      if      (node1 == node2) {
        bc = groupColor(node1->group());
      }
      else if (value) {
        QColor c1 = groupColor(node1->group());
        QColor c2 = groupColor(node2->group());

        double s = (1.0*maxValue_ - value)/maxValue_;

        int r = (c1.red  () + c2.red  () + s*grey)/3;
        int g = (c1.green() + c2.green() + s*grey)/3;
        int b = (c1.blue () + c2.blue () + s*grey)/3;

        bc = QColor(r, g, b);
      }

      QColor pc = bc.lighter(120);

      p->setPen  (pc);
      p->setBrush(bc);

      p->drawRect(QRectF(px, py, cs_, cs_));

      px += cs_;
    }

    py += cs_;
  }
}

QColor
CQChartsAdjacencyPlot::
groupColor(int group) const
{
  if (palette()) {
    CRGBA rgba = palette()->getColor((1.0*group)/maxGroup_).rgba();

    return CQUtil::toQColor(rgba);
  }
  else
    return QColor(255,255,255);
}
