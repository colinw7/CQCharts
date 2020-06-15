#ifndef CQChartsSankeyPlot_H
#define CQChartsSankeyPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
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
class CQChartsSankeyNodeObj;
class CQChartsSankeyEdgeObj;

/*!
 * \brief Sankey plot node
 * \ingroup Charts
 */
class CQChartsSankeyPlotNode {
 public:
  enum Shape {
    SHAPE_NONE,
    SHAPE_DIAMOND,
    SHAPE_BOX,
    SHAPE_POLYGON,
    SHAPE_CIRCLE,
    SHAPE_DOUBLE_CIRCLE
  };

  using Edges   = std::vector<CQChartsSankeyPlotEdge *>;
  using NodeSet = std::set<const CQChartsSankeyPlotNode *>;

 public:
  using Plot       = CQChartsSankeyPlot;
  using Edge       = CQChartsSankeyPlotEdge;
  using Node       = CQChartsSankeyPlotNode;
  using Obj        = CQChartsSankeyNodeObj;
  using OptReal    = CQChartsOptReal;
  using ModelIndex = CQChartsModelIndex;

 public:
  CQChartsSankeyPlotNode(const Plot *plot, const QString &str);
 ~CQChartsSankeyPlotNode();

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
  int srcDepth () const;
  //! get destination depth (from connections)
  int destDepth() const;

  //! get/set shape
  const Shape &shape() const { return shape_; }
  void setShape(const Shape &s) { shape_ = s; }

  //! get/set num side
  int numSides() const { return numSides_; }
  void setNumSides(int n) { numSides_ = n; }

  //! get/set color
  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  //! calc x pos
  int calcXPos() const;

  // get x pos
  int xpos() const { return xpos_; }

  //! get edge (max) sum
  double edgeSum() const;

  //! get source edge (max) sum
  double srcEdgeSum () const;

  //! get destination edge (max) sum
  double destEdgeSum() const;

 private:
  //! calc src/destination depth
  int calcSrcDepth (NodeSet &visited) const;
  int calcDestDepth(NodeSet &visited) const;

 private:
  const Plot* plot_      { nullptr };    //!< associated plot
  Node*       parent_    { nullptr };    //!< parent node
  QString     str_;                      //!< string
  int         id_        { -1 };         //!< id
  bool        visible_   { true };       //!< is visible
  ModelIndex  ind_;                      //!< model index
  QString     name_;                     //!< name
  QString     label_;                    //!< label
  OptReal     value_;                    //!< value
  int         group_     { -1 };         //!< group
  int         depth_     { -1 };         //!< depth
  Shape       shape_     { SHAPE_NONE }; //!< shape
  int         numSides_  { 4 };          //!< number of polygon sides
  QColor      color_;                    //!< fill color
  Edges       srcEdges_;                 //!< source edges
  Edges       destEdges_;                //!< destination edges
  Edges       nonPrimaryEdges_;          //!< non-priary edges
  int         srcDepth_  { -1 };         //!< source depth (calculated)
  int         destDepth_ { -1 };         //!< destination depth (calculated)
  int         xpos_      { -1 };         //!< x position
  Obj*        obj_       { nullptr };    //!< plot object
};

//---

/*!
 * \brief Sankey plot edge
 * \ingroup Charts
 */
class CQChartsSankeyPlotEdge {
 public:
  enum Shape {
    SHAPE_NONE,
    SHAPE_ARC,
    SHAPE_ARROW
  };

  using Plot    = CQChartsSankeyPlot;
  using Node    = CQChartsSankeyPlotNode;
  using Obj     = CQChartsSankeyEdgeObj;
  using OptReal = CQChartsOptReal;

 public:
  CQChartsSankeyPlotEdge(const Plot *plot, const OptReal &value, Node *srcNode, Node *destNode);

 ~CQChartsSankeyPlotEdge();

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
  const Shape &shape() const { return shape_; }
  void setShape(const Shape &s) { shape_ = s; }

  //! get/set object
  Obj *obj() const { return obj_; }
  void setObj(Obj *obj);

