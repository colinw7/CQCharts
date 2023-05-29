#include <CQChartsBoxWhisker.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQPropertyViewItem.h>

namespace CQChartsBoxWhiskerUtil {

void
drawWhisker(PaintDevice *device, const BoxWhisker &whisker, const BBox &bbox,
            const DrawData &drawData, PointData &pointData)
{
  std::vector<double> outliers;

  drawWhisker(device, whisker.statData(), outliers, bbox, drawData, pointData);
}

void
drawWhisker(PaintDevice *device, const CQStatData &statData,
            const std::vector<double> &outliers, const BBox &bbox,
            const DrawData &drawData, PointData &pointData)
{
  // calc widths and position
  double ww, bw;

  if (drawData.orientation == Qt::Horizontal) {
    ww = device->lengthWindowHeight(drawData.width);
    bw = ww;
  }
  else {
    ww = device->lengthWindowWidth(drawData.width);
    bw = ww;
  }

  double pos;

  if (drawData.orientation == Qt::Horizontal)
    pos = bbox.getYMid();
  else
    pos = bbox.getXMid();

  //---

  drawWhiskerBar(device, statData, pos, drawData.orientation, ww, bw,
                 drawData.cornerSize, drawData.notched, drawData.median,
                 outliers, pointData);
}

void
drawWhiskerBar(PaintDevice *device, const CQStatData &statData,
               double pos, const Qt::Orientation &orientation, double ww, double bw,
               const Length &cornerSize, bool notched, bool median,
               const std::vector<double> &outliers, PointData &pointData)
{
  auto pointPosValue = [&](double pos, double value) {
    if (statData.scaled)
      value = CMathUtil::map(value, statData.min, statData.max,
                             statData.scaleMin, statData.scaleMax);

    if (orientation != Qt::Horizontal)
      return Point(pos, value);
    else
      return Point(value, pos);
  };

  pointData.min = pointPosValue(pos - ww/2, statData.min        );
  pointData.lmd = pointPosValue(pos - bw/2, statData.lowerMedian);
  pointData.med = pointPosValue(pos       , statData.median     );
  pointData.umd = pointPosValue(pos + bw/2, statData.upperMedian);
  pointData.max = pointPosValue(pos + ww/2, statData.max        );

  //---

  if (notched) {
    double lnotch = std::max(statData.lnotch, statData.lowerMedian);
    double unotch = std::min(statData.unotch, statData.upperMedian);

    auto p6 = pointPosValue(pos - ww/4, lnotch);
    auto p7 = pointPosValue(pos + ww/4, unotch);

    Polygon poly;

    poly.addPoint(Point(pointData.lmd.x, pointData.lmd.y));

    if (orientation != Qt::Horizontal) {
      poly.addPoint(Point(pointData.umd.x, pointData.lmd.y));
      poly.addPoint(Point(pointData.umd.x, p6           .y));
      poly.addPoint(Point(p7           .x, pointData.med.y));
      poly.addPoint(Point(pointData.umd.x, p7           .y));
    }
    else {
      poly.addPoint(Point(pointData.lmd.x, pointData.umd.y));
      poly.addPoint(Point(p6           .x, pointData.umd.y));
      poly.addPoint(Point(pointData.med.x, p7           .y));
      poly.addPoint(Point(p7           .x, pointData.umd.y));
    }

    poly.addPoint(Point(pointData.umd.x, pointData.umd.y));

    if (orientation != Qt::Horizontal) {
      poly.addPoint(Point(pointData.lmd.x, pointData.umd.y));
      poly.addPoint(Point(pointData.lmd.x, p7           .y));
      poly.addPoint(Point(p6           .x, pointData.med.y));
      poly.addPoint(Point(pointData.lmd.x, p6           .y));
    }
    else {
      poly.addPoint(Point(pointData.umd.x, pointData.lmd.y));
      poly.addPoint(Point(p7           .x, pointData.lmd.y));
      poly.addPoint(Point(pointData.med.x, p6           .y));
      poly.addPoint(Point(p6           .x, pointData.lmd.y));
    }

    poly.addPoint(Point(pointData.lmd.x, pointData.lmd.y));

    device->drawPolygon(poly);
  }
  else {
    BBox pbbox(pointData.lmd, pointData.umd);

    if (pbbox.isValid())
      CQChartsDrawUtil::drawRoundedRect(device, pbbox, cornerSize);
  }

  //---

  // draw extent line
  if (orientation != Qt::Horizontal) {
    if (! median) {
      device->drawLine(Point(pointData.med.x, pointData.min.y),
                       Point(pointData.med.x, pointData.lmd.y));
      device->drawLine(Point(pointData.med.x, pointData.umd.y),
                       Point(pointData.med.x, pointData.max.y));
    }
    else {
      device->drawLine(Point(pointData.med.x, pointData.min.y),
                       Point(pointData.med.x, pointData.max.y));
    }
  }
  else {
    if (! median) {
      device->drawLine(Point(pointData.min.x, pointData.med.y),
                       Point(pointData.lmd.x, pointData.med.y));
      device->drawLine(Point(pointData.umd.x, pointData.med.y),
                       Point(pointData.max.x, pointData.med.y));
    }
    else {
      device->drawLine(Point(pointData.min.x, pointData.med.y),
                       Point(pointData.max.x, pointData.med.y));
    }
  }

  //---

  // draw lower/upper horizontal lines
  if (orientation != Qt::Horizontal) {
    device->drawLine(Point(pointData.min.x, pointData.min.y),
                     Point(pointData.max.x, pointData.min.y));
    device->drawLine(Point(pointData.min.x, pointData.max.y),
                     Point(pointData.max.x, pointData.max.y));
  }
  else {
    device->drawLine(Point(pointData.min.x, pointData.min.y),
                     Point(pointData.min.x, pointData.max.y));
    device->drawLine(Point(pointData.max.x, pointData.min.y),
                     Point(pointData.max.x, pointData.max.y));
  }

  //---

  // draw median line
  if (median) {
    if (orientation != Qt::Horizontal)
      device->drawLine(Point(pointData.lmd.x, pointData.med.y),
                       Point(pointData.umd.x, pointData.med.y));
    else
      device->drawLine(Point(pointData.med.x, pointData.lmd.y),
                       Point(pointData.med.x, pointData.umd.y));
  }

  //---

  if (! outliers.empty()) {
    CQChartsSymbolData symbol;

    for (const auto &outlier : outliers) {
      Point po;

      if (orientation != Qt::Horizontal)
        po = Point(pointData.med.x, outlier);
      else
        po = Point(outlier, pointData.med.y);

      if (symbol.symbol().isValid())
        CQChartsDrawUtil::drawSymbol(device, symbol.symbol(), po, symbol.size(), /*scale*/true);
    }
  }
}

}

