#include <CQChartsSVGPaintDevice.h>
#include <CQChartsUtil.h>
#include <CQChartsImage.h>
#include <QBuffer>

CQChartsSVGPaintDevice::
CQChartsSVGPaintDevice(CQChartsView *view, std::ostream &os) :
 CQChartsHtmlPaintDevice(view, os)
{
}

CQChartsSVGPaintDevice::
CQChartsSVGPaintDevice(CQChartsPlot *plot, std::ostream &os) :
 CQChartsHtmlPaintDevice(plot, os)
{
}

void
CQChartsSVGPaintDevice::
save()
{
  dataStack_.push_back(data_);
}

void
CQChartsSVGPaintDevice::
restore()
{
  assert(! dataStack_.empty());

  Data data = dataStack_.back();

  dataStack_.pop_back();

  setPen  (data.pen);
  setBrush(data.brush);

  data_ = data;
}

void
CQChartsSVGPaintDevice::
setClipPath(const QPainterPath &, Qt::ClipOperation)
{
  // TODO
}

void
CQChartsSVGPaintDevice::
setClipRect(const CQChartsGeom::BBox &, Qt::ClipOperation)
{
  // TODO
}

void
CQChartsSVGPaintDevice::
setPen(const QPen &pen)
{
  data_.pen = pen;
}

void
CQChartsSVGPaintDevice::
setBrush(const QBrush &brush)
{
  data_.brush = brush;
}

void
CQChartsSVGPaintDevice::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  setBrush(brush);

  *os_ << "<path d=\"";

  addPathParts(path);

  *os_ << "\"";

  *os_ << " style=\"";

  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
strokePath(const QPainterPath &path, const QPen &pen)
{
  setPen(pen);

  *os_ << "<path d=\"";

  addPathParts(path);

  *os_ << "\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawPath(const QPainterPath &path)
{
  *os_ << "<path d=\"";

  addPathParts(path);

  *os_ << "\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
addPathParts(const QPainterPath &path)
{
  QPainterPath ppath = windowToPixel(path);

  int n = ppath.elementCount();

  for (int i = 0; i < n; ++i) {
    const QPainterPath::Element &e = ppath.elementAt(i);

    if      (e.isMoveTo())
      *os_ << " M " << e.x << " " << e.y;
    else if (e.isLineTo())
      *os_ << " L " << e.x << ", " << e.y;
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = ppath.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = ppath.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        if (e2t == QPainterPath::CurveToDataElement) {
          *os_ << " C" << e.x << " " << e.y << " " <<
                  e1.x << " " << e1.y << " " << e2.x << " " << e2.y;

          i += 2;
        }
        else {
          *os_ << " Q" << e.x << " " << e.y << " " << e1.x << " " << e1.y;

          ++i;
        }
      }
    }
    else {
      assert(false);
    }
  }
}

