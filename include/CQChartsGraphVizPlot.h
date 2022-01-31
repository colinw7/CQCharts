#ifndef CQChartsGraphVizPlot_H
#define CQChartsGraphVizPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsValueInd.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Dot plot type
 * \ingroup Charts
 */
class CQChartsGraphVizPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsGraphVizPlotType();

  QString name() const override { return "graphviz"; }
  QString desc() const override { return "GraphViz"; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

  bool canEqualScale() const override { return true; }

  QString description() const override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsGraphVizPlot;
class CQChartsGraphVizNodeObj;
class CQChartsGraphVizEdgeObj;
class CQChartsGraphVizPlotEdge;

/*!
 * \brief Graphviz Plot Node
 * \ingroup Charts
 */
class CQChartsGraphVizPlotNode {
 public:
  using Obj         = CQChartsGraphVizNodeObj;
  using Edge        = CQChartsGraphVizPlotEdge;
  using Edges       = std::vector<Edge *>;
  using Node        = CQChartsGraphVizPlotNode;
  using OptReal     = CQChartsOptReal;
  using ModelIndex  = CQChartsModelIndex;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using FillPattern = CQChartsFillPattern;
  using LineDash    = CQChartsLineDash;
  using Length      = CQChartsLength;
  using ModelInds   = std::vector<ModelIndex>;
  using BBox        = CQChartsGeom::BBox;
  using Point       = CQChartsGeom::Point;
  using ShapeType   = CQChartsBoxObj::ShapeType;
  using EdgeRect    = std::map<Edge *, BBox>;

  CQChartsGraphVizPlotNode(const QString &str);
 ~CQChartsGraphVizPlotNode();

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

  //! get/set rect
  const BBox &rect() const;
  void setRect(const BBox &rect);

  //! get/set edge parent
  Node *parent() const { return parent_; }
  void setParent(Node *parent) { parent_ = parent; }

  //! set string (name ?)
  QString str() const { return str_; }

  //! get/set unique id
  int id() const { return id_; }
  void setId(int id) { id_ = id; }

  int dotId() const { return dotId_; }
  void setDotId(int i) { dotId_ = i; }

  //! get/set name
  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &v) { label_ = v; }

  //! get/set value
  bool hasValue() const { return value_.isSet(); }
  const OptReal value() const { return value_; }
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

  //! add source edge (dest -> src)
  void addSrcEdge(Edge *edge, bool primary=true);

  //! add destination edge (src -> dest)
  void addDestEdge(Edge *edge, bool primary=true);

  //! has destination edge
  bool hasDestNode(Node *node) const;

  //---

  Edge *findSrcEdge (Node *node) const;
  Edge *findDestEdge(Node *node) const;

  //---

  const ModelInds &modelInds() const { return modelInds_; }

  void addModelInd(const ModelIndex &i) { modelInds_.push_back(i); }

  //---

  //--- custom appearance

  //! get/set shape
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s) { shapeType_ = s; }

  //! get/set number of sides
  int numSides() const { return numSides_; }
  void setNumSides(int n) { numSides_ = n; }

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

  void moveBy(const Point &delta);
  void scale(double fx, double fy);

  //---

  bool hasEdgePoint(Edge *edge) const;
  void setEdgePoint(Edge *edge, const Point &p);
  Point edgePoint(Edge *edge) const;

 private:
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

  using EdgePoints = std::map<int, Point>;

  Obj* obj_     { nullptr }; //!< node plot object
  bool visible_ { true };    //!< is visible

  Node*   parent_        { nullptr }; //!< parent node
  QString str_;                       //!< string
  int     id_            { -1 };      //!< id
  int     dotId_         { -1 };      //!< dot id
  QString name_;                      //!< name
  QString label_;                     //!< label
  OptReal value_;                     //!< value
  int     group_         { -1 };      //!< group
  int     ngroup_        { 0 };       //!< number of groups
  int     depth_         { -1 };      //!< depth
  int     graphId_       { -1 };      //!< graph id
  int     parentGraphId_ { -1 };      //!< parent graph id

  ModelIndex ind_; //!< model index

  BBox rect_; //!< placed rectangle

  // appearance
  ShapeType  shapeType_ { ShapeType::BOX }; //!< shape type
  int        numSides_  { 4 };              //!< number of polygon sides
  FillData   fillData_;                     //!< fill data
  StrokeData strokeData_;                   //!< stroke data

  // connections
  Edges     srcEdges_;         //!< source edges
  Edges     destEdges_;        //!< destination edges
  Edges     nonPrimaryEdges_;  //!< non-primary edges
  int       srcDepth_  { -1 }; //!< source depth (calculated)
  int       destDepth_ { -1 }; //!< destination depth (calculated)
  ModelInds modelInds_;        //!< model inds

  EdgePoints edgePoints_;
};

