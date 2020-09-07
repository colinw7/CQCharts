#ifndef CQChartsBivariateDensity_H
#define CQChartsBivariateDensity_H

#include <CQChartsGeom.h>
#include <vector>

class CQChartsPlot;
class CQChartsPaintDevice;

/*!
 * \brief Bivariate (2D) density class
 * \ingroup Charts
 */
class CQChartsBivariateDensity {
 public:
  using Point   = CQChartsGeom::Point;
  using Values  = std::vector<Point>;
  using BBox    = CQChartsGeom::BBox;
  using RMinMax = CQChartsGeom::RMinMax;

  struct Data {
    int     gridSize;
    double  delta;
    Values  values;
    RMinMax xrange;
    RMinMax yrange;
  };

 public:
  CQChartsBivariateDensity() { }

  void draw(const CQChartsPlot *plot, CQChartsPaintDevice *device, const Data &data);
};

#endif
