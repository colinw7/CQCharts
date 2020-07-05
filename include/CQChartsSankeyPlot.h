#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsValueInd.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Sankey plot type
 * \ingroup Charts
 */
class CQChartsSankeyPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsSankeyPlotType();

  QString name() const override { return "sankey"; }
  QString desc() const override { return "Sankey"; }

  void addParameters() override;

  bool hasTitle() const override { return false; }

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

class CQChartsSankeyPlot;
class CQChartsSankeyPlotNode;
class CQChartsSankeyPlotEdge;
class CQChartsSankeyPlotGraph;
class CQChartsSankeyNodeObj;
class CQChartsSankeyEdgeObj;
class CQChartsSankeyGraphObj;

/*!
 * \brief Sankey plot node
 * \ingroup Charts
 */
class CQChartsSankeyPlotNode {
 public:
  using Edges      = std::vector<CQChartsSankeyPlotEdge *>;
  using NodeSet    = std::set<const CQChartsSankeyPlotNode *>;
  using Plot       = CQChartsSankeyPlot;
  using Graph      = CQChartsSankeyPlotGraph;
  using Edge       = CQChartsSankeyPlotEdge;
  using Node       = CQChartsSankeyPlotNode;
  using Obj        = CQChartsSankeyNodeObj;
  using OptReal    = CQChartsOptReal;
  using ModelIndex = CQChartsModelIndex;
  using BBox       = CQChartsGeom::BBox;
  using Point      = CQChartsGeom::Point;
  using ShapeType  = CQChartsBoxObj::ShapeType;

 public:
  CQChartsSankeyPlotNode(const Plot *plot, const QString &str);

  virtual ~CQChartsSankeyPlotNode();

  const Plot *plot() const { return plot_; }

  Node *parent() const { return parent_; }

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

  //! get/set group
  int graphId() const { return graphId_; }
  void setGraphId(int i) { graphId_ = i; }

  //! get/set parent group
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
  Obj*        obj_           { nullptr };         //!< plot object
};

//---

/*!
 * \brief Sankey plot edge
 * \ingroup Charts
 */
class CQChartsSankeyPlotEdge {
 public:
  enum class ShapeType {
    NONE,
    ARC,
    ARROW
  };

  using Plot    = CQChartsSankeyPlot;
  using Node    = CQChartsSankeyPlotNode;
  using Obj     = CQChartsSankeyEdgeObj;
  using OptReal = CQChartsOptReal;

 public:
  CQChartsSankeyPlotEdge(const Plot *plot, const OptReal &value, Node *srcNode, Node *destNode);

  virtual ~CQChartsSankeyPlotEdge();

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
  Obj*        obj_       { nullptr };         //!< associated edge object
};

//---

class CQChartsSankeyPlotGraph : public CQChartsSankeyPlotNode {
 public:
  using Node  = CQChartsSankeyPlotNode;
  using Nodes = std::vector<Node *>;

  struct DepthData {
    Nodes  nodes;        // nodes at depth
    double size { 0.0 }; // total size
  };

  using DepthNodesMap = std::map<int,DepthData>;
  using IndNodeMap    = std::map<int,Node *>;
  using PosNodesMap   = std::map<int,Nodes>;
  using Graph         = CQChartsSankeyPlotGraph;
  using Graphs        = std::vector<Graph *>;
  using Obj           = CQChartsSankeyGraphObj;

 public:
  CQChartsSankeyPlotGraph(const Plot *plot, const QString &str);

  virtual ~CQChartsSankeyPlotGraph() { }

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

  int minNodeDepth() const { return minNodeDepth_; }
  void setMinNodeDepth(int i) { minNodeDepth_ = i; }

  int maxNodeDepth() const { return maxNodeDepth_; }
  void setMaxNodeDepth(int i) { maxNodeDepth_ = i; }

  int maxHeight() const { return maxHeight_; }
  void setMaxHeight(int i) { maxHeight_ = i; }

  double totalSize() const { return totalSize_; }
  void setTotalSize(double r) { totalSize_ = r; }

  const DepthNodesMap &depthNodesMap() const { return depthNodesMap_; }

  void addDepthNode(int depth, Node *node) { depthNodesMap_[depth].nodes.push_back(node); }
  void addDepthSize(int depth, double size) { depthNodesMap_[depth].size += size; }

  const PosNodesMap &posNodesMap() const { return posNodesMap_; }
  void resetPosNodes() { posNodesMap_.clear(); }
  void addPosNode(Node *node) { posNodesMap_[node->xpos()].push_back(node); }
  bool hasPosNodes(int pos) const { return (posNodesMap_.find(pos) != posNodesMap_.end()); }
  const Nodes &posNodes(int pos) const {
    auto p = posNodesMap_.find(pos); assert(p != posNodesMap_.end()); return (*p).second; }

  //---

  void updateRect();

  //---

  Nodes placeNodes() const;

  //---

  //! get source depth (from external node edges)
  int srcDepth () const override;
  //! get destination depth (from external node edges)
  int destDepth() const override;

  void setRect(const BBox &r) override;

  //---

