#ifndef CQChartsDendrogramPlot_H
#define CQChartsDendrogramPlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsHierPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDendrogram.h>

class CQChartsDendrogramPlot;
class CQChartsNamePair;

namespace CBuchHeim {
class Tree;
class DrawTree;
}

//---

/*!
 * \brief Dendrogram plot type
 * \ingroup Charts
 */
class CQChartsDendrogramPlotType : public CQChartsHierPlotType {
 public:
  CQChartsDendrogramPlotType();

  QString name() const override { return "dendrogram"; }
  QString desc() const override { return "Dendrogram"; }

  Category category() const override { return Category::HIER; }

  void addParameters() override;

  void addExtraHierParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  bool canEqualScale() const override { return true; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Dendrogram Plot Node object
 * \ingroup Charts
 */
class CQChartsDendrogramNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using DendrogramPlot = CQChartsDendrogramPlot;
  using NodeObj        = CQChartsDendrogramNodeObj;
  using Node           = CQChartsDendrogram::Node;
  using Length         = CQChartsLength;
  using Angle          = CQChartsAngle;
  using Sides          = CQChartsSides;
  using OptReal        = CQChartsOptReal;

  //---

  struct Child {
    Child() = default;

    Child(NodeObj *node, const OptReal &value) :
     node(node), value(value) {
    }

    NodeObj *node { nullptr };
    OptReal  value;
  };

  using Children = std::vector<Child>;

 public:
  CQChartsDendrogramNodeObj(const DendrogramPlot *plot, Node *node, const BBox &rect,
                            const ColorInd &ig, const ColorInd &iv);

  QString typeName() const override { return "node"; }

  //---

  const DendrogramPlot *plot() const { return dendrogramPlot_; }

  const NodeObj *parent() const { return parent_; }
  void setParent(NodeObj *p) { parent_ = p; }

  const Children &children() const { return children_; }

  void addChild(NodeObj *node, const OptReal &value=OptReal());

  const Node *node() const { return node_; }
  void resetNode() { node_ = nullptr; }

  //---

  const BBox &saveRect() const { return saveRect_; }
  void setSaveRect(const BBox &r) { saveRect_ = r; }

  //---

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  const QString &name() const { return name_; }

  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r) { value_ = r; }

  double childTotal() const { return childTotal_; }

  //---

  bool isRoot() const { return isRoot_; }
  void setRoot(bool b) { isRoot_ = b; }

  bool isHier() const { return isHier_; }
  void setHier(bool b) { isHier_ = b; }

  bool isOpen() const { return open_; }
  void setOpen(bool b) { open_ = b; }

  //---

  ModelIndex modelIndex() const { return modelIndex_; }
  void setModelIndex(const ModelIndex &modelIndex) { modelIndex_ = modelIndex; }

  //---

  const OptReal &color() const { return color_; }
  void setColor(const OptReal &v) { color_ = v; }

  const OptReal &size() const { return size_; }
  void setSize(const OptReal &v) { size_ = v; }

  //---

  double hierColor() const { return hierColor_; }
  void setHierColor(double r) { hierColor_ = r; }

