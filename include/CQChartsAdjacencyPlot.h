#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsObjData.h>
#include <CQChartsPlotObj.h>
#include <CQChartsConnectionList.h>

//------

/*!
 * \brief Adjacency plot type
 * \ingroup Charts
 */
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

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//------

/*!
 * \brief node data
 * \ingroup Charts
 */
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
  int         id_       { 0 };   //!< id
  QString     name_;             //!< name
  int         group_    { 0 };   //!< group
  QModelIndex ind_;              //!< model index
  double      count_    { 0.0 }; //!< total connections
  double      maxCount_ { 0.0 }; //!< max connections to single node
  NodeMap     nodes_;            //!< connected nodes
};

//------

class CQChartsAdjacencyPlot;

/*!
 * \brief Adjacency Grid Cell object
 * \ingroup Charts
 *
 * node1->node2 with connections count
 */
class CQChartsAdjacencyObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsAdjacencyObj(const CQChartsAdjacencyPlot *plot, CQChartsAdjacencyNode *node1,
                       CQChartsAdjacencyNode *node2, double value, const CQChartsGeom::BBox &rect,
                       const ColorInd &ig);

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 private:
  const CQChartsAdjacencyPlot* plot_  { nullptr }; //!< parent plot
  CQChartsAdjacencyNode*       node1_ { nullptr }; //!< row node
  CQChartsAdjacencyNode*       node2_ { nullptr }; //!< column node
  double                       value_ { 0 };       //!< connections value
};

//---

CQCHARTS_NAMED_SHAPE_DATA(EmptyCell,emptyCell)

/*!
 * \brief Connectivity Plot
 * \ingroup Charts
 *
 * made up of:
 *   + nodes             : number, name(opt), group(opt)
 *   + connections       : node->node
 *   + grid              : background color
 *   + row/column labels : text color, font
 *   + cell              : margin, corner size
 *   + empty cell        : color
 *
 * \todo use box obj for box config
 *
 * Plot Type
 *   + \ref CQChartsAdjacencyPlotType
 *
 * Example
 *   + \image html adjacency.png
 */
