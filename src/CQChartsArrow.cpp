#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQCharts.h>

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
  if      (plot_)
    return plot_->charts();
  else if (view_)
    return view_->charts();
  else
    return nullptr;
}

void
CQChartsArrow::
draw(QPainter *painter) const
{
  auto windowToPixel = [&](double wx, double wy, double &px, double &py) {
    if      (plot_)
      plot_->windowToPixel(wx, wy, px, py);
    else if (view_)
      view_->windowToPixel(wx, wy, px, py);
    else {
      px = wx;
      py = wy;
    }
  };

  auto lengthPixelWidth = [&](const CQChartsLength &l) {
    if      (plot_)
      return plot_->lengthPixelWidth(l);
    else if (view_)
      return view_->lengthPixelWidth(l);
    else
      return l.value();
  };

  auto lengthPixelHeight = [&](const CQChartsLength &l) {
    if      (plot_)
      return plot_->lengthPixelHeight(l);
    else if (view_)
      return view_->lengthPixelHeight(l);
    else
      return l.value();
  };

  //---

  painter_ = painter;

  //---

  QPointF from = from_;
  QPointF to   = (isRelative() ? from_ + to_ : to_);

  double fx, fy, tx, ty;

  windowToPixel(from.x(), from.y(), fx, fy);
  windowToPixel(to  .x(), to  .y(), tx, ty);

  double xw = (borderWidth().value() > 0 ? lengthPixelWidth (borderWidth()) : 4);
  double yw = (borderWidth().value() > 0 ? lengthPixelHeight(borderWidth()) : 4);

  double a = atan2(ty - fy, tx - fx);

  double aa = CMathUtil::Deg2Rad(angle() > 0 ? angle() : 45);

  double xl = (length().value() > 0 ? lengthPixelWidth (length()) : 8);
  double yl = (length().value() > 0 ? lengthPixelHeight(length()) : 8);

  double xl1 = xl*cos(aa);
  double yl1 = yl*cos(aa);

  double c = cos(a), s = sin(a);

  //---

  double x1 = fx, y1 = fy;
  double x4 = tx, y4 = ty;

  QPointF p1(x1, y1);
  QPointF p4(x4, y4);

#if 0
  if (debugLabels()) {
    drawPointLabel(p1, "p1", true, false);
    drawPointLabel(p4, "p4", true, false);
  }
#endif

  double x2 = x1 + xl1*c;
  double y2 = y1 + yl1*s;
  double x3 = x4 - xl1*c;
  double y3 = y4 - yl1*s;

  QPointF p2(x2, y2);
  QPointF p3(x3, y3);

#if 0
  if (debugLabels()) {
    drawPointLabel(p2, "p2", true, false);
    drawPointLabel(p3, "p3", true, false);
  }
#endif

  double x11 = x1, y11 = y1;
  double x41 = x4, y41 = y4;

  if (isFHead()) {
    if (! isLineEnds()) {
      x11 = x1 + xw*c;
      y11 = y1 + yw*s;
    }
  }

  if (isTHead()) {
    if (! isLineEnds()) {
      x41 = x4 - xw*c;
      y41 = y4 - yw*s;
    }
  }

  double ba = CMathUtil::Deg2Rad(backAngle() > 0 ? backAngle() : 90);

  //---

  bool linePoly = (lineWidth().value() > 0);

  //---

  std::vector<QPointF> fHeadPoints;
  QPointF              fHeadMid;
  QPointF              pf1, pf2, pf3;

  if (isFHead()) {
    double a1 = a + aa;
    double a2 = a - aa;

    double c1 = cos(a1), s1 = sin(a1);
    double c2 = cos(a2), s2 = sin(a2);

    double xf1 = x1 + xl*c1;
    double yf1 = y1 + yl*s1;
    double xf2 = x1 + xl*c2;
    double yf2 = y1 + yl*s2;

    pf1 = QPointF(xf1, yf1);
    pf2 = QPointF(xf2, yf2);

#if 0
    if (debugLabels()) {
      drawPointLabel(pf1, "pf1", true, false);
      drawPointLabel(pf2, "pf2", true, false);
    }
#endif

    double xf3 = x2, yf3 = y2;

    if (isLineEnds()) {
      drawLine(p1, pf1, xw, false);
      drawLine(p1, pf2, xw, false);
    }
    else {
      if (ba > aa && ba < M_PI) {
        double a3 = a + ba;

        double c3 = cos(a3), s3 = sin(a3);

        CQChartsUtil::intersectLines(x1, y1, x2, y2, xf1, yf1, xf1 - 10*c3, yf1 - 10*s3, xf3, yf3);

        pf3 = QPointF(xf3, yf3);

#if 0
        if (debugLabels())
          drawPointLabel(pf3, "pf3", true, false);
#endif

        x11 = xf3;
        y11 = yf3;

        fHeadMid = QPointF(x11, y11);
      }

      fHeadPoints.push_back(QPointF(x1 , y1 ));
      fHeadPoints.push_back(QPointF(xf1, yf1));
      fHeadPoints.push_back(QPointF(xf3, yf3));
      fHeadPoints.push_back(QPointF(xf2, yf2));

      if (! linePoly)
        drawPolygon(fHeadPoints, xw, isFilled());
    }
  }

  //---

  std::vector<QPointF> tHeadPoints;
  QPointF              tHeadMid;
  QPointF              pt1, pt2, pt3;

  if (isTHead()) {
    double a1 = a + M_PI - aa;
    double a2 = a + M_PI + aa;

    double c1 = cos(a1), s1 = sin(a1);
    double c2 = cos(a2), s2 = sin(a2);

    double xt1 = x4 + xl*c1;
    double yt1 = y4 + yl*s1;
    double xt2 = x4 + xl*c2;
    double yt2 = y4 + yl*s2;

    pt1 = QPointF(xt1, yt1);
    pt2 = QPointF(xt2, yt2);

#if 0
    if (debugLabels()) {
      drawPointLabel(pt1, "pt1", true, false);
      drawPointLabel(pt2, "pt2", true, false);
    }
#endif

    double xt3 = x3, yt3 = y3;

    if (isLineEnds()) {
      drawLine(p4, pt1, xw, false);
      drawLine(p4, pt2, xw, false);
    }
    else {
      if (ba > aa && ba < M_PI) {
        double a3 = a + M_PI - ba;

        double c3 = cos(a3), s3 = sin(a3);

        CQChartsUtil::intersectLines(x3, y3, x4, y4, xt1, yt1, xt1 - 10*c3, yt1 - 10*s3, xt3, yt3);

        pt3 = QPointF(xt3, yt3);

#if 0
        if (debugLabels())
          drawPointLabel(pt3, "pt3", true, false);
#endif

        x41 = xt3;
        y41 = yt3;

        tHeadMid = QPointF(x41, y41);
      }

      tHeadPoints.push_back(QPointF(x4 , y4 ));
      tHeadPoints.push_back(QPointF(xt1, yt1));
      tHeadPoints.push_back(QPointF(xt3, yt3));
      tHeadPoints.push_back(QPointF(xt2, yt2));

      if (! linePoly)
        drawPolygon(tHeadPoints, xw, isFilled());
    }
  }

  //---

  QPointF fl1, fl2;

  if (linePoly) {
    std::vector<QPointF> midPoints1, midPoints2;

    double xlw = lengthPixelWidth (lineWidth())/2.0;
    double ylw = lengthPixelHeight(lineWidth())/2.0;

    double lx1 = x11 + lengthPixelWidth (xlw*s);
    double ly1 = y11 + lengthPixelHeight(ylw*c);
    double lx2 = x11 - lengthPixelWidth (xlw*s);
    double ly2 = y11 - lengthPixelHeight(ylw*c);

    double lx3 = x41 + lengthPixelWidth (xlw*s);
    double ly3 = y41 + lengthPixelHeight(ylw*c);
    double lx4 = x41 - lengthPixelWidth (xlw*s);
    double ly4 = y41 - lengthPixelHeight(ylw*c);

    if (! fHeadPoints.empty()) {
      int np = fHeadPoints.size();

      double ix1 = lx1, iy1 = ly1;
      double ix2 = lx2, iy2 = ly2;

      for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
        double xt, yt;

        bool bi1 = CQChartsUtil::intersectLines(lx1, ly1, lx3, ly3,
                                                fHeadPoints[i1].x(), fHeadPoints[i1].y(),
                                                fHeadPoints[i2].x(), fHeadPoints[i2].y(),
                                                xt, yt);

        fl1 = QPointF(xt, yt);

        if (bi1) {
          if (xt > ix1) {
            ix1 = xt;
            iy1 = yt;
          }

#if 0
          if (debugLabels())
            drawPointLabel(fl1, "fl1", true, false);
#endif
        }

        bool bi2 = CQChartsUtil::intersectLines(lx2, ly2, lx4, ly4,
                                                fHeadPoints[i1].x(), fHeadPoints[i1].y(),
                                                fHeadPoints[i2].x(), fHeadPoints[i2].y(),
                                                xt, yt);

        fl2 = QPointF(xt, yt);

        if (bi2) {
          if (xt > ix2) {
            ix2 = xt;
            iy2 = yt;
          }

#if 0
          if (debugLabels())
            drawPointLabel(fl2, "fl2", true, false);
#endif
        }
      }

      lx1 = ix1; ly1 = iy1;
      lx2 = ix2; ly2 = iy2;

      QPointF fl3(lx1, ly1);
      QPointF fl4(lx2, ly2);

#if 0
      if (debugLabels()) {
        drawPointLabel(fl3, "fl3", true, false);
        drawPointLabel(fl4, "fl4", true, false);
      }
#endif

      //---

      midPoints1.push_back(fl4);
      midPoints1.push_back(fHeadMid);
      midPoints1.push_back(fl3);

      //drawPolygon(midPoints1, xw, isFilled());
    }

    QPointF tl1, tl2;

    if (! tHeadPoints.empty()) {
      int np = tHeadPoints.size();

      double ix3 = lx3, iy3 = ly3;
      double ix4 = lx4, iy4 = ly4;

      for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
        double xt, yt;

        bool bi1 = CQChartsUtil::intersectLines(lx1, ly1, lx3, ly3,
                                                tHeadPoints[i1].x(), tHeadPoints[i1].y(),
                                                tHeadPoints[i2].x(), tHeadPoints[i2].y(),
                                                xt, yt);

        tl1 = QPointF(xt, yt);

        if (bi1) {
          if (xt < ix3) {
            ix3 = xt;
            iy3 = yt;
          }

#if 0
          if (debugLabels())
            drawPointLabel(tl1, "tl1", true, false);
#endif
        }

        bool bi2 = CQChartsUtil::intersectLines(lx2, ly2, lx4, ly4,
                                                tHeadPoints[i1].x(), tHeadPoints[i1].y(),
                                                tHeadPoints[i2].x(), tHeadPoints[i2].y(),
                                                xt, yt);

        tl2 = QPointF(xt, yt);

        if (bi2) {
          if (xt < ix4) {
            ix4 = xt;
            iy4 = yt;
          }

#if 0
          if (debugLabels())
            drawPointLabel(tl2, "tl2", true, false);
#endif
        }
      }

      lx3 = ix3; ly3 = iy3;
      lx4 = ix4; ly4 = iy4;

      QPointF tl3(lx3, ly3);
      QPointF tl4(lx4, ly4);

#if 0
      if (debugLabels()) {
        drawPointLabel(tl3, "tl3", true, false);
        drawPointLabel(tl4, "tl4", true, false);
      }
#endif

      //---

      midPoints2.push_back(tl3);
      midPoints2.push_back(tHeadMid);
      midPoints2.push_back(tl4);

      //drawPolygon(midPoints2, xw, isFilled());
    }

    QPointF pl1(lx1, ly1);
    QPointF pl2(lx2, ly2);
    QPointF pl3(lx3, ly3);
    QPointF pl4(lx4, ly4);

