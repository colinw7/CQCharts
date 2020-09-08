#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsRectPlacer.h>

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

/*!
 * \brief Sankey plot node
 * \ingroup Charts
 */
class CQChartsSankeyPlotNode {
 public:
  using Node       = CQChartsSankeyPlotNode;
  using NodeSet    = std::set<const Node *>;
  using Edge       = CQChartsSankeyPlotEdge;
  using Edges      = std::vector<Edge *>;
  using EdgeSet    = std::set<const Edge *>;
  using Plot       = CQChartsSankeyPlot;
  using Obj        = CQChartsSankeyNodeObj;
  using OptReal    = CQChartsOptReal;
  using ModelIndex = CQChartsModelIndex;
  using BBox       = CQChartsGeom::BBox;
  using Point      = CQChartsGeom::Point;
  using EdgeRect   = std::map<Edge *, BBox>;
  using PathIdRect = std::map<int, BBox>;

 public:
  CQChartsSankeyPlotNode(const Plot *plot, const QString &str);

  virtual ~CQChartsSankeyPlotNode();

  //! get plot
  const Plot *plot() const { return plot_; }

  //! get edge parent
  Node *parent() const { return parent_; }

  //! string (name ?)
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
  void setGroup(int ig, int ng=0) { group_ = ig; ngroup_ = ng; }
  int ngroup() const { return ngroup_; }

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
  bool hasDestNode(CQChartsSankeyPlotNode *destNode) const;

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  //! get source depth (from connections)
  int srcDepth() const;
  //! get destination depth (from connections)
  int destDepth() const;

  //! get/set color
  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  // get/set x pos
  int xpos() const { return xpos_; }
  void setXPos(int x) { xpos_ = x; }

  // get/set rect
  const BBox &rect() const;
  void setRect(const BBox &r);

  //---

  //! get edge (max) sum
  double edgeSum() const;

  //! get source edge (max) sum
  double srcEdgeSum() const;

  //! get destination edge (max) sum
  double destEdgeSum() const;

  //! move node by delta
  void moveBy(const Point &delta);

  //! scale node by factor
  void scale(double fx, double fy);

  //---

  void placeEdges(bool reset=true);

  void clearSrcEdgeRects() { srcEdgeRect_.clear(); }

  void setSrcEdgeRect(Edge *edge, const BBox &bbox);

  bool hasSrcEdgeRect(Edge *edge) const {
    auto p = srcEdgeRect_.find(edge);

    return (p != srcEdgeRect_.end());
  }

  const BBox &srcEdgeRect(Edge *edge) const {
    auto p = srcEdgeRect_.find(edge);
    assert(p != srcEdgeRect_.end());

    return (*p).second;
  }

  const EdgeRect &srcEdgeRects() const { return srcEdgeRect_; }

  void moveSrcEdgeRectsBy(const Point &delta) {
    for (auto &edgeRect : srcEdgeRect_) {
      if (! edgeRect.second.isSet()) continue;

      edgeRect.second.moveBy(delta);
    }
  }

  //---

  void clearDestEdgeRects() { destEdgeRect_.clear(); }

  void setDestEdgeRect(Edge *edge, const BBox &bbox);

  bool hasDestEdgeRect(Edge *edge) const {
    auto p = destEdgeRect_.find(edge);

    return (p != destEdgeRect_.end());
  }

  const BBox &destEdgeRect(Edge *edge) const {
    auto p = destEdgeRect_.find(edge);
    assert(p != destEdgeRect_.end());

    return (*p).second;
  }

  const EdgeRect &destEdgeRects() const { return destEdgeRect_; }

  void moveDestEdgeRectsBy(const Point &delta) {
    for (auto &edgeRect : destEdgeRect_) {
      if (! edgeRect.second.isSet()) continue;

      edgeRect.second.moveBy(delta);
    }
  }

  //---

  void adjustSrcDestRects();

  //---

  void allSrcNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const;
  void allDestNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const;

  //---

  QColor calcColor() const;

 protected:
  //! calc src/destination depth
  int calcSrcDepth (NodeSet &visited) const;
  int calcDestDepth(NodeSet &visited) const;

