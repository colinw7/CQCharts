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

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//------

/*!
 * \brief node data
 * \ingroup Charts
 */
class CQChartsAdjacencyNode {
 public:
  using OptReal = CQChartsOptReal;

  //! edge data
  struct EdgeData {
    int     to { -1 };
    OptReal value;

    EdgeData() = default;

    EdgeData(int to, const OptReal &value) :
     to(to), value(value) {
    }
  };

  using Node       = CQChartsAdjacencyNode;
  using EdgeMap    = std::map<int, EdgeData>;
  using ModelIndex = CQChartsModelIndex;

 public:
  CQChartsAdjacencyNode(int id, const QString &name, int group) :
   id_(id), name_(name), group_(group) {
  }

  //! get unique id
  int id() const { return id_; }

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &label) { label_ = label; }

  //! get/set value
  bool hasValue() const { return value_.isSet(); }
  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r) { value_ = r; }

  //! get/set group
  int group() const { return group_; }
  void setGroup(int group) { group_ = group; }

  //! get/set depth
  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set model index (per id)
  ModelIndex ind(int id) const {
    auto p = idInd_.find(id);
    if (p == idInd_.end()) return ModelIndex();
    return (*p).second;
  }

  void setInd(int id, const ModelIndex &ind) { idInd_[id] = ind; }

  //! get total connected values
  double totalValue() const { return totalValue_; }

  //! get max connected value
  double maxValue() const { return maxValue_; }

  //! get connected edges
  const EdgeMap &edges() const { return edges_; }

  //! had connected node
  bool hasNode(Node *node) const {
    return (edges_.find(node->id()) != edges_.end());
  }

  //! add edge to another node
  void addEdge(Node *node, const OptReal &value=OptReal()) {
    edges_[node->id()] = EdgeData(node->id(), value);

    if (value.isSet()) {
      totalValue_ += value.real();

      updateMaxValue(value.real());
    }
  }

  // get connected node value
  double edgeValue(Node *node, double equalValue=0.0) const {
    double defValue = (node == this ? equalValue : 0.0);

    auto p = edges_.find(node->id());
    if (p == edges_.end()) return defValue;

    const EdgeData &edgeData = (*p).second;

    if (! edgeData.value.isSet())
      return defValue;

    return edgeData.value.real();
  }

  void setEdgeValue(Node *node, const OptReal &value) {
    auto p = edges_.find(node->id());
    if (p == edges_.end()) return;

    EdgeData &edgeData = (*p).second;

    if (edgeData.value.isSet())
      updateMaxValue(-edgeData.value.real());

    edgeData.value = value;

    if (value.isSet())
      updateMaxValue(value.real());
  }

  Node *parent() const { return parent_; }
  void setParent(Node *p) { parent_ = p; }

 private:
  void updateMaxValue(double value) {
    maxValue_ = std::max(maxValue_, value);
  }

 private:
  using IdInd = std::map<int, ModelIndex>;

  int     id_         { 0 };       //!< id
  QString name_;                   //!< name
  QString label_;                  //!< label
  int     group_      { 0 };       //!< group
  int     depth_      { -1 };      //!< depth
  bool    visible_    { true };    //!< is visible
  IdInd   idInd_;                  //!< model index per dest id
  OptReal value_;                  //!< total connections
  double  totalValue_ { 0.0 };     //!< max edge value
  double  maxValue_   { 0.0 };     //!< max edge value
  EdgeMap edges_;                  //!< connected edges
  Node*   parent_     { nullptr }; //!< parent node
};

//------

class CQChartsAdjacencyPlot;

/*!
 * \brief Adjacency Grid Cell object
 * \ingroup Charts
 *
 * node1->node2 with connections value
 */
class CQChartsAdjacencyCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using AdjacencyPlot = CQChartsAdjacencyPlot;
  using AdjacencyNode = CQChartsAdjacencyNode;

 public:
  CQChartsAdjacencyCellObj(const AdjacencyPlot *plot, AdjacencyNode *node1,
                           AdjacencyNode *node2, double value, const BBox &rect,
                           const ColorInd &ig);

  QString typeName() const override { return "cell"; }

  AdjacencyNode *node1() const { return node1_; }
  AdjacencyNode *node2() const { return node2_; }

  QString calcId() const override;

  QString calcTipId() const override;

  double value() const { return value_; }

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 private:
  const AdjacencyPlot* plot_  { nullptr }; //!< parent plot
  AdjacencyNode*       node1_ { nullptr }; //!< row node
  AdjacencyNode*       node2_ { nullptr }; //!< column node
  double               value_ { 0.0 };     //!< connections value
};

