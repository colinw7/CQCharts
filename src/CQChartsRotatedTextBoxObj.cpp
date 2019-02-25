#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsRotatedText.h>
#include <QPainter>

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

  if      (plot())
    view()->setPlotPainterFont(plot(), painter, textFont());
  else if (view())
    view()->setPainterFont(painter, textFont());

  QFontMetricsF fm(painter->font());

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

    CQChartsRotatedText::bboxData(center.x(), center.y(), text, painter->font(), angle, margin(),
                                  bbox, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);
  }

  //---

  // draw text
  QColor c = interpTextColor(0, 1);

  QPen pen;

  if      (plot())
    plot()->setPen(pen, true, c, textAlpha());
  else if (view())
    view()->setPen(pen, true, c, textAlpha());

  painter->setPen(pen);

  CQChartsRotatedText::draw(painter, center.x() + cd, center.y(), text, angle,
                            align, /*alignBBox*/ true);

  painter->restore();
}

CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(const QPointF &center, const QString &text, double angle, Qt::Alignment align) const
{
  QFont font;

  if      (plot())
    font = view()->plotFont(plot(), textFont());
  else if (view())
    font = view()->viewFont(textFont());

  QFontMetricsF fm(font);

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

    CQChartsRotatedText::bboxData(center.x(), center.y(), text, font, angle, margin(),
                                  qrect, points, align, /*alignBBox*/ true);
  }

  CQChartsGeom::BBox bbox;

  if      (plot())
    bbox = plot()->pixelToWindow(CQChartsUtil::fromQRect(qrect));
  else if (view())
    bbox = view()->pixelToWindow(CQChartsUtil::fromQRect(qrect));

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

  double ptx = 0.0, pty = 0.0;

  if      (plot())
    plot()->windowToPixel(tx, ty, ptx, pty);
  else if (view())
    view()->windowToPixel(tx, ty, ptx, pty);

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

    if       (plot()) {
      plot()->windowToPixel(lx1, ly1, lpx1, lpy1);
      plot()->windowToPixel(lx2, ly2, lpx2, lpy2);
    }
    else if (view()) {
      view()->windowToPixel(lx1, ly1, lpx1, lpy1);
      view()->windowToPixel(lx2, ly2, lpx2, lpy2);
    }
    else
      assert(false);

    int tickSize = 16;

    bool labelRight = (tc >= 0);

    if (plot() && plot()->isInvertX())
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