 protected:
  const Plot*   plot_          { nullptr }; //!< associated plot
  Node*         parent_        { nullptr }; //!< parent node
  QString       str_;                       //!< string
  int           id_            { -1 };      //!< id
  bool          visible_       { true };    //!< is visible
  ModelIndex    ind_;                       //!< model index
  QString       name_;                      //!< name
  QString       label_;                     //!< label
  OptReal       value_;                     //!< value
  int           group_         { -1 };      //!< group index
  int           ngroup_        { 0 };       //!< number of groups
  int           depth_         { -1 };      //!< depth
  CQChartsColor color_;                     //!< fill color
  Edges         srcEdges_;                  //!< source edges
  Edges         destEdges_;                 //!< destination edges
  Edges         nonPrimaryEdges_;           //!< non-primary edges
  int           srcDepth_      { -1 };      //!< source depth (calculated)
  int           destDepth_     { -1 };      //!< destination depth (calculated)
  int           xpos_          { -1 };      //!< x position
  BBox          rect_;                      //!< placed rectangle
  EdgeRect      srcEdgeRect_;               //!< edge to src
  EdgeRect      destEdgeRect_;              //!< edge to dest
  PathIdRect    srcPathIdRect_;             //!< src path id rect
  PathIdRect    destPathIdRect_;            //!< dest path id rect
  Obj*          obj_           { nullptr }; //!< plot object
};

//---

/*!
 * \brief Sankey plot edge
 * \ingroup Charts
 */
class CQChartsSankeyPlotEdge {
 public:
  using Plot    = CQChartsSankeyPlot;
  using Edge    = CQChartsSankeyPlotEdge;
  using Node    = CQChartsSankeyPlotNode;
  using Obj     = CQChartsSankeyEdgeObj;
  using OptReal = CQChartsOptReal;
  using BBox    = CQChartsGeom::BBox;
  using Point   = CQChartsGeom::Point;

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

  //! get/set color
  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  //! get/set path id
  int pathId() const { return pathId_; }
  void setPathId(int i) { pathId_ = i; }

  // get source node
  Node *srcNode() const { return srcNode_; }

  // get destination node
  Node *destNode() const { return destNode_; }

  // is self connect
  bool isSelf() const { return srcNode() == destNode(); }

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  //! get/set edge line
  bool isLine() const { return isLine_; }
  void setLine(bool b) { isLine_ = b; }

  //---

  bool overlaps(const Edge *edge) const;

  bool edgePath(QPainterPath &path, bool isLine=false) const;

 protected:
  const Plot*   plot_     { nullptr }; //!< plot
  int           id_       { -1 };      //!< unique id
  OptReal       value_;                //!< value
  QString       label_;                //!< label
  CQChartsColor color_;                //!< color
  int           pathId_   { -1 };      //!< path id
  Node*         srcNode_  { nullptr }; //!< source node
  Node*         destNode_ { nullptr }; //!< destination node
  Obj*          obj_      { nullptr }; //!< associated edge object
  bool          isLine_   { false };   //!< is edge a line
};

//---

/*!
 * \brief Graph Data (Nodes, Edges) for Sankey Plot Placement Data
 * \ingroup Charts
 */
class CQChartsSankeyPlotGraph {
 public:
  using Plot  = CQChartsSankeyPlot;
  using Node  = CQChartsSankeyPlotNode;
  using Nodes = std::vector<Node *>;
  using BBox  = CQChartsGeom::BBox;
  using Point = CQChartsGeom::Point;

  struct DepthData {
    Nodes  nodes;        //!< nodes at depth
    double size { 0.0 }; //!< total size
  };

  using DepthNodesMap = std::map<int, DepthData>;
  using IndNodeMap    = std::map<int, Node *>;
  using PosNodesMap   = std::map<int, Nodes>;

 public:
  CQChartsSankeyPlotGraph(const Plot *plot);

  virtual ~CQChartsSankeyPlotGraph() { }

  //! get plot
  const Plot *plot() const { return plot_; }

  //---

  //! get/add nodes
  const Nodes &nodes() const { return nodes_; }
  void addNode(Node *node);

  //--

  //! get/set max node depth
  int maxNodeDepth() const { return maxNodeDepth_; }
  void setMaxNodeDepth(int i) { maxNodeDepth_ = i; }

  //! get/set max height
  int maxHeight() const { return maxHeight_; }
  void setMaxHeight(int i) { maxHeight_ = i; }

  //! get/set total size
  double totalSize() const { return totalSize_; }
  void setTotalSize(double r) { totalSize_ = r; }

  //---

  //! get depth nodes
  const DepthNodesMap &depthNodesMap() const { return depthNodesMap_; }

  //! clear depth nodes
  void clearDepthNodesMap() { depthNodesMap_.clear(); }

  //! add node at depth
  void addDepthNode(int depth, Node *node);

  //! add to depth size
  void addDepthSize(int depth, double size);

  //---

  //! get pos nodes
  const PosNodesMap &posNodesMap() const { return posNodesMap_; }

  //! clear pos nodes
  void resetPosNodes() { posNodesMap_.clear(); }

