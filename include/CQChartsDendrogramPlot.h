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

  struct Edge {
    Edge() = default;

    Edge(NodeObj *node, const OptReal &value) :
     node(node), value(value) {
    }

    NodeObj *node { nullptr };
    OptReal  value;
  };

  using Children = std::vector<Edge>;

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

  const OptReal &hierValue() const { return hierValue_; }
  void setHierValue(const OptReal &r) { hierValue_ = r; }

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

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  const OptReal &colorValue() const { return colorValue_; }
  void setColorValue(const OptReal &v) { colorValue_ = v; }

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

  void getObjSelectIndices(Indices &inds) const override;

  bool inside(const Point &p) const override;

  //---

  BBox textRect() const;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void drawText(PaintDevice *device, const QColor &shapeColor) const;

  void calcTextPos(Point &p, const QFont &font, Angle &angle, uint &position,
                   Qt::Alignment &align, bool &centered) const;

  // convert value to string for node value
  virtual QString calcValueLabel() const;

  //---

  BBox displayRect() const;

//void movePerpCenter(double pos);

  void movePerpBy(double d);
  void moveParBy(double d);

 private:
  QSizeF calcScaledShapePixelSize() const;

 private:
  const DendrogramPlot* dendrogramPlot_ { nullptr }; //!< plot
  NodeObj*              parent_         { nullptr }; //!< parent
  Children              children_;                   //!< children
  Node*                 node_           { nullptr }; //!< associated node
  QString               name_;                       //!< name
  int                   ind_            { 0 };       //!< unique index
  OptReal               value_;                      //!< value
  OptReal               hierValue_;                  //!< hier value
  double                childTotal_     { 0.0 };     //!< child total
  bool                  isRoot_         { false };   //!< is root
  bool                  isHier_         { false };   //!< is hierarchical
  bool                  open_           { false };   //!< is open
  ModelIndex            modelIndex_;                 //!< model index
  Color                 color_;                      //!< optional color
  OptReal               colorValue_;                 //!< optional color value
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

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  const OptReal &colorValue() const { return colorValue_; }
  void setColorValue(const OptReal &v) { colorValue_ = v; }

  const OptReal &size() const { return size_; }
  void setSize(const OptReal &v) { size_ = v; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  //---

  void getEdgePoints(Point &p1, Point &p4) const;

 private:
  const DendrogramPlot* dendrogramPlot_ { nullptr }; //!< plot
  NodeObj*              fromNode_       { nullptr }; //!< from node object
  NodeObj*              toNode_         { nullptr }; //!< to node object
  int                   ind_            { 0 };       //!< unique index
  OptReal               value_;                      //!< optional value
  Color                 color_;                      //!< optional color
  OptReal               colorValue_;                 //!< optional color value
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

  Q_PROPERTY(CQChartsColumn swatchColorColumn READ swatchColorColumn WRITE setSwatchColorColumn)

  // root node data
  Q_PROPERTY(bool              rootVisible READ isRootVisible WRITE setRootVisible)
  Q_PROPERTY(CQChartsLength    rootSize    READ rootSize      WRITE setRootSize   )
  Q_PROPERTY(CQChartsLength    rootMinSize READ rootMinSize   WRITE setRootMinSize)
  Q_PROPERTY(double            rootAspect  READ rootAspect    WRITE setRootAspect )
  Q_PROPERTY(CQChartsShapeType rootShape   READ rootShape     WRITE setRootShape  )

  Q_PROPERTY(TextPosition rootTextPosition READ rootTextPosition  WRITE setRootTextPosition)
  Q_PROPERTY(bool         rootRotatedText  READ isRootRotatedText WRITE setRootRotatedText )
  Q_PROPERTY(double       rootTextMargin   READ rootTextMargin    WRITE setRootTextMargin  )
  Q_PROPERTY(bool         rootValueLabel   READ isRootValueLabel  WRITE setRootValueLabel  )

  // hier node data
  Q_PROPERTY(CQChartsLength    hierSize    READ hierSize    WRITE setHierSize   )
  Q_PROPERTY(CQChartsLength    hierMinSize READ hierMinSize WRITE setHierMinSize)
  Q_PROPERTY(double            hierAspect  READ hierAspect  WRITE setHierAspect )
  Q_PROPERTY(CQChartsShapeType hierShape   READ hierShape   WRITE setHierShape  )

  Q_PROPERTY(TextPosition hierTextPosition READ hierTextPosition  WRITE setHierTextPosition)
  Q_PROPERTY(bool         hierRotatedText  READ isHierRotatedText WRITE setHierRotatedText )
  Q_PROPERTY(double       hierTextMargin   READ hierTextMargin    WRITE setHierTextMargin  )
  Q_PROPERTY(bool         hierValueLabel   READ isHierValueLabel  WRITE setHierValueLabel  )
  Q_PROPERTY(bool         colorByHier      READ isColorByHier     WRITE setColorByHier     )

  // leaf node data
  Q_PROPERTY(CQChartsLength    leafSize    READ leafSize    WRITE setLeafSize   )
  Q_PROPERTY(CQChartsLength    leafMinSize READ leafMinSize WRITE setLeafMinSize)
  Q_PROPERTY(double            leafAspect  READ leafAspect  WRITE setLeafAspect )
  Q_PROPERTY(CQChartsShapeType leafShape   READ leafShape   WRITE setLeafShape  )

  Q_PROPERTY(TextPosition leafTextPosition READ leafTextPosition  WRITE setLeafTextPosition)
  Q_PROPERTY(bool         leafRotatedText  READ isLeafRotatedText WRITE setLeafRotatedText )
  Q_PROPERTY(double       leafTextMargin   READ leafTextMargin    WRITE setLeafTextMargin  )
  Q_PROPERTY(bool         leafValueLabel   READ isLeafValueLabel  WRITE setLeafValueLabel  )

  // gen node data
  Q_PROPERTY(ValueType nodeColorByValue READ nodeColorByValue WRITE setNodeColorByValue)
  Q_PROPERTY(ValueType nodeSizeByValue  READ nodeSizeByValue  WRITE setNodeSizeByValue )
  Q_PROPERTY(bool      colorClosed      READ isColorClosed    WRITE setColorClosed     )
  Q_PROPERTY(bool      swatchColor      READ isSwatchColor    WRITE setSwatchColor     )
  Q_PROPERTY(double    swatchSize       READ swatchSize       WRITE setSwatchSize      )

  // edge data
  Q_PROPERTY(EdgeType       edgeType         READ edgeType           WRITE setEdgeType        )
  Q_PROPERTY(CQChartsLength edgeWidth        READ edgeWidth          WRITE setEdgeWidth       )
  Q_PROPERTY(CQChartsLength minEdgeWidth     READ minEdgeWidth       WRITE setMinEdgeWidth    )
  Q_PROPERTY(bool           edgeColorByValue READ isEdgeColorByValue WRITE setEdgeColorByValue)
  Q_PROPERTY(bool           edgeSizeByValue  READ isEdgeSizeByValue  WRITE setEdgeSizeByValue )
  Q_PROPERTY(bool           edgeSelectable   READ isEdgeSelectable   WRITE setEdgeSelectable  )

  // options
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation )

  Q_PROPERTY(PlaceType placeType READ placeType WRITE setPlaceType)

  Q_PROPERTY(FitMode fitMode READ fitMode WRITE setFitMode)

  Q_PROPERTY(int hideDepth READ hideDepth WRITE setHideDepth)

  Q_PROPERTY(double sizeScale READ sizeScale WRITE setSizeScale)

  Q_PROPERTY(bool removeNodeOverlaps  READ isRemoveNodeOverlaps  WRITE setRemoveNodeOverlaps)
  Q_PROPERTY(bool removeGroupOverlaps READ isRemoveGroupOverlaps WRITE setRemoveGroupOverlaps)
  Q_PROPERTY(bool adjustOverlaps      READ isAdjustOverlaps      WRITE setAdjustOverlaps)
  Q_PROPERTY(bool spreadNodeOverlaps  READ isSpreadNodeOverlaps  WRITE setSpreadNodeOverlaps)

  Q_PROPERTY(bool propagateHier READ isPropagateHier WRITE setPropagateHier)
  Q_PROPERTY(bool hierValueTip  READ isHierValueTip  WRITE setHierValueTip)

  Q_PROPERTY(CQChartsLength overlapMargin READ overlapMargin WRITE setOverlapMargin)

  Q_PROPERTY(bool   depthSort   READ isDepthSort   WRITE setDepthSort)
  Q_PROPERTY(bool   reverseSort READ isReverseSort WRITE setReverseSort)
  Q_PROPERTY(double sortSize    READ sortSize      WRITE setSortSize)

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
  Q_ENUMS(FitMode)
  Q_ENUMS(TextPosition)
  Q_ENUMS(ValueType)
  Q_ENUMS(EdgeType)

 public:
  using NodeObj   = CQChartsDendrogramNodeObj;
  using EdgeObj   = CQChartsDendrogramEdgeObj;
  using Node      = CQChartsDendrogram::Node;
  using Font      = CQChartsFont;
  using Color     = CQChartsColor;
  using Length    = CQChartsLength;
  using PenBrush  = CQChartsPenBrush;
  using BrushData = CQChartsBrushData;
  using ColorInd  = CQChartsUtil::ColorInd;

  enum class PlaceType {
    DENDROGRAM,
    BUCHHEIM,
    SORTED,
    CIRCULAR
  };

  enum class FitMode {
    SCALE,
    SCROLL
  };

  enum class TextPosition {
    LEFT,
    RIGHT,
    CENTER,
    INSIDE,
    OUTSIDE
  };

  struct Edge {
    Node*   from { nullptr };
    Node*   to   { nullptr };
    OptReal value;
    Color   color;
    OptReal colorValue;
    OptReal sizeValue;

    Edge() = default;

    Edge(Node *from, Node *to, const OptReal &value) :
     from(from), to(to), value(value) {
    }
  };

  using Edges = std::vector<Edge *>;

  enum class ValueType {
    NONE,
    NODE,
    HIER,
    TARGET_NODE
  };

  enum class EdgeType {
    CURVE,
    LINE
  };

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

  void setValueColumn(const Column &c) override;

  //! get/set swatch color column
  const Column &swatchColorColumn() const { return swatchColorColumn_; }
  void setSwatchColorColumn(const Column &c);

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

  //! get/set root min size
  const Length &rootMinSize() const { return rootNodeData_.minSize; }
  void setRootMinSize(const Length &s);

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

  // get/set is root value label
  bool isRootValueLabel() const { return rootNodeData_.valueLabel; }
  void setRootValueLabel(bool b);

  QSizeF calcRootSize() const;

  //---

  // hier shape

  //! get/set hier size
  const Length &hierSize() const { return hierNodeData_.size; }
  void setHierSize(const Length &s);

  //! get/set hier min size
  const Length &hierMinSize() const { return hierNodeData_.minSize; }
  void setHierMinSize(const Length &s);

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

  // get/set is root value label
  bool isHierValueLabel() const { return hierNodeData_.valueLabel; }
  void setHierValueLabel(bool b);

  bool isColorByHier() const { return colorByHier_; }
  void setColorByHier(bool b);

  QSizeF calcHierSize() const;

  //---

  // leaf shape

  //! get/set leaf size
  const Length &leafSize() const { return leafNodeData_.size; }
  void setLeafSize(const Length &s);

  //! get/set leaf min size
  const Length &leafMinSize() const { return leafNodeData_.minSize; }
  void setLeafMinSize(const Length &s);

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

  // get/set is value label
  bool isLeafValueLabel() const { return leafNodeData_.valueLabel; }
  void setLeafValueLabel(bool b);

  QSizeF calcLeafSize() const;

  QSizeF lengthPixelSize(const Length &l, const Font &font) const;

  //---

  //! get/set is node color by value
  const ValueType &nodeColorByValue() const { return leafNodeData_.colorByValue; }
  void setNodeColorByValue(const ValueType &v);

  //! get/set is node size by value
  const ValueType &nodeSizeByValue() const { return leafNodeData_.sizeByValue; }
  void setNodeSizeByValue(const ValueType &v);

  bool isColorClosed() const { return colorClosed_; }
  void setColorClosed(bool b);

  bool isSwatchColor() const { return swatchColor_; }
  void setSwatchColor(bool b);

  double swatchSize() const { return swatchSize_; }
  void setSwatchSize(double s);

  //---

  const EdgeType &edgeType() const { return edgeData_.type; }
  void setEdgeType(const EdgeType &t);

  //! get/set edge width
  const Length &edgeWidth() const { return edgeData_.width; }
  void setEdgeWidth(const Length &l);

  //! get/set min edge width
  const Length &minEdgeWidth() const { return edgeData_.minWidth; }
  void setMinEdgeWidth(const Length &l);

  //! get/set is edge color by value
  bool isEdgeColorByValue() const { return edgeData_.colorByValue; }
  void setEdgeColorByValue(bool b);

  //! get/set is edge size by value
  bool isEdgeSizeByValue() const { return edgeData_.sizeByValue; }
  void setEdgeSizeByValue(bool b);

  //! get/set is edge selectable
  bool isEdgeSelectable() const { return edgeData_.selectable; }
  void setEdgeSelectable(bool b);

  //---

  double mapHierValue (double value) const;
  double mapValue     (double value) const;
  double mapDepthValue(int depth, double value) const;

  void depthValueRange(int depth, double &min, double &max) const;
  void valueRange     (double &min, double &max) const;

  double mapColor(double value) const;
  double mapSize(double value) const;

  double mapNodeValue(double value) const;
  double mapEdgeValue(double value) const;

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o);

  //! get/set place type
  const PlaceType &placeType() const { return placeType_; }
  void setPlaceType(const PlaceType &t);

  const FitMode &fitMode() const { return fitMode_; }
  void setFitMode(const FitMode &m);

  int hideDepth() const { return hideDepth_; }
  void setHideDepth(int i);

  double sizeScale() const { return sizeScale_; }
  void setSizeScale(double r);

  //---

  bool isRemoveNodeOverlaps() const { return removeNodeOverlaps_; }
  void setRemoveNodeOverlaps(bool b);

  bool isRemoveGroupOverlaps() const { return removeGroupOverlaps_; }
  void setRemoveGroupOverlaps(bool b);

  bool isSpreadNodeOverlaps() const { return spreadData_.enabled; }
  void setSpreadNodeOverlaps(bool b);

  const Length &overlapMargin() const { return overlapMargin_; }
  void setOverlapMargin(const Length &l);

  bool isAdjustOverlaps() const { return adjustOverlaps_; }
  void setAdjustOverlaps(bool b);

  double overlapScale() const { return overlapScale_; }

  //---

  bool isDepthSort() const { return depthSort_; }
  void setDepthSort(bool b);

  bool isReverseSort() const { return reverseSort_; }
  void setReverseSort(bool b);

  double sortSize() const { return sortSize_; }
  void setSortSize(double s);

  double sortSizeFactor() const { return sortSizeFactor_; }
  void setSortSizeFactor(double r);

  double calcSortSize() const;

  //---

  bool isPropagateHier() const { return propagateHier_; }
  void setPropagateHier(bool b);

  bool isHierValueTip() const { return hierValueTip_; }
  void setHierValueTip(bool b);

  //---

  void processEdgeNameValues(Edge *edge, const CQChartsNameValues &nameValues) const;

  void processNodeNameValues(Node *node, const CQChartsNameValues &nameValues) const;
  void processNodeNameValue(Node *node, const QString &name, const QString &valueStr) const;
  bool processNodeNameVar(Node *node, const QString &name, const QVariant &var) const;

  //---

  BBox calcExtraFitBBox() const override;

  bool isEdgeValue() const;

  //---

  void addProperties() override;

  Range calcRange() const override;

  void addHierName(const QString &nameStr, const ModelIndex &modelInd) const;

  void addNameValue(const QString &nameStr, const CQChartsNamePair &namePair,
                    const QStringList &names, const OptReal &value, const ModelIndex &modelInd,
                    const Color &color, const OptReal &colorValue, OptReal &sizeValue,
                    const CQChartsNameValues &nameValues, Edges &edges) const;

  //---

