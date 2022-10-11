#ifndef CQChartsDendrogramPlot_H
#define CQChartsDendrogramPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
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
class CQChartsDendrogramPlotType : public CQChartsPlotType {
 public:
  CQChartsDendrogramPlotType();

  QString name() const override { return "dendrogram"; }
  QString desc() const override { return "Dendrogram"; }

  void addParameters() override;

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
  using Plot     = CQChartsDendrogramPlot;
  using NodeObj  = CQChartsDendrogramNodeObj;
  using Node     = CQChartsDendrogram::Node;
  using Length   = CQChartsLength;
  using Angle    = CQChartsAngle;
  using Sides    = CQChartsSides;
  using OptReal  = CQChartsOptReal;

 public:
  CQChartsDendrogramNodeObj(const Plot *plot, Node *node, const BBox &rect);

  QString typeName() const override { return "node"; }

  //---

  const Plot *plot() const { return plot_; }

  const NodeObj *parent() const { return parent_; }
  void setParent(NodeObj *p) { parent_ = p; }

  void addChild(NodeObj *node, const OptReal &value=OptReal());

  const Node *node() const { return node_; }
  void resetNode() { node_ = nullptr; }

  //---

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  const QString &name() const { return name_; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

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

  void drawEdges(PaintDevice *device) const;

  void calcTextPos(Point &p, const QFont &font, Angle &angle,
                   Qt::Alignment &align, bool &centered) const;

  //---

 private:
  void drawEdge(PaintDevice *device, const NodeObj *child, const OptReal &value) const;

  BBox displayRect() const;

  double calcSymbolSize() const;

 private:
  struct Child {
    Child() { }

    Child(NodeObj *node, const OptReal &value) :
     node(node), value(value) {
    }

    NodeObj *node { nullptr };
    OptReal  value;
  };

  using Children = std::vector<Child>;

  const Plot*     plot_       { nullptr }; //!< plot
  NodeObj*        parent_     { nullptr }; //!< parent
  Children        children_;               //!< children
  Node*           node_       { nullptr }; //!< associated node
  QString         name_;                   //!< name
  int             ind_        { 0 };       //!< unique index
  double          value_      { 0.0 };     //!< value
  double          childTotal_ { 0.0 };     //!< value
  bool            isRoot_     { false };   //!< is root
  bool            isHier_     { false };   //!< is hierarchical
  bool            open_       { false };   //!< is open
  ModelIndex      modelIndex_;             //!< model index
  OptReal         color_;                  //!< optional color
  OptReal         size_;                   //!< optional size
  mutable double  hierColor_  { 0.0 };     //!< hier color
  mutable double  hierSize_   { 0.0 };     //!< hier size
};

//---

CQCHARTS_NAMED_TEXT_DATA(Hier, hier)
CQCHARTS_NAMED_TEXT_DATA(Leaf, leaf)

/*!
 * \brief Dendrogram Plot
 * \ingroup Charts
 */
class CQChartsDendrogramPlot : public CQChartsPlot,
 public CQChartsObjNodeShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjEdgeShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjHierTextData <CQChartsDendrogramPlot>,
 public CQChartsObjLeafTextData <CQChartsDendrogramPlot> {
  Q_OBJECT

  //! follow view
  Q_PROPERTY(bool followViewExpand READ isFollowViewExpand WRITE setFollowViewExpand)

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn linkColumn  READ linkColumn  WRITE setLinkColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn sizeColumn  READ sizeColumn  WRITE setSizeColumn )

  // node data
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)

  // edge data
//Q_PROPERTY(bool           edgeArrow   READ isEdgeArrow  WRITE setEdgeArrow )
  Q_PROPERTY(bool           edgeScaled  READ isEdgeScaled WRITE setEdgeScaled)
  Q_PROPERTY(CQChartsLength edgeWidth   READ edgeWidth    WRITE setEdgeWidth )
