#include <CQChartsPaintDevice.h>
#include <QBuffer>

QRectF
CQChartsPaintDevice::
windowToPixel(const QRectF &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->windowToPixel(r)) : view_->windowToPixel(r));
}

QRectF
CQChartsPaintDevice::
pixelToWindow(const QRectF &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->pixelToWindow(r)) : view_->pixelToWindow(r));
}

QPointF
CQChartsPaintDevice::
windowToPixel(const QPointF &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

QPolygonF
CQChartsPaintDevice::
windowToPixel(const QPolygonF &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

QPointF
CQChartsPaintDevice::
pixelToWindow(const QPointF &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->pixelToWindow(p)) : view_->pixelToWindow(p));
}

QSizeF
CQChartsPaintDevice::
pixelToWindowSize(const QSizeF &s) const
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

QPainterPath
CQChartsPaintDevice::
windowToPixel(const QPainterPath &path) const
{
  return (! view_ ? (! plot_ ? path : plot_->windowToPixel(path)) : view_->windowToPixel(path));
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
}

void
CQChartsViewPlotPainter::
setClipPath(const QPainterPath &path, Qt::ClipOperation operation)
{
  QPainterPath ppath = windowToPixel(path);

  painter_->setClipPath(ppath, operation);
}

void
CQChartsViewPlotPainter::
setClipRect(const QRectF &rect, Qt::ClipOperation operation)
{
  QRectF prect = windowToPixel(rect);

  painter_->setClipRect(prect, operation);
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
fillRect(const QRectF &rect, const QBrush &brush)
{
  QRectF prect = windowToPixel(rect);

  painter_->fillRect(prect, brush);
}

void
CQChartsViewPlotPainter::
drawRect(const QRectF &rect)
{
  QRectF prect = windowToPixel(rect);

  painter_->drawRect(prect);
}

void
CQChartsViewPlotPainter::
drawEllipse(const QRectF &rect)
{
  QRectF prect = windowToPixel(rect);

  painter_->drawEllipse(prect);
}

#if 0
void
CQChartsViewPlotPainter::
drawArc(const QRectF &rect, double a1, double a2)
{
  QRectF prect = windowToPixel(rect);

  painter_->drawArc(prect, a1*16, a2*16);
}
#endif

void
CQChartsViewPlotPainter::
drawPolygon(const QPolygonF &poly)
{
  QPolygonF ppoly = windowToPixel(poly);

  painter_->drawPolygon(ppoly);
}

void
CQChartsViewPlotPainter::
drawPolyline(const QPolygonF &poly)
{
  QPolygonF ppoly;

  int np = poly.size();

  for (int i = 0; i < np; ++i)
    ppoly << windowToPixel(poly[i]);

  painter_->drawPolyline(ppoly);
}

void
CQChartsViewPlotPainter::
drawLine(const QPointF &p1, const QPointF &p2)
{
  QPointF pp1 = windowToPixel(p1);
  QPointF pp2 = windowToPixel(p2);

  painter_->drawLine(pp1, pp2);
}

void
CQChartsViewPlotPainter::
drawPoint(const QPointF &p)
{
  QPointF pp = windowToPixel(p);

  painter_->drawPoint(pp);
}

void
CQChartsViewPlotPainter::
drawText(const QPointF &p, const QString &text)
{
  QPointF pp = windowToPixel(p);

  painter_->drawText(pp, text);
}

void
CQChartsViewPlotPainter::
drawTransformedText(const QPointF &p, const QString &text)
{
  painter_->drawText(p, text);
}

void
CQChartsViewPlotPainter::
drawImageInRect(const QRectF &rect, const QImage &image)
{
  QRectF prect = windowToPixel(rect);

  double w = prect.width ();
  double h = prect.height();

  painter_->drawImage(prect, image.scaled(w, h, Qt::IgnoreAspectRatio));
}

void
CQChartsViewPlotPainter::
drawImage(const QPointF &p, const QImage &image)
{
  QPointF pp = windowToPixel(p);

  painter_->drawImage(pp, image);
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
setTransformRotate(const QPointF &p, double angle)
{
  QPointF p1 = windowToPixel(p);

  QTransform t = painter_->transform();

  t.translate(p1.x(), p1.y());
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

CQChartsScriptPainter::
CQChartsScriptPainter(CQChartsView *view, std::ostream &os) :
 CQChartsPaintDevice(view), os_(&os)
{
}

CQChartsScriptPainter::
CQChartsScriptPainter(CQChartsPlot *plot, std::ostream &os) :
 CQChartsPaintDevice(plot), os_(&os)
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
setClipRect(const QRectF &rect, Qt::ClipOperation op)
{
  *os_ << "  charts.setClipRect(" <<
          rect.left () << ", " << rect.bottom() << ", " <<
          rect.right() << ", " << rect.top   () << ", " << int(op) << ");\n";
}

QPen
CQChartsScriptPainter::
pen() const
{
  return data_.pen;
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

QBrush
CQChartsScriptPainter::
brush() const
{
  return data_.brush;
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
resetData()
{
  data_.reset();
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
fillRect(const QRectF &rect, const QBrush &brush)
{
  setBrush(brush);

  *os_ << "  " << context() << ".fillRect(" <<
          rect.left () << ", " << rect.bottom() << ", " <<
          rect.right() << ", " << rect.top   () << ");\n";
}

void
CQChartsScriptPainter::
drawRect(const QRectF &rect)
{
  *os_ << "  " << context() << ".drawRect(" <<
          rect.left () << ", " << rect.bottom() << ", " <<
          rect.right() << ", " << rect.top   () << ");\n";
}

void
CQChartsScriptPainter::
drawEllipse(const QRectF &rect)
{
  *os_ << "  " << context() << ".drawEllipse(" <<
          rect.left () << ", " << rect.bottom() << ", " <<
          rect.right() << ", " << rect.top   () << ");\n";
}

#if 0
void
CQChartsScriptPainter::
drawArc(const QRectF &rect, double a1, double a2)
{
  *os_ << "  " << context() << ".drawEllipse(" <<
          rect.left () << ", " << rect.bottom() << ", " <<
          rect.right() << ", " << rect.top   () << ", " <<
          a1 << ", " << a2 << ");\n";
}
#endif

void
CQChartsScriptPainter::
drawPolygon(const QPolygonF &poly)
{
  *os_ << "  " << context() << ".drawPolygon([";

  int np = poly.size();

  for (int i = 0; i < np; ++i) {
    if (i > 0) *os_ << ", ";

    *os_ << poly[i].x() << ", " << poly[i].y();
  }

  *os_ << "]);\n";
}

void
CQChartsScriptPainter::
drawPolyline(const QPolygonF &poly)
{
  *os_ << "  " << context() << ".drawPolyline([";

  int np = poly.size();

  for (int i = 0; i < np; ++i) {
    if (i > 0) *os_ << ", ";

    *os_ << poly[i].x() << ", " << poly[i].y();
  }

  *os_ << "]);\n";
}

void
CQChartsScriptPainter::
drawLine(const QPointF &p1, const QPointF &p2)
{
  *os_ << "  " << context() << ".drawLine(" <<
    p1.x() << ", " << p1.y() << ", " << p2.x() << ", " << p2.y() << ");\n";
}

void
CQChartsScriptPainter::
drawPoint(const QPointF &p)
{
  *os_ << "  " << context() << ".drawPoint(" << p.x() << ", " << p.y() << ");\n";
}

void
CQChartsScriptPainter::
drawText(const QPointF &p, const QString &text)
{
  *os_ << "  " << context() << ".drawText(" << p.x() << ", " << p.y() << ", \"" <<
     text.toStdString() << "\");\n";
}

void
CQChartsScriptPainter::
drawTransformedText(const QPointF &p, const QString &text)
{
  QPointF pt = p +  data_.transformPoint;

  double a = CMathUtil::Deg2Rad(data_.transformAngle);

  *os_ << "  " << context() << ".drawRotatedText(" << pt.x() << ", " << pt.y() << ", \"" <<
     text.toStdString() << "\", " << a << ");\n";
}

void
CQChartsScriptPainter::
drawImageInRect(const QRectF &rect, const QImage &image)
{
  double x = rect.left  ();
  double y = rect.bottom();

  drawImage(QPointF(x, y), image);
}

void
CQChartsScriptPainter::
drawImage(const QPointF &p, const QImage &image)
{
  int iw = image.width ();
  int ih = image.height();

  *os_ << "  var iwidth = "  << iw << ";\n";
  *os_ << "  var iheight = " << ih << ";\n";
  *os_ << "\n";
  *os_ << "  var idata = " << context() << ".gc.createImageData(iwidth, iheight);\n";
  *os_ << "\n";

  int i = 0;

  for (int y = 0; y < ih; ++y) {
    for (int x = 0; x < iw; ++x, i += 4) {
      QRgb rgb = image.pixel(x, y);

      int a = qAlpha(rgb);

      if (a != 0) {
        int r = qRed  (rgb);
        int g = qGreen(rgb);
        int b = qBlue (rgb);

        if (r != 0) *os_ << "  idata.data[" << i     << "]=" << r << ";\n";
        if (g != 0) *os_ << "  idata.data[" << i + 1 << "]=" << g << ";\n";
        if (b != 0) *os_ << "  idata.data[" << i + 2 << "]=" << b << ";\n";

        *os_ << "  idata.data[" << i + 3 << "]=" << a << ";\n";
      }
    }
  }

  *os_ << "\n";
  *os_ << "  var px = " << context() << ".plotXToPixel(" << p.x() << ");\n";
  *os_ << "  var py = " << context() << ".plotYToPixel(" << p.y() << ");\n";
  *os_ << "\n";
  *os_ << "  " << context() << ".gc.putImageData(idata, px, py);\n";
}

const QFont &
CQChartsScriptPainter::
font() const
{
  return data_.font;
}

void
CQChartsScriptPainter::
setFont(const QFont &f)
{
  if (! data_.hasFont || f.pointSizeF() != data_.font.pointSizeF())
    *os_ << "  " << context() << ".gc.font = \"" << f.pointSizeF() << "px Sans\";\n";

  data_.font    = f;
  data_.hasFont = true;
}

void
CQChartsScriptPainter::
setTransformRotate(const QPointF &p, double angle)
{
  QTransform t = data_.transform;

  t.translate(p.x(), p.y());
  t.rotate(angle);

  data_.transform      = t;
  data_.transformPoint = p;
  data_.transformAngle = angle;
}

const QTransform &
CQChartsScriptPainter::
transform() const
{
  return data_.transform;
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

void
CQChartsScriptPainter::
setRenderHints(QPainter::RenderHints, bool)
{
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

void
CQChartsScriptPainter::
setColorNames()
{
  setColorNames("strokeColor", "fillColor");
}

void
CQChartsScriptPainter::
setColorNames(const QString &strokeName, const QString &fillName)
{
  setStrokeStyleName(strokeName);
  setFillStyleName  (fillName  );
}

void
CQChartsScriptPainter::
resetColorNames()
{
  setStrokeStyleName("");
  setFillStyleName  ("");
}

//---

CQChartsSVGPainter::
CQChartsSVGPainter(CQChartsView *view, std::ostream &os) :
 CQChartsPaintDevice(view), os_(&os)
{
}

CQChartsSVGPainter::
CQChartsSVGPainter(CQChartsPlot *plot, std::ostream &os) :
 CQChartsPaintDevice(plot), os_(&os)
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
setClipRect(const QRectF &, Qt::ClipOperation)
{
  // TODO
}

QPen
CQChartsSVGPainter::
pen() const
{
  return data_.pen;
}

void
CQChartsSVGPainter::
setPen(const QPen &pen)
{
  data_.pen = pen;
}

QBrush
CQChartsSVGPainter::
brush() const
{
  return data_.brush;
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
resetData()
{
  data_.reset();
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
fillRect(const QRectF &rect, const QBrush &brush)
{
  QRectF prect = windowToPixel(rect);

  setBrush(brush);

  *os_ << "<rect x=\"" << prect.left() << "\" y=\"" << prect.top() << "\" " <<
          "width=\"" << prect.width() << "\" height=\"" << prect.height() << "\"";

  *os_ << " style=\"";

  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
drawRect(const QRectF &rect)
{
  QRectF prect = windowToPixel(rect);

  *os_ << "<rect x=\"" << prect.left() << "\" y=\"" << prect.top() << "\" " <<
          "width=\"" << prect.width() << "\" height=\"" << prect.height() << "\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
drawEllipse(const QRectF &rect)
{
  QRectF prect = windowToPixel(rect);

  *os_ << "<ellipse cx=\"" << prect.center().x() << "\" cy=\"" << prect.center().y() << "\" " <<
          "rx=\"" << prect.width()/2 << " ry=\"" << prect.height()/2 << "\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\">\n";
}

#if 0
void
CQChartsSVGPainter::
drawArc(const QRectF &, double, double)
{
  // TODO
}
#endif

void
CQChartsSVGPainter::
drawPolygon(const QPolygonF &poly)
{
  QPolygonF ppoly = windowToPixel(poly);

  *os_ << "<path d=\"";

  int np = ppoly.size();

  for (int i = 0; i < np; ++i) {
    if (i == 0)
      *os_ << "M " << ppoly[i].x() << " " << poly[i].y();
    else
      *os_ << "L " << ppoly[i].x() << " " << poly[i].y();
  }

  *os_ << "z\"";

  *os_ << " style=\"";

  writePen();
  writeBrush();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
drawPolyline(const QPolygonF &poly)
{
  QPolygonF ppoly = windowToPixel(poly);

  *os_ << "<path d=\"";

  int np = ppoly.size();

  for (int i = 0; i < np; ++i) {
    if (i == 0)
      *os_ << "M " << ppoly[i].x() << " " << poly[i].y();
    else
      *os_ << "L " << ppoly[i].x() << " " << poly[i].y();
  }

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
drawLine(const QPointF &p1, const QPointF &p2)
{
  QPointF pp1 = windowToPixel(p1);
  QPointF pp2 = windowToPixel(p2);

  *os_ << "<line x1=\"" << pp1.x() << "\" y1=\"" << pp1.y() << "\" " <<
                "x2=\"" << pp2.x() << "\" y2=\"" << pp2.y() << "\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
drawPoint(const QPointF &p)
{
  QPointF pp = windowToPixel(p);

  *os_ << "<rect x=\"" << pp.x() << "\" y=\"" << pp.y() << "\" width=\"1\" height=\"1\"";

  *os_ << " style=\"";

  writePen();

  *os_ << "\"/>\n";
}

void
CQChartsSVGPainter::
drawText(const QPointF &p, const QString &text)
{
  QPointF pp = windowToPixel(p);

  *os_ << "<text xml:space=\"preserve\" x=\"" << pp.x() << "\" y=\"" << pp.y() << "\"";

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
drawTransformedText(const QPointF &p, const QString &text)
{
  QPointF pt = p +  data_.transformPoint;

  QPointF ppt = windowToPixel(pt);

  double a = CMathUtil::Deg2Rad(data_.transformAngle);
  double c = std::cos(a);
  double s = std::sin(a);

  *os_ << "<g transform=\"matrix(" << c << "," << -s << "," << s << "," << c << "," <<
          ppt.x() << "," << ppt.y() << ")\">\n";

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
drawImageInRect(const QRectF &rect, const QImage &image)
{
  QRectF prect = windowToPixel(rect);

  double px = prect.left();
  double py = prect.top ();

  QPointF pw = pixelToWindow(QPointF(px, py));

  drawImage(pw, image.scaled(prect.width(), prect.height(), Qt::IgnoreAspectRatio));
}

void
CQChartsSVGPainter::
drawImage(const QPointF &p, const QImage &image)
{
  QPointF pt = windowToPixel(p);

  int w = image.width ();
  int h = image.height();

  // writes image into ba in PNG format
  QByteArray ba;
  QBuffer qbuffer(&ba);
  qbuffer.open(QIODevice::WriteOnly);
  image.save(&qbuffer, "PNG");

  *os_ << "<image x=\"" << pt.x() << "\" y=\"" << pt.y() <<
          "\" width=\"" << w << "\" height=\"" << h << "\" preserveAspectRatio=\"none\" " <<
          "xlink:href=\"data:image/png;base64,";

  std::vector<unsigned char> buffer;

  QByteArray ba64 = ba.toBase64();

  *os_ << ba64.constData();

  *os_ << "\"/>\n";
}

const QFont &
CQChartsSVGPainter::
font() const
{
  return data_.font;
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
setTransformRotate(const QPointF &p, double angle)
{
  QTransform t = data_.transform;

  t.translate(p.x(), p.y());
  t.rotate(angle);

  data_.transform      = t;
  data_.transformPoint = p;
  data_.transformAngle = angle;
}

const QTransform &
CQChartsSVGPainter::
transform() const
{
  return data_.transform;
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
setRenderHints(QPainter::RenderHints, bool)
{
}

void
CQChartsSVGPainter::
startGroup(const QString &id)
{
  *os_ << "<g id=\"" << id.toStdString() << "\">\n";
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
  *os_ << " font-family=\"" << data_.font.family().toStdString() << "\""
          " font-size=\"" << data_.font.pointSizeF() << "\""
          " font-weight=\"" << data_.font.weight() << "\""
          " font-style=\"" << data_.font.styleName().toStdString() << "\"";
}
