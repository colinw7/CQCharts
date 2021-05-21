#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyViewModel.h>

#include <QPainterPath>

CQChartsArrow::
CQChartsArrow(View *view, const Point &from, const Point &to) :
 CQChartsObjShapeData<CQChartsArrow>(this), view_(view), from_(from), to_(to)
{
  init();
}

CQChartsArrow::
CQChartsArrow(Plot *plot, const Point &from, const Point &to) :
 CQChartsObjShapeData<CQChartsArrow>(this), plot_(plot), from_(from), to_(to)
{
  init();
}

void
CQChartsArrow::
init()
{
  setFilled (true);
  setStroked(false);
}

CQCharts *
CQChartsArrow::
charts() const
{
  if      (plot())
    return plot()->charts();
  else if (view())
    return view()->charts();
  else
    return nullptr;
}

//---

void
CQChartsArrow::
setVisible(bool b)
{
  visible_ = b; emit dataChanged();
}

void
CQChartsArrow::
setFrom(const Point &v)
{
  from_ = v; emit dataChanged();
}

void
CQChartsArrow::
setTo(const Point &v)
{
  to_ = v; emit dataChanged();
}

#if 0
void
CQChartsArrow::
setRelative(bool b)
{
  data_.setRelative(b); emit dataChanged();
}
#endif

//---

void
CQChartsArrow::
setLineWidth(const Length &l)
{
  data_.setLineWidth(l); emit dataChanged();
}

//---

void
CQChartsArrow::
setFrontVisible(bool b)
{
  data_.setFHead(b); emit dataChanged();
}

void
CQChartsArrow::
setFrontType(const HeadType &type)
{
  data_.setFHeadType((ArrowData::HeadType) type); emit dataChanged();
}

void
CQChartsArrow::
setFrontAngle(const Angle &a)
{
  data_.setFrontAngle(a); emit dataChanged();
}

void
CQChartsArrow::
setFrontBackAngle(const Angle &a)
{
  data_.setFrontBackAngle(a); emit dataChanged();
}

void
CQChartsArrow::
setFrontLength(const Length &l)
{
  data_.setFrontLength(l); emit dataChanged();
}

void
CQChartsArrow::
setFrontLineEnds(bool b)
{
  data_.setFrontLineEnds(b); emit dataChanged();
}

//---

void
CQChartsArrow::
setTailVisible(bool b)
{
  data_.setTHead(b); emit dataChanged();
}

void
CQChartsArrow::
setTailType(const HeadType &type)
{
  data_.setTHeadType((ArrowData::HeadType) type); emit dataChanged();
}

void
CQChartsArrow::
setTailAngle(const Angle &a)
{
  data_.setTailAngle(a); emit dataChanged();
}

void
CQChartsArrow::
setTailBackAngle(const Angle &a)
{
  data_.setTailBackAngle(a); emit dataChanged();
}

void
CQChartsArrow::
setTailLength(const Length &l)
{
  data_.setTailLength(l); emit dataChanged();
}

void
CQChartsArrow::
setTailLineEnds(bool b)
{
  data_.setTailLineEnds(b); emit dataChanged();
}

//---

void
CQChartsArrow::
setAngle(const Angle &a)
{
  data_.setAngle(a); emit dataChanged();
}

void
CQChartsArrow::
setBackAngle(const Angle &a)
{
  data_.setBackAngle(a); emit dataChanged();
}

void
CQChartsArrow::
setLength(const Length &l)
{
  data_.setLength(l); emit dataChanged();
}

void
CQChartsArrow::
setLineEnds(bool b)
{
  data_.setLineEnds(b); emit dataChanged();
}

//---

void
CQChartsArrow::
setRectilinear(bool b)
{
  rectilinear_ = b; emit dataChanged();
}

//---

void
CQChartsArrow::
setData(const ArrowData &data)
{
  data_ = data; emit dataChanged();
}

//---

bool
CQChartsArrow::
isSolid() const
{
  if (isRectilinear())
    return true;

  bool linePoly = (lineWidth().value() > 0);

  if (! linePoly)
    return true;

  bool isFrontSolid = (isFrontVisible() && ! isFrontLineEnds());
  bool isTailSolid  = (isTailVisible () && ! isTailLineEnds ());

  return (! isFrontSolid && ! isTailSolid);
}

void
CQChartsArrow::
draw(CQChartsPaintDevice *device) const
{
  PenBrush penBrush;

  auto fc = interpFillColor  (ColorInd());
  auto sc = interpStrokeColor(ColorInd());

  CQChartsUtil::setPen  (penBrush.pen  , isStroked(), sc, strokeAlpha());
  CQChartsUtil::setBrush(penBrush.brush, isFilled (), fc, fillAlpha(), fillPattern());

  draw(device, penBrush);
}

void
CQChartsArrow::
draw(CQChartsPaintDevice *device, const PenBrush &penBrush) const
{
  device_  = device;

  drawContents(penBrush);

  device_  = nullptr;
}

