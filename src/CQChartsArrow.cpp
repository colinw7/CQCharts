#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

#include <QPainter>
#include <QPainterPath>

CQChartsArrow::
CQChartsArrow(CQChartsView *view, const QPointF &from, const QPointF &to) :
 CQChartsObjShapeData<CQChartsArrow>(this),
 view_(view), from_(from), to_(to)
{
}

CQChartsArrow::
CQChartsArrow(CQChartsPlot *plot, const QPointF &from, const QPointF &to) :
 CQChartsObjShapeData<CQChartsArrow>(this),
 plot_(plot), from_(from), to_(to)
{
}

CQCharts *
CQChartsArrow::
charts() const
{
  return (plot_ ? plot_->charts() : view_->charts());
}

void
CQChartsArrow::
draw(QPainter *painter)
{
  painter_ = painter;

  //---

  QPointF from = from_;
  QPointF to   = (isRelative() ? from_ + to_ : to_);

  double fx, fy, tx, ty;

  if (plot_) {
    plot_->windowToPixel(from.x(), from.y(), fx, fy);
    plot_->windowToPixel(to  .x(), to  .y(), tx, ty);
  }
  else {
    view_->windowToPixel(from.x(), from.y(), fx, fy);
    view_->windowToPixel(to  .x(), to  .y(), tx, ty);
  }

  double xw, yw;

  if (plot_) {
    xw = (borderWidth().value() > 0 ? plot_->lengthPixelWidth (borderWidth()) : 4);
    yw = (borderWidth().value() > 0 ? plot_->lengthPixelHeight(borderWidth()) : 4);
  }
  else {
    xw = (borderWidth().value() > 0 ? view_->lengthPixelWidth (borderWidth()) : 4);
    yw = (borderWidth().value() > 0 ? view_->lengthPixelHeight(borderWidth()) : 4);
  }

  double a = atan2(ty - fy, tx - fx);

  double aa = CMathUtil::Deg2Rad(angle() > 0 ? angle() : 45);

  double xl, yl;

  if (plot_) {
    xl = (length().value() > 0 ? plot_->lengthPixelWidth (length()) : 8);
    yl = (length().value() > 0 ? plot_->lengthPixelHeight(length()) : 8);
  }
  else {
    xl = (length().value() > 0 ? view_->lengthPixelWidth (length()) : 8);
    yl = (length().value() > 0 ? view_->lengthPixelHeight(length()) : 8);
  }

  double xl1 = xl*cos(aa);
  double yl1 = yl*cos(aa);

  double c = cos(a), s = sin(a);

  //---

  double x1 = fx, y1 = fy;
  double x4 = tx, y4 = ty;

#if 0
  if (isDebugTextVisible()) {
    drawPointLabel(QPointF(x1, y1), "p1", true, false);
    drawPointLabel(QPointF(x4, y4), "p4", true, false);
  }
#endif

  double x2 = x1 + xl1*c;
  double y2 = y1 + yl1*s;
  double x3 = x4 - xl1*c;
  double y3 = y4 - yl1*s;

#if 0
  if (isDebugTextVisible()) {
    drawPointLabel(QPointF(x2, y2), "p2", true, false);
    drawPointLabel(QPointF(x3, y3), "p3", true, false);
  }
#endif

  double x11 = x1, y11 = y1;
  double x41 = x4, y41 = y4;

  if (isFHead()) {
    if (isFilled() || isEmpty()) {
      x11 = x2;
      y11 = y2;
    }
    else {
      x11 = x1 + xw*c;
      y11 = y1 + yw*s;
    }
  }

  if (isTHead()) {
    if (isFilled() || isEmpty()) {
      x41 = x3;
      y41 = y3;
    }
    else {
      x41 = x4 - xw*c;
      y41 = y4 - yw*s;
    }
  }

  double ba = CMathUtil::Deg2Rad(backAngle() > 0 ? backAngle() : 90);

  if (isFHead()) {
    double a1 = a + aa;
    double a2 = a - aa;

    double c1 = cos(a1), s1 = sin(a1);
    double c2 = cos(a2), s2 = sin(a2);

    double xf1 = x1 + xl*c1;
    double yf1 = y1 + yl*s1;
    double xf2 = x1 + xl*c2;
    double yf2 = y1 + yl*s2;

#if 0
    if (isDebugTextVisible()) {
      drawPointLabel(QPointF(xf1, yf1), "pf1", true, false);
      drawPointLabel(QPointF(xf2, yf2), "pf2", true, false);
    }
#endif

    double xf3 = x2, yf3 = y2;

    if (! isFilled() && ! isEmpty()) {
      drawLine(QPointF(x1, y1), QPointF(xf1, yf1), xw, false);
      drawLine(QPointF(x1, y1), QPointF(xf2, yf2), xw, false);
    }
    else {
      if (ba > aa && ba < M_PI) {
        double a3 = a + ba;

        double c3 = cos(a3), s3 = sin(a3);

        CQChartsUtil::intersectLines(x1, y1, x2, y2, xf1, yf1, xf1 - 10*c3, yf1 - 10*s3, xf3, yf3);

#if 0
        if (isDebugTextVisible())
          drawPointLabel(QPointF(xf3, yf3), "pf3", true, false);
#endif

        x11 = xf3;
        y11 = yf3;
      }

      std::vector<QPointF> points;

      points.push_back(QPointF(x1 , y1 ));
      points.push_back(QPointF(xf1, yf1));
      points.push_back(QPointF(xf3, yf3));
      points.push_back(QPointF(xf2, yf2));

      drawPolygon(points, xw, isFilled(), isBorder());
    }
  }

  if (isTHead()) {
    double a1 = a + M_PI - aa;
    double a2 = a + M_PI + aa;

    double c1 = cos(a1), s1 = sin(a1);
    double c2 = cos(a2), s2 = sin(a2);

    double xt1 = x4 + xl*c1;
    double yt1 = y4 + yl*s1;
    double xt2 = x4 + xl*c2;
    double yt2 = y4 + yl*s2;

#if 0
    if (isDebugTextVisible()) {
      drawPointLabel(QPointF(xt1, yt1), "pt1", true, false);
      drawPointLabel(QPointF(xt2, yt2), "pt2", true, false);
    }
#endif

    double xt3 = x3, yt3 = y3;

    if (! isFilled() && ! isEmpty()) {
      drawLine(QPointF(x4, y4), QPointF(xt1, yt1), xw, false);
      drawLine(QPointF(x4, y4), QPointF(xt2, yt2), xw, false);
    }
    else {
      if (ba > aa && ba < M_PI) {
        double a3 = a + M_PI - ba;

        double c3 = cos(a3), s3 = sin(a3);

        CQChartsUtil::intersectLines(x3, y3, x4, y4, xt1, yt1, xt1 - 10*c3, yt1 - 10*s3, xt3, yt3);

#if 0
        if (isDebugTextVisible())
          drawPointLabel(QPointF(xt3, yt3), "pt3", true, false);
#endif

        x41 = xt3;
        y41 = yt3;
      }

      std::vector<QPointF> points;

      points.push_back(QPointF(x4 , y4 ));
      points.push_back(QPointF(xt1, yt1));
      points.push_back(QPointF(xt3, yt3));
      points.push_back(QPointF(xt2, yt2));

      drawPolygon(points, xw, isFilled(), isBorder());
    }
  }

  drawLine(QPointF(x11, y11), QPointF(x41, y41), xw, false);
}

