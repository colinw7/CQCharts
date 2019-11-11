#ifndef CQChartsRotatedText_H
#define CQChartsRotatedText_H

#include <CQChartsGeom.h>
#include <QPointF>
#include <QRectF>
#include <QFont>
#include <vector>

class CQChartsPaintDevice;

namespace CQChartsRotatedText {

using Points = std::vector<QPointF>;

void draw(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
          double angle=0.0, Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
          bool alignBBox=false, bool contrast=false, double contrastAlpha=0.05);

QRectF bbox(double x, double y, const QString &text, const QFont &font,
            double angle=0.0, double border=0.0,
            Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
            bool alignBBox=false);

Points bboxPoints(double x, double y, const QString &text, const QFont &font,
                  double angle=0.0, double border=0.0,
                  Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
                  bool alignBBox=false);

void bboxData(double x, double y, const QString &text, const QFont &font, double angle,
              const CQChartsGeom::Margin &border, QRectF &bbox, Points &points,
              Qt::Alignment align=Qt::AlignLeft | Qt::AlignBottom,
              bool alignBBox=false);

}

#endif
