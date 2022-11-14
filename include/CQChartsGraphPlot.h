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

  bool canEqualScale() const override { return true; }

  QString description() const override;

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

#include <CQChartsGraphMgr.h>

class CQChartsGraphPlot;
class CQChartsGraphGraphObj;
class CQChartsGraphNodeObj;
class CQChartsGraphEdgeObj;

/*!
 * \brief Graph Plot Graph Manager
 * \ingroup Charts
 */
class CQChartsGraphPlotMgr : public CQChartsGraphMgr {
 public:
  CQChartsGraphPlotMgr(CQChartsGraphPlot *plot);

  CQChartsGraphPlot *graphPlot() const { return graphPlot_; }

  Graph *createGraph(const QString &name) const override;

  Node *createNode(const QString &name) const override;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const override;

 private:
  CQChartsGraphPlot* graphPlot_ { nullptr };
};

/*!
 * \brief Graph Plot Graph
 * \ingroup Charts
 */
class CQChartsGraphPlotGraph : public CQChartsGraphGraph {
 public:
  using Obj = CQChartsGraphGraphObj;

 public:
  CQChartsGraphPlotGraph(GraphMgr *mgr, const QString &name);
 ~CQChartsGraphPlotGraph() override;

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  void updateRect() override;

  void moveBy(const Point &delta) override;
  void scale(double fx, double fy) override;

 private:
  Obj* obj_ { nullptr }; //!< graph plot object
};

/*!
 * \brief Graph Plot Node
 * \ingroup Charts
 */
class CQChartsGraphPlotNode : public CQChartsGraphNode {
 public:
  using Obj = CQChartsGraphNodeObj;

  CQChartsGraphPlotNode(GraphMgr *mgr, const QString &name);
 ~CQChartsGraphPlotNode() override;

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  const BBox &rect() const override;
  void setRect(const BBox &rect) override;

  void moveBy(const Point &delta) override;
  void scale(double fx, double fy) override;

 private:
  Obj* obj_ { nullptr }; //!< node plot object
};

/*!
 * \brief Graph Plot Edge
 * \ingroup Charts
 */
class CQChartsGraphPlotEdge : public CQChartsGraphEdge {
 public:
  using Obj = CQChartsGraphEdgeObj;

 public:
  CQChartsGraphPlotEdge(GraphMgr *mgr, const OptReal &value, Node *srcNode, Node *destNode);
 ~CQChartsGraphPlotEdge() override;

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

 private:
  Obj* obj_ { nullptr }; //!< edge plot object
};

//---

class CQChartsGraphPlot;

/*!
 * \brief Graph Plot Node object
 * \ingroup Charts
 */
class CQChartsGraphNodeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString       hierName  READ hierName  WRITE setHierName )
  Q_PROPERTY(QString       name      READ name      WRITE setName     )
  Q_PROPERTY(double        value     READ value     WRITE setValue    )
  Q_PROPERTY(int           depth     READ depth     WRITE setDepth    )
  Q_PROPERTY(ShapeType     shapeType READ shapeType WRITE setShapeType)
  Q_PROPERTY(int           numSides  READ numSides  WRITE setNumSides )
  Q_PROPERTY(double        aspect    READ aspect    WRITE setAspect   )
  Q_PROPERTY(CQChartsColor color     READ fillColor WRITE setFillColor)

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE          = static_cast<int>(CQChartsBoxObj::ShapeType::NONE),
    TRIANGLE      = static_cast<int>(CQChartsBoxObj::ShapeType::TRIANGLE),
    DIAMOND       = static_cast<int>(CQChartsBoxObj::ShapeType::DIAMOND),
    BOX           = static_cast<int>(CQChartsBoxObj::ShapeType::BOX),
    POLYGON       = static_cast<int>(CQChartsBoxObj::ShapeType::POLYGON),
    CIRCLE        = static_cast<int>(CQChartsBoxObj::ShapeType::CIRCLE),
    DOUBLE_CIRCLE = static_cast<int>(CQChartsBoxObj::ShapeType::DOUBLE_CIRCLE)
  };

  using GraphPlot = CQChartsGraphPlot;
  using Node      = CQChartsGraphNode;
  using Edge      = CQChartsGraphEdge;
  using Color     = CQChartsColor;
  using Angle     = CQChartsAngle;

 public:
  CQChartsGraphNodeObj(const GraphPlot *plot, const BBox &rect, Node *node, const ColorInd &ind);

  virtual ~CQChartsGraphNodeObj();

  //---

  //! get plot
  const GraphPlot *graphPlot() const { return graphPlot_; }

  //! get node
  Node *node() const { return node_; }

  //---

  //! get/set hier name
  const QString &hierName() const { return hierName_; }
  void setHierName(const QString &s) { hierName_ = s; }

  //! get/set name
  QString name() const;
  void setName(const QString &s);

  //! get/set value
  double value() const;
  void setValue(double r);

  //! get/set depth
  int depth() const;
  void setDepth(int depth);

  //---

  //! get/set shape type
  ShapeType shapeType() const;
  void setShapeType(const ShapeType &s);

  //! get/set num side
  int numSides() const;
  void setNumSides(int n);

  //! get/set aspect
  double aspect() const;
  void setAspect(double r);

  //! get/set color
  Color fillColor() const;
  void setFillColor(const Color &c);

  //---

  //! typename
  QString typeName() const override { return "node"; }

  //! get unique id
  QString calcId() const override;

  // get tip string
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

  // Implement edit interface
  bool editPress  (const Point &p) override;
  bool editMove   (const Point &p) override;
  bool editMotion (const Point &p) override;
  bool editRelease(const Point &p) override;

  void setEditBBox(const BBox &bbox, const ResizeSide &) override;

  //---

  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  QColor calcFillColor() const;

 protected:
  const GraphPlot* graphPlot_   { nullptr }; //!< parent plot
  Node*            node_        { nullptr }; //!< node
  QString          hierName_;                //!< node hier name
  bool             editChanged_ { false };   //!< edit is changed
};

