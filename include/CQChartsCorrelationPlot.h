#ifndef CQChartsCorrelationPlot_H
#define CQChartsCorrelationPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

class CQChartsFilterModel;
class CQChartsCorrelationModel;

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

  bool canProbe() const override { return true; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsCorrelationPlot;

/*!
 * \brief Correlation Plot cell object
 * \ingroup Charts
 */
class CQChartsCorrelationCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsCorrelationCellObj(const CQChartsCorrelationPlot *plot, const CQChartsGeom::BBox &rect,
                             int row, int col, double value, const QModelIndex &ind);

  //---

  QString typeName() const override { return "correlation"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  CQChartsGeom::Size calcTextSize() const;

  void draw(CQChartsPaintDevice *device) override;

  void drawCellLabel(CQChartsPaintDevice *device, const QString &str);
  void drawCellLabel(CQChartsPaintDevice *device, const QString &str,
                     const CQChartsGeom::BBox &rect, double fontInc=0.0);

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void valueColorInd(ColorInd &ic) const;

  //---

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

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

  Q_PROPERTY(bool cellLabels READ isCellLabels WRITE setCellLabels)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Cell,cell)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(CellLabel,cellLabel)

  // x/y labels
  Q_PROPERTY(bool xLabels READ isXLabels WRITE setXLabels)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(XLabel,xLabel)

  Q_PROPERTY(bool yLabels READ isYLabels WRITE setYLabels)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(YLabel,yLabel)

  // cell types
  Q_PROPERTY(DiagonalType    diagonalType      READ diagonalType      WRITE setDiagonalType     )
  Q_PROPERTY(OffDiagonalType upperDiagonalType READ upperDiagonalType WRITE setUpperDiagonalType)
  Q_PROPERTY(OffDiagonalType lowerDiagonalType READ lowerDiagonalType WRITE setLowerDiagonalType)

  Q_ENUMS(DiagonalType)
  Q_ENUMS(OffDiagonalType)

 public:
  enum class DiagonalType {
    NONE,
    NAME,
    MIN_MAX,
    DENSITY
  };

  enum class OffDiagonalType {
    NONE,
    PIE,
    SHADE,
    ELLIPSE,
    POINTS,
    CONFIDENCE
  };

 public:
  CQChartsCorrelationPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsCorrelationPlot();

  //---

  CQChartsFilterModel *correlationModel() const { return correlationModel_; }

  CQChartsCorrelationModel *baseModel() const { return baseModel_; }

  //---

  // dimension
  int numColumns() const { return nc_; }

  double labelScale() const { return labelScale_; }

  //---

  // cell labels
  bool isCellLabels() const { return cellLabels_; }

  // x labels
  bool isXLabels() const { return xLabels_; }

  // y labels
  bool isYLabels() const { return yLabels_; }

  //---

  // cell types
  const DiagonalType &diagonalType() const { return diagonalType_; }
  void setDiagonalType(const DiagonalType &t);

  const OffDiagonalType &lowerDiagonalType() const { return lowerDiagonalType_; }
  void setLowerDiagonalType(const OffDiagonalType &t);

  const OffDiagonalType &upperDiagonalType() const { return upperDiagonalType_; }
  void setUpperDiagonalType(const OffDiagonalType &t);

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  void preDrawObjs(CQChartsPaintDevice *device) const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

  //---

  CQChartsGeom::BBox calcAnnotationBBox() const override;

 public slots:
  void setCellLabels(bool b);

  void setXLabels(bool b);
  void setYLabels(bool b);

 private slots:
  void diagonalTypeSlot(bool);
  void upperDiagonalTypeSlot(bool);
  void lowerDiagonalTypeSlot(bool);

 private:
  void addCellObj(int row, int col, double x, double y, double dx, double dy, double value,
                  const QModelIndex &ind, PlotObjs &objs) const;

  void drawXLabels(CQChartsPaintDevice *device) const;
  void drawYLabels(CQChartsPaintDevice *device) const;

 private:
  CQChartsFilterModel*      correlationModel_  { nullptr }; //!< correlation model
  ModelP                    correlationModelP_;             //!< correlation model shared pointer
  CQChartsCorrelationModel* baseModel_         { nullptr }; //!< base correlation model
  bool                      cellLabels_        { true };    //!< cell labels
  bool                      xLabels_           { true };    //!< x labels
  bool                      yLabels_           { true };    //!< y labels
  int                       nc_                { 0 };       //!< number of grid columns
  mutable double            labelScale_        { 1.0 };     //!< label scale
  DiagonalType              diagonalType_      { DiagonalType::NAME };
  OffDiagonalType           upperDiagonalType_ { OffDiagonalType::PIE };
  OffDiagonalType           lowerDiagonalType_ { OffDiagonalType::SHADE };
};

#endif
