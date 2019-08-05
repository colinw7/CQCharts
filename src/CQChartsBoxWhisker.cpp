#include <CQChartsBoxWhisker.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <QPainter>

namespace CQChartsBoxWhiskerUtil {

void
drawWhisker(const CQChartsPlot *plot, QPainter *painter, const CQChartsBoxWhisker &whisker,
            const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
            const Qt::Orientation &orientation)
{
  drawWhisker(plot, painter, whisker.statData(), bbox, width, orientation);
}

void
drawWhisker(const CQChartsPlot *plot, QPainter *painter, const CQChartsStatData &statData,
            const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
            const Qt::Orientation &orientation)
{
  double ww;

  if (orientation == Qt::Horizontal)
    ww = plot->lengthPlotHeight(width);
  else
    ww = plot->lengthPlotWidth(width);

  double pos;

  if (orientation == Qt::Horizontal)
    pos = bbox.getYMid();
  else
    pos = bbox.getXMid();

  //---

  auto windowToPixel = [&](double pos, double value) {
    if (orientation != Qt::Horizontal)
      return plot->windowToPixel(CQChartsGeom::Point(pos, value));
    else
      return plot->windowToPixel(CQChartsGeom::Point(value, pos));
  };

  CQChartsGeom::Point p1 = windowToPixel(statData.min        , pos - ww/2);
  CQChartsGeom::Point p2 = windowToPixel(statData.lowerMedian, pos - ww/2);
  CQChartsGeom::Point p3 = windowToPixel(statData.median     , pos       );
  CQChartsGeom::Point p4 = windowToPixel(statData.upperMedian, pos + ww/2);
  CQChartsGeom::Point p5 = windowToPixel(statData.max        , pos + ww/2);

  //---

  double cxs = 0.0, cys = 0.0;

  QRectF rect(p2.x, p2.y, p4.x - p2.x, p4.y - p2.y);

  CQChartsDrawUtil::drawRoundedPolygon(painter, rect, cxs, cys);

  if (orientation == Qt::Horizontal) {
    painter->drawLine(p1.x, p1.y, p1.x, p5.y);
    painter->drawLine(p5.x, p1.y, p5.x, p5.y);

    painter->drawLine(p1.x, p3.y, p2.x, p3.y);
    painter->drawLine(p4.x, p3.y, p5.x, p3.y);
  }
  else {
    painter->drawLine(p1.x, p1.y, p5.x, p1.y);
    painter->drawLine(p1.x, p5.y, p5.x, p5.y);

    painter->drawLine(p3.x, p1.y, p3.x, p2.y);
    painter->drawLine(p3.x, p4.y, p3.x, p5.y);
  }
}

void
drawWhiskerBar(const CQChartsPlot *plot, QPainter *painter, const CQChartsStatData &statData,
               double pos, const Qt::Orientation &orientation,
               double ww, double bw, const CQChartsLength &cornerSize, bool notched)
{
  auto windowToPixel = [&](double pos, double value) {
    if (orientation != Qt::Horizontal)
      return plot->windowToPixel(CQChartsGeom::Point(pos, value));
    else
      return plot->windowToPixel(CQChartsGeom::Point(value, pos));
  };

  CQChartsGeom::Point p1 = windowToPixel(pos - ww/2, statData.min        );
  CQChartsGeom::Point p2 = windowToPixel(pos - bw/2, statData.lowerMedian);
  CQChartsGeom::Point p3 = windowToPixel(pos       , statData.median     );
  CQChartsGeom::Point p4 = windowToPixel(pos + bw/2, statData.upperMedian);
  CQChartsGeom::Point p5 = windowToPixel(pos + ww/2, statData.max        );

  double lnotch = std::max(statData.lnotch, statData.lowerMedian);
  double unotch = std::min(statData.unotch, statData.upperMedian);

  CQChartsGeom::Point p6 = windowToPixel(pos - ww/4, lnotch);
  CQChartsGeom::Point p7 = windowToPixel(pos + ww/4, unotch);

  //---

  if (notched) {
    QPolygonF poly;

    poly << QPointF(p2.x, p2.y);
    poly << QPointF(p4.x, p2.y);
    poly << QPointF(p4.x, p6.y);
    poly << QPointF(p7.x, p3.y);
    poly << QPointF(p4.x, p7.y);
    poly << QPointF(p4.x, p4.y);
    poly << QPointF(p2.x, p4.y);
    poly << QPointF(p2.x, p7.y);
    poly << QPointF(p6.x, p3.y);
    poly << QPointF(p2.x, p6.y);
    poly << QPointF(p2.x, p2.y);

    painter->drawPolygon(poly);
  }
  else {
    QRectF prect(p2.x, p2.y, p4.x - p2.x, p4.y - p2.y);

    double cxs = plot->lengthPixelWidth (cornerSize);
    double cys = plot->lengthPixelHeight(cornerSize);

    CQChartsDrawUtil::drawRoundedPolygon(painter, prect, cxs, cys);
  }

  //---

  // draw extent line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(p3.x, p1.y), QPointF(p3.x, p5.y));
  else
    painter->drawLine(QPointF(p1.x, p3.y), QPointF(p5.x, p3.y));

  //---

  // draw lower/upper horizontal lines
  if (orientation != Qt::Horizontal) {
    painter->drawLine(QPointF(p1.x, p1.y), QPointF(p5.x, p1.y));
    painter->drawLine(QPointF(p1.x, p5.y), QPointF(p5.x, p5.y));
  }
  else {
    painter->drawLine(QPointF(p1.x, p1.y), QPointF(p1.x, p5.y));
    painter->drawLine(QPointF(p5.x, p1.y), QPointF(p5.x, p5.y));
  }

  //---

  // draw median line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(p2.x, p3.y), QPointF(p4.x, p3.y));
  else
    painter->drawLine(QPointF(p3.x, p2.y), QPointF(p3.x, p4.y));
}

void
drawOutliers(const CQChartsPlot *plot, QPainter *painter, const std::vector<double> &ovalues,
             double pos, const CQChartsSymbolData &symbol, const QPen &pen,
             const QBrush &brush, const Qt::Orientation &orientation)
{
  double sx, sy;

  plot->pixelSymbolSize(symbol.size(), sx, sy);

  for (auto &o : ovalues) {
    CQChartsGeom::Point p1;

    if (orientation != Qt::Horizontal)
      p1 = plot->windowToPixel(CQChartsGeom::Point(pos, o));
    else
      p1 = plot->windowToPixel(CQChartsGeom::Point(o, pos));

    plot->drawSymbol(painter, QPointF(p1.x, p1.y), symbol.type(),
                     CMathUtil::avg(sx, sy), pen, brush);
  }
}

}