  double hierSize() const { return hierSize_; }
  void setHierSize(double r) { hierSize_ = r; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  BBox textRect() const;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void drawText(PaintDevice *device) const;

  void calcTextPos(Point &p, const QFont &font, Angle &angle, uint &position,
                   Qt::Alignment &align, bool &centered) const;

  //---

  BBox displayRect() const;

  void movePerpCenter(double pos);

  void movePerpBy(double d);

 private:
  double calcScaledShapeSize() const;

 private:
  const DendrogramPlot* dendrogramPlot_ { nullptr }; //!< plot
  NodeObj*              parent_         { nullptr }; //!< parent
  Children              children_;                   //!< children
  Node*                 node_           { nullptr }; //!< associated node
  QString               name_;                       //!< name
  int                   ind_            { 0 };       //!< unique index
  OptReal               value_;                      //!< value
  double                childTotal_     { 0.0 };     //!< child total
  bool                  isRoot_         { false };   //!< is root
  bool                  isHier_         { false };   //!< is hierarchical
  bool                  open_           { false };   //!< is open
  ModelIndex            modelIndex_;                 //!< model index
  OptReal               color_;                      //!< optional color
  OptReal               size_;                       //!< optional size
  mutable double        hierColor_      { 0.0 };     //!< hier color
  mutable double        hierSize_       { 0.0 };     //!< hier size
  mutable BBox          saveRect_;                   //!< save rect
};

//---

class CQChartsDendrogramEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using DendrogramPlot = CQChartsDendrogramPlot;
  using NodeObj        = CQChartsDendrogramNodeObj;
  using Angle          = CQChartsAngle;
  using OptReal        = CQChartsOptReal;

 public:
  CQChartsDendrogramEdgeObj(const DendrogramPlot *plot, NodeObj *fromNode,
                            NodeObj *toNode, const BBox &rect,
                            const ColorInd &ig, const ColorInd &iv);

  QString typeName() const override { return "edge"; }

  const DendrogramPlot *plot() const { return dendrogramPlot_; }

  NodeObj *fromNode() const { return fromNode_; }
  NodeObj *toNode() const { return toNode_; }

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  const OptReal &value() const { return value_; }
  void setValue(const OptReal &v) { value_ = v; }

  const OptReal &color() const { return color_; }
  void setColor(const OptReal &v) { color_ = v; }

  const OptReal &size() const { return size_; }
  void setSize(const OptReal &v) { size_ = v; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 private:
  const DendrogramPlot* dendrogramPlot_ { nullptr }; //!< plot
  NodeObj*              fromNode_       { nullptr }; //!< from node object
  NodeObj*              toNode_         { nullptr }; //!< to node object
  int                   ind_            { 0 };       //!< unique index
  OptReal               value_;                      //!< optional value
  OptReal               color_;                      //!< optional color
  OptReal               size_;                       //!< optional size
  mutable QPainterPath  path_;                       //!< edge path
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Root, root)
CQCHARTS_NAMED_SHAPE_DATA(Leaf, leaf)

CQCHARTS_NAMED_TEXT_DATA(Root, root)
CQCHARTS_NAMED_TEXT_DATA(Leaf, leaf)

/*!
 * \brief Dendrogram Plot
 * \ingroup Charts
 */
class CQChartsDendrogramPlot : public CQChartsHierPlot,
 public CQChartsObjRootShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjHierShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjLeafShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjEdgeShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjRootTextData <CQChartsDendrogramPlot>,
 public CQChartsObjHierTextData <CQChartsDendrogramPlot>,
 public CQChartsObjLeafTextData <CQChartsDendrogramPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn linkColumn READ linkColumn WRITE setLinkColumn)
  Q_PROPERTY(CQChartsColumn sizeColumn READ sizeColumn WRITE setSizeColumn)

  // root node data
  Q_PROPERTY(bool              rootVisible READ isRootVisible WRITE setRootVisible)
  Q_PROPERTY(CQChartsLength    rootSize    READ rootSize      WRITE setRootSize   )
  Q_PROPERTY(double            rootAspect  READ rootAspect    WRITE setRootAspect )
  Q_PROPERTY(CQChartsShapeType rootShape   READ rootShape     WRITE setRootShape  )

  Q_PROPERTY(TextPosition rootTextPosition READ rootTextPosition  WRITE setRootTextPosition)
  Q_PROPERTY(bool         rootRotatedText  READ isRootRotatedText WRITE setRootRotatedText )
  Q_PROPERTY(double       rootTextMargin   READ rootTextMargin    WRITE setRootTextMargin  )

  // hier node data
  Q_PROPERTY(CQChartsLength    hierSize   READ hierSize   WRITE setHierSize  )
  Q_PROPERTY(double            hierAspect READ hierAspect WRITE setHierAspect)
  Q_PROPERTY(CQChartsShapeType hierShape  READ hierShape  WRITE setHierShape )

  Q_PROPERTY(TextPosition hierTextPosition READ hierTextPosition  WRITE setHierTextPosition)
  Q_PROPERTY(bool         hierRotatedText  READ isHierRotatedText WRITE setHierRotatedText )
  Q_PROPERTY(double       hierTextMargin   READ hierTextMargin    WRITE setHierTextMargin  )

  // leaf node data
  Q_PROPERTY(CQChartsLength    leafSize   READ leafSize   WRITE setLeafSize  )
  Q_PROPERTY(double            leafAspect READ leafAspect WRITE setLeafAspect)
  Q_PROPERTY(CQChartsShapeType leafShape  READ leafShape  WRITE setLeafShape )

  Q_PROPERTY(TextPosition leafTextPosition READ leafTextPosition  WRITE setLeafTextPosition)
  Q_PROPERTY(bool         leafRotatedText  READ isLeafRotatedText WRITE setLeafRotatedText )
  Q_PROPERTY(double       leafTextMargin   READ leafTextMargin    WRITE setLeafTextMargin  )

  // edge data
