#ifndef CQChartsDendrogramPlot_H
#define CQChartsDendrogramPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDendrogram.h>

class CQChartsDendrogramPlot;

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
  using Plot = CQChartsDendrogramPlot;
  using Node = CQChartsDendrogram::Node;

 public:
  CQChartsDendrogramNodeObj(const Plot *plot, Node *node, const BBox &rect);

  QString typeName() const override { return "node"; }

  QString calcId() const override;

  BBox textRect() const;

  void draw(PaintDevice *device) override;

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

  // options
  Q_PROPERTY(double circleSize READ circleSize WRITE setCircleSize )
  Q_PROPERTY(double textMargin READ textMargin WRITE setTextMargin )

  // node stroke/fill
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node, node)

  // edge line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge, edge)

  // labels
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using HierNode = CQChartsDendrogram::HierNode;
  using Node     = CQChartsDendrogram::Node;

 public:
  CQChartsDendrogramPlot(View *view, const ModelP &model);

 ~CQChartsDendrogramPlot();

  //---

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  double circleSize() const { return circleSize_; }
  void setCircleSize(double r);

  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

  //---

  BBox calcAnnotationBBox() const override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  void addNameValue(const QString &name, double value) const;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void addNodeObjs(HierNode *hier, int depth, PlotObjs &objs) const;
  void addNodeObj (Node *node, PlotObjs &objs) const;

  //---

  bool selectPress(const Point &p, SelMod selMod) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  void drawNodes(PaintDevice *device, HierNode *hier, int depth) const;

  void drawNode(PaintDevice *device, HierNode *hier, Node *node) const;

  //---

  using NodeObj = CQChartsDendrogramNodeObj;

  virtual NodeObj *createNodeObj(Node *node, const BBox &rect) const;

 private:
  using Dendrogram = CQChartsDendrogram;

  Column      nameColumn_;              //!< name column
  Column      valueColumn_;             //!< value column
  Dendrogram* dendrogram_  { nullptr }; //!< dendrogram class
  double      circleSize_  { 8.0 };     //!< circle size
  double      textMargin_  { 4.0 };     //!< text margin
};

#endif
