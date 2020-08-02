#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsEnv.h>
#include <CQHandDrawnPainter.h>
#include <QBuffer>

CQChartsPixelPaintDevice::
CQChartsPixelPaintDevice(QPainter *painter) :
 CQChartsViewPlotPaintDevice(painter)
{
}

//---

CQChartsViewPaintDevice::
CQChartsViewPaintDevice(CQChartsView *view, QPainter *painter) :
 CQChartsViewPlotPaintDevice(view, painter)
{
}

//---

CQChartsPlotPaintDevice::
CQChartsPlotPaintDevice(CQChartsPlot *plot, QPainter *painter) :
 CQChartsViewPlotPaintDevice(plot, painter)
{
}

//---

CQChartsViewPlotPaintDevice::
CQChartsViewPlotPaintDevice(CQChartsView *view, QPainter *painter) :
 CQChartsPaintDevice(view), painter_(painter)
{
  setHandDrawn    (view->isHandDrawn());
  setHandRoughness(view->handRoughness());
  setHandFillDelta(view->handFillDelta());
}

CQChartsViewPlotPaintDevice::
CQChartsViewPlotPaintDevice(CQChartsPlot *plot, QPainter *painter) :
 CQChartsPaintDevice(plot), painter_(painter)
{
  setHandDrawn    (plot->view()->isHandDrawn());
  setHandRoughness(plot->view()->handRoughness());
  setHandFillDelta(plot->view()->handFillDelta());
}

CQChartsViewPlotPaintDevice::
CQChartsViewPlotPaintDevice(QPainter *painter) :
 CQChartsPaintDevice(), painter_(painter)
{
}

CQChartsViewPlotPaintDevice::
~CQChartsViewPlotPaintDevice()
{
  delete hdPainter_;
}

//---

void
CQChartsViewPlotPaintDevice::
setHandDrawn(bool b)
{
  handDrawn_ = b;

  if (handDrawn_) {
    if (! hdPainter_)
      hdPainter_ = new CQHandDrawnPainter(painter_);

    hdPainter_->setRoughness(handRoughness());
    hdPainter_->setFillDelta(handFillDelta());
  }
  else {
    if (hdPainter_) {
      delete hdPainter_;

      hdPainter_ = nullptr;
    }
  }
}

void
CQChartsViewPlotPaintDevice::
setHandRoughness(double r)
{
  handRoughness_ = r;

  if (hdPainter_)
    hdPainter_->setRoughness(r);
}

void
CQChartsViewPlotPaintDevice::
setHandFillDelta(double r)
{
  handFillDelta_ = r;

  if (hdPainter_)
    hdPainter_->setFillDelta(r);
}

//---

void
CQChartsViewPlotPaintDevice::
save()
{
  painter_->save();
}

void
CQChartsViewPlotPaintDevice::
restore()
{
  painter_->restore();

  clipPath_ = QPainterPath();
  clipRect_ = BBox();
}

void
CQChartsViewPlotPaintDevice::
setClipPath(const QPainterPath &path, Qt::ClipOperation operation)
{
  clipPath_ = path;
  clipRect_ = BBox();

  auto ppath = windowToPixel(path);

  painter_->setClipPath(ppath, operation);
}

void
CQChartsViewPlotPaintDevice::
setClipRect(const BBox &bbox, Qt::ClipOperation operation)
{
  if (! bbox.isValid()) return;

  clipRect_ = bbox;
  clipPath_ = QPainterPath();

  auto pbbox = windowToPixel(bbox);

  painter_->setClipRect(pbbox.qrect(), operation);
}

CQChartsGeom::BBox
CQChartsViewPlotPaintDevice::
clipRect() const
{
  if      (clipRect_.isValid())
    return clipRect_;
  else if (clipPath_.isEmpty())
    return BBox(clipPath_.boundingRect());
  else
    return BBox();
}

QPen
CQChartsViewPlotPaintDevice::
pen() const
{
  return painter_->pen();
}

