#ifndef CQRotatedText_H
#define CQRotatedText_H

#include <QPointF>
#include <QRectF>
#include <QFont>
#include <vector>

class CQChartsRenderer;

namespace CQRotatedText {

typedef std::vector<QPointF> Points;

void drawRotatedText(CQChartsRenderer *renderer, double x, double y, const QString &text,
                     double angle=0.0, Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
                     bool alignBBox=false);

QRectF bbox(double x, double y, const QString &text, const QFont &font,
            double angle=0.0, double border=0.0,
            Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
            bool alignBBox=false);

Points bboxPoints(double x, double y, const QString &text, const QFont &font,
                  double angle=0.0, double border=0.0,
                  Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
                  bool alignBBox=false);

void bboxData(double x, double y, const QString &text, const QFont &font, double angle,
              double border, QRectF &bbox, Points &points,
              Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
              bool alignBBox=false);

}

#endif
