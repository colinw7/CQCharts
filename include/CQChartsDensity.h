#ifndef CQChartsDensity_H
#define CQChartsDensity_H

#include <CQChartsGeom.h>
#include <CQChartsData.h>
#include <CQChartsDrawUtil.h>
#include <CQStatData.h>

#include <QObject>
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
class CQChartsDensity : public QObject {
  Q_OBJECT

  Q_PROPERTY(int             numSamples      READ numSamples      WRITE setNumSamples     )
  Q_PROPERTY(double          smoothParameter READ smoothParameter WRITE setSmoothParameter)
  Q_PROPERTY(DrawType        drawType        READ drawType        WRITE setDrawType       )
  Q_PROPERTY(Qt::Orientation orientation     READ orientation     WRITE setOrientation    )

  Q_ENUMS(DrawType)

 public:
  enum DrawType {
    WHISKER,
    WHISKER_BAR,
    DISTRIBUTION,
    CROSS_BAR,
    POINT_RANGE,
    ERROR_BAR
  };

  using XVals   = std::vector<double>;
  using Point   = CQChartsGeom::Point;
  using Points  = std::vector<Point>;
  using BBox    = CQChartsGeom::BBox;
  using Polygon = CQChartsGeom::Polygon;

 public:
  CQChartsDensity();

  //! get/set draw type
  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &v) { drawType_ = v; emit dataChanged(); }

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &v) { orientation_ = v; emit dataChanged(); }

  //! get/set x values
  const XVals &xvals() const { return xvals_; }
  void setXVals(const XVals &xvals) { xvals_ = xvals; invalidate(); }

  const Points &opoints() const { return opoints_; }

  //! get/set num samples
  int numSamples() const { return numSamples_; }
  void setNumSamples(int i) { numSamples_ = i; invalidate(); }

  //! get/set smooth parameter
  double smoothParameter() const { return smoothParameter_; }
  void setSmoothParameter(double r) { smoothParameter_ = r; invalidate(); }

  //---

  double xmin() const { constCalc(); return xmin_; }
  double xmax() const { constCalc(); return xmax_; }

  double ymin() const { constCalc(); return ymin_; }
  double ymax() const { constCalc(); return ymax_; }

  double xmin1() const { constCalc(); return xmin1_; }
  double xmax1() const { constCalc(); return xmax1_; }

  double ymin1() const { constCalc(); return ymin1_; }
  double ymax1() const { constCalc(); return ymax1_; }

  double area() const { constCalc(); return area_; }

  //---

  double yval(double x) const;

  //---

  void draw(const CQChartsPlot *plot, CQChartsPaintDevice *device, const BBox &rect);

  BBox bbox(const BBox &rect) const;

  //---

  void drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device, const BBox &rect,
                   const Qt::Orientation &orientation) const;
  void drawWhiskerBar(const CQChartsPlot *plot, CQChartsPaintDevice *device, const BBox &rect,
                      const Qt::Orientation &orientation) const;

  void drawDistribution(const CQChartsPlot *plot, CQChartsPaintDevice *device, const BBox &rect,
                        const Qt::Orientation &orientation,
                        const CQChartsWhiskerOpts &opts=CQChartsWhiskerOpts()) const;

  void calcDistributionPoly(Polygon &ppoly, const CQChartsPlot *plot, const BBox &rect,
                            const Qt::Orientation &orientation,
                            const CQChartsWhiskerOpts &opts=CQChartsWhiskerOpts()) const;

  void drawBuckets(const CQChartsPlot *plot, CQChartsPaintDevice *device, const BBox &rect,
                   const Qt::Orientation &orientation) const;

  //---

  static void drawCrossBar(const CQChartsPlot *plot, CQChartsPaintDevice *device, const BBox &rect,
                           double mean, const Qt::Orientation &orientation,
                           const CQChartsLength &cornerSize);

  static void drawPointRange(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                             const BBox &rect, double mean,
                             const Qt::Orientation &orientation, const CQChartsSymbolData &symbol);

  static void drawErrorBar(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                           const BBox &rect, double mean,
                           const Qt::Orientation &orientation, const CQChartsSymbolData &symbol);

  static void drawLineRange(const CQChartsPlot *plot, CQChartsPaintDevice *device,
                            const BBox &rect, const Qt::Orientation &orientation);

 private:
  void invalidate() {
    initialized_ = false;
    calced_      = false;

    emit dataChanged();
  }

  void constCalc() const;
  void calc();

  void constInit() const;
  void init();

  double eval(double x) const;

 signals:
  void dataChanged();

 public:
  DrawType        drawType_         { DrawType::WHISKER };
  Qt::Orientation orientation_      { Qt::Horizontal };
  XVals           xvals_;
  Points          opoints_;
  double          smoothParameter_  { -1.0 };
  int             numSamples_       { 100 };
  bool            initialized_      { false };
  bool            calced_           { false };
  int             nx_               { 0 };

  // init data
  XVals           sxvals_;
  CQStatData      statData_;
  double          xmin_             { 0.0 };
  double          xmax_             { 0.0 };
  double          ymin_             { 0.0 };
  double          ymax_             { 0.0 };
  double          avg_              { 0.0 };
  double          sigma_            { 0.0 };
  double          defaultBandwidth_ { 0.0 };

  // calc data
  double          xmin1_            { 0.0 };
  double          xmax1_            { 0.0 };
  double          ymin1_            { 0.0 };
  double          ymax1_            { 0.0 };
  double          area_             { 1.0 };
};

#endif
