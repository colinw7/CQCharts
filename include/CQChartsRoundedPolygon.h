#ifndef CQChartsRoundedPolygon_H
#define CQChartsRoundedPolygon_H

#include <CQChartsSides.h>
#include <QPolygonF>

class CQChartsPaintDevice;

namespace CQChartsRoundedPolygon {

void draw(CQChartsPaintDevice *device, const QRectF &rect, double xsize=0.0, double ysize=0.0,
          const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));

void draw(CQChartsPaintDevice *device, const QPolygonF &poly, double xsize=0.0, double ysize=0.0);

}

#endif