  //! add pos node
  void addPosNode(Node *node);

  //! has nodes at pos
  bool hasPosNodes(int pos) const;

  //! get nodes at pos
  const Nodes &posNodes(int pos) const;

  //---

  //! get/set rect
  const BBox &rect() const { return rect_; }
  void setRect(const BBox &r);

  //! update rectangle from nodes
  void updateRect();

  //---

  //! get placeable nodes
  Nodes placeableNodes() const;

  //---

  //! get/set value margin
  double valueMargin() const { return valueMargin_; }
  void setValueMargin(double s) { valueMargin_ = s; }

  //! get/set value scale
  double valueScale() const { return valueScale_; }
  void setValueScale(double s) { valueScale_ = s; }

  //---

  //! move node by delta
  void moveBy(const Point &delta);

  //! scale node by factor
  void scale(double fx, double fy);

 protected:
  const Plot*   plot_         { nullptr }; //!< associated plot
  Nodes         nodes_;                    //!< nodes in graph
  BBox          rect_;                     //!< placed rectangle
  int           maxNodeDepth_ { 0 };       //!< max depth of nodes
  int           maxHeight_    { 0 };       //!< max height of all depth nodes
  double        totalSize_    { 0.0 };     //!< total size of all depth nodes
  DepthNodesMap depthNodesMap_;            //!< nodes data at depth
  PosNodesMap   posNodesMap_;              //!< pos node map
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

  Q_PROPERTY(QString       hierName READ hierName WRITE setHierName)
  Q_PROPERTY(QString       name     READ name     WRITE setName    )
  Q_PROPERTY(double        value    READ value    WRITE setValue   )
  Q_PROPERTY(int           depth    READ depth    WRITE setDepth   )
  Q_PROPERTY(CQChartsColor color    READ color    WRITE setColor   )

 public:
  using Plot = CQChartsSankeyPlot;
  using Node = CQChartsSankeyPlotNode;
  using Edge = CQChartsSankeyPlotEdge;

 public:
  CQChartsSankeyNodeObj(const Plot *plot, const BBox &rect, Node *node,
                        const ColorInd &ig, const ColorInd &iv);

  virtual ~CQChartsSankeyNodeObj();

  Node *node() const { return node_; }

  //---

  //! get/set hier name
  const QString &hierName() const { return hierName_; }
  void setHierName(const QString &s) { hierName_ = s; }

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //! get/set value
  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  //! get/set depth
  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

  //---

  //! get/set color
  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

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

  void placeEdges(bool reset=true);

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

  void drawConnectionMouseOver(PaintDevice *device, int mouseColoring, int pathId=-1) const;

  void drawFg(PaintDevice *device) const override;

  void drawFgRect(PaintDevice *device, const BBox &rect) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  QColor calcFillColor() const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot*   plot_        { nullptr }; //!< parent plot
  Node*         node_        { nullptr }; //!< node
  QString       hierName_;                //!< node hier name
  QString       name_;                    //!< node name
  double        value_       { 0.0 };     //!< node value
  int           depth_       { -1 };      //!< node depth
  CQChartsColor color_;                   //!< custom color
  bool          editChanged_ { false };   //!< edit is changed
};

//---

/*!
 * \brief Sankey Plot Edge object
 * \ingroup Charts
 */
class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsSankeyPlot;
  using Edge = CQChartsSankeyPlotEdge;
  using Node = CQChartsSankeyPlotNode;

 public:
  CQChartsSankeyEdgeObj(const Plot *plot, const BBox &rect, Edge *edge);

  virtual ~CQChartsSankeyEdgeObj();

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

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! is point inside
  bool inside(const Point &p) const override;

  //! get connected objects
  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) override;

#if 0
  void drawFg(PaintDevice *device) const override;
#endif

  bool edgePath(QPainterPath &path, bool isLine=false) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot*  plot_      { nullptr }; //!< parent plot
  Edge*        edge_      { nullptr }; //!< edge
  BBox         srcRect_;               //!< src rect
  BBox         destRect_;              //!< dest rect
  QPainterPath path_;                  //!< painter path
};

//---

/*!
 * \brief Sankey Plot
 * \ingroup Charts
 */
