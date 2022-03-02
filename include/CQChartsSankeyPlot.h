#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

class CQChartsTextPlacer;

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
  using Node        = CQChartsSankeyPlotNode;
  using NodeSet     = std::set<const Node *>;
  using Edge        = CQChartsSankeyPlotEdge;
  using Edges       = std::vector<Edge *>;
  using EdgeSet     = std::set<const Edge *>;
  using Plot        = CQChartsSankeyPlot;
  using Obj         = CQChartsSankeyNodeObj;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using LineDash    = CQChartsLineDash;
  using FillPattern = CQChartsFillPattern;
  using Image       = CQChartsImage;
  using OptReal     = CQChartsOptReal;
  using ModelIndex  = CQChartsModelIndex;
  using ModelInds   = std::vector<ModelIndex>;
  using BBox        = CQChartsGeom::BBox;
  using EdgeRect    = std::map<Edge *, BBox>;
  using Point       = CQChartsGeom::Point;
#ifdef CQCHARTS_GRAPH_PATH_ID
  using PathIdRect  = std::map<int, BBox>;
#endif
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsSankeyPlotNode(const Plot *plot, const QString &str);

  virtual ~CQChartsSankeyPlotNode();

  //! get plot
  const Plot *plot() const { return plot_; }

  //! get/set edge parent
  Node *parent() const { return parent_; }
  void setParent(Node *parent) { parent_ = parent; }

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
  const Edges &srcEdges() const { return srcEdges_; }
  void setSrcEdges(const Edges &edges) { srcEdges_  = edges; }

  //! get/set destination edges
  const Edges &destEdges() const { return destEdges_; }
  void setDestEdges(const Edges &edges) { destEdges_ = edges; }

  //! add source edge
  void addSrcEdge(Edge *edge, bool primary=true);

  //! add destination edge
  void addDestEdge(Edge *edge, bool primary=true);

#ifdef CQCHARTS_GRAPH_PATH_ID
  //! sort edges path id
  void sortPathIdEdges(bool force=false);
#endif

#ifdef CQCHARTS_GRAPH_PATH_ID
  int minPathId() const;
