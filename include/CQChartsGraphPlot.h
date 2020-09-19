#ifndef CQChartsGraphPlot_H
#define CQChartsGraphPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsValueInd.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Graph plot type
 * \ingroup Charts
 */
class CQChartsGraphPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsGraphPlotType();

  QString name() const override { return "graph"; }
  QString desc() const override { return "Graph"; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsGraphPlot;
class CQChartsGraphPlotNode;
class CQChartsGraphPlotEdge;
class CQChartsGraphPlotGraph;
class CQChartsGraphNodeObj;
class CQChartsGraphEdgeObj;
class CQChartsGraphGraphObj;

/*!
 * \brief Graph plot node
 * \ingroup Charts
 */
class CQChartsGraphPlotNode {
 public:
  using Edges      = std::vector<CQChartsGraphPlotEdge *>;
  using NodeSet    = std::set<const CQChartsGraphPlotNode *>;
  using Plot       = CQChartsGraphPlot;
  using Graph      = CQChartsGraphPlotGraph;
  using Edge       = CQChartsGraphPlotEdge;
  using Node       = CQChartsGraphPlotNode;
  using Obj        = CQChartsGraphNodeObj;
  using OptReal    = CQChartsOptReal;
  using ModelIndex = CQChartsModelIndex;
  using BBox       = CQChartsGeom::BBox;
  using Point      = CQChartsGeom::Point;
  using ShapeType  = CQChartsBoxObj::ShapeType;

 public:
  CQChartsGraphPlotNode(const Plot *plot, const QString &str);

  virtual ~CQChartsGraphPlotNode();

  //! get plot
  const Plot *plot() const { return plot_; }

  //! get edge parent
  Node *parent() const { return parent_; }

  //! set string (name /)
  QString str() const { return str_; }

  //! get/set unique id
  int id() const { return id_; }
  void setId(int id) { id_ = id; }

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  //! get/set value
  bool hasValue() const { return value_.isSet(); }
  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r) { value_ = r; }

  //! get/set group
  int group() const { return group_; }
  void setGroup(int i) { group_ = i; }

  //! get/set graph
  int graphId() const { return graphId_; }
  void setGraphId(int i) { graphId_ = i; }

  //! get/set parent graph
  int parentGraphId() const { return parentGraphId_; }
  void setParentGraphId(int i) { parentGraphId_ = i; }

  //! get/set depth
  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set model index
  const ModelIndex &ind() const { return ind_; }
  void setInd(const ModelIndex &ind) { ind_ = ind; }

  //! get/set source edges
  const Edges &srcEdges() const { return srcEdges_ ; }
  void setSrcEdges(const Edges &edges) { srcEdges_  = edges; }

  //! get/set destination edges
  const Edges &destEdges() const { return destEdges_; }
  void setDestEdges(const Edges &edges) { destEdges_ = edges; }

  //! add source edge
  void addSrcEdge(Edge *edge, bool primary=true);

  //! add destination edge
  void addDestEdge(Edge *edge, bool primary=true);

  //! has destination edge
  bool hasDestNode(CQChartsGraphPlotNode *destNode) const;

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  //! get source depth (from connections)
  virtual int srcDepth () const;
  //! get destination depth (from connections)
  virtual int destDepth() const;

  //! get/set shape
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s) { shapeType_ = s; }

  //! get/set number of sides
  int numSides() const { return numSides_; }
  void setNumSides(int n) { numSides_ = n; }

  //! get/set color
  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  // get/set x pos
  int xpos() const { return xpos_; }
  void setXPos(int x) { xpos_ = x; }

  // get/set rect
  const BBox &rect() const;
  virtual void setRect(const BBox &r);

  //---

  // get graph (if any)
  Graph *graph() const;

  //---

  //! get edge (max) sum
  double edgeSum() const;

  //! get source edge (max) sum
  double srcEdgeSum () const;

  //! get destination edge (max) sum
  double destEdgeSum() const;

  //! is placed
  virtual bool isPlaced() const { return true; }

  //! place
  virtual void place() const { assert(false); }

  //! move node by delta
  virtual void moveBy(const Point &delta);

  //! scale node by factor
  virtual void scale(double fx, double fy);

 protected:
  //! calc src/destination depth
  int calcSrcDepth (NodeSet &visited) const;
  int calcDestDepth(NodeSet &visited) const;

 protected:
  const Plot* plot_          { nullptr };         //!< associated plot
  Node*       parent_        { nullptr };         //!< parent node
  QString     str_;                               //!< string
  int         id_            { -1 };              //!< id
  bool        visible_       { true };            //!< is visible
  ModelIndex  ind_;                               //!< model index
  QString     name_;                              //!< name
  QString     label_;                             //!< label
  OptReal     value_;                             //!< value
  int         group_         { -1 };              //!< group
  int         graphId_       { -1 };              //!< graph id
  int         parentGraphId_ { -1 };              //!< parent graph id
  int         depth_         { -1 };              //!< depth
  ShapeType   shapeType_     { ShapeType::NONE }; //!< shape type
  int         numSides_      { 4 };               //!< number of polygon sides
  QColor      color_;                             //!< fill color
  Edges       srcEdges_;                          //!< source edges
  Edges       destEdges_;                         //!< destination edges
  Edges       nonPrimaryEdges_;                   //!< non-primary edges
  int         srcDepth_      { -1 };              //!< source depth (calculated)
  int         destDepth_     { -1 };              //!< destination depth (calculated)
  int         xpos_          { -1 };              //!< x position
  BBox        rect_;                              //!< placed rectangle
  Obj*        obj_           { nullptr };         //!< node plot object
};

