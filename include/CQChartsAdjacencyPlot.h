#ifndef CQChartsAdjacencyPlot_H
#define CQChartsAdjacencyPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsObjData.h>
#include <CQChartsPlotObj.h>
#include <CQChartsConnectionList.h>

//------

/*!
 * \brief Adjacency plot type
 * \ingroup Charts
 */
class CQChartsAdjacencyPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsAdjacencyPlotType();

  QString name() const override { return "adjacency"; }
  QString desc() const override { return "Adjacency"; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; } // TODO: value range key

  bool canProbe() const override { return false; }

  QString description() const override;

  //---

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

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
  CQChartsAdjacencyNode(int id, const QString &name, int group, const CQChartsModelIndex &ind) :
   id_(id), name_(name), group_(group), ind_(ind) {
  }

  int id() const { return id_; }

  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

  int group() const { return group_; }
  void setGroup(int group) { group_ = group; }

  const CQChartsModelIndex &ind() const { return ind_; }
  void setInd(const CQChartsModelIndex &ind) { ind_ = ind; }

  double value() const { return value_; }

  double maxValue() const { return maxValue_; }

  NodeMap nodes() const { return nodes_; }

  bool hasNode(CQChartsAdjacencyNode *node) const {
    return (nodes_.find(node->id()) != nodes_.end());
  }

  void addNode(CQChartsAdjacencyNode *node, double value) {
    nodes_[node->id()] = NodeValue(node, value);

    value_ += value;

    maxValue_ = std::max(maxValue_, value);
  }

  double nodeValue(CQChartsAdjacencyNode *node) const {
    if (node == this) return 1;

    auto p = nodes_.find(node->id());

    if (p == nodes_.end())
      return 0;

    return (*p).second.second;
  }

 private:
  int                id_       { 0 };   //!< id
  QString            name_;             //!< name
  int                group_    { 0 };   //!< group
  CQChartsModelIndex ind_;              //!< model index
  double             value_    { 0.0 }; //!< total connections
  double             maxValue_ { 0.0 }; //!< max connections to single node
  NodeMap            nodes_;            //!< connected nodes
};

//------

class CQChartsAdjacencyPlot;

/*!
 * \brief Adjacency Grid Cell object
 * \ingroup Charts
 *
 * node1->node2 with connections value
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

  double value() const { return value_; }

  void getObjSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

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
class CQChartsAdjacencyPlot : public CQChartsConnectionPlot,
 public CQChartsObjBackgroundFillData<CQChartsAdjacencyPlot>,
 public CQChartsObjShapeData         <CQChartsAdjacencyPlot>,
 public CQChartsObjTextData          <CQChartsAdjacencyPlot>,
 public CQChartsObjEmptyCellShapeData<CQChartsAdjacencyPlot> {
  Q_OBJECT

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
    COUNT /*! sort by value */
  };

 public:
  CQChartsAdjacencyPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsAdjacencyPlot();

  //----

  void clearNodes();

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

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  virtual CQChartsAdjacencyObj *createObj(CQChartsAdjacencyNode *node1,
                                          CQChartsAdjacencyNode *node2, double value,
                                          const CQChartsGeom::BBox &rect, const ColorInd &ig);

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
    CQChartsModelIndex ind;          //!< model index
    int                node  { -1 }; //!< node numbe
    QString            name;         //!< name
    int                group { -1 }; //!< group index
    Connections        connections;  //!< connections list
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

 private:
  bool getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connections) const;

  CQChartsAdjacencyNode *findNode(const QString &str) const;

  //---

  bool initHierObjs(PlotObjs &objs) const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsAddConnection(const QString &srcStr, double srcValue,
                                 const QString &destStr, double destValue) const;

  //---

  bool initLinkObjs      (PlotObjs &objs) const;
  bool initConnectionObjs(PlotObjs &objs) const;
  bool initTableObjs     (PlotObjs &objs) const;

  void createNameNodeObjs(PlotObjs &objs) const;

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

  // options
  SortType       sortType_     { SortType::GROUP }; //!< sort type
  CQChartsLength bgMargin_     { "2px" };           //!< background margin
  NodeMap        nodes_;                            //!< all nodes
  NameNodeMap    nameNodeMap_;                      //!< name node map
  double         factor_       { 1.0 };             //!< font factor
  AdjacencyObj*  insideObj_    { nullptr };         //!< last inside object
  NodeArray      sortedNodes_;                      //!< sorted nodes
  NodeData       nodeData_;                         //!< node data
};

#endif