#endif

  //! has destination edge
  bool hasDestNode(CQChartsSankeyPlotNode *destNode) const;

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  //! get/set selected
  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  //---

  //! get source depth (from connections)
  int srcDepth() const;
  //! get destination depth (from connections)
  int destDepth() const;

  //---

  //--- custom appearance

  //! get/set fill color
  const Color &fillColor() const { return fillData_.color; }
  void setFillColor(const Color &c) { fillData_.color = c; }

  //! get/set fill alpha
  const Alpha &fillAlpha() const { return fillData_.alpha; }
  void setFillAlpha(const Alpha &c) { fillData_.alpha = c; }

  //! get/set fill pattern
  const FillPattern &fillPattern() const { return fillData_.pattern; }
  void setFillPattern(const FillPattern &c) { fillData_.pattern = c; }

  //--

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

  const Image &image() const { return image_; }
  void setImage(const Image &i) { image_ = i; }

  //---

  // get/set position
  int pos() const { return pos_; }
  void setPos(int x) { assert(x >= 0); pos_ = x; }

  // get/set rect
  const BBox &rect() const;
  void setRect(const BBox &r);

  //---

  const ModelInds &modelInds() const { return modelInds_; }
  void addModelInd(const ModelIndex &i) { modelInds_.push_back(i); }

  //---

  //! get edge (max) sum
  double edgeSum() const;

  //! get source edge (max) sum
  double srcEdgeSum() const;

  //! get destination edge (max) sum
  double destEdgeSum() const;

  //---

  //! move node by delta
  void moveBy(const Point &delta);

  //! scale node by factor
  void scale(double fx, double fy);

  //---

  void placeEdges(bool reset);

  //---

  void clearSrcEdgeRects() { srcEdgeRect_.clear(); }

  void setSrcEdgeRect(Edge *edge, const BBox &bbox);

  bool hasSrcEdgeRect(Edge *edge) const {
    auto p = srcEdgeRect_.find(edge);

    return (p != srcEdgeRect_.end());
  }

  BBox srcEdgeRect(Edge *edge) const {
    auto p = srcEdgeRect_.find(edge);
    //assert(p != srcEdgeRect_.end());
    if (p == srcEdgeRect_.end()) return BBox();

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

  BBox destEdgeRect(Edge *edge) const {
    auto p = destEdgeRect_.find(edge);
    //assert(p != destEdgeRect_.end());
    if (p == destEdgeRect_.end()) return BBox();

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

  void adjustPathIdSrcDestRects();

  //---

  void allSrcNodesAndEdges (NodeSet &nodeSet, EdgeSet &edgeSet) const;
  void allDestNodesAndEdges(NodeSet &nodeSet, EdgeSet &edgeSet) const;

  //---

  QColor calcColor() const;

 protected:
  //! calc src/destination depth
  int calcSrcDepth (NodeSet &visited) const;
  int calcDestDepth(NodeSet &visited) const;

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

  const Plot* plot_    { nullptr }; //!< associated plot
  Node*       parent_  { nullptr }; //!< parent node (source node for dest node)
  QString     str_;                 //!< string
  int         id_      { -1 };      //!< id
  bool        visible_ { true };    //!< is visible
  ModelIndex  ind_;                 //!< model index
  QString     name_;                //!< name
  QString     label_;               //!< label
  OptReal     value_;               //!< value
  int         group_   { -1 };      //!< group index
  int         ngroup_  { 0 };       //!< number of groups
  int         depth_   { -1 };      //!< depth

  // appearance
  FillData   fillData_;   //!< fill data
  StrokeData strokeData_; //!< stroke data
  Image      image_;      //!< image

  // connections
  Edges       srcEdges_;                    //!< source edges
  Edges       destEdges_;                   //!< destination edges
  Edges       nonPrimaryEdges_;             //!< non-primary edges
  int         srcDepth_        { -1 };      //!< source depth (calculated)
  int         destDepth_       { -1 };      //!< destination depth (calculated)
  int         pos_            { -1 };       //!< position
  BBox        rect_;                        //!< placed rectangle
  ModelInds   modelInds_;                   //!< model inds
  EdgeRect    srcEdgeRect_;                 //!< edge to src
  EdgeRect    destEdgeRect_;                //!< edge to dest
#ifdef CQCHARTS_GRAPH_PATH_ID
  PathIdRect  srcPathIdRect_;               //!< src path id rect
  PathIdRect  destPathIdRect_;              //!< dest path id rect
#endif
  Obj*        obj_             { nullptr }; //!< plot object
  bool        selected_        { false };   //!< is selected
};

//---

/*!
 * \brief Sankey plot edge
 * \ingroup Charts
 */
class CQChartsSankeyPlotEdge {
 public:
  using Plot       = CQChartsSankeyPlot;
  using Edge       = CQChartsSankeyPlotEdge;
  using Node       = CQChartsSankeyPlotNode;
  using Obj        = CQChartsSankeyEdgeObj;
  using ModelIndex = CQChartsModelIndex;
  using ModelInds  = std::vector<ModelIndex>;
  using Column     = CQChartsColumn;
  using Color      = CQChartsColor;
  using OptReal    = CQChartsOptReal;
  using BBox       = CQChartsGeom::BBox;
  using Point      = CQChartsGeom::Point;

 public:
  CQChartsSankeyPlotEdge(const Plot *plot, const OptReal &value, Node *srcNode, Node *destNode);

  virtual ~CQChartsSankeyPlotEdge();

  //---

  //! get plot
  const Plot *plot() const { return plot_; }

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

#if 0
  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }
#endif

  //---

  // -- custom appearance --

  //! get/set fill color
  const Color &fillColor() const { return color_; }
  void setFillColor(const Color &c) { color_ = c; }

  //---

#ifdef CQCHARTS_GRAPH_PATH_ID
  //! get/set path id
  int pathId() const { return pathId_; }
  void setPathId(int i) { pathId_ = i; }
#endif

  //---

  const QModelIndex &modelInd() const { return modelInd_; }
  void setModelInd(const QModelIndex &v) { modelInd_ = v; }

  const ModelInds &modelInds() const { return modelInds_; }
  void addModelInd(const ModelIndex &i) { modelInds_.push_back(i); }

  //---

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
  using NamedColumn = std::map<QString, Column>;

  const Plot* plot_     { nullptr }; //!< plot
  int         id_       { -1 };      //!< unique id
  OptReal     value_;                //!< value
  NamedColumn namedColumn_;          //!< named columns
//QString     label_;                //!< label
  Color       color_;                //!< color
#ifdef CQCHARTS_GRAPH_PATH_ID
  int         pathId_   { -1 };      //!< path id
#endif
  QModelIndex modelInd_;             //!< original row model index
  ModelInds   modelInds_;            //!< model inds
  Node*       srcNode_  { nullptr }; //!< source node
  Node*       destNode_ { nullptr }; //!< destination node
  Obj*        obj_      { nullptr }; //!< associated edge object
  bool        isLine_   { false };   //!< is edge a line
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

  //! data for nodes at depth
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

  //---

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

  int minNodeX() const {
    if (depthNodesMap_.empty()) return 0;

    return depthNodesMap_.begin()->first;
  }

  int maxNodeX() const {
    if (depthNodesMap_.empty()) return 0;

    return depthNodesMap_.rbegin()->first;
  }

  //---

  //! get depth nodes
  const DepthNodesMap &depthNodesMap() const { return depthNodesMap_; }
  DepthNodesMap &depthNodesMap() { return depthNodesMap_; }

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
  PosNodesMap   posNodesMap_;              //!< pos node map (needed ?)
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

  Q_PROPERTY(QString       hierName  READ hierName  WRITE setHierName )
  Q_PROPERTY(QString       name      READ name      WRITE setName     )
  Q_PROPERTY(double        value     READ value     WRITE setValue    )
  Q_PROPERTY(int           depth     READ depth     WRITE setDepth    )
  Q_PROPERTY(CQChartsColor fillColor READ fillColor WRITE setFillColor)

 public:
  using Plot  = CQChartsSankeyPlot;
  using Node  = CQChartsSankeyPlotNode;
  using Edge  = CQChartsSankeyPlotEdge;
  using Angle = CQChartsAngle;

 public:
  CQChartsSankeyNodeObj(const Plot *plot, const BBox &rect, Node *node,
                        const ColorInd &ig, const ColorInd &iv);

  virtual ~CQChartsSankeyNodeObj();

  //---

  //! typename
  QString typeName() const override { return "node"; }

  //---

  //! get plot
  const Plot *plot() const { return plot_; }

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

  //! get/set color
  CQChartsColor fillColor() const;
  void setFillColor(const CQChartsColor &c);

  //---

  //! get unique id
  QString calcId() const override;

  //! get tip string
  QString calcTipId() const override;

  //---

  void setSelected(bool b) override { CQChartsPlotObj::setSelected(b); node_->setSelected(b); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! move node by delta
  void moveBy(const Point &delta);

  //! scale node by factor
  void scale(double fx, double fy);

  //---

  void placeEdges(bool reset);

  //---

  // Implement edit interface
  bool editPress  (const Point &p) override;
  bool editMove   (const Point &p) override;
  bool editMotion (const Point &p) override;
  bool editRelease(const Point &p) override;

  void setEditBBox(const BBox &bbox, const ResizeSide &) override;

  //---

  //! get select indices
  void getObjSelectIndices(Indices &inds) const override;

  //! get connected objects
  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) const override;

#ifdef CQCHARTS_GRAPH_PATH_ID
  void drawConnectionMouseOver(PaintDevice *device, int mouseColoring, int pathId=-1) const;
#else
  void drawConnectionMouseOver(PaintDevice *device, int mouseColoring) const;
#endif

  void drawFg(PaintDevice *device) const override;

  void drawFgRect(PaintDevice *device, const BBox &rect) const;

  void drawFgImage(PaintDevice *device, const BBox &rect) const;
  void drawFgText (PaintDevice *device, const BBox &rect) const;

  void drawValueLabel(PaintDevice *device, const BBox &rect) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  QColor calcFillColor() const;

 protected:
  const Plot* plot_        { nullptr }; //!< parent plot
  Node*       node_        { nullptr }; //!< node
  QString     hierName_;                //!< node hier name
  bool        editChanged_ { false };   //!< edit is changed
};

