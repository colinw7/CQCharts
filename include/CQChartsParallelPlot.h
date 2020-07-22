#ifndef CQChartsParallelPlot_H
#define CQChartsParallelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Parallel plot type
 * \ingroup Charts
 */
class CQChartsParallelPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsParallelPlotType();

  QString name() const override { return "parallel"; }
  QString desc() const override { return "Parallel"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsParallelPlot;

/*!
 * \brief Parallel Plot Line object
 * \ingroup Charts
 */
class CQChartsParallelLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsParallelPlot;

 public:
  CQChartsParallelLineObj(const Plot *plot, const BBox &rect, const Polygon &poly,
                          const QModelIndex &ind, const ColorInd &is);

  //---

  QString typeName() const override { return "line"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return polyLine_; }

  bool isSolid() const override { return false; }

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  //bool interpY(double x, std::vector<double> &yvals) const;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

 private:
  void getPolyLine(Polygon &poly) const;

 private:
  const Plot* plot_ { nullptr }; //!< plot
  Polygon     poly_;             //!< polygon
  Polygon     polyLine_;         //!< polyline
};

//---

/*!
 * \brief Parallel Plot Point object
 * \ingroup Charts
 */
class CQChartsParallelPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsParallelPlot;
  using Length = CQChartsLength;
  using Symbol = CQChartsSymbol;

 public:
  CQChartsParallelPointObj(const Plot *plot, const BBox &rect, double yval, double x, double y,
                           const QModelIndex &ind, const ColorInd &is, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const Plot* plot_  { nullptr };
  double      yval_  { 0.0 };
  double      x_     { 0.0 };
  double      y_     { 0.0 };
};

//---

/*!
 * \brief Parallel Plot
 * \ingroup Charts
 */
class CQChartsParallelPlot : public CQChartsPlot,
 public CQChartsObjLineData <CQChartsParallelPlot>,
 public CQChartsObjPointData<CQChartsParallelPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn  READ xColumn  WRITE setXColumn )
  Q_PROPERTY(CQChartsColumns yColumns READ yColumns WRITE setYColumns)

  // options
  Q_PROPERTY(bool horizontal READ isHorizontal WRITE setHorizontal)

  // lines (display, stroke)
  CQCHARTS_LINE_DATA_PROPERTIES

  Q_PROPERTY(bool linesSelectable READ isLinesSelectable WRITE setLinesSelectable)

  // points (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  CQChartsParallelPlot(View *view, const ModelP &model);

 ~CQChartsParallelPlot();

  //---

  // columns
  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &c);

  //---

  // options
  bool isHorizontal() const { return horizontal_; }

  //---

  // lines
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  //---

  const Range &setRange(int i) const { return setRanges_[i]; }

  Axis *axis(int i) { return axes_[i]; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool rowColValue(int row, const Column &column, const QModelIndex &parent,
                   double &value, double defVal) const;

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu) override;

  //---

  BBox axesFitBBox() const override;

  BBox calcAnnotationBBox() const override;

  bool hasFgAxes() const override;

  void drawFgAxes(CQChartsPaintDevice *device) const override;

  void postDraw() override;

  void setObjRange       (CQChartsPaintDevice *device);
  void setNormalizedRange(CQChartsPaintDevice *device);

 protected:
  virtual CQChartsParallelLineObj *createLineObj(const BBox &rect, const Polygon &poly,
                                                 const QModelIndex &ind, const ColorInd &is) const;

  virtual CQChartsParallelPointObj *createPointObj(const BBox &rect, double yval,
                                                   double x, double y, const QModelIndex &ind,
                                                   const ColorInd &is, const ColorInd &iv) const;

 public slots:
  // set horizontal
  void setHorizontal(bool b);

 private:
  enum class RangeType {
    NONE,
    OBJ,
    NORMALIZED
  };

  using Ranges = std::vector<Range>;
  using YAxes  = std::vector<CQChartsAxis*>;

  Column             xColumn_;                             //!< x value column
  Columns            yColumns_;                            //!< y value columns
  bool               horizontal_      { false };           //!< horizontal bars
  bool               linesSelectable_ { false };           //!< are lines selectable
  Ranges             setRanges_;                           //!< value set ranges
  Qt::Orientation    adir_            { Qt::Horizontal };  //!< axis direction
  Axis*              masterAxis_      { nullptr };         //!< master axis
  YAxes              axes_;                                //!< value axes
  mutable std::mutex axesMutex_;                           //!< value axes
  Range              normalizedDataRange_;                 //!< normalized data range
  double             max_tw_          { 0.0 };             //!< max text width
  BBox               axesBBox_;                            //!< axes bbox
  RangeType          rangeType_       { RangeType::NONE }; //!< current range type
};

#endif