//Q_PROPERTY(bool           edgeArrow    READ isEdgeArrow  WRITE setEdgeArrow   )
  Q_PROPERTY(bool           edgeScaled   READ isEdgeScaled WRITE setEdgeScaled  )
  Q_PROPERTY(CQChartsLength edgeWidth    READ edgeWidth    WRITE setEdgeWidth   )
  Q_PROPERTY(CQChartsLength minEdgeWidth READ minEdgeWidth WRITE setMinEdgeWidth)
//Q_PROPERTY(double         arrowWidth   READ arrowWidth   WRITE setArrowWidth  )

  // options
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation )

  Q_PROPERTY(PlaceType placeType READ placeType WRITE setPlaceType)

  Q_PROPERTY(int hideDepth READ hideDepth WRITE setHideDepth)

  Q_PROPERTY(double sizeScale READ sizeScale WRITE setSizeScale)

  Q_PROPERTY(bool removeNodeOverlaps  READ isRemoveNodeOverlaps  WRITE setRemoveNodeOverlaps)
  Q_PROPERTY(bool removeGroupOverlaps READ isRemoveGroupOverlaps WRITE setRemoveGroupOverlaps)
  Q_PROPERTY(bool adjustOverlaps      READ isAdjustOverlaps      WRITE setAdjustOverlaps)

  Q_PROPERTY(CQChartsLength overlapMargin READ overlapMargin WRITE setOverlapMargin)

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Root, root)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Hier, hier)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Leaf, leaf)

  // edge line
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // hier, leaf labels
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Root, root)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Hier, hier)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Leaf, leaf)

  Q_ENUMS(PlaceType)
  Q_ENUMS(TextPosition)

 public:
  using NodeObj   = CQChartsDendrogramNodeObj;
  using EdgeObj   = CQChartsDendrogramEdgeObj;
  using Node      = CQChartsDendrogram::Node;
  using Color     = CQChartsColor;
  using Length    = CQChartsLength;
  using PenBrush  = CQChartsPenBrush;
  using BrushData = CQChartsBrushData;
  using ColorInd  = CQChartsUtil::ColorInd;

  enum class PlaceType {
    DENDROGRAM,
    BUCHHEIM,
    CIRCULAR
  };

  enum class TextPosition {
    LEFT,
    RIGHT,
    CENTER
  };

  struct Edge {
    Node*   from { nullptr };
    Node*   to   { nullptr };
    OptReal value;
    OptReal colorValue;
    OptReal sizeValue;

    Edge() = default;

    Edge(Node *from, Node *to, const OptReal &value) :
     from(from), to(to), value(value) {
    }
  };

  using Edges = std::vector<Edge *>;

 public:
  CQChartsDendrogramPlot(View *view, const ModelP &model);
 ~CQChartsDendrogramPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set link column
  const Column &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const Column &c);

  //! get/set value column
  const Column &sizeColumn() const { return sizeColumn_; }
  void setSizeColumn(const Column &c);

  void setColorColumn(const Column &c) override;

  //---

  //! get/set named column
  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  // root shape

  //! get/set root visible
  bool isRootVisible() const { return rootVisible_; }
  void setRootVisible(bool b);

  //! get/set root size
  const Length &rootSize() const { return rootNodeData_.size; }
  void setRootSize(const Length &s);

  //! get/set root aspect
  double rootAspect() const { return rootNodeData_.aspect; }
  void setRootAspect(double a);

  //! get/set root shape
  const CQChartsShapeType &rootShape() const { return rootNodeData_.shape; }
  void setRootShape(const CQChartsShapeType &s);

  //! get/set root text position
  const TextPosition &rootTextPosition() const { return rootNodeData_.textPosition; }
  void setRootTextPosition(const TextPosition &p);

  // get/set is rotated text label (for circular placement)
  bool isRootRotatedText() const { return rootNodeData_.rotatedText; }
  void setRootRotatedText(bool b);

  //! get/set root text margin
  double rootTextMargin() const { return rootNodeData_.textMargin; }
  void setRootTextMargin(double r);

  double calcRootSize() const;

  //---

  // hier shape

  //! get/set hier size
  const Length &hierSize() const { return hierNodeData_.size; }
  void setHierSize(const Length &s);

  //! get/set hier aspect
  double hierAspect() const { return hierNodeData_.aspect; }
  void setHierAspect(double a);

  //! get/set hier shape
  const CQChartsShapeType &hierShape() const { return hierNodeData_.shape; }
  void setHierShape(const CQChartsShapeType &s);

  //! get/set hier text position
  const TextPosition &hierTextPosition() const { return hierNodeData_.textPosition; }
  void setHierTextPosition(const TextPosition &p);

  // get/set is rotated text label (for circular placement)
  bool isHierRotatedText() const { return hierNodeData_.rotatedText; }
  void setHierRotatedText(bool b);

  //! get/set hier text margin
  double hierTextMargin() const { return hierNodeData_.textMargin; }
  void setHierTextMargin(double r);

  double calcHierSize() const;

  //---

  // leaf shape

  //! get/set leaf size
  const Length &leafSize() const { return leafNodeData_.size; }
  void setLeafSize(const Length &s);

  //! get/set leaf aspect
  double leafAspect() const { return leafNodeData_.aspect; }
  void setLeafAspect(double a);

  //! get/set leaf shape
  const CQChartsShapeType &leafShape() const { return leafNodeData_.shape; }
  void setLeafShape(const CQChartsShapeType &s);

  //! get/set leaf text position
  const TextPosition &leafTextPosition() const { return leafNodeData_.textPosition; }
  void setLeafTextPosition(const TextPosition &p);

  // get/set is rotated text label (for circular placement)
  bool isLeafRotatedText() const { return leafNodeData_.rotatedText; }
  void setLeafRotatedText(bool b);

  //! get/set leaf text margin
  double leafTextMargin() const { return leafNodeData_.textMargin; }
  void setLeafTextMargin(double r);

  double calcLeafSize() const;

  //---

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeScaled_; }
  void setEdgeScaled(bool b);

  //! get/set edge width
  const Length &edgeWidth() const { return edgeWidth_; }
  void setEdgeWidth(const Length &l);

  //! get/set min edge width
  const Length &minEdgeWidth() const { return minEdgeWidth_; }
  void setMinEdgeWidth(const Length &l);

  //---

  double edgeValueScale(const EdgeObj *edge, double value) const;

  double mapColor(double value) const;

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o);

  //! get/set place type
  const PlaceType &placeType() const { return placeType_; }
  void setPlaceType(const PlaceType &t);

  int hideDepth() const { return hideDepth_; }
  void setHideDepth(int i);

  double sizeScale() const { return sizeScale_; }
  void setSizeScale(double r);

  //---

  bool isRemoveNodeOverlaps() const { return removeNodeOverlaps_; }
  void setRemoveNodeOverlaps(bool b);

  bool isRemoveGroupOverlaps() const { return removeGroupOverlaps_; }
  void setRemoveGroupOverlaps(bool b);

  const Length &overlapMargin() const { return overlapMargin_; }
  void setOverlapMargin(const Length &l);

  bool isAdjustOverlaps() const { return adjustOverlaps_; }
  void setAdjustOverlaps(bool b);

  double overlapScale() const { return overlapScale_; }

  //---

  BBox calcExtraFitBBox() const override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  void addHierName(const QString &nameStr, const ModelIndex &modelInd) const;

  void addNameValue(const QString &nameStr, const CQChartsNamePair &namePair,
                    const QStringList &names, const OptReal &value,
                    const ModelIndex &modelInd, const OptReal &colorValue,
                    OptReal &sizeValue, Edges &edges) const;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void execMoveNonRoot(const PlotObjs &objs);

  //---

  void saveOverlapRects();
  void saveOverlapRects(const PlotObjs &objs);
  void restoreOverlapRects();

  void execRemoveOverlaps();
  void execRemoveOverlaps(const PlotObjs &objs);

