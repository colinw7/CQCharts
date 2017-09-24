#ifndef CQRotatedText_H
#define CQRotatedText_H

#include <QPainter>

namespace CQRotatedText {

typedef std::vector<QPointF> Points;

void
drawRotatedText(QPainter *painter, double x, double y, const QString &text,
                double angle=0.0, Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom);

QRectF bbox(double x, double y, const QString &text, const QFont &font,
            double angle=0.0, double border=0.0);

Points bboxPoints(double x, double y, const QString &text, const QFont &font,
                  double angle=0.0, double border=0.0);

void bboxData(double x, double y, const QString &text, const QFont &font, double angle,
              double border, QRectF &bbox, Points &points);

}

#endif