//---

/*!
 * \brief Graph plot edge
 * \ingroup Charts
 */
class CQChartsGraphPlotEdge {
 public:
  enum class ShapeType {
    NONE,
    ARC,
    ARROW
  };

  using Plot    = CQChartsGraphPlot;
  using Node    = CQChartsGraphPlotNode;
  using Obj     = CQChartsGraphEdgeObj;
  using OptReal = CQChartsOptReal;

 public:
  CQChartsGraphPlotEdge(const Plot *plot, const OptReal &value, Node *srcNode, Node *destNode);

  virtual ~CQChartsGraphPlotEdge();

  const Plot *plot() const { return plot_; }

  //! get/set unique id
  int id() const { return id_; }
  void setId(int id) { id_ = id; }

  //! get/set value
  bool hasValue() const { return value_.isSet(); }
  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r) { value_ = r; }

  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  // get source node
  Node *srcNode() const { return srcNode_; }

  // get destination node
  Node *destNode() const { return destNode_; }

  // is self connect
  bool isSelf() const { return srcNode() == destNode(); }

  //! get/set shape
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s) { shapeType_ = s; }

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

 protected:
  const Plot* plot_      { nullptr };         //!< plot
  int         id_        { -1 };              //!< unique id
  OptReal     value_;                         //!< value
  QString     label_;                         //!< label
  Node*       srcNode_   { nullptr };         //!< source node
  Node*       destNode_  { nullptr };         //!< destination node
  ShapeType   shapeType_ { ShapeType::NONE }; //!< shape type
  Obj*        obj_       { nullptr };         //!< edge node object
};

//---

/*!
 * \brief Graph Plot Graph (Edge/Node/Sub Graph container)
 * \ingroup Charts
 */
class CQChartsGraphPlotGraph : public CQChartsGraphPlotNode {
 public:
  using Node  = CQChartsGraphPlotNode;
  using Nodes = std::vector<Node *>;

  //! data for nodes at depth
  struct DepthData {
    Nodes  nodes;        //!< nodes at depth
    double size { 0.0 }; //!< total size
  };

  using DepthNodesMap = std::map<int, DepthData>;
  using IndNodeMap    = std::map<int, Node *>;
  using PosNodesMap   = std::map<int, Nodes>;
  using Graph         = CQChartsGraphPlotGraph;
  using Graphs        = std::vector<Graph *>;
  using Obj           = CQChartsGraphGraphObj;

