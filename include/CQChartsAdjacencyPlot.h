#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxObj.h>

//------

// node data
class CQChartsAdjacencyNode {
 public:
  using NodeValue = std::pair<CQChartsAdjacencyNode*,double>;
  using NodeMap   = std::map<int,NodeValue>;

 public:
  CQChartsAdjacencyNode(int id, const QString &name, int group, const QModelIndex &ind) :
   id_(id), name_(name), group_(group), ind_(ind) {
  }

  int id() const { return id_; }

  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

  int group() const { return group_; }
  void setGroup(int group) { group_ = group; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &ind) { ind_ = ind; }

  double count() const { return count_; }

  double maxCount() const { return maxCount_; }

  NodeMap nodes() const { return nodes_; }

  void addNode(CQChartsAdjacencyNode *node, double count) {
    nodes_[node->id()] = NodeValue(node, count);

    count_ += count;

    maxCount_ = std::max(maxCount_, count);
  }

  double nodeValue(CQChartsAdjacencyNode *node) const {
    if (node == this) return 1;

    auto p = nodes_.find(node->id());

    if (p == nodes_.end())
      return 0;

    return (*p).second.second;
  }

 private:
  int         id_       { 0 };   // id
  QString     name_;             // name
  int         group_    { 0 };   // group
  QModelIndex ind_;              // model index
  double      count_    { 0.0 }; // total connections
  double      maxCount_ { 0.0 }; // max connections to single node
  NodeMap     nodes_;            // connected nodes
};

//------

class CQChartsAdjacencyPlot;

// grid cell object
//  node1->node2 with connections count
class CQChartsAdjacencyObj : public CQChartsPlotObj {
 public:
  CQChartsAdjacencyObj(CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                       CQChartsAdjacencyNode *node2, double value, const CQChartsGeom::BBox &rect);

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsAdjacencyPlot *plot_  { nullptr }; // parent plot
  CQChartsAdjacencyNode *node1_ { nullptr }; // row node
  CQChartsAdjacencyNode *node2_ { nullptr }; // column node
  double                 value_ { 0 };       // connections value
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
//   misc              : margin
//
// TODO: use box obj for box config
class CQChartsAdjacencyPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(CQChartsColumn valueColumn       READ valueColumn       WRITE setValueColumn      )
  Q_PROPERTY(CQChartsColumn groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(SortType       sortType          READ sortType          WRITE setSortType         )
  Q_PROPERTY(CQChartsColor  bgColor           READ bgColor           WRITE setBgColor          )
  Q_PROPERTY(CQChartsColor  borderColor       READ borderColor       WRITE setBorderColor      )
  Q_PROPERTY(double         borderAlpha       READ borderAlpha       WRITE setBorderAlpha      )
  Q_PROPERTY(CQChartsColor  emptyCellColor    READ emptyCellColor    WRITE setEmptyCellColor   )
  Q_PROPERTY(CQChartsLength cornerSize        READ cornerSize        WRITE setCornerSize       )
  Q_PROPERTY(CQChartsColor  textColor         READ textColor         WRITE setTextColor        )
  Q_PROPERTY(double         textAlpha         READ textAlpha         WRITE setTextAlpha        )
  Q_PROPERTY(QFont          font              READ font              WRITE setFont             )
  Q_PROPERTY(double         margin            READ margin            WRITE setMargin           )

  Q_ENUMS(SortType)

 public:
  enum class SortType {
    GROUP,
    NAME,
    COUNT
  };

 public:
  CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsAdjacencyPlot();

  //---

  const CQChartsColumn &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const CQChartsColumn &c) { nodeColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const CQChartsColumn &c) {
    connectionsColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c) { valueColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const CQChartsColumn &c) { groupColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c) { nameColumn_ = c; updateRangeAndObjs(); }

  //---

  const SortType &sortType() const { return sortType_; }
  void setSortType(const SortType &v) { sortType_ = v; updateRangeAndObjs(); }

  //---

  const CQChartsColor &bgColor() const { return bgBox_.backgroundColor(); }
  void setBgColor(const CQChartsColor &c) { bgBox_.setBackgroundColor(c); update(); }

  QColor interpBgColor(int i, int n) const { return bgBox_.interpBackgroundColor(i, n); }

  const CQChartsColor &borderColor() const { return cellBox_.borderColor(); }
  void setBorderColor(const CQChartsColor &c) { cellBox_.setBorderColor(c); update(); }

  QColor interpBorderColor(int i, int n) const { return cellBox_.interpBorderColor(i, n); }

  double borderAlpha() const { return cellBox_.borderAlpha(); }
  void setBorderAlpha(double r) { cellBox_.setBorderAlpha(r); update(); }

  const CQChartsColor &emptyCellColor() const { return emptyCellBox_.backgroundColor(); }
  void setEmptyCellColor(const CQChartsColor &s) { emptyCellBox_.setBackgroundColor(s); update(); }

  QColor interpEmptyCellColor(int i, int n) const {
    return emptyCellBox_.interpBackgroundColor(i, n); }

  const CQChartsLength &cornerSize() const { return cellBox_.cornerSize(); }
  void setCornerSize(const CQChartsLength &s) { cellBox_.setCornerSize(s); update(); }

  const CQChartsColor &textColor() const { return textData_.color; }
  void setTextColor(const CQChartsColor &c) { textData_.color = c; update(); }

  double textAlpha() const { return textData_.alpha; }
  void setTextAlpha(double a) { textData_.alpha = a; update(); }

  QColor interpTextColor(int i, int n) const { return textColor().interpColor(this, i, n); }

  const QFont &font() const { return textData_.font; }
  void setFont(const QFont &f) { textData_.font = f; update(); }

  double margin() const { return bgBox_.margin(); }
  void setMargin(double r) { bgBox_.setMargin(r); updateObjs(); }

  //---

  CQChartsAdjacencyObj *insideObj() const { return insideObj_; }
  void setInsideObj(CQChartsAdjacencyObj *obj) { insideObj_ = obj; }

  double maxValue() const { return maxValue_; }

  int numNodes() const { return sortedNodes_.size(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  QColor interpGroupColor(int) const;

  //---

  void autoFit() override;

  //---

  void handleResize() override;

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

  bool initHierObjs();
  bool initConnectionObjs();

  void sortNodes();

 private:
  using NodeMap   = std::map<int,CQChartsAdjacencyNode*>;
  using NodeArray = std::vector<CQChartsAdjacencyNode*>;

  CQChartsColumn        nodeColumn_;                            // node column
  CQChartsColumn        connectionsColumn_;                     // connections column
  CQChartsColumn        valueColumn_;                           // value column
  CQChartsColumn        groupColumn_;                           // group column
  CQChartsColumn        nameColumn_;                            // name column
  SortType              sortType_          { SortType::GROUP }; // sort type
  CQChartsBoxObj        bgBox_;                                 // background box data
  CQChartsBoxObj        cellBox_;                               // cell box data
  CQChartsBoxObj        emptyCellBox_;                          // empty cell box data
  CQChartsTextData      textData_;                              // text style data
  IdConnectionsData     idConnections_;                         // connections by id
  NodeMap               nodes_;                                 // all nodes
  NodeArray             sortedNodes_;                           // sorted nodes
  CQChartsAdjacencyObj* insideObj_         { nullptr };         // last inside object
  double                maxValue_          { 0 };               // max node value
  int                   maxGroup_          { 0 };               // max node group
  int                   maxLen_            { 0 };               // max text length
  double                scale_             { 1.0 };             // box size
  double                factor_            { 1.0 };             // font factor
  double                drawFactor_        { 1.0 };             // saved font factor
};

#endif