void
CQChartsArrow::
drawContents(const PenBrush &penBrush) const
{
  frontLine1_.reset();
  frontLine2_.reset();
  endLine1_  .reset();
  endLine2_  .reset();
  midLine_   .reset();
  frontPoly_ .reset();
  tailPoly_  .reset();
  arrowPoly_ .reset();

  //---

#if DEBUG_LABELS
  pointLabels_.clear();

  auto addPointLabel = [&](const Point &point, const QString &text, bool above) {
    pointLabels_.emplace_back(point, text, above);
  };
#endif

  //---

  auto windowToPixel = [&](const Point &w) {
    if      (plot()) return plot()->windowToPixel(w);
    else if (view()) return view()->windowToPixel(w);
    else             return w;
  };

  auto lengthPixelWidth = [&](const CQChartsLength &l) {
    if      (plot()) return plot()->lengthPixelWidth(l);
    else if (view()) return view()->lengthPixelWidth(l);
    else             return l.value();
  };

//auto lengthPixelHeight = [&](const CQChartsLength &l) {
//  if      (plot()) return plot()->lengthPixelHeight(l);
//  else if (view()) return view()->lengthPixelHeight(l);
//  else             return l.value();
//};

  auto lengthLocalWidth = [&](const CQChartsLength &l) {
    if      (plot()) return CQChartsLength(plot()->lengthPlotWidth(l), CQChartsUnits::PLOT);
    else if (view()) return CQChartsLength(view()->lengthViewWidth(l), CQChartsUnits::VIEW);
    else             return l;
  };

//auto lengthLocalHeight = [&](const CQChartsLength &l) {
//  if      (plot()) return CQChartsLength(plot()->lengthPlotHeight(l), CQChartsUnits::PLOT);
//  else if (view()) return CQChartsLength(view()->lengthViewHeight(l), CQChartsUnits::VIEW);
//  else             return l;
//};

  //---

  bool isStroked = (penBrush.pen  .style() != Qt::NoPen  );
  bool isFilled  = (penBrush.brush.style() != Qt::NoBrush);

  //---

  auto from = from_;
  auto to   = to_;
//auto to   = (isRelative() ? from_ + to_ : to_);

  if (from.x > to.x)
    std::swap(from, to);

  //---

  // convert start/end point to pixel start/end point
  auto p1 = windowToPixel(from);
  auto p4 = windowToPixel(to  );

#if DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(p1, "p1", /*above*/false);
    addPointLabel(p4, "p4", /*above*/true );
  }
#endif

  //---

  // convert line width, front/tail arrow length to pixel
  double lpw = lengthPixelWidth (lengthLocalWidth (lineWidth()));
//double lph = lengthPixelHeight(lengthLocalHeight(lineWidth()));

  double fl = (frontLength().value() > 0 ? lengthPixelWidth(frontLength()) : 8);
  double tl = (tailLength ().value() > 0 ? lengthPixelWidth(tailLength ()) : 8);

  bool linePoly = (lineWidth().value() > 0);

  //---

  // calc stroke width
  double strokeWidth =
    (this->strokeWidth().value() >= 0 ? lengthPixelWidth(this->strokeWidth()) : 4);

  //---

  // calc line angle (radians)
  ArrowAngle a;

  if (! isRectilinear())
    a = ArrowAngle(p1, p4);

  // calc front/tail arrow angles (radians)
  ArrowAngle faa(CMathUtil::Deg2Rad(frontAngle().value() > 0 ? frontAngle().value() : 45));
  ArrowAngle taa(CMathUtil::Deg2Rad(tailAngle ().value() > 0 ? tailAngle ().value() : 45));

  // calc front/tail arrow back angles (radians)
  ArrowAngle fba(CMathUtil::Deg2Rad(frontBackAngle().value() > 0 ? frontBackAngle().value() : 90));
  ArrowAngle tba(CMathUtil::Deg2Rad(tailBackAngle ().value() > 0 ? tailBackAngle ().value() : 90));

  if (isRectilinear()) {
    faa = ArrowAngle(CMathUtil::Deg2Rad(30));
    taa = ArrowAngle(CMathUtil::Deg2Rad(30));

    fba = ArrowAngle(CMathUtil::Deg2Rad(90));
    tba = ArrowAngle(CMathUtil::Deg2Rad(90));
  }

  //---

  // calc front/tail arrow length along line
  double fpl1 = fl*faa.cos;
  double tpl1 = tl*taa.cos;

  //---

  // calc front/tail arrow end point (along line)
  auto p2 = movePointOnLine(p1, a,  fpl1);
  auto p3 = movePointOnLine(p4, a, -tpl1);

#if DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(p2, "p2", /*above*/false);
    addPointLabel(p3, "p3", /*above*/true );
  }
