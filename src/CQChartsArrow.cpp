#include <CQChartsArrow.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsSmooth.h>

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

CQChartsView *
CQChartsArrow::
view() const
{
  return view_.data();
}

CQChartsPlot *
CQChartsArrow::
plot() const
{
  return plot_.data();
}

void
CQChartsArrow::
setVisible(bool b)
{
  visible_ = b; Q_EMIT dataChanged();
}

void
CQChartsArrow::
setFrom(const Point &v)
{
  from_ = v; Q_EMIT dataChanged();
}

void
CQChartsArrow::
setTo(const Point &v)
{
  to_ = v; Q_EMIT dataChanged();
}

#if 0
void
CQChartsArrow::
setRelative(bool b)
{
  data_.setRelative(b); Q_EMIT dataChanged();
}
#endif

//---

void
CQChartsArrow::
setLineWidth(const Length &l)
{
  data_.setLineWidth(l); Q_EMIT dataChanged();
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

    Q_EMIT dataChanged();
  }
}

void
CQChartsArrow::
setFrontType(const HeadType &type)
{
  data_.setFHeadType(static_cast<ArrowData::HeadType>(type)); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setFrontAngle(const Angle &a)
{
  data_.setFrontAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setFrontBackAngle(const Angle &a)
{
  data_.setFrontBackAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setFrontLength(const Length &l)
{
  data_.setFrontLength(l); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setFrontLineEnds(bool b)
{
  data_.setFrontLineEnds(b); Q_EMIT dataChanged();
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

    Q_EMIT dataChanged();
  }
}

void
CQChartsArrow::
setTailType(const HeadType &type)
{
  data_.setTHeadType(static_cast<ArrowData::HeadType>(type)); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setTailAngle(const Angle &a)
{
  data_.setTailAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setTailBackAngle(const Angle &a)
{
  data_.setTailBackAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setTailLength(const Length &l)
{
  data_.setTailLength(l); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setTailLineEnds(bool b)
{
  data_.setTailLineEnds(b); Q_EMIT dataChanged();
}

//---

void
CQChartsArrow::
setMidVisible(bool b)
{
  if (b != data_.isMidHead()) {
    data_.setMidHead(b);

    Q_EMIT dataChanged();
  }
}

void
CQChartsArrow::
setMidType(const HeadType &type)
{
  data_.setMidHeadType(static_cast<ArrowData::HeadType>(type)); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setMidAngle(const Angle &a)
{
  data_.setMidAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setMidBackAngle(const Angle &a)
{
  data_.setMidBackAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setMidLength(const Length &l)
{
  data_.setMidLength(l); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setMidLineEnds(bool b)
{
  data_.setMidLineEnds(b); Q_EMIT dataChanged();
}

//---

void
CQChartsArrow::
setAngle(const Angle &a)
{
  data_.setAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setBackAngle(const Angle &a)
{
  data_.setBackAngle(a); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setLength(const Length &l)
{
  data_.setLength(l); Q_EMIT dataChanged();
}

void
CQChartsArrow::
setLineEnds(bool b)
{
  data_.setLineEnds(b); Q_EMIT dataChanged();
}

//---

void
CQChartsArrow::
setRectilinear(bool b)
{
  rectilinear_ = b; Q_EMIT dataChanged();
}

//---

void
CQChartsArrow::
setData(const ArrowData &data)
{
  data_ = data; Q_EMIT dataChanged();
}

//---

bool
CQChartsArrow::
isSolid() const
{
  if (isRectilinear())
    return true;

  bool linePoly = (lineWidth().value() > 0);
  if (linePoly) return true;

  bool isFrontSolid = (isFrontVisible() && ! isFrontLineEnds());
  bool isTailSolid  = (isTailVisible () && ! isTailLineEnds ());

  return (! isFrontSolid && ! isTailSolid);
}

//---

void
CQChartsArrow::
connectDisconnectDataChanged(bool b, const QObject *obj, const char *slotName) const
{
  if (b)
    connect(this, SIGNAL(dataChanged()), obj, slotName);
  else
    disconnect(this, SIGNAL(dataChanged()), obj, slotName);
}

//---

void
CQChartsArrow::
draw(PaintDevice *device) const
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
draw(PaintDevice *device, const PenBrush &penBrush) const
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

  bool swapped = false;

  if (from1.x > to1.x) {
    std::swap(from1, to1);

    swapped = true;
  }

  //---

  // convert start/end point to pixel start/end point
  auto startPoint = device->windowToPixel(from1);
  auto endPoint   = device->windowToPixel(to1  );

#if DEBUG_LABELS
  if (drawData.debugLabels) {
    addPointLabel(startPoint, "startPoint", /*above*/false);
    addPointLabel(endPoint  , "endPoint"  , /*above*/true );
  }
#endif

  //---

  bool linePoly = (arrowData.lineWidth().value() > 0);

  // convert line width, front/tail arrow length to pixel
  double linePixelWidth  = 0.0;
//double linePixelHeight = 0.0;

  if (linePoly) {
    linePixelWidth  = device->lengthPixelWidth (lengthLocalWidth (arrowData.lineWidth()));
  //linePixelHeight = device->lengthPixelHeight(lengthLocalHeight(arrowData.lineWidth()));
  }

  // calc stroke width
  double strokeWidth1 = (strokeWidth.value() >= 0 ? device->lengthPixelWidth(strokeWidth) : 4);

  //---

  GenHeadData frontData, midData, tailData;

  // set arrow len
  frontData.len = device->lengthPixelWidth(arrowData.calcFrontLength());
  midData  .len = device->lengthPixelWidth(arrowData.calcMidLength  ());
  tailData .len = device->lengthPixelWidth(arrowData.calcTailLength ());

  //---

  // calc line angle (radians)
  ArrowAngle lineAngle;

  if (! rectilinear)
    lineAngle = ArrowAngle(startPoint, endPoint);

  // calc front/tail arrow angles (radians)
  frontData.angle = ArrowAngle(CMathUtil::Deg2Rad(arrowData.calcFrontAngle().value()));
  midData  .angle = ArrowAngle(CMathUtil::Deg2Rad(arrowData.calcMidAngle  ().value()));
  tailData .angle = ArrowAngle(CMathUtil::Deg2Rad(arrowData.calcTailAngle ().value()));

  // calc front/tail arrow back angles (radians)
  frontData.backAngle = ArrowAngle(CMathUtil::Deg2Rad(arrowData.calcFrontBackAngle().value()));
  midData  .backAngle = ArrowAngle(CMathUtil::Deg2Rad(arrowData.calcMidBackAngle  ().value()));
  tailData .backAngle = ArrowAngle(CMathUtil::Deg2Rad(arrowData.calcTailBackAngle ().value()));

  if (rectilinear) {
    frontData.angle = ArrowAngle(CMathUtil::Deg2Rad(30));
    midData  .angle = ArrowAngle(CMathUtil::Deg2Rad(30));
    tailData .angle = ArrowAngle(CMathUtil::Deg2Rad(30));

    frontData.backAngle = ArrowAngle(CMathUtil::Deg2Rad(90));
    midData  .backAngle = ArrowAngle(CMathUtil::Deg2Rad(90));
    tailData .backAngle = ArrowAngle(CMathUtil::Deg2Rad(90));
  }

  //---

  // calc front/tail arrow length along line
  frontData.lineLen = frontData.len*frontData.angle.cos;
  midData  .lineLen = midData  .len*midData  .angle.cos;
  tailData .lineLen = tailData .len*tailData .angle.cos;

  //---

  midData.headMid = Point((startPoint.x + endPoint.x)/2.0, (startPoint.y + endPoint.y)/2.0);

  // calc front/tail arrow end point (along line)
  auto startPointI = movePointOnLine(startPoint, lineAngle,  frontData.lineLen);
  auto endPointI   = movePointOnLine(endPoint  , lineAngle, -tailData .lineLen);

  auto headMidL = movePointOnLine(midData.headMid, lineAngle, -midData.lineLen/2.0);
  auto headMidR = movePointOnLine(midData.headMid, lineAngle,  midData.lineLen/2.0);

#if DEBUG_LABELS
  if (drawData.debugLabels) {
    addPointLabel(startPointI, "startPointI", /*above*/false);
    addPointLabel(endPointI  , "endPointI"  , /*above*/true );

    addPointLabel(midData.headMid, "midData.headMid", /*above*/true );
    addPointLabel(headMidL, "headMidL", /*above*/true );
    addPointLabel(headMidR, "headMidR", /*above*/false);
  }
#endif

  //---

  frontData.isLineEnds = false;
  midData  .isLineEnds = false;
  tailData .isLineEnds = false;

  if (! rectilinear) {
    frontData.isLineEnds = (arrowData.isFrontLineEnds() && arrowData.calcIsFHead  ());
    midData  .isLineEnds = (arrowData.isMidLineEnds  () && arrowData.calcIsMidHead());
    tailData .isLineEnds = (arrowData.isTailLineEnds () && arrowData.calcIsTHead  ());
  }

  frontData.isPoints = (! arrowData.isFrontLineEnds() && arrowData.calcIsFHead());
  midData  .isPoints = (! arrowData.isMidLineEnds  () && arrowData.calcIsMidHead());
  tailData .isPoints = (! arrowData.isTailLineEnds () && arrowData.calcIsTHead());

  //---

  // calc front/tail head mid point (on line)
  frontData.headMid = startPoint;
  tailData .headMid = endPoint;

  if (arrowData.calcIsFHead()) {
    if (! frontData.isLineEnds)
      frontData.headMid = movePointOnLine(startPoint, lineAngle, strokeWidth1);
  }

  if (arrowData.calcIsTHead()) {
    if (! tailData.isLineEnds)
      tailData.headMid = movePointOnLine(endPoint, lineAngle, -strokeWidth1);
  }

  //---

  // create polygon for arrow shape if has width
  Point headMidR1, headMidR2;

  if (linePoly) {
    // calc front head mid point offset by line width
    addWidthToPoint(frontData.headMid, lineAngle, linePixelWidth,
                    frontData.headMid1, frontData.headMid2);

    // calc tail head mid point offset by line width
    addWidthToPoint(tailData.headMid, lineAngle, linePixelWidth,
                    tailData.headMid1, tailData.headMid2);

    // calc mid head mid point offset by line width
    addWidthToPoint(midData.headMid, lineAngle, linePixelWidth,
                    midData.headMid1, midData.headMid2);

    if (! swapped)
      addWidthToPoint(headMidR, lineAngle, linePixelWidth, headMidR1, headMidR2);
    else
      addWidthToPoint(headMidL, lineAngle, linePixelWidth, headMidR1, headMidR2);
  }

  //---

  if (arrowData.calcIsFHead())
    calcHeadPolyData(startPoint, startPointI, lineAngle, linePoly,
                     linePixelWidth, frontData, tailData, drawData);

  if (arrowData.calcIsTHead())
    calcTailPolyData(endPoint, endPointI, lineAngle, linePoly,
                     linePixelWidth, frontData, tailData, drawData);

  if (arrowData.calcIsMidHead())
    calcMidPolyData(startPoint, endPoint, lineAngle, linePoly, swapped,
                    linePixelWidth, headMidR1, headMidR2,
                    frontData, tailData, midData, drawData);

  //---

  if (frontData.isLineEnds && ! linePoly) {
    drawLine(device, startPoint, frontData.tipPoint1, strokeWidth1, penBrush);
    drawLine(device, startPoint, frontData.tipPoint2, strokeWidth1, penBrush);
  }

  if (tailData.isLineEnds && ! linePoly) {
    drawLine(device, endPoint, tailData.tipPoint1, strokeWidth1, penBrush);
    drawLine(device, endPoint, tailData.tipPoint2, strokeWidth1, penBrush);
  }

  if (midData.isLineEnds && ! linePoly) {
    drawLine(device, midData.headMid, midData.tipPoint1, strokeWidth1, penBrush);
    drawLine(device, midData.headMid, midData.tipPoint2, strokeWidth1, penBrush);
  }

  //---

  // draw line (no line width)
  if (! linePoly) {
    // draw line (no line width)
    drawData.midLine = Line(frontData.headMid, tailData.headMid);

    drawLine(device, frontData.headMid, tailData.headMid, strokeWidth1, penBrush);
  }

  //---

  // draw arrows (no line width and not line ends)
  if (arrowData.calcIsFHead() && ! frontData.isLineEnds && ! linePoly)
    drawPolygon(device, frontData.headPoints1, strokeWidth1, isFilled, isStroked,
                penBrush, drawData.path);

  if (arrowData.calcIsTHead() && ! tailData.isLineEnds && ! linePoly)
    drawPolygon(device, tailData.headPoints1, strokeWidth1, isFilled, isStroked,
                penBrush, drawData.path);

  if (arrowData.calcIsMidHead() && ! midData.isLineEnds && ! linePoly)
    drawPolygon(device, midData.headPoints1, strokeWidth1, isFilled, isStroked,
                penBrush, drawData.path);

  //---

  // update head and tail (non line) polygon for arrow shape with line width
  if (linePoly) {
    if (! rectilinear) {
      if (! frontData.headPoints1.empty() && ! frontData.isLineEnds)
        updateFrontLinePoly(frontData, tailData);

      if (! tailData.headPoints1.empty() && ! tailData.isLineEnds)
        updateTailLinePoly(frontData, tailData);

      if (! midData.headPoints1.empty() && ! midData.isLineEnds)
        updateMidLinePoly(midData, frontData, tailData);
    }
    else {
      if (arrowData.calcIsFHead())
        addWidthToPoint(startPointI, lineAngle, linePixelWidth,
                        frontData.headMid1, frontData.headMid2);
      else
        addWidthToPoint(startPoint, lineAngle, linePixelWidth,
                        frontData.headMid1, frontData.headMid2);

      if (arrowData.calcIsTHead())
        addWidthToPoint(endPointI, lineAngle, linePixelWidth, tailData.headMid1, tailData.headMid2);
      else
        addWidthToPoint(endPoint, lineAngle, linePixelWidth, tailData.headMid1, tailData.headMid2);
    }

    //---

#if DEBUG_LABELS
    if (drawData.debugLabels) {
      addPointLabel(frontData.headMid1, "frontData.headMid1", /*above*/false);
      addPointLabel(frontData.headMid2, "frontData.headMid2", /*above*/true );

      addPointLabel(tailData.headMid1, "tailData.headMid1", /*above*/false);
      addPointLabel(tailData.headMid2, "tailData.headMid2", /*above*/true );

      addPointLabel(midData.headMid1, "midData.headMid1", /*above*/false);
      addPointLabel(midData.headMid2, "midData.headMid2", /*above*/true );
    }
#endif

    //---

    // draw line polygon (has line width)
    GeomPolygon points;

    // arrow front (line width)
    auto addFrontPoints = [&]() {
      points.addPoint(frontData.headMid2); // front head above mid line
      points.addPoint(frontData.tipPoint2); // front head tip (above)
      points.addPoint(startPoint);  // start tip (on line)
      points.addPoint(frontData.tipPoint1); // front head tip (below)
      points.addPoint(frontData.headMid1); // front head below mid line
    };

    // arrow tail (line width)
    auto addTailPoints = [&]() {
      points.addPoint(tailData.headMid1); // tail head below mid line
      points.addPoint(tailData.tipPoint1); // tail head tip (below)
      points.addPoint(endPoint);  // end tip (on line)
      points.addPoint(tailData.tipPoint2); // tail head tip (above)
      points.addPoint(tailData.headMid2); // tail head above mid line
    };

    // arrow mid (line width)
    auto addMidPoints1 = [&]() {
      if (! swapped) {
        points.addPoint(midData.backLine1);
        points.addPoint(midData.tipPoint1);
        points.addPoint(headMidR1);
      }
      else {
        points.addPoint(headMidR1);
        points.addPoint(midData.tipPoint1);
        points.addPoint(midData.backLine1);
      }
    };

    auto addMidPoints2 = [&]() {
      if (! swapped) {
        points.addPoint(headMidR2);
        points.addPoint(midData.tipPoint2);
        points.addPoint(midData.backLine2);
      }
      else {
        points.addPoint(midData.backLine2);
        points.addPoint(midData.tipPoint2);
        points.addPoint(headMidR2);
      }
    };

    // flat front
    auto addFrontLinePoints = [&]() {
      points.addPoint(frontData.headMid2); // front head above mid line
      points.addPoint(frontData.headMid1); // front head below mid line
    };

    // flat fail
    auto addTailLinePoints = [&]() {
      points.addPoint(tailData.headMid1); // tail head below mid line
      points.addPoint(tailData.headMid2); // tail head above mid line
    };

    auto addLMidPoints = [&]() {
      if (rectilinear) {
        auto prm = (startPoint + endPoint)/2.0;

        Point plr1, plr2;

        if (startPoint.y > endPoint.y) {
          plr1 = Point(prm.x + linePixelWidth/2.0, frontData.headMid1.y);
          plr2 = Point(prm.x + linePixelWidth/2.0, tailData.headMid1.y);
        }
        else {
          plr1 = Point(prm.x - linePixelWidth/2.0, frontData.headMid1.y);
          plr2 = Point(prm.x - linePixelWidth/2.0, tailData.headMid1.y);
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
        auto prm = (startPoint + endPoint)/2.0;

        Point pur1, pur2;

        if (startPoint.y > endPoint.y) {
          pur1 = Point(prm.x - linePixelWidth/2.0, tailData.headMid2.y);
          pur2 = Point(prm.x - linePixelWidth/2.0, frontData.headMid2.y);
        }
        else {
          pur1 = Point(prm.x + linePixelWidth/2.0, tailData.headMid2.y);
          pur2 = Point(prm.x + linePixelWidth/2.0, frontData.headMid2.y);
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

    // line front (no width)
    auto addFHeadPoints = [&]() {
      for (auto &p : frontData.headPoints1.qpoly())
        points.addPoint(p);
    };

    // line tail (no width)
    auto addTHeadPoints = [&]() {
      for (auto &p : tailData.headPoints1.qpoly())
        points.addPoint(p);
    };

    // line mid
    auto addMidHeadPoints1 = [&]() {
      for (auto &p : midData.headPoints1.qpoly())
        points.addPoint(p);
    };

    auto addMidHeadPoints2 = [&]() {
      for (auto &p : midData.headPoints2.qpoly())
        points.addPoint(p);
    };

    //---

    if      (frontData.isLineEnds)
      addFHeadPoints();
    else if (frontData.isPoints)
      addFrontPoints();
    else
      addFrontLinePoints();

    if      (midData.isLineEnds)
      addMidHeadPoints1();
    else if (midData.isPoints)
      addMidPoints1();

    addLMidPoints();

    if      (tailData.isLineEnds)
      addTHeadPoints();
    else if (tailData.isPoints)
      addTailPoints();
    else
      addTailLinePoints();

    addUMidPoints();

    if      (midData.isLineEnds)
      addMidHeadPoints2();
    else if (midData.isPoints)
      addMidPoints2();

    drawData.arrowPoly = points;

    drawPolygon(device, points, strokeWidth1, isFilled, isStroked, penBrush, drawData.path);
  }

  //---

#if DEBUG_LABELS
  // draw debug labels
  for (const auto &pointLabel : drawData.pointLabels)
    CQChartsDrawUtil::drawPointLabel(device, device->pixelToWindow(pointLabel.point),
                                     pointLabel.text, pointLabel.above);
#endif
}

void
CQChartsArrow::
calcHeadPolyData(const Point &startPoint, const Point &startPointI,
                 const ArrowAngle &lineAngle, bool linePoly, double linePixelWidth,
                 GenHeadData &frontData, const GenHeadData &tailData, DrawData &drawData)
{
#if DEBUG_LABELS
  auto addPointLabel = [&](const Point &point, const QString &text, bool above) {
    drawData.pointLabels.emplace_back(point, text, above);
  };
#endif

  // calc front head angle (relative to line)
  auto frontAngle1 = ArrowAngle(lineAngle.angle + frontData.angle.angle);
  auto frontAngle2 = ArrowAngle(lineAngle.angle - frontData.angle.angle);

  //---

  // calc front head arrow tip points
  frontData.tipPoint1 = movePointOnLine(startPoint, frontAngle1, frontData.len);
  frontData.tipPoint2 = movePointOnLine(startPoint, frontAngle2, frontData.len);

#if DEBUG_LABELS
  if (drawData.debugLabels) {
    addPointLabel(frontData.tipPoint1, "frontData.tipPoint1", /*above*/false);
    addPointLabel(frontData.tipPoint2, "frontData.tipPoint2", /*above*/true );
  }
#endif

  //---

  if (frontData.isLineEnds) {
    if (! linePoly) {
      drawData.frontLine1 = Line(startPoint, frontData.tipPoint1);
      drawData.frontLine2 = Line(startPoint, frontData.tipPoint2);
    }
    else {
      // calc front head angle (relative to line)
      auto frontAngle1 = ArrowAngle(lineAngle.angle + frontData.angle.angle);
      auto frontAngle2 = ArrowAngle(lineAngle.angle - frontData.angle.angle);

      // calc line points offset from end arrow lines
      // (startPoint->frontData.tipPoint1, startPoint->frontData.tipPoint2)
      auto frontTip11 = movePointPerpLine(frontData.tipPoint1, frontAngle1,  linePixelWidth);
      auto frontTip21 = movePointPerpLine(frontData.tipPoint2, frontAngle2, -linePixelWidth);

      // calc point at line width from start point along line
      auto startPointI1 = movePointOnLine(startPoint, lineAngle, linePixelWidth);

      // intersect front head lines to line offset by width
      bool inside;

      Point startPointI11, startPointI21;

      intersectLine(frontData.headMid2, tailData.headMid2,
                    frontTip21, startPointI1, startPointI11, inside);
      intersectLine(frontData.headMid1, tailData.headMid1,
                    frontTip11, startPointI1, startPointI21, inside);

      //---

#if DEBUG_LABELS
      if (drawData.debugLabels) {
        addPointLabel(frontTip11   , "frontTip11"   , /*above*/false);
        addPointLabel(frontTip21   , "frontTip21"   , /*above*/true );
        addPointLabel(startPointI1 , "startPointI1" , /*above*/false);
        addPointLabel(startPointI11, "startPointI11", /*above*/true );
        addPointLabel(startPointI21, "startPointI21", /*above*/false);
      }
#endif

      frontData.headPoints1.addPoint(startPointI11);        // intersect with line top
      frontData.headPoints1.addPoint(frontTip21);           // top arrow line end bottom
      frontData.headPoints1.addPoint(frontData.tipPoint2);  // top arrow line end top
      frontData.headPoints1.addPoint(startPoint);           // line start (left)
      frontData.headPoints1.addPoint(frontData.tipPoint1);  // bottom arrow line end bottom
      frontData.headPoints1.addPoint(frontTip11);           // bottom arrow line end top
      frontData.headPoints1.addPoint(startPointI21);        // intersect with line bottom
    }
  }
  else {
    auto backTip1 = startPointI;

    // if valid back angle intersect arrow mid line (startPoint, startPointI) to back line
    if (frontData.backAngle.angle > frontData.angle.angle && frontData.backAngle.angle < M_PI) {
      auto a3 = ArrowAngle(lineAngle.angle + frontData.backAngle.angle);

      auto backTip2 = movePointOnLine(frontData.tipPoint1, a3, -10);

      bool inside;
      intersectLine(startPoint, startPointI, frontData.tipPoint1, backTip2, backTip1, inside);

#if DEBUG_LABELS
      if (drawData.debugLabels)
        addPointLabel(backTip1, "backTip1", /*above*/false);
#endif

      frontData.headMid = backTip1;
    }

    frontData.headPoints1.addPoint(startPoint);          // tip (on line)
    frontData.headPoints1.addPoint(frontData.tipPoint1); // tip (below)
    frontData.headPoints1.addPoint(backTip1);            // back line intersect or arrow right
                                                         // point (start + length)
    frontData.headPoints1.addPoint(frontData.tipPoint2); // tip (above)

    if (! linePoly)
      drawData.frontPoly = frontData.headPoints1;
  }
}

void
CQChartsArrow::
calcTailPolyData(const Point &endPoint, const Point &endPointI,
                 const ArrowAngle &lineAngle, bool linePoly, double linePixelWidth,
                 const GenHeadData &frontData, GenHeadData &tailData, DrawData &drawData)
{
#if DEBUG_LABELS
  auto addPointLabel = [&](const Point &point, const QString &text, bool above) {
    drawData.pointLabels.emplace_back(point, text, above);
  };
#endif

  // calc tail head angle (relative to line)
  auto tailAngle1 = ArrowAngle(lineAngle.angle + M_PI - tailData.angle.angle);
  auto tailAngle2 = ArrowAngle(lineAngle.angle + M_PI + tailData.angle.angle);

  //---

  // calc tail head arrow tip points
  tailData.tipPoint1 = movePointOnLine(endPoint, tailAngle1, tailData.len);
  tailData.tipPoint2 = movePointOnLine(endPoint, tailAngle2, tailData.len);

#if DEBUG_LABELS
  if (drawData.debugLabels) {
    addPointLabel(tailData.tipPoint1, "tailData.tipPoint1", /*above*/false);
    addPointLabel(tailData.tipPoint2, "tailData.tipPoint2", /*above*/true );
  }
#endif

  //---

  if (tailData.isLineEnds) {
    if (! linePoly) {
      drawData.endLine1 = Line(endPoint, tailData.tipPoint1);
      drawData.endLine2 = Line(endPoint, tailData.tipPoint2);
    }
    else {
      // calc tail head angle (relative to line)
      auto tailAngle1 = ArrowAngle(lineAngle.angle + M_PI - tailData.angle.angle);
      auto tailAngle2 = ArrowAngle(lineAngle.angle + M_PI + tailData.angle.angle);

      // calc line points offset from end arrow lines
      // (startPoint->frontData.tipPoint1, startPoint->frontData.tipPoint2)
      auto tailTip11 = movePointPerpLine(tailData.tipPoint1, tailAngle1, -linePixelWidth);
      auto tailTip21 = movePointPerpLine(tailData.tipPoint2, tailAngle2,  linePixelWidth);

      // calc point at line width from start point along line
      auto endPointI1 = movePointOnLine(endPoint, lineAngle, -linePixelWidth);

      // intersect tail head lines to line offset by width
      bool inside;

      Point endPointI11, endPointI12;

      intersectLine(frontData.headMid2, tailData.headMid2,
                    tailTip21, endPointI1, endPointI11, inside);
      intersectLine(frontData.headMid1, tailData.headMid1,
                    tailTip11, endPointI1, endPointI12, inside);

      //---

#if DEBUG_LABELS
      if (drawData.debugLabels) {
        addPointLabel(tailTip11  , "tailTip11"  , /*above*/false);
        addPointLabel(tailTip21  , "tailTip21"  , /*above*/true );
        addPointLabel(endPointI1 , "endPointI1" , /*above*/false);
        addPointLabel(endPointI11, "endPointI11", /*above*/true );
        addPointLabel(endPointI12, "endPointI12", /*above*/false);
      }
#endif

      tailData.headPoints1.addPoint(endPointI12);        // intersect with line bottom
      tailData.headPoints1.addPoint(tailTip11);          // bottom arrow line end top
      tailData.headPoints1.addPoint(tailData.tipPoint1); // bottom arrow line end bottom
      tailData.headPoints1.addPoint(endPoint);           // line end (right)
      tailData.headPoints1.addPoint(tailData.tipPoint2); // top arrow line end top
      tailData.headPoints1.addPoint(tailTip21);          // top arrow line end bottom
      tailData.headPoints1.addPoint(endPointI11);        // intersect with line top
    }
  }
  else {
    auto pt3 = endPointI;

    // if valid back angle intersect arrow mid line (startPoint, startPointI) to back line
    if (tailData.backAngle.angle > tailData.angle.angle && tailData.backAngle.angle < M_PI) {
      auto a3 = ArrowAngle(lineAngle.angle + M_PI - tailData.backAngle.angle);

      auto tailTip11 = movePointOnLine(tailData.tipPoint1, a3, -10);

      bool inside;
      intersectLine(endPointI, endPoint, tailData.tipPoint1, tailTip11, pt3, inside);

#if DEBUG_LABELS
      if (drawData.debugLabels)
        addPointLabel(pt3, "pt3", /*above*/false);
#endif

      tailData.headMid = pt3;
    }

    tailData.headPoints1.addPoint(endPoint);           // tip (on line)
    tailData.headPoints1.addPoint(tailData.tipPoint1); // tip (below)
    tailData.headPoints1.addPoint(pt3);                // back line intersect or arrow left
                                                       // point (end - length)
    tailData.headPoints1.addPoint(tailData.tipPoint2); // tip (above)

    if (! linePoly)
      drawData.tailPoly = tailData.headPoints1;
  }
}

void
CQChartsArrow::
calcMidPolyData(const Point &startPoint, const Point &endPoint,
                const ArrowAngle &lineAngle, bool linePoly, bool swapped, double linePixelWidth,
                const Point &headMidR1, const Point &headMidR2,
                const GenHeadData &frontData, const GenHeadData &tailData,
                GenHeadData &midData, DrawData &drawData)
{
#if DEBUG_LABELS
  auto addPointLabel = [&](const Point &point, const QString &text, bool above) {
    drawData.pointLabels.emplace_back(point, text, above);
  };
#endif

  // calc mid head angle (relative to line)
  ArrowAngle midAngle1, midAngle2;

  if (! swapped) {
    midAngle1 = ArrowAngle(lineAngle.angle + M_PI - midData.angle.angle);
    midAngle2 = ArrowAngle(lineAngle.angle + M_PI + midData.angle.angle);
  }
  else {
    midAngle1 = ArrowAngle(lineAngle.angle + midData.angle.angle);
    midAngle2 = ArrowAngle(lineAngle.angle - midData.angle.angle);
  }

  //---

  // calc mid head arrow tip points
  if (! midData.isLineEnds && linePoly) {
#if DEBUG_LABELS
    if (drawData.debugLabels) {
      addPointLabel(headMidR1, "headMidR1", /*above*/false);
      addPointLabel(headMidR2, "headMidR2", /*above*/true );
    }
#endif

    midData.tipPoint1 = movePointOnLine(headMidR1, midAngle1, midData.len);
    midData.tipPoint2 = movePointOnLine(headMidR2, midAngle2, midData.len);
  }
  else {
    midData.tipPoint1 = movePointOnLine(midData.headMid, midAngle1, midData.len);
    midData.tipPoint2 = movePointOnLine(midData.headMid, midAngle2, midData.len);
  }

#if DEBUG_LABELS
  if (drawData.debugLabels) {
    addPointLabel(midData.tipPoint1, "midData.tipPoint1", /*above*/false);
    addPointLabel(midData.tipPoint2, "midData.tipPoint2", /*above*/true );
  }
#endif

  //---

  if (midData.isLineEnds) {
    if (! linePoly) {
      drawData.endLine1 = Line(midData.headMid, midData.tipPoint1);
      drawData.endLine2 = Line(midData.headMid, midData.tipPoint2);
    }
    else {
      // calc mid head angle (relative to line)
      ArrowAngle midAngle1, midAngle2;

      if (! swapped) {
        midAngle1 = ArrowAngle(lineAngle.angle + M_PI - midData.angle.angle);
        midAngle2 = ArrowAngle(lineAngle.angle + M_PI + midData.angle.angle);
      }
      else {
        midAngle1 = ArrowAngle(lineAngle.angle + midData.angle.angle);
        midAngle2 = ArrowAngle(lineAngle.angle - midData.angle.angle);
      }

      // calc line points offset from end arrow lines
      // (headMidL->midData.tipPoint1, headMidL->midData.tipPoint2)
      auto pmh11 = movePointPerpLine(midData.tipPoint1, midAngle1, -linePixelWidth/2.0);
      auto pmh12 = movePointPerpLine(midData.tipPoint1, midAngle1,  linePixelWidth/2.0);
      auto pmh21 = movePointPerpLine(midData.tipPoint2, midAngle2,  linePixelWidth/2.0);
      auto pmh22 = movePointPerpLine(midData.tipPoint2, midAngle2, -linePixelWidth/2.0);

      // intersect mid head lines to line offset by width
      bool inside;

      Point pmh4, pmh5;

      intersectLine(frontData.headMid1, tailData.headMid1,
                    midData.tipPoint1, midData.headMid, pmh4, inside);
      intersectLine(frontData.headMid2, tailData.headMid2,
                    midData.tipPoint2, midData.headMid, pmh5, inside);

      auto pmh41 = movePointOnLine(pmh4, midAngle1,  linePixelWidth/2.0);
      auto pmh42 = movePointOnLine(pmh4, midAngle1, -linePixelWidth/2.0);
      auto pmh51 = movePointOnLine(pmh5, midAngle2,  linePixelWidth/2.0);
      auto pmh52 = movePointOnLine(pmh5, midAngle2, -linePixelWidth/2.0);

      Point pmh43, pmh44, pmh53, pmh54;

      intersectLine(frontData.headMid1, tailData.headMid1, pmh11, pmh41, pmh43, inside);
      intersectLine(frontData.headMid1, tailData.headMid1, pmh12, pmh42, pmh44, inside);
      intersectLine(frontData.headMid2, tailData.headMid2, pmh21, pmh51, pmh53, inside);
      intersectLine(frontData.headMid2, tailData.headMid2, pmh22, pmh52, pmh54, inside);

      //---

#if DEBUG_LABELS
      if (drawData.debugLabels) {
        addPointLabel(pmh4, "pmh4", /*above*/false);
        addPointLabel(pmh5, "pmh5", /*above*/true );

        addPointLabel(pmh11, "pmh11", /*above*/false);
        addPointLabel(pmh12, "pmh12", /*above*/false);
        addPointLabel(pmh21, "pmh21", /*above*/true );
        addPointLabel(pmh22, "pmh22", /*above*/true );

        addPointLabel(pmh41, "pmh41", /*above*/false);
        addPointLabel(pmh42, "pmh42", /*above*/false);
        addPointLabel(pmh51, "pmh51", /*above*/true );
        addPointLabel(pmh52, "pmh52", /*above*/true );

        addPointLabel(pmh43, "pmh43", /*above*/false);
        addPointLabel(pmh44, "pmh44", /*above*/false);
        addPointLabel(pmh53, "pmh53", /*above*/true );
        addPointLabel(pmh54, "pmh54", /*above*/true );
      }
#endif

      midData.headPoints1.addPoint(pmh44); // intersect with line bottom
      midData.headPoints1.addPoint(pmh11); // bottom arrow line end top
      midData.headPoints1.addPoint(pmh12); // bottom arrow line end bottom
      midData.headPoints1.addPoint(pmh43); // bottom arrow line end bottom

      midData.headPoints2.addPoint(pmh53); // bottom arrow line end bottom
      midData.headPoints2.addPoint(pmh22); // top arrow line end top
      midData.headPoints2.addPoint(pmh21); // top arrow line end bottom
      midData.headPoints2.addPoint(pmh54); // intersect with line top
    }
  }
  else {
    // arrow tip (right)
    // if valid back angle intersect arrow mid line (startPoint, startPointI) to back line
    Point pmh30;

    if (midData.backAngle.angle > midData.angle.angle && midData.backAngle.angle < M_PI) {
      auto a3 = ArrowAngle(lineAngle.angle + M_PI - midData.backAngle.angle);
      auto a4 = ArrowAngle(lineAngle.angle        + midData.backAngle.angle);

      auto dl = 20;

      Point midTip11, midTip21;

      if (! swapped) {
        midTip11 = movePointOnLine(midData.tipPoint1, a3, -dl);
        midTip21 = movePointOnLine(midData.tipPoint2, a4,  dl);
      }
      else {
        midTip11 = movePointOnLine(midData.tipPoint1, a4, -dl);
        midTip21 = movePointOnLine(midData.tipPoint2, a3,  dl);
      }

#if DEBUG_LABELS
      if (drawData.debugLabels) {
        addPointLabel(midTip11, "midTip11", /*above*/false);
        addPointLabel(midTip21, "midTip21", /*above*/false);
      }
#endif

      bool inside;
      intersectLine(startPoint , endPoint , midData.tipPoint1, midTip11, pmh30, inside);

      intersectLine(frontData.headMid1, tailData.headMid1,
                    midData.tipPoint1, midTip11, midData.backLine1, inside);
      intersectLine(frontData.headMid2, tailData.headMid2,
                    midData.tipPoint2, midTip21, midData.backLine2, inside);
    }
    else {
      bool inside;
      intersectLine(startPoint , endPoint , midData.tipPoint1, midData.tipPoint2, pmh30, inside);

      intersectLine(frontData.headMid1, tailData.headMid1,
                    midData.tipPoint1, midData.tipPoint2, midData.backLine1, inside);
      intersectLine(frontData.headMid2, tailData.headMid2,
                    midData.tipPoint1, midData.tipPoint2, midData.backLine2, inside);
    }

#if DEBUG_LABELS
    if (drawData.debugLabels) {
      addPointLabel(midData.backLine1, "midData.backLine1", /*above*/false);
      addPointLabel(midData.backLine2, "midData.backLine2", /*above*/false);
    }
#endif

    midData.headPoints1.addPoint(pmh30);
    midData.headPoints1.addPoint(midData.tipPoint1);
    midData.headPoints1.addPoint(midData.headMid);
    midData.headPoints1.addPoint(midData.tipPoint2);

    if (! linePoly)
      drawData.midPoly = midData.headPoints1;
  }
}

void
CQChartsArrow::
updateFrontLinePoly(GenHeadData &frontData, const GenHeadData &tailData)
{
  // intersect front head point lines with arrow line (offset by width)
  Point headPoint11, headPoint12;

  int np = frontData.headPoints1.size();

  auto headMid11 = frontData.headMid1;
  auto headMid21 = frontData.headMid2;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    bool inside;

    intersectLine(frontData.headMid1, tailData.headMid1,
                  frontData.headPoints1.point(i1), frontData.headPoints1.point(i2),
                  headPoint11, inside);

    // if intersect inside, and more to right, update intersection (above)
    if (inside && (headPoint11.x > headMid11.x))
      headMid11 = headPoint11;

    intersectLine(frontData.headMid2, tailData.headMid2,
                  frontData.headPoints1.point(i1), frontData.headPoints1.point(i2),
                  headPoint12, inside);

    // if intersect inside, and more to right, update intersection (below)
    if (inside && (headPoint12.x > headMid21.x))
      headMid21 = headPoint12;
  }

  frontData.headMid1 = headMid11;
  frontData.headMid2 = headMid21;
}

void
CQChartsArrow::
updateTailLinePoly(const GenHeadData &frontData, GenHeadData &tailData)
{
  // intersect front head point lines with arrow line (offset by width)
  Point tailPoint11, tailPoint12;

  int np = tailData.headPoints1.size();

  auto headMid11 = tailData.headMid1;
  auto headMid21 = tailData.headMid2;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    bool inside;

    intersectLine(frontData.headMid1, tailData.headMid1,
                  tailData.headPoints1.point(i1), tailData.headPoints1.point(i2),
                  tailPoint11, inside);

    // if intersect inside, and more to left, update intersection (above)
    if (inside && (tailPoint11.x < headMid11.x))
      headMid11 = tailPoint11;

    intersectLine(frontData.headMid2, tailData.headMid2,
                  tailData.headPoints1.point(i1), tailData.headPoints1.point(i2),
                  tailPoint12, inside);

    // if intersect inside, and more to left, update intersection (below)
    if (inside && (tailPoint12.x < headMid21.x))
      headMid21 = tailPoint12;
  }

  tailData.headMid1 = headMid11;
  tailData.headMid2 = headMid21;
}

void
CQChartsArrow::
updateMidLinePoly(GenHeadData &midData, const GenHeadData &frontData, const GenHeadData &tailData)
{
  // intersect mid head point lines with arrow line (offset by width)
  Point midPoint11, midPoint12;

  int np = midData.headPoints1.size();

  auto headMid11 = midData.headMid1;
  auto headMid21 = midData.headMid2;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    bool inside;

    intersectLine(frontData.headMid1, tailData.headMid1,
                  midData.headPoints1.point(i1), midData.headPoints1.point(i2), midPoint11, inside);

    // if intersect inside, and more to left, update intersection (above)
    if (inside && (midPoint11.x < headMid11.x))
      headMid11 = midPoint11;

    intersectLine(frontData.headMid2, tailData.headMid2,
                  midData.headPoints1.point(i1), midData.headPoints1.point(i2), midPoint12, inside);

    // if intersect inside, and more to left, update intersection (below)
    if (inside && (midPoint12.x < headMid21.x))
      headMid21 = midPoint12;
  }

  midData.headMid1 = headMid11;
  midData.headMid2 = headMid21;
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
selfPath(PaintDevice *device, QPainterPath &path, const BBox &rect,
         bool fhead, bool thead, double lw)
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

  ArrowData arrowData;

  arrowData.setFHeadType(fhead ? ArrowData::HeadType::ARROW : ArrowData::HeadType::NONE);
  arrowData.setTHeadType(thead ? ArrowData::HeadType::ARROW : ArrowData::HeadType::NONE);

  pathAddArrows(device, lpath, arrowData, lw, Length::factor(1.0), path);
}

// lw in device units
// frontLen, tailLen supports percent so can be scaled to line width
void
CQChartsArrow::
pathAddArrows(PaintDevice *device, const QPainterPath &path, const ArrowData &arrowData,
              double lw, const Length &arrowLen, QPainterPath &arrowPath)
{
  pathAddArrows(device, path, arrowData, lw, lw, arrowLen, arrowLen, arrowPath);
}

void
CQChartsArrow::
pathAddArrows(PaintDevice *device, const QPainterPath &path, const ArrowData &arrowData,
              double xw, double yw, const Length &frontLen, const Length &tailLen,
              QPainterPath &arrowPath)
{
  class PathVisitor : public CQChartsDrawUtil::PathVisitor {
   public:
    PathVisitor(PaintDevice *device, double xw, double yw, double aw,
                double frontLen, double tailLen, const ArrowData &arrowData) :
     device_(device), xw_(xw), yw_(yw), aw_(aw), frontLen_(frontLen), tailLen_(tailLen),
     arrowData_(arrowData) {
      bool isFHead   = arrowData_.calcIsFHead();
      bool isTHead   = arrowData_.calcIsTHead();
      bool isMidHead = arrowData.calcIsMidHead();

      arrowData_.setFHeadType  (arrowData_.fheadType());
      arrowData_.setTHeadType  (arrowData_.theadType());
      arrowData_.setMidHeadType(arrowData_.midHeadType());

      arrowData_.setFHeadType  (isFHead   ? arrowData.fheadType  () : ArrowData::HeadType::NONE);
      arrowData_.setTHeadType  (isTHead   ? arrowData.theadType  () : ArrowData::HeadType::NONE);
      arrowData_.setMidHeadType(isMidHead ? arrowData.midHeadType() : ArrowData::HeadType::NONE);
    }

    void init() override {
      midAngle_ = ArrowAngle(CMathUtil::Deg2Rad(-path->angleAtPercent(0.5)));
      midPoint_ = Point(path->pointAtPercent(0.5));

#if DEBUG_LABELS
      CQChartsDrawUtil::drawPointLabel(device_, midPoint_, "midPoint", false);
#endif
    }

    void moveTo(const Point &p) override {
      p1_ = p;
      p2_ = p;
    }

    void lineTo(const Point &p) override {
      p1_ = p2_;
      p2_ = p;

      handleFirst();

      double d;

      if (CQChartsUtil::PointLineDistance(midPoint_, p1_, p2_, &d)) {
        if (d < 1E-6)
          (void) handleMid();
      }

      //---

      auto a1 = ArrowAngle(p1_, p2_);

      if (nextP != p2_) {
        auto a2 = ArrowAngle(p2_, nextP);

        Point lp1, lp2, lp3, lp4, lp5, lp6, lp7, lp8;

        addWidthToPoint(p1_  , a1, xw_, yw_, lp2, lp1); // above/below
        addWidthToPoint(p2_  , a1, xw_, yw_, lp4, lp3); // above/below
        addWidthToPoint(p2_  , a2, xw_, yw_, lp6, lp5); // above/below
        addWidthToPoint(nextP, a2, xw_, yw_, lp8, lp7); // above/below

        Point  pi1, pi2;
        double mu1, mu2;

        if (CQChartsUtil::intersectLines(lp1, lp3, lp5, lp7, pi1, mu1, mu2) &&
            CQChartsUtil::intersectLines(lp2, lp4, lp6, lp8, pi2, mu1, mu2)) {
          arrowPath1_.lineTo(pi1.qpoint());
          arrowPath2_.lineTo(pi2.qpoint());
        }
        else {
          //std::cerr << "lineTo: Intersect failed\n";
        }
      }
      else {
        //auto pf = (arrowData_.calcIsTHead() && isLast() ? movePointOnLine(p2_, a1, -xw_) : p2_);
        bool skipLast = (arrowData_.calcIsTHead() && isLast());

        Point lp1, lp2;

        addWidthToPoint(p2_, a1, xw_, yw_, lp2, lp1); // above/below

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

      pc1_ = pc1;
      p2_  = p2;

      CQChartsBezier2 bezier(p1_, pc1_, p2_);

      double t;

      if (bezier.interp(midPoint_, &t)) {
        CQChartsBezier2 bezier1, bezier2;

        bezier.split(t, bezier1, bezier2);

        auto a1 = ArrowAngle(bezier1.getFirstPoint  (), bezier1.getControlPoint());
        auto a2 = ArrowAngle(bezier1.getControlPoint(), bezier1.getLastPoint   ());

        Point lp11, lp21, lp12, lp22;

        addWidthToPoint(bezier1.getControlPoint(), a1, xw_, yw_, lp21, lp11); // above/below
        addWidthToPoint(bezier1.getLastPoint   (), a2, xw_, yw_, lp22, lp12); // above/below

#if DEBUG_LABELS
        CQChartsDrawUtil::drawPointLabel(device_, lp12, "lp12", /*above*/false);
        CQChartsDrawUtil::drawPointLabel(device_, lp22, "lp22", /*above*/true);
#endif

        arrowPath1_.quadTo(lp11.qpoint(), lp12.qpoint());
        arrowPath2_.quadTo(lp21.qpoint(), lp22.qpoint());

        if (handleMid()) {
          if (bezier.interp(endPoint_, &t)) {
            bezier.split(t, bezier1, bezier2);
          }
        }

        p1_  = bezier2.getFirstPoint  ();
        pc1_ = bezier2.getControlPoint();
        p2_  = bezier2.getLastPoint   ();
      }

      //---

      auto a1 = ArrowAngle(p1_ , pc1_);
      auto a2 = ArrowAngle(pc1_, p2_ );

      //auto pf = (arrowData_.calcIsTHead() && isLast() ? movePointOnLine(p2, a2, -xw_) : p2);
      bool skipLast = (arrowData_.calcIsTHead() && isLast());

      Point lp11, lp21, lp12, lp22;

      addWidthToPoint(pc1_, a1, xw_, yw_, lp21, lp11); // above/below
      addWidthToPoint(p2  , a2, xw_, yw_, lp22, lp12); // above/below

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

      p1_ = pc1_;
    }

    void curveTo(const Point &pc1, const Point &pc2, const Point &p2) override {
      p1_ = p2_;
      p2_ = pc1;

      handleFirst();

      //---

      pc1_ = pc1;
      pc2_ = pc2;
      p2_  = p2;

      CQChartsBezier3 bezier(p1_, pc1_, pc2_, p2_);

      double t;

      if (bezier.interp(midPoint_, &t)) {
        CQChartsBezier3 bezier1, bezier2;

        bezier.split(t, bezier1, bezier2);

        auto a1 = ArrowAngle(bezier1.getFirstPoint   (), bezier1.getControlPoint1());
        auto a2 = ArrowAngle(bezier1.getControlPoint1(), bezier1.getControlPoint2());
        auto a3 = ArrowAngle(bezier1.getControlPoint2(), bezier1.getLastPoint    ());

        Point lp11, lp21, lp12, lp22, lp13, lp23;

        addWidthToPoint(bezier1.getControlPoint1(), a1, xw_, yw_, lp21, lp11); // above/below
        addWidthToPoint(bezier1.getControlPoint2(), a2, xw_, yw_, lp22, lp12); // above/below
        addWidthToPoint(bezier1.getLastPoint    (), a3, xw_, yw_, lp23, lp13); // above/below

#if DEBUG_LABELS
        CQChartsDrawUtil::drawPointLabel(device_, lp13, "lp13", /*above*/false);
        CQChartsDrawUtil::drawPointLabel(device_, lp23, "lp23", /*above*/true);
#endif

        arrowPath1_.cubicTo(lp11.qpoint(), lp12.qpoint(), lp13.qpoint());
        arrowPath2_.cubicTo(lp21.qpoint(), lp22.qpoint(), lp23.qpoint());

        if (handleMid()) {
          if (bezier.interp(endPoint_, &t)) {
            bezier.split(t, bezier1, bezier2);
          }
        }

        p1_  = bezier2.getFirstPoint   ();
        pc1_ = bezier2.getControlPoint1();
        pc2_ = bezier2.getControlPoint2();
        p2_  = bezier2.getLastPoint    ();
      }

      //---

      auto a1 = ArrowAngle(p1_ , pc1_);
      auto a2 = ArrowAngle(pc1_, pc2_);
      auto a3 = ArrowAngle(pc2_, p2_ );

      //auto pf = (arrowData_.calcIsTHead() && isLast() ? movePointOnLine(p2_, a3, -xw_) : p2_);
      bool skipLast = (arrowData_.calcIsTHead() && isLast());

      Point lp11, lp21, lp12, lp22, lp13, lp23;

      addWidthToPoint(pc1_, a1, xw_, yw_, lp21, lp11); // above/below
      addWidthToPoint(pc2_, a2, xw_, yw_, lp22, lp12); // above/below
      addWidthToPoint(p2_ , a3, xw_, yw_, lp23, lp13); // above/below

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

      p1_ = pc2_;
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
          auto pf = movePointOnLine(p1_, a, frontLen_);

          //---

          // get end arrow points bottom top using front angle
          auto a1 = ArrowAngle(a.angle - arrowData_.calcFrontAngle().radians());
          auto a2 = ArrowAngle(a.angle + arrowData_.calcFrontAngle().radians());

          auto pf1 = movePointOnLine(p1_, a1, aw_);
          auto pf2 = movePointOnLine(p1_, a2, aw_);

          Point ap1, ap2, ap3, ap4;

          addWidthToPoint(p1_, a, xw_, yw_, ap2, ap1); // above/below
          addWidthToPoint(pf , a, xw_, yw_, ap4, ap3); // above/below

          Point  pi1, pi2;
          double mu1, mu2;

          if (CQChartsUtil::intersectLines(p1_, pf1, pf, ap3, pi1, mu1, mu2) &&
              CQChartsUtil::intersectLines(p1_, pf2, pf, ap4, pi2, mu1, mu2)) {
            // get back angle intersection with line border
            auto a3 = ArrowAngle(a.angle - arrowData_.calcFrontBackAngle().radians());
            auto a4 = ArrowAngle(a.angle + arrowData_.calcFrontBackAngle().radians());

            auto pf3 = movePointOnLine(pi1, a3, aw_);
            auto pf4 = movePointOnLine(pi2, a4, aw_);

            Point pi3, pi4;

            if (CQChartsUtil::intersectLines(pi1, pf3, ap1, ap3, pi3, mu1, mu2) &&
                CQChartsUtil::intersectLines(pi2, pf4, ap2, ap4, pi4, mu1, mu2)) {
              arrowPath1_.lineTo(pi1.qpoint());
              arrowPath2_.lineTo(pi2.qpoint());

              arrowPath1_.lineTo(pi3.qpoint());
              arrowPath2_.lineTo(pi4.qpoint());
            }
            else {
              //std::cerr << "handleFirst: Intersect failed\n";
            }
          }
          else {
            //std::cerr << "handleFirst: Intersect failed\n";
          }
        }
        else {
          arrowPath1_.moveTo(p1_.qpoint());
          arrowPath2_.moveTo(p1_.qpoint());

          Point lp1, lp2;

          addWidthToPoint(p1_, a, xw_, yw_, lp2, lp1); // above/below

          arrowPath1_.lineTo(lp1.qpoint());
          arrowPath2_.lineTo(lp2.qpoint());
        }

        first_ = false;
      }
    }

    bool isLast() const {
      return (i == (n - 1));
    }

    bool handleMid() {
      if (! arrowData_.calcIsMidHead())
        return false;

      Point lp1, lp2;
      addWidthToPoint(midPoint_, midAngle_, xw_, yw_, lp2, lp1); // above/below

      auto pathLen = path->length();

      auto lenPercent = std::min(0.5 + (pathLen > 0.0 ? frontLen_/pathLen : 0.0), 1.0);

      Point lp3, lp4;
      addWidthToPoint(midPoint_, midAngle_, xw_ + frontLen_, yw_ + frontLen_,
                      lp4, lp3); // above/below

      endAngle_ = ArrowAngle(CMathUtil::Deg2Rad(-path->angleAtPercent(lenPercent)));
      endPoint_ = Point(path->pointAtPercent(lenPercent));

      Point lp5, lp6;
      addWidthToPoint(endPoint_, endAngle_, xw_, yw_, lp6, lp5); // above/below

      arrowPath1_.lineTo(lp1.qpoint());
      arrowPath1_.lineTo(lp3.qpoint());
      arrowPath1_.lineTo(lp5.qpoint());

      arrowPath2_.lineTo(lp2.qpoint());
      arrowPath2_.lineTo(lp4.qpoint());
      arrowPath2_.lineTo(lp6.qpoint());

#if DEBUG_LABELS
      CQChartsDrawUtil::drawPointLabel(device_, lp3, "lp3", false);
      CQChartsDrawUtil::drawPointLabel(device_, lp4, "lp4", true);
      CQChartsDrawUtil::drawPointLabel(device_, lp5, "lp5", false);
      CQChartsDrawUtil::drawPointLabel(device_, lp6, "lp6", true);
#endif

      return true;
    }

    void term() override {
      if (! first_) {
        if (arrowData_.calcIsTHead()) {
          ArrowAngle a(p1_, p2_);

          // move in to arrow left edge
          auto pf = movePointOnLine(p2_, a, -tailLen_);

          //---

          // get end arrow points bottom top using front angle
          auto a1 = ArrowAngle(a.angle + M_PI + arrowData_.calcTailAngle().radians());
          auto a2 = ArrowAngle(a.angle + M_PI - arrowData_.calcTailAngle().radians());

          auto pf1 = movePointOnLine(p2_, a1, aw_);
          auto pf2 = movePointOnLine(p2_, a2, aw_);

          Point ap1, ap2, ap3, ap4;

          addWidthToPoint(p2_, a, xw_, yw_, ap2, ap1); // above/below
          addWidthToPoint(pf , a, xw_, yw_, ap4, ap3); // above/below

          Point  pi1, pi2;
          double mu1, mu2;

          if (CQChartsUtil::intersectLines(p2_, pf1, pf, ap3, pi1, mu1, mu2) &&
              CQChartsUtil::intersectLines(p2_, pf2, pf, ap4, pi2, mu1, mu2)) {
            // get back angle intersection with line border
            auto a3 = ArrowAngle(a.angle + M_PI + arrowData_.calcTailBackAngle().radians());
            auto a4 = ArrowAngle(a.angle + M_PI - arrowData_.calcTailBackAngle().radians());

            auto pf3 = movePointOnLine(pi1, a3, aw_);
            auto pf4 = movePointOnLine(pi2, a4, aw_);

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
            else {
              //std::cerr << "term: Intersect failed\n";
            }
          }
          else {
            //std::cerr << "term: Intersect failed\n";
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
    PaintDevice *device_ { nullptr };

    double       xw_       { 1.0 };  // x line width
    double       yw_       { 1.0 };  // y line width
    double       aw_       { 1.0 };  // arrow width
    double       frontLen_ { 1.0 };  // arrow front length
    double       tailLen_  { 1.0 };  // arrow tail length
    ArrowData    arrowData_;         // arrow data
    ArrowAngle   midAngle_;
    Point        midPoint_;
    ArrowAngle   endAngle_;
    Point        endPoint_;
    Point        p1_, p2_;           // start is front point, end is end point
    Point        pc1_, pc2_;
    bool         first_    { true }; // is first point
    QPainterPath arrowPath1_;        // top path
    QPainterPath arrowPath2_;        // bottom path
    int          skipN_    { 0 };
    Point        skipLP1_;
    Point        skipUP1_;
    Point        skipLP2_;
    Point        skipUP2_;
  };

  //---

  bool isLine = (xw <= 0.0);

  auto aw = (! isLine ? xw : std::max(-xw, 0.01));

  if (isLine) {
    xw = aw/1000.0;
    yw = xw;
  }

  //---

  double frontLen1, tailLen1;

  if (frontLen.units() == Length::Units::PERCENT)
    frontLen1 = frontLen.value()*aw/100.0;
  else if (device)
    frontLen1 = device->lengthWindowWidth(frontLen);
  else
    frontLen1 = frontLen.value();

  if (tailLen.units() == Length::Units::PERCENT)
    tailLen1 = tailLen.value()*aw/100.0;
  else if (device)
    tailLen1 = device->lengthWindowWidth(tailLen);
  else
    tailLen1 = tailLen.value();

  PathVisitor visitor(device, xw, yw, aw, frontLen1, tailLen1, arrowData);

  CQChartsDrawUtil::visitPath(path, visitor);

  arrowPath = visitor.arrowPath();
}

void
CQChartsArrow::
addWidthToPoint(const Point &p, const ArrowAngle &a, double lw, Point &p1, Point &p2)
{
  addWidthToPoint(p, a, lw, lw, p1, p2);
}

void
CQChartsArrow::
addWidthToPoint(const Point &p, const ArrowAngle &a, double wx, double wy, Point &p1, Point &p2)
{
  double dx = wx*a.sin/2.0;
  double dy = wy*a.cos/2.0;

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