//---

/*!
 * \brief Sankey Plot Edge object
 * \ingroup Charts
 */
class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double value READ value)

 public:
  using Plot = CQChartsSankeyPlot;
  using Edge = CQChartsSankeyPlotEdge;
  using Node = CQChartsSankeyPlotNode;

 public:
  CQChartsSankeyEdgeObj(const Plot *plot, const BBox &rect, Edge *edge);

  virtual ~CQChartsSankeyEdgeObj();

  //---

  //! get plot
  const Plot *plot() const { return plot_; }

  //! get edge
  Edge *edge() const { return edge_; }

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

  double value() const { return (edge()->hasValue() ? edge()->value().real() : 0.0); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  //! is point inside
  bool inside(const Point &p) const override;

  //---

  //! get select indices
  void getObjSelectIndices(Indices &inds) const override;

  //! get connected objects
  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawConnectionMouseOver(PaintDevice *device, int mouseColoring) const;

#if 0
  void drawFg(PaintDevice *device) const override;
#endif

  bool edgePath(QPainterPath &path, bool isLine=false) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 protected:
  const Plot*          plot_      { nullptr }; //!< parent plot
  Edge*                edge_      { nullptr }; //!< edge
  BBox                 srcRect_;               //!< src rect
  BBox                 destRect_;              //!< dest rect
  mutable QPainterPath path_;                  //!< painter path
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
  Q_PROPERTY(CQChartsLength  nodeMargin    READ nodeMargin    WRITE setNodeMargin   )
  Q_PROPERTY(double          minNodeMargin READ minNodeMargin WRITE setMinNodeMargin)
  Q_PROPERTY(CQChartsLength  nodeWidth     READ nodeWidth     WRITE setNodeWidth    )
  Q_PROPERTY(bool            edgeLine      READ isEdgeLine    WRITE setEdgeLine     )
  Q_PROPERTY(Qt::Orientation orientation   READ orientation   WRITE setOrientation  )
  Q_PROPERTY(bool            valueLabel    READ isValueLabel  WRITE setValueLabel   )

  // coloring
  Q_PROPERTY(bool           srcColoring       READ isSrcColoring       WRITE setSrcColoring      )
  Q_PROPERTY(BlendType      blendEdgeColor    READ blendEdgeColor      WRITE setBlendEdgeColor   )
  Q_PROPERTY(ConnectionType mouseColoring     READ mouseColoring       WRITE setMouseColoring    )
  Q_PROPERTY(bool           mouseNodeColoring READ isMouseNodeColoring WRITE setMouseNodeColoring)

  // placement
  Q_PROPERTY(Align  align              READ align                WRITE setAlign             )
  Q_PROPERTY(bool   alignFirstLast     READ isAlignFirstLast     WRITE setAlignFirstLast    )
  Q_PROPERTY(Spread spread             READ spread               WRITE setSpread            )
  Q_PROPERTY(bool   alignEnds          READ isAlignEnds          WRITE setAlignEnds         )
#ifdef CQCHARTS_GRAPH_PATH_ID
  Q_PROPERTY(bool   sortPathIdNodes    READ isSortPathIdNodes    WRITE setSortPathIdNodes   )
  Q_PROPERTY(bool   sortPathIdEdges    READ isSortPathIdEdges    WRITE setSortPathIdEdges   )
#endif
  Q_PROPERTY(bool   adjustNodes        READ isAdjustNodes        WRITE setAdjustNodes       )
  Q_PROPERTY(bool   adjustCenters      READ isAdjustCenters      WRITE setAdjustCenters     )
  Q_PROPERTY(bool   removeOverlaps     READ isRemoveOverlaps     WRITE setRemoveOverlaps    )
  Q_PROPERTY(bool   reorderEdges       READ isReorderEdges       WRITE setReorderEdges      )
//Q_PROPERTY(bool   adjustEdgeOverlaps READ isAdjustEdgeOverlaps WRITE setAdjustEdgeOverlaps)
//Q_PROPERTY(bool   adjustSelected     READ isAdjustSelected     WRITE setAdjustSelected    )
  Q_PROPERTY(int    adjustIterations   READ adjustIterations     WRITE setAdjustIterations  )
  Q_PROPERTY(bool   adjustText         READ isAdjustText         WRITE setAdjustText        )
  Q_PROPERTY(bool   constrainMove      READ isConstrainMove      WRITE setConstrainMove     )

  // edge scaling
  Q_PROPERTY(bool useMaxTotals READ useMaxTotals WRITE setUseMaxTotals)

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // text visible on mouse inside/selected (when text invisible)
  Q_PROPERTY(bool insideTextVisible   READ isInsideTextVisible   WRITE setInsideTextVisible  )
  Q_PROPERTY(bool selectedTextVisible READ isSelectedTextVisible WRITE setSelectedTextVisible)

  // text style
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_PROPERTY(bool textInternal READ isTextInternal WRITE setTextInternal)

  // pos scale
  Q_PROPERTY(int minPos READ minPos)
  Q_PROPERTY(int maxPos READ maxPos)

  Q_ENUMS(ConnectionType)
  Q_ENUMS(Align)
  Q_ENUMS(Spread)
  Q_ENUMS(BlendType)

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
    LARGEST,
    RAND
  };

  enum class Spread {
    NONE,
    FIRST,
    LAST,
    FIRST_LAST,
    ALL
  };

  enum class BlendType {
    NONE,
    FILL_AVERAGE,
    STROKE_AVERAGE,
    FILL_STROKE_AVERAGE,
    FILL_GRADIENT
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
  using Length      = CQChartsLength;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using TextOptions = CQChartsTextOptions;
  using ColorInd    = CQChartsUtil::ColorInd;

  struct NodePerpPos {
    double perp   { 0.0 };
    int    id     { -1 };
#ifdef CQCHARTS_GRAPH_PATH_ID
    int    pathId { -1 };
#endif

#ifdef CQCHARTS_GRAPH_PATH_ID
    NodePerpPos(double perp, int id, int pathId=-1) :
     perp(perp), id(id), pathId(pathId) {
    }
#else
    NodePerpPos(double perp, int id) :
     perp(perp), id(id) {
    }
#endif

    friend bool operator<(const NodePerpPos &pos1, const NodePerpPos &pos2) {
      if (std::fabs(pos1.perp - pos2.perp) >= 1E-4)
        return pos1.perp < pos2.perp;

#ifdef CQCHARTS_GRAPH_PATH_ID
      if (pos1.pathId != pos2.pathId)
        return (pos1.pathId < pos2.pathId);
#endif

      return (pos1.id < pos2.id);
    }

    friend bool operator==(const NodePerpPos &pos1, const NodePerpPos &pos2) {
      if (std::fabs(pos1.perp - pos2.perp) >= 1E-4)
        return false;

#ifdef CQCHARTS_GRAPH_PATH_ID
      if (pos1.pathId != pos2.pathId)
        return false;
#endif

      return (pos1.id == pos2.id);
    }
  };

  using PosNodeMap = std::map<NodePerpPos, Node *>;
  using PosEdgeMap = std::map<NodePerpPos, Edge *>;

 public:
  CQChartsSankeyPlot(View *view, const ModelP &model);
 ~CQChartsSankeyPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set node y margin
  const Length &nodeMargin() const { return nodeMargin_; }
  void setNodeMargin(const Length &l);

  //! get/set min node y margin
  double minNodeMargin() const { return minNodeMargin_; }
  void setMinNodeMargin(double r);

  //---

  //! get/set x width
  const Length &nodeWidth() const { return nodeWidth_; }
  void setNodeWidth(const Length &l);

  //---

  //! get/set edge line
  bool isEdgeLine() const { return edgeLine_; }
  void setEdgeLine(bool b);

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o);

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }
  bool isVertical  () const { return orientation() == Qt::Vertical  ; }

  //---

  //! get/set show value label
  bool isValueLabel() const { return valueLabel_; }
  void setValueLabel(bool b);

  //---

  //! get/set node color from source nodes
  bool isSrcColoring() const { return srcColoring_; }
  void setSrcColoring(bool b);

  //! get/set blend node colors for edge
  const BlendType &blendEdgeColor() const { return blendEdgeColor_; }
  void setBlendEdgeColor(const BlendType &t);

  //! get/set mouse over node/edge coloring
  const ConnectionType &mouseColoring() const { return mouseColoring_; }
  void setMouseColoring(const ConnectionType &t) { mouseColoring_ = t; }

  //! get/set mouse over node coloring (edge)
  bool isMouseNodeColoring() const { return mouseNodeColoring_; }
  void setMouseNodeColoring(bool b) { mouseNodeColoring_ = b; }

  //---

  //! get/set node align
  const Align &align() const { return align_; }
  void setAlign(const Align &a);

  //! get/set node align first/last
  bool isAlignFirstLast() const { return alignFirstLast_; }
  void setAlignFirstLast(bool b);

  //! get/set node spread
  const Spread &spread() const { return spread_; }
  void setSpread(const Spread &s);

  //---

  //! get/set align ends
  bool isAlignEnds() const { return alignEnds_; }
  void setAlignEnds(bool b);

