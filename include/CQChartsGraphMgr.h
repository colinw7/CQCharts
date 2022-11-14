#ifndef CQChartsGraphMgr_H
#define CQChartsGraphMgr_H

#include <CQChartsLength.h>
#include <CQChartsOptReal.h>
#include <CQChartsColor.h>
#include <CQChartsAlpha.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsModelIndex.h>
#include <CQChartsBoxObj.h>

#include <map>
#include <set>
#include <vector>

class CQChartsPlot;
class CQChartsGraphGraph;
class CQChartsGraphNode;
class CQChartsGraphEdge;

class CQChartsGraphMgr {
 public:
  using Plot    = CQChartsPlot;
  using Graph   = CQChartsGraphGraph;
  using Node    = CQChartsGraphNode;
  using Edge    = CQChartsGraphEdge;
  using Edges   = std::vector<Edge *>;
  using Length  = CQChartsLength;
  using Units   = CQChartsUnits::Type;
  using OptReal = CQChartsOptReal;

  enum class Align {
    SRC,
    DEST,
    JUSTIFY,
    LARGEST,
    RAND
  };

  enum NodeShape {
    NODE_SHAPE_NONE,
    NODE_SHAPE_DIAMOND,
    NODE_SHAPE_BOX,
    NODE_SHAPE_POLYGON,
    NODE_SHAPE_CIRCLE,
    NODE_SHAPE_DOUBLE_CIRCLE
  };

 public:
  using Graphs = std::map<int, Graph *>;

 public:
  CQChartsGraphMgr(Plot *plot);

  virtual ~CQChartsGraphMgr();

  Plot *plot() const;

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o) { orientation_ = o; }

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }
  bool isVertical  () const { return orientation() == Qt::Vertical  ; }

  //---

  const Graphs &graphs() const { return graphs_; }

  bool hasGraph(int graphId) const;

  void addGraph(int graphId, Graph *graph);

  Graph *getOrCreateGraph(int graphId, int parentGraphId) const;

  virtual Graph *createGraph(const QString &name) const;

  Graph *getGraph(int graphId) const;

  void clearGraphs();

  //---

  Node *addNode(const QString &name) const;
  void addNode(Node *node) const;

  Edge *addEdge(const OptReal &value, Node *srcNode, Node *destNode) const;
  void addEdge(Edge *edge) const;

  Node *findNode(const QString &name) const;

  virtual Node *createNode(const QString &name) const;
  virtual Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  //---

  //! get/set node align
  const Align &align() const { return align_; }
  void setAlign(const Align &a) { align_ = a; }

  //! get/set node align first/last
  bool isAlignFirstLast() const { return alignFirstLast_; }
  void setAlignFirstLast(bool b) { alignFirstLast_ = b; }

  //! get/set number of random values for align
  int alignRand() const { return alignRand_; }
  void setAlignRand(int i) { alignRand_ = i; }

  //---

  //! get/set is node parallel scaled (x)
  bool isNodeScaled() const { return nodeScaled_; }
  void setNodeScaled(bool b) { nodeScaled_ = b; }

  //! get/set is node perp scaled (y)
  bool isNodePerpScaled() const { return nodePerpScaled_; }
  void setNodePerpScaled(bool b) { nodePerpScaled_ = b; }

  //! get/set is node perp scaled to node value
  bool isNodeValueScaled() const { return nodeValueScaled_; }
  void setNodeValueScaled(bool b) { nodeValueScaled_ = b; }

  //---

  //! get/set is adjust nodes
  bool isAdjustNodes() const { return adjustNodes_; }
  void setAdjustNodes(bool b) { adjustNodes_ = b; }

  //! get/set adjust center
  bool isAdjustCenters() const { return adjustCenters_; }
  void setAdjustCenters(bool b) { adjustCenters_ = b; }

  //! get/set remove overlaps
  bool isRemoveOverlaps() const { return removeOverlaps_; }
  void setRemoveOverlaps(bool b) { removeOverlaps_ = b; }

  //! get/set reorder edges
  bool isReorderEdges() const { return reorderEdges_; }
  void setReorderEdges(bool b) { reorderEdges_ = b; }

  //! get/set adjust iterations
  int adjustIterations() const { return adjustIterations_; }
  void setAdjustIterations(int i) { adjustIterations_ = i; }

  //! get/set node width
  const Length &nodeWidth() const { return nodeWidth_; }
  void setNodeWidth(const Length &l) { nodeWidth_ = l; }

  //! get/set node height
  const Length &nodeHeight() const { return nodeHeight_; }
  void setNodeHeight(const Length &l) { nodeHeight_ = l; }

  //! get/set node margin (perp)
  const Length &nodeMargin() const { return nodeMargin_; }
  void setNodeMargin(const Length &l) { nodeMargin_ = l; }

  //! get/set node margin (parallel)
  const Length &nodeSpacing() const { return nodeSpacing_; }
  void setNodeSpacing(const Length &l) { nodeSpacing_ = l; }

  //! get/set node shape
  const NodeShape &nodeShape() { return nodeShape_; }
  void setNodeShape(const NodeShape &s) { nodeShape_ = s; }

  //---

  int minNodeMargin() const { return minNodeMargin_; }

  //---

  int numNodes() const { return int(nameNodeMap_.size()); }

  void clearNodesAndEdges();

 private:
  using PlotP       = QPointer<Plot>;
  using NameNodeMap = std::map<QString, Node *>;
  using IndNodeMap  = std::map<int, Node *>;

  PlotP           plot_;                                    //!< parent plot
  Graphs          graphs_;                                  //!< graphs
  Qt::Orientation orientation_      { Qt::Vertical };       //!< orientation
  Align           align_            { Align::SRC };         //!< align
  bool            alignFirstLast_   { true };               //!< align first/last
  int             alignRand_        { 10 };                 //!< number of random values for align
  bool            nodeScaled_       { false };              //!< is node parallel scaled
  bool            nodePerpScaled_   { true };               //!< is node perp scaled
  bool            nodeValueScaled_  { false };              //!< is node perp size scaled by value
  bool            adjustNodes_      { true };               //!< adjust nodes
  bool            adjustCenters_    { true };               //!< adjust centers
  bool            removeOverlaps_   { true };               //!< remove overlaps
  bool            reorderEdges_     { true };               //!< reorder edges
  int             adjustIterations_ { 25 };                 //!< number of adjust iterations
  Length          nodeWidth_        { Length::pixel(64) };  //!< node width
  Length          nodeHeight_       { Length::pixel(64) };  //!< node height
  Length          nodeMargin_       { Length::plot(0.05) }; //!< node margin (perp)
  Length          nodeSpacing_      { Length::plot(0.05) }; //!< node spacing (parallel)
  NodeShape       nodeShape_        { NODE_SHAPE_NONE };    //!< node shape
  int             minNodeMargin_    { 4 };                  //!< minimum node margin (in pixels)
  NameNodeMap     nameNodeMap_;                             //!< name node map
  IndNodeMap      indNodeMap_;                              //!< ind node map
  Edges           edges_;                                   //!< all edges
};

