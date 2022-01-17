#ifndef CQChartsDendrogramPlot_H
#define CQChartsDendrogramPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDendrogram.h>

class CQChartsDendrogramPlot;

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
  using HierNode = CQChartsDendrogram::HierNode;
  using Node     = CQChartsDendrogram::Node;
  using Length   = CQChartsLength;
  using Angle    = CQChartsAngle;
  using Sides    = CQChartsSides;
  using OptReal  = CQChartsOptReal;

 public:
  CQChartsDendrogramNodeObj(const Plot *plot, Node *node, const BBox &rect);

  QString typeName() const override { return "node"; }

  const Plot *plot() const { return plot_; }

  const NodeObj *parent() const { return parent_; }
  void setParent(NodeObj *p) { parent_ = p; }

  void addChild(NodeObj *child) { children_.push_back(child); }

  const Node *node() const { return node_; }
  void resetNode() { node_ = nullptr; }

  const QString &name() const { return name_; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  bool isHier() const { return isHier_; }
  void setHier(bool b) { isHier_ = b; }

  bool isOpen() const { return open_; }
  void setOpen(bool b) { open_ = b; }

  ModelIndex modelIndex() const { return modelIndex_; }
  void setModelIndex(const ModelIndex &modelIndex) { modelIndex_ = modelIndex; }

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

 private:
  void drawEdge(PaintDevice *device, const NodeObj *child) const;

  BBox displayRect() const;

 private:
  using Children = std::vector<NodeObj *>;

  const Plot*     plot_      { nullptr };
  NodeObj*        parent_    { nullptr };
  Children        children_;
  Node*           node_      { nullptr };
  QString         name_;
  double          value_     { 0.0 };
  bool            isHier_    { false };
  bool            open_      { false };
  ModelIndex      modelIndex_;
  OptReal         color_;
  OptReal         size_;
  mutable double  hierColor_ { 0.0 };
  mutable double  hierSize_  { 0.0 };
};

//---

CQCHARTS_NAMED_TEXT_DATA(HierLabel, hierLabel)
CQCHARTS_NAMED_TEXT_DATA(LeafLabel, leafLabel)

/*!
 * \brief Dendrogram Plot
 * \ingroup Charts
 */
class CQChartsDendrogramPlot : public CQChartsPlot,
 public CQChartsObjNodeShapeData    <CQChartsDendrogramPlot>,
 public CQChartsObjEdgeLineData     <CQChartsDendrogramPlot>,
 public CQChartsObjHierLabelTextData<CQChartsDendrogramPlot>,
 public CQChartsObjLeafLabelTextData<CQChartsDendrogramPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn sizeColumn  READ sizeColumn  WRITE setSizeColumn )

  // node
  Q_PROPERTY(CQChartsLength circleSize READ circleSize WRITE setCircleSize)

  // label
  Q_PROPERTY(double textMargin READ textMargin WRITE setTextMargin)

  // options
  Q_PROPERTY(PlaceType placeType READ placeType WRITE setPlaceType)

  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation )

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)

  // edge line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge, edge)

  // hier, leaf labels
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(HierLabel, hierLabel)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(LeafLabel, leafLabel)

  Q_ENUMS(PlaceType)

 public:
  using NodeObj   = CQChartsDendrogramNodeObj;
  using HierNode  = CQChartsDendrogram::HierNode;
  using Node      = CQChartsDendrogram::Node;
  using Color     = CQChartsColor;
  using Length    = CQChartsLength;
  using PenBrush  = CQChartsPenBrush;
  using BrushData = CQChartsBrushData;
  using ColorInd  = CQChartsUtil::ColorInd;

  enum class PlaceType {
    DENDROGRAM,
    BUCHHEIM
  };

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

  //! get/set circle size
  const Length &circleSize() const { return circleSize_; }
  void setCircleSize(const Length &s);

  double calcCircleSize() const;

  //---

  //! get/set text margin
  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

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

  void addNameValue(const QString &name, double value, const ModelIndex &modelInd,
                    const OptReal &colorValue, OptReal &sizeValue) const;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void addNodeObjs(HierNode *hier, int depth, NodeObj *parentObj, PlotObjs &objs) const;

  NodeObj *addNodeObj(Node *node, PlotObjs &objs, bool isHier) const;

  BBox getBBox(Node *node) const;

  //---

  //! handle select press
  bool handleSelectPress(const Point &p, SelMod selMod) override;

  //---

  NodeObj *rootNodeObj() const { return rootNodeObj_; }

  virtual NodeObj *createNodeObj(Node *node, const BBox &rect) const;

 protected:
  void placeModel() const;

  void place() const;

  void addBuchheimHierNode(CBuchHeim::Tree *tree, HierNode *hierNode) const;
  void moveBuchheimHierNode(CBuchHeim::DrawTree *tree) const;

  double calcHierColor(const HierNode *hierNode) const;
  double calcHierSize (const HierNode *hierNode) const;

  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using Dendrogram = CQChartsDendrogram;

  Column          nameColumn_;                            //!< name column
  Column          valueColumn_;                           //!< value column
  Column          sizeColumn_;                            //!< size column
  Dendrogram*     dendrogram_  { nullptr };               //!< dendrogram class
  Length          circleSize_  { Length::pixel(32.0) };   //!< circle size
  double          textMargin_  { 4.0 };                   //!< text margin
  PlaceType       placeType_   { PlaceType::DENDROGRAM }; //!< place type
  Qt::Orientation orientation_ { Qt::Horizontal };        //!< draw direction

  NodeObj *rootNodeObj_ { nullptr };

  mutable bool needsReload_ { true };
  mutable bool needsPlace_  { true };

  mutable CBuchHeim::Tree*     buchheimTree_     { nullptr };
  mutable CBuchHeim::DrawTree* buchheimDrawTree_ { nullptr };
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

  void addWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 protected:
  void connectSlots(bool b) override;

 public slots:
  void updateWidgets() override;

 private:
  CQChartsDendrogramPlot* plot_ { nullptr };
};

#endif
