#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxObj.h>

//------

// adjacency plot type
class CQChartsAdjacencyPlotType : public CQChartsPlotType {
 public:
  CQChartsAdjacencyPlotType();

  QString name() const override { return "adjacency"; }
  QString desc() const override { return "Adjacency"; }

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; } // TODO: value range key

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

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

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

 private:
  CQChartsAdjacencyPlot *plot_  { nullptr }; // parent plot
  CQChartsAdjacencyNode *node1_ { nullptr }; // row node
  CQChartsAdjacencyNode *node2_ { nullptr }; // column node
  double                 value_ { 0 };       // connections value
};

//---

// connectivity plot:
//   nodes             : number, name(opt), group(opt)
//   connections       : node->node
//   grid              : background color
//   row/column labels : text color, font
//   cell              : margin, corner size
//   empty cell        : color
//
// TODO: use box obj for box config
class CQChartsAdjacencyPlot : public CQChartsPlot,
 public CQChartsPlotShapeData<CQChartsAdjacencyPlot>,
 public CQChartsPlotTextData <CQChartsAdjacencyPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(CQChartsColumn namePairColumn    READ namePairColumn    WRITE setNamePairColumn   )
  Q_PROPERTY(CQChartsColumn countColumn       READ countColumn       WRITE setCountColumn      )
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(CQChartsColumn groupColumn       READ groupColumn       WRITE setGroupColumn      )

  // options
  Q_PROPERTY(SortType      sortType READ sortType WRITE setSortType)
  Q_PROPERTY(CQChartsColor bgColor  READ bgColor  WRITE setBgColor )
  Q_PROPERTY(double        margin   READ margin   WRITE setMargin  )

  // cell style
  CQCHARTS_SHAPE_DATA_PROPERTIES

  Q_PROPERTY(CQChartsLength cornerSize READ cornerSize WRITE setCornerSize)

  // empty cell style
  Q_PROPERTY(CQChartsColor  emptyCellColor      READ emptyCellColor      WRITE setEmptyCellColor)
  Q_PROPERTY(CQChartsLength emptyCellCornerSize READ emptyCellCornerSize
                                                WRITE setEmptyCellCornerSize )

  CQCHARTS_TEXT_DATA_PROPERTIES

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

  // columns
  const CQChartsColumn &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const CQChartsColumn &c);

  const CQChartsColumn &namePairColumn() const { return namePairColumn_; }
  void setNamePairColumn(const CQChartsColumn &c);

  const CQChartsColumn &countColumn() const { return countColumn_; }
  void setCountColumn(const CQChartsColumn &c);

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const CQChartsColumn &c);

  //---

  const SortType &sortType() const { return sortType_; }
  void setSortType(const SortType &v);

  //---

  // cell background
  const CQChartsColor &bgColor() const { return bgBox_.backgroundColor(); }
  void setBgColor(const CQChartsColor &c);

  QColor interpBgColor(int i, int n) const;

  const CQChartsLength &cornerSize() const { return cornerSize_; }
  void setCornerSize(const CQChartsLength &s);

  //---

  // empty cell background
  const CQChartsColor &emptyCellColor() const { return emptyCellBox_.backgroundColor(); }
  void setEmptyCellColor(const CQChartsColor &s);

  QColor interpEmptyCellColor(int i, int n) const;

  const CQChartsLength &emptyCellCornerSize() const { return emptyCellBox_.cornerSize(); }
  void setEmptyCellCornerSize(const CQChartsLength &s);

  //---

  double margin() const { return bgBox_.margin(); }
  void setMargin(double r);

  //---

  CQChartsAdjacencyObj *insideObj() const { return insideObj_; }
  void setInsideObj(CQChartsAdjacencyObj *obj) { insideObj_ = obj; }

  double maxValue() const { return maxValue_; }

  int numNodes() const { return sortedNodes_.size(); }

  //---

  void addProperties() override;

  void calcRange() override;

  bool initObjs() override;

  //---

  QColor interpGroupColor(int) const;

  //---

  void autoFit() override;

  //---

  void handleResize() override;

  //---

  void drawBackground(QPainter *) override;

  bool hasForeground() const override;

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

  CQChartsColumn        connectionsColumn_;                     // connections column
  CQChartsColumn        namePairColumn_;                        // name pairs column
  CQChartsColumn        countColumn_;                           // count column
  CQChartsColumn        groupColumn_;                           // group column
  CQChartsColumn        nameColumn_;                            // name column
  SortType              sortType_          { SortType::GROUP }; // sort type
  CQChartsBoxObj        bgBox_;                                 // background box data
  CQChartsLength        cornerSize_        { "0px" };           // cell box corner size
  CQChartsBoxObj        emptyCellBox_;                          // empty cell box data
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
