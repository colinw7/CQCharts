#ifndef CQRoundedPolygon_H
#define CQRoundedPolygon_H

#include <QPolygonF>

class QPainter;

namespace CQRoundedPolygon {

void draw(QPainter *painter, const QRectF &rect, double size=0.0);

void draw(QPainter *painter, const QPolygonF &poly, double size=0.0);

}

#endif