#ifdef CQCHARTS_GRAPH_PATH_ID
  //! get/set sort path id edges
  bool isSortPathIdNodes() const { return sortPathIdNodes_; }
  void setSortPathIdNodes(bool b);

  //! get/set sort path id edges
  bool isSortPathIdEdges() const { return sortPathIdEdges_; }
  void setSortPathIdEdges(bool b);
#endif

  //! get/set adjust nodes
  bool isAdjustNodes() const { return adjustNodes_; }
  void setAdjustNodes(bool b);

  //! get/set adjust center
  bool isAdjustCenters() const { return adjustCenters_; }
  void setAdjustCenters(bool b);

  //! get/set remove overlaps
  bool isRemoveOverlaps() const { return removeOverlaps_; }
  void setRemoveOverlaps(bool b);

  //! get/set reorder edges
  bool isReorderEdges() const { return reorderEdges_; }
  void setReorderEdges(bool b);

#if 0
  //! get/set adjust edge overlaps
  bool isAdjustEdgeOverlaps() const { return adjustEdgeOverlaps_; }
  void setAdjustEdgeOverlaps(bool b);
#endif

#if 0
  //! get/set adjust selected
  bool isAdjustSelected() const { return adjustSelected_; }
  void setAdjustSelected(bool b);
#endif

  //! get/set adjust iterations
  int adjustIterations() const { return adjustIterations_; }
  void setAdjustIterations(int n);

  //---

  //! get/set adjust text
  bool isAdjustText() const { return adjustText_; }
  void setAdjustText(bool b);

  //---

  //! get/set constraint move
  bool isConstrainMove() const { return constrainMove_; }
  void setConstrainMove(bool b) { constrainMove_ = b; }

  //---

  //! text visible on inside (when text invisible)
  bool isInsideTextVisible() const { return insideTextVisible_; }
  void setInsideTextVisible(bool b) { insideTextVisible_ = b; }

  //! text visible on selected (when text invisible)
  bool isSelectedTextVisible() const { return selectedTextVisible_; }
  void setSelectedTextVisible(bool b) { selectedTextVisible_ = b; }

  //---

  //! get/set use max of source/destination nodes for edge scaling
  bool useMaxTotals() const { return useMaxTotals_; }
  void setUseMaxTotals(bool b);

  //---

  //! text is internal to plot
  bool isTextInternal() const { return textInternal_; }
  void setTextInternal(bool b);

  //---

  // min/max X
  int minPos() const { return graph_->minNodeX(); }
  int maxPos() const { return graph_->maxNodeX(); }

  //---

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

  Range objTreeRange() const override;

  BBox nodesBBox() const;

  //---

  bool createObjs(PlotObjs &objs) const override;

  bool fitToBBox() const;

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

  int numNodes() const { return int(nameNodeMap_.size()); }

  int maxNodeDepth() const { return maxNodeDepth_; }

  //---

  bool keyPress(int key, int modifier) override;

  //---

  Graph *graph() const { return graph_; }

  void placeGraph(bool placed=false) const;
  void placeGraphNodes(const Nodes &nodes, bool placed=false) const;

  void placeEdges() const;

  void calcGraphNodesPos(const Nodes &nodes) const;

  //---

  void addObjects(PlotObjs &objs) const;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