//------

CQChartsAxisBoxWhisker::
CQChartsAxisBoxWhisker(Plot *plot, const Qt::Orientation &direction) :
 CQChartsObj(plot->charts()), plot_(plot), direction_(direction)
{
}

CQChartsPlot *
CQChartsAxisBoxWhisker::
plot() const
{
  return plot_.data();
}

void
CQChartsAxisBoxWhisker::
setSide(const Side &s)
{
  CQChartsUtil::testAndSet(side_, s, [&]() { dataInvalidate(DataType::SIDE); } );
}

void
CQChartsAxisBoxWhisker::
setDirection(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(direction_, o, [&]() { dataInvalidate(DataType::DIRECTION); } );
}

void
CQChartsAxisBoxWhisker::
setWidth(const Length &l)
{
  CQChartsUtil::testAndSet(width_, l, [&]() { dataInvalidate(DataType::WIDTH); } );
}

void
CQChartsAxisBoxWhisker::
setMargin(const Length &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { dataInvalidate(DataType::MARGIN); } );
}

void
CQChartsAxisBoxWhisker::
setAlpha(const Alpha &a)
{
  CQChartsUtil::testAndSet(alpha_, a, [&]() { dataInvalidate(DataType::ALPHA); } );
}

void
CQChartsAxisBoxWhisker::
setDrawType(const DrawType &t)
{
  CQChartsUtil::testAndSet(drawType_, t, [&]() { dataInvalidate(DataType::DRAW_TYPE); } );
}

CQChartsGeom::BBox
CQChartsAxisBoxWhisker::
calcNBBox(int n) const
{
  auto bbox = calcBBox();

  if (direction() == Qt::Horizontal)
    bbox.setHeight(bbox.getHeight()*n);
  else
    bbox.setWidth (bbox.getWidth ()*n);

  return bbox;
}

CQChartsGeom::BBox
CQChartsAxisBoxWhisker::
calcNDeltaBBox(int n, double delta) const
{
  auto bbox = calcDeltaBBox(delta);

  if (direction() == Qt::Horizontal)
    bbox.setHeight(bbox.getHeight()*n);
  else
    bbox.setWidth (bbox.getWidth ()*n);

  return bbox;
}

CQChartsGeom::BBox
CQChartsAxisBoxWhisker::
calcBBox() const
{
  return calcDeltaBBox(0.0);
}

