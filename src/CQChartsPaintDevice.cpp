#include <CQChartsPaintDevice.h>
#include <QBuffer>

CQChartsGeom::BBox
CQChartsPaintDevice::
windowToPixel(const CQChartsGeom::BBox &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->windowToPixel(r)) : view_->windowToPixel(r));
}

CQChartsGeom::BBox
CQChartsPaintDevice::
pixelToWindow(const CQChartsGeom::BBox &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->pixelToWindow(r)) : view_->pixelToWindow(r));
}

CQChartsGeom::Point
CQChartsPaintDevice::
windowToPixel(const CQChartsGeom::Point &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

CQChartsGeom::Point
CQChartsPaintDevice::
pixelToWindow(const CQChartsGeom::Point &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->pixelToWindow(p)) : view_->pixelToWindow(p));
}

CQChartsGeom::Polygon
CQChartsPaintDevice::
windowToPixel(const CQChartsGeom::Polygon &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

CQChartsGeom::Size
CQChartsPaintDevice::
pixelToWindowSize(const CQChartsGeom::Size &s) const
{
  return (! view_ ? (! plot_ ? s : plot_->pixelToWindowSize(s)) : view_->pixelToWindowSize(s));
}

double
CQChartsPaintDevice::
lengthPixelWidth(const CQChartsLength &w) const
{
  return (! view_ ? (! plot_ ? w.value() :
    plot_->lengthPixelWidth(w)) : view_->lengthPixelWidth(w));
}

double
CQChartsPaintDevice::
lengthPixelHeight(const CQChartsLength &h) const
{
  return (! view_ ? (! plot_ ? h.value() :
    plot_->lengthPixelHeight(h)) : view_->lengthPixelHeight(h));
}

double
CQChartsPaintDevice::
lengthWindowWidth(const CQChartsLength &w) const
{
  return (! view_ ? (! plot_ ? w.value() :
    plot_->lengthPlotWidth(w)) : view_->lengthViewWidth(w));
}

double
CQChartsPaintDevice::
lengthWindowHeight(const CQChartsLength &h) const
{
  return (! view_ ? (! plot_ ? h.value() :
    plot_->lengthPlotHeight(h)) : view_->lengthViewHeight(h));
}

double
CQChartsPaintDevice::
pixelToWindowWidth(double pw) const
{
  return (! view_ ? (! plot_ ? 0.0 :
    plot_->pixelToWindowWidth(pw)) : view_->pixelToWindowWidth(pw));
}

double
CQChartsPaintDevice::
pixelToWindowHeight(double ph) const
{
  return (! view_ ? (! plot_ ? 0.0 :
    plot_->pixelToWindowHeight(ph)) : view_->pixelToWindowHeight(ph));
}

QPainterPath
CQChartsPaintDevice::
windowToPixel(const QPainterPath &path) const
{
  return (! view_ ? (! plot_ ? path : plot_->windowToPixel(path)) : view_->windowToPixel(path));
}

bool
CQChartsPaintDevice::
isInvertX() const
{
  return (plot_ ? plot_->isInvertX() : false);
}

bool
CQChartsPaintDevice::
isInvertY() const
{
  return (plot_ ? plot_->isInvertY() : false);
}

//------

CQChartsPixelPainter::
CQChartsPixelPainter(QPainter *painter) :
 CQChartsViewPlotPainter(painter)
{
}

//---

CQChartsViewPainter::
CQChartsViewPainter(CQChartsView *view, QPainter *painter) :
 CQChartsViewPlotPainter(view, painter)
{
}

//---

CQChartsPlotPainter::
CQChartsPlotPainter(CQChartsPlot *plot, QPainter *painter) :
 CQChartsViewPlotPainter(plot, painter)
{
}

//---

CQChartsViewPlotPainter::
CQChartsViewPlotPainter(CQChartsView *view, QPainter *painter) :
 CQChartsPaintDevice(view), painter_(painter)
{
}

CQChartsViewPlotPainter::
CQChartsViewPlotPainter(CQChartsPlot *plot, QPainter *painter) :
 CQChartsPaintDevice(plot), painter_(painter)
{
}

CQChartsViewPlotPainter::
CQChartsViewPlotPainter(QPainter *painter) :
 CQChartsPaintDevice(), painter_(painter)
{
}

void
CQChartsViewPlotPainter::
save()
{
  painter_->save();
}

void
CQChartsViewPlotPainter::
restore()
{
  painter_->restore();

  clipPath_ = QPainterPath();
  clipRect_ = CQChartsGeom::BBox();
}

void
CQChartsViewPlotPainter::
setClipPath(const QPainterPath &path, Qt::ClipOperation operation)
{
  clipPath_ = path;
  clipRect_ = CQChartsGeom::BBox();

  QPainterPath ppath = windowToPixel(path);

  painter_->setClipPath(ppath, operation);
}

void
CQChartsViewPlotPainter::
setClipRect(const CQChartsGeom::BBox &bbox, Qt::ClipOperation operation)
{
  if (! bbox.isValid()) return;

  clipRect_ = bbox;
  clipPath_ = QPainterPath();

  auto pbbox = windowToPixel(bbox);

  painter_->setClipRect(pbbox.qrect(), operation);
}

CQChartsGeom::BBox
CQChartsViewPlotPainter::
clipRect() const
{
  if      (clipRect_.isValid())
    return clipRect_;
  else if (clipPath_.isEmpty())
    return CQChartsGeom::BBox(clipPath_.boundingRect());
  else
    return CQChartsGeom::BBox();
}

QPen
CQChartsViewPlotPainter::
pen() const
{
  return painter_->pen();
}

void
CQChartsViewPlotPainter::
setPen(const QPen &pen)
{
  painter_->setPen(pen);
}

QBrush
CQChartsViewPlotPainter::
brush() const
{
  return painter_->brush();
}

void
CQChartsViewPlotPainter::
setBrush(const QBrush &brush)
{
  painter_->setBrush(brush);
}

void
CQChartsViewPlotPainter::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  QPainterPath ppath = windowToPixel(path);

  painter_->fillPath(ppath, brush);
}