#if 0
    if (debugLabels()) {
      drawPointLabel(pl1, "pl1", true, false);
      drawPointLabel(pl2, "pl2", true, false);
      drawPointLabel(pl3, "pl3", true, false);
      drawPointLabel(pl4, "pl4", true, false);
    }
#endif

    std::vector<QPointF> points;

    if (! isLineEnds()) {
      if (isFHead() && isTHead()) {
        points.push_back(p1);
        points.push_back(pf1);
        points.push_back(pl1);
        points.push_back(pl3);
        points.push_back(pt1);
        points.push_back(p4);
        points.push_back(pt2);
        points.push_back(pl4);
        points.push_back(pl2);
        points.push_back(pf2);
      }
      else if (isTHead()) {
        points.push_back(pl1);
        points.push_back(pl3);
        points.push_back(pt1);
        points.push_back(p4);
        points.push_back(pt2);
        points.push_back(pl4);
        points.push_back(pl2);
      }
      else if (isFHead()) {
        points.push_back(p1);
        points.push_back(pf1);
        points.push_back(pl1);
        points.push_back(pl3);
        points.push_back(pl4);
        points.push_back(pl2);
        points.push_back(pf2);
      }
      else {
        points.push_back(pl1);
        points.push_back(pl3);
        points.push_back(pl4);
        points.push_back(pl2);
      }
    }
    else {
      points.push_back(pl1);
      points.push_back(pl3);
      points.push_back(pl4);
      points.push_back(pl2);
    }

    drawPolygon(points, xw, isFilled());