/*!
 * \brief Graphviz Plot Edge
 * \ingroup Charts
 */
class CQChartsGraphVizPlotEdge {
 public:
  enum class ShapeType {
    NONE,
    ARROW
  };

  using Obj        = CQChartsGraphVizEdgeObj;
  using Node       = CQChartsGraphVizPlotNode;
  using Column     = CQChartsColumn;
  using ModelIndex = CQChartsModelIndex;
  using ModelInds  = std::vector<ModelIndex>;
  using Color      = CQChartsColor;
  using OptReal    = CQChartsOptReal;
//using Line       = CQChartsGeom::Line;

 public:
  CQChartsGraphVizPlotEdge(const OptReal &value, Node *srcNode, Node *destNode);
 ~CQChartsGraphVizPlotEdge();

  //---

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

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

  //! get draw path
  const QPainterPath &path() const { return path_; }

  //! get/set edge path
  const QPainterPath &edgePath() const { return edgePath_; }
  void setEdgePath(const QPainterPath &path) { edgePath_ = path; }

#if 0
  //! get/set edge line
  const Line &line() const { return line_; }
  void setLine(const Line &line) { line_ = line; }
#endif

  //! get/set is directed
  bool isDirected() const { return directed_; }
  void setDirected(bool b) { directed_ = b; }

 private:
  using NamedColumn = std::map<QString, Column>;

  Obj*         obj_       { nullptr };         //!< edge plot object
  int          id_        { -1 };              //!< unique id
  OptReal      value_;                         //!< value
  NamedColumn  namedColumn_;                   //!< named columns
  QString      label_;                         //!< label
  ShapeType    shapeType_ { ShapeType::NONE }; //!< shape type
  Color        color_;                         //!< color
  ModelInds    modelInds_;                     //!< model inds
  Node*        srcNode_   { nullptr };         //!< source node
  Node*        destNode_  { nullptr };         //!< destination node
  QPainterPath path_;                          //!< draw path
  QPainterPath edgePath_;                      //!< edge path
//Line         line_;                          //!< edge line
  bool         directed_  { false };           //!< is directed
};

//---

class CQChartsGraphVizPlot;

/*!
 * \brief Graphviz Plot Node object
 * \ingroup Charts
 */
class CQChartsGraphVizNodeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString       hierName  READ hierName  WRITE setHierName )
  Q_PROPERTY(QString       name      READ name      WRITE setName     )
  Q_PROPERTY(double        value     READ value     WRITE setValue    )
  Q_PROPERTY(int           depth     READ depth     WRITE setDepth    )
  Q_PROPERTY(ShapeType     shapeType READ shapeType WRITE setShapeType)
  Q_PROPERTY(int           numSides  READ numSides  WRITE setNumSides )
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
    DOUBLE_CIRCLE = static_cast<int>(CQChartsBoxObj::ShapeType::DOUBLE_CIRCLE),
    RECORD        = static_cast<int>(CQChartsBoxObj::ShapeType::RECORD),
    PLAIN_TEXT    = static_cast<int>(CQChartsBoxObj::ShapeType::PLAIN_TEXT)
  };

  using Plot  = CQChartsGraphVizPlot;
  using Node  = CQChartsGraphVizPlotNode;
  using Edge  = CQChartsGraphVizPlotEdge;
  using Color = CQChartsColor;
  using Angle = CQChartsAngle;

 public:
  CQChartsGraphVizNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &ind);

  virtual ~CQChartsGraphVizNodeObj();

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

  //! get/set shape type
  ShapeType shapeType() const;
  void setShapeType(const ShapeType &s);

  //! get/set num side
  int numSides() const;
  void setNumSides(int n);

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

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  QColor calcFillColor() const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot* plot_        { nullptr }; //!< parent plot
  Node*       node_        { nullptr }; //!< node
  QString     hierName_;                //!< node hier name
  bool        editChanged_ { false };   //!< edit is changed
};