void
CQChartsViewPlotPainter::
strokePath(const QPainterPath &path, const QPen &pen)
{
  QPainterPath ppath = windowToPixel(path);

  painter_->strokePath(ppath, pen);
}

void
CQChartsViewPlotPainter::
drawPath(const QPainterPath &path)
{
  QPainterPath ppath = windowToPixel(path);

  painter_->drawPath(ppath);
}

void
CQChartsViewPlotPainter::
fillRect(const CQChartsGeom::BBox &bbox, const QBrush &brush)
{
  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  painter_->fillRect(pbbox.qrect(), brush);
}

void
CQChartsViewPlotPainter::
drawRect(const CQChartsGeom::BBox &bbox)
{
  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  painter_->drawRect(pbbox.qrect());
}

void
CQChartsViewPlotPainter::
drawEllipse(const CQChartsGeom::BBox &bbox)
{
  auto pbbox = windowToPixel(bbox);

  QRectF prect = pbbox.qrect();

  if (prect.isValid())
    painter_->drawEllipse(prect);
}

#if 0
void
CQChartsViewPlotPainter::
drawArc(const CQChartsGeom::BBox &rect, const CQChartsAngle &a1, const CQChartsAngle &a2)
{
  auto pbbox = windowToPixel(bbox);

  QRectF prect = pbbox.qrect();

  if (prect.isValid())
    painter_->drawArc(prect, a1.value()*16, a2.value()*16);
}
#endif

void
CQChartsViewPlotPainter::
drawPolygon(const CQChartsGeom::Polygon &poly)
{
  auto ppoly = windowToPixel(poly);

  painter_->drawPolygon(ppoly.qpoly());
}