//---

/*!
 * \brief Graph Plot Edge object
 * \ingroup Charts
 */
class CQChartsGraphEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)
  Q_PROPERTY(double    value     READ value)

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE,
    ARC,
    ARROW
  };

  using GraphPlot = CQChartsGraphPlot;
  using Edge      = CQChartsGraphEdge;
  using Node      = CQChartsGraphNode;
  using Length    = CQChartsLength;
  using Angle     = CQChartsAngle;

 public:
  CQChartsGraphEdgeObj(const GraphPlot *plot, const BBox &rect, Edge *edge);

  virtual ~CQChartsGraphEdgeObj();

  //---

  //! get plot
  const GraphPlot *graphPlot() const { return graphPlot_; }

  //! get/set edge
  Edge *edge() const { return edge_; }
  void setEdge(Edge *edge) { edge_ = edge; }

  //---

  //! typename
  QString typeName() const override { return "edge"; }

  //! get unique id
  QString calcId() const override;

  //! get tip string
  QString calcTipId() const override;

  //! get source/destination rect
  void setSrcRect (const BBox &rect) { srcRect_  = rect; }
  void setDestRect(const BBox &rect) { destRect_ = rect; }

  //! get/set shape type
  ShapeType shapeType() const;
  void setShapeType(const ShapeType &s);

  double value() const { return (edge()->hasValue() ? edge()->value().real() : 0.0); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! is point inside
  bool inside(const Point &p) const override;

  //---

  //! get connected objects
  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 protected:
  const GraphPlot*     graphPlot_ { nullptr }; //!< parent plot
  Edge*                edge_      { nullptr }; //!< edge
  BBox                 srcRect_;               //!< src rect
  BBox                 destRect_;              //!< dest rect
  mutable QPainterPath path_;                  //!< painter path
};

//---

/*!
 * \brief Graph Plot Graph object
 * \ingroup Charts
 */
class CQChartsGraphGraphObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using GraphPlot = CQChartsGraphPlot;
  using Graph     = CQChartsGraphGraph;

 public:
  CQChartsGraphGraphObj(const GraphPlot *plot, const BBox &rect, Graph *graph);

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

  // Implement edit interface
  bool editPress  (const Point &p) override;
  bool editMove   (const Point &p) override;
  bool editMotion (const Point &p) override;
  bool editRelease(const Point &p) override;

  void setEditBBox(const BBox &bbox, const ResizeSide &) override;

  bool isEditResize() const override { return true; }

  //---

  void draw(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 protected:
  const GraphPlot* graphPlot_   { nullptr }; //!< parent plot
  Graph*           graph_       { nullptr }; //!< node
  bool             editChanged_ { false };
};

//---

/*!
 * \brief Graph Plot
 * \ingroup Charts
 */
