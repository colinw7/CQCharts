#ifndef CQChartsRotatedText_H
#define CQChartsRotatedText_H

#include <CQChartsGeom.h>
#include <QFont>
#include <vector>

class  CQChartsPaintDevice;
struct CQChartsTextOptions;

namespace CQChartsRotatedText {

using Points = std::vector<CQChartsGeom::Point>;

void drawInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect, const QString &text,
               const CQChartsTextOptions &options, bool alignBBox=false, bool isRadial=false);

void draw(CQChartsPaintDevice *device, const CQChartsGeom::Point &p, const QString &text,
          const CQChartsTextOptions &options, bool alignBBox=false, bool isRadial=false);

void drawDelta(CQChartsPaintDevice *device, const CQChartsGeom::Point &p, const QString &text,
               const CQChartsTextOptions &options, double tx, double ty, double ax, double ay);

CQChartsGeom::BBox calcBBox(double px, double py, const QString &text, const QFont &font,
                            const CQChartsTextOptions &options, double border=0.0,
                            bool alignBBox=false, bool isRadial=false);

Points bboxPoints(double px, double py, const QString &text,
                  const QFont &font, const CQChartsTextOptions &options, double border=0.0,
                  bool alignBBox=false, bool isRadial=false);

void calcBBoxData(double px, double py, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, double border,
                  CQChartsGeom::BBox &bbox, Points &points, bool alignBBox=false,
                  bool isRadial=false);
void calcBBoxData(double px, double py, const QString &text, const QFont &font,
                  const CQChartsTextOptions &options, const CQChartsGeom::Margin &border,
                  CQChartsGeom::BBox &bbox, Points &points, bool alignBBox=false,
                  bool isRadial=false);

}

#endif
