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
  CQChartsParallelPlotType();

  QString name() const override { return "parallel"; }
  QString desc() const override { return "Parallel"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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
  CQChartsParallelLineObj(const CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                          const QPolygonF &poly, const QModelIndex &ind, const ColorInd &is);

  QString typeName() const override { return "line"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  //bool interpY(double x, std::vector<double> &yvals) const;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  void getPolyLine(QPolygonF &poly) const;

 private:
  const CQChartsParallelPlot* plot_ { nullptr };
  QPolygonF                   poly_;
};

//---

/*!
 * \brief Parallel Plot Point object
 * \ingroup Charts
 */
class CQChartsParallelPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsParallelPointObj(const CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                           double yval, double x, double y, const QModelIndex &ind,
                           const ColorInd &is, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsParallelPlot* plot_  { nullptr };
  double                      yval_  { 0.0 };
  double                      x_     { 0.0 };
  double                      y_     { 0.0 };
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
  CQChartsParallelPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsParallelPlot();

  //---

  // columns
  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumns &yColumns() const { return yColumns_; }
  void setYColumns(const CQChartsColumns &c);

  //---

  // options
  bool isHorizontal() const { return horizontal_; }

  //---

  // lines
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  //---

  const CQChartsGeom::Range &setRange(int i) const { return setRanges_[i]; }

  CQChartsAxis *axis(int i) { return axes_[i]; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool rowColValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
                   double &value, double defVal) const;

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu) override;

  //---

  CQChartsGeom::BBox axesFitBBox() const override;

  CQChartsGeom::BBox annotationBBox() const override;

  bool hasFgAxes() const override;

  void drawFgAxes(CQChartsPaintDevice *device) const override;

  void postDraw() override;

  void setObjRange       (CQChartsPaintDevice *device);
  void setNormalizedRange(CQChartsPaintDevice *device);

 public slots:
  // set horizontal
  void setHorizontal(bool b);

 private:
  enum class RangeType {
    NONE,
    OBJ,
    NORMALIZED
  };

  using Ranges = std::vector<CQChartsGeom::Range>;
  using YAxes  = std::vector<CQChartsAxis*>;

  CQChartsColumn      xColumn_;                             //!< x value column
  CQChartsColumns     yColumns_;                            //!< y value columns
  bool                horizontal_      { false };           //!< horizontal bars
  bool                linesSelectable_ { false };           //!< are lines selectable
  Ranges              setRanges_;                           //!< value set ranges
  Qt::Orientation     adir_            { Qt::Horizontal };  //!< axis direction
  YAxes               axes_;                                //!< value axes
  mutable std::mutex  axesMutex_;                           //!< value axes
  CQChartsGeom::Range normalizedDataRange_;                 //!< normalized data range
  double              max_tw_          { 0.0 };             //!< max text width
  CQChartsGeom::BBox  axesBBox_;                            //!< axes bbox
  RangeType           rangeType_       { RangeType::NONE }; //!< current range type
};

#endif
