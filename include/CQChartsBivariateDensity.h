#ifndef CQChartsBivariateDensity_H
#define CQChartsBivariateDensity_H

#include <CQChartsGeom.h>
#include <vector>

class CQChartsPlot;
class CQChartsPaintDevice;

class CQChartsBivariateDensity {
 public:
  using Values = std::vector<CQChartsGeom::Point>;

  struct Data {
    int                   gridSize;
    double                delta;
    Values                values;
    CQChartsGeom::RMinMax xrange;
    CQChartsGeom::RMinMax yrange;
  };

 public:
  CQChartsBivariateDensity() { }

  void draw(const CQChartsPlot *plot, CQChartsPaintDevice *device, const Data &data);
};

#endif