 public:
  CQChartsGraphPlotGraph(const Plot *plot, const QString &str);

  virtual ~CQChartsGraphPlotGraph();

  Graph *parent() const { return parent_; }
  void setParent(Graph *graph) { parent_ = graph; }

  void addChild(Graph *graph);

  const Graphs &children() const { return children_; }

  const Nodes &nodes() const { return nodes_; }
  void addNode(Node *node);

  void removeAllNodes();

  //--

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  //---

  //! get/set min node depth
  int minNodeDepth() const { return minNodeDepth_; }
  void setMinNodeDepth(int i) { minNodeDepth_ = i; }

  //! get/set max node depth
  int maxNodeDepth() const { return maxNodeDepth_; }
  void setMaxNodeDepth(int i) { maxNodeDepth_ = i; }

  //! get/set max height
  int maxHeight() const { return maxHeight_; }
  void setMaxHeight(int i) { maxHeight_ = i; }

  //! get/set total size
  double totalSize() const { return totalSize_; }
  void setTotalSize(double r) { totalSize_ = r; }

  //! get depth nodes
  const DepthNodesMap &depthNodesMap() const { return depthNodesMap_; }

  //! add node at depth
  void addDepthNode(int depth, Node *node) { depthNodesMap_[depth].nodes.push_back(node); }

  //! add to depth size
  void addDepthSize(int depth, double size) { depthNodesMap_[depth].size += size; }

  //! get pos nodes
  const PosNodesMap &posNodesMap() const { return posNodesMap_; }

  //! clear pos nodes
  void resetPosNodes() { posNodesMap_.clear(); }

  //! add pos node
  void addPosNode(Node *node) { posNodesMap_[node->xpos()].push_back(node); }

  //! has nodes at pos
  bool hasPosNodes(int pos) const { return (posNodesMap_.find(pos) != posNodesMap_.end()); }

  //! get nodes at pos
  const Nodes &posNodes(int pos) const {
    auto p = posNodesMap_.find(pos); assert(p != posNodesMap_.end()); return (*p).second; }

  //---

  //! update rectangle from node
  void updateRect();

  //---

  //! get nodes to place
  Nodes placeNodes() const;

  //---

  //! get source depth (from external node edges)
  int srcDepth () const override;
  //! get destination depth (from external node edges)
  int destDepth() const override;

  //! set rect
  void setRect(const BBox &r) override;

  //---

  //! get/set value margin
  double valueMargin() const { return valueMargin_; }
  void setValueMargin(double s) { valueMargin_ = s; }

  //! get/set value scale
  double valueScale() const { return valueScale_; }
  void setValueScale(double s) { valueScale_ = s; }

  //---

  //! is placed
  bool isPlaced() const override { return placed_; }
  void setPlaced(bool b) { placed_ = b; }

  //! place
  void place() const override;

  //! move node by delta
  void moveBy(const Point &delta) override;

  //! scale node by factor
  void scale(double fx, double fy) override;

 protected:
  Graph*        parent_       { nullptr }; //!< parent graph
  Graphs        children_;                 //!< child graphs
  Nodes         nodes_;                    //!< nodes in graph
  bool          placed_       { false };   //!< is placed
  int           minNodeDepth_ { 0 };       //!< min depth of nodes
  int           maxNodeDepth_ { 0 };       //!< max depth of nodes
  int           maxHeight_    { 0 };       //!< max height of all depth nodes
  double        totalSize_    { 0.0 };     //!< total size of all depth nodes
  DepthNodesMap depthNodesMap_;            //!< nodes data at depth
  PosNodesMap   posNodesMap_;              //!< pos node map
  Obj*          obj_          { nullptr }; //!< graph plot object
  double        valueMargin_  { 0.0 };     //!< y value margin
  double        valueScale_   { 1.0 };     //!< y value scale
};

//---

