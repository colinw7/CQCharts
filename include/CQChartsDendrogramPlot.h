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
  using HierNode = CQChartsDendrogram::HierNode;
  using Node     = CQChartsDendrogram::Node;
  using Angle    = CQChartsAngle;

 public:
  CQChartsDendrogramNodeObj(const Plot *plot, Node *node, const BBox &rect);

  QString typeName() const override { return "node"; }

  QString calcId() const override;

  BBox textRect() const;

  void draw(PaintDevice *device) const override;

 private:
  const Plot* plot_ { nullptr };
  Node*       node_ { nullptr };
};

//---

/*!
 * \brief Dendrogram Plot
 * \ingroup Charts
 */
class CQChartsDendrogramPlot : public CQChartsPlot,
 public CQChartsObjNodeShapeData<CQChartsDendrogramPlot>,
 public CQChartsObjEdgeLineData <CQChartsDendrogramPlot>,
 public CQChartsObjTextData     <CQChartsDendrogramPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  // node
  Q_PROPERTY(double circleSize READ circleSize WRITE setCircleSize)

  // label
  Q_PROPERTY(double textMargin READ textMargin WRITE setTextMargin)

  // options
  Q_PROPERTY(PlaceType placeType READ placeType WRITE setPlaceType)

  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation )

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)

  // edge line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge, edge)

  // labels
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(PlaceType)

 public:
  using HierNode  = CQChartsDendrogram::HierNode;
  using Node      = CQChartsDendrogram::Node;
  using Color     = CQChartsColor;
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

  //---

  //! get/set named column
  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  //! get/set circle size
  double circleSize() const { return circleSize_; }
  void setCircleSize(double r);

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

  void addNameValue(const QString &name, double value) const;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void addNodeObjs(HierNode *hier, int depth, PlotObjs &objs) const;

  void addNodeObj(Node *node, PlotObjs &objs) const;

  BBox getBBox(Node *node) const;

  //---

  //! handle select press
  bool handleSelectPress(const Point &p, SelMod selMod) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  void drawNodes(PaintDevice *device, HierNode *hier, int depth) const;

  void drawNode(PaintDevice *device, HierNode *hier, Node *node) const;

  //---

  using NodeObj = CQChartsDendrogramNodeObj;

  virtual NodeObj *createNodeObj(Node *node, const BBox &rect) const;

 protected:
  void place() const;

  void addBuchheimHierNode(CBuchHeim::Tree *tree, HierNode *hierNode) const;
  void moveBuchheimHierNode(CBuchHeim::DrawTree *tree) const;

  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using Dendrogram = CQChartsDendrogram;

  Column          nameColumn_;                            //!< name column
  Column          valueColumn_;                           //!< value column
  Dendrogram*     dendrogram_  { nullptr };               //!< dendrogram class
  double          circleSize_  { 8.0 };                   //!< circle size
  double          textMargin_  { 4.0 };                   //!< text margin
  PlaceType       placeType_   { PlaceType::DENDROGRAM }; //!< place type
  Qt::Orientation orientation_ { Qt::Horizontal };        //!< draw direction

  mutable CBuchHeim::Tree*     buchheimTree_     { nullptr };
  mutable CBuchHeim::DrawTree* buchheimDrawTree_ { nullptr };
};

//---

#include <CQChartsPlotCustomControls.h>

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