//---

/*!
 * \brief Graph plot node
 * \ingroup Charts
 */
class CQChartsGraphNode {
 public:
  using GraphMgr    = CQChartsGraphMgr;
  using Node        = CQChartsGraphNode;
  using NodeSet     = std::set<const Node *>;
  using Edge        = CQChartsGraphEdge;
  using Edges       = std::vector<Edge *>;
  using Graph       = CQChartsGraphGraph;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using LineDash    = CQChartsLineDash;
  using FillPattern = CQChartsFillPattern;
  using OptReal     = CQChartsOptReal;
  using ModelIndex  = CQChartsModelIndex;
  using ModelInds   = std::vector<ModelIndex>;
  using BBox        = CQChartsGeom::BBox;
  using EdgeRect    = std::map<Edge *, BBox>;
  using Point       = CQChartsGeom::Point;
  using ShapeType   = CQChartsBoxObj::ShapeType;

 public:
  CQChartsGraphNode(GraphMgr *mgr, const QString &str);

  virtual ~CQChartsGraphNode();

  //---

  //! get/set edge parent
  Node *parent() const { return parent_; }
  void setParent(Node *parent) { parent_ = parent; }

  //! set string (name ?)
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

  //---

  //! get/set source edges
  const Edges &srcEdges() const { return srcEdges_; }
  void setSrcEdges(const Edges &edges) { srcEdges_ = edges; }

  //! get/set destination edges
  const Edges &destEdges() const { return destEdges_; }
  void setDestEdges(const Edges &edges) { destEdges_ = edges; }

  //! add source edge
  void addSrcEdge(Edge *edge, bool primary=true);

  //! add destination edge
  void addDestEdge(Edge *edge, bool primary=true);

  //! has destination edge
  bool hasDestNode(Node *destNode) const;

  //---

