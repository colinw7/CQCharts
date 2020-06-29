#ifndef CQChartsRotatedText_H
#define CQChartsRotatedText_H

#include <CQChartsGeom.h>
#include <QFont>
#include <vector>

class  CQChartsPaintDevice;
struct CQChartsTextOptions;

namespace CQChartsRotatedText {

using Point  = CQChartsGeom::Point;
using Points = std::vector<Point>;
using BBox   = CQChartsGeom::BBox;
using Margin = CQChartsGeom::Margin;

void drawInBox(CQChartsPaintDevice *device, const BBox &rect, const QString &text,
               const CQChartsTextOptions &options, bool alignBBox=false, bool isRadial=false);

void draw(CQChartsPaintDevice *device, const Point &p, const QString &text,
          const CQChartsTextOptions &options, bool alignBBox=false, bool isRadial=false);

void drawDelta(CQChartsPaintDevice *device, const Point &p, const QString &text,
               const CQChartsTextOptions &options, double tx, double ty, double ax, double ay);

BBox calcBBox(double px, double py, const QString &text, const QFont &font,
              const CQChartsTextOptions &options, double border=0.0,
              bool alignBBox=false, bool isRadial=false);

Points bboxPoints(double px, double py, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, double border=0.0,
                  bool alignBBox=false, bool isRadial=false);

void calcBBoxData(double px, double py, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, double border,
                  BBox &bbox, Points &points, bool alignBBox=false,
                  bool isRadial=false);
void calcBBoxData(double px, double py, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, const Margin &border,
                  BBox &bbox, Points &points, bool alignBBox=false,
                  bool isRadial=false);

}

#endif