/*!
 * \brief Graph Plot Node object
 * \ingroup Charts
 */
class CQChartsGraphNodeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString   hierName  READ hierName  WRITE setHierName )
  Q_PROPERTY(QString   name      READ name      WRITE setName     )
  Q_PROPERTY(double    value     READ value     WRITE setValue    )
  Q_PROPERTY(int       depth     READ depth     WRITE setDepth    )
  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)
  Q_PROPERTY(int       numSides  READ numSides  WRITE setNumSides )
  Q_PROPERTY(QColor    color     READ color     WRITE setColor    )

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE          = (int) CQChartsBoxObj::ShapeType::NONE,
    TRIANGLE      = (int) CQChartsBoxObj::ShapeType::TRIANGLE,
    DIAMOND       = (int) CQChartsBoxObj::ShapeType::DIAMOND,
    BOX           = (int) CQChartsBoxObj::ShapeType::BOX,
    POLYGON       = (int) CQChartsBoxObj::ShapeType::POLYGON,
    CIRCLE        = (int) CQChartsBoxObj::ShapeType::CIRCLE,
    DOUBLE_CIRCLE = (int) CQChartsBoxObj::ShapeType::DOUBLE_CIRCLE
  };

  using Plot = CQChartsGraphPlot;
  using Node = CQChartsGraphPlotNode;
  using Edge = CQChartsGraphPlotEdge;

 public:
  CQChartsGraphNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &ind);

  virtual ~CQChartsGraphNodeObj();

  Node *node() const { return node_; }
  void setNode(Node *node) { node_ = node; }

  const QString &hierName() const { return hierName_; }
  void setHierName(const QString &s) { hierName_ = s; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  //---

  const BBox &srcEdgeRect(Edge *edge) const {
    auto p = srcEdgeRect_.find(edge);
    assert(p != srcEdgeRect_.end());

    return (*p).second;
  }

  const BBox &destEdgeRect(Edge *edge) const {
    auto p = destEdgeRect_.find(edge);
    assert(p != destEdgeRect_.end());

    return (*p).second;
  }

  //---

  //! get/set shape type
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s) { shapeType_ = s; }

  //! get/set num side
  int numSides() const { return numSides_; }
  void setNumSides(int n) { numSides_ = n; }

  //! get/set color
  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  //---

  QString typeName() const override { return "node"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! move node by delta
  void moveBy(const Point &delta);

  //! scale node by factor
  void scale(double fx, double fy);

  //---

  void placeEdges();

  //---

  bool editPress  (const Point &p) override;
  bool editMove   (const Point &p) override;
  bool editMotion (const Point &p) override;
  bool editRelease(const Point &p) override;

  void setEditBBox(const BBox &bbox, const ResizeSide &) override;

  //---

  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  using EdgeRect = std::map<Edge *, BBox>;

  const Plot* plot_        { nullptr };         //!< parent plot
  Node*       node_        { nullptr };         //!< node
  EdgeRect    srcEdgeRect_;                     //!< edge to src
  EdgeRect    destEdgeRect_;                    //!< edge to dest
  QString     hierName_;                        //!< node hier name
  QString     name_;                            //!< node name
  double      value_       { 0.0 };             //!< node value
  int         depth_       { -1 };              //!< node depth
  ShapeType   shapeType_   { ShapeType::NONE }; //!< shape type
  int         numSides_    { 4 };               //!< num sides
  QColor      color_;                           //!< custom color
  bool        editChanged_ { false };           //!< edit is changed
};

//---

/*!
 * \brief Graph Plot Edge object
 * \ingroup Charts
 */
class CQChartsGraphEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE,
    ARC,
    ARROW
  };

  using Plot   = CQChartsGraphPlot;
  using Edge   = CQChartsGraphPlotEdge;
  using Node   = CQChartsGraphPlotNode;
  using Length = CQChartsLength;

 public:
  CQChartsGraphEdgeObj(const Plot *plot, const BBox &rect, Edge *edge);

  virtual ~CQChartsGraphEdgeObj();

  //! typename
  QString typeName() const override { return "edge"; }

  //! get edge
  Edge *edge() const { return edge_; }
  void setEdge(Edge *edge) { edge_ = edge; }

  //! get unique id
  QString calcId() const override;

  //! get tip string
  QString calcTipId() const override;

  //! get source/destination rect
  void setSrcRect (const BBox &rect) { srcRect_  = rect; }
  void setDestRect(const BBox &rect) { destRect_ = rect; }

  //! get/set shape type
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s) { shapeType_ = s; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! is point inside
  bool inside(const Point &p) const override;

  //! get connected objects
  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot*  plot_      { nullptr };        //!< parent plot
  Edge*        edge_      { nullptr };        //!< edge
  BBox         srcRect_;                      //!< src rect
  BBox         destRect_;                     //!< dest rect
  ShapeType    shapeType_ { ShapeType::ARC }; //!< shape type
  QPainterPath path_;                         //!< painter path
};

//---

/*!
 * \brief Graph Plot Graph object
 * \ingroup Charts
 */
class CQChartsGraphGraphObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot  = CQChartsGraphPlot;
  using Graph = CQChartsGraphPlotGraph;

 public:
  CQChartsGraphGraphObj(const Plot *plot, const BBox &rect, Graph *graph);

  virtual ~CQChartsGraphGraphObj();

  Graph *graph() const { return graph_; }
  void setGraph(Graph *graph) { graph_ = graph; }

  //---

  QString typeName() const override { return "graph"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! move node by delta
  void moveBy(const Point &delta);

  //! scale node by factor
  void scale(double fx, double fy);

  //---

  bool editPress  (const Point &p) override;
  bool editMove   (const Point &p) override;
  bool editMotion (const Point &p) override;
  bool editRelease(const Point &p) override;

  void setEditBBox(const BBox &bbox, const ResizeSide &) override;

  bool isEditResize() const override { return true; }

  //---

  void draw(PaintDevice *device) override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot* plot_        { nullptr }; //!< parent plot
  Graph*      graph_       { nullptr }; //!< node
  bool        editChanged_ { false };
};

//---

/*!
 * \brief Graph Plot
 * \ingroup Charts
 */
