#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsEnv.h>
#include <CQHandDrawnPainter.h>
#include <QBitmap>
#include <QBuffer>

CQChartsViewPaintDevice::
CQChartsViewPaintDevice(View *view, QPainter *painter) :
 CQChartsViewPlotPaintDevice(view, painter)
{
}

//---

CQChartsPlotPaintDevice::
CQChartsPlotPaintDevice(Plot *plot, QPainter *painter) :
 CQChartsViewPlotPaintDevice(plot, painter)
{
}

//---

CQChartsViewPlotPaintDevice::
CQChartsViewPlotPaintDevice(View *view, QPainter *painter) :
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
  ++saveDepth_;

  if (painter_)
    painter_->save();
}

void
CQChartsViewPlotPaintDevice::
restore()
{
  assert(saveDepth_ > 0);

  if (painter_)
    painter_->restore();

  clipPath_ = QPainterPath();
  clipRect_ = BBox();

  --saveDepth_;
}

void
CQChartsViewPlotPaintDevice::
setClipPath(const QPainterPath &path, Qt::ClipOperation operation)
{
  assert(saveDepth_ > 0);

  clipPath_ = path;
  clipRect_ = BBox();

  if (painter_) {
    auto ppath = windowToPixel(path);

    painter_->setClipPath(ppath, operation);
  }
}

void
CQChartsViewPlotPaintDevice::
setClipRect(const BBox &bbox, Qt::ClipOperation operation)
{
  assert(saveDepth_ > 0);

  if (! bbox.isValid()) return;

  clipRect_ = bbox;
  clipPath_ = QPainterPath();

  if (painter_) {
    auto pbbox = windowToPixel(bbox);

    painter_->setClipRect(pbbox.qrect(), operation);
  }
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
  return (painter_ ? painter_->pen() : QPen());
}

void
CQChartsViewPlotPaintDevice::
setPen(const QPen &pen)
{
  if (painter_)
    painter_->setPen(pen);
}

QBrush
CQChartsViewPlotPaintDevice::
brush() const
{
  return (painter_ ? painter_->brush() : QBrush());
}

void
CQChartsViewPlotPaintDevice::
setBrush(const QBrush &brush)
{
  if (painter_)
    painter_->setBrush(brush);
}

void
CQChartsViewPlotPaintDevice::
setAltColor(const QColor &c)
{
  altColor_ = c;

  if (isHandDrawn())
    hdPainter_->setAltColor(c);

  updateBackground();
}

void
CQChartsViewPlotPaintDevice::
setAltAlpha(double a)
{
  altAlpha_ = a;

  //if (isHandDrawn())
  //  hdPainter_->setAltAlpha(a);

  updateBackground();
}

void
CQChartsViewPlotPaintDevice::
updateBackground()
{
  if (! painter_)
    return;

  auto c = altColor_;

  if (c.isValid() && c.alpha()) {
    c.setAlphaF(altAlpha_);

    painter_->setBackgroundMode(Qt::OpaqueMode);
    painter_->setBackground(c);
  }
  else {
    painter_->setBackgroundMode(Qt::TransparentMode);
    painter_->setBackground(c);
  }
}

void
CQChartsViewPlotPaintDevice::
setFillAngle(double a)
{
  if (isHandDrawn()) {
    if (a > 0.0)
      hdPainter_->setFillAngle(a);
    else
      hdPainter_->setFillAngle(45.0);
  }

  fillAngle_ = a;
}

void
CQChartsViewPlotPaintDevice::
setFillType(CQChartsFillPattern::Type type)
{
  fillType_ = type;
}

void
CQChartsViewPlotPaintDevice::
setFillRadius(double r)
{
  fillRadius_ = r;
}

void
CQChartsViewPlotPaintDevice::
setFillDelta(double r)
{
  fillDelta_ = r;
}

void
CQChartsViewPlotPaintDevice::
setFillWidth(double w)
{
  fillWidth_ = w;
}

void
CQChartsViewPlotPaintDevice::
fillPath(const QPainterPath &path, const QBrush &brush)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto ppath = windowToPixel(path);

  QBrush brush1;

  if (adjustFillBrush(brush, BBox(ppath.boundingRect()), brush1))
    painter_->setBrushOrigin(ppath.boundingRect().topLeft());

  if (isHandDrawn())
    hdPainter_->fillPath(ppath, brush1);
  else
    painter_->fillPath(ppath, brush1);
}

