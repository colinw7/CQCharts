#ifndef CQChartsRotatedText_H
#define CQChartsRotatedText_H

#include <CQChartsGeom.h>
#include <QFont>
#include <vector>

class  CQChartsPaintDevice;
struct CQChartsTextOptions;

namespace CQChartsRotatedText {

using PaintDevice = CQChartsPaintDevice;
using TextOptions = CQChartsTextOptions;
using Point       = CQChartsGeom::Point;
using Points      = std::vector<Point>;
using BBox        = CQChartsGeom::BBox;
using Margin      = CQChartsGeom::Margin;

void drawInBox(PaintDevice *device, const BBox &rect, const QString &text,
               const TextOptions &options, bool alignBBox=false, bool isRadial=false);

void draw(PaintDevice *device, const Point &p, const QString &text,
          const TextOptions &options, bool alignBBox=false, bool isRadial=false);

void drawDelta(PaintDevice *device, const Point &p, const QString &text,
               const TextOptions &options, double tx, double ty, double ax, double ay);

BBox calcBBox(double px, double py, const QString &text, const QFont &font,
              const TextOptions &options, double border=0.0, bool alignBBox=false,
              bool isRadial=false);

Points bboxPoints(double px, double py, const QString &text, const QFont &font,
                  const TextOptions &options, double border=0.0, bool alignBBox=false,
                  bool isRadial=false);

void calcBBoxData(double px, double py, const QString &text, const QFont &font,
                  const TextOptions &options, double border, BBox &bbox, Points &points,
                  bool alignBBox=false, bool isRadial=false);
void calcBBoxData(double px, double py, const QString &text, const QFont &font,
                  const TextOptions &options, const Margin &border, BBox &bbox, Points &points,
                  bool alignBBox=false, bool isRadial=false);

}

#endif