 private:
  const Plot* plot_     { nullptr };    //!< plot
  int         id_       { -1 };         //!< unique id
  OptReal     value_;                   //!< value
  QString     label_;                   //!< label
  Node*       srcNode_  { nullptr };    //!< source node
  Node*       destNode_ { nullptr };    //!< destination node
  Shape       shape_    { SHAPE_NONE }; //!< shape
  Obj*        obj_      { nullptr };    //!< associated edge object
};

//---

/*!
 * \brief Sankey Plot Node object
 * \ingroup Charts
 */
class CQChartsSankeyNodeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(Shape shape READ shape WRITE setShape)

  Q_ENUMS(Shape)

 public:
  enum Shape {
    SHAPE_NONE,
    SHAPE_DIAMOND,
    SHAPE_BOX,
    SHAPE_POLYGON,
    SHAPE_CIRCLE,
    SHAPE_DOUBLE_CIRCLE
  };

  using Plot = CQChartsSankeyPlot;
  using Node = CQChartsSankeyPlotNode;
  using Edge = CQChartsSankeyPlotEdge;

 public:
  CQChartsSankeyNodeObj(const Plot *plot, const BBox &rect, Node *node, const ColorInd &ind);

  Node *node() const { return node_; }

  const QString &hierName() const { return hierName_; }
  void setHierName(const QString &s) { hierName_ = s; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  int depth() const { return depth_; }
  void setDepth(int i) { depth_ = i; }

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

  //! get/set shape
  const Shape &shape() const { return shape_; }
  void setShape(const Shape &s) { shape_ = s; }

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

  void moveBy(const Point &delta);

  //---

  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  using EdgeRect = std::map<Edge *,BBox>;

  const Plot* plot_     { nullptr };    //!< parent plot
  Node*       node_     { nullptr };    //!< node
  EdgeRect    srcEdgeRect_;             //!< edge to src
  EdgeRect    destEdgeRect_;            //!< edge to dest
  QString     hierName_;                //!< node hier name
  QString     name_;                    //!< node name
  double      value_    { 0.0 };        //!< node value
  int         depth_    { -1 };         //!< node depth
  Shape       shape_    { SHAPE_NONE }; //!< shape
  int         numSides_ { 4 };          //!< num sides
  QColor      color_;                   //!< custom color
};

//---

/*!
 * \brief Sankey Plot Edge object
 * \ingroup Charts
 */
class CQChartsSankeyEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(Shape shape READ shape WRITE setShape)

  Q_ENUMS(Shape)

 public:
  enum Shape {
    SHAPE_NONE,
    SHAPE_ARC,
    SHAPE_ARROW
  };

  using Plot = CQChartsSankeyPlot;
  using Edge = CQChartsSankeyPlotEdge;
  using Node = CQChartsSankeyPlotNode;

 public:
  CQChartsSankeyEdgeObj(const Plot *plot, const BBox &rect, Edge *edge);

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

  //! get/set shape
  const Shape &shape() const { return shape_; }
  void setShape(const Shape &s) { shape_ = s; }

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

 private:
  const Plot*  plot_     { nullptr };   //!< parent plot
  Edge*        edge_     { nullptr };   //!< edge
  BBox         srcRect_;                //!< src rect
  BBox         destRect_;               //!< dest rect
  Shape        shape_    { SHAPE_ARC }; //!< shape
  QPainterPath path_;                   //!< painter path
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Edge,edge)

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
  Q_PROPERTY(double    nodeMargin READ nodeMargin   WRITE setNodeMargin)
  Q_PROPERTY(double    nodeWidth  READ nodeWidth    WRITE setNodeWidth )
  Q_PROPERTY(bool      nodeScaled READ isNodeScaled WRITE setNodeScaled)
  Q_PROPERTY(NodeShape nodeShape  READ nodeShape    WRITE setNodeShape )
  Q_PROPERTY(EdgeShape edgeShape  READ edgeShape    WRITE setEdgeShape )
  Q_PROPERTY(bool      edgeScaled READ isEdgeScaled WRITE setEdgeScaled)

  // align
  Q_PROPERTY(Align align READ align WRITE setAlign)

  // node/edge shape style
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge,edge)

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
  using NameNodeMap = std::map<QString,Node *>;
  using IndNodeMap  = std::map<int,Node *>;
  using NodeSet     = std::set<Node *>;
  using Edge        = CQChartsSankeyPlotEdge;
  using Edges       = std::vector<Edge *>;
  using NodeObj     = CQChartsSankeyNodeObj;
  using EdgeObj     = CQChartsSankeyEdgeObj;

 public:
  CQChartsSankeyPlot(View *view, const ModelP &model);

 ~CQChartsSankeyPlot();

  //---

  //! get/set node margin
  double nodeMargin() const { return nodeMargin_; }
  void setNodeMargin(double r);

  //---

  //! get/set x margin
  double nodeWidth() const { return nodeWidth_; }
  void setNodeWidth(double r);

  //! get/set is node scaled
  bool isNodeScaled() const { return nodeScaled_; }
  void setNodeScaled(bool b);

  //! get/set node shape
  const NodeShape &nodeShape() const { return nodeShape_; }
  void setNodeShape(const NodeShape &s);

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

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

  Range getCalcDataRange() const override;

  //---

  bool createObjs(PlotObjs &objs) const override;

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
                      const CQChartsNameValues &) const override;

  //---

  bool initLinkObjs      () const;
  bool initConnectionObjs() const;
  bool initTableObjs     () const;

  void filterObjs();

  //---

  void updateMaxDepth() const;

  //---

  int numNodes() const { return nameNodeMap_.size(); }

  int maxHeight() const { return maxHeight_; }

  int maxNodeDepth() const { return maxNodeDepth_; }

  double valueScale() const { return valueScale_; }

  //---

  void keyPress(int key, int modifier) override;

  //---

 protected:
  void clearNodesAndEdges();

  Node *findNode(const QString &name) const;

  Node *createNode(const QString &name) const;
  Edge *createEdge(const OptReal &value, Node *srcNode, Node *destNode) const;

  void createGraph(PlotObjs &objs) const;

  void createDepthNodes(const IndNodeMap &nodes) const;

  EdgeObj *addEdgeObj(Edge *edge) const;

  void adjustNodes() const;

  void initPosNodesMap();

  void adjustNodeCenters();

  void removeOverlaps() const;

  void reorderNodeEdges() const;

  void adjustNode(Node *node) const;

  //---

  virtual NodeObj *createNodeObj(const BBox &rect, Node *node, const ColorInd &ind) const;
  virtual EdgeObj *createEdgeObj(const BBox &rect, Edge *edge) const;

 private:
  using PosNodesMap = std::map<int,IndNodeMap>;

  // options
  Align     align_      { Align::JUSTIFY };  //!< align
  double    nodeMargin_ { 0.2 };             //!< node margin
  double    nodeWidth_  { 16 };              //!< node x margin in pixels
  bool      nodeScaled_ { true };            //!< is node scaled
  NodeShape nodeShape_  { NODE_SHAPE_NONE }; //!< node shape
  EdgeShape edgeShape_  { EDGE_SHAPE_ARC };  //!< edge shape
  bool      edgeScaled_ { true };            //!< is edge scaled

  // data
  NameNodeMap nameNodeMap_;             //!< name node map
  IndNodeMap  indNodeMap_;              //!< ind node map
  PosNodesMap posNodesMap_;             //!< pos node map
  Edges       edges_;                   //!< edges
  BBox        bbox_;                    //!< bbox
  int         maxHeight_     { 0 };     //!< max height
  int         maxNodeDepth_  { 0 };     //!< max node depth
  double      minNodeMargin_ { 4 };     //!< min node margin in pixels
  double      boxMargin_     { 0.01 };  //!< bounding box margin
  double      edgeMargin_    { 0.01 };  //!< edge bounding box margin
  double      valueScale_    { 1.0 };   //!< value scale
  double      valueMargin_   { 0.0 };   //!< value margin
  bool        pressed_       { false }; //!< mouse pressed
  bool        nodeYSet_      { false }; //!< node y set
  double      nodeYMin_      { 0.0 };   //!< node y min
  double      nodeYMax_      { 0.0 };   //!< node y max
};

#endif
