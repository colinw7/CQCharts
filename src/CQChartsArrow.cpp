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
  if (b != data_.isFHead()) {
    data_.setFHead(b);

    if (b) {
      if (data_.fheadType() == ArrowData::HeadType::NONE)
        data_.setFHeadType(ArrowData::HeadType::ARROW);
    }
    else {
      if (data_.fheadType() != ArrowData::HeadType::NONE)
        data_.setFHeadType(ArrowData::HeadType::NONE);
    }

    emit dataChanged();
  }
}

void
CQChartsArrow::
setFrontType(const HeadType &type)
{
  data_.setFHeadType(static_cast<ArrowData::HeadType>(type)); emit dataChanged();
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
  if (b != data_.isTHead()) {
    data_.setTHead(b);

    if (b) {
      if (data_.fheadType() == ArrowData::HeadType::NONE)
        data_.setFHeadType(ArrowData::HeadType::ARROW);
    }
    else {
      if (data_.fheadType() != ArrowData::HeadType::NONE)
        data_.setFHeadType(ArrowData::HeadType::NONE);
    }

    emit dataChanged();
  }
}

void
CQChartsArrow::
setTailType(const HeadType &type)
{
  data_.setTHeadType(static_cast<ArrowData::HeadType>(type)); emit dataChanged();
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
#if DEBUG_LABELS
  drawData_.debugLabels = isDebugLabels();
#endif

  drawContents(device, from_, to_, data_, strokeWidth(), isRectilinear(), penBrush, drawData_);
}

