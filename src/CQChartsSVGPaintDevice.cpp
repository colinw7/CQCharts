#include <CQChartsSVGPaintDevice.h>
#include <CQChartsSVGUtil.h>
#include <CQChartsUtil.h>
#include <CQChartsImage.h>
#include <CQChartsPath.h>
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

  auto data = dataStack_.back();

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
setClipRect(const BBox &, Qt::ClipOperation)
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
  auto ppath = windowToPixel(path);

  *os_ << " " << CQChartsSVGUtil::pathToString(ppath).toStdString();
}

void
CQChartsSVGPaintDevice::
fillRect(const BBox &bbox)
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
drawRect(const BBox &bbox)
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
drawEllipse(const BBox &bbox, const CQChartsAngle &)
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
drawArc(const BBox &, const CQChartsAngle &, const CQChartsAngle &)
{
  // TODO
}
#endif

void
CQChartsSVGPaintDevice::
drawPolygon(const Polygon &poly)
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
drawPolyline(const Polygon &poly)
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
drawLine(const Point &p1, const Point &p2)
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
drawPoint(const Point &p)
{
  auto pp = windowToPixel(p);

  *os_ << "<rect x=\"" << pp.x << "\" y=\"" << pp.y << "\" width=\"1\" height=\"1\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPaintDevice::
drawText(const Point &p, const QString &text)
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
drawTransformedText(const Point &p, const QString &text)
{
  Point pt(p.x + data_.transformPoint.x, p.y + data_.transformPoint.y);

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
drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool /*stretch*/)
{
  auto pbbox = windowToPixel(bbox);

  double px = pbbox.getXMin();
  double py = pbbox.getYMin();

  auto pw = pixelToWindow(Point(px, py));

  auto qimage = image.sizedImage(int(pbbox.getWidth()), int(pbbox.getHeight()));

  drawImage(pw, qimage);
}

void
CQChartsSVGPaintDevice::
drawImage(const Point &p, const QImage &image)
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

  auto ba64 = ba.toBase64();

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
  auto styleName = data_.font.styleName();

  if (! styleName.length())
    styleName = "normal";

  *os_ << " font-family=\"" << data_.font.family().toStdString() << "\""
          " font-size=\"" << data_.font.pointSizeF() << "\""
          " font-weight=\"" << data_.font.weight() << "\""
          " font-style=\"" << styleName.toStdString() << "\"";
}