void
CQChartsViewPlotPainter::
drawPolyline(const CQChartsGeom::Polygon &poly)
{
  auto ppoly = windowToPixel(poly);

  painter_->drawPolyline(ppoly.qpoly());
}

void
CQChartsViewPlotPainter::
drawLine(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2)
{
  auto pp1 = windowToPixel(p1);
  auto pp2 = windowToPixel(p2);

  painter_->drawLine(pp1.qpoint(), pp2.qpoint());
}

void
CQChartsViewPlotPainter::
drawPoint(const CQChartsGeom::Point &p)
{
  auto pp = windowToPixel(p);

  painter_->drawPoint(pp.qpoint());
}

void
CQChartsViewPlotPainter::
drawText(const CQChartsGeom::Point &p, const QString &text)
{
  auto pp = windowToPixel(p);

  painter_->drawText(pp.qpoint(), text);
}

void
CQChartsViewPlotPainter::
drawTransformedText(const CQChartsGeom::Point &p, const QString &text)
{
  // NOTE: p is in pixels
  painter_->drawText(p.qpoint(), text);
}

void
CQChartsViewPlotPainter::
drawImageInRect(const CQChartsGeom::BBox &bbox, const QImage &image)
{
  if (! bbox.isSet()) return;

  auto pbbox = windowToPixel(bbox);

  double w = pbbox.getWidth ();
  double h = pbbox.getHeight();

  painter_->drawImage(pbbox.qrect(), image.scaled(int(w), int(h), Qt::IgnoreAspectRatio));
}

void
CQChartsViewPlotPainter::
drawImage(const CQChartsGeom::Point &p, const QImage &image)
{
  auto pp = windowToPixel(p);

  painter_->drawImage(pp.qpoint(), image);
}

const QFont &
CQChartsViewPlotPainter::
font() const
{
  return painter_->font();
}

void
CQChartsViewPlotPainter::
setFont(const QFont &f)
{
  painter_->setFont(f);
}

void
CQChartsViewPlotPainter::
setTransformRotate(const CQChartsGeom::Point &p, double angle)
{
  auto p1 = windowToPixel(p);

  QTransform t = painter_->transform();

  t.translate(p1.x, p1.y);
  t.rotate(-angle);

  painter_->setTransform(t);
}

const QTransform &
CQChartsViewPlotPainter::
transform() const
{
  return painter_->transform();
}

void
CQChartsViewPlotPainter::
setTransform(const QTransform &t, bool combine)
{
  painter_->setTransform(t, combine);
}

void
CQChartsViewPlotPainter::
setRenderHints(QPainter::RenderHints hints, bool on)
{
  painter_->setRenderHints(hints, on);
}

//---

CQChartsHtmlPainter::
CQChartsHtmlPainter(CQChartsView *view, std::ostream &os) :
 CQChartsPaintDevice(view), os_(&os)
{
}

CQChartsHtmlPainter::
CQChartsHtmlPainter(CQChartsPlot *plot, std::ostream &os) :
 CQChartsPaintDevice(plot), os_(&os)
{
}

QPen
CQChartsHtmlPainter::
pen() const
{
  return data_.pen;
}

QBrush
CQChartsHtmlPainter::
brush() const
{
  return data_.brush;
}

const QFont &
CQChartsHtmlPainter::
font() const
{
  return data_.font;
}

void
CQChartsHtmlPainter::
resetData()
{
  data_.reset();
}

void
CQChartsHtmlPainter::
setTransformRotate(const CQChartsGeom::Point &p, double angle)
{
  QTransform t = data_.transform;

  t.translate(p.x, p.y);
  t.rotate(angle);

  data_.transform      = t;
  data_.transformPoint = p;
  data_.transformAngle = angle;
}

const QTransform &
CQChartsHtmlPainter::
transform() const
{
  return data_.transform;
}

void
CQChartsHtmlPainter::
setRenderHints(QPainter::RenderHints, bool)
{
}

