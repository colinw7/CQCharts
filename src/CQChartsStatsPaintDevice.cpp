#include <CQChartsStatsPaintDevice.h>
#include <CQChartsVariant.h>
#include <QPainterPath>

CQChartsStatsPaintDevice::TypeCount CQChartsStatsPaintDevice::s_typeCount;

CQChartsStatsPaintDevice::
CQChartsStatsPaintDevice(CQChartsView *view) :
 CQChartsPaintDevice(view)
{
}

CQChartsStatsPaintDevice::
CQChartsStatsPaintDevice(CQChartsPlot *plot) :
 CQChartsPaintDevice(plot)
{
}

void
CQChartsStatsPaintDevice::
fillPath(const QPainterPath &path, const QBrush &)
{
  BBox bbox(path.boundingRect());

  quadTree_.add(new DrawData(DrawType::PATH, bbox));
}

void
CQChartsStatsPaintDevice::
strokePath(const QPainterPath &path, const QPen &)
{
  BBox bbox(path.boundingRect());

  quadTree_.add(new DrawData(DrawType::PATH, bbox));
}

void
CQChartsStatsPaintDevice::
drawPath(const QPainterPath &path)
{
  BBox bbox(path.boundingRect());

  quadTree_.add(new DrawData(DrawType::PATH, bbox));
}

void
CQChartsStatsPaintDevice::
fillRect(const BBox &bbox)
{
  quadTree_.add(new DrawData(DrawType::RECT, bbox));
}

void
CQChartsStatsPaintDevice::
drawRect(const BBox &bbox)
{
  quadTree_.add(new DrawData(DrawType::RECT, bbox));
}

void
CQChartsStatsPaintDevice::
drawEllipse(const BBox &bbox, const Angle &)
{
  quadTree_.add(new DrawData(DrawType::ELLIPSE, bbox));
}

void
CQChartsStatsPaintDevice::
drawPolygon(const Polygon &polygon)
{
  quadTree_.add(new DrawData(DrawType::POLYGON, polygon.boundingBox()));
}

void
CQChartsStatsPaintDevice::
drawPolyline(const Polygon &polygon)
{
  quadTree_.add(new DrawData(DrawType::POLYLINE, polygon.boundingBox()));
}

void
CQChartsStatsPaintDevice::
drawImage(const Point &p, const Image &image)
{
  auto qimage = image.image();

  auto w = pixelToWindowWidth (qimage.width ());
  auto h = pixelToWindowHeight(qimage.height());

  BBox bbox(p.x, p.y, p.x + w, p.y + h);

  quadTree_.add(new DrawData(DrawType::IMAGE, bbox));
}

void
CQChartsStatsPaintDevice::
drawImageInRect(const BBox &bbox, const Image &, bool, const Angle &)
{
  quadTree_.add(new DrawData(DrawType::IMAGE, bbox));
}

void
CQChartsStatsPaintDevice::
drawPoint(const Point &p)
{
  auto w = pixelToWindowWidth (1);
  auto h = pixelToWindowHeight(1);

  BBox bbox(p.x, p.y, p.x + w, p.y + h);

  quadTree_.add(new DrawData(DrawType::POINT, bbox));
}

void
CQChartsStatsPaintDevice::
drawLine(const Point &p1, const Point &p2)
{
  BBox bbox(p1, p2);

  quadTree_.add(new DrawData(DrawType::LINE, bbox));
}

void
CQChartsStatsPaintDevice::
drawText(const Point &p, const QString &text)
{
  addText(p, text);
}

void
CQChartsStatsPaintDevice::
drawTransformedText(const Point &p, const QString &text)
{
  addText(p, text);
}

void
CQChartsStatsPaintDevice::
addText(const Point &p, const QString &text)
{
  QFontMetricsF fm(font());

  auto w  = pixelToWindowWidth(fm.horizontalAdvance(text));
  auto fa = pixelToWindowHeight(fm.ascent());
  auto fd = pixelToWindowHeight(fm.descent());

  BBox bbox(p.x, p.y - fd, p.x + w, p.y + fa);

  quadTree_.add(new DrawData(DrawType::TEXT, bbox));
}

void
CQChartsStatsPaintDevice::
print(const BBox &bbox) const
{
  auto *th = const_cast<CQChartsStatsPaintDevice *>(this);

  th->s_typeCount.clear();

  if (bbox.isValid()) {
    QuadTree::DataList dataList;

    quadTree_.dataInsideRect(bbox, dataList);

    for (const auto &data : dataList)
      printDrawData(data);
  }
  else {
    th->quadTree_.process(printDrawData);
  }

  auto qbbox = this->bbox();

  auto qvar = CQChartsVariant::fromBBox(qbbox);
  bool ok;

  std::cerr << "BBox: " << CQChartsVariant::toString(qvar, ok).toStdString() << "\n";

  for (const auto &pt : s_typeCount)
    std::cerr << typeToString(pt.first).toStdString() << ": " << pt.second << "\n";
}

void
CQChartsStatsPaintDevice::
printDrawData(DrawData *drawData)
{
  ++s_typeCount[drawData->type];

  //std::cerr << typeToString(drawData->type).toStdString() << ": " <<
  //             drawData->bbox.toString().toStdString() << "\n";
}

CQChartsGeom::BBox
CQChartsStatsPaintDevice::
bbox() const
{
  return quadTree_.rect();
}

QString
CQChartsStatsPaintDevice::
typeToString(const DrawType &drawType)
{
  switch (drawType) {
    case DrawType::PATH    : return "path";
    case DrawType::RECT    : return "rect";
    case DrawType::ELLIPSE : return "ellipse";
    case DrawType::POLYGON : return "polygon";
    case DrawType::POLYLINE: return "polyline";
    case DrawType::POINT   : return "point";
    case DrawType::LINE    : return "line";
    case DrawType::IMAGE   : return "image";
    case DrawType::TEXT    : return "text";
    default:                 break;
  }

  return "none";
}
