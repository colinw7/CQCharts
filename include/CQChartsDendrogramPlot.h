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

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Dendrogram Plot Node object
 * \ingroup Charts
 */
class CQChartsDendrogramNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsDendrogramNodeObj(const CQChartsDendrogramPlot *plot, CQChartsDendrogram::Node *node,
                            const CQChartsGeom::BBox &rect);

  QString typeName() const override { return "node"; }

  QString calcId() const override;

  CQChartsGeom::BBox textRect() const;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsDendrogramPlot* plot_ { nullptr };
  CQChartsDendrogram::Node*     node_ { nullptr };
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
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Node,node)

  // edge line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Edge,edge)

  // labels
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  CQChartsDendrogramPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsDendrogramPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  //---

  double circleSize() const { return circleSize_; }
  void setCircleSize(double r);

  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  void addNameValue(const QString &name, double value) const;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void addNodeObjs(CQChartsDendrogram::HierNode *hier, int depth, PlotObjs &objs) const;
  void addNodeObj (CQChartsDendrogram::Node *node, PlotObjs &objs) const;

  //---

  bool selectPress(const CQChartsGeom::Point &p, SelMod selMod) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *) const override;

  void drawNodes(CQChartsPaintDevice *device, CQChartsDendrogram::HierNode *hier, int depth) const;

  void drawNode(CQChartsPaintDevice *device, CQChartsDendrogram::HierNode *hier,
                CQChartsDendrogram::Node *node) const;

 private:
  CQChartsColumn      nameColumn_;              //!< name column
  CQChartsColumn      valueColumn_;             //!< value column
  CQChartsDendrogram* dendrogram_  { nullptr }; //!< dendrogram class
  double              circleSize_  { 8.0 };     //!< circle size
  double              textMargin_  { 4.0 };     //!< text margin
};

#endif