class CQChartsGraphPlot : public CQChartsConnectionPlot,
 public CQChartsObjNodeShapeData <CQChartsGraphPlot>,
 public CQChartsObjNodeTextData  <CQChartsGraphPlot>,
 public CQChartsObjEdgeShapeData <CQChartsGraphPlot>,
 public CQChartsObjEdgeTextData  <CQChartsGraphPlot>,
 public CQChartsObjGraphShapeData<CQChartsGraphPlot> {
  Q_OBJECT

  // node options
  Q_PROPERTY(CQChartsLength nodeXMargin    READ nodeXMargin      WRITE setNodeXMargin   )
  Q_PROPERTY(CQChartsLength nodeYMargin    READ nodeYMargin      WRITE setNodeYMargin   )
  Q_PROPERTY(CQChartsLength nodeWidth      READ nodeWidth        WRITE setNodeWidth     )
  Q_PROPERTY(CQChartsLength nodeHeight     READ nodeHeight       WRITE setNodeHeight    )
  Q_PROPERTY(bool           nodeXScaled    READ isNodeXScaled    WRITE setNodeXScaled   )
  Q_PROPERTY(bool           nodeYScaled    READ isNodeYScaled    WRITE setNodeYScaled   )
  Q_PROPERTY(NodeShape      nodeShape      READ nodeShape        WRITE setNodeShape     )
  Q_PROPERTY(bool           nodeTextInside READ isNodeTextInside WRITE setNodeTextInside)

  // edge options
  Q_PROPERTY(EdgeShape edgeShape  READ edgeShape    WRITE setEdgeShape )
  Q_PROPERTY(bool      edgeScaled READ isEdgeScaled WRITE setEdgeScaled)

  // graph options
  Q_PROPERTY(Qt::Orientation orientation READ orientation   WRITE setOrientation)

  // coloring
  Q_PROPERTY(bool blendEdgeColor READ isBlendEdgeColor WRITE setBlendEdgeColor)

  // placement
  Q_PROPERTY(Align align            READ align              WRITE setAlign           )
  Q_PROPERTY(bool  alignFirstLast   READ isAlignFirstLast   WRITE setAlignFirstLast  )
  Q_PROPERTY(bool  adjustNodes      READ isAdjustNodes      WRITE setAdjustNodes     )
  Q_PROPERTY(bool  adjustCenters    READ isAdjustCenters    WRITE setAdjustCenters   )
  Q_PROPERTY(bool  removeOverlaps   READ isRemoveOverlaps   WRITE setRemoveOverlaps  )
  Q_PROPERTY(bool  reorderEdges     READ isReorderEdges     WRITE setReorderEdges    )
  Q_PROPERTY(int   adjustIterations READ adjustIterations   WRITE setAdjustIterations)
  Q_PROPERTY(bool  autoCreateGraphs READ isAutoCreateGraphs WRITE setAutoCreateGraphs)

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES (Node, node)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES (Edge, edge)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Graph, graph)

  // text style

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

  using Node        = CQChartsGraphNode;
  using Nodes       = std::vector<Node *>;
  using IndNodeMap  = std::map<int, Node *>;
  using NodeSet     = std::set<Node *>;
  using Edge        = CQChartsGraphEdge;
  using Edges       = std::vector<Edge *>;
  using NodeObj     = CQChartsGraphNodeObj;
  using EdgeObj     = CQChartsGraphEdgeObj;
  using Graph       = CQChartsGraphGraph;
  using GraphObj    = CQChartsGraphGraphObj;
  using Length      = CQChartsLength;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsGraphPlot(View *view, const ModelP &model);
 ~CQChartsGraphPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set node x margin
  const Length &nodeXMargin() const;
  void setNodeXMargin(const Length &l);

  //! get/set node y margin
  const Length &nodeYMargin() const;
  void setNodeYMargin(const Length &l);

  //---

  //! get/set parallel width
  const Length &nodeWidth() const;
  void setNodeWidth(const Length &l);

  //! get/set perp height
  const Length &nodeHeight() const;
  void setNodeHeight(const Length &l);

  //! get/set is node x scaled
  bool isNodeXScaled() const;
  void setNodeXScaled(bool b);

  //! get/set is node y scaled
  bool isNodeYScaled() const;
  void setNodeYScaled(bool b);

  //! get/set node shape
  NodeShape nodeShape() const;
  void setNodeShape(const NodeShape &s);

  //! get/set is node text inside
  bool isNodeTextInside() const { return nodeTextInside_; }
  void setNodeTextInside(bool b);

  //---

  //! get/set edge shape
  const EdgeShape &edgeShape() const { return edgeShape_; }
  void setEdgeShape(const EdgeShape &s);

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeScaled_; }
  void setEdgeScaled(bool b);

  //! get/set edge width
  const Length &edgeWidth() const { return edgeWidth_; }
  void setEdgeWidth(const Length &l);

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const;
  void setOrientation(const Qt::Orientation &o);

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }
  bool isVertical  () const { return orientation() == Qt::Vertical  ; }

  //---

  //! get/set blend node colors for edge
  bool isBlendEdgeColor() const { return blendEdgeColor_; }
  void setBlendEdgeColor(bool b);

  //---

  //! get/set node align
  Align align() const;
  void setAlign(const Align &a);

  //! get/set node align first/last
  bool isAlignFirstLast() const;
  void setAlignFirstLast(bool b);

  //---

  //! get/set adjust nodes
  bool isAdjustNodes() const;
  void setAdjustNodes(bool b);

  //! get/set adjust center
  bool isAdjustCenters() const;
  void setAdjustCenters(bool b);

  //! get/set remove overlaps
  bool isRemoveOverlaps() const;
  void setRemoveOverlaps(bool b);

  //! get/set reorder edges
  bool isReorderEdges() const;
  void setReorderEdges(bool b);

  //! get/set adjust iterations
  int adjustIterations() const;
  void setAdjustIterations(int n);

  //---

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

  const Nodes &nodes() const { return nodes_; }

  int numNodes() const { return int(nodes_.size()); }

  const Edges &edges() const { return edges_; }

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

  void addConnectionObj(int id, const ConnectionsData &connectionsData,
                        const NodeIndex &nodeIndex) const override;

  //---

  bool initTableObjs() const;

  //---

  void processNodeNameValues(Node *node, const NameValues &valueValues) const;
  void processNodeNameValue (Node *node, const QString &name, const QString &value) const;

  void processEdgeNameValues(Edge *edge, const NameValues &valueValues) const;

  //---

  void filterObjs();

  //---

  int maxNodeDepth() const { return maxNodeDepth_; }

  //---

  bool keyPress(int key, int modifier) override;

  //---

  void placeGraphs() const;

  //---

  void autoCreateGraphs() const;

  void addObjects(PlotObjs &objs) const;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

 protected:
  void clearNodesAndEdges();

  Node *findNode(const QString &name) const;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  Node *createNode(const QString &name) const;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  void createObjsGraph(PlotObjs &objs) const;

  void createGraphs() const;

  Graph *getGraph(int group, int parentGroup=-1) const;

  void clearGraphs();

  NodeObj *createObjFromNode(Graph *graph, Node *node) const;

  EdgeObj *addEdgeObj(Edge *edge) const;

  //---

  bool adjustNodes() const;

  //---

  const BBox &bbox() const { return bbox_; }

  //---

  virtual NodeObj  *createNodeObj (const BBox &rect, Node *node, const ColorInd &ind) const;
  virtual EdgeObj  *createEdgeObj (const BBox &rect, Edge *edge) const;
  virtual GraphObj *createGraphObj(const BBox &rect, Graph *graph) const;

  //---

  void printStats();

 protected Q_SLOTS:
  void fixSelected();
  void unfixSelected();

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 protected:
  friend class CQChartsGraphNode;

 protected:
  using GraphMgr  = CQChartsGraphPlotMgr;
  using GraphMgrP = std::unique_ptr<GraphMgr>;

  // node
  bool nodeTextInside_ { false };

  // placement
  Align     align_            { Align::JUSTIFY };  //!< align
  bool      adjustNodes_      { true };            //!< adjust nodes
  bool      autoCreateGraphs_ { false };           //!< auto create graphs
  EdgeShape edgeShape_        { EDGE_SHAPE_ARC };  //!< edge shape
  bool      edgeScaled_       { false };           //!< is edge scaled

  // bbox, margin, node width
  BBox   targetBBox_ { -1, -1, 1, 1 };     //!< target range bbox
  Length edgeWidth_  { Length::pixel(8) }; //!< edge width

  // coloring
  bool blendEdgeColor_ { true }; //!< blend edge color

  // data
  GraphMgrP        graphMgr_;                //!< graph mgr
  Nodes            nodes_;                   //!< all nodes
  Edges            edges_;                   //!< all edges
  BBox             bbox_;                    //!< bbox
  CQChartsValueInd groupValueInd_;           //!< group value ind
  int              maxNodeDepth_  { 0 };     //!< max node depth (all graphs)
//double           boxMargin_     { 0.01 };  //!< bounding box margin
  double           edgeMargin_    { 0.01 };  //!< edge bounding box margin
  bool             pressed_       { false }; //!< mouse pressed
  int              numGroups_     { 1 };     //!< node number of groups

  struct FixedNode {
    FixedNode() { }

    FixedNode(const BBox &r) { rect = r; }

    BBox rect;
  };

  using FixedNodes = std::map<QString, FixedNode>;

  FixedNodes fixedNodes_;
};

//---

/*!
 * \brief Graph Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsGraphPlotCustomControls : public CQChartsConnectionPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsGraphPlotCustomControls(CQCharts *charts);

  void init() override;

  void addWidgets() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected:
  CQChartsGraphPlot* graphPlot_ { nullptr };
};

#endif
