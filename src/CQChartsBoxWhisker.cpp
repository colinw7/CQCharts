#include <CQChartsBoxWhisker.h>
#include <CQChartsPlot.h>
#include <CQChartsRoundedPolygon.h>
#include <QPainter>

namespace CQChartsBoxWhiskerUtil {

void
drawWhisker(CQChartsPlot *plot, QPainter *painter, const CQChartsBoxWhisker &whisker,
            const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
            const Qt::Orientation &orientation)
{
  CQChartsWhiskerData data;

  drawWhisker(plot, painter, whisker.data(), bbox, width, orientation);
}

void
drawWhisker(CQChartsPlot *plot, QPainter *painter, const CQChartsWhiskerData &data,
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

  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5;

  if (orientation == Qt::Horizontal) {
    plot->windowToPixel(data.min   , pos - ww/2, px1, py1);
    plot->windowToPixel(data.lower , pos - ww/2, px2, py2);
    plot->windowToPixel(data.median, pos       , px3, py3);
    plot->windowToPixel(data.upper , pos + ww/2, px4, py4);
    plot->windowToPixel(data.max   , pos + ww/2, px5, py5);
  }
  else {
    plot->windowToPixel(pos - ww/2, data.min   , px1, py1);
    plot->windowToPixel(pos - ww/2, data.lower , px2, py2);
    plot->windowToPixel(pos       , data.median, px3, py3);
    plot->windowToPixel(pos + ww/2, data.upper , px4, py4);
    plot->windowToPixel(pos + ww/2, data.max   , px5, py5);
  }

  double cxs = 0.0, cys = 0.0;

  QRectF rect(px2, py2, px4 - px2, py4 - py2);

  CQChartsRoundedPolygon::draw(painter, rect, cxs, cys);

  if (orientation == Qt::Horizontal) {
    painter->drawLine(px1, py1, px1, py5);
    painter->drawLine(px5, py1, px5, py5);

    painter->drawLine(px1, py3, px2, py3);
    painter->drawLine(px4, py3, px5, py3);
  }
  else {
    painter->drawLine(px1, py1, px5, py1);
    painter->drawLine(px1, py5, px5, py5);

    painter->drawLine(px3, py1, px3, py2);
    painter->drawLine(px3, py4, px3, py5);
  }
}

void
drawWhiskerBar(CQChartsPlot *plot, QPainter *painter, const CQChartsWhiskerData &data,
               double pos, const Qt::Orientation &orientation,
               double ww, double bw, const CQChartsLength &cornerSize, bool notched)
{
  double px1, py1, px2, py2, px3, py3, px4, py4, px5, py5, px6, py6, px7, py7;

  if (orientation != Qt::Horizontal) {
    plot->windowToPixel(pos - ww/2, data.min   , px1, py1);
    plot->windowToPixel(pos - bw/2, data.lower , px2, py2);
    plot->windowToPixel(pos       , data.median, px3, py3);
    plot->windowToPixel(pos + bw/2, data.upper , px4, py4);
    plot->windowToPixel(pos + ww/2, data.max   , px5, py5);
    plot->windowToPixel(pos - ww/4, data.lnotch, px6, py6);
    plot->windowToPixel(pos + ww/4, data.unotch, px7, py7);
  }
  else {
    plot->windowToPixel(data.min   , pos - ww/2, px1, py1);
    plot->windowToPixel(data.lower , pos - bw/2, px2, py2);
    plot->windowToPixel(data.median, pos       , px3, py3);
    plot->windowToPixel(data.upper , pos + bw/2, px4, py4);
    plot->windowToPixel(data.max   , pos + ww/2, px5, py5);
    plot->windowToPixel(data.lnotch, pos - ww/4, px6, py6);
    plot->windowToPixel(data.unotch, pos + ww/4, px7, py7);
  }

  //---

  if (notched) {
    QPolygonF poly;

    poly << QPointF(px2, py2);
    poly << QPointF(px4, py2);
    poly << QPointF(px4, py6);
    poly << QPointF(px7, py3);
    poly << QPointF(px4, py7);
    poly << QPointF(px4, py4);
    poly << QPointF(px2, py4);
    poly << QPointF(px2, py7);
    poly << QPointF(px6, py3);
    poly << QPointF(px2, py6);
    poly << QPointF(px2, py2);

    painter->drawPolygon(poly);
  }
  else {
    QRectF prect(px2, py2, px4 - px2, py4 - py2);

    double cxs = plot->lengthPixelWidth (cornerSize);
    double cys = plot->lengthPixelHeight(cornerSize);

    CQChartsRoundedPolygon::draw(painter, prect, cxs, cys);
  }

  //---

  // draw extent line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(px3, py1), QPointF(px3, py5));
  else
    painter->drawLine(QPointF(px1, py3), QPointF(px5, py3));

  //---

  // draw lower/upper horizontal lines
  if (orientation != Qt::Horizontal) {
    painter->drawLine(QPointF(px1, py1), QPointF(px5, py1));
    painter->drawLine(QPointF(px1, py5), QPointF(px5, py5));
  }
  else {
    painter->drawLine(QPointF(px1, py1), QPointF(px1, py5));
    painter->drawLine(QPointF(px5, py1), QPointF(px5, py5));
  }

  //---

  // draw median line
  if (orientation != Qt::Horizontal)
    painter->drawLine(QPointF(px2, py3), QPointF(px4, py3));
  else
    painter->drawLine(QPointF(px3, py2), QPointF(px3, py4));
}

void
drawOutliers(CQChartsPlot *plot, QPainter *painter, const std::vector<double> &ovalues,
             double pos, const CQChartsSymbolData &symbol, const QPen &pen,
             const QBrush &brush, const Qt::Orientation &orientation)
{
  double sx, sy;

  plot->pixelSymbolSize(symbol.size, sx, sy);

  for (auto &o : ovalues) {
    double px1, py1;

    if (orientation != Qt::Horizontal)
      plot->windowToPixel(pos, o, px1, py1);
    else
      plot->windowToPixel(o, pos, px1, py1);

    plot->drawSymbol(painter, QPointF(px1, py1), symbol.type, CQChartsUtil::avg(sx, sy),
                     pen, brush);
  }
}

}