#if 0
  BBox nodesBBox() const;
  BBox nodesBBox(const PlotObjs &objs) const;
#endif

  //---

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  bool isEdgeRows() const { return isEdgeRows_; }

  //--

  void autoFit() override;

  //---

  void updateZoomScroll() override;

  void hscrollBy(double dx) override;
  void vscrollBy(double dx) override;

  bool allowZoomX() const override;
  bool allowZoomY() const override;

  void wheelVScroll(int delta) override;

  void wheelZoom(const Point &pp, int delta) override;

  void updateScrollOffset();

  //---

  void execMoveNonRoot(const PlotObjs &objs);

  //---

  void saveOverlapRects();
  void saveOverlapRects(const PlotObjs &objs);
  void restoreOverlapRects();

  void execRemoveOverlaps();
  void execRemoveOverlaps(const PlotObjs &objs);

  void execSpreadOverlaps();
  void execSpreadOverlaps(const PlotObjs &objs);

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

  //! handle double click
  bool handleSelectDoubleClick(const Point &p, SelMod selMod) override;

  //---

  Node *rootNode() const { return (dendrogram_ ? dendrogram_->root() : nullptr); }

  NodeObj *rootNodeObj() const { return cacheData_.rootNodeObj; }

  virtual NodeObj *createNodeObj(Node *node, const BBox &rect,
                                 const ColorInd &ig, const ColorInd &iv) const;
  virtual EdgeObj *createEdgeObj(NodeObj *fromNode, NodeObj *toNode, const BBox &rect,
                                 const ColorInd &ig, const ColorInd &iv) const;

  //---