#ifdef CQCHARTS_GRAPH_PATH_ID
  bool hasAnyPathId() const { return pathIdMinMax_.isSet(); }

  const IMinMax &pathIdMinMax() const { return pathIdMinMax_; }
#endif

  //---

  void addDrawText(PaintDevice *device, const QString &str, const Point &point,
                   const TextOptions &textOptions, const Point &targetPoint,
                   bool centered=false) const;

 protected:
  void clearNodesAndEdges();

  Node *findNode(const QString &name) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  Node *createNode(const QString &name) const;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  void createObjsGraph(PlotObjs &objs) const;

#ifdef CQCHARTS_GRAPH_PATH_ID
  void sortPathIdDepthNodes(bool force=false);
#endif

#ifdef CQCHARTS_GRAPH_PATH_ID
  void adjustPathIdSrcDestRects() const;
#endif

  void createGraph() const;

  void clearGraph();

  void placeDepthNodes() const;
  void placeDepthSubNodes(int pos, const Nodes &nodes) const;

  void calcValueMarginScale();

  double calcNodeMargin() const;

  NodeObj *createObjFromNode(Node *node) const;

  int calcPos(Node *node) const;

  EdgeObj *addEdgeObj(Edge *edge) const;

  //---

  void updateGraphMaxDepth(const Nodes &nodes) const;

  bool adjustNodes(bool placed=false, bool force=false) const;
  bool adjustGraphNodes(const Nodes &nodes, bool placed=false, bool force=false) const;

  void initPosNodesMap(const Nodes &nodes) const;

  bool adjustNodeCenters(bool placed=false, bool force=false) const;

  bool adjustNodeCentersLtoR(bool placed=false, bool force=false) const;
  bool adjustNodeCentersRtoL(bool placed=false, bool force=false) const;

  bool adjustPosNodes(int pos, bool placed, bool useSrc=true, bool useDest=true) const;

