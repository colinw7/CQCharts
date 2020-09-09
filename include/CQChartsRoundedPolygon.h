#ifndef CQChartsRoundedPolygon_H
#define CQChartsRoundedPolygon_H

#include <CQChartsSides.h>
#include <CQChartsGeom.h>

class CQChartsPaintDevice;

namespace CQChartsRoundedPolygon {

using PaintDevice = CQChartsPaintDevice;
using Sides       = CQChartsSides;
using BBox        = CQChartsGeom::BBox;
using Polygon     = CQChartsGeom::Polygon;

void draw(PaintDevice *device, const BBox &bbox, double xsize=0.0, double ysize=0.0,
          const Sides &sides=Sides(Sides::Side::ALL));

void draw(PaintDevice *device, const Polygon &poly, double xsize=0.0, double ysize=0.0);

}

#endif