CQChartsGeom::BBox
CQChartsAxisBoxWhisker::
calcDeltaBBox(double delta) const
{
  BBox bbox;

  if (! isVisible())
    return bbox;

  auto dataRange = CQChartsGeom::Range(plot()->calcDataRange());

  //---

  CQChartsGeom::Point p1, p2;

  if (direction() == Qt::Horizontal) {
    double wm = plot()->lengthPlotHeight(margin());
    double ww = plot()->lengthPlotHeight(width ());

    double s = ww + 2*wm;

    if (side().type() == Side::Type::BOTTOM_LEFT) {
      p1 = Point(dataRange.xmin(), dataRange.ymin() - delta - s);
      p2 = Point(dataRange.xmax(), dataRange.ymin() - delta);
    }
    else {
      p1 = Point(dataRange.xmin(), dataRange.ymax() + delta);
      p2 = Point(dataRange.xmax(), dataRange.ymax() + delta + s);
    }
  }
  else {
    double wm = plot()->lengthPlotWidth(margin());
    double ww = plot()->lengthPlotWidth(width ());

    double s = ww + 2*wm;

    if (side().type() == Side::Type::BOTTOM_LEFT) {
      p1 = Point(dataRange.xmin() - delta - s, dataRange.ymin());
      p2 = Point(dataRange.xmin() - delta    , dataRange.ymax());
    }
    else {
      p1 = Point(dataRange.xmax() + delta    , dataRange.ymin());
      p2 = Point(dataRange.xmax() + delta + s, dataRange.ymax());
    }
  }

  bbox += p1;
  bbox += p2;

  return bbox;
}

bool
CQChartsAxisBoxWhisker::
contains(const Point &p) const
{
  return calcBBox().inside(p);
}

void
CQChartsAxisBoxWhisker::
dataInvalidate(int)
{
  plot()->resetExtraFitBBox();

  plot()->drawObjs();
}

void
CQChartsAxisBoxWhisker::
addProperties(const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(plot()->addProperty(path, this, name, alias)->setDesc(desc));
  };

  addProp(path, "visible" , "", desc + " visible");
  addProp(path, "side"    , "", desc + " side");
  addProp(path, "width"   , "", desc + " width");
  addProp(path, "margin"  , "", desc + " margin");
  addProp(path, "drawType", "", desc + " draw type");

  addProp(path + "/fill", "alpha", "", desc + " alpha");
}

void
CQChartsAxisBoxWhisker::
draw(PaintDevice *device, const PenBrush &penBrush, int ind, double delta)
{
  auto penBrush1 = penBrush;

  CQChartsDrawUtil::setBrushAlpha(penBrush1.brush, alpha().value());

  CQChartsDrawUtil::setPenBrush(device, penBrush1);

  //---

  auto dataRange = CQChartsGeom::Range(plot()->calcDataRange());

  BBox rect;

  if (direction() == Qt::Horizontal) {
    double ww = plot()->lengthPlotHeight(width ());
    double wm = plot()->lengthPlotHeight(margin());

    double pos = (side().type() == Side::Type::BOTTOM_LEFT ?
      dataRange.ymin() - delta - (ind + 1)*ww - wm :
      dataRange.ymax() + delta +  ind     *ww + wm);

    rect = BBox(whisker_.min(), pos, whisker_.max(), pos + ww);
  }
  else {
    double ww = plot()->lengthPlotWidth(width ());
    double wm = plot()->lengthPlotWidth(margin());

    double pos = (side().type() == Side::Type::BOTTOM_LEFT ?
      dataRange.xmin() - delta - (ind + 1)*ww - wm :
      dataRange.xmax() + delta +  ind     *ww + wm);

    rect = BBox(pos, whisker_.min(), pos + ww, whisker_.max());
  }

  switch (drawType()) {
    case DrawType::WHISKER: {
      CQChartsBoxWhiskerUtil::DrawData  drawData;
      CQChartsBoxWhiskerUtil::PointData pointData;

      drawData.width       = width();
      drawData.orientation = direction();

      CQChartsBoxWhiskerUtil::drawWhisker(device, whisker_, rect, drawData, pointData);

      break;
    }
    case DrawType::WHISKER_BAR: {
      std::vector<double> outliers;

      for (const auto &x : whisker_.values()) {
        if (whisker_.statData().isOutlier(x))
          outliers.push_back(x);
      }

      CQChartsBoxWhiskerUtil::DrawData  drawData;
      CQChartsBoxWhiskerUtil::PointData pointData;

      drawData.width       = width();
      drawData.orientation = direction();

      CQChartsBoxWhiskerUtil::drawWhisker(device, whisker_.statData(), outliers, rect,
                                          drawData, pointData);

      break;
    }
    default:
      break;
  }

  if (plot()->showBoxes())
    plot()->drawWindowColorBox(device, rect, Qt::red);
}

//------

CQChartsAxisDensity::
CQChartsAxisDensity(Plot *plot, const Qt::Orientation &direction) :
 CQChartsObj(plot->charts()), plot_(plot), direction_(direction)
{
}

CQChartsPlot *
CQChartsAxisDensity::
plot() const
{
  return plot_.data();
}