#endif

  //---

  bool isFrontLineEnds = false;
  bool isTailLineEnds  = false;

  if (! isRectilinear()) {
    isFrontLineEnds = (this->isFrontLineEnds() && isFrontVisible());
    isTailLineEnds  = (this->isTailLineEnds () && isTailVisible ());
  }

  //---

  // calc front/tail head mid point (on line)
  auto fHeadMid = p1;
  auto tHeadMid = p4;

  if (isFrontVisible()) {
    if (! isFrontLineEnds)
      fHeadMid = movePointOnLine(p1, a,  strokeWidth);
  }

  if (isTailVisible()) {
    if (! isTailLineEnds)
      tHeadMid = movePointOnLine(p4, a, -strokeWidth);
  }

  //---

  // create polygon for arrow shape if has width
  Point pl1, pl2, pl3, pl4;

  if (linePoly) {
    // calc front head mid point offset by line width
    addWidthToPoint(fHeadMid, a, lpw, pl1, pl2);

    // calc tail head mid point offset by line width
    addWidthToPoint(tHeadMid, a, lpw, pl3, pl4);
  }

  //---

  GeomPolygon fHeadPoints;
  Point       pf1, pf2;

  if (isFrontVisible()) {
    // calc front head angle (relative to line)
    auto a1 = ArrowAngle(a.angle + faa.angle);
    auto a2 = ArrowAngle(a.angle - faa.angle);

    //---

    // calc front head arrow tip points
    pf1 = movePointOnLine(p1, a1, fl);
    pf2 = movePointOnLine(p1, a2, fl);

#if DEBUG_LABELS
    if (isDebugLabels()) {
      addPointLabel(pf1, "pf1", /*above*/false);
      addPointLabel(pf2, "pf2", /*above*/true );
    }
#endif

    //---

    if (isFrontLineEnds) {
      if (! linePoly) {
        frontLine1_ = Line(p1, pf1);
        frontLine2_ = Line(p1, pf2);

        drawLine(p1, pf1, strokeWidth, penBrush);
        drawLine(p1, pf2, strokeWidth, penBrush);
      }
      else {
        // calc front head angle (relative to line)
        auto a1 = ArrowAngle(a.angle + faa.angle);
        auto a2 = ArrowAngle(a.angle - faa.angle);

        // calc line points offset from end arrow lines (p1->pf1, p1->pf2)
        auto pf11 = movePointPerpLine(pf1, a1,  lpw);
        auto pf21 = movePointPerpLine(pf2, a2, -lpw);

        // calc point at line width from start point along line
        auto pf31 = movePointOnLine(p1, a, lpw);

        // intersect front head lines to line offset by width
        bool inside;

        Point pf41, pf51;

        intersectLine(pl2, pl4, pf21, pf31, pf41, inside);
        intersectLine(pl1, pl3, pf11, pf31, pf51, inside);

        //---

#if DEBUG_LABELS
        if (isDebugLabels()) {
          addPointLabel(pf11, "pf11", /*above*/false);
          addPointLabel(pf21, "pf21", /*above*/true );
          addPointLabel(pf31, "pf31", /*above*/false);
          addPointLabel(pf41, "pf41", /*above*/true );
          addPointLabel(pf51, "pf51", /*above*/false);
        }
#endif

        fHeadPoints.addPoint(pf41); // intersect with line top
        fHeadPoints.addPoint(pf21); // top arrow line end bottom
        fHeadPoints.addPoint(pf2);  // top arrow line end top
        fHeadPoints.addPoint(p1);   // line start (left)
        fHeadPoints.addPoint(pf1);  // bottom arrow line end bottom
        fHeadPoints.addPoint(pf11); // bottom arrow line end top
        fHeadPoints.addPoint(pf51); // intersect with line bottom
      }
    }
    else {
      auto pf3 = p2;

      // if valid back angle intersect arrow mid line (p1, p2) to back line
      if (fba.angle > faa.angle && fba.angle < M_PI) {
        auto a3 = ArrowAngle(a.angle + fba.angle);

        auto pf1t = movePointOnLine(pf1, a3, -10);

        bool inside;
        intersectLine(p1, p2, pf1, pf1t, pf3, inside);

#if DEBUG_LABELS
        if (isDebugLabels())
          addPointLabel(pf3, "pf3", /*above*/false);
#endif

        fHeadMid = pf3;
      }

      fHeadPoints.addPoint(p1);  // tip (on line)
      fHeadPoints.addPoint(pf1); // tip (below)
      fHeadPoints.addPoint(pf3); // back line intersect or arrow right point (start + length)
      fHeadPoints.addPoint(pf2); // tip (above)

      if (! linePoly) {
        frontPoly_ = fHeadPoints;

        drawPolygon(fHeadPoints, strokeWidth, isFilled, isStroked, penBrush);
      }
    }
  }

  //---

  GeomPolygon tHeadPoints;
  Point       pt1, pt2;

  if (isTailVisible()) {
    // calc tail head angle (relative to line)
    auto a1 = ArrowAngle(a.angle + M_PI - taa.angle);
    auto a2 = ArrowAngle(a.angle + M_PI + taa.angle);

    //---

    // calc tail head arrow tip points
    pt1 = movePointOnLine(p4, a1, tl);
    pt2 = movePointOnLine(p4, a2, tl);

#if DEBUG_LABELS
    if (isDebugLabels()) {
      addPointLabel(pt1, "pt1", /*above*/false);
      addPointLabel(pt2, "pt2", /*above*/true );
    }
#endif

    //---

    if (isTailLineEnds) {
      if (! linePoly) {
        endLine1_ = Line(p4, pt1);
        endLine2_ = Line(p4, pt2);

        drawLine(p4, pt1, strokeWidth, penBrush);
        drawLine(p4, pt2, strokeWidth, penBrush);
      }
      else {
        // calc tail head angle (relative to line)
        auto a1 = ArrowAngle(a.angle + M_PI - taa.angle);
        auto a2 = ArrowAngle(a.angle + M_PI + taa.angle);

        // calc line points offset from end arrow lines (p1->pf1, p1->pf2)
        auto pt11 = movePointPerpLine(pt1, a1, -lpw);
        auto pt21 = movePointPerpLine(pt2, a2,  lpw);

        // calc point at line width from start point along line
        auto pt31 = movePointOnLine(p4, a, -lpw);

        // intersect tail head lines to line offset by width
        bool inside;

        Point pt41, pt51;

        intersectLine(pl2, pl4, pt21, pt31, pt41, inside);
        intersectLine(pl1, pl3, pt11, pt31, pt51, inside);

        //---

#if DEBUG_LABELS
        if (isDebugLabels()) {
          addPointLabel(pt11, "pt11", /*above*/false);
          addPointLabel(pt21, "pt21", /*above*/true );
          addPointLabel(pt31, "pt31", /*above*/false);
          addPointLabel(pt41, "pt41", /*above*/true );
          addPointLabel(pt51, "pt51", /*above*/false);
        }
#endif

        tHeadPoints.addPoint(pt51); // intersect with line bottom
        tHeadPoints.addPoint(pt11); // bottom arrow line end top
        tHeadPoints.addPoint(pt1);  // bottom arrow line end bottom
        tHeadPoints.addPoint(p4);   // line end (right)
        tHeadPoints.addPoint(pt2);  // top arrow line end top
        tHeadPoints.addPoint(pt21); // top arrow line end bottom
        tHeadPoints.addPoint(pt41); // intersect with line top
      }
    }
    else {
      auto pt3 = p3;

      // if valid back angle intersect arrow mid line (p1, p2) to back line
      if (tba.angle > taa.angle && tba.angle < M_PI) {
        auto a3 = ArrowAngle(a.angle + M_PI - tba.angle);

        auto pt1t = movePointOnLine(pt1, a3, -10);

        bool inside;
        intersectLine(p3, p4, pt1, pt1t, pt3, inside);

#if DEBUG_LABELS
        if (isDebugLabels())
          addPointLabel(pt3, "pt3", /*above*/false);
#endif

        tHeadMid = pt3;
      }

      tHeadPoints.addPoint(p4);  // tip (on line)
      tHeadPoints.addPoint(pt1); // tip (below)
      tHeadPoints.addPoint(pt3); // back line intersect or arrow left point (end - length)
      tHeadPoints.addPoint(pt2); // tip (above)

      if (! linePoly) {
        tailPoly_ = tHeadPoints;

        drawPolygon(tHeadPoints, strokeWidth, isFilled, isStroked, penBrush);
      }
    }
  }

  //---

  // update head and tail (non line) polygon for arrow shape with line width
  if (linePoly) {
    if (! isRectilinear()) {
      // intersect front head point lines with arrow line (offset by width)
      if (! fHeadPoints.empty() && ! isFrontLineEnds) {
        Point fl1, fl2;

        int np = fHeadPoints.size();

        auto pi1 = pl1;
        auto pi2 = pl2;

        for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
          bool inside;

          intersectLine(pl1, pl3, fHeadPoints.point(i1), fHeadPoints.point(i2), fl1, inside);

          // if intersect inside, and more to right, update intersection (above)
          if (inside && (fl1.x > pi1.x))
            pi1 = fl1;

          intersectLine(pl2, pl4, fHeadPoints.point(i1), fHeadPoints.point(i2), fl2, inside);

          // if intersect inside, and more to right, update intersection (below)
          if (inside && (fl2.x > pi2.x))
            pi2 = fl2;
        }

        pl1 = pi1;
        pl2 = pi2;
      }

      //---

      // intersect front head point lines with arrow line (offset by width)
      if (! tHeadPoints.empty() && ! isTailLineEnds) {
        Point tl1, tl2;

        int np = tHeadPoints.size();

        auto pi3 = pl3;
        auto pi4 = pl4;

        for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
          bool inside;

          intersectLine(pl1, pl3, tHeadPoints.point(i1), tHeadPoints.point(i2), tl1, inside);

          // if intersect inside, and more to left, update intersection (above)
          if (inside && (tl1.x < pi3.x))
            pi3 = tl1;

          intersectLine(pl2, pl4, tHeadPoints.point(i1), tHeadPoints.point(i2), tl2, inside);

          // if intersect inside, and more to left, update intersection (below)
          if (inside && (tl2.x < pi4.x))
            pi4 = tl2;
        }

        pl3 = pi3;
        pl4 = pi4;
      }
    }
    else {
      if (isFrontVisible())
        addWidthToPoint(p2, a, lpw, pl1, pl2);
      else
        addWidthToPoint(p1, a, lpw, pl1, pl2);

      if (isTailVisible())
        addWidthToPoint(p3, a, lpw, pl3, pl4);
      else
        addWidthToPoint(p4, a, lpw, pl3, pl4);
    }
  }

  //---

