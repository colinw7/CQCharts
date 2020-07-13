#ifndef CQChartsRoundedPolygon_H
#define CQChartsRoundedPolygon_H

#include <CQChartsSides.h>
#include <CQChartsGeom.h>

class CQChartsPaintDevice;

namespace CQChartsRoundedPolygon {

using BBox    = CQChartsGeom::BBox;
using Polygon = CQChartsGeom::Polygon;

void draw(CQChartsPaintDevice *device, const BBox &bbox, double xsize=0.0, double ysize=0.0,
          const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));

void draw(CQChartsPaintDevice *device, const Polygon &poly, double xsize=0.0, double ysize=0.0);

}

#endif
