#ifndef CQChartsDensity_H
#define CQChartsDensity_H

#include <CQChartsGeom.h>
#include <CQChartsData.h>
#include <QPolygonF>
#include <QPointF>
#include <vector>

class CQChartsPlot;

struct CQChartsWhiskerOpts {
  bool violin  { false };
  bool fitTail { false };
};

/*!
 * \brief Density plot data
 * \ingroup Charts
 */
class CQChartsDensity {
 public:
  using XVals  = std::vector<double>;
  using Points = std::vector<QPointF>;

 public:
  CQChartsDensity() { }

  void setXVals(const XVals &xvals) {
    xvals_       = xvals;
    initialized_ = false;
  }

  const Points &opoints() const { return opoints_; }

  int numSamples() const { return numSamples_; }
  void setNumSamples(int i) { numSamples_ = i; }

  double xmin() const { return xmin_; }
  double xmax() const { return xmax_; }

  double ymin() const { return ymin_; }
  double ymax() const { return ymax_; }

  double xmin1() const { return xmin1_; }
  double xmax1() const { return xmax1_; }

  double ymin1() const { return ymin1_; }
  double ymax1() const { return ymax1_; }

  void calc();

  double yval(double x) const;

  //---

  void drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                   const CQChartsGeom::BBox &rect, const Qt::Orientation &orientation,
                   const CQChartsWhiskerOpts &opts=CQChartsWhiskerOpts()) const;

  void calcWhiskerPoly(QPolygonF &ppoly, const CQChartsPlot *plot, const CQChartsGeom::BBox &rect,
                       const Qt::Orientation &orientation,
                       const CQChartsWhiskerOpts &opts=CQChartsWhiskerOpts()) const;

  static void drawCrossBar(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                           const CQChartsGeom::BBox &rect, double mean,
                           const Qt::Orientation &orientation, const CQChartsLength &cornerSize);

  static void drawPointRange(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                             const CQChartsGeom::BBox &rect, double mean,
                             const Qt::Orientation &orientation, const CQChartsSymbolData &symbol,
                             const QPen &pen, const QBrush &brush);

  static void drawErrorBar(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                           const CQChartsGeom::BBox &rect, const Qt::Orientation &orientation);

  static void drawLineRange(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                            const CQChartsGeom::BBox &rect, const Qt::Orientation &orientation);

 private:
  void init();

  double eval(double x) const;

 public:
  XVals  xvals_;
  Points opoints_;
  double smoothParameter_  { -1.0 };
  int    numSamples_       { 100 };
  bool   initialized_      { false };
  int    nx_               { 0 };
  double xmin_             { 0.0 };
  double xmax_             { 0.0 };
  double ymin_             { 0.0 };
  double ymax_             { 0.0 };
  double avg_              { 0.0 };
  double sigma_            { 0.0 };
  double defaultBandwidth_ { 0.0 };
  double xmin1_            { 0.0 };
  double xmax1_            { 0.0 };
  double ymin1_            { 0.0 };
  double ymax1_            { 0.0 };
  double area_             { 1.0 };
};

#endif