void
CQChartsAxisDensity::
setSide(const Side &s)
{
  CQChartsUtil::testAndSet(side_, s, [&]() { dataInvalidate(DataType::SIDE); } );
}

void
CQChartsAxisDensity::
setDirection(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(direction_, o, [&]() { dataInvalidate(DataType::DIRECTION); } );
}

void
CQChartsAxisDensity::
setWidth(const Length &l)
{
  CQChartsUtil::testAndSet(width_, l, [&]() { dataInvalidate(DataType::WIDTH); } );
}

void
CQChartsAxisDensity::
setAlpha(const Alpha &a)
{
  CQChartsUtil::testAndSet(alpha_, a, [&]() { dataInvalidate(DataType::ALPHA); } );
}

void
CQChartsAxisDensity::
setDrawType(const DrawType &t)
{
  CQChartsUtil::testAndSet(drawType_, t, [&]() { dataInvalidate(DataType::DRAW_TYPE); } );
}

CQChartsGeom::BBox
CQChartsAxisDensity::
calcBBox() const
{
  return calcDeltaBBox(0.0);
}

CQChartsGeom::BBox
CQChartsAxisDensity::
calcDeltaBBox(double delta) const
{
  BBox bbox;

  if (! isVisible())
    return bbox;

  auto dataRange = CQChartsGeom::Range(plot()->calcDataRange());

  //---

  CQChartsGeom::Point p1, p2;

  if (direction() == Qt::Horizontal) {
    double dh = plot()->lengthPlotHeight(width());

    if (side().type() == Side::Type::BOTTOM_LEFT) {
      p1 = Point(dataRange.xmin(), dataRange.ymin() - delta - dh);
      p2 = Point(dataRange.xmax(), dataRange.ymin() - delta);
    }
    else {
      p1 = Point(dataRange.xmin(), dataRange.ymax() + delta);
      p2 = Point(dataRange.xmax(), dataRange.ymax() + delta  + dh);
    }
  }
  else {
    double dw = plot()->lengthPlotWidth(width());

    if (side().type() == Side::Type::BOTTOM_LEFT) {
      p1 = Point(dataRange.xmin() - delta - dw, dataRange.ymin());
      p2 = Point(dataRange.xmin() - delta     , dataRange.ymax());
    }
    else {
      p1 = Point(dataRange.xmax() + delta     , dataRange.ymin());
      p2 = Point(dataRange.xmax() + delta + dw, dataRange.ymax());
    }
  }

  bbox += p1;
  bbox += p2;

  return bbox;
}

bool
CQChartsAxisDensity::
contains(const Point &p) const
{
  return calcBBox().inside(p);
}

void
CQChartsAxisDensity::
dataInvalidate(int)
{
  plot()->resetExtraFitBBox();

  plot()->drawObjs();
}

void
CQChartsAxisDensity::
addProperties(const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(plot()->addProperty(path, this, name, alias)->setDesc(desc));
  };

  addProp(path, "visible" , "", desc + " visible");
  addProp(path, "side"    , "", desc + " side");
  addProp(path, "width"   , "", desc + " width");
  addProp(path, "drawType", "", desc + " draw type");

  addProp(path + "/fill", "alpha", "", desc + " alpha");
}

void
CQChartsAxisDensity::
draw(PaintDevice *device, const PenBrush &penBrush, double delta)
{
  auto penBrush1 = penBrush;

  CQChartsDrawUtil::setBrushAlpha(penBrush1.brush, alpha().value());

  CQChartsDrawUtil::setPenBrush(device, penBrush1);

  //---

  auto dataRange = CQChartsGeom::Range(plot()->calcDataRange());

  double xmin = density().xmin1();
  double xmax = density().xmax1();

  BBox rect;

  if (direction() == Qt::Horizontal) {
    double dh = plot()->lengthPlotHeight(width());

    double pos = (side().type() == Side::Type::BOTTOM_LEFT ? dataRange.ymin() - delta - dh :
                                                             dataRange.ymax() + delta);

    rect = BBox(xmin, pos, xmax, pos + dh);
  }
  else {
    double dw = plot()->lengthPlotWidth(width());

    double pos = (side().type() == Side::Type::BOTTOM_LEFT ? dataRange.xmin() - delta - dw :
                                                             dataRange.xmax() + delta);

    rect = BBox(pos, xmin, pos + dw, xmax);
  }

  //---

  switch (drawType()) {
    case DrawType::DISTRIBUTION:
      density().drawDistribution(plot(), device, rect, direction());
      break;
    case DrawType::BUCKETS:
      density().drawBuckets(device, rect, direction());
      break;
    default:
      break;
  }

  if (plot()->showBoxes())
    plot()->drawWindowColorBox(device, rect, Qt::red);
}