#if DEBUG_LABELS
   if (isDebugLabels()) {
     addPointLabel(pl1, "pl1", /*above*/false);
     addPointLabel(pl2, "pl2", /*above*/true );
     addPointLabel(pl3, "pl3", /*above*/false);
     addPointLabel(pl4, "pl4", /*above*/true );
   }
#endif

  //---

  if (linePoly) {
    // draw line polygon (has line width)
    GeomPolygon points;

    auto addFrontPoints = [&]() {
      points.addPoint(pl2); // front head above mid line
      points.addPoint(pf2); // front head tip (above)
      points.addPoint(p1);  // start tip (on line)
      points.addPoint(pf1); // front head tip (below)
      points.addPoint(pl1); // front head below mid line
    };

    auto addTailPoints = [&]() {
      points.addPoint(pl3); // tail head below mid line
      points.addPoint(pt1); // tail head tip (below)
      points.addPoint(p4);  // end tip (on line)
      points.addPoint(pt2); // tail head tip (above)
      points.addPoint(pl4); // tail head above mid line
    };

    auto addFrontLinePoints = [&]() {
      points.addPoint(pl2); // front head above mid line
      points.addPoint(pl1); // front head below mid line
    };

    auto addTailLinePoints = [&]() {
      points.addPoint(pl3); // tail head below mid line
      points.addPoint(pl4); // tail head above mid line
    };

    auto addLMidPoints = [&]() {
      if (isRectilinear()) {
        auto prm = (p1 + p4)/2.0;

        Point plr1, plr2;

        if (p1.y > p4.y) {
          plr1 = Point(prm.x + lpw/2.0, pl1.y);
          plr2 = Point(prm.x + lpw/2.0, pl3.y);
        }
        else {
          plr1 = Point(prm.x - lpw/2.0, pl1.y);
          plr2 = Point(prm.x - lpw/2.0, pl3.y);
        }

        points.addPoint(plr1);
        points.addPoint(plr2);

#if DEBUG_LABELS
       if (isDebugLabels()) {
         addPointLabel(plr1, "plr1", /*above*/false);
         addPointLabel(plr2, "plr2", /*above*/false);
       }
#endif
      }
    };

    auto addUMidPoints = [&]() {
      if (isRectilinear()) {
        auto prm = (p1 + p4)/2.0;

        Point pur1, pur2;

        if (p1.y > p4.y) {
          pur1 = Point(prm.x - lpw/2.0, pl4.y);
          pur2 = Point(prm.x - lpw/2.0, pl2.y);
        }
        else {
          pur1 = Point(prm.x + lpw/2.0, pl4.y);
          pur2 = Point(prm.x + lpw/2.0, pl2.y);
        }

        points.addPoint(pur1);
        points.addPoint(pur2);

#if DEBUG_LABELS
       if (isDebugLabels()) {
         addPointLabel(pur1, "pur1", /*above*/true);
         addPointLabel(pur2, "pur2", /*above*/true);
       }
#endif
      }
    };

    auto addFHeadPoints = [&]() {
      for (auto &p : fHeadPoints.qpoly())
        points.addPoint(p);
    };

    auto addTHeadPoints = [&]() {
      for (auto &p : tHeadPoints.qpoly())
        points.addPoint(p);
    };

    //---

    if      (! isFrontLineEnds && ! isTailLineEnds) {
      if      (isFrontVisible() && isTailVisible()) {
        addFrontPoints(); addLMidPoints();
        addTailPoints (); addUMidPoints();
      }
      else if (isTailVisible()) {
        addFrontLinePoints(); addLMidPoints();
        addTailPoints     (); addUMidPoints();
      }
      else if (isFrontVisible()) {
        addFrontPoints   (); addLMidPoints();
        addTailLinePoints(); addUMidPoints();
      }
      else {
        addFrontLinePoints(); addLMidPoints();
        addTailLinePoints (); addUMidPoints();
      }
    }
    else if (isFrontLineEnds && ! isTailLineEnds) {
      if (isTailVisible()) {
        addFHeadPoints();
        addTailPoints ();
      }
      else {
        addFHeadPoints   ();
        addTailLinePoints();
      }
    }
    else if (isTailLineEnds && ! isFrontLineEnds) {
      if (isFrontVisible()) {
        addFrontPoints();
        addTHeadPoints();
      }
      else {
        addFrontLinePoints();
        addTHeadPoints    ();
      }
    }
    else {
      addFHeadPoints();
      addTHeadPoints();
    }

    arrowPoly_ = points;

    drawPolygon(points, strokeWidth, isFilled, isStroked, penBrush);
  }
  else {
    // draw line (no line width)
    midLine_ = Line(fHeadMid, tHeadMid);

    drawLine(fHeadMid, tHeadMid, strokeWidth, penBrush);
  }

  //---

