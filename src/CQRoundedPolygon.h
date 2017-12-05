#ifndef CQRoundedPolygon_H
#define CQRoundedPolygon_H

#include <QPolygonF>

class CQChartsRenderer;

namespace CQRoundedPolygon {

void draw(CQChartsRenderer *painter, const QRectF &rect, double size=0.0);

void draw(CQChartsRenderer *painter, const QPolygonF &poly, double size=0.0);

}

#endif