//Q_PROPERTY(double         arrowWidth  READ arrowWidth   WRITE setArrowWidth)

  // label
  Q_PROPERTY(double textMargin  READ textMargin    WRITE setTextMargin )
  Q_PROPERTY(bool   rotatedText READ isRotatedText WRITE setRotatedText)

  // options
  Q_PROPERTY(PlaceType placeType READ placeType WRITE setPlaceType)

  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation )

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)

  // edge line
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Edge, edge)

  // hier, leaf labels
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Hier, hier)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Leaf, leaf)

  Q_ENUMS(PlaceType)

 public:
  using NodeObj   = CQChartsDendrogramNodeObj;
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

  struct Edge {
    Node*   from { nullptr };
    Node*   to   { nullptr };
    OptReal value;

    Edge() { }

    Edge(Node *from, Node *to, const OptReal &value) :
     from(from), to(to), value(value) {
    }
  };

  using Edges = std::vector<Edge>;

 public:
  CQChartsDendrogramPlot(View *view, const ModelP &model);
 ~CQChartsDendrogramPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set name column
  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  //! get/set link column
  const Column &linkColumn() const { return linkColumn_; }
  void setLinkColumn(const Column &c);

  //! get/set value column
  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  void setColorColumn(const Column &c) override;

  //! get/set value column
  const Column &sizeColumn() const { return sizeColumn_; }
  void setSizeColumn(const Column &c);

  //---

  //! get/set named column
  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  //! get/set follow view expand
  bool isFollowViewExpand() const { return followViewExpand_; }
  void setFollowViewExpand(bool b);

  //---

  //! get/set circle size
  const Length &symbolSize() const { return symbolSize_; }
  void setSymbolSize(const Length &s);

  double calcSymbolSize() const;

  //---

  //! get/set is edge scaled
  bool isEdgeScaled() const { return edgeScaled_; }
  void setEdgeScaled(bool b);

  //! get/set edge width
  const Length &edgeWidth() const { return edgeWidth_; }
  void setEdgeWidth(const Length &l);

  //---

  //! get/set text margin
  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

  // get/set is rotated text label
  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b);

  //---

  //! get/set place type
  const PlaceType &placeType() const { return placeType_; }
  void setPlaceType(const PlaceType &t);

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &o);

  //---

  BBox calcExtraFitBBox() const override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  void addHierName(const QString &nameStr, const ModelIndex &modelInd) const;

  void addNameValue(const QString &nameStr, const CQChartsNamePair &namePair, const OptReal &value,
                    const ModelIndex &modelInd, const OptReal &colorValue,
                    OptReal &sizeValue, Edges &edges) const;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void addNodeObjs(Node *hier, int depth, NodeObj *parentObj, PlotObjs &objs) const;

  NodeObj *addNodeObj(Node *node, PlotObjs &objs, bool isHier) const;

  //---

  void preDrawObjs(PaintDevice *device) const override;

  //---

  void modelViewExpansionChanged() override;

  void resetNodeExpansion(bool expanded);
  void resetNodeExpansion(Node *hierNode, bool expanded);

  void setNodeExpansion(Node *hierNode, const std::set<QModelIndex> &indSet);

  void expandedModelIndices(std::set<QModelIndex> &indSet) const;

  //---

  BBox getBBox(Node *node) const;

  //---

  //! handle select press
  bool handleSelectPress(const Point &p, SelMod selMod) override;

  //---

  Node *rootNode() const { return (dendrogram_ ? dendrogram_->root() : nullptr); }

  NodeObj *rootNodeObj() const { return cacheData_.rootNodeObj; }

  virtual NodeObj *createNodeObj(Node *node, const BBox &rect) const;

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
  void addBuchheimHierNode(CBuchHeim::Tree *tree, Node *hierNode) const;
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

  bool isExpandModelIndex(const QModelIndex &ind) const;

  void expandModelIndex(const QModelIndex &ind, bool b);

 private Q_SLOTS:
  void expandSlot();
  void expandAllSlot();
  void collapseAllSlot();

 private:
  using Dendrogram = CQChartsDendrogram;
  using NodeObjs   = std::map<Node *, NodeObj *>;

  // columns
  Column nameColumn_;  //!< name column
  Column linkColumn_;  //!< link column
  Column valueColumn_; //!< value column
  Column sizeColumn_;  //!< size column

  bool followViewExpand_ { false }; //!< follow view expand

  Dendrogram* dendrogram_ { nullptr }; //!< dendrogram class
  Node*       tempRoot_   { nullptr };

  // node data
  Length symbolSize_ { Length::pixel(16.0) }; //!< circle size

  // edge data
  bool   edgeScaled_ { false };            //!< is edge scaled
  Length edgeWidth_  { Length::pixel(8) }; //!< edge width

  // test data
  double textMargin_  { 4.0 };   //!< text margin
  bool   rotatedText_ { false }; //!< is label rotated

  // plot data
  PlaceType       placeType_   { PlaceType::BUCHHEIM }; //!< place type
  Qt::Orientation orientation_ { Qt::Horizontal };      //!< draw direction

  //---

  // cached data
  struct CacheData {
    bool needsReload { true }; //!< needs reload
    bool needsPlace  { true }; //!< needs place

    CBuchHeim::Tree*     buchheimTree     { nullptr }; //!< buchheim tree
    CBuchHeim::DrawTree* buchheimDrawTree { nullptr }; //!< buchheim draw tree

    NodeObj *rootNodeObj { nullptr }; //!< root node obj

    NodeObjs nodeObjs;

    int nodeInd { 0 };
  };

  mutable CacheData cacheData_;
};

//---

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Dendrogram Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsDendrogramPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsDendrogramPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(CQChartsPlot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

  void orientationSlot();

 protected:
  void addWidgets() override;

  void addOptionsWidgets() override;

  void addColumnWidgets() override;

  void connectSlots(bool b) override;

 protected:
  CQChartsDendrogramPlot* plot_ { nullptr }; //!< plot

  FrameData optionsFrame_;

  CQChartsEnumParameterEdit* orientationCombo_ { nullptr };
};

#endif