  //! get/set fixed
  bool isFixed() const { return fixed_; }
  void setFixed(bool b) { fixed_ = b; }

  //---

  //! get source depth (from connections)
  virtual int srcDepth () const;
  //! get destination depth (from connections)
  virtual int destDepth() const;

  //---

  //--- custom appearance

  //! get/set shape
  ShapeType shapeType() const { return static_cast<ShapeType>(shapeTypeData_.shapeType); }
  void setShapeType(const ShapeType &s) {
    shapeTypeData_.shapeType = static_cast<CQChartsShapeType>(s); }

  //! get/set number of sides
  int numSides() const { return shapeTypeData_.numSides; }
  void setNumSides(int n) { shapeTypeData_.numSides = n; }

  //! get/set aspect
  double aspect() const { return shapeTypeData_.aspect; }
  void setAspect(double r) { shapeTypeData_.aspect = r; }

  //! get/set fill color
  const Color &fillColor() const { return fillData_.color; }
  void setFillColor(const Color &c) { fillData_.color = c; }

  //! get/set fill alpha
  const Alpha &fillAlpha() const { return fillData_.alpha; }
  void setFillAlpha(const Alpha &c) { fillData_.alpha = c; }

  //! get/set fill pattern
  const FillPattern &fillPattern() const { return fillData_.pattern; }
  void setFillPattern(const FillPattern &c) { fillData_.pattern = c; }

  //! get/set stroke color
  const Color &strokeColor() const { return strokeData_.color; }
  void setStrokeColor(const Color &c) { strokeData_.color = c; }

  //! get/set stroke alpha
  const Alpha &strokeAlpha() const { return strokeData_.alpha; }
  void setStrokeAlpha(const Alpha &a) { strokeData_.alpha = a; }

  //! get/set stroke width
  const Length &strokeWidth() const { return strokeData_.width; }
  void setStrokeWidth(const Length &w) { strokeData_.width = w; }

  //! get/set stroke dash
  const LineDash &strokeDash() const { return strokeData_.dash; }
  void setStrokeDash(const LineDash &d) { strokeData_.dash = d; }

  //---

  // get/set x pos
  int pos() const { return pos_; }
  void setPos(int pos) { pos_ = pos; }

  // get/set rect
  virtual const BBox &rect() const;
  virtual void setRect(const BBox &r);

  //---

  // get graph (if any)
  Graph *graph() const;

  //---

  const ModelInds &modelInds() const { return modelInds_; }

  void addModelInd(const ModelIndex &i) { modelInds_.push_back(i); }

  //---

  //! get edge (max) sum
  double edgeSum() const;

  //! get source edge (max) sum
  double srcEdgeSum () const;

  //! get destination edge (max) sum
  double destEdgeSum() const;

  //---

  //! is placed
  virtual bool isPlaced() const { return true; }

  //! place
  virtual void place(const BBox &) const { assert(false); }

  //! move node by delta
  virtual void moveBy(const Point &delta);

  //! scale node by factor
  virtual void scale(double fx, double fy);

  //---

  void placeEdges();

  //---

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

 protected:
  friend class CQChartsGraphGraph;

  //! calc src/destination depth
  int calcSrcDepth (NodeSet &visited) const;
  int calcDestDepth(NodeSet &visited) const;

  bool isCalculating() const { return calculating_; }
  void setCalculating(bool b) { calculating_ = b; }

 protected:
  struct FillData {
    Color       color;   //!< fill color
    Alpha       alpha;   //!< fill alpha
    FillPattern pattern; //!< fill pattern
  };

  struct StrokeData {
    Color    color; //!< stroke color
    Alpha    alpha; //!< stroke color alpha
    Length   width; //!< stroke width
    LineDash dash;  //!< stroke dash
  };

  GraphMgr* mgr_           { nullptr }; //!< graph manager
  Node*     parent_        { nullptr }; //!< parent node
  QString   str_;                       //!< string
  int       id_            { -1 };      //!< id
  bool      visible_       { true };    //!< is visible
  QString   name_;                      //!< name
  QString   label_;                     //!< label
  OptReal   value_;                     //!< value
  int       group_         { -1 };      //!< group
  int       ngroup_        { 0 };       //!< number of groups
  int       depth_         { -1 };      //!< depth
  int       graphId_       { -1 };      //!< graph id
  int       parentGraphId_ { -1 };      //!< parent graph id

  bool fixed_ { false }; //!< fixed

  ModelIndex ind_; //!< model index