  double valueMargin() const { return valueMargin_; }
  void setValueMargin(double s) { valueMargin_ = s; }

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
  Obj*          obj_          { nullptr }; //!< graph object
  double        valueMargin_  { 0.0 };     //!< y value margin
  double        valueScale_   { 1.0 };     //!< y value scale
};

//---

/*!
 * \brief Sankey Plot Node object
 * \ingroup Charts
 */
class CQChartsSankeyNodeObj : public CQChartsPlotObj {
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
  using Plot      = CQChartsSankeyPlot;
  using Node      = CQChartsSankeyPlotNode;
  using Edge      = CQChartsSankeyPlotEdge;
  using ShapeType = CQChartsBoxObj::ShapeType;

 public:
  CQChartsSankeyNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &ind);

  virtual ~CQChartsSankeyNodeObj();

  Node *node() const { return node_; }

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
  using EdgeRect = std::map<Edge *,BBox>;

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
 * \brief Sankey Plot Edge object
 * \ingroup Charts
 */
class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE,
    ARC,
    ARROW
  };

  using Plot = CQChartsSankeyPlot;
  using Edge = CQChartsSankeyPlotEdge;
  using Node = CQChartsSankeyPlotNode;

 public:
  CQChartsSankeyEdgeObj(const Plot *plot, const BBox &rect, Edge *edge);

  virtual ~CQChartsSankeyEdgeObj() { }

  //! typename
  QString typeName() const override { return "edge"; }

  //! get edge
  Edge *edge() const { return edge_; }

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
 * \brief Sankey Plot Graph object
 * \ingroup Charts
 */
class CQChartsSankeyGraphObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot  = CQChartsSankeyPlot;
  using Graph = CQChartsSankeyPlotGraph;

 public:
  CQChartsSankeyGraphObj(const Plot *plot, const BBox &rect, Graph *graph);

  virtual ~CQChartsSankeyGraphObj();

  Graph *graph() const { return graph_; }

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

CQCHARTS_NAMED_SHAPE_DATA(Edge,edge)
CQCHARTS_NAMED_SHAPE_DATA(Graph,graph)

/*!
 * \brief Sankey Plot
 * \ingroup Charts
 */
class CQChartsSankeyPlot : public CQChartsConnectionPlot,
 public CQChartsObjTextData<CQChartsSankeyPlot>,
 public CQChartsObjNodeShapeData <CQChartsSankeyPlot>,
 public CQChartsObjEdgeShapeData <CQChartsSankeyPlot>,
 public CQChartsObjGraphShapeData<CQChartsSankeyPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double    nodeXMargin READ nodeXMargin   WRITE setNodeXMargin)
  Q_PROPERTY(double    nodeYMargin READ nodeYMargin   WRITE setNodeYMargin)
  Q_PROPERTY(double    nodeWidth   READ nodeWidth     WRITE setNodeWidth  )
  Q_PROPERTY(bool      nodeXScaled READ isNodeXScaled WRITE setNodeXScaled)
  Q_PROPERTY(bool      nodeYScaled READ isNodeYScaled WRITE setNodeYScaled)
  Q_PROPERTY(NodeShape nodeShape   READ nodeShape     WRITE setNodeShape  )
  Q_PROPERTY(EdgeShape edgeShape   READ edgeShape     WRITE setEdgeShape  )
  Q_PROPERTY(bool      edgeScaled  READ isEdgeScaled  WRITE setEdgeScaled )

  // align
  Q_PROPERTY(Align align READ align WRITE setAlign)

  // placement
  Q_PROPERTY(bool adjustNodes      READ isAdjustNodes      WRITE setAdjustNodes     )
  Q_PROPERTY(bool autoCreateGraphs READ isAutoCreateGraphs WRITE setAutoCreateGraphs)

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge,edge)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Graph,graph)

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

  using Node        = CQChartsSankeyPlotNode;
  using Nodes       = std::vector<Node *>;
  using NameNodeMap = std::map<QString,Node *>;
  using IndNodeMap  = std::map<int,Node *>;
  using NodeSet     = std::set<Node *>;
  using Edge        = CQChartsSankeyPlotEdge;
  using Edges       = std::vector<Edge *>;
  using NodeObj     = CQChartsSankeyNodeObj;
  using EdgeObj     = CQChartsSankeyEdgeObj;
  using Graph       = CQChartsSankeyPlotGraph;
  using GraphObj    = CQChartsSankeyGraphObj;
  using PosNodeMap  = std::map<double, Node *>;
  using PosEdgeMap  = std::map<double, Edge *>;

 public:
  CQChartsSankeyPlot(View *view, const ModelP &model);

 ~CQChartsSankeyPlot();

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

  //! get/set is node y scaled
  bool isNodeYScaled() const { return nodeYScaled_; }
  void setNodeYScaled(bool b);

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

  Range getCalcDataRange() const override;

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

  void addPathValue(const QStringList &, double) const override;

  void propagatePathValues();

  //---

  bool initFromToObjs() const;

  void addFromToValue(const QString &, const QString &, double,
                      const CQChartsNameValues &, const GroupData &) const override;

  //---

  bool initLinkObjs      () const;
  bool initConnectionObjs() const;
  bool initTableObjs     () const;

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
  friend class CQChartsSankeyPlotNode;

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
  bool      nodeYScaled_      { true };            //!< is node y scaled
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
};

#endif
