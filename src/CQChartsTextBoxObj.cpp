#include <CQChartsTextBoxObj.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRotatedText.h>
#include <QPainter>

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsView *view) :
 CQChartsBoxObj(view)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  boxData_.shape.background.visible = false;
  boxData_.shape.border    .visible = false;
}

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsPlot *plot) :
 CQChartsBoxObj(plot)
{
  CQChartsColor themeFg(CQChartsColor::Type::INTERFACE_VALUE, 1);

  setTextColor(themeFg);

  boxData_.shape.background.visible = false;
  boxData_.shape.border    .visible = false;
}

QColor
CQChartsTextBoxObj::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(plot_, i, n);
}

void
CQChartsTextBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textStr"     , "text"    );
  model->addProperty(textPath, this, "textFont"    , "font"    );
  model->addProperty(textPath, this, "textColor"   , "color"   );
  model->addProperty(textPath, this, "textAlpha"   , "alpha"   );
  model->addProperty(textPath, this, "textAngle"   , "angle"   );
  model->addProperty(textPath, this, "textContrast", "contrast");
  model->addProperty(textPath, this, "textAlign"   , "align"   );
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (! isTextVisible())
    return;

  CQChartsBoxObj::draw(painter, rect);

  //---

  drawText(painter, rect, textStr());
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  CQChartsBoxObj::draw(painter, poly);

  //---

  QRectF rect = poly.boundingRect();

  drawText(painter, rect, textStr());
}

void
CQChartsTextBoxObj::
drawText(QPainter *painter, const QRectF &rect, const QString &text) const
{
  QFontMetricsF fm(textFont());

  QColor c = interpTextColor(0, 1);

  c.setAlphaF(textAlpha());

  QPen pen(c);

  painter->setPen (pen);
  painter->setFont(textFont());

  painter->drawText(QPointF(rect.left() + margin(),
                            rect.bottom() - margin() - fm.descent()), text);
}

//------

CQChartsRotatedTextBoxObj::
CQChartsRotatedTextBoxObj(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
}

void
CQChartsRotatedTextBoxObj::
draw(QPainter *painter, const QPointF &center, const QString &text, double angle,
     Qt::Alignment align) const
{
  painter->save();

  QFontMetricsF fm(textFont());

  double tw = fm.width(text);

  double tw1 = tw + 2*margin();
  double th1 = fm.height() + 2*margin();

  double cx = center.x();
  double cy = center.y() - th1/2;
  double cd = 0.0;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
    cd  = -margin();
  }
  else {
    cd  = margin();
  }

  rect_ = QRectF(cx, cy, tw1, th1);

  //---

  // draw box
  if (CMathUtil::isZero(angle)) {
    CQChartsBoxObj::draw(painter, rect_);
  }
  else {
    QRectF                      bbox;
    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(center.x(), center.y(), text, textFont(), angle, margin(),
                                  bbox, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);
  }

  //---

  // draw text
  painter->setFont(textFont());

  QColor c = interpTextColor(0, 1);

  c.setAlphaF(textAlpha());

  QPen pen(c);

  painter->setPen(pen);

  CQChartsRotatedText::drawRotatedText(painter, center.x() + cd, center.y(), text, angle,
                                       align, /*alignBBox*/ true);

  painter->restore();
}

CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(const QPointF &center, const QString &text, double angle, Qt::Alignment align) const
{
  QFontMetricsF fm(textFont());

  double tw = fm.width(text);

  double tw1 = tw + 2*margin();
  double th1 = fm.height() + 2*margin();

  double cx = center.x();
  double cy = center.y() - th1/2;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
  }

  QRectF qrect;

  if (angle) {
    qrect = QRectF(cx, cy, tw1, th1);
  }
  else {
    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(center.x(), center.y(), text, textFont(), angle, margin(),
                                  qrect, points, align, /*alignBBox*/ true);
  }

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(CQChartsUtil::fromQRect(qrect), bbox);

  return bbox;
}

void
CQChartsRotatedTextBoxObj::
drawConnectedRadialText(QPainter *painter, const QPointF &center, double ro, double lr,
                        double ta, const QString &text, const QPen &lpen, bool isRotated)
{
  CQChartsGeom::BBox tbbox;

  drawCalcConnectedRadialText(painter, center, ro, lr, ta, text, lpen, isRotated, tbbox);
}

void
CQChartsRotatedTextBoxObj::
calcConnectedRadialTextBBox(const QPointF &center, double ro, double lr, double ta,
                            const QString &text, bool isRotated, CQChartsGeom::BBox &tbbox)
{
  QPen lpen;

  drawCalcConnectedRadialText(0, center, ro, lr, ta, text, lpen, isRotated, tbbox);
}

void
CQChartsRotatedTextBoxObj::
drawCalcConnectedRadialText(QPainter *painter, const QPointF &center, double ro, double lr,
                            double ta, const QString &text, const QPen &lpen, bool isRotated,
                            CQChartsGeom::BBox &tbbox)
{
  if (painter)
    painter->save();

  //---

  double tangle = CMathUtil::Deg2Rad(ta);

  double tc = cos(tangle);
  double ts = sin(tangle);

  double tx = center.x() + lr*tc;
  double ty = center.y() + lr*ts;

  double ptx, pty;

  plot_->windowToPixel(tx, ty, ptx, pty);

  //---

  double        dx    = 0.0;
  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  // connect text to outer edge
  if (lr > ro) {
    double lx1 = center.x() + ro*tc;
    double ly1 = center.y() + ro*ts;
    double lx2 = center.x() + lr*tc;
    double ly2 = center.y() + lr*ts;

    double lpx1, lpy1, lpx2, lpy2;

    plot_->windowToPixel(lx1, ly1, lpx1, lpy1);
    plot_->windowToPixel(lx2, ly2, lpx2, lpy2);

    int tickSize = 16;

    bool labelRight = (tc >= 0);

    if (plot_->isInvertX())
      labelRight = ! labelRight;

    if (labelRight) {
      dx    = tickSize;
      align = Qt::AlignLeft | Qt::AlignVCenter;
    }
    else {
      dx    = -tickSize;
      align = Qt::AlignRight | Qt::AlignVCenter;
    }

    if (painter) {
      painter->setPen(lpen);

      painter->drawLine(QPointF(lpx1, lpy1), QPointF(lpx2     , lpy2));
      painter->drawLine(QPointF(lpx2, lpy2), QPointF(lpx2 + dx, lpy2));
    }
  }

  //---

  // draw text
  QPointF pt(ptx + dx, pty);

  double angle = 0.0;

  if (isRotated)
    angle = (tc >= 0 ? ta : 180.0 + ta);

  if (painter)
    draw(painter, pt, text, angle, align);
  else
    tbbox = this->bbox(pt, text, angle, align);

  //---

  if (painter)
    painter->restore();
}
