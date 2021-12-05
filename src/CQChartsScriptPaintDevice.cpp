#include <CQChartsScriptPaintDevice.h>
#include <CQChartsUtil.h>
#include <CQChartsImage.h>
#include <QBuffer>

CQChartsScriptPaintDevice::
CQChartsScriptPaintDevice(CQChartsView *view, std::ostream &os) :
 CQChartsHtmlPaintDevice(view, os)
{
}

CQChartsScriptPaintDevice::
CQChartsScriptPaintDevice(CQChartsPlot *plot, std::ostream &os) :
 CQChartsHtmlPaintDevice(plot, os)
{
}

void
CQChartsScriptPaintDevice::
save()
{
  dataStack_.push_back(data_);

  *os_ << "  " << context() << ".gc.save();\n";
}

void
CQChartsScriptPaintDevice::
restore()
{
  assert(! dataStack_.empty());

  auto data = dataStack_.back();

  dataStack_.pop_back();

  setPen  (data.pen);
  setBrush(data.brush);

  data_ = data;

  *os_ << "  " << context() << ".gc.restore();\n";
}

void
CQChartsScriptPaintDevice::
setClipPath(const QPainterPath &, Qt::ClipOperation op)
{
  *os_ << "  charts.setClipPath(path, " << int(op) << ");\n";
}

void
CQChartsScriptPaintDevice::
setClipRect(const BBox &bbox, Qt::ClipOperation op)
{
  *os_ << "  charts.setClipRect(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ", " << int(op) << ");\n";
}

void
CQChartsScriptPaintDevice::
setPen(const QPen &pen)
{
  if (strokeStyleName() == "") {
    if (pen.style() != data_.pen.style() || pen.color() != data_.pen.color()) {
      if (pen.style() == Qt::NoPen)
        *os_ << "  " << context() << ".gc.strokeStyle=\"#00000000\";\n";
      else
        *os_ << "  " << context() << ".gc.strokeStyle=\"" <<
          CQChartsUtil::encodeScriptColor(pen.color()).toStdString() << "\";\n";
    }
  }
  else {
    *os_ << "  " << context() << ".gc.strokeStyle=this." <<
            strokeStyleName().toStdString() << ";\n";
  }

  if (pen.widthF() != data_.pen.widthF())
    *os_ << "  " << context() << ".gc.lineWidth=" << pen.widthF() << "\n";

  data_.pen = pen;
}

void
CQChartsScriptPaintDevice::
setBrush(const QBrush &brush)
{
  if (fillStyleName() == "") {
    if (brush.style() != data_.brush.style() || brush.color() != data_.brush.color()) {
      if (brush.style() == Qt::NoBrush)
        *os_ << "  " << context() << ".gc.fillStyle=\"#00000000\";\n";
      else
        *os_ << "  " << context() << ".gc.fillStyle=\"" <<
          CQChartsUtil::encodeScriptColor(brush.color()).toStdString() << "\";\n";
    }
  }
  else {
    *os_ << "  " << context() << ".gc.fillStyle=this." <<
            fillStyleName().toStdString() << ";\n";
  }

  data_.brush = brush;
}

void
CQChartsScriptPaintDevice::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  setBrush(brush);

  addPathParts(path);

  *os_ << "  " << context() << ".gc.fill();\n";
}

void
CQChartsScriptPaintDevice::
strokePath(const QPainterPath &path, const QPen &pen)
{
  setPen(pen);

  addPathParts(path);

  *os_ << "  " << context() << ".gc.stroke();\n";
}

void
CQChartsScriptPaintDevice::
drawPath(const QPainterPath &path)
{
  addPathParts(path);

  *os_ << "  " << context() << ".gc.fill();\n";
  *os_ << "  " << context() << ".gc.stroke();\n";
}

void
CQChartsScriptPaintDevice::
addPathParts(const QPainterPath &path)
{
  *os_ << "  " << context() << ".gc.beginPath();\n";

  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(std::ostream *os, const std::string &context) :
     os_(os), context_(context) {
    }

    void moveTo(const Point &p) override {
      *os_ << "  " << context_ << ".moveTo(" << p.x << ", " << p.y << ");\n";
    }

    void lineTo(const Point &p) override {
      *os_ << "  " << context_ << ".lineTo(" << p.x << ", " << p.y << ");\n";
    }

    void quadTo(const Point &p1, const Point &p2) override {
      *os_ << "  " << context_ << ".quadTo(" <<
              p1.x << ", " << p1.y << ", " << p2.x << ", " << p2.y << ");\n";
    }

    void curveTo(const Point &p1, const Point &p2, const Point &p3) override {
      *os_ << "  " << context_ << ".curveTo(" << p1.x << ", " << p1.y << ", " <<
              p2.x << ", " << p2.y << ", " << p3.x << ", " << p3.y << ");\n";
    }

   private:
    std::ostream *os_ { nullptr };
    std::string   context_;
  };

  PathVisitor visitor(os_, context());

  CQChartsDrawUtil::visitPath(path, visitor);

  //*os_ << "  " << context() << ".gc.closePath();\n";
}

void
CQChartsScriptPaintDevice::
fillRect(const BBox &bbox)
{
  *os_ << "  " << context() << ".fillRect(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ");\n";
}