//---

/*!
 * \brief Graphviz Plot Edge object
 * \ingroup Charts
 */
class CQChartsGraphVizEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE,
    ARROW
  };

  using Plot   = CQChartsGraphVizPlot;
  using Edge   = CQChartsGraphVizPlotEdge;
  using Node   = CQChartsGraphVizPlotNode;
  using Length = CQChartsLength;
  using Angle  = CQChartsAngle;

 public:
  CQChartsGraphVizEdgeObj(const Plot *plot, const BBox &rect, Edge *edge);

  virtual ~CQChartsGraphVizEdgeObj();

  //---

  //! get plot
  const Plot *plot() const { return plot_; }

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

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot*          plot_      { nullptr }; //!< parent plot
  Edge*                edge_      { nullptr }; //!< edge
  BBox                 srcRect_;               //!< src rect
  BBox                 destRect_;              //!< dest rect
  mutable QPainterPath path_;                  //!< painter path
};

//---

/*!
 * \brief Graph Plot
 * \ingroup Charts
 */
class CQChartsGraphVizPlot : public CQChartsConnectionPlot,
 public CQChartsObjTextData<CQChartsGraphVizPlot>,
 public CQChartsObjNodeShapeData<CQChartsGraphVizPlot>,
 public CQChartsObjEdgeShapeData<CQChartsGraphVizPlot> {
  Q_OBJECT

  // plot type
  Q_PROPERTY(PlotType plotType READ plotType WRITE setPlotType)

  // node data
  Q_PROPERTY(NodeShape nodeShape READ nodeShape WRITE setNodeShape)

  // edge data
  Q_PROPERTY(EdgeShape      edgeShape    READ edgeShape      WRITE setEdgeShape   )
  Q_PROPERTY(bool           edgeArrow    READ isEdgeArrow    WRITE setEdgeArrow   )
  Q_PROPERTY(bool           edgeScaled   READ isEdgeScaled   WRITE setEdgeScaled  )
  Q_PROPERTY(CQChartsLength edgeWidth    READ edgeWidth      WRITE setEdgeWidth   )
  Q_PROPERTY(bool           edgeCentered READ isEdgeCentered WRITE setEdgeCentered)
  Q_PROPERTY(bool           edgePath     READ isEdgePath     WRITE setEdgePath    )
  Q_PROPERTY(double         arrowWidth   READ arrowWidth     WRITE setArrowWidth  )
  Q_PROPERTY(bool           edgeWeighted READ isEdgeWeighted WRITE setEdgeWeighted)

  // options
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

  // coloring
  Q_PROPERTY(bool blendEdgeColor READ isBlendEdgeColor WRITE setBlendEdgeColor)

  // node/edge shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // text style
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(NodeShape)
  Q_ENUMS(EdgeShape)
  Q_ENUMS(PlotType)

 public:
  enum class NodeShape {
    NONE,
    DIAMOND,
    BOX,
    POLYGON,
    CIRCLE,
    DOUBLE_CIRCLE
  };

  enum class EdgeShape {
    NONE,
    LINE,
    ARC
  };

  enum class PlotType {
    DOT,
    NEATO,
    TWOPI,
    CIRCO,
    FDP,
    OSAGE,
    PATCHWORK,
    SFDP
  };

  using Node     = CQChartsGraphVizPlotNode;
  using Nodes    = std::vector<Node *>;
  using NodeSet  = std::set<Node *>;
  using Edge     = CQChartsGraphVizPlotEdge;
  using Edges    = std::vector<Edge *>;
  using NodeObj  = CQChartsGraphVizNodeObj;
  using EdgeObj  = CQChartsGraphVizEdgeObj;
  using Length   = CQChartsLength;
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsGraphVizPlot(View *view, const ModelP &model);
 ~CQChartsGraphVizPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set node shape
  NodeShape nodeShape() const { return nodeShape_; }
  void setNodeShape(const NodeShape &s);

  //---

  //! get/set edge shape
  const EdgeShape &edgeShape() const { return edgeShape_; }
  void setEdgeShape(const EdgeShape &s);

  //! get/set has arrow
  bool isEdgeArrow() const { return edgeArrow_; }
  void setEdgeArrow(bool b);

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeScaled_; }
  void setEdgeScaled(bool b);

  //! get/set edge width
  const Length &edgeWidth() const { return edgeWidth_; }
  void setEdgeWidth(const Length &l);

  //! get/set is edge centered
  bool isEdgeCentered() const { return edgeCentered_; }
  void setEdgeCentered(bool b);

  //! get/set use edge path
  bool isEdgePath() const { return edgePath_; }
  void setEdgePath(bool b);

  //! get/set edge directed arrow width
  double arrowWidth() const { return arrowWidth_; }
  void setArrowWidth(double r);

  //! get/set edge weighted
  bool isEdgeWeighted() const { return edgeWeighted_; }
  void setEdgeWeighted(bool b);

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

  const PlotType &plotType() const { return plotType_; }
  void setPlotType(const PlotType &t);

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

  int numNodes() const { return nodes_.size(); }

  const Edges &edges() const { return edges_; }

  //---

  bool createObjs(PlotObjs &objs) const override;

  void writeGraph(bool weighted=true) const;

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

  static void stringToShapeType(const QString &str, Node::ShapeType &shapeType);
  static QString shapeTypeToString(const Node::ShapeType &shapeType);

  static void stringToShapeType(const QString &str, Edge::ShapeType &shapeType);
  static QString shapeTypeToString(const Edge::ShapeType &shapeType);

  //---

  void filterObjs();

  //---

  int maxNodeDepth() const { return maxNodeDepth_; }

  //---

  bool keyPress(int key, int modifier) override;

  //---

  void addObjects(PlotObjs &objs) const;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

  const OptReal &maxEdgeValue() const { return maxEdgeValue_; }

 protected:
  void clearNodesAndEdges();

  Node *findNode(const QString &name) const;

  Node *findIdNode(int dotId) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  Node *createNode(const QString &name) const;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  NodeObj *createObjFromNode(Node *node) const;

  EdgeObj *addEdgeObj(Edge *edge) const;

  //---

  const BBox &bbox() const { return bbox_; }

  //---

  virtual NodeObj *createNodeObj(const BBox &rect, Node *node, const ColorInd &ind) const;
  virtual EdgeObj *createEdgeObj(const BBox &rect, Edge *edge) const;

  //---

  void printStats();

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 protected:
  friend class CQChartsGraphVizPlotNode;

 protected:
  using NameNodeMap = std::map<QString, Node *>;
  using IndNodeMap  = std::map<int, Node *>;

  // node data
  NodeShape nodeShape_ { NodeShape::NONE }; //!< node shape

  // edge data
  EdgeShape edgeShape_    { EdgeShape::ARC };   //!< edge shape
  bool      edgeArrow_    { true };             //!< edge arrow
  bool      edgeScaled_   { false };            //!< is edge scaled
  Length    edgeWidth_    { Length::pixel(8) }; //!< edge width
  bool      edgeCentered_ { false };            //!< is edge centered
  bool      edgePath_     { true };             //!< use edge path

  // plot data
  Qt::Orientation orientation_ { Qt::Vertical };  //!< orientation
  PlotType        plotType_    { PlotType::FDP }; //!< plot type

  // bbox, margin, node width
  BBox   targetBBox_ { -1, -1, 1, 1 }; //!< target range bbox

  // coloring
  bool blendEdgeColor_ { true }; //!< blend edge color

  // data
  Nodes            nodes_;                  //!< all nodes
  NameNodeMap      nameNodeMap_;            //!< name node map
  IndNodeMap       indNodeMap_;             //!< ind node map
  Edges            edges_;                  //!< all edges
  BBox             bbox_;                   //!< bbox
  CQChartsValueInd groupValueInd_;          //!< group value ind
  int              maxNodeDepth_  { 0 };    //!< max node depth (all graphs)
  double           edgeMargin_    { 0.01 }; //!< edge bounding box margin
  int              numGroups_     { 1 };    //!< node number of groups
  double           arrowWidth_    { 1.5 };  //!< edge bounding box margin
  bool             edgeWeighted_  { true }; //!< edge placement weighted by value

  OptReal maxEdgeValue_;
};

//---

/*!
 * \brief Graphviz Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsGraphVizPlotCustomControls : public CQChartsConnectionPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsGraphVizPlotCustomControls(CQCharts *charts);

  void init() override;

  void addWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 protected:
  void connectSlots(bool b) override;

 public slots:
  void updateWidgets() override;

 protected:
  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 private:
  CQChartsGraphVizPlot* plot_ { nullptr };
};

#endif