class CQChartsSankeyPlot : public CQChartsConnectionPlot,
 public CQChartsObjTextData<CQChartsSankeyPlot>,
 public CQChartsObjNodeShapeData<CQChartsSankeyPlot>,
 public CQChartsObjEdgeShapeData<CQChartsSankeyPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double nodeMargin READ nodeMargin WRITE setNodeMargin)
  Q_PROPERTY(double nodeWidth  READ nodeWidth  WRITE setNodeWidth )
  Q_PROPERTY(bool   edgeLine   READ isEdgeLine WRITE setEdgeLine  )

  // coloring
  Q_PROPERTY(bool           srcColoring   READ isSrcColoring WRITE setSrcColoring  )
  Q_PROPERTY(ConnectionType mouseColoring READ mouseColoring WRITE setMouseColoring)

  // align
  Q_PROPERTY(Align align READ align WRITE setAlign)

  // placement
  Q_PROPERTY(bool adjustNodes READ isAdjustNodes WRITE setAdjustNodes)
  Q_PROPERTY(bool adjustText  READ isAdjustText  WRITE setAdjustText )

  // edge scaling
  Q_PROPERTY(bool useMaxTotals READ useMaxTotals WRITE setUseMaxTotals)

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // text visible on inside/selected (when text invisible)
  Q_PROPERTY(bool insideTextVisible   READ isInsideTextVisible   WRITE setInsideTextVisible  )
  Q_PROPERTY(bool selectedTextVisible READ isSelectedTextVisible WRITE setSelectedTextVisible)

  // text style
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(ConnectionType)
  Q_ENUMS(Align)

 public:
  enum class ConnectionType {
    NONE,
    SRC,
    DEST,
    ALL_SRC,
    ALL_DEST,
    SRC_DEST,
    ALL_SRC_DEST
  };

  enum class Align {
    SRC,
    DEST,
    JUSTIFY,
    RAND
  };

  using Node        = CQChartsSankeyPlotNode;
  using Nodes       = std::vector<Node *>;
  using NameNodeMap = std::map<QString, Node *>;
  using IndNodeMap  = std::map<int, Node *>;
  using NodeSet     = std::set<Node *>;
  using Edge        = CQChartsSankeyPlotEdge;
  using Edges       = std::vector<Edge *>;
  using NodeObj     = CQChartsSankeyNodeObj;
  using EdgeObj     = CQChartsSankeyEdgeObj;
  using Graph       = CQChartsSankeyPlotGraph;
  using PosNodeMap  = std::map<double, Node *>;
  using PosEdgeMap  = std::map<double, Edge *>;

 public:
  CQChartsSankeyPlot(View *view, const ModelP &model);

 ~CQChartsSankeyPlot();

  //---

  //! get/set node y margin
  double nodeMargin() const { return nodeMargin_; }
  void setNodeMargin(double r);

  //---

  //! get/set x width
  double nodeWidth() const { return nodeWidth_; }
  void setNodeWidth(double r);

  //---

  //! get/set edge line
  bool isEdgeLine() const { return edgeLine_; }
  void setEdgeLine(bool b);

  //---

  //! get/set color by source nodes
  bool isSrcColoring() const { return srcColoring_; }
  void setSrcColoring(bool b);

  const ConnectionType &mouseColoring() const { return mouseColoring_; }
  void setMouseColoring(const ConnectionType &t) { mouseColoring_ = t; }

  //---

  //! get/set text align
  const Align &align() const { return align_; }
  void setAlign(const Align &a);

  //---

  //! get/set adjust nodes
  bool isAdjustNodes() const { return adjustNodes_; }
  void setAdjustNodes(bool b);

  //---

  //! get/set adjust text
  bool isAdjustText() const { return adjustText_; }
  void setAdjustText(bool b);

  //---

  //! text visible on inside (when text invisible)
  bool isInsideTextVisible() const { return insideTextVisible_; }
  void setInsideTextVisible(bool b) { insideTextVisible_ = b; }

  //! text visible on selected (when text invisible)
  bool isSelectedTextVisible() const { return selectedTextVisible_; }
  void setSelectedTextVisible(bool b) { selectedTextVisible_ = b; }

  //---

  //! get/set use of of source/destination nodes for edge scaling
  bool useMaxTotals() const { return useMaxTotals_; }
  void setUseMaxTotals(bool b);

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

  //---

  void preDrawFgObjs (CQChartsPaintDevice *device) const override;
  void postDrawFgObjs(CQChartsPaintDevice *device) const override;

  //---

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

  void addFromToValue(const QString &fromStr, const QString &toStr, double value,
                      const FromToData &fromToData) const override;

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

  Graph *graph() const { return graph_; }

  void placeGraph() const;
  void placeGraphNodes(const Nodes &nodes) const;

  void calcGraphNodesXPos(const Nodes &nodes) const;

  //---

  void addObjects(PlotObjs &objs) const;

 protected:
  void clearNodesAndEdges();

  Node *findNode(const QString &name) const;

  Node *createNode(const QString &name) const;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  void createObjsGraph(PlotObjs &objs) const;

  void createGraph() const;

  void clearGraph();

  void placeDepthNodes() const;
  void placeDepthSubNodes(int xpos, const Nodes &nodes) const;

  void calcValueMarginScale();

  double calcNodeMargin() const;

  NodeObj *createObjFromNode(Node *node) const;

  int calcXPos(Node *node) const;

  EdgeObj *addEdgeObj(Edge *edge) const;

  //---

  void updateGraphMaxDepth(const Nodes &nodes) const;

  bool adjustNodes() const;
  bool adjustGraphNodes(const Nodes &nodes) const;

  void initPosNodesMap(const Nodes &nodes) const;

  bool adjustNodeCenters() const;

  bool adjustEdgeOverlaps() const;

  bool removeOverlaps() const;

  bool removePosOverlaps(const Nodes &nodes) const;

  void spreadPosNodes(const Nodes &nodes) const;

  bool reorderNodeEdges(const Nodes &nodes) const;

  void createPosNodeMap(const Nodes &nodes, PosNodeMap &posNodeMap) const;
  void createPosEdgeMap(const Edges &edges, PosEdgeMap &posEdgeMap, bool isSrc) const;

  bool adjustNode(Node *node) const;

  //---

  int minNodeMargin() const { return minNodeMargin_; }

  //---

  virtual NodeObj *createNodeObj(const BBox &rect, Node *node,
                                 const ColorInd &ig, const ColorInd &iv) const;
  virtual EdgeObj *createEdgeObj(const BBox &rect, Edge *edge) const;

  //---

  void printStats();

 protected:
  // options
  Align  align_       { Align::JUSTIFY }; //!< align
  int    alignRand_   { 10 };             //!< number of random values for align
  bool   adjustNodes_ { true };           //!< adjust nodes
  bool   adjustText_  { false };          //!< adjust text position
  double nodeMargin_  { 0.2 };            //!< node margin (y)
  double nodeWidth_   { 16 };             //!< node x width in pixels
  bool   edgeLine_    { false };          //!< draw line for edge
  BBox   targetBBox_  { -1, -1, 1, 1 };   //!< target range bbox

  // text visible
  bool insideTextVisible_   { false }; //!< is inside text visble (when text invisible)
  bool selectedTextVisible_ { false }; //!< is selected text visble (when text invisible)

  // coloring
  bool           srcColoring_   { false };                    //!< color by source nodes
  ConnectionType mouseColoring_ { ConnectionType::ALL_DEST }; //!< mouse over connections

  // data
  NameNodeMap nameNodeMap_;               //!< name node map
  IndNodeMap  indNodeMap_;                //!< ind node map
  Graph*      graph_         { nullptr }; //!< graph
  Edges       edges_;                     //!< edges
  BBox        bbox_;                      //!< bbox
  int         maxNodeDepth_  { 0 };       //!< max node depth (all graphs)
  double      minNodeMargin_ { 4 };       //!< minimum node margin (in pixels)
  double      boxMargin_     { 0.01 };    //!< bounding box margin
  double      edgeMargin_    { 0.01 };    //!< edge bounding box margin
  bool        useMaxTotals_  { true };    //!< use max total for node src/dest scaling
  bool        pressed_       { false };   //!< mouse pressed

 public:
  struct DrawText : public CQChartsRectPlacer::RectData {
    using Rect = CQChartsRectPlacer::Rect;

    DrawText(const QString &str, const Point &point, const CQChartsTextOptions &options,
             const QColor &color, const Alpha &alpha, const Point &targetPoint) :
     str(str), point(point), options(options), color(color), alpha(alpha),
     targetPoint(targetPoint) {
      origPoint = point;
    }

    void setBBox(const BBox &bbox) {
      textRect = Rect(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
      origRect = textRect;
    }

    const Rect &rect() const override {
      return textRect;
    }

    void setRect(const Rect &r) override {
      textRect = r;

      double dx = textRect.xmin() - origRect.xmin();
      double dy = textRect.ymin() - origRect.ymin();

      point = Point(origPoint.x + dx, origPoint.y + dy);
    }

    QString             str;
    Point               point;
    Point               origPoint;
    CQChartsTextOptions options;
    QColor              color;
    Alpha               alpha;
    Rect                textRect;
    Rect                origRect;
    Point               targetPoint;
  };

  void addDrawText(DrawText *drawText) const {
    drawTexts_.push_back(drawText);
  }

 private:
  using DrawTexts = std::vector<DrawText *>;

  mutable DrawTexts drawTexts_;
};

#endif