void
CQChartsHtmlPainter::
setColorNames()
{
  setColorNames("strokeColor", "fillColor");
}

void
CQChartsHtmlPainter::
setColorNames(const QString &strokeName, const QString &fillName)
{
  setStrokeStyleName(strokeName);
  setFillStyleName  (fillName  );
}

void
CQChartsHtmlPainter::
resetColorNames()
{
  setStrokeStyleName("");
  setFillStyleName  ("");
}

void
CQChartsHtmlPainter::
createButton(const CQChartsGeom::BBox &bbox, const QString &text, const QString &id,
             const QString &clickProc)
{
  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  *os_ << "<button id=\"" << id.toStdString() << "\" style=\"position:absolute;"
          " left:"  << pbbox.getXMin () << "px; top:"    << pbbox.getYMin  () << "px;"
          " width:" << pbbox.getWidth() << "px; height:" << pbbox.getHeight() << "px;";

  QString styleName = data_.font.styleName();

  if (! styleName.length())
    styleName = "normal";

  *os_ << " font-family:" << data_.font.family().toStdString() << ";" <<
          " font-size:" << data_.font.pointSizeF() << ";" <<
          " font-weight:" << data_.font.weight() << ";" <<
          " font-style:" << styleName.toStdString() << ";";

  *os_ << "\"";

  if (clickProc.length())
    *os_ << " onclick=\"" << clickProc.toStdString() << "('" << id.toStdString() << "')\"";

  *os_ << ">" << text.toStdString() << "</button>\n";
}

//---

CQChartsScriptPainter::
CQChartsScriptPainter(CQChartsView *view, std::ostream &os) :
 CQChartsHtmlPainter(view, os)
{
}

CQChartsScriptPainter::
CQChartsScriptPainter(CQChartsPlot *plot, std::ostream &os) :
 CQChartsHtmlPainter(plot, os)
{
}

void
CQChartsScriptPainter::
save()
{
  dataStack_.push_back(data_);

  *os_ << "  " << context() << ".gc.save();\n";
}

void
CQChartsScriptPainter::
restore()
{
  assert(! dataStack_.empty());

  Data data = dataStack_.back();

  dataStack_.pop_back();

  setPen  (data.pen);
  setBrush(data.brush);

  data_ = data;

  *os_ << "  " << context() << ".gc.restore();\n";
}

void
CQChartsScriptPainter::
setClipPath(const QPainterPath &, Qt::ClipOperation op)
{
  *os_ << "  charts.setClipPath(path, " << int(op) << ");\n";
}

void
CQChartsScriptPainter::
setClipRect(const CQChartsGeom::BBox &bbox, Qt::ClipOperation op)
{
  *os_ << "  charts.setClipRect(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ", " << int(op) << ");\n";
}

void
CQChartsScriptPainter::
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
CQChartsScriptPainter::
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
CQChartsScriptPainter::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  setBrush(brush);

  addPathParts(path);

  *os_ << "  " << context() << ".gc.fill();\n";
}

void
CQChartsScriptPainter::
strokePath(const QPainterPath &path, const QPen &pen)
{
  setPen(pen);

  addPathParts(path);

  *os_ << "  " << context() << ".gc.stroke();\n";
}

void
CQChartsScriptPainter::
drawPath(const QPainterPath &path)
{
  addPathParts(path);

  *os_ << "  " << context() << ".gc.fill();\n";
  *os_ << "  " << context() << ".gc.stroke();\n";
}