//bool adjustEdgeOverlaps(bool force=false) const;

  bool removeOverlaps(bool spread=true, bool constrain=true, bool force=false) const;
  bool removePosOverlaps(int pos, const Nodes &nodes, bool spread=true, bool constrain=true) const;

  bool spreadNodes() const;
  bool spreadPosNodes(int pos, const Nodes &nodes) const;

  bool constrainNodes(bool center=false) const;
  bool constrainPosNodes(int pos, const Nodes &nodes, bool center=false) const;

  bool reorderNodeEdges(const Nodes &nodes, bool force=false) const;

  void createPosNodeMap(int pos, const Nodes &nodes, PosNodeMap &posNodeMap, bool increasing) const;
  void createPosEdgeMap(const Edges &edges, PosEdgeMap &posEdgeMap, bool isSrc) const;

  bool adjustNode(Node *node, bool placed, bool useSrc=true, bool useDest=true) const;

  //--

  virtual NodeObj *createNodeObj(const BBox &rect, Node *node,
                                 const ColorInd &ig, const ColorInd &iv) const;
  virtual EdgeObj *createEdgeObj(const BBox &rect, Edge *edge) const;

  //---

  void printStats();

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 protected:
  // placement
  Align  align_              { Align::JUSTIFY };     //!< align
  bool   alignFirstLast_     { false };              //!< align first/last
  Spread spread_             { Spread::FIRST_LAST }; //!< spread
  int    alignRand_          { 10 };                 //!< number of random values for align
  bool   alignEnds_          { true };               //!< align start left and end right