void
CQChartsViewPlotPaintDevice::
strokePath(const QPainterPath &path, const QPen &pen)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto ppath = windowToPixel(path);

  if (isHandDrawn())
    hdPainter_->strokePath(ppath, pen);
  else
    painter_->strokePath(ppath, pen);
}

void
CQChartsViewPlotPaintDevice::
drawPath(const QPainterPath &path)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto ppath = windowToPixel(path);

  if (isHandDrawn())
    hdPainter_->drawPath(ppath);
  else
    painter_->drawPath(ppath);
}

void
CQChartsViewPlotPaintDevice::
fillRect(const BBox &bbox)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  QBrush brush1;

  if (adjustFillBrush(this->brush(), pbbox, brush1))
    painter_->setBrushOrigin(pbbox.qrect().topLeft());

  if (isHandDrawn())
    hdPainter_->fillRect(pbbox.qrect(), brush1);
  else
    painter_->fillRect(pbbox.qrect(), brush1);
}

bool
CQChartsViewPlotPaintDevice::
adjustFillBrush(const QBrush &brush, const BBox &pbbox, QBrush &brush1) const
{
  brush1 = brush;

  if (brush1.style() != Qt::TexturePattern)
    return false;

  //---

  auto image = brush1.textureImage();

  //auto format = image.format();
  //assert(format == QImage::Format_ARGB32 || format == QImage::Format_ARGB32_Premultiplied);

  auto imageType = image.text("imageType");

  //---

  auto image1 = image.scaled(int(pbbox.getWidth()), int(pbbox.getHeight()));

  //auto format1 = image1.format();
  //assert(format1 == QImage::Format_ARGB32 || format1 == QImage::Format_ARGB32_Premultiplied);

  //---

  QBitmap maskBitmap;
  bool    masked = false;

  int depth = image1.depth();

  if (depth < 24)
    image1 = image1.convertToFormat(QImage::Format_ARGB32_Premultiplied);

  if      (imageType == "texture") {
    int ir = brush1.color().red  ();
    int ig = brush1.color().green();
    int ib = brush1.color().blue ();

    for (int y = 0; y < image1.height(); ++y) {
      for (int x = 0; x < image1.width(); ++x) {
        auto rgb = image1.pixel(x, y);

        int g1 = qGray(rgb);

        image1.setPixel(x, y, qRgba(ir, ig, ib, g1));
      }
    }
  }
  else if (imageType == "mask") {
    auto altColorStr = image.text("altColor");
    auto altAlphaStr = image.text("altAlpha");

    auto altColor = QColor(Qt::transparent);

    if (altColorStr.length()) {
      altColor = CQChartsUtil::stringToColor(altColorStr);

      if (altAlphaStr.length()) {
        bool ok;

        auto alpha = CQChartsUtil::toReal(altAlphaStr, ok);

        if (ok && alpha < 1.0) {
          altColor.setAlphaF(alpha);

          masked = true;
        }
      }
    }

    if (masked) {
      maskBitmap = QBitmap(image1.width(), image1.height());

      maskBitmap.fill(Qt::color1);
    }

    QPainter maskPainter;

    if (masked) {
      maskPainter.begin(&maskBitmap);

      maskPainter.setPen(Qt::color0);
    }

    int ir = brush1.color().red  ();
    int ig = brush1.color().green();
    int ib = brush1.color().blue ();
    int ia = brush1.color().alpha();

    int iar = altColor.red  ();
    int iag = altColor.green();
    int iab = altColor.blue ();
    int iaa = altColor.alpha();

    for (int y = 0; y < image1.height(); ++y) {
      for (int x = 0; x < image1.width(); ++x) {
        auto rgb = image1.pixel(x, y);

        int g1 = qGray(rgb);

        if (g1 < 128)
          image1.setPixel(x, y, qRgba(ir, ig, ib, ia));
        else {
          if (! masked)
            image1.setPixel(x, y, qRgba(iar, iag, iab, iaa));
          else
            maskPainter.drawPoint(x, y);
        }
      }
    }

    if (masked)
      maskPainter.end();
  }
  else {
    double a = brush1.color().alphaF();

    if (a < 1.0) {
      double r = brush1.color().redF  ();
      double g = brush1.color().greenF();
      double b = brush1.color().blueF ();

      for (int y = 0; y < image1.height(); ++y) {
        for (int x = 0; x < image1.width(); ++x) {
          auto rgb = image1.pixel(x, y);

          double r1 = qRed  (rgb)/255.0;
          double g1 = qGreen(rgb)/255.0;
          double b1 = qBlue (rgb)/255.0;

          int ir = int(255.0*(r1*a + (1 - a)*r));
          int ig = int(255.0*(g1*a + (1 - a)*g));
          int ib = int(255.0*(b1*a + (1 - a)*b));

          image1.setPixel(x, y, qRgba(ir, ig, ib, 255));
        }
      }
    }
  }

  //brush1.setTextureImage(image1);
  auto pixmap = QPixmap::fromImage(image1);

  if (masked)
    pixmap.setMask(maskBitmap);

  brush1.setTexture(pixmap);

  return true;
}