void
CQChartsScriptPainter::
addPathParts(const QPainterPath &path)
{
  *os_ << "  " << context() << ".gc.beginPath();\n";

  int n = path.elementCount();

  for (int i = 0; i < n; ++i) {
    const QPainterPath::Element &e = path.elementAt(i);

    if      (e.isMoveTo())
      *os_ << "  " << context() << ".moveTo(" << e.x << ", " << e.y << ");\n";
    else if (e.isLineTo())
      *os_ << "  " << context() << ".lineTo(" << e.x << ", " << e.y << ");\n";
    else if (e.isCurveTo()) {
      QPainterPath::Element     e1, e2;
      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (i < n - 1) {
        e1  = path.elementAt(i + 1);
        e1t = e1.type;
      }

      if (i < n - 2) {
        e2  = path.elementAt(i + 2);
        e2t = e2.type;
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        if (e2t == QPainterPath::CurveToDataElement) {
          *os_ << "  " << context() << ".curveTo(" << e.x << ", " << e.y << ", " <<
                  e1.x << ", " << e1.y << ", " << e2.x << ", " << e2.y << ");\n";

          i += 2;
        }
        else {
          *os_ << "  " << context() << ".quadTo(" <<
                  e.x << ", " << e.y << ", " << e1.x << ", " << e1.y << ");\n";

          ++i;
        }
      }
    }
    else {
      assert(false);
    }
  }

  //*os_ << "  " << context() << ".gc.closePath();\n";
}

void
CQChartsScriptPainter::
fillRect(const CQChartsGeom::BBox &bbox, const QBrush &brush)
{
  setBrush(brush);

  *os_ << "  " << context() << ".fillRect(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ");\n";
}

void
CQChartsScriptPainter::
drawRect(const CQChartsGeom::BBox &bbox)
{
  *os_ << "  " << context() << ".drawRect(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ");\n";
}

void
CQChartsScriptPainter::
drawEllipse(const CQChartsGeom::BBox &bbox)
{
  *os_ << "  " << context() << ".drawEllipse(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ");\n";
}

#if 0
void
CQChartsScriptPainter::
drawArc(const CQChartsGeom::BBox &bbox, const CQChartsAngle &a1, const CQChartsAngle &a2)
{
  *os_ << "  " << context() << ".drawEllipse(" <<
          bbox.getXMin() << ", " << bbox.getYMin() << ", " <<
          bbox.getXMax() << ", " << bbox.getYMax() << ", " <<
          a1.value() << ", " << a2.value() << ");\n";
}
#endif

void
CQChartsScriptPainter::
drawPolygon(const CQChartsGeom::Polygon &poly)
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
CQChartsScriptPainter::
drawPolyline(const CQChartsGeom::Polygon &poly)
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
CQChartsScriptPainter::
drawLine(const CQChartsGeom::Point &p1, const CQChartsGeom::Point &p2)
{
  *os_ << "  " << context() << ".drawLine(" <<
    p1.x << ", " << p1.y << ", " << p2.x << ", " << p2.y << ");\n";
}

void
CQChartsScriptPainter::
drawPoint(const CQChartsGeom::Point &p)
{
  *os_ << "  " << context() << ".drawPoint(" << p.x << ", " << p.y << ");\n";
}

void
CQChartsScriptPainter::
drawText(const CQChartsGeom::Point &p, const QString &text)
{
  *os_ << "  " << context() << ".drawText(" << p.x << ", " << p.y << ", \"" <<
    text.toStdString() << "\");\n";
}

void
CQChartsScriptPainter::
drawTransformedText(const CQChartsGeom::Point &p, const QString &text)
{
  CQChartsGeom::Point pt(p.x + data_.transformPoint.x, p.y + data_.transformPoint.y);

  double a = CMathUtil::Deg2Rad(data_.transformAngle);

  *os_ << "  " << context() << ".drawRotatedText(" << pt.x << ", " << pt.y << ", \"" <<
    text.toStdString() << "\", " << a << ");\n";
}

void
CQChartsScriptPainter::
drawImageInRect(const CQChartsGeom::BBox &bbox, const QImage &image)
{
  double x = (! isInvertX() ? bbox.getXMin() : bbox.getXMax());
  double y = (! isInvertY() ? bbox.getYMax() : bbox.getYMin());

  auto pbbox = windowToPixel(bbox);

  double w = pbbox.getWidth ();
  double h = pbbox.getHeight();

  drawImage(CQChartsGeom::Point(x, y), image.scaled(int(w), int(h), Qt::IgnoreAspectRatio));
}

