#ifndef CQChartsDelaunayPlot_H
#define CQChartsDelaunayPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Delaunay plot type
 * \ingroup Charts
 */
class CQChartsDelaunayPlotType : public CQChartsPlotType {
 public:
  CQChartsDelaunayPlotType();

  QString name() const override { return "delaunay"; }
  QString desc() const override { return "Delaunay"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsDelaunayPlot;
class CQChartsDelaunay;

//---

/*!
 * \brief Delaunay Plot Point object
 * \ingroup Charts
 */
class CQChartsDelaunayPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsDelaunayPointObj(const CQChartsDelaunayPlot *plot, const CQChartsGeom::BBox &rect,
                           double x, double y, const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsDelaunayPlot* plot_ { nullptr };
  double                      x_    { 0.0 };
  double                      y_    { 0.0 };
};

//---

/*!
 * \brief Delaunay Plot
 * \ingroup Charts
 */
class CQChartsDelaunayPlot : public CQChartsPlot,
 public CQChartsObjLineData <CQChartsDelaunayPlot>,
 public CQChartsObjPointData<CQChartsDelaunayPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn    READ xColumn    WRITE setXColumn   )
  Q_PROPERTY(CQChartsColumn yColumn    READ yColumn    WRITE setYColumn   )
  Q_PROPERTY(CQChartsColumn nameColumn READ nameColumn WRITE setNameColumn)

  // voronoi
  Q_PROPERTY(bool   voronoi          READ isVoronoi        WRITE setVoronoi         )
  Q_PROPERTY(double voronoiPointSize READ voronoiPointSize WRITE setVoronoiPointSize)

  // lines (display, color, width, dash, ...)
  CQCHARTS_LINE_DATA_PROPERTIES

  // points (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  CQChartsDelaunayPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsDelaunayPlot();

  //---

  // columns
  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  //---

  // voronoi
  bool isVoronoi() const { return voronoi_; }

  double voronoiPointSize() const { return voronoiPointSize_; }
  void setVoronoiPointSize(double r);

  //---

  const QString &yname() const { return yname_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  void addPointObj(double x, double y, const QModelIndex &xind,
                   int r, int nr, PlotObjs &objs) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

 public slots:
  void setVoronoi(bool b);

 private:
  void drawDelaunay(CQChartsPaintDevice *device) const;
  void drawVoronoi (CQChartsPaintDevice *device) const;

 private:
  CQChartsColumn    xColumn_;                      //!< x column
  CQChartsColumn    yColumn_;                      //!< y column
  CQChartsColumn    nameColumn_;                   //!< name column
  bool              voronoi_          { true };    //!< is voronoi
  double            voronoiPointSize_ { 2 };       //!< voronoi point size
  CQChartsDelaunay* delaunay_         { nullptr }; //!< delaunay data
  QString           yname_;                        //!< y name
};

#endif