void
CQChartsSVGPaintDevice::
fillRect(const CQChartsGeom::BBox &bbox)
{
  auto pbbox = windowToPixel(bbox);

  *os_ << "<rect x=\"" << pbbox.getXMin() << "\" y=\"" << pbbox.getYMin() << "\" " <<
          "width=\"" << pbbox.getWidth() << "\" height=\"" << pbbox.getHeight() << "\"";

  *os_ << " style=\"";

  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawRect(const CQChartsGeom::BBox &bbox)
{
  auto pbbox = windowToPixel(bbox);

  *os_ << "<rect x=\"" << pbbox.getXMin() << "\" y=\"" << pbbox.getYMin() << "\" " <<
          "width=\"" << pbbox.getWidth() << "\" height=\"" << pbbox.getHeight() << "\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawEllipse(const CQChartsGeom::BBox &bbox, const CQChartsAngle &)
{
  auto pbbox = windowToPixel(bbox);

  *os_ << "<ellipse cx=\"" << pbbox.getXMid() << "\" cy=\"" << pbbox.getYMid() << "\" " <<
          "rx=\"" << pbbox.getWidth()/2 << " ry=\"" << pbbox.getHeight()/2 << "\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\">\n";
}

#if 0
void
CQChartsSVGPaintDevice::
drawArc(const CQChartsGeom::BBox &, const CQChartsAngle &, const CQChartsAngle &)
{
  // TODO
}
#endif

void
CQChartsSVGPaintDevice::
drawPolygon(const CQChartsGeom::Polygon &poly)
{
  auto ppoly = windowToPixel(poly);

  *os_ << "<path d=\"";

  int np = ppoly.size();

  for (int i = 0; i < np; ++i) {
    const auto &p = ppoly.point(i);

    if (i == 0)
      *os_ << "M " << p.x << " " << p.y;
    else
      *os_ << "L " << p.x << " " << p.y;
  }

  *os_ << "z\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawPolyline(const CQChartsGeom::Polygon &poly)
{
  auto ppoly = windowToPixel(poly);

  *os_ << "<path d=\"";

  int np = ppoly.size();

  for (int i = 0; i < np; ++i) {
    const auto &p = ppoly.point(i);

    if (i == 0)
      *os_ << "M " << p.x << " " << p.y;
    else
      *os_ << "L " << p.x << " " << p.y;
  }

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawLine(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2)
{
  auto pp1 = windowToPixel(p1);
  auto pp2 = windowToPixel(p2);

  *os_ << "<line x1=\"" << pp1.x << "\" y1=\"" << pp1.y << "\" " <<
                "x2=\"" << pp2.x << "\" y2=\"" << pp2.y << "\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawPoint(const CQChartsGeom::Point &p)
{
  auto pp = windowToPixel(p);

  *os_ << "<rect x=\"" << pp.x << "\" y=\"" << pp.y << "\" width=\"1\" height=\"1\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawText(const CQChartsGeom::Point &p, const QString &text)
{
  auto pp = windowToPixel(p);

  *os_ << "<text xml:space=\"preserve\" x=\"" << pp.x << "\" y=\"" << pp.y << "\"";

  writeFont();

  *os_ << " fill=\"" << CQChartsUtil::encodeSVGColor(data_.pen.color()).toStdString() << "\" ";

  if (data_.brush.color().alpha() < 255)
    *os_ << " fill-opacity=\"" << data_.pen.color().alphaF() << "\" ";

  *os_ << " stroke=\"none\"";

  *os_ << ">" << text.toStdString();

  *os_ << "</text>\n";
}

void
CQChartsSVGPaintDevice::
drawTransformedText(const CQChartsGeom::Point &p, const QString &text)
{
  CQChartsGeom::Point pt(p.x + data_.transformPoint.x, p.y + data_.transformPoint.y);

  auto ppt = windowToPixel(pt);

  double a = CMathUtil::Deg2Rad(data_.transformAngle);
  double c = std::cos(a);
  double s = std::sin(a);

  *os_ << "<g transform=\"matrix(" << c << "," << -s << "," << s << "," << c << "," <<
          ppt.x << "," << ppt.y << ")\">\n";

  *os_ << "<text x=\"0\" y=\"0\"";

  writeFont();

  *os_ << " style=\"";

  writePen();

  *os_ << "\">" << text.toStdString();
  *os_ << "</text>\n";

  *os_ << "</g>\n";
}

void
CQChartsSVGPaintDevice::
drawImageInRect(const CQChartsGeom::BBox &bbox, const CQChartsImage &image, bool /*stretch*/)
{
  const QImage qimage = image.image();

  auto pbbox = windowToPixel(bbox);

  double px = pbbox.getXMin();
  double py = pbbox.getYMin();

  auto pw = pixelToWindow(CQChartsGeom::Point(px, py));

  drawImage(pw,
    qimage.scaled(int(pbbox.getWidth()), int(pbbox.getHeight()), Qt::IgnoreAspectRatio));
}

void
CQChartsSVGPaintDevice::
drawImage(const CQChartsGeom::Point &p, const QImage &image)
{
  auto pt = windowToPixel(p);

  int w = image.width ();
  int h = image.height();

  // writes image into ba in PNG format
  QByteArray ba;
  QBuffer qbuffer(&ba);
  qbuffer.open(QIODevice::WriteOnly);
  image.save(&qbuffer, "PNG");

  *os_ << "<image x=\"" << pt.x << "\" y=\"" << pt.y <<
          "\" width=\"" << w << "\" height=\"" << h << "\" preserveAspectRatio=\"none\" " <<
          "xlink:href=\"data:image/png;base64,";

  std::vector<unsigned char> buffer;

  QByteArray ba64 = ba.toBase64();

  *os_ << ba64.constData();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
setFont(const QFont &f)
{
  data_.font    = f;
  data_.hasFont = true;
}

void
CQChartsSVGPaintDevice::
setTransform(const QTransform &t, bool combine)
{
  if (combine)
    data_.transform *= t;
  else
    data_.transform = t;
}

void
CQChartsSVGPaintDevice::
startGroup(const QString &id, const GroupData &groupData)
{
  *os_ << "<g id=\"" << id.toStdString() << "\"";

  if (! groupData.visible)
    *os_ << " visibility=\"hidden\"";

  if (groupData.onclick)
    *os_ << " onclick=\"" <<
      groupData.clickProc.toStdString() << "('" << id.toStdString() << "')\"";

  if (groupData.hasTip) {
    *os_ << " onmousemove=\"showTooltip(evt, '" << groupData.tipStr.toStdString() << "')\"";
    *os_ << " onmouseout=\"hideTooltip()\"";
  }

  *os_ << ">\n";
}

void
CQChartsSVGPaintDevice::
endGroup()
{
  *os_ << "</g>\n";
}

void
CQChartsSVGPaintDevice::
writePen() const
{
  if (data_.pen.style() == Qt::NoPen)
    *os_ << "stroke-opacity:0; ";
  else {
    *os_ << "stroke:" << CQChartsUtil::encodeSVGColor(data_.pen.color()).toStdString() << "; ";

    if (data_.pen.color().alpha() < 255)
      *os_ << "stroke-opacity:" << data_.pen.color().alphaF() << "; ";
  }

  double w = std::max(data_.pen.widthF(), 1.0);

  *os_ << "stroke-width:" << w << "; ";
}

void
CQChartsSVGPaintDevice::
writeBrush() const
{
  if (data_.brush.style() == Qt::NoBrush)
    *os_ << "fill-opacity:0; ";
  else {
    *os_ << "fill:" << CQChartsUtil::encodeSVGColor(data_.brush.color()).toStdString() << "; ";

    if (data_.brush.color().alpha() < 255)
      *os_ << "fill-opacity:" << data_.brush.color().alphaF() << "; ";
  }
}

void
CQChartsSVGPaintDevice::
writeFont() const
{
  QString styleName = data_.font.styleName();

  if (! styleName.length())
    styleName = "normal";

  *os_ << " font-family=\"" << data_.font.family().toStdString() << "\""
          " font-size=\"" << data_.font.pointSizeF() << "\""
          " font-weight=\"" << data_.font.weight() << "\""
          " font-style=\"" << styleName.toStdString() << "\"";
}
