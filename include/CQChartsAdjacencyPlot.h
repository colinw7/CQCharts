#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

//------

// node data
class CQChartsAdjacencyNode {
 public:
  using NodeValue = std::pair<CQChartsAdjacencyNode*,int>;
  using NodeMap   = std::map<int,NodeValue>;

 public:
  CQChartsAdjacencyNode(int id, const std::string &name, int group, const QModelIndex &ind) :
   id_(id), name_(name), group_(group), ind_(ind) {
  }

  int                id   () const { return id_   ; }
  const std::string &name () const { return name_ ; }
  int                group() const { return group_; }
  const QModelIndex &ind  () const { return ind_  ; }

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

    auto p = nodes_.find(node->id());

    if (p == nodes_.end())
      return 0;

    return (*p).second.second;
  }

 private:
  int         id_       { 0 }; // id
  std::string name_;           // name
  int         group_    { 0 }; // group
  QModelIndex ind_;            // model index
  int         count_    { 0 }; // total connections
  int         maxCount_ { 0 }; // max connections to single node
  NodeMap     nodes_;          // connected nodes
};

//------

class CQChartsAdjacencyPlot;

// grid cell object
//  node1->node2 with connections count
class CQChartsAdjacencyObj : public CQChartsPlotObj {
 public:
  CQChartsAdjacencyObj(CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                       CQChartsAdjacencyNode *node2, int value, const CQChartsGeom::BBox &rect);

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsAdjacencyPlot *plot_  { nullptr };
  CQChartsAdjacencyNode *node1_ { nullptr }; // row node
  CQChartsAdjacencyNode *node2_ { nullptr }; // column node
  int                    value_ { 0 };       // connections value
};

//---

// connectivity plot type
class CQChartsAdjacencyPlotType : public CQChartsPlotType {
 public:
  CQChartsAdjacencyPlotType();

  QString name() const override { return "adjacency"; }
  QString desc() const override { return "Adjacency"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// connectivity plot:
//   nodes             : number, name(opt), group(opt)
//   connections       : node->node
//   grid cell         : background color, empty cell color
//   row/column labels : text color, font
class CQChartsAdjacencyPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(int    connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(int    groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(int    nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(QColor bgColor           READ bgColor           WRITE setBgColor          )
  Q_PROPERTY(QColor textColor         READ textColor         WRITE setTextColor        )
  Q_PROPERTY(QColor emptyCellColor    READ emptyCellColor    WRITE setEmptyCellColor   )
  Q_PROPERTY(QFont  font              READ font              WRITE setFont             )

 public:
  CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model);

  int nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(int i) { nodeColumn_ = i; update(); }

  int groupColumn() const { return groupColumn_; }
  void setGroupColumn(int i) { groupColumn_ = i; update(); }

  int connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(int i) { connectionsColumn_ = i; update(); }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  const QColor &bgColor() const { return bgColor_; }
  void setBgColor(const QColor &c) { bgColor_ = c; update(); }

  const QColor &textColor() const { return textColor_; }
  void setTextColor(const QColor &c) { textColor_ = c; update(); }

  const QColor &emptyCellColor() const { return emptyCellColor_; }
  void setEmptyCellColor(const QColor &c) { emptyCellColor_ = c; update(); }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; update(); }

  //---

  CQChartsAdjacencyObj *insideObj() const { return insideObj_; }
  void setInsideObj(CQChartsAdjacencyObj *obj) { insideObj_ = obj; }

  int maxValue() const { return maxValue_; }

  int numNodes() const { return sortedNodes_.size(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  QColor groupColor(int) const;

  //---

  void autoFit() override;

  //---

  void draw(QPainter *) override;

  void drawBackground(QPainter *) override;
  void drawForeground(QPainter *) override;

 private:
  struct ConnectionData {
    int node;
    int count;

    ConnectionData(int node=-1, int count=-1) :
     node(node), count(count) {
    }
  };

  using ConnectionDataArray = std::vector<ConnectionData>;

  struct ConnectionsData {
    QModelIndex         ind;
    int                 node;
    QString             name;
    int                 group;
    ConnectionDataArray connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

 private:
  bool decodeConnections(const QString &str, ConnectionDataArray &connections);
  bool decodeConnection(const QString &pointStr, ConnectionData &connection);

  void sortNodes();

 private:
  using NodeMap   = std::map<int,CQChartsAdjacencyNode*>;
  using NodeArray = std::vector<CQChartsAdjacencyNode*>;

  int                   nodeColumn_        { 0 };
  int                   connectionsColumn_ { 1 };
  int                   nameColumn_        { -1 };
  int                   groupColumn_       { -1 };
  QColor                bgColor_           { 200, 200, 200 };
  QColor                textColor_         { 0, 0, 0 };
  QColor                emptyCellColor_    { 238, 238, 238 };
  QFont                 font_;
  IdConnectionsData     idConnections_;
  NodeMap               nodes_;
  NodeArray             sortedNodes_;
  CQChartsAdjacencyObj* insideObj_         { nullptr };
  int                   maxValue_          { 0 };
  int                   maxGroup_          { 0 };
  int                   maxLen_            { 0 };
  double                scale_             { 1.0 };
  double                factor_            { 1.0 };
  double                drawFactor_        { 1.0 };
};

#endif