void
CQChartsViewPlotPaintDevice::
drawRect(const BBox &bbox)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  QBrush brush1;

  if (adjustFillBrush(this->brush(), pbbox, brush1)) {
    setBrush(brush1);

    painter_->setBrushOrigin(pbbox.qrect().topLeft());
  }

  if (isHandDrawn())
    hdPainter_->drawRect(pbbox.qrect());
  else if (int(fillType_) >= int(CQChartsFillPattern::Type::PATTERN_HATCH) &&
           int(fillType_) <= int(CQChartsFillPattern::Type::PATTERN_SQUARES)) {
    int fillType1 = int(fillType_) - int(CQChartsFillPattern::PatternOffset::VALUE);

    CQFillTexture texture;

    texture.setFillType(static_cast<CQFillTexture::FillType>(fillType1));

    texture.setBgColor (painter_->brush().color());
    texture.setFgColor (painter_->pen().color());
    texture.setAltColor(altColor_);
    texture.setAngle   (fillAngle_);
    texture.setRadius  (fillRadius_ > 0 ? fillRadius_ : 8);
    texture.setDelta   (fillDelta_ > 0 ? fillDelta_ : 4);
    texture.setWidth   (fillWidth_ > 0 ? fillWidth_ : 2);

    texture.fillRect(painter_, pbbox.qrect());

    painter_->setBrush(Qt::transparent);

    painter_->drawRect(pbbox.qrect());
  }
  else {
    painter_->drawRect(pbbox.qrect());
  }
}

void
CQChartsViewPlotPaintDevice::
drawEllipse(const BBox &bbox, const Angle &a)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto pbbox = windowToPixel(bbox);

  auto prect = pbbox.qrect();
  if (! prect.isValid()) return;

  if (! isHandDrawn() && a.isZero()) {
    painter_->drawEllipse(prect);
  }
  else {
    QPainterPath path, path1;

    auto rect = bbox.qrect();

    path.addEllipse(rect);

    if (! a.isZero()) {
      auto c = rect.center();

      QTransform t;

      // Note: reverse order (move to zero, rotate, move back)
      t.translate(c.x(), c.y());
      t.rotate(a.degrees());
      t.translate(-c.x(), -c.y());

      path1 = t.map(path);
    }
    else
      path1 = path;

    path = windowToPixel(path1);

    if (isHandDrawn())
      hdPainter_->drawPath(path);
    else
      painter_->drawPath(path);
  }
}

#if 0
void
CQChartsViewPlotPaintDevice::
drawArc(const BBox &rect, const Angle &a1, const Angle &a2)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto pbbox = windowToPixel(bbox);

  auto prect = pbbox.qrect();
  if (! prect.isValid()) return;

  if (isHandDrawn())
    assert(false);
  else
    painter_->drawArc(prect, a1.value()*16, a2.value()*16);
}
#endif

void
CQChartsViewPlotPaintDevice::
drawPolygon(const Polygon &poly)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto ppoly = windowToPixel(poly);

  if (isHandDrawn())
    hdPainter_->drawPolygon(ppoly.qpoly());
  else
    painter_->drawPolygon(ppoly.qpoly());
}

void
CQChartsViewPlotPaintDevice::
drawPolyline(const Polygon &poly)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto ppoly = windowToPixel(poly);

  if (isHandDrawn())
    hdPainter_->drawPolyline(ppoly.qpoly());
  else
    painter_->drawPolyline(ppoly.qpoly());
}

void
CQChartsViewPlotPaintDevice::
drawLine(const Point &p1, const Point &p2)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto pp1 = windowToPixel(p1);
  auto pp2 = windowToPixel(p2);

  if (isHandDrawn())
    hdPainter_->drawLine(pp1.qpoint(), pp2.qpoint());
  else
    painter_->drawLine(pp1.qpoint(), pp2.qpoint());
}

