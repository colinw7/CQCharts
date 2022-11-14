#ifndef CContour_H
#define CContour_H

#include <QObject>
#include <QColor>
#include <QPointer>

#include <vector>

class CQChartsPlot;
class CQChartsPaintDevice;

/*!
 * \brief Contour Data Object
 * \ingroup Charts
 */
class CQChartsContour : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool   solid            READ isSolid          WRITE setSolid)
  Q_PROPERTY(int    numContourLevels READ numContourLevels WRITE setNumContourLevels)
  Q_PROPERTY(QColor backgroundColor  READ backgroundColor  WRITE setBackgroundColor)
  Q_PROPERTY(QColor gridPointColor   READ gridPointColor   WRITE setGridPointColor)

 public:
  using Plot          = CQChartsPlot;
  using PlotP         = QPointer<Plot>;
  using PaintDevice   = CQChartsPaintDevice;
  using ContourLevels = std::vector<double>;

#if 0
  using ColorArray = std::vector<QColor>;
#endif

 public:
  CQChartsContour(Plot *plot=nullptr);

  //---

  Plot *plot() const;
  void setPlot(Plot *p);

  bool isSolid() const { return solid_; }
  void setSolid(bool b) { solid_ = b; }

  int numContourLevels() const { return numLevels_; }
  void setNumContourLevels(int i);

  const ContourLevels &contourLevels() const { return levels_; }
  void setContourLevels(const ContourLevels &levels);

  const QColor &backgroundColor() const { return backgroundColor_; }
  void setBackgroundColor(const QColor &v) { backgroundColor_ = v; }

  const QColor &gridPointColor() const { return gridPointColor_; }
  void setGridPointColor(const QColor &v) { gridPointColor_ = v; }

  //---

  void setData(double *x, double *y, double *z, int numX, int numY);

#if 0
  void setContourColors(const ColorArray &colors);
#endif

  void drawContour(PaintDevice *device);

 private:
  void drawContourLines(PaintDevice *device);
  void drawContourSolid(PaintDevice *device);

  QColor getLevelColor(int l) const;

  void initLevels(ContourLevels &levels) const;

  void fillContourBox(PaintDevice *, double, double, double, double,
                      double, double, double, double, const ContourLevels &);

  void drawPoint(PaintDevice *, double, double);
  void drawLine(PaintDevice *, double, double, double, double);

  void fillPolygon(PaintDevice *, const double *, const double *, int);

 private:
  using RealArray  = std::vector<double>;

  PlotP      plot_;
  bool       solid_           { false };
  RealArray  x_;
  RealArray  y_;
  RealArray  z_;
  RealArray  levels_;
  int        numLevels_       { 10 };
#if 0
  ColorArray colors_;
#endif
  QColor     backgroundColor_ { 255, 255, 255 };
  QColor     gridPointColor_  { 128, 128, 128 };
  double     xmin_            { 0.0 };
  double     ymin_            { 0.0 };
  double     zmin_            { 0.0 };
  double     xmax_            { 1.0 };
  double     ymax_            { 1.0 };
  double     zmax_            { 1.0 };
  double     minX_            { 0.0 };
  double     minY_            { 0.0 };
};

#endif