#if 0
    if (isFilled()) {
      std::vector<QPointF> points;

      points.push_back(pl1);
      points.push_back(pl3);
      points.push_back(pl4);
      points.push_back(pl2);

      drawPolygon(points, xw, isFilled());
    }
    else {
      drawLine(pl1, pl3, xw, false);
      drawLine(pl2, pl4, xw, false);
    }
#endif
  }
  else {
    drawLine(QPointF(x11, y11), QPointF(x41, y41), xw, false);
  }
}

void
CQChartsArrow::
drawPolygon(const std::vector<QPointF> &points, double width, bool filled) const
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

    CQChartsUtil::setBrush(brush, true, fc, fillAlpha(), fillPattern());

    painter_->fillPath(path, brush);
  }
  else {
    QPen pen;

    QColor sc = interpBorderColor(0, 1);

    CQChartsUtil::setPen(pen, true, sc, borderAlpha(), width);

    painter_->strokePath(path, pen);
  }
}

void
CQChartsArrow::
drawLine(const QPointF &point1, const QPointF &point2, double width, bool mapping) const
{
  auto windowToPixel = [&](double wx, double wy, double &px, double &py) {
    if      (plot_)
      plot_->windowToPixel(wx, wy, px, py);
    else if (view_)
      view_->windowToPixel(wx, wy, px, py);
    else {
      px = wx;
      py = wy;
    }
  };

  //---

  QPen p = painter_->pen();

  QColor sc = interpBorderColor(0, 1);

  p.setColor (sc);
  p.setWidthF(width);

  painter_->setPen(p);

  double px1, py1, px2, py2;

  if (mapping) {
    windowToPixel(point1.x(), point1.y(), px1, py1);
    windowToPixel(point2.x(), point2.y(), px2, py2);
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
drawPointLabel(const QPointF &point, const QString &text, bool above, bool mapping) const
{
  double px, py;

  if (mapping) {
    windowToPixel(point.x(), point.y(), px, py);
  }
  else {
    px = point.x();
    py = point.y();
  }

  QPen tpen;

  QColor tc = interpDebugTextColor(0, 1);

  CQChartsUtil::>setPen(tpen, true, tc, 1.0, CQChartsLength("1.0"));

  painter_->setPen(tpen);

  painter_->drawLine(px - 4, py    , px + 4, py    );
  painter_->drawLine(px    , py - 4, px    , py + 4);

  QFontMetricsF fm(painter_->font());

  double w = fm.width(text);
  double h = fm.height();

  CQChartsDrawUtil::drawSimpleText(painter_, px - w/2, py + (above ? -h : h), text);
}
#endif
