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

  auto p1 = pointPosValue(pos - ww/2, statData.min        );
  auto p2 = pointPosValue(pos - bw/2, statData.lowerMedian);
  auto p3 = pointPosValue(pos       , statData.median     );
  auto p4 = pointPosValue(pos + bw/2, statData.upperMedian);
  auto p5 = pointPosValue(pos + ww/2, statData.max        );

  //---

  if (notched) {
    double lnotch = std::max(statData.lnotch, statData.lowerMedian);
    double unotch = std::min(statData.unotch, statData.upperMedian);

    auto p6 = pointPosValue(pos - ww/4, lnotch);
    auto p7 = pointPosValue(pos + ww/4, unotch);

    CQChartsGeom::Polygon poly;

    poly.addPoint(CQChartsGeom::Point(p2.x, p2.y));
    poly.addPoint(CQChartsGeom::Point(p4.x, p2.y));
    poly.addPoint(CQChartsGeom::Point(p4.x, p6.y));
    poly.addPoint(CQChartsGeom::Point(p7.x, p3.y));
    poly.addPoint(CQChartsGeom::Point(p4.x, p7.y));
    poly.addPoint(CQChartsGeom::Point(p4.x, p4.y));
    poly.addPoint(CQChartsGeom::Point(p2.x, p4.y));
    poly.addPoint(CQChartsGeom::Point(p2.x, p7.y));
    poly.addPoint(CQChartsGeom::Point(p6.x, p3.y));
    poly.addPoint(CQChartsGeom::Point(p2.x, p6.y));
    poly.addPoint(CQChartsGeom::Point(p2.x, p2.y));

    device->drawPolygon(poly);
  }
  else {
    CQChartsGeom::BBox pbbox(p2, p4);

    if (pbbox.isValid())
      CQChartsDrawUtil::drawRoundedPolygon(device, pbbox, cornerSize);
  }

  //---

  // draw extent line
  if (orientation != Qt::Horizontal) {
    if (! median) {
      device->drawLine(CQChartsGeom::Point(p3.x, p1.y), CQChartsGeom::Point(p3.x, p2.y));
      device->drawLine(CQChartsGeom::Point(p3.x, p4.y), CQChartsGeom::Point(p3.x, p5.y));
    }
    else {
      device->drawLine(CQChartsGeom::Point(p3.x, p1.y), CQChartsGeom::Point(p3.x, p5.y));
    }
  }
  else {
    if (! median) {
      device->drawLine(CQChartsGeom::Point(p1.x, p3.y), CQChartsGeom::Point(p2.x, p3.y));
      device->drawLine(CQChartsGeom::Point(p4.x, p3.y), CQChartsGeom::Point(p5.x, p3.y));
    }
    else {
      device->drawLine(CQChartsGeom::Point(p1.x, p3.y), CQChartsGeom::Point(p5.x, p3.y));
    }
  }

  //---

  // draw lower/upper horizontal lines
  if (orientation != Qt::Horizontal) {
    device->drawLine(CQChartsGeom::Point(p1.x, p1.y), CQChartsGeom::Point(p5.x, p1.y));
    device->drawLine(CQChartsGeom::Point(p1.x, p5.y), CQChartsGeom::Point(p5.x, p5.y));
  }
  else {
    device->drawLine(CQChartsGeom::Point(p1.x, p1.y), CQChartsGeom::Point(p1.x, p5.y));
    device->drawLine(CQChartsGeom::Point(p5.x, p1.y), CQChartsGeom::Point(p5.x, p5.y));
  }

  //---

  // draw median line
  if (median) {
    if (orientation != Qt::Horizontal)
      device->drawLine(CQChartsGeom::Point(p2.x, p3.y), CQChartsGeom::Point(p4.x, p3.y));
    else
      device->drawLine(CQChartsGeom::Point(p3.x, p2.y), CQChartsGeom::Point(p3.x, p4.y));
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

      plot->drawSymbol(device, po, symbol.type(), symbol.size());
    }
  }
}

}
