#ifndef CQChartsRotatedText_H
#define CQChartsRotatedText_H

#include <CQChartsGeom.h>
#include <QPointF>
#include <QRectF>
#include <QFont>
#include <vector>

class  CQChartsPaintDevice;
struct CQChartsTextOptions;

namespace CQChartsRotatedText {

using Points = std::vector<QPointF>;

void drawInBox(CQChartsPaintDevice *device, const QRectF &rect, const QString &text,
               const CQChartsTextOptions &options, bool alignBBox=false);

void draw(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
          const CQChartsTextOptions &options, bool alignBBox=false);

void drawDelta(CQChartsPaintDevice *device, const QPointF &p, const QString &text,
               const CQChartsTextOptions &options, double tx, double ty, double ax, double ay);

QRectF calcBBox(double x, double y, const QString &text, const QFont &font,
                const CQChartsTextOptions &options, double border=0.0,
                bool alignBBox=false);

Points bboxPoints(double x, double y, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, double border=0.0,
                  bool alignBBox=false);

void calcBBoxData(double x, double y, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, double border,
                  QRectF &bbox, Points &points, bool alignBBox=false);
void calcBBoxData(double x, double y, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, const CQChartsGeom::Margin &border,
                  QRectF &bbox, Points &points, bool alignBBox=false);

}

#endif
