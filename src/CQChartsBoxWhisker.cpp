#include <CQChartsBoxWhisker.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

namespace CQChartsBoxWhiskerUtil {

void
drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device,
            const CQChartsBoxWhisker &whisker, const CQChartsGeom::BBox &bbox,
            const CQChartsLength &width, const Qt::Orientation &orientation,
            const CQChartsLength &cornerSize)
{
  drawWhisker(plot, device, whisker.statData(), bbox, width, orientation, cornerSize);
}

void
drawWhisker(const CQChartsPlot *plot, CQChartsPaintDevice *device, const CQStatData &statData,
            const CQChartsGeom::BBox &bbox, const CQChartsLength &width,
            const Qt::Orientation &orientation, const CQChartsLength &cornerSize)
{
  // calc widths and position
  double ww, bw;

  if (orientation == Qt::Horizontal) {
    ww = plot->lengthPlotHeight(width);
    bw = ww;
  }
  else {
    ww = plot->lengthPlotWidth(width);
    bw = ww;
  }

  double pos;

  if (orientation == Qt::Horizontal)
    pos = bbox.getYMid();
  else
    pos = bbox.getXMid();

  //---

  bool notched = false;
  bool median  = false;

  std::vector<double> outliers;

  drawWhiskerBar(plot, device, statData, pos, orientation, ww, bw, cornerSize,
                 notched, median, outliers);
}

void
drawWhiskerBar(const CQChartsPlot *plot, CQChartsPaintDevice *device, const CQStatData &statData,
               double pos, const Qt::Orientation &orientation, double ww, double bw,
               const CQChartsLength &cornerSize, bool notched, bool median,
               const std::vector<double> &outliers)
{
  auto pointPosValue = [&](double pos, double value) {
    if (orientation != Qt::Horizontal)
      return CQChartsGeom::Point(pos, value);
    else
      return CQChartsGeom::Point(value, pos);
  };

  CQChartsGeom::Point p1 = pointPosValue(pos - ww/2, statData.min        );
  CQChartsGeom::Point p2 = pointPosValue(pos - bw/2, statData.lowerMedian);
  CQChartsGeom::Point p3 = pointPosValue(pos       , statData.median     );
  CQChartsGeom::Point p4 = pointPosValue(pos + bw/2, statData.upperMedian);
  CQChartsGeom::Point p5 = pointPosValue(pos + ww/2, statData.max        );

  //---

  if (notched) {
    double lnotch = std::max(statData.lnotch, statData.lowerMedian);
    double unotch = std::min(statData.unotch, statData.upperMedian);

    CQChartsGeom::Point p6 = pointPosValue(pos - ww/4, lnotch);
    CQChartsGeom::Point p7 = pointPosValue(pos + ww/4, unotch);

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

    device->drawPolygon(poly);
  }
  else {
    QRectF prect(p2.x, p2.y, p4.x - p2.x, p4.y - p2.y);

    CQChartsDrawUtil::drawRoundedPolygon(device, prect, cornerSize, cornerSize);
  }

  //---

  // draw extent line
  if (orientation != Qt::Horizontal) {
    if (! median) {
      device->drawLine(QPointF(p3.x, p1.y), QPointF(p3.x, p2.y));
      device->drawLine(QPointF(p3.x, p4.y), QPointF(p3.x, p5.y));
    }
    else {
      device->drawLine(QPointF(p3.x, p1.y), QPointF(p3.x, p5.y));
    }
  }
  else {
    if (! median) {
      device->drawLine(QPointF(p1.x, p3.y), QPointF(p2.x, p3.y));
      device->drawLine(QPointF(p4.x, p3.y), QPointF(p5.x, p3.y));
    }
    else {
      device->drawLine(QPointF(p1.x, p3.y), QPointF(p5.x, p3.y));
    }
  }

  //---

  // draw lower/upper horizontal lines
  if (orientation != Qt::Horizontal) {
    device->drawLine(QPointF(p1.x, p1.y), QPointF(p5.x, p1.y));
    device->drawLine(QPointF(p1.x, p5.y), QPointF(p5.x, p5.y));
  }
  else {
    device->drawLine(QPointF(p1.x, p1.y), QPointF(p1.x, p5.y));
    device->drawLine(QPointF(p5.x, p1.y), QPointF(p5.x, p5.y));
  }

  //---

  // draw median line
  if (median) {
    if (orientation != Qt::Horizontal)
      device->drawLine(QPointF(p2.x, p3.y), QPointF(p4.x, p3.y));
    else
      device->drawLine(QPointF(p3.x, p2.y), QPointF(p3.x, p4.y));
  }

  //---

  if (! outliers.empty()) {
    CQChartsSymbolData symbol;

    for (const auto &outlier : outliers) {
      CQChartsGeom::Point po;

      if (orientation != Qt::Horizontal)
        po = CQChartsGeom::Point(p3.x, outlier);
      else
        po = CQChartsGeom::Point(outlier, p3.y);

      plot->drawSymbol(device, po.qpoint(), symbol.type(), symbol.size());
    }
  }
}

}