void
CQChartsArrow::
drawContents(PaintDevice *device, const Point &from, const Point &to,
             const ArrowData &arrowData, const Length &strokeWidth, bool rectilinear,
             const PenBrush &penBrush, DrawData &drawData)
{
  drawData.reset();

  //---

#if DEBUG_LABELS
  drawData.pointLabels.clear();

  auto addPointLabel = [&](const Point &point, const QString &text, bool above) {
    drawData.pointLabels.emplace_back(point, text, above);
  };
#endif

  //---

  auto lengthLocalWidth = [&](const CQChartsLength &l) {
    return CQChartsLength(device->lengthWindowWidth(l), device->parentUnits());
  };

  //---

  bool isStroked = (penBrush.pen  .style() != Qt::NoPen  );
  bool isFilled  = (penBrush.brush.style() != Qt::NoBrush);

  //---

  auto from1 = from;
  auto to1   = to;
//auto to1   = (isRelative() ? from + to : to);

  if (from1.x > to1.x)
    std::swap(from1, to1);

  //---

  // convert start/end point to pixel start/end point
  auto p1 = device->windowToPixel(from1);
  auto p4 = device->windowToPixel(to1  );

#if DEBUG_LABELS
  if (drawData.debugLabels) {
    addPointLabel(p1, "p1", /*above*/false);
    addPointLabel(p4, "p4", /*above*/true );
  }
#endif

  //---

  bool linePoly = (arrowData.lineWidth().value() > 0);

  // convert line width, front/tail arrow length to pixel
  double lpw = 0.0;
//double lph = 0.0;

  if (linePoly) {
    lpw = device->lengthPixelWidth (lengthLocalWidth (arrowData.lineWidth()));
  //lph = device->lengthPixelHeight(lengthLocalHeight(arrowData.lineWidth()));
  }

  double fl = device->lengthPixelWidth(arrowData.calcFrontLength());
  double tl = device->lengthPixelWidth(arrowData.calcTailLength ());

  //---

  // calc stroke width
  double strokeWidth1 = (strokeWidth.value() >= 0 ? device->lengthPixelWidth(strokeWidth) : 4);

  //---

  // calc line angle (radians)
  ArrowAngle a;

  if (! rectilinear)
    a = ArrowAngle(p1, p4);

  // calc front/tail arrow angles (radians)
  ArrowAngle faa(CMathUtil::Deg2Rad(arrowData.calcFrontAngle().value()));
  ArrowAngle taa(CMathUtil::Deg2Rad(arrowData.calcTailAngle ().value()));

  // calc front/tail arrow back angles (radians)
  ArrowAngle fba(CMathUtil::Deg2Rad(arrowData.calcFrontBackAngle().value()));
  ArrowAngle tba(CMathUtil::Deg2Rad(arrowData.calcTailBackAngle ().value()));

  if (rectilinear) {
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
  if (drawData.debugLabels) {
    addPointLabel(p2, "p2", /*above*/false);
    addPointLabel(p3, "p3", /*above*/true );
  }
#endif

  //---

  bool isFrontLineEnds = false;
  bool isTailLineEnds  = false;

  if (! rectilinear) {
    isFrontLineEnds = (arrowData.isFrontLineEnds() && arrowData.calcIsFHead());
    isTailLineEnds  = (arrowData.isTailLineEnds () && arrowData.calcIsTHead());
  }

  //---

  // calc front/tail head mid point (on line)
  auto fHeadMid = p1;
  auto tHeadMid = p4;

  if (arrowData.calcIsFHead()) {
    if (! isFrontLineEnds)
      fHeadMid = movePointOnLine(p1, a,  strokeWidth1);
  }

  if (arrowData.calcIsTHead()) {
    if (! isTailLineEnds)
      tHeadMid = movePointOnLine(p4, a, -strokeWidth1);
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

  if (arrowData.calcIsFHead()) {
    // calc front head angle (relative to line)
    auto a1 = ArrowAngle(a.angle + faa.angle);
    auto a2 = ArrowAngle(a.angle - faa.angle);

    //---

    // calc front head arrow tip points
    pf1 = movePointOnLine(p1, a1, fl);
    pf2 = movePointOnLine(p1, a2, fl);

#if DEBUG_LABELS
    if (drawData.debugLabels) {
      addPointLabel(pf1, "pf1", /*above*/false);
      addPointLabel(pf2, "pf2", /*above*/true );
    }
#endif

    //---

    if (isFrontLineEnds) {
      if (! linePoly) {
        drawData.frontLine1 = Line(p1, pf1);
        drawData.frontLine2 = Line(p1, pf2);

        drawLine(device, p1, pf1, strokeWidth1, penBrush);
        drawLine(device, p1, pf2, strokeWidth1, penBrush);
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
        if (drawData.debugLabels) {
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
        if (drawData.debugLabels)
          addPointLabel(pf3, "pf3", /*above*/false);
#endif

        fHeadMid = pf3;
      }

      fHeadPoints.addPoint(p1);  // tip (on line)
      fHeadPoints.addPoint(pf1); // tip (below)
      fHeadPoints.addPoint(pf3); // back line intersect or arrow right point (start + length)
      fHeadPoints.addPoint(pf2); // tip (above)

      if (! linePoly) {
        drawData.frontPoly = fHeadPoints;

        drawPolygon(device, fHeadPoints, strokeWidth1, isFilled, isStroked,
                    penBrush, drawData.path);
      }
    }
  }

  //---

  GeomPolygon tHeadPoints;
  Point       pt1, pt2;

  if (arrowData.calcIsTHead()) {
    // calc tail head angle (relative to line)
    auto a1 = ArrowAngle(a.angle + M_PI - taa.angle);
    auto a2 = ArrowAngle(a.angle + M_PI + taa.angle);

    //---

    // calc tail head arrow tip points
    pt1 = movePointOnLine(p4, a1, tl);
    pt2 = movePointOnLine(p4, a2, tl);

#if DEBUG_LABELS
    if (drawData.debugLabels) {
      addPointLabel(pt1, "pt1", /*above*/false);
      addPointLabel(pt2, "pt2", /*above*/true );
    }
#endif

    //---

    if (isTailLineEnds) {
      if (! linePoly) {
        drawData.endLine1 = Line(p4, pt1);
        drawData.endLine2 = Line(p4, pt2);

        drawLine(device, p4, pt1, strokeWidth1, penBrush);
        drawLine(device, p4, pt2, strokeWidth1, penBrush);
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
        if (drawData.debugLabels) {
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
        if (drawData.debugLabels)
          addPointLabel(pt3, "pt3", /*above*/false);
#endif

        tHeadMid = pt3;
      }

      tHeadPoints.addPoint(p4);  // tip (on line)
      tHeadPoints.addPoint(pt1); // tip (below)
      tHeadPoints.addPoint(pt3); // back line intersect or arrow left point (end - length)
      tHeadPoints.addPoint(pt2); // tip (above)

      if (! linePoly) {
        drawData.tailPoly = tHeadPoints;

        drawPolygon(device, tHeadPoints, strokeWidth1, isFilled, isStroked,
                    penBrush, drawData.path);
      }
    }
  }

  //---

  // update head and tail (non line) polygon for arrow shape with line width
  if (linePoly) {
    if (! rectilinear) {
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
      if (arrowData.calcIsFHead())
        addWidthToPoint(p2, a, lpw, pl1, pl2);
      else
        addWidthToPoint(p1, a, lpw, pl1, pl2);

      if (arrowData.calcIsTHead())
        addWidthToPoint(p3, a, lpw, pl3, pl4);
      else
        addWidthToPoint(p4, a, lpw, pl3, pl4);
    }
  }

  //---

#if DEBUG_LABELS
   if (drawData.debugLabels) {
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
      if (rectilinear) {
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
        if (drawData.debugLabels) {
          addPointLabel(plr1, "plr1", /*above*/false);
          addPointLabel(plr2, "plr2", /*above*/false);
        }
#endif
      }
    };

    auto addUMidPoints = [&]() {
      if (rectilinear) {
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
        if (drawData.debugLabels) {
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
      if      (arrowData.calcIsFHead() && arrowData.calcIsTHead()) {
        addFrontPoints(); addLMidPoints();
        addTailPoints (); addUMidPoints();
      }
      else if (arrowData.calcIsTHead()) {
        addFrontLinePoints(); addLMidPoints();
        addTailPoints     (); addUMidPoints();
      }
      else if (arrowData.calcIsFHead()) {
        addFrontPoints   (); addLMidPoints();
        addTailLinePoints(); addUMidPoints();
      }
      else {
        addFrontLinePoints(); addLMidPoints();
        addTailLinePoints (); addUMidPoints();
      }
    }
    else if (isFrontLineEnds && ! isTailLineEnds) {
      if (arrowData.calcIsTHead()) {
        addFHeadPoints();
        addTailPoints ();
      }
      else {
        addFHeadPoints   ();
        addTailLinePoints();
      }
    }
    else if (isTailLineEnds && ! isFrontLineEnds) {
      if (arrowData.calcIsFHead()) {
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

    drawData.arrowPoly = points;

    drawPolygon(device, points, strokeWidth1, isFilled, isStroked, penBrush, drawData.path);
  }
  else {
    // draw line (no line width)
    drawData.midLine = Line(fHeadMid, tHeadMid);

    drawLine(device, fHeadMid, tHeadMid, strokeWidth1, penBrush);
  }

  //---

#if DEBUG_LABELS
  // draw debug labels
  for (const auto &pointLabel : drawData.pointLabels)
    drawPointLabel(device, pointLabel.point, pointLabel.text, pointLabel.above);
#endif
}

void
CQChartsArrow::
drawPolygon(PaintDevice *device, const GeomPolygon &points, double width,
            bool filled, bool stroked, const PenBrush &penBrush,
            QPainterPath &path)
{
  path = QPainterPath();

  auto p0 = device->pixelToWindow(points.point(0));

  path.moveTo(p0.qpoint());

  for (int i = 1; i < points.size(); ++i) {
    auto p1 = device->pixelToWindow(points.point(i));

    path.lineTo(p1.qpoint());
  }

  path.closeSubpath();

  //---

  if (filled) {
    device->fillPath(path, penBrush.brush);

    if (stroked) {
      auto pen1 = penBrush.pen;

      pen1.setWidthF(width);

      device->strokePath(path, pen1);
    }
  }
  else {
    auto pen1 = penBrush.pen;

    if (! stroked)
      pen1 = QPen(Qt::NoPen);

    pen1.setWidthF(width);

    device->strokePath(path, pen1);
  }
}

void
CQChartsArrow::
drawLine(PaintDevice *device, const Point &point1, const Point &point2,
         double width, const PenBrush &penBrush)
{
  bool isStroked = (penBrush.pen  .style() != Qt::NoPen  );
  bool isFilled  = (penBrush.brush.style() != Qt::NoBrush);

  //---

  auto pen1 = penBrush.pen;

  if      (isStroked)
    pen1.setColor(penBrush.pen.color());
  else if (isFilled)
    pen1 = QPen(penBrush.brush.color());
  else
    pen1 = QPen(Qt::NoPen);

  pen1.setWidthF(width);

  device->setPen(pen1);

  auto p1 = device->pixelToWindow(point1);
  auto p2 = device->pixelToWindow(point2);

  device->drawLine(Point(p1), Point(p2));
}

#if DEBUG_LABELS
void
CQChartsArrow::
drawPointLabel(PaintDevice *device, const Point &point, const QString &text, bool above)
{
  // draw cross symbol
  QPen tpen;

  auto tc = Qt::black;

  CQChartsUtil::setPen(tpen, true, tc);

  device->setPen(tpen);

  Point p1(point.x - 4, point.y    );
  Point p2(point.x + 4, point.y    );
  Point p3(point.x    , point.y - 4);
  Point p4(point.x    , point.y + 4);

  device->drawLine(device->pixelToWindow(p1), device->pixelToWindow(p2));
  device->drawLine(device->pixelToWindow(p3), device->pixelToWindow(p4));

  //---

  QFontMetricsF fm(device->font());

  double fw = fm.width(text);
  double fa = fm.ascent();
  double fd = fm.descent();

  Point pt(point.x - fw/2, point.y + (above ? -(fd + 4) : fa + 4));

  CQChartsDrawUtil::drawContrastText(device, device->pixelToWindow(pt), text, CQChartsAlpha(0.5));
}
#endif

//---

bool
CQChartsArrow::
contains(const Point &p) const
{
  if (drawData_.arrowPoly.valid && drawData_.arrowPoly.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (drawData_.frontPoly.valid && drawData_.frontPoly.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (drawData_.tailPoly.valid && drawData_.tailPoly.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (drawData_.frontLine1.valid && (drawData_.frontLine1.distance(p) < 4))
    return true;
  if (drawData_.frontLine2.valid && (drawData_.frontLine2.distance(p) < 4))
    return true;

  if (drawData_.endLine1.valid && (drawData_.endLine1.distance(p) < 4))
    return true;
  if (drawData_.endLine2.valid && (drawData_.endLine2.distance(p) < 4))
    return true;

  if (drawData_.midLine.valid && (drawData_.midLine.distance(p) < 4))
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
drawArrow(PaintDevice *device, const Point &from, const Point &to,
          const ArrowData &data, const Length &strokeWidth, bool rectilinear,
          const PenBrush &penBrush)
{
  DrawData drawData;

  drawContents(device, from, to, data, strokeWidth, rectilinear, penBrush, drawData);
}

void
CQChartsArrow::
selfPath(QPainterPath &path, const BBox &rect, bool fhead, bool thead, double lw)
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

  arrowData.setFHeadType(fhead ? ArrowData::HeadType::ARROW : ArrowData::HeadType::NONE);
  arrowData.setTHeadType(thead ? ArrowData::HeadType::ARROW : ArrowData::HeadType::NONE);

  pathAddArrows(lpath, arrowData, lw, 1.0, 1.0, path);
}

void
CQChartsArrow::
pathAddArrows(const QPainterPath &path, const CQChartsArrowData &arrowData,
              double lw, double frontLen, double tailLen, QPainterPath &arrowPath)
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(double lw, double frontLen, double tailLen, const CQChartsArrowData &arrowData) :
     lw_(lw), frontLen_(frontLen), tailLen_(tailLen), arrowData_(arrowData) {
      bool isFHead = arrowData_.calcIsFHead();
      bool isTHead = arrowData_.calcIsTHead();

      arrowData_.setFHeadType(arrowData_.fheadType());
      arrowData_.setTHeadType(arrowData_.theadType());

      arrowData_.setFHeadType(isFHead ? arrowData.fheadType() : ArrowData::HeadType::NONE);
      arrowData_.setTHeadType(isTHead ? arrowData.theadType() : ArrowData::HeadType::NONE);
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

        Point  pi1, pi2;
        double mu1, mu2;

        if (CQChartsUtil::intersectLines(lp1, lp3, lp5, lp7, pi1, mu1, mu2) &&
            CQChartsUtil::intersectLines(lp2, lp4, lp6, lp8, pi2, mu1, mu2)) {
          arrowPath1_.lineTo(pi1.qpoint());
          arrowPath2_.lineTo(pi2.qpoint());
        }
      }
      else {
        //auto pf = (arrowData_.calcIsTHead() && isLast() ? movePointOnLine(p2_, a1, -lw_) : p2_);
        bool skipLast = (arrowData_.calcIsTHead() && isLast());

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

      //auto pf = (arrowData_.calcIsTHead() && isLast() ? movePointOnLine(p2, a2, -lw_) : p2);
      bool skipLast = (arrowData_.calcIsTHead() && isLast());

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

      //auto pf = (arrowData_.calcIsTHead() && isLast() ? movePointOnLine(p2, a3, -lw_) : p2);
      bool skipLast = (arrowData_.calcIsTHead() && isLast());

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

        if (arrowData_.calcIsFHead()) {
          // head point bottom/top
          arrowPath1_.moveTo(p1_.qpoint());
          arrowPath2_.moveTo(p1_.qpoint());

          //---

          // move in to arrow right edge
          auto pf = movePointOnLine(p1_, a, frontLen_*lw_);

          //---

          // get end arrow points bottom top using front angle
          auto a1 = ArrowAngle(a.angle - arrowData_.calcFrontAngle().radians());
          auto a2 = ArrowAngle(a.angle + arrowData_.calcFrontAngle().radians());

          auto pf1 = movePointOnLine(p1_, a1, lw_);
          auto pf2 = movePointOnLine(p1_, a2, lw_);

          Point ap1, ap2, ap3, ap4;

          addWidthToPoint(p1_, a, lw_, ap2, ap1); // above/below
          addWidthToPoint(pf , a, lw_, ap4, ap3); // above/below

          Point  pi1, pi2;
          double mu1, mu2;

          if (CQChartsUtil::intersectLines(p1_, pf1, pf, ap3, pi1, mu1, mu2) &&
              CQChartsUtil::intersectLines(p1_, pf2, pf, ap4, pi2, mu1, mu2)) {
            // get back angle intersection with line border
            auto a3 = ArrowAngle(a.angle - arrowData_.calcFrontBackAngle().radians());
            auto a4 = ArrowAngle(a.angle + arrowData_.calcFrontBackAngle().radians());

            auto pf3 = movePointOnLine(pi1, a3, lw_);
            auto pf4 = movePointOnLine(pi2, a4, lw_);

            Point pi3, pi4;

            if (CQChartsUtil::intersectLines(pi1, pf3, ap1, ap3, pi3, mu1, mu2) &&
                CQChartsUtil::intersectLines(pi2, pf4, ap2, ap4, pi4, mu1, mu2)) {
              arrowPath1_.lineTo(pi1.qpoint());
              arrowPath2_.lineTo(pi2.qpoint());

              arrowPath1_.lineTo(pi3.qpoint());
              arrowPath2_.lineTo(pi4.qpoint());
            }
          }
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
        if (arrowData_.calcIsTHead()) {
          ArrowAngle a(p1_, p2_);

          // move in to arrow left edge
          auto pf = movePointOnLine(p2_, a, -tailLen_*lw_);

          //---

          // get end arrow points bottom top using front angle
          auto a1 = ArrowAngle(a.angle + M_PI + arrowData_.calcTailAngle().radians());
          auto a2 = ArrowAngle(a.angle + M_PI - arrowData_.calcTailAngle().radians());

          auto pf1 = movePointOnLine(p2_, a1, lw_);
          auto pf2 = movePointOnLine(p2_, a2, lw_);

          Point ap1, ap2, ap3, ap4;

          addWidthToPoint(p2_, a, lw_, ap2, ap1); // above/below
          addWidthToPoint(pf , a, lw_, ap4, ap3); // above/below

          Point  pi1, pi2;
          double mu1, mu2;

          if (CQChartsUtil::intersectLines(p2_, pf1, pf, ap3, pi1, mu1, mu2) &&
              CQChartsUtil::intersectLines(p2_, pf2, pf, ap4, pi2, mu1, mu2)) {
            // get back angle intersection with line border
            auto a3 = ArrowAngle(a.angle + M_PI + arrowData_.calcTailBackAngle().radians());
            auto a4 = ArrowAngle(a.angle + M_PI - arrowData_.calcTailBackAngle().radians());

            auto pf3 = movePointOnLine(pi1, a3, lw_);
            auto pf4 = movePointOnLine(pi2, a4, lw_);

            Point pi3, pi4;

            if (CQChartsUtil::intersectLines(pi1, pf3, ap3, ap1, pi3, mu1, mu2) &&
                CQChartsUtil::intersectLines(pi2, pf4, ap4, ap2, pi4, mu1, mu2)) {
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
    double            lw_       { 1.0 };  // line width
    double            frontLen_ { 1.0 };  // arrow length
    double            tailLen_  { 1.0 };  // arrow length
    CQChartsArrowData arrowData_;         // arrow data
    Point             p1_, p2_;           // start is front point, end is end point
    bool              first_    { true }; // is first point
    QPainterPath      arrowPath1_;        // top path
    QPainterPath      arrowPath2_;        // bottom path
    int               skipN_    { 0 };
    Point             skipLP1_;
    Point             skipUP1_;
    Point             skipLP2_;
    Point             skipUP2_;
  };

  //---

  PathVisitor visitor(lw, frontLen, tailLen, arrowData);

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
  double xi, yi, mu1, mu2;

  inside = CQChartsUtil::intersectLines(l1s.x, l1s.y, l1e.x, l1e.y,
                                        l2s.x, l2s.y, l2e.x, l2e.y, xi, yi, mu1, mu2);

  pi = Point(xi, yi);

  return inside;
}
