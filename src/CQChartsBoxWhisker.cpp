#include <CQChartsBoxWhisker.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQPropertyViewItem.h>

namespace CQChartsBoxWhiskerUtil {

void
drawWhisker(PaintDevice *device, const BoxWhisker &whisker, const BBox &bbox,
            const Length &width, const Qt::Orientation &orientation, const Length &cornerSize)
{
  std::vector<double> outliers;

  drawWhisker(device, whisker.statData(), outliers, bbox, width, orientation, cornerSize);
}

void
drawWhisker(PaintDevice *device, const CQStatData &statData,
            const std::vector<double> &outliers, const BBox &bbox, const Length &width,
            const Qt::Orientation &orientation, const Length &cornerSize)
{
  // calc widths and position
  double ww, bw;

  if (orientation == Qt::Horizontal) {
    ww = device->lengthWindowHeight(width);
    bw = ww;
  }
  else {
    ww = device->lengthWindowWidth(width);
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

  drawWhiskerBar(device, statData, pos, orientation, ww, bw, cornerSize,
                 notched, median, outliers);
}

void
drawWhiskerBar(PaintDevice *device, const CQStatData &statData,
               double pos, const Qt::Orientation &orientation, double ww, double bw,
               const Length &cornerSize, bool notched, bool median,
               const std::vector<double> &outliers)
{
  auto pointPosValue = [&](double pos, double value) {
    if (orientation != Qt::Horizontal)
      return Point(pos, value);
    else
      return Point(value, pos);
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

    Polygon poly;

    poly.addPoint(Point(p2.x, p2.y));
    poly.addPoint(Point(p4.x, p2.y));
    poly.addPoint(Point(p4.x, p6.y));
    poly.addPoint(Point(p7.x, p3.y));
    poly.addPoint(Point(p4.x, p7.y));
    poly.addPoint(Point(p4.x, p4.y));
    poly.addPoint(Point(p2.x, p4.y));
    poly.addPoint(Point(p2.x, p7.y));
    poly.addPoint(Point(p6.x, p3.y));
    poly.addPoint(Point(p2.x, p6.y));
    poly.addPoint(Point(p2.x, p2.y));

    device->drawPolygon(poly);
  }
  else {
    BBox pbbox(p2, p4);

    if (pbbox.isValid())
      CQChartsDrawUtil::drawRoundedPolygon(device, pbbox, cornerSize);
  }

  //---

  // draw extent line
  if (orientation != Qt::Horizontal) {
    if (! median) {
      device->drawLine(Point(p3.x, p1.y), Point(p3.x, p2.y));
      device->drawLine(Point(p3.x, p4.y), Point(p3.x, p5.y));
    }
    else {
      device->drawLine(Point(p3.x, p1.y), Point(p3.x, p5.y));
    }
  }
  else {
    if (! median) {
      device->drawLine(Point(p1.x, p3.y), Point(p2.x, p3.y));
      device->drawLine(Point(p4.x, p3.y), Point(p5.x, p3.y));
    }
    else {
      device->drawLine(Point(p1.x, p3.y), Point(p5.x, p3.y));
    }
  }

  //---

  // draw lower/upper horizontal lines
  if (orientation != Qt::Horizontal) {
    device->drawLine(Point(p1.x, p1.y), Point(p5.x, p1.y));
    device->drawLine(Point(p1.x, p5.y), Point(p5.x, p5.y));
  }
  else {
    device->drawLine(Point(p1.x, p1.y), Point(p1.x, p5.y));
    device->drawLine(Point(p5.x, p1.y), Point(p5.x, p5.y));
  }

  //---

  // draw median line
  if (median) {
    if (orientation != Qt::Horizontal)
      device->drawLine(Point(p2.x, p3.y), Point(p4.x, p3.y));
    else
      device->drawLine(Point(p3.x, p2.y), Point(p3.x, p4.y));
  }

  //---

  if (! outliers.empty()) {
    CQChartsSymbolData symbol;

    for (const auto &outlier : outliers) {
      Point po;

      if (orientation != Qt::Horizontal)
        po = Point(p3.x, outlier);
      else
        po = Point(outlier, p3.y);

      CQChartsDrawUtil::drawSymbol(device, symbol.type(), po, symbol.size());
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

void
CQChartsAxisBoxWhisker::
setSide(const Side &s)
{
  CQChartsUtil::testAndSet(side_, s, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisBoxWhisker::
setDirection(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(direction_, o, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisBoxWhisker::
setWidth(const Length &l)
{
  CQChartsUtil::testAndSet(width_, l, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisBoxWhisker::
setMargin(const Length &l)
{
  CQChartsUtil::testAndSet(margin_, l, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisBoxWhisker::
setAlpha(const Alpha &a)
{
  CQChartsUtil::testAndSet(alpha_, a, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisBoxWhisker::
setDrawType(const DrawType &t)
{
  CQChartsUtil::testAndSet(drawType_, t, [&]() { dataInvalidate(); } );
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

    if (side() == Side::Type::BOTTOM_LEFT) {
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

    if (side() == Side::Type::BOTTOM_LEFT) {
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
dataInvalidate()
{
  plot()->resetAnnotationBBox();

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

    double pos = (side() == Side::Type::BOTTOM_LEFT ?
      dataRange.ymin() - delta - (ind + 1)*ww - wm :
      dataRange.ymax() + delta +  ind     *ww + wm);

    rect = BBox(whisker_.min(), pos, whisker_.max(), pos + ww);
  }
  else {
    double ww = plot()->lengthPlotWidth(width ());
    double wm = plot()->lengthPlotWidth(margin());

    double pos = (side() == Side::Type::BOTTOM_LEFT ?
      dataRange.xmin() - delta - (ind + 1)*ww - wm :
      dataRange.xmax() + delta +  ind     *ww + wm);

    rect = BBox(pos, whisker_.min(), pos + ww, whisker_.max());
  }

  switch (drawType()) {
    case DrawType::WHISKER:
      CQChartsBoxWhiskerUtil::drawWhisker(device, whisker_, rect, width(), direction());
      break;
    case DrawType::WHISKER_BAR: {
      std::vector<double> outliers;

      for (const auto &x : whisker_.values()) {
        if (whisker_.statData().isOutlier(x))
          outliers.push_back(x);
      }

      CQChartsBoxWhiskerUtil::drawWhisker(device, whisker_.statData(), outliers, rect,
                                          width(), direction());
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

void
CQChartsAxisDensity::
setSide(const Side &s)
{
  CQChartsUtil::testAndSet(side_, s, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisDensity::
setDirection(const Qt::Orientation &o)
{
  CQChartsUtil::testAndSet(direction_, o, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisDensity::
setWidth(const Length &l)
{
  CQChartsUtil::testAndSet(width_, l, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisDensity::
setAlpha(const Alpha &a)
{
  CQChartsUtil::testAndSet(alpha_, a, [&]() { dataInvalidate(); } );
}

void
CQChartsAxisDensity::
setDrawType(const DrawType &t)
{
  CQChartsUtil::testAndSet(drawType_, t, [&]() { dataInvalidate(); } );
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

    if (side() == Side::Type::BOTTOM_LEFT ) {
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

    if (side() == Side::Type::BOTTOM_LEFT ) {
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
dataInvalidate()
{
  plot()->resetAnnotationBBox();

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

    double pos = (side() == Side::Type::BOTTOM_LEFT ? dataRange.ymin() - delta - dh :
                                                      dataRange.ymax() + delta);

    rect = BBox(xmin, pos, xmax, pos + dh);
  }
  else {
    double dw = plot()->lengthPlotWidth(width());

    double pos = (side() == Side::Type::BOTTOM_LEFT ? dataRange.xmin() - delta - dw :
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
