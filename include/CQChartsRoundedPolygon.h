#ifndef CQChartsRoundedPolygon_H
#define CQChartsRoundedPolygon_H

#include <CQChartsSides.h>
#include <CQChartsGeom.h>

class CQChartsPaintDevice;

namespace CQChartsRoundedPolygon {

void draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
          double xsize=0.0, double ysize=0.0,
          const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));

void draw(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly,
          double xsize=0.0, double ysize=0.0);

}

#endif