class CQChartsGraphPlot : public CQChartsConnectionPlot,
 public CQChartsObjTextData<CQChartsGraphPlot>,
 public CQChartsObjNodeShapeData <CQChartsGraphPlot>,
 public CQChartsObjEdgeShapeData <CQChartsGraphPlot>,
 public CQChartsObjGraphShapeData<CQChartsGraphPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double    nodeXMargin READ nodeXMargin   WRITE setNodeXMargin)
  Q_PROPERTY(double    nodeYMargin READ nodeYMargin   WRITE setNodeYMargin)
  Q_PROPERTY(double    nodeWidth   READ nodeWidth     WRITE setNodeWidth  )
  Q_PROPERTY(bool      nodeXScaled READ isNodeXScaled WRITE setNodeXScaled)
  Q_PROPERTY(NodeShape nodeShape   READ nodeShape     WRITE setNodeShape  )
  Q_PROPERTY(EdgeShape edgeShape   READ edgeShape     WRITE setEdgeShape  )
  Q_PROPERTY(bool      edgeScaled  READ isEdgeScaled  WRITE setEdgeScaled )

  // align
  Q_PROPERTY(Align align READ align WRITE setAlign)

  // placement
  Q_PROPERTY(bool adjustNodes      READ isAdjustNodes      WRITE setAdjustNodes     )
  Q_PROPERTY(bool autoCreateGraphs READ isAutoCreateGraphs WRITE setAutoCreateGraphs)

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Graph, graph)

  // text style
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(Align)
  Q_ENUMS(NodeShape)
  Q_ENUMS(EdgeShape)

 public:
  enum class Align {
    SRC,
    DEST,
    JUSTIFY
  };

  enum NodeShape {
    NODE_SHAPE_NONE,
    NODE_SHAPE_DIAMOND,
    NODE_SHAPE_BOX,
    NODE_SHAPE_POLYGON,
    NODE_SHAPE_CIRCLE,
    NODE_SHAPE_DOUBLE_CIRCLE
  };

  enum EdgeShape {
    EDGE_SHAPE_NONE,
    EDGE_SHAPE_ARC,
    EDGE_SHAPE_ARROW
  };

  using Node        = CQChartsGraphPlotNode;
  using Nodes       = std::vector<Node *>;
  using NameNodeMap = std::map<QString, Node *>;
  using IndNodeMap  = std::map<int, Node *>;
  using NodeSet     = std::set<Node *>;
  using Edge        = CQChartsGraphPlotEdge;
  using Edges       = std::vector<Edge *>;
  using NodeObj     = CQChartsGraphNodeObj;
  using EdgeObj     = CQChartsGraphEdgeObj;
  using Graph       = CQChartsGraphPlotGraph;
  using GraphObj    = CQChartsGraphGraphObj;
  using PosNodeMap  = std::map<double, Node *>;
  using PosEdgeMap  = std::map<double, Edge *>;

 public:
  CQChartsGraphPlot(View *view, const ModelP &model);
 ~CQChartsGraphPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set node x margin
  double nodeXMargin() const { return nodeXMargin_; }
  void setNodeXMargin(double r);

  //! get/set node y margin
  double nodeYMargin() const { return nodeYMargin_; }
  void setNodeYMargin(double r);

  //---

  //! get/set x width
  double nodeWidth() const { return nodeWidth_; }
  void setNodeWidth(double r);

  //! get/set is node x scaled
  bool isNodeXScaled() const { return nodeXScaled_; }
  void setNodeXScaled(bool b);

  //! get/set node shape
  const NodeShape &nodeShape() const { return nodeShape_; }
  void setNodeShape(const NodeShape &s);

  //---

  //! get/set edge shape
  const EdgeShape &edgeShape() const { return edgeShape_; }
  void setEdgeShape(const EdgeShape &s);

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeScaled_; }
  void setEdgeScaled(bool b);

  //---

  //! get/set text align
  const Align &align() const { return align_; }
  void setAlign(const Align &a);

  //---

  bool isAdjustNodes() const { return adjustNodes_; }
  void setAdjustNodes(bool b);

  bool isAutoCreateGraphs() const { return autoCreateGraphs_; }
  void setAutoCreateGraphs(bool b);

  //---

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

