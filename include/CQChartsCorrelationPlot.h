#ifndef CQChartsCorrelationPlot_H
#define CQChartsCorrelationPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

class CQChartsFilterModel;

//---

/*!
 * \brief Table plot type
 * \ingroup Charts
 */
class CQChartsCorrelationPlotType : public CQChartsPlotType {
 public:
  CQChartsCorrelationPlotType();

  QString name() const override { return "correlation"; }
  QString desc() const override { return "Correlation"; }

  // no dimension (uses whole model)
  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsCorrelationPlot;

/*!
 * \brief Image Plot cell object
 * \ingroup Charts
 */
class CQChartsCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsCellObj(const CQChartsCorrelationPlot *plot, const CQChartsGeom::BBox &rect,
                  int row, int col, double value, const QModelIndex &ind,
                  const ColorInd &iv);

  //---

  QString typeName() const override { return "correlation"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  void getSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(CQChartsScriptPainter *device) const override;

  //---

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 private:
  const CQChartsCorrelationPlot* plot_  { nullptr }; //!< parent plot
  int                            row_   { -1 };      //!< row
  int                            col_   { -1 };      //!< column
  double                         value_ { 0.0 };     //!< value
};

//---

/*!
 * \brief Correlation Plot
 * \ingroup Charts
 */
class CQChartsCorrelationPlot : public CQChartsPlot,
 public CQChartsObjCellShapeData    <CQChartsCorrelationPlot>,
 public CQChartsObjCellLabelTextData<CQChartsCorrelationPlot>,
 public CQChartsObjXLabelTextData   <CQChartsCorrelationPlot>,
 public CQChartsObjYLabelTextData   <CQChartsCorrelationPlot> {
  Q_OBJECT

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Cell,cell)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(CellLabel,cellLabel)

  // x/y labels
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(XLabel,xLabel)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(YLabel,yLabel)

 public:
  CQChartsCorrelationPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsCorrelationPlot();

  //---

  CQChartsFilterModel *correlationModel() const { return correlationModel_; }

  //---

  // dimension
  int numColumns() const { return nc_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

  //---

  CQChartsGeom::BBox annotationBBox() const override;

 private:
  void addCellObj(int row, int col, double x, double y, double dx, double dy, double value,
                  const QModelIndex &ind, PlotObjs &objs) const;

  void drawXLabels(CQChartsPaintDevice *device) const;
  void drawYLabels(CQChartsPaintDevice *device) const;

 private:
  CQChartsFilterModel* correlationModel_ { nullptr }; //!< correlation mode
  int                  nc_               { 0 };       //!< number of grid columns
};

#endif