//---

CQCHARTS_NAMED_SHAPE_DATA(EmptyCell, emptyCell)

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
  Q_PROPERTY(SortType       sortType      READ sortType        WRITE setSortType     )
  Q_PROPERTY(bool           forceDiagonal READ isForceDiagonal WRITE setForceDiagonal)
  Q_PROPERTY(CQChartsLength bgMargin      READ bgMargin        WRITE setBgMargin     )

  // background
  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(Background, background)

  // cell style
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // empty cell style
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(EmptyCell, emptyCell)

  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(SortType)

 public:
  enum class SortType {
    GROUP /*! sort by group */,
    NAME  /*! sort by name  */,
    COUNT /*! sort by value */
  };

  using AdjacencyNode = CQChartsAdjacencyNode;
  using CellObj       = CQChartsAdjacencyCellObj;

 public:
  CQChartsAdjacencyPlot(View *view, const ModelP &model);
 ~CQChartsAdjacencyPlot();

  //---

  void init() override;
  void term() override;

  //---

  void clearNodes();

  //---

  // options
  const SortType &sortType() const { return sortType_; }
  void setSortType(const SortType &v);

  bool isForceDiagonal() const { return forceDiagonal_; }
  void setForceDiagonal(bool b);

  const Length &bgMargin() const { return bgMargin_; }
  void setBgMargin(const Length &r);

  //---

  CellObj *insideObj() const { return insideObj_; }
  void setInsideObj(CellObj *obj) { insideObj_ = obj; }

  //---

  double maxValue() const { return nodeData_.maxValue; }
  int    maxGroup() const { return nodeData_.maxGroup; }
  int    maxNode () const { return nodeData_.maxNode ; }
  int    maxLen  () const { return nodeData_.maxLen  ; }

  double scale() const { return nodeData_.scale; }

  double drawFactor() const { return nodeData_.drawFactor; }

  //---

  int numVisibleNodes() const { return sortedNodes_.size(); }

  int numNodes() const { return nodes_.size(); }

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  virtual CellObj *createCellObj(AdjacencyNode *node1, AdjacencyNode *node2,
                                 double value, const BBox &rect, const ColorInd &ig);

  //---

  QColor interpGroupColor(int) const;

  //---

  void initFactor();

  void autoFit() override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *) const override;

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

 private:
  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    ModelIndex  ind;          //!< model index
    int         node  { -1 }; //!< node number
    QString     name;         //!< name
    int         group { -1 }; //!< group index
    Connections connections;  //!< connections list
  };

  using IdConnectionsData = std::map<int, ConnectionsData>;

 private:
  bool getRowConnections(const ModelVisitor::VisitData &data, ConnectionsData &connections) const;

  AdjacencyNode *findNode(const QString &str) const;

  //---

  bool initHierObjs() const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsAddConnection(const QString &srcStr , double srcValue ,
                                 const QString &destStr, double destValue) const;

  //---

  bool initPathObjs() const;

  void addPathValue(const QStringList &pathStrs, double value) const override;

  void propagatePathValues();

  //---

  bool initFromToObjs() const;

  void addFromToValue(const QString &fromStr, const QString &toStr, double value,
                      const FromToData &fromToData) const override;

  //---

  bool initLinkObjs() const;

  void addLinkConnection(const LinkConnectionData &linkConnectionData) const override;

  //---

  bool initTableObjs() const;

  bool initConnectionObjs(PlotObjs &objs) const;

  void filterObjs();

  //---

  void createNameNodeObjs(PlotObjs &objs) const;

 private:
  using NodeMap   = std::map<int, AdjacencyNode*>;
  using NodeArray = std::vector<AdjacencyNode*>;

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
  using NameNodeMap = std::map<QString, AdjacencyNode *>;

  // options
  SortType    sortType_      { SortType::GROUP }; //!< sort type
  bool        forceDiagonal_ { false };           //!< force diagonal
  Length      bgMargin_      { "2px" };           //!< background margin
  NodeMap     nodes_;                             //!< all nodes
  NameNodeMap nameNodeMap_;                       //!< name node map
  double      factor_        { -1.0 };            //!< font factor
  CellObj*    insideObj_     { nullptr };         //!< last inside object
  NodeArray   sortedNodes_;                       //!< sorted nodes
  NodeData    nodeData_;                          //!< node data
  int         maxNodeDepth_  { -1 };
};

#endif
