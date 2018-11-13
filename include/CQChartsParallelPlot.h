#ifndef CQChartsParallelPlot_H
#define CQChartsParallelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAxis.h>
#include <CQChartsData.h>

//---

class CQChartsParallelPlotType : public CQChartsPlotType {
 public:
  CQChartsParallelPlotType();

  QString name() const override { return "parallel"; }
  QString desc() const override { return "Parallel"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsParallelPlot;

class CQChartsParallelLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsParallelLineObj(CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                          const QPolygonF &poly, const QModelIndex &ind, int i, int n);

  QString calcId() const override;

  QString calcTipId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 private:
  void getPolyLine(QPolygonF &poly) const;

 private:
  CQChartsParallelPlot *plot_ { nullptr };
  QPolygonF             poly_;
  QModelIndex           ind_;
  int                   i_    { -1 };
  int                   n_    { -1 };
};

//---

class CQChartsParallelPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsParallelPointObj(CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                           double yval, double x, double y, const QModelIndex &ind,
                           int iset, int nset, int i, int n);

  QString calcId() const override;

  QString calcTipId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 private:
  CQChartsParallelPlot* plot_  { nullptr };
  double                yval_  { 0.0 };
  double                x_     { 0.0 };
  double                y_     { 0.0 };
  QModelIndex           ind_;
  int                   iset_  { -1 };
  int                   nset_  { -1 };
  int                   i_     { -1 };
  int                   n_     { -1 };
};

//---

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

  const CQChartsGeom::Range &setRange(int i) { return setRanges_[i]; }

  CQChartsAxis *axis(int i) { return axes_[i]; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() override;

  bool createObjs() override;

  //---

  bool rowColValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
                   double &value, double defVal);

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu) override;

  //---

  CQChartsGeom::BBox axesFitBBox() const override;

  CQChartsGeom::BBox annotationBBox() const override;

  bool hasFgAxes() const override;

  void drawFgAxes(QPainter *painter) override;

  void setObjRange();

  void setNormalizedRange();

 public slots:
  // set horizontal
  void setHorizontal(bool b);

 private:
  using Ranges  = std::vector<CQChartsGeom::Range>;
  using YAxes   = std::vector<CQChartsAxis*>;
  using AxisDir = CQChartsAxis::Direction;

  CQChartsColumn      xColumn_;                                 // x value column
  CQChartsColumns     yColumns_;                                // y value columns
  bool                horizontal_      { false };               // horizontal bars
  bool                linesSelectable_ { false };               // are lines selectable
  Ranges              setRanges_;                               // value set ranges
  AxisDir             adir_            { AxisDir::HORIZONTAL }; // axis direction
  YAxes               axes_;                                    // value axes
  CQChartsGeom::Range normalizedDataRange_;
  double              max_tw_          { 0.0 };
  CQChartsGeom::BBox  axesBBox_;
};

#endif
