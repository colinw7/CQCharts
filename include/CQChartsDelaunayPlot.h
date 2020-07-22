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

  Plot *create(View *view, const ModelP &model) const override;
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
  using Plot   = CQChartsDelaunayPlot;
  using Length = CQChartsLength;
  using Symbol = CQChartsSymbol;

 public:
  CQChartsDelaunayPointObj(const Plot *plot, const BBox &rect, double x, double y, double value,
                           const QModelIndex &ind, const ColorInd &iv);

  const Plot *plot() const { return plot_; }

  double x() const { return x_; }
  double y() const { return y_; }

  double value() const { return value_; }

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const Plot* plot_  { nullptr };
  double      x_     { 0.0 };
  double      y_     { 0.0 };
  double      value_ { 0.0 };
};

//---

CQCHARTS_NAMED_LINE_DATA (Delaunay,delaunay)
CQCHARTS_NAMED_LINE_DATA (Voronoi,voronoi)
CQCHARTS_NAMED_POINT_DATA(Voronoi,voronoi)
CQCHARTS_NAMED_SHAPE_DATA(Voronoi,voronoi)

/*!
 * \brief Delaunay Plot
 * \ingroup Charts
 */
class CQChartsDelaunayPlot : public CQChartsPlot,
 public CQChartsObjDelaunayLineData<CQChartsDelaunayPlot>,
 public CQChartsObjVoronoiLineData <CQChartsDelaunayPlot>,
 public CQChartsObjVoronoiPointData<CQChartsDelaunayPlot>,
 public CQChartsObjVoronoiShapeData<CQChartsDelaunayPlot>,
 public CQChartsObjPointData       <CQChartsDelaunayPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn     READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(CQChartsColumn yColumn     READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  // delaunay, voronoi
  Q_PROPERTY(bool delaunay       READ isDelaunay       WRITE setDelaunay      )
  Q_PROPERTY(bool voronoi        READ isVoronoi        WRITE setVoronoi       )
  Q_PROPERTY(bool voronoiCircles READ isVoronoiCircles WRITE setVoronoiCircles)
  Q_PROPERTY(bool voronoiPolygon READ isVoronoiPolygon WRITE setVoronoiPolygon)

  // delaunay lines
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES (Delaunay, delaunay)

  // voronoi lines, points, shapes
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES (Voronoi, voronoi)
  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Voronoi, voronoi)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Voronoi, voronoi)

  // points (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  CQChartsDelaunayPlot(View *view, const ModelP &model);

 ~CQChartsDelaunayPlot();

  //---

  // columns
  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &yColumn() const { return yColumn_; }
  void setYColumn(const Column &c);

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  // voronoi
  bool isDelaunay() const { return delaunay_; }

  bool isVoronoi() const { return voronoi_; }

  bool isVoronoiCircles() const { return voronoiCircles_; }
  void setVoronoiCircles(bool b);

  bool isVoronoiPolygon() const { return voronoiPolygon_; }
  void setVoronoiPolygon(bool b);

  //---

  const QString &yname() const { return yname_; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  void addPointObj(double x, double y, double value, const QModelIndex &xind,
                   int r, int nr, PlotObjs &objs) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  bool hasBackground() const override;
  bool hasForeground() const override;

  void execDrawBackground(CQChartsPaintDevice *device) const override;
  void execDrawForeground(CQChartsPaintDevice *device) const override;

 public slots:
  void setDelaunay(bool b);
  void setVoronoi(bool b);

 protected:
  void drawDelaunay(CQChartsPaintDevice *device) const;
  void drawVoronoi (CQChartsPaintDevice *device) const;

  //---

  virtual CQChartsDelaunayPointObj *createPointObj(const BBox &rect, double x, double y,
                                                   double value, const QModelIndex &ind,
                                                   const ColorInd &iv) const;

 private:
  Column            xColumn_;                    //!< x column
  Column            yColumn_;                    //!< y column
  Column            nameColumn_;                 //!< name column
  Column            valueColumn_;                //!< value column
  bool              delaunay_       { false };   //!< is delaunay
  bool              voronoi_        { true };    //!< is voronoi
  bool              voronoiCircles_ { false };   //!< voronoi circle
  bool              voronoiPolygon_ { false };   //!< voronoi polygon
  RMinMax           valueRange_;                 //!< value range
  CQChartsDelaunay* delaunayData_   { nullptr }; //!< delaunay data
  QString           yname_;                      //!< y name
};

#endif
