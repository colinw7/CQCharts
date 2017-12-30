#ifndef CQChartsRoundedPolygon_H
#define CQChartsRoundedPolygon_H

#include <QPolygonF>

class QPainter;

namespace CQChartsRoundedPolygon {

void draw(QPainter *painter, const QRectF &rect, double size=0.0);

void draw(QPainter *painter, const QPolygonF &poly, double size=0.0);

}

#endif
