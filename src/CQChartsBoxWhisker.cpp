#include <CQChartsBoxWhisker.h>
#include <CQChartsPlot.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

namespace CQChartsBoxWhiskerUtil {

void
drawWhisker(const CQChartsPlot *plot, QPainter *painter, const CQChartsBoxWhisker &whisker,
            const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
            const Qt::Orientation &orientation)
{
  drawWhisker(plot, painter, whisker.data(), bbox, width, orientation);
}

void
drawWhisker(const CQChartsPlot *plot, QPainter *painter, const CQChartsWhiskerData &data,
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

  CQChartsGeom::Point p1, p2, p3, p4, p5;

  if (orientation == Qt::Horizontal) {
    p1 = plot->windowToPixel(CQChartsGeom::Point(data.min   , pos - ww/2));
    p2 = plot->windowToPixel(CQChartsGeom::Point(data.lower , pos - ww/2));
    p3 = plot->windowToPixel(CQChartsGeom::Point(data.median, pos       ));
    p4 = plot->windowToPixel(CQChartsGeom::Point(data.upper , pos + ww/2));
    p5 = plot->windowToPixel(CQChartsGeom::Point(data.max   , pos + ww/2));
  }
  else {
    p1 = plot->windowToPixel(CQChartsGeom::Point(pos - ww/2, data.min   ));
    p2 = plot->windowToPixel(CQChartsGeom::Point(pos - ww/2, data.lower ));
    p3 = plot->windowToPixel(CQChartsGeom::Point(pos       , data.median));
    p4 = plot->windowToPixel(CQChartsGeom::Point(pos + ww/2, data.upper ));
    p5 = plot->windowToPixel(CQChartsGeom::Point(pos + ww/2, data.max   ));
  }

  double cxs = 0.0, cys = 0.0;

  QRectF rect(p2.x, p2.y, p4.x - p2.x, p4.y - p2.y);

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);

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
drawWhiskerBar(const CQChartsPlot *plot, QPainter *painter, const CQChartsWhiskerData &data,
               double pos, const Qt::Orientation &orientation,
               double ww, double bw, const CQChartsLength &cornerSize, bool notched)
{
  CQChartsGeom::Point p1, p2, p3, p4, p5, p6, p7;

  if (orientation != Qt::Horizontal) {
    p1 = plot->windowToPixel(CQChartsGeom::Point(pos - ww/2, data.min   ));
    p2 = plot->windowToPixel(CQChartsGeom::Point(pos - bw/2, data.lower ));
    p3 = plot->windowToPixel(CQChartsGeom::Point(pos       , data.median));
    p4 = plot->windowToPixel(CQChartsGeom::Point(pos + bw/2, data.upper ));
    p5 = plot->windowToPixel(CQChartsGeom::Point(pos + ww/2, data.max   ));
    p6 = plot->windowToPixel(CQChartsGeom::Point(pos - ww/4, data.lnotch));
    p7 = plot->windowToPixel(CQChartsGeom::Point(pos + ww/4, data.unotch));
  }
  else {
    p1 = plot->windowToPixel(CQChartsGeom::Point(data.min   , pos - ww/2));
    p2 = plot->windowToPixel(CQChartsGeom::Point(data.lower , pos - bw/2));
    p3 = plot->windowToPixel(CQChartsGeom::Point(data.median, pos       ));
    p4 = plot->windowToPixel(CQChartsGeom::Point(data.upper , pos + bw/2));
    p5 = plot->windowToPixel(CQChartsGeom::Point(data.max   , pos + ww/2));
    p6 = plot->windowToPixel(CQChartsGeom::Point(data.lnotch, pos - ww/4));
    p7 = plot->windowToPixel(CQChartsGeom::Point(data.unotch, pos + ww/4));
  }

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

    CQChartsRoundedPolygon::draw(painter, prect, cxs, cys);
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
