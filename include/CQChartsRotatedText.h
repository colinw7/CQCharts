#ifndef CQChartsRotatedText_H
#define CQChartsRotatedText_H

#include <QPointF>
#include <QRectF>
#include <QFont>
#include <vector>

class CQChartsPaintDevice;

namespace CQChartsRotatedText {

typedef std::vector<QPointF> Points;

void draw(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
          double angle=0.0, Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
          bool alignBBox=false, bool contrast=false);

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