#if 0
  double targetValue() const { return cacheData_.targetValue; }
#endif

 protected:
  bool processMetaNodeValue(const QString &name, const QString &key,
                            const QVariant &value) override;
  bool processMetaEdgeValue(const QString &name, const QString &key,
                            const QVariant &value) override;

 protected:
  using NodesArray = std::vector<Node *>;
  using NodeSet    = std::set<Node *>;
  using DepthNodes = std::map<int, NodesArray>;

  struct AnglePair {
    double a  { 0.0 };
    double da { 2.0*M_PI };
  };

  using NodeAngles = std::map<Node *, AnglePair>;

  struct CircularNode {
    NodesArray nodes;
  };

  using CircularDepth = std::map<int, CircularNode>;

  void placeModel() const;

  void place() const;

  void placeBuchheim() const;
  void addBuchheimHierNode(CBuchHeim::Tree *tree, Node *hierNode, int depth) const;
  void moveBuchheimHierNode(CBuchHeim::DrawTree *tree) const;

  void placeSorted() const;
  void initSortedDepth(Node *hierNode, DepthNodes &depthNodes, int depth, int &maxDepth) const;

  void placeCircular() const;
  void initCircularDepth(Node *hierNode, CircularDepth &circularDepth,
                         int depth, int &maxDepth) const;

  double calcHierColor(const Node *hierNode) const;
  double calcHierSize (const Node *hierNode) const;

  bool addMenuItems(QMenu *menu, const Point &p) override;

  bool isOpen(NodeObj *nodeObj) const;
  void setOpen(NodeObj *nodeObj, bool open);

  bool executeSlotFn(const QString &name, const QVariantList &args, QVariant &res) override;

  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  void expandNode(Node *hierNode, bool all);
  void collapseNode(Node *hierNode, bool all);

 private Q_SLOTS:
  void modelChangedSlot() override;

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

  Column swatchColorColumn_; //!< swatch color column

  DendrogramP dendrogram_;             //!< dendrogram class
  Node*       tempRoot_   { nullptr }; //!< temp root for unconnected nodes

  struct NodeData {
    Length            size         { Length::pixel(16.0) };              //!< size
    Length            minSize      { Length::pixel(8.0) };               //!< min size
    double            aspect       { 1.0 };                              //!< aspect
    CQChartsShapeType shape        { CQChartsShapeType::Type::DIAMOND }; //!< shape
    TextPosition      textPosition { TextPosition::RIGHT };              //!< text position
    bool              rotatedText  { false };                            //!< is label rotated
    bool              valueLabel   { false };                            //!< is value in label
    double            textMargin   { 4.0 };                              //!< text margin
    ValueType         colorByValue { ValueType::NONE };                  //!< color by value
    ValueType         sizeByValue  { ValueType::NONE };                  //!< size by value
  };

  // root node data
  bool     rootVisible_ { true }; //!< root is visible
  NodeData rootNodeData_;         //!< root node data

  // hier node data
  NodeData hierNodeData_;          //!< hier node data
  bool     colorByHier_ { false }; //! color by hierarchy

  // leaf node data
  NodeData leafNodeData_; //!< leaf node data

  // global node data
  bool   colorClosed_ { true };  //! fill closed node
  bool   swatchColor_ { false }; //! draw color as swatch
  double swatchSize_  { 0.1 };   //! swatch size

  // edge data
  struct EdgeData {
    EdgeType type         { EdgeType::CURVE };    //!< edge type
    bool     scaled       { false };              //!< scaled
    Length   width        { Length::pixel(8.0) }; //!< width
    Length   minWidth     { Length::pixel(1.0) }; //!< min width
    bool     colorByValue { false };              //!< color by value
    bool     sizeByValue  { false };              //!< size by value
    bool     selectable   { true };               //!< is selectable
  };

  EdgeData edgeData_;

  mutable bool isEdgeRows_ { false };

  // plot data
  Qt::Orientation orientation_ { Qt::Horizontal };      //!< draw direction
  PlaceType       placeType_   { PlaceType::BUCHHEIM }; //!< place type
  FitMode         fitMode_     { FitMode::SCALE };      //!< fit mode
  int             hideDepth_   { -1 };                  //!< place type

  double sizeScale_ { 1.0 }; //!< size scale

  // overlap
  bool           removeNodeOverlaps_  { false };              //!< remove node overlaps
  bool           removeGroupOverlaps_ { false };              //!< remove group overlaps
  Length         overlapMargin_       { Length::pixel(4.0) }; //!< overlap margin
  bool           adjustOverlaps_      { false };              //!< adjust overlaps
  mutable double overlapScale_        { 1.0 };                //!< overlap scale factor

  bool   depthSort_      { true };  //!< sort per depth
  bool   reverseSort_    { false }; //!< reverse sort
  double sortSize_       { 1.0 };   //!< sort perp range size
  double sortSizeFactor_ { 1.0 };   //!< sort size factor

  //---

  struct SpreadData {
    bool   enabled { false }; //!< spread node overlaps
    double scale   { 1.0 };   //!< spread scale
    double pos     { 0.0 };   //!< spread pos
    BBox   bbox;              //!< spread bbox

    void reset() {
      scale = 1.0;
      pos   = 0.0;
      bbox  = BBox();
    }
  };

  SpreadData spreadData_;

  //---

  struct SortedData {
    double pos { 0.0 }; //!< sorted pos
  };

  SortedData sortedData_;

  //---

  bool propagateHier_ { false }; //!< propagate values through hierarchy
  bool hierValueTip_  { true };  //!< show hier value in tip

  RMinMax valueRange_; //!< value column range
  RMinMax colorRange_; //!< color column range
  RMinMax sizeRange_;  //!< size column range

  RMinMax nodeValueRange_; //!< node value range
  RMinMax edgeValueRange_; //!< edge value range

  using DepthValueRange = std::map<int, RMinMax>;

  DepthValueRange depthValueRange_; //!< depth value column range

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

//  double targetValue { 0.0 };
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

  void addButtonWidgets();

  void connectSlots(bool b) override;

 protected:
  CQChartsDendrogramPlot* dendrogramPlot_ { nullptr }; //!< plot

  FrameData optionsFrame_;
  FrameData buttonsFrame_;

  CQChartsEnumParameterEdit* orientationCombo_ { nullptr };

  QPushButton *expand_      { nullptr };
  QPushButton *expandAll_   { nullptr };
  QPushButton *collapseAll_ { nullptr };
};

#endif