void
CQChartsViewPlotPaintDevice::
drawPoint(const Point &p)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto pp = windowToPixel(p);

  if (isHandDrawn())
    hdPainter_->drawPoint(pp.qpoint());
  else
    painter_->drawPoint(pp.qpoint());
}

void
CQChartsViewPlotPaintDevice::
drawText(const Point &p, const QString &text)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto pp = windowToPixel(p);

  if (isHandDrawn())
    hdPainter_->drawText(pp.qpoint(), text);
  else
    painter_->drawText(pp.qpoint(), text);
}

void
CQChartsViewPlotPaintDevice::
drawTransformedText(const Point &p, const QString &text)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  // NOTE: p is in pixels
  if (isHandDrawn())
    hdPainter_->drawText(p.qpoint(), text);
  else
    painter_->drawText(p.qpoint(), text);
}

void
CQChartsViewPlotPaintDevice::
drawImageInRect(const BBox &bbox, const Image &image, bool stretch, const Angle &angle)
{
  if (isNull())
    return;

  if (! painter_)
    return;

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

  if      (view_)
    image.resolve(view_->charts());
  else if (plot_)
    image.resolve(plot_->charts());

  auto qimage = image.sizedImage(int(w), int(h));

  auto qrect = pbbox1.qrect();

  auto qc = qrect.center();

  auto *ipainter = (isHandDrawn() ? const_cast<QPainter *>(hdPainter_->painter()) : painter_);

  if (! angle.isZero()) {
    ipainter->save();

    QTransform t;

    t.translate(qc.x(), qc.y());
    t.rotate(-angle.degrees());
    t.translate(-qc.x(), -qc.y());

    ipainter->setTransform(t);

    if (isHandDrawn())
      hdPainter_->drawImage(qrect, qimage);
    else
      ipainter->drawImage(qrect, qimage);

    ipainter->restore();
  }
  else {
    if (isHandDrawn())
      hdPainter_->drawImage(qrect, qimage);
    else
      ipainter->drawImage(qrect, qimage);
  }
}

void
CQChartsViewPlotPaintDevice::
drawImage(const Point &p, const QImage &image)
{
  if (isNull())
    return;

  if (! painter_)
    return;

  auto pp = windowToPixel(p);

  if (isHandDrawn())
    hdPainter_->drawImage(pp.qpoint(), image);
  else
    painter_->drawImage(pp.qpoint(), image);
}

const QFont &
CQChartsViewPlotPaintDevice::
font() const
{
  if (painter_)
    return painter_->font();
  else
    return font_;
}

void
CQChartsViewPlotPaintDevice::
setFont(const QFont &f, bool scale)
{
  if (scale && isZoomFont() && plot_) {
    auto f1 = CQChartsUtil::scaleFontSize(f, plot_->dataScale(), 1.0/72.0);

    if (painter_) {
      painter_->setFont(f1);

      font_ = painter_->font();
    }
    else
      font_ = f1;
  }
  else {
    if (painter_) {
      painter_->setFont(f);

      font_ = painter_->font();
    }
    else
      font_ = f;
  }
}

void
CQChartsViewPlotPaintDevice::
setTransformRotate(const Point &p, double angle)
{
  if (! painter_)
    return;

  auto p1 = windowToPixel(p);

  auto t = painter_->transform();

  // Note: reverse order (rotate at zero and move to pos)
  t.translate(p1.x, p1.y);
  t.rotate(-angle);

  painter_->setTransform(t);
}

const QTransform &
CQChartsViewPlotPaintDevice::
transform() const
{
  static QTransform t;

  return (painter_ ? painter_->transform() : t);
}

void
CQChartsViewPlotPaintDevice::
setTransform(const QTransform &t, bool combine)
{
  if (painter_)
    painter_->setTransform(t, combine);
}

void
CQChartsViewPlotPaintDevice::
setRenderHints(QPainter::RenderHints hints, bool on)
{
  if (painter_)
    painter_->setRenderHints(hints, on);
}

void
CQChartsViewPlotPaintDevice::
setPainterFont(const Font &font)
{
  if      (plot_)
    plot_->setPainterFont(this, font);
  else if (view_)
    view_->setPainterFont(this, font);
  else
    assert(false);
}