  // appearance
  CQChartsShapeTypeData shapeTypeData_; //!< shape type data

  FillData   fillData_;   //!< fill data
  StrokeData strokeData_; //!< stroke data

  // connections
  Edges     srcEdges_;                  //!< source edges
  Edges     destEdges_;                 //!< destination edges
  Edges     nonPrimaryEdges_;           //!< non-primary edges
  int       srcDepth_      { -1 };      //!< source depth (calculated)
  int       destDepth_     { -1 };      //!< destination depth (calculated)
  int       pos_           { -1 };      //!< position
  BBox      rect_;                      //!< placed rectangle
  ModelInds modelInds_;                 //!< model inds
  EdgeRect  srcEdgeRect_;               //!< edge to src
  EdgeRect  destEdgeRect_;              //!< edge to dest

  bool calculating_ { false };
};

//---

/*!
 * \brief Graph plot edge
 * \ingroup Charts
 */
class CQChartsGraphEdge {
 public:
  enum class ShapeType {
    NONE,
    ARC,
    ARROW
  };

  using GraphMgr   = CQChartsGraphMgr;
  using Node       = CQChartsGraphNode;
  using ModelIndex = CQChartsModelIndex;
  using ModelInds  = std::vector<ModelIndex>;
  using Column     = CQChartsColumn;
  using Color      = CQChartsColor;
  using OptReal    = CQChartsOptReal;

 public:
  CQChartsGraphEdge(GraphMgr *mgr, const OptReal &value, Node *srcNode, Node *destNode);

  virtual ~CQChartsGraphEdge();

  //---

  //! get/set unique id
  int id() const { return id_; }
  void setId(int id) { id_ = id; }

  //! get/set value
  bool hasValue() const { return value_.isSet(); }
  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r) { value_ = r; }

  //---

  //! get/set named column
  bool hasNamedColumn(const QString &name) const {
    return (namedColumn_.find(name) != namedColumn_.end());
  }

  Column namedColumn(const QString &name) const {
    auto p = namedColumn_.find(name);
    return (p != namedColumn_.end() ? (*p).second : Column());
  }

  void setNamedColumn(const QString &name, const Column &c) { namedColumn_[name] = c; }

  //---

  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  //---

  //--- custom appearance

  //! get/set shape
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s) { shapeType_ = s; }

  //! get/set fill color
  const Color &fillColor() const { return color_; }
  void setFillColor(const Color &c) { color_ = c; }

  //---

  const ModelInds &modelInds() const { return modelInds_; }

  void addModelInd(const ModelIndex &i) { modelInds_.push_back(i); }

  //---

  // get source node
  Node *srcNode() const { return srcNode_; }

  // get destination node
  Node *destNode() const { return destNode_; }

  // is self connect
  bool isSelf() const { return srcNode() == destNode(); }

  //---

 protected:
  using NamedColumn = std::map<QString, Column>;

  GraphMgr*   mgr_       { nullptr };         //!< graph manager
  int         id_        { -1 };              //!< unique id
  OptReal     value_;                         //!< value
  NamedColumn namedColumn_;                   //!< named columns
  QString     label_;                         //!< label
  ShapeType   shapeType_ { ShapeType::NONE }; //!< shape type
  Color       color_;                         //!< color
  ModelInds   modelInds_;                     //!< model inds
  Node*       srcNode_   { nullptr };         //!< source node
  Node*       destNode_  { nullptr };         //!< destination node
};

//---

/*!
 * \brief Graph Plot Graph (Edge/Node/Sub Graph container)
 * \ingroup Charts
 */
class CQChartsGraphGraph : public CQChartsGraphNode {
 public:
  using Node  = CQChartsGraphNode;
  using Nodes = std::vector<Node *>;

  //! data for nodes at depth
  struct DepthData {
    Nodes  nodes;        //!< nodes at depth
    double size { 0.0 }; //!< total size
  };

  using GraphMgr      = CQChartsGraphMgr;
  using DepthNodesMap = std::map<int, DepthData>;
  using PosNodesMap   = std::map<int, Nodes>;
  using Graph         = CQChartsGraphGraph;
  using Graphs        = std::vector<Graph *>;
  using Edge          = CQChartsGraphEdge;

  struct NodePos {
    double pos { 0.0 };
    int    id  { -1 };

    NodePos(double pos, int id) :
     pos(pos), id(id) {
    }

