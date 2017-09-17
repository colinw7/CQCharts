#ifndef CQRotatedText_H
#define CQRotatedText_H

#include <QPainter>

namespace CQRotatedText {

void
drawRotatedText(QPainter *painter, double x, double y, const QString &text,
                double angle=0.0, Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom);

QRectF bbox(double x, double y, const QString &text, const QFont &font, double angle=0.0);

}

#endif