void
CQChartsScriptPainter::
drawImage(const CQChartsGeom::Point &p, const QImage &image)
{
  // writes image into ba in PNG format
  QByteArray ba;
  QBuffer qbuffer(&ba);
  qbuffer.open(QIODevice::WriteOnly);
  image.save(&qbuffer, "PNG");

  *os_ << "  var imageData=\"data:image/png;base64,";

  std::vector<unsigned char> buffer;

  QByteArray ba64 = ba.toBase64();

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
CQChartsScriptPainter::
setFont(const QFont &f)
{
  if (! data_.hasFont || f.pointSizeF() != data_.font.pointSizeF())
    *os_ << "  " << context() << ".setFont(" << f.pointSizeF() << ");\n";

  data_.font    = f;
  data_.hasFont = true;
}

void
CQChartsScriptPainter::
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
CQChartsScriptPainter::
context() const
{
  return (context_.size() ? context_ : "this");
}

void
CQChartsScriptPainter::
setContext(const std::string &context)
{
  context_ = context;
}

//---

CQChartsSVGPainter::
CQChartsSVGPainter(CQChartsView *view, std::ostream &os) :
 CQChartsHtmlPainter(view, os)
{
}

CQChartsSVGPainter::
CQChartsSVGPainter(CQChartsPlot *plot, std::ostream &os) :
 CQChartsHtmlPainter(plot, os)
{
}

void
CQChartsSVGPainter::
save()
{
  dataStack_.push_back(data_);
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
setClipPath(const QPainterPath &, Qt::ClipOperation)
{
  // TODO
}

void
CQChartsSVGPainter::
setClipRect(const CQChartsGeom::BBox &, Qt::ClipOperation)
{
  // TODO
}

void
CQChartsSVGPainter::
setPen(const QPen &pen)
{
  data_.pen = pen;
}

void
CQChartsSVGPainter::
setBrush(const QBrush &brush)
{
  data_.brush = brush;
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
fillRect(const CQChartsGeom::BBox &bbox, const QBrush &brush)
{
  auto pbbox = windowToPixel(bbox);

  setBrush(brush);

  *os_ << "<rect x=\"" << pbbox.getXMin() << "\" y=\"" << pbbox.getYMin() << "\" " <<
          "width=\"" << pbbox.getWidth() << "\" height=\"" << pbbox.getHeight() << "\"";

  *os_ << " style=\"";

  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
drawEllipse(const CQChartsGeom::BBox &bbox)
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
CQChartsSVGPainter::
drawArc(const CQChartsGeom::BBox &, const CQChartsAngle &, const CQChartsAngle &)
{
  // TODO
}
#endif

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
drawPoint(const CQChartsGeom::Point &p)
{
  auto pp = windowToPixel(p);

  *os_ << "<rect x=\"" << pp.x << "\" y=\"" << pp.y << "\" width=\"1\" height=\"1\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
drawImageInRect(const CQChartsGeom::BBox &bbox, const QImage &image)
{
  auto pbbox = windowToPixel(bbox);

  double px = pbbox.getXMin();
  double py = pbbox.getYMin();

  auto pw = pixelToWindow(CQChartsGeom::Point(px, py));

  drawImage(pw, image.scaled(int(pbbox.getWidth()), int(pbbox.getHeight()), Qt::IgnoreAspectRatio));
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
setFont(const QFont &f)
{
  data_.font    = f;
  data_.hasFont = true;
}

void
CQChartsSVGPainter::
setTransform(const QTransform &t, bool combine)
{
  if (combine)
    data_.transform *= t;
  else
    data_.transform = t;
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
endGroup()
{
  *os_ << "</g>\n";
}

void
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
CQChartsSVGPainter::
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