#if DEBUG_LABELS
  // draw debug labels
  for (const auto &pointLabel : pointLabels_)
    drawPointLabel(pointLabel.point, pointLabel.text, pointLabel.above);
#endif
}

void
CQChartsArrow::
drawPolygon(const GeomPolygon &points, double width, bool filled, bool stroked,
            const PenBrush &penBrush) const
{
  path_ = QPainterPath();

  auto p0 = device_->pixelToWindow(points.point(0));

  path_.moveTo(p0.qpoint());

  for (int i = 1; i < points.size(); ++i) {
    auto p1 = device_->pixelToWindow(points.point(i));

    path_.lineTo(p1.qpoint());
  }

  path_.closeSubpath();

  //---

  if (filled) {
    device_->fillPath(path_, penBrush.brush);

    if (stroked) {
      auto pen1 = penBrush.pen;

      pen1.setWidthF(width);

      device_->strokePath(path_, pen1);
    }
  }
  else {
    auto pen1 = penBrush.pen;

    if (! stroked)
      pen1 = QPen(Qt::NoPen);

    pen1.setWidthF(width);

    device_->strokePath(path_, pen1);
  }
}

void
CQChartsArrow::
drawLine(const Point &point1, const Point &point2, double width, const PenBrush &penBrush) const
{
  bool isStroked = (penBrush.pen  .style() != Qt::NoPen  );
  bool isFilled  = (penBrush.brush.style() != Qt::NoBrush);

  //---

  auto pixelToWindow = [&](const Point &w) {
    if      (plot()) return plot()->pixelToWindow(w);
    else if (view()) return view()->pixelToWindow(w);
    else             return w;
  };

  //---

  auto pen1 = penBrush.pen;

  if      (isStroked)
    pen1.setColor(penBrush.pen.color());
  else if (isFilled)
    pen1 = QPen(penBrush.brush.color());
  else
    pen1 = QPen(Qt::NoPen);

  pen1.setWidthF(width);

  device_->setPen(pen1);

  auto p1 = pixelToWindow(point1);
  auto p2 = pixelToWindow(point2);

  device_->drawLine(Point(p1), Point(p2));
}