void
CQChartsArrow::
drawPolygon(const std::vector<QPointF> &points, double width, bool filled, bool stroked)
{
  QPainterPath path;

  double px = points[0].x();
  double py = points[0].y();

  path.moveTo(px, py);

  for (uint i = 1; i < points.size(); ++i) {
    px = points[i].x();
    py = points[i].y();

    path.lineTo(px, py);
  }

  path.closeSubpath();

  //---

  if (filled) {
    QBrush brush;

    QColor fc = interpFillColor(0, 1);

    if (plot_)
      plot_->setBrush(brush, true, fc, fillAlpha(), fillPattern());
    else
      view_->setBrush(brush, true, fc, fillAlpha(), fillPattern());

    painter_->fillPath(path, brush);
  }

  //---

  if (stroked) {
    QPen pen;

    QColor sc = interpBorderColor(0, 1);

    if (plot_)
      plot_->setPen(pen, true, sc, borderAlpha(), width);
    else
      view_->setPen(pen, true, sc, borderAlpha(), width);

    painter_->strokePath(path, pen);
  }
}

void
CQChartsArrow::
drawLine(const QPointF &point1, const QPointF &point2, double width, bool mapping)
{
  QPen p = painter_->pen();

  QColor sc = interpBorderColor(0, 1);

  p.setColor (sc);
  p.setWidthF(width);

  painter_->setPen(p);

  double px1, py1, px2, py2;

  if (mapping) {
    if (plot_) {
      plot_->windowToPixel(point1.x(), point1.y(), px1, py1);
      plot_->windowToPixel(point2.x(), point2.y(), px2, py2);
    }
    else {
      view_->windowToPixel(point1.x(), point1.y(), px1, py1);
      view_->windowToPixel(point2.x(), point2.y(), px2, py2);
    }
  }
  else {
    px1 = point1.x();
    py1 = point1.y();
    px2 = point2.x();
    py2 = point2.y();
  }

  painter_->drawLine(px1, py1, px2, py2);
}

#if 0
void
CQChartsArrow::
drawPointLabel(const QPointF &point, const QString &text, bool above, bool mapping)
{
  double px, py;

  if (mapping) {
    if (plot_)
      plot_->windowToPixel(point.x(), point.y(), px, py);
    else
      view_->windowToPixel(point.x(), point.y(), px, py);
  }
  else {
    px = point.x();
    py = point.y();
  }

  QPen tpen;

  QColor tc = interpDebugTextColor(0, 1);

  if (plot_)
    plot_->setPen(tpen, true, tc, 1.0, CQChartsLength("1.0"));
  else
    view_->setPen(tpen, true, tc, 1.0, CQChartsLength("1.0"));

  painter_->setPen(tpen);

  painter_->drawLine(px - 4, py    , px + 4, py    );
  painter_->drawLine(px    , py - 4, px    , py + 4);

  QFontMetricsF fm(painter_->font());

  double w = fm.width(text);
  double h = fm.height();

  painter_->drawText(px - w/2, py + (above ? -h : h), text);
}
#endif