//Range getCalcDataRange() const override;

  Range objTreeRange() const override;

  BBox nodesBBox() const;

  //---

  bool createObjs(PlotObjs &objs) const override;

  void fitToBBox(const BBox &bbox);

  bool initHierObjs() const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsAddConnection(const QString &srcStr, const QString &destStr, int srcDepth,
                                 double value, Node* &srcNode, Node* &destNode) const;

  //---

  bool initPathObjs() const;

  void addPathValue(const PathData &pathData) const override;

  void propagatePathValues();

  //---

  bool initFromToObjs() const;

  void addFromToValue(const FromToData &fromToData) const override;

  //---

  bool initLinkObjs() const;

  void addLinkConnection(const LinkConnectionData &linkConnectionData) const override;

  //---

  bool initConnectionObjs() const;

  void addConnectionObj(int id, const ConnectionsData &connectionsData) const override;

  //---

  bool initTableObjs() const;

  void filterObjs();

  //---

  int numNodes() const { return nameNodeMap_.size(); }

  int maxNodeDepth() const { return maxNodeDepth_; }

  //---

  void keyPress(int key, int modifier) override;

  //---

  void placeGraphs() const;
  void placeGraph(Graph *graph) const;
  void placeGraphNodes(Graph *graph, const Nodes &nodes) const;

  //---

  void autoCreateGraphs() const;

  void addObjects(PlotObjs &objs) const;

 protected:
  void clearNodesAndEdges();

  Node *findNode(const QString &name) const;

  Node *createNode(const QString &name) const;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  void createObjsGraph(PlotObjs &objs) const;

  void createGraphs() const;

  Graph *getGraph(int group, int parentGroup=-1) const;

  void clearGraphs();

  void placeDepthNodes(Graph *graph) const;
  void placeDepthSubNodes(Graph *graph, int xpos, const Nodes &nodes) const;

  void calcValueMarginScale(Graph *graph);

  double calcNodeXMargin() const;
  double calcNodeYMargin() const;

  NodeObj *createObjFromNode(Graph *graph, Node *node) const;

  int calcXPos(Graph *graph, Node *node) const;

  EdgeObj *addEdgeObj(Edge *edge) const;

  //---

  void updateGraphMaxDepth(Graph *graph, const Nodes &nodes) const;

  bool adjustNodes() const;
  bool adjustGraphNodes(Graph *graph, const Nodes &nodes) const;

  void initPosNodesMap(Graph *graph, const Nodes &nodes) const;

  bool adjustNodeCenters(Graph *graph) const;

  bool removeOverlaps(Graph *graph) const;

  bool removePosOverlaps(Graph *graph, const Nodes &nodes) const;

  void spreadPosNodes(Graph *graph, const Nodes &nodes) const;

  bool reorderNodeEdges(Graph *graph, const Nodes &nodes) const;

  void createPosNodeMap(const Nodes &nodes, PosNodeMap &posNodeMap) const;
  void createPosEdgeMap(const Edges &edges, PosEdgeMap &posEdgeMap, bool isSrc) const;

  bool adjustNode(Node *node) const;

  void adjustGraphs() const;

  //---

  int minNodeMargin() const { return minNodeMargin_; }

  //---

  virtual NodeObj  *createNodeObj (const BBox &rect, Node *node, const ColorInd &ind) const;
  virtual EdgeObj  *createEdgeObj (const BBox &rect, Edge *edge) const;
  virtual GraphObj *createGraphObj(const BBox &rect, Graph *graph) const;

  //---

  void printStats();

 protected:
  friend class CQChartsGraphPlotNode;

 protected:
  using Graphs = std::map<int, Graph *>;

  // options
  Align     align_            { Align::JUSTIFY };  //!< align
  bool      adjustNodes_      { true };            //!< adjust nodes
  bool      autoCreateGraphs_ { false };           //!< auto create graphs
  double    nodeXMargin_      { 0.01 };            //!< node x margin
  double    nodeYMargin_      { 0.2 };             //!< node y margin
  double    nodeWidth_        { 16 };              //!< node x width in pixels
  bool      nodeXScaled_      { false };           //!< is node x scaled
  NodeShape nodeShape_        { NODE_SHAPE_NONE }; //!< node shape
  EdgeShape edgeShape_        { EDGE_SHAPE_ARC };  //!< edge shape
  bool      edgeScaled_       { true };            //!< is edge scaled

  BBox      targetBBox_ { -1, -1, 1, 1 }; //!< target range bbox
  // data
  NameNodeMap      nameNodeMap_;             //!< name node map
  IndNodeMap       indNodeMap_;              //!< ind node map
  Graphs           graphs_;                  //!< graphs
  Edges            edges_;                   //!< edges
  BBox             bbox_;                    //!< bbox
  CQChartsValueInd groupValueInd_;           //!< group value ind
  int              maxNodeDepth_  { 0 };     //!< max node depth (all graphs)
  double           minNodeMargin_ { 4 };     //!< minimum node margin (in pixels)
  double           boxMargin_     { 0.01 };  //!< bounding box margin
  double           edgeMargin_    { 0.01 };  //!< edge bounding box margin
  bool             pressed_       { false }; //!< mouse pressed
//bool             nodeYSet_      { false }; //!< node y is set
//double           nodeYMin_      { 0.0 };   //!< node y min
//double           nodeYMax_      { 0.0 };   //!< node y max
  int              numGroups_     { 1 };     //!< node number of groups
};

#endif
