#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

//------

class CQChartsAdjacencyNode {
 public:
  typedef std::pair<CQChartsAdjacencyNode *,int> NodeValue;
  typedef std::map<int,NodeValue>                NodeMap;

 public:
  CQChartsAdjacencyNode(int id, const std::string &name, int group) :
   id_(id), name_(name), group_(group) {
  }

  int                id   () const { return id_   ; }
  const std::string &name () const { return name_ ; }
  int                group() const { return group_; }

  int count() const { return count_; }

  int maxCount() const { return maxCount_; }

  NodeMap nodes() const { return nodes_; }

  void addNode(CQChartsAdjacencyNode *node, int count) {
    nodes_[node->id()] = NodeValue(node, count);

    count_ += count;

    maxCount_ = std::max(maxCount_, count);
  }

  int nodeValue(CQChartsAdjacencyNode *node) const {
    if (node == this) return 1;

    NodeMap::const_iterator p = nodes_.find(node->id());

    if (p == nodes_.end())
      return 0;

    return (*p).second.second;
  }

 private:
  int         id_ { 0 };
  std::string name_;
  int         group_ { 0 };
  int         count_ { 0 };
  int         maxCount_ { 0 };
  NodeMap     nodes_;
};

//------

class CQChartsAdjacencyPlot;

class CQChartsAdjacencyObj : public CQChartsPlotObj {
 public:
  CQChartsAdjacencyObj(CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                       CQChartsAdjacencyNode *node2, int value, const CBBox2D &rect);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsAdjacencyPlot *plot_  { nullptr };
  CQChartsAdjacencyNode *node1_ { nullptr };
  CQChartsAdjacencyNode *node2_ { nullptr };
  int                    value_ { 0 };
};

//---

class CQChartsAdjacencyPlotType : public CQChartsPlotType {
 public:
  CQChartsAdjacencyPlotType();

  QString name() const override { return "adjacency"; }
  QString desc() const override { return "Adjacency"; }
};

//---

class CQChartsAdjacencyPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(int    connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(int    groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(int    nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(QColor bgColor           READ bgColor           WRITE setBgColor          )
  Q_PROPERTY(QColor textColor         READ textColor         WRITE setTextColor        )
  Q_PROPERTY(QColor emptyCellColor    READ emptyCellColor    WRITE setEmptyCellColor   )

 public:
  CQChartsAdjacencyPlot(CQChartsView *view, QAbstractItemModel *model);

  int nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(int i) { nodeColumn_ = i; update(); }

  int groupColumn() const { return groupColumn_; }
  void setGroupColumn(int i) { groupColumn_ = i; update(); }

  int connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(int i) { connectionsColumn_ = i; update(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; }

  const QColor &textColor() const { return textColor_; }
  void setTextColor(const QColor &c) { textColor_ = c; }

  const QColor &emptyCellColor() const { return emptyCellColor_; }
  void setEmptyCellColor(const QColor &c) { emptyCellColor_ = c; }

  int maxValue() const { return maxValue_; }

  int numNodes() const { return sortedNodes_.size(); }

  //---

  void addProperties();

  void updateRange();

  void initObjs(bool force=false);

  QColor groupColor(int) const;

  //---

  void draw(QPainter *) override;

  void drawBackground(QPainter *) override;

 private:
  struct ConnectionData {
    int node;
    int count;

    ConnectionData(int node=-1, int count=-1) :
     node(node), count(count) {
    }
  };

  typedef std::vector<ConnectionData> ConnectionDataArray;

  struct ConnectionsData {
    int                 node;
    QString             name;
    int                 group;
    ConnectionDataArray connections;
  };

  typedef std::map<int,ConnectionsData> IdConnectionsData;

 private:
  bool decodeConnections(const QString &str, ConnectionDataArray &connections);
  bool decodeConnection(const QString &pointStr, ConnectionData &connection);

  void sortNodes();

 private:
  typedef std::map<int,CQChartsAdjacencyNode *> NodeMap;
  typedef std::vector<CQChartsAdjacencyNode *>  NodeArray;

  int               nodeColumn_        { 0 };
  int               connectionsColumn_ { 1 };
  int               nameColumn_        { -1 };
  int               groupColumn_       { -1 };
  QColor            bgColor_           { 200, 200, 200 };
  QColor            textColor_         { 0, 0, 0 };
  QColor            emptyCellColor_    { 238, 238, 238 };
  IdConnectionsData idConnections_;
  NodeMap           nodes_;
  NodeArray         sortedNodes_;
  int               maxValue_ { 0 };
  int               maxGroup_ { 0 };
  int               maxLen_   { 0 };
  double            scale_    { 1 };
};

#endif
