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
  using Plot        = CQChartsPlot;
  using PaintDevice = CQChartsPaintDevice;
  using Point       = CQChartsGeom::Point;
  using Values      = std::vector<Point>;
  using BBox        = CQChartsGeom::BBox;
  using RMinMax     = CQChartsGeom::RMinMax;

  //! grid input data
  struct Data {
    int     gridSize { 16 };  // grid size
    double  delta    { 0.0 }; // if defined (> 0.0) then defined interval for alpha
    Values  values;           // scatter x,y values
    RMinMax xrange;           // x range
    RMinMax yrange;           // y range
  };

  struct Cell {
    BBox   bbox;
    double value { 0.0 };
    double alpha { 1.0 };
  };

 public:
  CQChartsBivariateDensity() { }

  void calc(const Plot *plot, const Data &data);

  void draw(const Plot *plot, PaintDevice *device);

 private:
  using Cells = std::vector<Cell>;

  Cells cells_;
};

#endif