#ifdef CQCHARTS_GRAPH_PATH_ID
  bool   sortPathIdNodes_    { true };               //!< sort nodes at depth by path id
  bool   sortPathIdEdges_    { true };               //!< sort node edges by path id
#endif
  bool   adjustNodes_        { true };               //!< adjust nodes
  bool   adjustCenters_      { true };               //!< adjust centers
  bool   removeOverlaps_     { true };               //!< remove overlaps
  bool   reorderEdges_       { true };               //!< reorder edges
//bool   adjustEdgeOverlaps_ { false };              //!< adjust edge overlaps
//bool   adjustSelected_     { false };              //!< adjust only selected
  int    adjustIterations_   { 25 };                 //!< number of adjust iterations
  bool   adjustText_         { false };              //!< adjust text position
  bool   constrainMove_      { true };               //!< constrain move

  // options
  bool            edgeLine_    { false };          //!< draw line for edge
  Qt::Orientation orientation_ { Qt::Horizontal }; //!< orientation
  bool            valueLabel_  { false };          //!< value label

  // bbox, margin, node width
  BBox   targetBBox_    { -1, -1, 1, 1 };      //!< target range bbox
  Length nodeMargin_    { Length::plot(0.2) }; //!< node margin (perp)
  double minNodeMargin_ { 0.1 };               //!< minimum node margin (in pixels)
  Length nodeWidth_     { Length::pixel(16) }; //!< node x width in pixels

  // mouse inside/selected text visible
  bool insideTextVisible_   { false }; //!< is inside text visble (when text invisible)
  bool selectedTextVisible_ { false }; //!< is selected text visble (when text invisible)

  // coloring
  bool           srcColoring_       { false };                   //!< color by source nodes
  BlendType      blendEdgeColor_    { BlendType::FILL_AVERAGE }; //!< blend edge color type
  ConnectionType mouseColoring_     { ConnectionType::NONE };    //!< mouse over color connections
  bool           mouseNodeColoring_ { false };                   //!< mouse over color nodes

  // text
  bool textInternal_ { true }; //!< text internal to plot

  // data
  NameNodeMap nameNodeMap_;               //!< name node map
  IndNodeMap  indNodeMap_;                //!< ind node map
  Graph*      graph_         { nullptr }; //!< graph
  Edges       edges_;                     //!< edges
  BBox        bbox_;                      //!< bbox
  int         maxNodeDepth_  { 0 };       //!< max node depth (all graphs)
  double      boxMargin_     { 0.01 };    //!< bounding box margin
  double      edgeMargin_    { 0.01 };    //!< edge bounding box margin
  bool        useMaxTotals_  { true };    //!< use max total for node src/dest scaling

#ifdef CQCHARTS_GRAPH_PATH_ID
  mutable IMinMax pathIdMinMax_; //!< min/max path id
#endif

  mutable CQChartsTextPlacer *placer_;
};

//---

/*!
 * \brief Sankey Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsSankeyPlotCustomControls : public CQChartsConnectionPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsSankeyPlotCustomControls(CQCharts *charts);

  void init() override;

  void addWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 protected:
  void connectSlots(bool b) override;

 public slots:
  void updateWidgets() override;

 private:
  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 private:
  CQChartsSankeyPlot* plot_ { nullptr };
};

#endif