class CQChartsAdjacencyPlot : public CQChartsPlot,
 public CQChartsObjBackgroundFillData<CQChartsAdjacencyPlot>,
 public CQChartsObjShapeData         <CQChartsAdjacencyPlot>,
 public CQChartsObjTextData          <CQChartsAdjacencyPlot>,
 public CQChartsObjEmptyCellShapeData<CQChartsAdjacencyPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nodeColumn        READ nodeColumn        WRITE setNodeColumn       )
  Q_PROPERTY(CQChartsColumn connectionsColumn READ connectionsColumn WRITE setConnectionsColumn)
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )

  Q_PROPERTY(CQChartsColumn namePairColumn READ namePairColumn WRITE setNamePairColumn)
  Q_PROPERTY(CQChartsColumn countColumn    READ countColumn    WRITE setCountColumn   )

  Q_PROPERTY(CQChartsColumn groupIdColumn READ groupIdColumn WRITE setGroupIdColumn)

  // options
  Q_PROPERTY(SortType       sortType READ sortType WRITE setSortType)
  Q_PROPERTY(CQChartsLength bgMargin READ bgMargin WRITE setBgMargin)

  // background
  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(Background,background)

  // cell style
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // empty cell style
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(EmptyCell,emptyCell)

  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(SortType)

 public:
  enum class SortType {
    GROUP /*! sort by group */,
    NAME  /*! sort by name  */,
    COUNT /*! sort by count */
  };

 public:
  CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsAdjacencyPlot();

  //----

  void clearNodes();

  //----

  // columns
  //  . node connections (node id, connection list, name)
  const CQChartsColumn &nodeColumn() const { return nodeColumn_; }
  void setNodeColumn(const CQChartsColumn &c);

  const CQChartsColumn &connectionsColumn() const { return connectionsColumn_; }
  void setConnectionsColumn(const CQChartsColumn &c);

  const CQChartsColumn &namePairColumn() const { return namePairColumn_; }
  void setNamePairColumn(const CQChartsColumn &c);

  const CQChartsColumn &countColumn() const { return countColumn_; }
  void setCountColumn(const CQChartsColumn &c);

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &groupIdColumn() const { return groupIdColumn_; }
  void setGroupIdColumn(const CQChartsColumn &c);

  //----

  // options
  const SortType &sortType() const { return sortType_; }
  void setSortType(const SortType &v);

  const CQChartsLength &bgMargin() const { return bgMargin_; }
  void setBgMargin(const CQChartsLength &r);

  //---

  CQChartsAdjacencyObj *insideObj() const { return insideObj_; }
  void setInsideObj(CQChartsAdjacencyObj *obj) { insideObj_ = obj; }

  //---

  double maxValue() const { return nodeData_.maxValue; }
  int    maxGroup() const { return nodeData_.maxGroup; }
  int    maxNode () const { return nodeData_.maxNode ; }
  int    maxLen  () const { return nodeData_.maxLen  ; }

  double scale() const { return nodeData_.scale; }

  double drawFactor() const { return nodeData_.drawFactor; }

  //---

  int numNodes() const { return sortedNodes_.size(); }

  //---

  const ColumnType &namePairColumnType() const { return namePairColumnType_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  QColor interpGroupColor(int) const;

  //---

  void autoFit() override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(CQChartsPaintDevice *) const override;

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *) const override;

 private:
  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    QModelIndex ind;
    int         node;
    QString     name;
    int         group;
    Connections connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

 private:
  bool getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connections) const;

  bool decodeConnections(const QString &str, Connections &connections) const;

  CQChartsAdjacencyNode *getNodeByName(const QString &str) const;

  bool initHierObjs(PlotObjs &objs) const;
  bool initConnectionObjs(PlotObjs &objs) const;

 private:
  using NodeMap   = std::map<int,CQChartsAdjacencyNode*>;
  using NodeArray = std::vector<CQChartsAdjacencyNode*>;

  struct NodeData {
    double maxValue   { 0 };   //!< max node value
    int    maxGroup   { 0 };   //!< max node group
    int    maxNode    { 0 };   //!< max node ind
    int    maxLen     { 0 };   //!< max text length
    double scale      { 1.0 }; //!< box size
    double drawFactor { 1.0 }; //!< saved font factor
  };

 private:
  void sortNodes(const NodeMap &nodes, NodeArray &sortedNodes, NodeData &nodeData) const;

 private:
  using NameNodeMap = std::map<QString,CQChartsAdjacencyNode *>;

  using AdjacencyObj = CQChartsAdjacencyObj;

  // node, connections columns
  CQChartsColumn nodeColumn_;                                 //!< connection node column
  CQChartsColumn connectionsColumn_;                          //!< connections column

  // name, name pairs columns
  CQChartsColumn nameColumn_;                                 //!< name column
  CQChartsColumn namePairColumn_;                             //!< name pairs column
  CQChartsColumn countColumn_;                                //!< count column

  CQChartsColumn groupIdColumn_;                              //!< group id column
  SortType       sortType_              { SortType::GROUP };  //!< sort type
  CQChartsLength bgMargin_              { "2px" };            //!< background margin
  NodeMap        nodes_;                                      //!< all nodes
  NameNodeMap    nameNodeMap_;                                //!< name node map
  double         factor_                { 1.0 };              //!< font factor
  AdjacencyObj*  insideObj_             { nullptr };          //!< last inside object
  ColumnType     connectionsColumnType_ { ColumnType::NONE }; //!< connection column type
  ColumnType     namePairColumnType_    { ColumnType::NONE }; //!< name pair column type
  NodeArray      sortedNodes_;                                //!< sorted nodes
  NodeData       nodeData_;                                   //!< node data
};

#endif