void
CQChartsScriptPaintDevice::
drawRect(const BBox &bbox)
{
  *os_ << "  " << context() << ".drawRect(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ");\n";
}

void
CQChartsScriptPaintDevice::
drawEllipse(const BBox &bbox, const CQChartsAngle &)
{
  *os_ << "  " << context() << ".drawEllipse(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ");\n";
}

#if 0
void
CQChartsScriptPaintDevice::
drawArc(const BBox &bbox, const CQChartsAngle &a1, const CQChartsAngle &a2)
{
  *os_ << "  " << context() << ".drawEllipse(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ", " <<
          a1.value() << ", " << a2.value() << ");\n";
}
#endif

void
CQChartsScriptPaintDevice::
drawPolygon(const Polygon &poly)
{
  *os_ << "  " << context() << ".drawPolygon([";

  int np = poly.size();

  for (int i = 0; i < np; ++i) {
    if (i > 0) *os_ << ", ";

    const auto &p = poly.point(i);

    *os_ << p.x << ", " << p.y;
  }

  *os_ << "]);\n";
}

void
CQChartsScriptPaintDevice::
drawPolyline(const Polygon &poly)
{
  *os_ << "  " << context() << ".drawPolyline([";

  int np = poly.size();

  for (int i = 0; i < np; ++i) {
    if (i > 0) *os_ << ", ";

    const auto &p = poly.point(i);

    *os_ << p.x << ", " << p.y;
  }

  *os_ << "]);\n";
}

void
CQChartsScriptPaintDevice::
drawLine(const Point &p1, const Point &p2)
{
  *os_ << "  " << context() << ".drawLine(" <<
    p1.x << ", " << p1.y << ", " << p2.x << ", " << p2.y << ");\n";
}

void
CQChartsScriptPaintDevice::
drawPoint(const Point &p)
{
  *os_ << "  " << context() << ".drawPoint(" << p.x << ", " << p.y << ");\n";
}

void
CQChartsScriptPaintDevice::
drawText(const Point &p, const QString &text)
{
  *os_ << "  " << context() << ".drawText(" << p.x << ", " << p.y << ", \"" <<
    text.toStdString() << "\");\n";
}

void
CQChartsScriptPaintDevice::
drawTransformedText(const Point &p, const QString &text)
{
  Point pt(p.x + data_.transformPoint.x, p.y + data_.transformPoint.y);

  double a = CMathUtil::Deg2Rad(data_.transformAngle);

  *os_ << "  " << context() << ".drawRotatedText(" << pt.x << ", " << pt.y << ", \"" <<
    text.toStdString() << "\", " << a << ");\n";
}

void
CQChartsScriptPaintDevice::
drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool /*stretch*/, const Angle &)
{
  double x = (! isInvertX() ? bbox.getXMin() : bbox.getXMax());
  double y = (! isInvertY() ? bbox.getYMax() : bbox.getYMin());

  auto pbbox = windowToPixel(bbox);

  double w = pbbox.getWidth ();
  double h = pbbox.getHeight();

  auto qimage = image.sizedImage(int(w), int(h));

  drawImage(Point(x, y), qimage);
}

void
CQChartsScriptPaintDevice::
drawImage(const Point &p, const QImage &image)
{
  // writes image into ba in PNG format
  QByteArray ba;
  QBuffer qbuffer(&ba);
  qbuffer.open(QIODevice::WriteOnly);
  image.save(&qbuffer, "PNG");

  *os_ << "  var imageData=\"data:image/png;base64,";

  std::vector<unsigned char> buffer;

  auto ba64 = ba.toBase64();

  *os_ << ba64.constData();

  *os_ << "\";\n";

  *os_ << "  var image = new Image();\n";
  *os_ << "  image.src = imageData;\n";

  *os_ << "\n";
  *os_ << "  var px = " << context() << ".plotXToPixel(" << p.x << ");\n";
  *os_ << "  var py = " << context() << ".plotYToPixel(" << p.y << ");\n";
  *os_ << "\n";

  *os_ << "  charts.gc.drawImage(image, px, py);\n";
}

void
CQChartsScriptPaintDevice::
setFont(const QFont &f)
{
  if (! data_.hasFont || f.pointSizeF() != data_.font.pointSizeF())
    *os_ << "  " << context() << ".setFont(" << f.pointSizeF() << ");\n";

  data_.font    = f;
  data_.hasFont = true;
}

void
CQChartsScriptPaintDevice::
setTransform(const QTransform &t, bool combine)
{
  if (combine)
    data_.transform *= t;
  else
    data_.transform = t;

#if 0
  *os_ << "  " << context() << ".gc.setTransform(" <<
    data_.transform.m11() << ", " << data_.transform.m21() << ", " <<
    data_.transform.m12() << ", " << data_.transform.m22() << ", " <<
    data_.transform.dx () << ", " << data_.transform.dy () << ");\n";
#endif
}

std::string
CQChartsScriptPaintDevice::
context() const
{
  return (context_.size() ? context_ : "this");
}

void
CQChartsScriptPaintDevice::
setContext(const std::string &context)
{
  context_ = context;
}