void
CQChartsViewPlotPaintDevice::
setPen(const QPen &pen)
{
  painter_->setPen(pen);
}

QBrush
CQChartsViewPlotPaintDevice::
brush() const
{
  return painter_->brush();
}

void
CQChartsViewPlotPaintDevice::
setBrush(const QBrush &brush)
{
  painter_->setBrush(brush);
}

void
CQChartsViewPlotPaintDevice::
setAltColor(const QColor &c)
{
  if (c.isValid()) {
    painter_->setBackgroundMode(Qt::OpaqueMode);
    painter_->setBackground(c);
  }
  else {
    painter_->setBackgroundMode(Qt::TransparentMode);
    painter_->setBackground(c);
  }

  if (handDrawn_)
    hdPainter_->setAltColor(c);
}

void
CQChartsViewPlotPaintDevice::
setFillAngle(double a)
{
  if (handDrawn_) {
    if (a > 0.0)
      hdPainter_->setFillAngle(a);
    else
      hdPainter_->setFillAngle(45.0);
  }
}

void
CQChartsViewPlotPaintDevice::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  auto ppath = windowToPixel(path);

  if (handDrawn_)
    hdPainter_->fillPath(ppath, brush);
  else
    painter_->fillPath(ppath, brush);
}

void
CQChartsViewPlotPaintDevice::
strokePath(const QPainterPath &path, const QPen &pen)
{
  auto ppath = windowToPixel(path);

  if (handDrawn_)
    hdPainter_->strokePath(ppath, pen);
  else
    painter_->strokePath(ppath, pen);
}

void
CQChartsViewPlotPaintDevice::
drawPath(const QPainterPath &path)
{
  auto ppath = windowToPixel(path);

  if (handDrawn_)
    hdPainter_->drawPath(ppath);
  else
    painter_->drawPath(ppath);
}

void
CQChartsViewPlotPaintDevice::
fillRect(const BBox &bbox)
{
  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  if (handDrawn_)
    hdPainter_->fillRect(pbbox.qrect(), this->brush());
  else
    painter_->fillRect(pbbox.qrect(), this->brush());
}

void
CQChartsViewPlotPaintDevice::
drawRect(const BBox &bbox)
{
  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  if (handDrawn_)
    hdPainter_->drawRect(pbbox.qrect());
  else
    painter_->drawRect(pbbox.qrect());
}

void
CQChartsViewPlotPaintDevice::
drawEllipse(const BBox &bbox, const CQChartsAngle &a)
{
  auto pbbox = windowToPixel(bbox);

  QRectF prect = pbbox.qrect();
  if (! prect.isValid()) return;

  if (handDrawn_) {
    // TODO: rotated ellipsse
    //assert(false);

    QPainterPath path;

    path.addEllipse(prect);

    hdPainter_->drawPath(path);
  }
  else {
    if (a.value() != 0.0) {
      painter_->save();

      painter_->translate(prect.center());
      painter_->rotate   (-a.value());
      painter_->translate(-prect.center());

      painter_->drawEllipse(prect);

      painter_->restore();
    }
    else {
      painter_->drawEllipse(prect);
    }
  }
}

#if 0
void
CQChartsViewPlotPaintDevice::
drawArc(const BBox &rect, const CQChartsAngle &a1, const CQChartsAngle &a2)
{
  auto pbbox = windowToPixel(bbox);

  QRectF prect = pbbox.qrect();
  if (! prect.isValid()) return;

  if (handDrawn_)
    assert(false);
  else
    painter_->drawArc(prect, a1.value()*16, a2.value()*16);
}
#endif

void
CQChartsViewPlotPaintDevice::
drawPolygon(const Polygon &poly)
{
  auto ppoly = windowToPixel(poly);

  if (handDrawn_)
    hdPainter_->drawPolygon(ppoly.qpoly());
  else
    painter_->drawPolygon(ppoly.qpoly());
}