//void calcNodeSize(PlotObjs &objs);

  //---

  void addNodeObjs(Node *hier, int depth, NodeObj *parentObj, PlotObjs &objs) const;

  NodeObj *addNodeObj(Node *node, PlotObjs &objs, bool isHier) const;
  EdgeObj *addEdgeObj(NodeObj *fromNode, NodeObj *toObj) const;

  //---

  void modelViewExpansionChanged() override;

  void resetNodeExpansion(bool expanded);
  void resetNodeExpansion(Node *hierNode, bool expanded);

  void setNodeExpansion(Node *hierNode, const std::set<QModelIndex> &indSet);

  //---

  BBox getBBox(Node *node) const;

  //---

  //! handle select press
  bool handleSelectPress(const Point &p, SelMod selMod) override;

  //---

  Node *rootNode() const { return (dendrogram_ ? dendrogram_->root() : nullptr); }

  NodeObj *rootNodeObj() const { return cacheData_.rootNodeObj; }

  virtual NodeObj *createNodeObj(Node *node, const BBox &rect,
                                 const ColorInd &ig, const ColorInd &iv) const;
  virtual EdgeObj *createEdgeObj(NodeObj *fromNode, NodeObj *toNode, const BBox &rect,
                                 const ColorInd &ig, const ColorInd &iv) const;

 protected:
  using DepthNodes   = std::vector<Node *>;
  using DepthNodeSet = std::set<Node *>;

  struct AnglePair {
    double a  { 0.0 };
    double da { 2.0*M_PI };
  };

  using NodeAngles = std::map<Node *, AnglePair>;

  struct CircularNode {
    DepthNodes nodes;
  };

  using CircularDepth = std::map<int, CircularNode>;

  void placeModel() const;

  void place() const;

  void placeBuchheim() const;
  void addBuchheimHierNode(CBuchHeim::Tree *tree, Node *hierNode, int depth) const;
  void moveBuchheimHierNode(CBuchHeim::DrawTree *tree) const;

  void placeCircular() const;
  void initCircularDepth(Node *hierNode, CircularDepth &circularDepth,
                         int depth, int &maxDepth) const;

  double calcHierColor(const Node *hierNode) const;
  double calcHierSize (const Node *hierNode) const;

  bool addMenuItems(QMenu *menu, const Point &p) override;

  void setOpen(NodeObj *nodeObj, bool open);

  bool executeSlotFn(const QString &name, const QVariantList &args, QVariant &res) override;

  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  void expandNode(Node *hierNode, bool all);
  void collapseNode(Node *hierNode, bool all);

 private Q_SLOTS:
  void expandSlot();
  void expandAllSlot();
  void collapseAllSlot();

 private:
  using Dendrogram  = CQChartsDendrogram;
  using DendrogramP = std::unique_ptr<Dendrogram>;
  using NodeObjs   = std::map<Node *, NodeObj *>;

  // columns
  Column linkColumn_; //!< link column
  Column sizeColumn_; //!< size column

  DendrogramP dendrogram_;             //!< dendrogram class
  Node*       tempRoot_   { nullptr }; //!< temp root for unconnected nodes

  struct NodeData {
    Length            size         { Length::pixel(16.0) };              //!< size
    double            aspect       { 1.0 };                              //!< aspect
    CQChartsShapeType shape        { CQChartsShapeType::Type::DIAMOND }; //!< shape
    TextPosition      textPosition { TextPosition::RIGHT };              //!< text position
    bool              rotatedText  { false };                            //!< is label rotated
    double            textMargin   { 4.0 };                              //!< text margin
  };

  // root node data
  bool     rootVisible_ { true }; //!< root is visible
  NodeData rootNodeData_;         //!< root node data

  // hier node data
  NodeData hierNodeData_; //!< hier node data

  // leaf node data
  NodeData leafNodeData_; //!< leaf node data

  // edge data
  bool   edgeScaled_   { false };              //!< is edge scaled
  Length edgeWidth_    { Length::pixel(8.0) }; //!< edge width
  Length minEdgeWidth_ { Length::pixel(1.0) }; //!< min edge width

  // plot data
  Qt::Orientation orientation_ { Qt::Horizontal };      //!< draw direction
  PlaceType       placeType_   { PlaceType::BUCHHEIM }; //!< place type
  int             hideDepth_   { -1 };                  //!< place type
  double          sizeScale_   { 1.0 };                 //!< size scale

  // overlap
  bool           removeNodeOverlaps_  { false };              //!< remove node overlaps
  bool           removeGroupOverlaps_ { false };              //!< remove group overlaps
  Length         overlapMargin_       { Length::pixel(4.0) }; //!< overlap margin
  bool           adjustOverlaps_      { false };              //!< adjust overlaps
  mutable double overlapScale_        { 1.0 };                //!< overlap scale factor

  RMinMax valueRange_; //!< value range
  RMinMax colorRange_; //!< color range

  //---

  // cached data
  struct CacheData {
    using TreeP     = std::unique_ptr<CBuchHeim::Tree>;
    using DrawTreeP = std::unique_ptr<CBuchHeim::DrawTree>;

    bool needsReload { true }; //!< needs reload
    bool needsPlace  { true }; //!< needs place

    TreeP     buchheimTree;     //!< buchheim tree
    DrawTreeP buchheimDrawTree; //!< buchheim draw tree

    NodeObj *rootNodeObj { nullptr }; //!< root node obj

    NodeObjs nodeObjs;

    int numNodes { 0 };
    int numEdges { 0 };
    int depth    { 0 };

    int nodeInd { 0 };
    int edgeInd { 0 };
  };

  mutable CacheData cacheData_;
};

//---

/*!
 * \brief Dendrogram Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsDendrogramPlotCustomControls : public CQChartsHierPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsDendrogramPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

  void orientationSlot();

 protected:
  void addWidgets() override;

  void addOptionsWidgets() override;

  void addColumnWidgets() override;

  void connectSlots(bool b) override;

 protected:
  CQChartsDendrogramPlot* dendrogramPlot_ { nullptr }; //!< plot

  FrameData optionsFrame_;

  CQChartsEnumParameterEdit* orientationCombo_ { nullptr };
};

#endif
