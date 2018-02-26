#ifndef CQChartsRoundedPolygon_H
#define CQChartsRoundedPolygon_H

#include <QPolygonF>

class QPainter;

namespace CQChartsRoundedPolygon {

void draw(QPainter *painter, const QRectF    &rect, double xsize=0.0, double ysize=0.0);
void draw(QPainter *painter, const QPolygonF &poly, double xsize=0.0, double ysize=0.0);

}

#endif