#if DEBUG_LABELS
void
CQChartsArrow::
drawPointLabel(const Point &point, const QString &text, bool above) const
{
  // draw cross symbol
  QPen tpen;

  auto tc = Qt::black;

  CQChartsUtil::setPen(tpen, true, tc);

  device_->setPen(tpen);

  Point p1(point.x - 4, point.y    );
  Point p2(point.x + 4, point.y    );
  Point p3(point.x    , point.y - 4);
  Point p4(point.x    , point.y + 4);

  device_->drawLine(device_->pixelToWindow(p1), device_->pixelToWindow(p2));
  device_->drawLine(device_->pixelToWindow(p3), device_->pixelToWindow(p4));

  //---

  QFontMetricsF fm(device_->font());

  double fw = fm.width(text);
  double fa = fm.ascent();
  double fd = fm.descent();

  Point pt(point.x - fw/2, point.y + (above ? -(fd + 4) : fa + 4));

  CQChartsDrawUtil::drawContrastText(device_, device_->pixelToWindow(pt), text, CQChartsAlpha(0.5));
}
#endif

//---

bool
CQChartsArrow::
contains(const Point &p) const
{
  if (arrowPoly_.valid && arrowPoly_.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (frontPoly_.valid && frontPoly_.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (tailPoly_.valid && tailPoly_.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (frontLine1_.valid && (frontLine1_.distance(p) < 4))
    return true;
  if (frontLine2_.valid && (frontLine2_.distance(p) < 4))
    return true;

  if (endLine1_.valid && (endLine1_.distance(p) < 4))
    return true;
  if (endLine2_.valid && (endLine2_.distance(p) < 4))
    return true;

  if (midLine_.valid && (midLine_.distance(p) < 4))
    return true;

  return false;
}

//---

double
CQChartsArrow::
pointLineDistance(const Point &p, const Point &p1, const Point &p2)
{
  double d;

  (void) CQChartsUtil::PointLineDistance(Point(p), Point(p1), Point(p2), &d);

  return d;
}

//---

void
CQChartsArrow::
write(std::ostream &os, const QString &varName) const
{
  assert(plot());

  auto plotName = [&]() {
    return (varName != "" ? varName : "plot");
  };

  CQPropertyViewModel::NameValues nameValues;

  plot()->propertyModel()->getChangedNameValues(this, nameValues, /*tcl*/true);

  if (! nameValues.empty())
    os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str.clear();

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}

//----

void
CQChartsArrow::
selfPath(QPainterPath &path, const BBox &rect, double fhead, double thead, double lw)
{
  double xr = rect.getWidth ()/2.0;
  double yr = rect.getHeight()/2.0;

  double a = M_PI/4.0;

  double c = std::cos(a);
  double s = std::sin(a);

  double xm = rect.getXMid();
  double ym = rect.getYMid();

  double yt = rect.getYMax() + yr/2.0;

  double x1 = xm - xr*c, y1 = ym + xr*s;
  double x2 = xm + xr*c, y2 = y1;

  QPainterPath lpath;

  lpath.moveTo (QPointF(x1, y1));
  lpath.cubicTo(QPointF(x1, yt), QPointF(x2, yt), QPointF(x2, y2));

  //---

  CQChartsArrowData arrowData;

  arrowData.setFHead(fhead);
  arrowData.setTHead(thead);

  pathAddArrows(lpath, arrowData, lw, 1.0, path);
}

void
CQChartsArrow::
pathAddArrows(const QPainterPath &path, const CQChartsArrowData &arrowData,
              double lw, double alen, QPainterPath &arrowPath)
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(double lw, double alen, const CQChartsArrowData &arrowData) :
     lw_(lw), alen_(alen), arrowData_(arrowData) {
      bool isFHead = arrowData_.isFHead();
      bool isTHead = arrowData_.isTHead();

      arrowData_.setFHeadType(arrowData_.fheadType());
      arrowData_.setTHeadType(arrowData_.theadType());

      arrowData_.setFHead(isFHead);
      arrowData_.setTHead(isTHead);
    }

    void moveTo(const Point &p) override {
      p1_ = p;
      p2_ = p;
    }

    void lineTo(const Point &p) override {
      p1_ = p2_;
      p2_ = p;

      handleFirst();

      //---

      auto a1 = ArrowAngle(p1_, p2_);

      if (nextP != p2_) {
        auto a2 = ArrowAngle(p2_, nextP);

        Point lp1, lp2, lp3, lp4, lp5, lp6, lp7, lp8;

        addWidthToPoint(p1_  , a1, lw_, lp2, lp1); // above/below
        addWidthToPoint(p2_  , a1, lw_, lp4, lp3); // above/below
        addWidthToPoint(p2_  , a2, lw_, lp6, lp5); // above/below
        addWidthToPoint(nextP, a2, lw_, lp8, lp7); // above/below

        Point pi1, pi2;

        CQChartsUtil::intersectLines(lp1, lp3, lp5, lp7, pi1);
        CQChartsUtil::intersectLines(lp2, lp4, lp6, lp8, pi2);

        arrowPath1_.lineTo(pi1.qpoint());
        arrowPath2_.lineTo(pi2.qpoint());
      }
      else {
        //auto pf = (arrowData_.isTHead() && isLast() ? movePointOnLine(p2_, a1, -lw_) : p2_);
        bool skipLast = (arrowData_.isTHead() && isLast());

        Point lp1, lp2;

        addWidthToPoint(p2_, a1, lw_, lp2, lp1); // above/below

        if (! skipLast) {
          arrowPath1_.lineTo(lp1.qpoint());
          arrowPath2_.lineTo(lp2.qpoint());
        }
        else {
          skipN_ = 1;
        }
      }
    }

    void quadTo(const Point &pc1, const Point &p2) override {
      p1_ = p2_;
      p2_ = pc1;

      handleFirst();

      //---

      auto a1 = ArrowAngle(p1_, pc1);
      auto a2 = ArrowAngle(pc1, p2 );

      //auto pf = (arrowData_.isTHead() && isLast() ? movePointOnLine(p2, a2, -lw_) : p2);
      bool skipLast = (arrowData_.isTHead() && isLast());

      Point lp11, lp21, lp12, lp22;

      addWidthToPoint(pc1, a1, lw_, lp21, lp11); // above/below
      addWidthToPoint(p2 , a2, lw_, lp22, lp12); // above/below

      if (! skipLast) {
        arrowPath1_.quadTo(lp11.qpoint(), lp12.qpoint());
        arrowPath2_.quadTo(lp21.qpoint(), lp22.qpoint());
      }
      else {
        skipN_   = 2;
        skipLP1_ = lp11;
        skipUP1_ = lp21;
      }

      //---

      p1_ = pc1;
      p2_ = p2;
    }

    void curveTo(const Point &pc1, const Point &pc2, const Point &p2) override {
      p1_ = p2_;
      p2_ = pc1;

      handleFirst();

      //---

      auto a1 = ArrowAngle(p1_, pc1);
      auto a2 = ArrowAngle(pc1, pc2);
      auto a3 = ArrowAngle(pc2, p2 );

      //auto pf = (arrowData_.isTHead() && isLast() ? movePointOnLine(p2, a3, -lw_) : p2);
      bool skipLast = (arrowData_.isTHead() && isLast());

      Point lp11, lp21, lp12, lp22, lp13, lp23;

      addWidthToPoint(pc1, a1, lw_, lp21, lp11); // above/below
      addWidthToPoint(pc2, a2, lw_, lp22, lp12); // above/below
      addWidthToPoint(p2 , a3, lw_, lp23, lp13); // above/below

      if (! skipLast) {
        arrowPath1_.cubicTo(lp11.qpoint(), lp12.qpoint(), lp13.qpoint());
        arrowPath2_.cubicTo(lp21.qpoint(), lp22.qpoint(), lp23.qpoint());
      }
      else {
        skipN_   = 3;
        skipLP1_ = lp11;
        skipUP1_ = lp21;
        skipLP2_ = lp12;
        skipUP2_ = lp22;
      }

      //---

      p1_ = pc2;
      p2_ = p2;
    }

    void handleFirst() {
      if (first_) {
        ArrowAngle a(p1_, p2_);

        if (arrowData_.isFHead()) {
          // head point bottom/top
          arrowPath1_.moveTo(p1_.qpoint());
          arrowPath2_.moveTo(p1_.qpoint());

          //---

          // move in to arrow right edge
          auto pf = movePointOnLine(p1_, a, alen_*lw_);

          //---

          // get end arrow points bottom top using front angle
          auto a1 = ArrowAngle(a.angle - arrowData_.frontAngle().radians());
          auto a2 = ArrowAngle(a.angle + arrowData_.frontAngle().radians());

          auto pf1 = movePointOnLine(p1_, a1, lw_);
          auto pf2 = movePointOnLine(p1_, a2, lw_);

          Point ap1, ap2, ap3, ap4;

          addWidthToPoint(p1_, a, lw_, ap2, ap1); // above/below
          addWidthToPoint(pf , a, lw_, ap4, ap3); // above/below

          Point pi1, pi2;

          CQChartsUtil::intersectLines(p1_, pf1, pf, ap3, pi1);
          CQChartsUtil::intersectLines(p1_, pf2, pf, ap4, pi2);

          //---

          // get back angle intersection with line border
          auto a3 = ArrowAngle(a.angle - arrowData_.frontBackAngle().radians());
          auto a4 = ArrowAngle(a.angle + arrowData_.frontBackAngle().radians());

          auto pf3 = movePointOnLine(pi1, a3, lw_);
          auto pf4 = movePointOnLine(pi2, a4, lw_);

          Point pi3, pi4;

          CQChartsUtil::intersectLines(pi1, pf3, ap1, ap3, pi3);
          CQChartsUtil::intersectLines(pi2, pf4, ap2, ap4, pi4);

          //---

          arrowPath1_.lineTo(pi1.qpoint());
          arrowPath2_.lineTo(pi2.qpoint());

          arrowPath1_.lineTo(pi3.qpoint());
          arrowPath2_.lineTo(pi4.qpoint());
        }
        else {
          arrowPath1_.moveTo(p1_.qpoint());
          arrowPath2_.moveTo(p1_.qpoint());

          Point lp1, lp2;

          addWidthToPoint(p1_, a, lw_, lp2, lp1); // above/below

          arrowPath1_.lineTo(lp1.qpoint());
          arrowPath2_.lineTo(lp2.qpoint());
        }

        first_ = false;
      }
    }

    bool isLast() const {
      return (i == (n - 1));
    }

    void term() override {
      if (! first_) {
        if (arrowData_.isTHead()) {
          ArrowAngle a(p1_, p2_);

          // move in to arrow left edge
          auto pf = movePointOnLine(p2_, a, -alen_*lw_);

          //---

          // get end arrow points bottom top using front angle
          auto a1 = ArrowAngle(a.angle + M_PI + arrowData_.tailAngle().radians());
          auto a2 = ArrowAngle(a.angle + M_PI - arrowData_.tailAngle().radians());

          auto pf1 = movePointOnLine(p2_, a1, lw_);
          auto pf2 = movePointOnLine(p2_, a2, lw_);

          Point ap1, ap2, ap3, ap4;

          addWidthToPoint(p2_, a, lw_, ap2, ap1); // above/below
          addWidthToPoint(pf , a, lw_, ap4, ap3); // above/below

          Point pi1, pi2;

          CQChartsUtil::intersectLines(p2_, pf1, pf, ap3, pi1);
          CQChartsUtil::intersectLines(p2_, pf2, pf, ap4, pi2);

          //---

          // get back angle intersection with line border
          auto a3 = ArrowAngle(a.angle + M_PI + arrowData_.tailBackAngle().radians());
          auto a4 = ArrowAngle(a.angle + M_PI - arrowData_.tailBackAngle().radians());

          auto pf3 = movePointOnLine(pi1, a3, lw_);
          auto pf4 = movePointOnLine(pi2, a4, lw_);

          Point pi3, pi4;

          CQChartsUtil::intersectLines(pi1, pf3, ap3, ap1, pi3);
          CQChartsUtil::intersectLines(pi2, pf4, ap4, ap2, pi4);

          //---

          assert(skipN_ >= 1 && skipN_ <= 3);

          if      (skipN_ == 1) {
            arrowPath1_.lineTo(pi3.qpoint());
            arrowPath2_.lineTo(pi4.qpoint());
          }
          else if (skipN_ == 2) {
            arrowPath1_.quadTo(skipLP1_.qpoint(), pi3.qpoint());
            arrowPath2_.quadTo(skipUP1_.qpoint(), pi4.qpoint());
          }
          else if (skipN_ == 3) {
            arrowPath1_.cubicTo(skipLP1_.qpoint(), skipLP2_.qpoint(), pi3.qpoint());
            arrowPath2_.cubicTo(skipUP1_.qpoint(), skipUP2_.qpoint(), pi4.qpoint());
          }

          arrowPath1_.lineTo(pi1.qpoint());
          arrowPath2_.lineTo(pi2.qpoint());

          //---

          // tail point bottom/top
          arrowPath1_.lineTo(p2_.qpoint());
          arrowPath2_.lineTo(p2_.qpoint());
        }
      }

      //printPath("arrowPath1", arrowPath1_);
      //printPath("arrowPath2", arrowPath2_);

      arrowPath2_ = CQChartsPath::reversePath(arrowPath2_);
      //printPath("reversed", arrowPath2_);

      arrowPath1_ = CQChartsPath::combinePaths(arrowPath1_, arrowPath2_);
      //printPath("combined", arrowPath1_);
    }

#if 0
    void printPath(const QString &name, const QPainterPath &path) {
      std::cerr << name.toStdString() << ": " <<
        CQChartsSVGUtil::pathToString(path).toStdString() << "\n";
    }
#endif

    const QPainterPath &arrowPath() const { return arrowPath1_; }

   private:
    double            lw_      { 1.0 };         // line width
    double            alen_    { 1.0 };         // arrow length
    CQChartsArrowData arrowData_;               // arrow data
    Point             p1_, p2_;                 // start is front point, end is end point
    bool              first_   { true };        // is first point
    QPainterPath      arrowPath1_, arrowPath2_; // top, bottom path
    int               skipN_   { 0 };
    Point             skipLP1_;
    Point             skipUP1_;
    Point             skipLP2_;
    Point             skipUP2_;
  };

  //---

  PathVisitor visitor(lw, alen, arrowData);

  CQChartsDrawUtil::visitPath(path, visitor);

  arrowPath = visitor.arrowPath();
}

void
CQChartsArrow::
addWidthToPoint(const Point &p, const ArrowAngle &a, double lw, Point &p1, Point &p2)
{
  double dx = lw*a.sin/2.0;
  double dy = lw*a.cos/2.0;

  p1 = Point(p.x - dx, p.y + dy); // above
  p2 = Point(p.x + dx, p.y - dy); // below
}

CQChartsArrow::Point
CQChartsArrow::
movePointOnLine(const Point &p, const ArrowAngle &a, double d)
{
  return Point(p.x + d*a.cos, p.y + d*a.sin);
}

CQChartsArrow::Point
CQChartsArrow::
movePointPerpLine(const Point &p, const ArrowAngle &a, double d)
{
  return Point(p.x + d*a.sin, p.y - d*a.cos);
}

bool
CQChartsArrow::
intersectLine(const Point &l1s, const Point &l1e,
              const Point &l2s, const Point &l2e, Point &pi, bool &inside)
{
  double xi, yi;

  inside = CQChartsUtil::intersectLines(l1s.x, l1s.y, l1e.x, l1e.y,
                                        l2s.x, l2s.y, l2e.x, l2e.y, xi, yi);

  pi = Point(xi, yi);

  return inside;
}