void
CQChartsViewPlotPaintDevice::
drawPolyline(const Polygon &poly)
{
  auto ppoly = windowToPixel(poly);

  if (handDrawn_)
    hdPainter_->drawPolyline(ppoly.qpoly());
  else
    painter_->drawPolyline(ppoly.qpoly());
}

void
CQChartsViewPlotPaintDevice::
drawLine(const Point &p1, const Point &p2)
{
  auto pp1 = windowToPixel(p1);
  auto pp2 = windowToPixel(p2);

  if (handDrawn_)
    hdPainter_->drawLine(pp1.qpoint(), pp2.qpoint());
  else
    painter_->drawLine(pp1.qpoint(), pp2.qpoint());
}

void
CQChartsViewPlotPaintDevice::
drawPoint(const Point &p)
{
  auto pp = windowToPixel(p);

  if (handDrawn_)
    hdPainter_->drawPoint(pp.qpoint());
  else
    painter_->drawPoint(pp.qpoint());
}

void
CQChartsViewPlotPaintDevice::
drawText(const Point &p, const QString &text)
{
  auto pp = windowToPixel(p);

  if (handDrawn_)
    hdPainter_->drawText(pp.qpoint(), text);
  else
    painter_->drawText(pp.qpoint(), text);
}

void
CQChartsViewPlotPaintDevice::
drawTransformedText(const Point &p, const QString &text)
{
  // NOTE: p is in pixels
  if (handDrawn_)
    hdPainter_->drawText(p.qpoint(), text);
  else
    painter_->drawText(p.qpoint(), text);
}

void
CQChartsViewPlotPaintDevice::
drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool stretch)
{
  if (! bbox.isSet()) return;

  auto pbbox = windowToPixel(bbox);

  double w = pbbox.getWidth ();
  double h = pbbox.getHeight();

  BBox pbbox1;

  if (! stretch) {
    int iw = image.width ();
    int ih = image.height();

    double xs = (iw > 0 ? w/iw : 1.0);
    double ys = (ih > 0 ? h/ih : 1.0);

    double w1, h1;

    if (xs < ys) {
      w1 = xs*iw;
      h1 = xs*ih;
    }
    else {
      w1 = ys*iw;
      h1 = ys*ih;
    }

    pbbox1 = BBox(pbbox.getXMid() - w1/2.0, pbbox.getYMid() - h1/2.0,
                  pbbox.getXMid() + w1/2.0, pbbox.getYMid() + h1/2.0);
  }
  else {
    pbbox1 = pbbox;
  }

  QImage qimage = image.sizedImage(int(w), int(h));

  if (handDrawn_)
    hdPainter_->drawImage(pbbox1.qrect(), qimage);
  else
    painter_->drawImage(pbbox1.qrect(), qimage);
}

void
CQChartsViewPlotPaintDevice::
drawImage(const Point &p, const QImage &image)
{
  auto pp = windowToPixel(p);

  if (handDrawn_)
    hdPainter_->drawImage(pp.qpoint(), image);
  else
    painter_->drawImage(pp.qpoint(), image);
}

const QFont &
CQChartsViewPlotPaintDevice::
font() const
{
  return painter_->font();
}

void
CQChartsViewPlotPaintDevice::
setFont(const QFont &f)
{
  painter_->setFont(f);
}

void
CQChartsViewPlotPaintDevice::
setTransformRotate(const Point &p, double angle)
{
  auto p1 = windowToPixel(p);

  QTransform t = painter_->transform();

  t.translate(p1.x, p1.y);
  t.rotate(-angle);

  painter_->setTransform(t);
}

const QTransform &
CQChartsViewPlotPaintDevice::
transform() const
{
  return painter_->transform();
}

void
CQChartsViewPlotPaintDevice::
setTransform(const QTransform &t, bool combine)
{
  painter_->setTransform(t, combine);
}

void
CQChartsViewPlotPaintDevice::
setRenderHints(QPainter::RenderHints hints, bool on)
{
  painter_->setRenderHints(hints, on);
}