    friend bool operator<(const NodePos &pos1, const NodePos &pos2) {
      if (std::fabs(pos1.pos - pos2.pos) >= 1E-4)
        return pos1.pos < pos2.pos;

      return (pos1.id < pos2.id);
    }

    friend bool operator==(const NodePos &pos1, const NodePos &pos2) {
      if (std::fabs(pos1.pos - pos2.pos) >= 1E-4)
        return false;

      return (pos1.id == pos2.id);
    }
  };

  using PosNodeMap = std::map<NodePos, Node *>;
  using PosEdgeMap = std::map<NodePos, Edge *>;

 public:
  CQChartsGraphGraph(GraphMgr *mgr, const QString &name);

  virtual ~CQChartsGraphGraph();

  //---

  //! get/set parent
  Graph *parent() const { return parent_; }
  void setParent(Graph *graph) { parent_ = graph; }

  //---

  void addChild(Graph *graph);

  const Graphs &children() const { return children_; }

  const Nodes &nodes() const { return nodes_; }
  void addNode(Node *node);

  void removeAllNodes();

  //--

  //! get/set min node depth
  int minNodeDepth() const { return minNodeDepth_; }
  void setMinNodeDepth(int i) { minNodeDepth_ = i; }

  //! get/set max node depth (parallel)
  int maxNodeDepth() const { return maxNodeDepth_; }
  void setMaxNodeDepth(int i) { maxNodeDepth_ = i; }

  //! get/set max height (perp)
  int maxHeight() const { return maxHeight_; }
  void setMaxHeight(int i) { maxHeight_ = i; }

  //! get/set total size (perp)
  double totalSize() const { return totalSize_; }
  void setTotalSize(double r) { totalSize_ = r; }

  //----

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
  void addPosNode(Node *node) { posNodesMap_[node->pos()].push_back(node); }

  //! has nodes at pos
  bool hasPosNodes(int pos) const { return (posNodesMap_.find(pos) != posNodesMap_.end()); }

  //! get nodes at pos
  const Nodes &posNodes(int pos) const {
    auto p = posNodesMap_.find(pos); assert(p != posNodesMap_.end()); return (*p).second; }

  //---

  //! update rectangle from node
  virtual void updateRect();

  //---

  void placeGraph(const BBox &bbox) const;

  void placeNodes(const Nodes &nodes) const;

  void updateMaxDepth(const Nodes &nodes) const;

  int calcPos(Node *node) const;

  void calcValueMarginScale();

  double calcNodeMargin() const;
  double calcNodeSpacing() const;

  void placeDepthNodes() const;
  void placeDepthSubNodes(int pos, const Nodes &nodes) const;

  bool adjustGraphNodes(const Nodes &nodes) const;

  void initPosNodesMap(const Nodes &nodes) const;

  bool adjustNodeCenters() const;

  bool adjustNodeCentersLtoR() const;
  bool adjustNodeCentersRtoL() const;

  bool reorderNodeEdges(const Nodes &nodes) const;

  bool adjustPosNodes(int pos) const;

  bool adjustNode(Node *node) const;

  bool removeOverlaps() const;

  bool removePosOverlaps(const Nodes &nodes) const;

  bool spreadPosNodes(const Nodes &nodes) const;

  void createPosNodeMap(const Nodes &nodes, PosNodeMap &posNodeMap) const;
  void createPosEdgeMap(const Edges &edges, PosEdgeMap &posEdgeMap, bool isSrc) const;

  //---

  //! get nodes to place
  Nodes placeableNodes() const;

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
  void place(const BBox &bbox) const override;

  //! move node by delta
  void moveBy(const Point &delta) override;

  //! scale node by factor
  void scale(double fx, double fy) override;

 protected:
  Graph*        parent_       { nullptr }; //!< parent graph
  Graphs        children_;                 //!< child graphs
  Nodes         nodes_;                    //!< nodes in graph
  bool          placed_       { false };   //!< is placed
  BBox          bbox_;                     //!< place bbox
  int           minNodeDepth_ { 0 };       //!< min depth of nodes
  int           maxNodeDepth_ { 0 };       //!< max depth of nodes
  int           maxHeight_    { 0 };       //!< max height of all depth nodes
  double        totalSize_    { 0.0 };     //!< total size of all depth nodes
  DepthNodesMap depthNodesMap_;            //!< nodes data at depth
  PosNodesMap   posNodesMap_;              //!< pos node map
  double        valueMargin_  { 0.0 };     //!< y value margin
  double        valueScale_   { 1.0 };     //!< y value scale
};

#endif
