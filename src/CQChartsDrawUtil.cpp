#include <CQChartsDrawUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsUtil.h>
#include <CQChartsRectiConnect.h>

#include <CQHtmlTextPainter.h>
#include <CMathUtil.h>

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStylePainter>
#include <QStyleOptionSlider>

namespace CQChartsDrawUtil {

void
setPenBrush(PaintDevice *device, const PenBrush &penBrush)
{
  device->setPen       (penBrush.pen);
  device->setBrush     (penBrush.brush);
  device->setAltColor  (penBrush.altColor);
  device->setAltAlpha  (penBrush.altAlpha);
  device->setFillAngle (penBrush.fillAngle);
  device->setFillType  (penBrush.fillType);
  device->setFillRadius(penBrush.fillRadius);
  device->setFillDelta (penBrush.fillDelta);
}

void
setPenBrush(QPainter *painter, const PenBrush &penBrush)
{
  painter->setPen  (penBrush.pen);
  painter->setBrush(penBrush.brush);
}

void
setBrush(QBrush &brush, const BrushData &data)
{
  CQChartsUtil::setBrush(brush, data.isVisible(), data.color(), data.alpha(), data.pattern());
}

void
updateBrushColor(QBrush &brush, const QColor &c)
{
  auto c2 = c;

  c2.setAlphaF(brush.color().alphaF());

  brush.setColor(c2);
}

void
setPenGray(QPen &pen, double alpha)
{
  auto c  = pen.color();
  auto gc = CQChartsUtil::grayColor(c);

  if (alpha > 0.0 && alpha < 1.0)
    gc.setAlphaF(alpha);

  pen.setColor(gc);
}

void
setBrushGray(QBrush &brush, double alpha)
{
  auto c  = brush.color();
  auto gc = CQChartsUtil::grayColor(c);

  if (alpha > 0.0 && alpha < 1.0)
    gc.setAlphaF(alpha);

  brush.setColor(gc);
}

void
setBrushAlpha(QBrush &brush, double a)
{
  auto c = brush.color();

  c.setAlphaF(a);

  brush.setColor(c);
}

void
setPenAlpha(QPen &pen, double a)
{
  auto c = pen.color();

  c.setAlphaF(a);

  pen.setColor(c);
}

//---

void
drawShape(PaintDevice *device, const CQChartsShapeTypeData &data, const BBox &rect)
{
  switch (data.shapeType) {
    case CQChartsShapeType::Type::NONE: {
      break;
    }
    case CQChartsShapeType::Type::BOX: {
      if (data.cornerSize.isSet()) {
        CQChartsDrawUtil::drawRoundedRect(device, rect, data.cornerSize, data.sides, data.angle);
      }
      else {
        if (! data.angle.isZero())
          drawPolygonSides(device, rect, 4, data.angle + Angle::degrees(45));
        else
          device->drawRect(rect);
      }

      break;
    }
    case CQChartsShapeType::Type::POLYGON: {
      // rounded ?
      drawPolygonSides(device, rect, data.numSides > 2 ? data.numSides : 4, data.angle);

      break;
    }
    case CQChartsShapeType::Type::CIRCLE: {
      device->drawEllipse(rect, data.angle);

      break;
    }
    case CQChartsShapeType::Type::DOT: {
      auto xc = rect.getXMid();
      auto yc = rect.getYMid();
      auto w  = 0.1*rect.getWidth();
      auto h  = 0.1*rect.getWidth();

      device->drawEllipse(BBox(xc - w/2.0, yc - h/2.0, xc + w/2.0, yc + h/2.0), data.angle);

      break;
    }
    case CQChartsShapeType::Type::TRIANGLE: {
      // rounded ?
      drawPolygonSides(device, rect, 3, data.angle);

      break;
    }
    case CQChartsShapeType::Type::DIAMOND: {
      if (! data.angle.isZero())
        drawPolygonSides(device, rect, 4, data.angle);
      else
        drawDiamond(device, rect);

      break;
    }
    case CQChartsShapeType::Type::TRAPEZIUM: {
      QPainterPath path;

      auto dx = 0.1*rect.getWidth();

      path.moveTo(rect.getXMin()     , rect.getYMin());
      path.lineTo(rect.getXMin() + dx, rect.getYMin());
      path.lineTo(rect.getXMax() - dx, rect.getYMax());
      path.lineTo(rect.getXMax()     , rect.getYMax());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::PARALLELOGRAM: {
      QPainterPath path;

      auto dx = 0.1*rect.getWidth();

      path.moveTo(rect.getXMin()     , rect.getYMin());
      path.lineTo(rect.getXMin() + dx, rect.getYMax());
      path.lineTo(rect.getXMax()     , rect.getYMax());
      path.lineTo(rect.getXMax() - dx, rect.getYMin());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::HOUSE: {
      QPainterPath path;

      path.moveTo(rect.getXMin(), rect.getYMin());
      path.lineTo(rect.getXMin(), rect.getYMid());
      path.lineTo(rect.getXMid(), rect.getYMax());
      path.lineTo(rect.getXMax(), rect.getYMid());
      path.lineTo(rect.getXMax(), rect.getYMin());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::PENTAGON: {
      drawPolygonSides(device, rect, 5, data.angle);

      break;
    }
    case CQChartsShapeType::Type::HEXAGON: {
      drawPolygonSides(device, rect, 6, data.angle);

      break;
    }
    case CQChartsShapeType::Type::SEPTAGON: {
      drawPolygonSides(device, rect, 7, data.angle);

      break;
    }
    case CQChartsShapeType::Type::OCTAGON: {
      drawPolygonSides(device, rect, 8, data.angle);

      break;
    }
    case CQChartsShapeType::Type::DOUBLE_CIRCLE: {
      double dx = 0.1*rect.getWidth ();
      double dy = 0.1*rect.getHeight();

      auto rect1 = rect.expanded(dx, dy, -dx, -dy);

      device->drawEllipse(rect , data.angle);
      device->drawEllipse(rect1, data.angle);

      break;
    }
    case CQChartsShapeType::Type::STAR: {
      QPainterPath path;

      auto xc = rect.getXMid();
      auto yc = rect.getYMid();

      auto ri = 0.25*rect.getWidth ();
      auto ro = 0.50*rect.getWidth ();

      auto n  = std::max(data.numSides, 5);
      auto a  = M_PI/2.0;
      auto da = 2*M_PI/n;

      auto ia = rect.iaspect(); // h/w

      for (int i = 0; i < n; ++i) {
        auto x1 = xc +    ro*std::cos(a);
        auto y1 = yc + ia*ro*std::sin(a);

        if (i == 0)
          path.moveTo(x1, y1);
        else
          path.lineTo(x1, y1);

        a += da/2.0;

        auto x2 = xc +    ri*std::cos(a);
        auto y2 = yc + ia*ri*std::sin(a);

        path.lineTo(x2, y2);

        a += da/2.0;
      }

      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      // TODO:
      break;
    }
    case CQChartsShapeType::Type::UNDERLINE: {
      QPainterPath path;

      path.moveTo(rect.getXMin(), rect.getYMin());
      path.lineTo(rect.getXMax(), rect.getYMin());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::CYLINDER: {
      double dy = 0.1*rect.getHeight();

      QPainterPath path;

      path.moveTo (rect.getXMin(), rect.getYMin() + dy);
      path.cubicTo(rect.getXMin(), rect.getYMin(),
                   rect.getXMax(), rect.getYMin(),
                   rect.getXMax(), rect.getYMin() + dy);
      path.lineTo (rect.getXMax(), rect.getYMax() - dy);
      path.cubicTo(rect.getXMax(), rect.getYMax(),
                   rect.getXMin(), rect.getYMax(),
                   rect.getXMin(), rect.getYMax() - dy);
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::NOTE: {
      double dx = 0.1*rect.getWidth();
      double dy = 0.1*rect.getHeight();

      QPainterPath path;

      path.moveTo (rect.getXMin()     , rect.getYMin());
      path.lineTo (rect.getXMax()     , rect.getYMin());
      path.lineTo (rect.getXMax()     , rect.getYMax() - dy);
      path.lineTo (rect.getXMax() - dx, rect.getYMax());
      path.lineTo (rect.getXMin()     , rect.getYMax());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      QPainterPath path1;

      path1.moveTo(rect.getXMax()     , rect.getYMax() - dy);
      path1.lineTo(rect.getXMax() - dx, rect.getYMax() - dy);
      path1.lineTo(rect.getXMax() - dx, rect.getYMax()     );

      device->strokePath(rotatePath(path1, data.angle.degrees()), device->pen());

      break;
    }
    case CQChartsShapeType::Type::TAB: {
      double dx = 0.2*rect.getWidth();
      double dy = 0.1*rect.getHeight();

      QPainterPath path;

      path.moveTo (rect.getXMin()     , rect.getYMin());
      path.lineTo (rect.getXMax()     , rect.getYMin());
      path.lineTo (rect.getXMax()     , rect.getYMax() - dy);
      path.lineTo (rect.getXMin() + dx, rect.getYMax() - dy);
      path.lineTo (rect.getXMin() + dx, rect.getYMax());
      path.lineTo (rect.getXMin()     , rect.getYMax());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      QPainterPath path1;

      path1.moveTo(rect.getXMin()     , rect.getYMax() - dy);
      path1.lineTo(rect.getXMin() + dx, rect.getYMax() - dy);

      device->strokePath(rotatePath(path1, data.angle.degrees()), device->pen());

      break;
    }
    case CQChartsShapeType::Type::FOLDER: {
      double dx1 = 0.2*rect.getWidth();
      double dx2 = 0.1*rect.getWidth();
      double dy  = 0.1*rect.getHeight();

      QPainterPath path;

      path.moveTo (rect.getXMin()              , rect.getYMin());
      path.lineTo (rect.getXMax()              , rect.getYMin());
      path.lineTo (rect.getXMax()              , rect.getYMax() - dy);
      path.lineTo (rect.getXMax() - dx2        , rect.getYMax());
      path.lineTo (rect.getXMax() - dx1 - dx2  , rect.getYMax());
      path.lineTo (rect.getXMax() - dx1 - 2*dx2, rect.getYMax() - dy);
      path.lineTo (rect.getXMin()              , rect.getYMax() - dy);
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::BOX3D: {
      double dx = 0.1*rect.getWidth();
      double dy = 0.1*rect.getHeight();

      QPainterPath path;

      path.moveTo (rect.getXMin()     , rect.getYMin());
      path.lineTo (rect.getXMax() - dx, rect.getYMin());
      path.lineTo (rect.getXMax()     , rect.getYMin() + dy);
      path.lineTo (rect.getXMax()     , rect.getYMax());
      path.lineTo (rect.getXMin() + dx, rect.getYMax());
      path.lineTo (rect.getXMin()     , rect.getYMax() - dy);
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      QPainterPath path1;

      path1.moveTo(rect.getXMin()     , rect.getYMax() - dy);
      path1.lineTo(rect.getXMax() - dx, rect.getYMax() - dy);
      path1.lineTo(rect.getXMax() - dx, rect.getYMin()     );

      device->strokePath(rotatePath(path1, data.angle.degrees()), device->pen());

      break;
    }
    case CQChartsShapeType::Type::COMPONENT: {
      double dx = 0.1*rect.getWidth();
      double dy = 0.1*rect.getHeight();

      QPainterPath path;

      path.moveTo (rect.getXMin() + dx/2.0, rect.getYMin());
      path.lineTo (rect.getXMax()         , rect.getYMin());
      path.lineTo (rect.getXMax()         , rect.getYMax());
      path.lineTo (rect.getXMin() + dx/2.0, rect.getYMax());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      for (int i = 0; i < 2; ++i) {
        QPainterPath path1;

        auto dy1 = (i == 0 ? rect.getYMin() + dy : rect.getYMax() - 2*dy);

        path1.moveTo (rect.getXMin()     , dy1     );
        path1.lineTo (rect.getXMin() + dx, dy1     );
        path1.lineTo (rect.getXMin() + dx, dy1 + dy);
        path1.lineTo (rect.getXMin()     , dy1 + dy);
        path1.closeSubpath();

        device->drawPath(rotatePath(path1, data.angle.degrees()));
      }

      break;
    }
    case CQChartsShapeType::Type::CDS: {
      QPainterPath path;

      auto xr = CMathUtil::lerp(0.75, rect.getXMin(), rect.getXMax());

      path.moveTo(rect.getXMin(), rect.getYMin());
      path.lineTo(rect.getXMin(), rect.getYMax());
      path.lineTo(xr            , rect.getYMax());
      path.lineTo(rect.getXMax(), rect.getYMid());
      path.lineTo(xr            , rect.getYMin());
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::ARROW: {
      QPainterPath path;

      double dx = 0.25*rect.getWidth ();
      double dy = 0.20*rect.getHeight();

      path.moveTo(rect.getXMin()     , rect.getYMin() + dy);
      path.lineTo(rect.getXMin()     , rect.getYMax() - dy);
      path.lineTo(rect.getXMax() - dx, rect.getYMax() - dy);
      path.lineTo(rect.getXMax() - dx, rect.getYMax()     );
      path.lineTo(rect.getXMax()     , rect.getYMid()     );
      path.lineTo(rect.getXMax() - dx, rect.getYMin()     );
      path.lineTo(rect.getXMax() - dx, rect.getYMin() + dy);
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::LPROMOTER: {
      QPainterPath path;

      double dx = 0.25*rect.getWidth ();
      double dy = 0.20*rect.getHeight();

      path.moveTo(rect.getXMax()     , rect.getYMin()     );
      path.lineTo(rect.getXMax()     , rect.getYMax() - dy);
      path.lineTo(rect.getXMin() + dx, rect.getYMax() - dy);
      path.lineTo(rect.getXMin() + dx, rect.getYMax()     );
      path.lineTo(rect.getXMin()     , rect.getYMid()     );
      path.lineTo(rect.getXMin() + dx, rect.getYMin()     );
      path.lineTo(rect.getXMin() + dx, rect.getYMin() + dy);
      path.lineTo(rect.getXMax() - dx, rect.getYMin() + dy);
      path.lineTo(rect.getXMax() - dx, rect.getYMin()     );
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
    case CQChartsShapeType::Type::RPROMOTER: {
      QPainterPath path;

      double dx = 0.25*rect.getWidth ();
      double dy = 0.20*rect.getHeight();

      path.moveTo(rect.getXMin()     , rect.getYMin()     );
      path.lineTo(rect.getXMin()     , rect.getYMax() - dy);
      path.lineTo(rect.getXMax() - dx, rect.getYMax() - dy);
      path.lineTo(rect.getXMax() - dx, rect.getYMax()     );
      path.lineTo(rect.getXMax()     , rect.getYMid()     );
      path.lineTo(rect.getXMax() - dx, rect.getYMin()     );
      path.lineTo(rect.getXMax() - dx, rect.getYMin() + dy);
      path.lineTo(rect.getXMin() + dx, rect.getYMin() + dy);
      path.lineTo(rect.getXMin() + dx, rect.getYMin()     );
      path.closeSubpath();

      device->drawPath(rotatePath(path, data.angle.degrees()));

      break;
    }
//  default: {
//    device->drawRect(rect);
//  }
  }
}

void
drawShapeSwatch(PaintDevice *device, const CQChartsShapeTypeData &data,
                const BBox &rect, double size)
{
  switch (data.shapeType) {
    default:
    case CQChartsShapeType::Type::NONE: {
      break;
    }
    case CQChartsShapeType::Type::BOX: {
      auto rect1 = BBox(rect.getXMin(), rect.getYMin(),
                        rect.getXMax(), rect.getYMin() + size*rect.getHeight());

      if (data.cornerSize.isSet()) {
        CQChartsDrawUtil::drawRoundedRect(device, rect1, data.cornerSize, data.sides, data.angle);
      }
      else {
        if (! data.angle.isZero())
          drawPolygonSides(device, rect1, 4, data.angle + Angle::degrees(45));
        else
          device->drawRect(rect1);
      }

      break;
    }
  }
}

//---

void
drawDotLine(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
            const Length &lineWidth, bool horizontal, const Symbol &symbol,
            const Length &symbolSize, const PenBrush &symbolPenBrush, const Angle &angle)
{
  // draw solid line
  double lw = (! horizontal ? device->lengthPixelWidth (lineWidth) :
                              device->lengthPixelHeight(lineWidth));

  auto c = bbox.getCenter();

  if (! horizontal) {
    if (lw < 3.0 && angle.isZero()) {
      setPenBrush(device, penBrush);

      device->drawLine(Point(c.x, bbox.getYMin()), Point(c.x, bbox.getYMax()));
    }
    else {
      auto pbbox = device->windowToPixel(bbox);

      double pxc = pbbox.getXMid();

      BBox pbbox1(pxc - lw/2.0, pbbox.getYMin(), pxc + lw/2.0, pbbox.getYMax());

      drawRoundedRect(device, penBrush, device->pixelToWindow(pbbox1),
                      Length(), Sides(Sides::Side::ALL), angle);
    }
  }
  else {
    if (lw < 3.0 && angle.isZero()) {
      setPenBrush(device, penBrush);

      device->drawLine(Point(bbox.getXMin(), c.y), Point(bbox.getXMax(), c.y));
    }
    else {
      auto pbbox = device->windowToPixel(bbox);

      double pyc = pbbox.getYMid();

      BBox pbbox1(pbbox.getXMid(), pyc - lw/2.0, pbbox.getXMax(), pyc + lw/2.0);

      drawRoundedRect(device, penBrush, device->pixelToWindow(pbbox1),
                      Length(), Sides(Sides::Side::ALL), angle);
    }
  }

  //---

  // draw dot
  Point p;

  if (! horizontal)
    p = Point(bbox.getXMid(), bbox.getYMax());
  else
    p = Point(bbox.getXMax(), bbox.getYMid());

  if (! angle.isZero())
    p = p.rotate(c, angle.radians());

  if (symbol.isValid())
    drawSymbol(device, symbolPenBrush, symbol, p, symbolSize, /*scale*/true);
}

//---

void
drawRoundedRect(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
                const Length &cornerSize, const Sides &sides, const Angle &angle)
{
  setPenBrush(device, penBrush);

  drawRoundedRect(device, bbox, cornerSize, cornerSize, sides, angle);
}

void
drawRoundedRect(PaintDevice *device, const BBox &bbox,
                const Length &cornerSize, const Sides &sides, const Angle &angle)
{
  drawRoundedRect(device, bbox, cornerSize, cornerSize, sides, angle);
}

void
drawRoundedRect(PaintDevice *device, const BBox &bbox, const Length &xCornerSize,
                const Length &yCornerSize, const Sides &sides, const Angle &angle)
{
  static double minSize1 = 2.5; // pixels
  static double minSize2 = 1.5; // pixels

  auto pbbox = device->windowToPixel(bbox);

  double minSize = pbbox.getMinSize();

  double xsize = device->lengthWindowWidth (xCornerSize);
  double ysize = device->lengthWindowHeight(yCornerSize);

  // if wide enough (max dimension) just draw normal rounded rect
  if      (minSize >= minSize1) {
    drawAdjustedRoundedRect(device, bbox, xsize, ysize, sides, angle);
  }
  // if thin (but not two thin) then adjust stroke alpha to not hide fill
  else if (minSize >= minSize2) {
    auto pen = device->pen();

    auto pc = pen.color();
    auto f  = (minSize - minSize2)/(minSize1 - minSize2);

    pc.setAlphaF(f*pc.alphaF());

    pen.setColor(pc);

    device->setPen(pen);

    drawAdjustedRoundedRect(device, bbox, xsize, ysize, sides, angle);
  }
  // if too thin just draw line
  else {
    auto bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (pbbox.getWidth() > pbbox.getHeight())
      device->drawLine(Point(bbox.getXMin(), bbox.getYMid()),
                       Point(bbox.getXMax(), bbox.getYMid()));
    else
      device->drawLine(Point(bbox.getXMid(), bbox.getYMin()),
                       Point(bbox.getXMid(), bbox.getYMax()));
  }
}

// draw rounded rect (already checked for size limits)
void
drawAdjustedRoundedRect(PaintDevice *device, const BBox &bbox, double xsize, double ysize,
                        const CQChartsSides &sides, const Angle &angle)
{
  // draw rounded rect if corners
  if (xsize > 0 || ysize > 0) {
    // TODO: support sides with corners
    QPainterPath path;

    path.addRoundedRect(bbox.qrect(), xsize, ysize);

    if (! angle.isZero())
      device->drawPath(rotatePath(path, angle.degrees()));
    else
      device->drawPath(path);
  }
  // draw rect if no corners
  else {
    QPainterPath path;

    if (sides.isAll()) {
      if (! angle.isZero()) {
        QPainterPath path;

        auto x1 = bbox.getXMin(), y1 = bbox.getYMin();
        auto x2 = bbox.getXMax(), y2 = bbox.getYMax();

        path.moveTo(x1, y1);
        path.lineTo(x2, y1);
        path.lineTo(x2, y2);
        path.lineTo(x1, y2);

        path.closeSubpath();

        device->drawPath(rotatePath(path, angle.degrees()));
      }
      else
        device->drawRect(bbox);
    }
    else {
      device->fillRect(bbox);

      // draw sides
      if (sides.isLeft  ()) device->drawLine(bbox.getLL(), bbox.getUL());
      if (sides.isRight ()) device->drawLine(bbox.getLR(), bbox.getUR());
      if (sides.isTop   ()) device->drawLine(bbox.getUL(), bbox.getUR());
      if (sides.isBottom()) device->drawLine(bbox.getLL(), bbox.getLR());
    }
  }
}

void
drawRoundedPolygon(PaintDevice *device, const PenBrush &penBrush, const Polygon &poly,
                   const Length &cornerSize, const Angle &angle)
{
  setPenBrush(device, penBrush);

  drawRoundedPolygon(device, poly, cornerSize, cornerSize, angle);
}

void
drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &cornerSize,
                   const Angle &angle)
{
  drawRoundedPolygon(device, poly, cornerSize, cornerSize, angle);
}

void
drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &xCornerSize,
                   const Length &yCornerSize, const Angle &)
{
  static double minSize = 2.5; // pixels

  auto bbox = poly.boundingBox();

  double pw = device->lengthPixelWidth (Length(bbox.getWidth (), device->parentUnits()));
  double ph = device->lengthPixelHeight(Length(bbox.getHeight(), device->parentUnits()));

  // if wide enough (max dimension) just draw normal rounded polygon
  if (pw > minSize && ph > minSize) {
    double xsize = device->lengthWindowWidth (xCornerSize);
    double ysize = device->lengthWindowHeight(yCornerSize);

    QPainterPath path;

    if (! roundedPolygonPath(path, poly, xsize, ysize)) {
      device->drawPolygon(poly);
      return;
    }

    device->drawPath(path);
  }
  // if too thin just draw line
  else {
    auto bc = device->brush().color();

    device->setPen(bc);
    device->setBrush(Qt::NoBrush);

    if (bbox.getWidth() > minSize) // horizontal line
      device->drawLine(Point(bbox.getXMin(), bbox.getYMid()),
                       Point(bbox.getXMax(), bbox.getYMin()));
    else                           // vertical line
      device->drawLine(Point(bbox.getXMid(), bbox.getYMin()),
                       Point(bbox.getXMid(), bbox.getYMax()));
  }
}

//------

void
drawTextInCircle(PaintDevice *device, const BBox &rect,
                 const QString &text, const TextOptions &options)
{
  drawTextInBox(device, rect, text, options, 1/sqrt(2.0));
}

void
drawTextInBox(PaintDevice *device, const BBox &rect,
              const QString &text, const TextOptions &options, double adjustScale)
{
  assert(rect.isValid());

  if (! text.length())
    return;

  //---

  // handle html separately
  if (options.html) {
    bool rotateRect = false;

    if (options.scaled)
      CQChartsDrawPrivate::drawScaledHtmlText(device, rect, text, options, adjustScale, rotateRect);
    else
      CQChartsDrawPrivate::drawHtmlText(device, rect, text, options, rotateRect);

    return;
  }

  //---

  auto pen = device->pen();

  if (options.clipped)
    device->save();

  if (options.angle.isZero()) {
    auto text1 = clipTextToLength(text, device->font(), options.clipLength, options.clipElide);

    if (options.clipped)
      device->setClipRect(rect, Qt::IntersectClip);

    device->setPen(pen);

    //---

    QStringList strs;

    if (options.formatted) {
      auto prect = device->windowToPixel(rect);

      if (options.scaled) {
        auto w = prect.getWidth ();
        auto h = prect.getHeight();

        prect = BBox(prect.getXMid() - w/2.0, prect.getYMid() - h/2.0,
                     prect.getXMid() + w/2.0, prect.getYMid() + h/2.0);
      }

      CQChartsUtil::formatStringInRect(text1, device->font(), prect, strs,
                                       CQChartsUtil::FormatData(options.formatSeps));
    }
    else
      strs << text1;

    //---

    drawTextsInBox(device, rect, strs, options, adjustScale);
  }
  else {
    if (options.clipped)
      device->setClipRect(rect, Qt::IntersectClip);

    drawRotatedTextInBox(device, rect, text, pen, options);
  }

  if (options.clipped)
    device->restore();
}

void
drawTextsInCircle(PaintDevice *device, const BBox &rect, const QStringList &strs,
                  const TextOptions &options)
{
  drawTextsInBox(device, rect, strs, options, 1/sqrt(2.0));
}

void
drawTextsInBox(PaintDevice *device, const BBox &rect, const QStringList &strs,
               const TextOptions &options, double adjustScale)
{
  auto prect = device->windowToPixel(rect);

  QFontMetricsF fm(device->font());

  double th = strs.size()*fm.height() + 2*options.margin;

  if (options.scaled) {
    // calc text scale
    double s = options.scale;

    if (s <= 0.0) {
      double tw = 0;

      for (int i = 0; i < strs.size(); ++i)
        tw = std::max(tw, fm.horizontalAdvance(strs[i]));

      tw += 2*options.margin;

      double sx = (tw > 0 ? prect.getWidth ()/tw : 1);
      double sy = (th > 0 ? prect.getHeight()/th : 1);

      s = std::min(sx, sy);
    }

    s *= adjustScale;

    //---

    // scale font
    updateScaledFontSize(s);

    bool zoomFont = device->isZoomFont();
    device->setZoomFont(false);

    options.calcFontScale = s;

    device->setFont(CQChartsUtil::scaleFontSize(
      device->font(), s, options.minScaleFontSize, options.maxScaleFontSize));

    device->setZoomFont(zoomFont);

    fm = QFontMetricsF(device->font());

    th = strs.size()*fm.height();
  }

  //---

  double dy = 0.0;

  if      (options.align & Qt::AlignVCenter)
    dy = (prect.getHeight() - th)/2.0;
  else if (options.align & Qt::AlignBottom)
    dy = prect.getHeight() - th;

  double y = prect.getYMin() + dy + fm.ascent();

  for (int i = 0; i < strs.size(); ++i) {
    double dx = 0.0;

    double tw = fm.horizontalAdvance(strs[i]);

    if      (options.align & Qt::AlignHCenter)
      dx = (prect.getWidth() - tw)/2;
    else if (options.align & Qt::AlignRight)
      dx = prect.getWidth() - tw;

    double x = prect.getXMin() + dx;

    auto pt = device->pixelToWindow(Point(x, y));

    if (! options.skipDraw) {
      if (options.contrast)
        drawContrastText(device, pt, strs[i], options.contrastAlpha);
      else
        drawSimpleText(device, pt, strs[i]);
    }

    y += fm.height();
  }
}

void
drawRotatedTextInBox(PaintDevice *device, const BBox &rect,
                     const QString &text, const QPen &pen, const TextOptions &options)
{
  assert(rect.isValid());

  device->setPen(pen);

  CQChartsRotatedText::drawInBox(device, rect, text, options, /*alignBox*/false);
}

//------

CQChartsGeom::BBox
calcTextsAtPointRect(PaintDevice *device, const Point &point, const QStringList &texts,
                     const TextOptions &options, bool centered, double pdx, double pdy)
{
  std::vector<BBox> bboxes;

  double h = 0.0;

  for (const auto &text : texts) {
    auto bbox = calcTextAtPointRect(device, point, text, options, centered, pdx, pdy);

    bboxes.push_back(bbox);

    h += bbox.getHeight();
  }

  BBox bbox1;

  auto y = point.y + h/2.0;

  for (auto &bbox : bboxes) {
    bbox.moveBy(Point(0.0, y - bbox.getYMax()));

    y -= bbox.getHeight();

    bbox1 += bbox;
  }

  return bbox1;
}

CQChartsGeom::BBox
calcTextAtPointRect(PaintDevice *device, const Point &point, const QString &text,
                    const TextOptions &options, bool centered, double pdx, double pdy)
{
  // handle html separately
  if (options.html) {
    Size psize = CQChartsDrawPrivate::calcHtmlTextSize(text, device->font(), options.margin);

    auto sw = device->pixelToWindowWidth (psize.width () + 4);
    auto sh = device->pixelToWindowHeight(psize.height() + 4);

    auto c = point;

#if 1
    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) c.x -= sw/2.0;
      else if (options.align & Qt::AlignRight  ) c.x -= sw;
    }
    else {
      if      (options.align & Qt::AlignLeft ) c.x += sw/2.0;
      else if (options.align & Qt::AlignRight) c.x -= sw/2.0;
    }

    if      (options.align & Qt::AlignVCenter) c.y -= sh/2.0;
    else if (options.align & Qt::AlignTop    ) c.y -= sh;
#else
    auto options1 = options;

    options1.html = false;

    auto bbox1 = calcTextAtPointRect(device, point, text, options1, centered, pdx, pdy);

    c.x = bbox1.getXMin();
    c.y = bbox1.getYMin();
#endif

    auto rect = BBox(c.x, c.y, c.x + sw, c.y + sh);

    return rect;
  }

  //---

  auto text1 = clipTextToLength(text, device->font(), options.clipLength, options.clipElide);

  QFontMetricsF fm(device->font());

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = fm.horizontalAdvance(text1);

  //---

  if (options.angle.isZero()) {
    // calc dx : point is left or hcenter of text (
    // drawContrastText and drawSimpleText wants left aligned
    double dx1 = 0.0, dy1 = 0.0; // pixel

    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) dx1 = -tw/2.0;
      else if (options.align & Qt::AlignRight  ) dx1 = -tw - pdx;
    }
    else {            // point is center
      if      (options.align & Qt::AlignLeft ) dx1 =  tw/2.0 + pdx;
      else if (options.align & Qt::AlignRight) dx1 = -tw/2.0 - pdx;
    }

    if      (options.align & Qt::AlignTop    ) dy1 =  ta + pdy;
    else if (options.align & Qt::AlignBottom ) dy1 = -td - pdy;
  //else if (options.align & Qt::AlignVCenter) dy1 = -ta/2.0 + td;
    else if (options.align & Qt::AlignVCenter) dy1 = (ta - td)/2.0;

    auto tp = point;

    if (dx1 != 0.0 || dy1 != 0.0) {
      // apply delta (pixels)
      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));
    }

    auto pp = device->windowToPixel(tp);

    BBox prect(pp.x, pp.y - ta, pp.x + tw, pp.y + td);

    return device->pixelToWindow(prect);
  }
  else {
    // calc dx : point is left or hcenter of text
    // CQChartsRotatedText::draw wants center aligned
    auto tp = point;

    if (! centered) {
      double dx1 = -tw/2.0;

      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y));
    }

    auto pp = device->windowToPixel(tp);

    //BBox prect(pp.x, pp.y - ta, pp.x + tw, point.y + td);

    auto prect = CQChartsRotatedText::calcBBox(pp.x, pp.y, text1, device->font(),
                                               options, 0, /*alignBox*/true);

    return device->pixelToWindow(prect);
  }
}

void
drawTextsAtPoint(PaintDevice *device, const Point &point, const QStringList &texts,
                const TextOptions &options)
{
  if      (texts.size() == 1) {
    drawTextAtPoint(device, point, texts[0], options);
  }
  else if (texts.size() == 2) {
    QFontMetricsF fm(device->font());

    double th = fm.height();

    auto ppoint = device->windowToPixel(point);

    auto point1 = device->pixelToWindow(Point(ppoint.x, ppoint.y - th/2));
    auto point2 = device->pixelToWindow(Point(ppoint.x, ppoint.y + th/2));

    drawTextAtPoint(device, point1, texts[0], options);
    drawTextAtPoint(device, point2, texts[1], options);
  }
  else {
    assert(false);
  }
}

void
drawTextAtPoint(PaintDevice *device, const Point &point, const QString &text,
                const TextOptions &options, bool centered, double pdx, double pdy)
{
  // handle html separately
  if (options.html) {
    auto psize = CQChartsDrawPrivate::calcHtmlTextSize(text, device->font(), options.margin);

    auto sw = device->pixelToWindowWidth (psize.width () + 4);
    auto sh = device->pixelToWindowHeight(psize.height() + 4);

    auto dx = device->pixelToSignedWindowWidth (pdx);
    auto dy = device->pixelToSignedWindowHeight(pdy);

    auto c = Point(point.x + dx, point.y + dy);

#if 1
    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) c.x -= sw/2.0;
      else if (options.align & Qt::AlignRight  ) c.x -= sw;
    }
    else {
      if      (options.align & Qt::AlignLeft ) c.x += sw/2.0;
      else if (options.align & Qt::AlignRight) c.x -= sw/2.0;
    }

    if      (options.align & Qt::AlignVCenter) c.y -= sh/2.0;
    else if (options.align & Qt::AlignTop    ) c.y -= sh;
#else
    auto options1 = options;

    options1.html = false;

    auto bbox1 = calcTextAtPointRect(device, point, text, options1, centered, pdx, pdy);

    c.x = bbox1.getXMin();
    c.y = bbox1.getYMin();
#endif

    auto rect = BBox(c.x, c.y, c.x + sw, c.y + sh);

    if (options.scaled)
      CQChartsDrawPrivate::drawScaledHtmlText(device, rect, text, options);
    else
      CQChartsDrawPrivate::drawHtmlText(device, c, rect, text, options, pdx, pdy);

    return;
  }

  //---

  auto text1 = clipTextToLength(text, device->font(), options.clipLength, options.clipElide);

  QFontMetricsF fm(device->font());

  double ta = fm.ascent();
  double td = fm.descent();

  double tw = fm.horizontalAdvance(text1);

  //---

  if (options.angle.isZero()) {
    // calc dx : point is left or hcenter of text (
    // drawContrastText and drawSimpleText wants left aligned
    double dx1 = 0.0, dy1 = 0.0; // pixel

    if (! centered) { // point is left
      if      (options.align & Qt::AlignHCenter) dx1 = -tw/2.0;
      else if (options.align & Qt::AlignRight  ) dx1 = -tw - pdx;
    }
    else {            // point is center
      if      (options.align & Qt::AlignLeft ) dx1 =  tw/2.0 + pdx;
      else if (options.align & Qt::AlignRight) dx1 = -tw/2.0 - pdx;
    }

    if      (options.align & Qt::AlignTop    ) dy1 =  ta + pdy;
    else if (options.align & Qt::AlignBottom ) dy1 = -td - pdy;
  //else if (options.align & Qt::AlignVCenter) dy1 = -ta/2.0 + td;
    else if (options.align & Qt::AlignVCenter) dy1 = (ta - td)/2.0;

    auto tp = point;

    if (dx1 != 0.0 || dy1 != 0.0) {
      // apply delta (pixels)
      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));
    }

    if (options.contrast)
      drawContrastText(device, tp, text1, options.contrastAlpha);
    else
      drawSimpleText(device, tp, text1);
  }
  else {
    // calc dx : point is left or hcenter of text
    // CQChartsRotatedText::draw wants center aligned
    auto tp = point;

    if (! centered) {
      double dx1 = -tw/2.0;

      auto pp = device->windowToPixel(tp);

      tp = device->pixelToWindow(Point(pp.x + dx1, pp.y));
    }

    CQChartsRotatedText::draw(device, tp, text1, options, /*alignBox*/true);
  }
}

//------

void
drawAlignedText(PaintDevice *device, const Point &p, const QString &text,
                Qt::Alignment align, double pdx, double pdy)
{
  QFontMetricsF fm(device->font());

  double tw = fm.horizontalAdvance(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  double dx1 = 0.0, dy1 = 0.0;

  if      (align & Qt::AlignLeft   ) dx1 = pdx;
  else if (align & Qt::AlignRight  ) dx1 = -tw - pdx;
  else if (align & Qt::AlignHCenter) dx1 = -tw/2;

  if      (align & Qt::AlignTop    ) dy1 =  ta + pdy;
  else if (align & Qt::AlignBottom ) dy1 = -td - pdy;
  else if (align & Qt::AlignVCenter) dy1 = -ta/2.0 + td;

  auto pp = device->windowToPixel(p);
  auto pt = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));

  drawSimpleText(device, pt, text);
}

//------

CQChartsGeom::BBox
calcAlignedTextRect(PaintDevice *device, const QFont &font, const Point &p,
                    const QString &text, Qt::Alignment align, double pdx, double pdy)
{
  QFontMetricsF fm(font);

  double tw = fm.horizontalAdvance(text);
  double ta = fm.ascent ();
  double td = fm.descent();

  double dx1 = 0.0, dy1 = 0.0;

  if      (align & Qt::AlignLeft   ) dx1 = pdx;
  else if (align & Qt::AlignRight  ) dx1 = -tw - pdx;
  else if (align & Qt::AlignHCenter) dx1 = -tw/2;

  if      (align & Qt::AlignTop    ) dy1 =  ta + pdy;
  else if (align & Qt::AlignBottom ) dy1 = -td - pdy;
  else if (align & Qt::AlignVCenter) dy1 = -ta/2.0 + td;

  auto pp = device->windowToPixel(p);
  auto pt = device->pixelToWindow(Point(pp.x + dx1, pp.y + dy1));

  BBox pbbox(pt.x, pt.y - ta, pt.x + tw, pt.y + td);

  return device->pixelToWindow(pbbox);
}

//------

void
drawContrastText(PaintDevice *device, const Point &p, const QString &text, const Alpha &alpha)
{
  auto pen = device->pen();

  //---

  // set contrast outline color
  // TODO: allow set type (invert, bw) and alpha
//auto outlineColor = CQChartsUtil::invColor(pen.color());
  auto outlineColor = CQChartsUtil::bwColor(pen.color());

  auto cc = device->contrastColor();

  if (cc.isValid()) {
    int d = std::abs(CQChartsUtil::grayValue(cc) - CQChartsUtil::grayValue(outlineColor));

    if (d < 64)
      outlineColor = QColor(128, 128, 128);
  }

  setColorAlpha(outlineColor, alpha);

  //---

  auto pp = device->windowToPixel(p);

  // draw contrast outline
  device->setPen(outlineColor);

  for (int dy = -2; dy <= 2; ++dy) {
    for (int dx = -2; dx <= 2; ++dx) {
      if (dx != 0 || dy != 0) {
        auto p1 = device->pixelToWindow(Point(pp.x + dx, pp.y + dy));

        drawSimpleText(device, p1, text);
      }
    }
  }

  //---

  // draw text
  device->setPen(pen);

  drawSimpleText(device, p, text);
}

//------

CQChartsGeom::Size
calcTextSize(const QString &text, const QFont &font, const TextOptions &options)
{
  if (options.html)
    return CQChartsDrawPrivate::calcHtmlTextSize(text, font, options.margin);

  //---

  QFontMetricsF fm(font);

  return Size(fm.horizontalAdvance(text), fm.height());
}

//------

void
drawCenteredText(PaintDevice *device, const Point &pos, const QString &text)
{
  QFontMetricsF fm(device->font());

  auto ppos = device->windowToPixel(pos);

  Point ppos1(ppos.x - fm.horizontalAdvance(text)/2, ppos.y + (fm.ascent() - fm.descent())/2);

  drawSimpleText(device, device->pixelToWindow(ppos1), text);
}

//------

void
drawSimpleText(PaintDevice *device, const Point &pos, const QString &text)
{
  device->drawText(pos, text);
}

//---

void
drawSelectedOutline(PaintDevice *device, const BBox &rect, double margin, double width)
{
  auto rw = rect.getWidth ();
  auto rh = rect.getHeight();

  auto mx = margin*rw;
  auto my = margin*rh;

  auto wx = width*rw;
  auto wy = width*rh;

  auto dx1 = mx;
  auto dy1 = my;
  auto dx2 = mx + wx;
  auto dy2 = my + wy;

  auto rect1 = rect.expanded(-dx1, -dy1, dx1, dy1);
  auto rect2 = rect.expanded(-dx2, -dy2, dx2, dy2);

  QPainterPath path;

  outlinePath(path, rect1, rect2);

  device->save();

  device->drawPath(path);

  device->restore();
}

//---

void
drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
           const Point &c, const Length &size, bool scale)
{
  drawSymbol(device, penBrush, symbol, c, size, size, scale);
}

void
drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
           const Point &c, const Length &xsize, const Length &ysize, bool scale)
{
  setPenBrush(device, penBrush);

  drawSymbol(device, symbol, c, xsize, ysize, scale);
}

void
drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c,
           const Length &size, bool scale)
{
  drawSymbol(device, symbol, c, size, size, scale);
}

void
drawSymbol(PaintDevice *device, const Symbol &symbol, const BBox &bbox, bool scale)
{
  assert(bbox.isValid());

  auto pbbox = device->windowToPixel(bbox);

  double cx = bbox.getXMid();
  double cy = bbox.getYMid();
  double sx = pbbox.getWidth();
  double sy = pbbox.getHeight();

  auto xsize = Length::pixel(sx/2.0);
  auto ysize = Length::pixel(sy/2.0);

  drawSymbol(device, symbol, Point(cx, cy), xsize, ysize, scale);
}

void
drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c,
           const Length &xsize, const Length &ysize, bool scale)
{
  if (! symbol.isValid() || ! xsize.isValid() || ! ysize.isValid())
    return;

  CQChartsPlotSymbolRenderer srenderer(device, c, xsize, ysize);

  srenderer.setScale(scale);

  if      (symbol.isFilled() && ! symbol.isStroked())
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &srenderer); // filled
  else if (symbol.isStroked() && ! symbol.isFilled())
    CQChartsPlotSymbolMgr::drawSymbol(symbol, &srenderer); // wireframe
  else {
    CQChartsPlotSymbolMgr::fillSymbol  (symbol, &srenderer); // filled
    CQChartsPlotSymbolMgr::strokeSymbol(symbol, &srenderer); // stroked
  }
}

//---

void
drawPolygonSides(PaintDevice *device, const BBox &bbox, int n, const Angle &angle)
{
  QPainterPath path;

  if (! polygonSidesPath(path, bbox, n, angle))
    return;

  device->drawPath(path);
}

bool
polygonSidesPath(QPainterPath &path, const BBox &bbox, int n, const Angle &angle)
{
  path = QPainterPath();

  if (n < 3) return false;

  double xc = bbox.getXMid();
  double yc = bbox.getYMid();

  double xr = bbox.getWidth ()/2.0;
  double yr = bbox.getHeight()/2.0;

  double a  = M_PI/2.0 - angle.radians();
  double da = 2.0*M_PI/n;

  for (int i = 0; i < n; ++i) {
    double c = std::cos(a);
    double s = std::sin(a);

    double x = xc + c*xr;
    double y = yc + s*yr;

    if (i == 0)
      path.moveTo(QPointF(x, y));
    else
      path.lineTo(QPointF(x, y));

    a += da;
  }

  path.closeSubpath();

  return true;
}

void
drawDiamond(PaintDevice *device, const BBox &bbox)
{
  QPainterPath path;

  diamondPath(path, bbox);

  device->drawPath(path);
}

void
diamondPath(QPainterPath &path, const BBox &bbox)
{
  path = QPainterPath();

  double x1 = bbox.getXMin(), y1 = bbox.getYMin();
  double x2 = bbox.getXMid(), y2 = bbox.getYMid();
  double x3 = bbox.getXMax(), y3 = bbox.getYMax();

  path.moveTo(QPointF(x1, y2));
  path.lineTo(QPointF(x2, y1));
  path.lineTo(QPointF(x3, y2));
  path.lineTo(QPointF(x2, y3));

  path.closeSubpath();
}

void
trianglePath(QPainterPath &path, const Point &p1, const Point &p2, const Point &p3)
{
  path = QPainterPath();

  path.moveTo(p1.x, p1.y);
  path.lineTo(p2.x, p2.y);
  path.lineTo(p3.x, p3.y);

  path.closeSubpath();
}

void
linePath(QPainterPath &path, const Point &p1, const Point &p2)
{
  path = QPainterPath();

  path.moveTo(p1.x, p1.y);
  path.lineTo(p2.x, p2.y);
}

//---

void
editHandlePath(PaintDevice *, QPainterPath &path, const BBox &bbox)
{
  path = QPainterPath();

  //---

  auto ll = bbox.getLL();
  auto ur = bbox.getUR();

  path.moveTo(ll.x, ll.y);
  path.lineTo(ur.x, ll.y);
  path.lineTo(ur.x, ur.y);
  path.lineTo(ll.x, ur.y);

  path.closeSubpath();
}

//---

void
drawRoundedLine(PaintDevice *device, const Point &p1, const Point &p2, double w)
{
  QPainterPath path;

  roundedLinePath(path, p1, p2, w);

  device->drawPath(path);
}

void
fillRoundedLine(PaintDevice *device, const Point &p1, const Point &p2, double w)
{
  QPainterPath path;

  roundedLinePath(path, p1, p2, w);

  device->fillPath(path, QBrush(device->pen().color()));
}

void
roundedLinePath(QPainterPath &path, const Point &p1, const Point &p2, double lw)
{
  QPainterPath lpath;

  lpath.moveTo(p1.qpoint());
  lpath.lineTo(p2.qpoint());

  QPainterPathStroker stroker;

  stroker.setCapStyle   (Qt::RoundCap);
  stroker.setDashOffset (0.0);
  stroker.setDashPattern(Qt::SolidLine);
  stroker.setJoinStyle  (Qt::MiterJoin);
  stroker.setWidth      (lw);

  path = stroker.createStroke(lpath);

  path.setFillRule(Qt::WindingFill);
}

//---

bool
roundedPolygonPath(QPainterPath &path, const Polygon &poly, double xsize, double ysize)
{
  auto interpLine = [&](const QPointF &p1, const QPointF &p2, QPointF &pc1, QPointF &pc2) {
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();

    double l = std::hypot(dx, dy);

    if (l < 1E-6) {
      pc1 = p1;
      pc2 = p2;

      return;
    }

    double mx1 = (xsize < l/2 ? xsize/l : 0.5);
    double mx2 = 1.0 - mx1;

    double my1 = (ysize < l/2 ? ysize/l : 0.5);
    double my2 = 1.0 - my1;

    double x1 = p1.x() + mx1*dx;
    double y1 = p1.y() + my1*dy;

    double x2 = p1.x() + mx2*dx;
    double y2 = p1.y() + my2*dy;

    pc1 = QPointF(x1, y1);
    pc2 = QPointF(x2, y2);
  };

  //---

  path = QPainterPath();

  //---

  if (poly.size() < 3)
    return false;

  int i1 = poly.size() - 1;
  int i2 = 0;
  int i3 = 1;

  while (i2 < poly.size()) {
    const auto &p1 = poly.qpoint(i1);
    const auto &p2 = poly.qpoint(i2);
    const auto &p3 = poly.qpoint(i3);

    if (xsize > 0 || ysize > 0) {
      QPointF p12s, p12e, p23s, p23e;

      interpLine(p1, p2, p12s, p12e);
      interpLine(p2, p3, p23s, p23e);

      if (i2 == 0)
        path.moveTo(p12s);
      else
        path.lineTo(p12s);

      path.lineTo(p12e);
      path.quadTo(p2, p23s);
    }
    else {
      if (i2 == 0)
        path.moveTo(p2);
      else
        path.lineTo(p2);

      path.lineTo(p3);
    }

    i1 = i2;
    i2 = i3++;

    if (i2 == 0)
      break;

    if (i3 >= poly.size())
      i3 = 0;
  }

  path.closeSubpath();

  return true;
}

//---

void
drawPieSlice(PaintDevice *device, const Point &c, double ri, double ro,
             const Angle &angle1, const Angle &angle2, bool isInvertX, bool isInvertY)
{
  QPainterPath path;

  pieSlicePath(path, c, ri, ro, angle1, angle2, isInvertX, isInvertY);

  if (CQChartsAngle::isCircle(angle1, angle2)) {
    device->fillPath(path, device->brush());

    QPainterPath ipath, opath;

    ellipsePath(ipath, BBox(c.x - ri, c.y - ri, c.x + ri, c.y + ri));
    ellipsePath(opath, BBox(c.x - ro, c.y - ro, c.x + ro, c.y + ro));

    device->strokePath(ipath, device->pen());
    device->strokePath(opath, device->pen());
  }
  else {
    device->drawPath(path);
  }
}

void
pieSlicePath(QPainterPath &path, const Point &c, double ri, double ro, const Angle &a1,
             const Angle &a2, bool isInvertX, bool isInvertY)
{
  path = QPainterPath();

  //---

  BBox bbox(c.x - ro, c.y - ro, c.x + ro, c.y + ro);

  //---

  if (! CMathUtil::isZero(ri)) {
    BBox bbox1(c.x - ri, c.y - ri, c.x + ri, c.y + ri);

    //---

    double da = (isInvertX != isInvertY ? -1 : 1);

    double ra1 = da*a1.radians();
    double ra2 = da*a2.radians();

    auto p1 = CQChartsGeom::circlePoint(c, ri, ra1);
    auto p2 = CQChartsGeom::circlePoint(c, ro, ra1);
    auto p3 = CQChartsGeom::circlePoint(c, ri, ra2);
    auto p4 = CQChartsGeom::circlePoint(c, ro, ra2);

    path.moveTo(p1.x, p1.y);
    path.lineTo(p2.x, p2.y);

    path.arcTo(bbox.qrect(), -a1.value(), a1.value() - a2.value());

    path.lineTo(p4.x, p4.y);
    path.lineTo(p3.x, p3.y);

    path.arcTo(bbox1.qrect(), -a2.value(), a2.value() - a1.value());
  }
  else {
    if (CQChartsAngle::isCircle(a1, a2)) {
      path.addEllipse(bbox.qrect());
    }
    else {
      path.moveTo(QPointF(c.x, c.y));

      path.arcTo(bbox.qrect(), -a1.value(), a1.value() - a2.value());
    }
  }

  path.closeSubpath();
}

QPainterPath
pointsToPath(const std::vector<Point> &points)
{
  QPainterPath path;

  for (size_t i = 0; i < points.size(); ++i) {
    if (i == 0) path.moveTo(points[i].qpoint());
    else        path.lineTo(points[i].qpoint());
  }

  return path;
}

//---

void
drawEllipse(PaintDevice *device, const BBox &bbox)
{
  QPainterPath path;

  ellipsePath(path, bbox);

  device->drawPath(path);
}

void
drawDoubleEllipse(PaintDevice *device, const BBox &bbox)
{
  double dx = bbox.getWidth ()/10.0;
  double dy = bbox.getHeight()/10.0;

  auto bbox1 = bbox.expanded(dx, dy, -dx, -dy);

  device->drawEllipse(bbox );
  device->drawEllipse(bbox1);
}

void
ellipsePath(QPainterPath &path, const BBox &bbox)
{
  path = QPainterPath();

  //---

  double xc = bbox.getXMid();
  double yc = bbox.getYMid();

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  double xr = w/2.0;
  double yr = h/2.0;

  double f = 4.0*(std::sqrt(2.0) - 1.0)/3.0;

  double dx = xr*f;
  double dy = yr*f;

  path.moveTo (                                    xc + xr, yc     );
  path.cubicTo(xc + xr, yc + dy, xc + dx, yc + yr, xc     , yc + yr);
  path.cubicTo(xc - dx, yc + yr, xc - xr, yc + dy, xc - xr, yc     );
  path.cubicTo(xc - xr, yc - dy, xc - dx, yc - yr, xc     , yc - yr);
  path.cubicTo(xc + dx, yc - yr, xc + xr, yc - dy, xc + xr, yc     );

  path.closeSubpath();
}

//---

void
drawParetoGradient(PaintDevice *device, const Point &origin, const BBox &bbox,
                   const QColor &c1, const QColor &c2)
{
  auto c = bbox.getCenter();

  auto center = QPointF(origin.x < c.x ? 0.0 : 1.0, origin.y < c.y ? 1.0 : 0.0);
  auto focus  = center;

  double radius = 0.5;

  QRadialGradient rg(center, radius, focus);

  rg.setCoordinateMode(QGradient::ObjectBoundingMode);

  rg.setColorAt(0, c1);
  rg.setColorAt(1, c2);

  auto brush = QBrush(rg);

  device->setBrush(brush);

  device->fillRect(bbox);
}

#if 0
void
drawParetoCorner(PaintDevice *device, const Point &origin, const BBox &bbox,
                 double xf, double yf)
{
  auto invX = (origin.x > bbox.getXMid());
  auto invY = (origin.y > bbox.getYMid());

  auto opposite = Point(invX ? bbox.getXMin() : bbox.getXMax(),
                        invY ? bbox.getYMin() : bbox.getYMax());

  double dx = (opposite.x - origin.x)/10;
  double dy = (opposite.y - origin.y)/10;

  auto origin1 = origin + Point(dx, 0.0);
  auto origin2 = origin + Point(0.0, dy);

  device->drawLine(origin, origin1);
  device->drawLine(origin, origin2);
}
#endif

//---

void
outlinePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox)
{
  path = QPainterPath();

  //---

  double f = 4.0*(std::sqrt(2.0) - 1.0)/3.0;

  double ixc = ibbox.getXMid();
  double iyc = ibbox.getYMid();

  double iw = ibbox.getWidth ();
  double ih = ibbox.getHeight();

  double ixr = iw/2.0;
  double iyr = ih/2.0;

  double idx = ixr*f;
  double idy = iyr*f;

  path.moveTo (                                 ixc + ixr, iyc      );
  path.cubicTo(ixc + ixr, iyc + idy, ixc + idx, iyc + iyr, ixc      , iyc + iyr);
  path.cubicTo(ixc - idx, iyc + iyr, ixc - ixr, iyc + idy, ixc - ixr, iyc      );
  path.cubicTo(ixc - ixr, iyc - idy, ixc - idx, iyc - iyr, ixc      , iyc - iyr);
  path.cubicTo(ixc + idx, iyc - iyr, ixc + ixr, iyc - idy, ixc + ixr, iyc      );

  double oxc = obbox.getXMid();
  double oyc = obbox.getYMid();

  double ow = obbox.getWidth ();
  double oh = obbox.getHeight();

  double oxr = ow/2.0;
  double oyr = oh/2.0;

  double odx = oxr*f;
  double ody = oyr*f;

  path.lineTo (                                 oxc + oxr, oyc      );
  path.cubicTo(oxc + oxr, oyc + ody, oxc + odx, oyc + oyr, oxc      , oyc + oyr);
  path.cubicTo(oxc - odx, oyc + oyr, oxc - oxr, oyc + ody, oxc - oxr, oyc      );
  path.cubicTo(oxc - oxr, oyc - ody, oxc - odx, oyc - oyr, oxc      , oyc - oyr);
  path.cubicTo(oxc + odx, oyc - oyr, oxc + oxr, oyc - ody, oxc + oxr, oyc      );

  path.closeSubpath();
}

//---

void
drawArc(PaintDevice *device, const BBox &bbox, const Angle &angle, const Angle &dangle)
{
  QPainterPath path;

  arcPath(path, bbox, angle, dangle);

  device->drawPath(path);
}

void
arcPath(QPainterPath &path, const BBox &bbox, const Angle &angle, const Angle &dangle)
{
  path = QPainterPath();

  //---

  auto c = bbox.getCenter();

  auto rect = bbox.qrect();

  path.arcMoveTo(rect, -angle.value());
  path.arcTo    (rect, -angle.value(), -dangle.value());
  path.lineTo   (c.x, c.y);

  path.closeSubpath();
}

//---

void
drawArcSegment(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
               const Angle &angle, const Angle &dangle)
{
  QPainterPath path;

  arcSegmentPath(path, ibbox, obbox, angle, dangle);

  device->drawPath(path);
}

void
arcSegmentPath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
               const Angle &angle, const Angle &dangle)
{
  path = QPainterPath();

  // arc segment for start angle and delta angle for circles in inner and outer boxes
  auto angle2 = angle + dangle;

  auto irect = ibbox.qrect();
  auto orect = obbox.qrect();

  //---

  path.arcMoveTo(orect, -angle.value());
  path.arcTo    (orect, -angle.value() , -dangle.value());
  path.arcTo    (irect, -angle2.value(),  dangle.value());

  path.closeSubpath();
}

//---

void
drawArcsConnector(PaintDevice *device, const BBox &ibbox, const Angle &a1,
                  const Angle &da1, const Angle &a2, const Angle &da2, bool isSelf)
{
  QPainterPath path;

  arcsConnectorPath(path, ibbox, a1, da1, a2, da2, isSelf);

  // draw path
  device->drawPath(path);
}

void
arcsConnectorPath(QPainterPath &path, const BBox &ibbox, const Angle &a1,
                  const Angle &da1, const Angle &a2, const Angle &da2,
                  bool isSelf)
{
  path = QPainterPath();

  // draw connecting arc between inside of two arc segments
  // . arc segments have start angle and delta angle for circles in inner and outer boxes
  // isSelf is true if connecting arcs are the same
  auto a11 = a1 + da1;
  auto a21 = a2 + da2;

  auto irect = ibbox.qrect();
  auto c     = irect.center();

  //---

  path.arcMoveTo(irect, -a1 .value());   auto p1 = path.currentPosition();
  path.arcMoveTo(irect, -a11.value());   auto p2 = path.currentPosition();
  path.arcMoveTo(irect, -a2 .value()); //auto p3 = path.currentPosition();
  path.arcMoveTo(irect, -a21.value());   auto p4 = path.currentPosition();

  //--

  if (! isSelf) {
    path.moveTo(p1);
    path.quadTo(c, p4);
    path.arcTo (irect, -a21.value(), da2.value());
    path.quadTo(c, p2);
    path.arcTo (irect, -a11.value(), da1.value());

    path.closeSubpath();
  }
  else {
    path.moveTo(p1);
    path.quadTo(c, p2);
    path.arcTo (irect, -a11.value(), da1.value());

    path.closeSubpath();
  }
}

//---

// draw connecting edge, between two rectangles (line width from rectangle width)
// TODO: support path angle, offset control points by line width
void
edgePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox, const EdgeType &edgeType,
         const Angle &angle)
{
  auto orient = angle.orient();

  if (orient == Qt::Horizontal) {
    // ensure input on left, output on right
    double x1 = ibbox.getXMax(), x2 = obbox.getXMin();

    if (x1 > x2) {
      x1 = obbox.getXMax(); x2 = ibbox.getXMin();

      if (x1 < x2)
        return edgePath(path, obbox, ibbox, edgeType, angle);
    }
  }
  else {
    // ensure input on bottom, output on top
    double y1 = ibbox.getYMax(), y2 = obbox.getYMin();

    if (y1 > y2) {
      y1 = obbox.getYMax(); y2 = ibbox.getYMin();

      if (y1 < y2)
        return edgePath(path, obbox, ibbox, edgeType, angle);
    }
  }

  path = QPainterPath();

  if (orient == Qt::Horizontal) {
    double x1 = ibbox.getXMax(), x2 = obbox.getXMin();

    // start y range from source node, and end y range from dest node
    double y11 = ibbox.getYMax(), y12 = ibbox.getYMin();
    double y21 = obbox.getYMax(), y22 = obbox.getYMin();

    if      (edgeType == EdgeType::ARC) {
      // curve control point x at 1/3 and 2/3
      double x3 = CMathUtil::lerp(1.0/3.0, x1, x2);
      double x4 = CMathUtil::lerp(2.0/3.0, x1, x2);

      path.moveTo (QPointF(x1, y11));
      path.cubicTo(QPointF(x3, y11), QPointF(x4, y21), QPointF(x2, y21));
      path.lineTo (QPointF(x2, y22));
      path.cubicTo(QPointF(x4, y22), QPointF(x3, y12), QPointF(x1, y12));

      path.closeSubpath();
    }
    else if (edgeType == EdgeType::RECTILINEAR) {
      double xr  = CMathUtil::lerp(0.5, x1, x2);
      double lw  = std::min(ibbox.getHeight(), obbox.getHeight());
      double xr1 = xr - lw/2.0;
      double xr2 = xr + lw/2.0;

      path.moveTo(QPointF(x1, y11));

      if (ibbox.getYMid() < obbox.getYMid()) {
        path.lineTo(QPointF(xr1, y11));
        path.lineTo(QPointF(xr1, y21));
      }
      else {
        path.lineTo(QPointF(xr2, y11));
        path.lineTo(QPointF(xr2, y21));
      }

      path.lineTo(QPointF(x2, y21));
      path.lineTo(QPointF(x2, y22));

      if (ibbox.getYMid() < obbox.getYMid()) {
        path.lineTo(QPointF(xr2, y22));
        path.lineTo(QPointF(xr2, y12));
      }
      else {
        path.lineTo(QPointF(xr1, y22));
        path.lineTo(QPointF(xr1, y12));
      }

      path.lineTo(QPointF(x1, y12));
    }
    else if (edgeType == EdgeType::ROUNDED_LINE || edgeType == EdgeType::LINE) {
      path.moveTo(QPointF(x1, y11));
      path.lineTo(QPointF(x2, y21));
      path.lineTo(QPointF(x2, y22));
      path.lineTo(QPointF(x1, y12));
    }
  }
  else {
    double y1 = ibbox.getYMax(), y2 = obbox.getYMin();

    // start x range from source node, and end x range from dest node
    double x11 = ibbox.getXMax(), x12 = ibbox.getXMin();
    double x21 = obbox.getXMax(), x22 = obbox.getXMin();

    if      (edgeType == EdgeType::ARC) {
      // curve control point y at 1/3 and 2/3
      double y3 = CMathUtil::lerp(1.0/3.0, y1, y2);
      double y4 = CMathUtil::lerp(2.0/3.0, y1, y2);

      path.moveTo (QPointF(x11, y1));
      path.cubicTo(QPointF(x11, y3), QPointF(x21, y4), QPointF(x21, y2));
      path.lineTo (QPointF(x22, y2));
      path.cubicTo(QPointF(x22, y4), QPointF(x12, y3), QPointF(x12, y1));

      path.closeSubpath();
    }
    else if (edgeType == EdgeType::RECTILINEAR) {
      double yr  = CMathUtil::lerp(0.5, y1, y2);
      double lw  = std::min(ibbox.getWidth(), obbox.getWidth());
      double yr1 = yr - lw/2.0;
      double yr2 = yr + lw/2.0;

      path.moveTo(QPointF(x11, y1));

      if (ibbox.getXMid() < obbox.getXMid()) {
        path.lineTo(QPointF(x11, yr2));
        path.lineTo(QPointF(x21, yr2));
      }
      else {
        path.lineTo(QPointF(x11, yr1));
        path.lineTo(QPointF(x21, yr1));
      }

      path.lineTo(QPointF(x21, y2));
      path.lineTo(QPointF(x22, y2));

      if (ibbox.getXMid() < obbox.getXMid()) {
        path.lineTo(QPointF(x22, yr2));
        path.lineTo(QPointF(x12, yr2));
      }
      else {
        path.lineTo(QPointF(x22, yr1));
        path.lineTo(QPointF(x12, yr1));
      }

      path.lineTo(QPointF(x12, y1));
    }
    else if (edgeType == EdgeType::ROUNDED_LINE || edgeType == EdgeType::LINE) {
      path.moveTo(QPointF(x11, y1));
      path.lineTo(QPointF(x21, y2));
      path.lineTo(QPointF(x22, y2));
      path.lineTo(QPointF(x12, y1));
    }
  }
}

#if 0
// draw connecting edge, of line width, between two bounding boxes
void
drawEdgePath(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
             const EdgeType &edgeType, const Angle &angle1, const Angle &angle2)
{
  QPainterPath path;

  edgePath(path, ibbox, obbox, edgeType, angle1, angle2);

  device->drawPath(path);
}
#endif

// draw connecting edge, of line width, between two points
void
drawEdgePath(PaintDevice *device, const Point &p1, const Point &p2, double lw,
             const EdgeType &edgeType, const Angle &angle1, const Angle &angle2)
{
  QPainterPath path;

  edgePath(path, p1, p2, lw, edgeType, angle1, angle2);

  device->drawPath(path);
}

void
edgePath(QPainterPath &path, const Point &p1, const Point &p2, double lw,
         const EdgeType &edgeType, const Angle &angle1, const Angle &angle2)
{
  ConnectPoint c1(p1, angle1);
  ConnectPoint c2(p2, angle2);

  ConnectData data(edgeType, lw);

  path = edgePath(c1, c2, data);
}

QPainterPath
edgePath(const ConnectPoint &c1, const ConnectPoint &c2, const ConnectData &data)
{
  auto orient1 = c1.angle.orient();
  auto orient2 = c2.angle.orient();

  if      (orient1 == Qt::Horizontal && orient2 == Qt::Horizontal) {
    if (c1.p.x > c2.p.x)
      return edgePath(c2, c1, data);
  }
  else if (orient1 == Qt::Vertical && orient2 == Qt::Vertical) {
    if (c1.p.y > c2.p.y)
      return edgePath(c2, c1, data);
  }

  //---

  // convert line path into solid shape of line width
  auto strokerPath = [&](const QPainterPath &path, Qt::PenJoinStyle joinStyle) {
    QPainterPathStroker stroker;

    stroker.setCapStyle   (Qt::FlatCap);
    stroker.setDashOffset (0.0);
    stroker.setDashPattern(Qt::SolidLine);
    stroker.setJoinStyle  (joinStyle);
    stroker.setWidth      (data.lineWidth);

    auto path1 = stroker.createStroke(path);

    path1.setFillRule(Qt::WindingFill);

    return path1.simplified();
  };

  //---

  QPainterPath path;

  if      (data.edgeType == EdgeType::ARC) {
    auto calcControlFactor = [&]() {
      auto a1 = CQChartsGeom::pointAngle(c1.p, c2.p);
      double a2;
      if      (orient1 == Qt::Horizontal && orient2 == Qt::Horizontal)
        a2 = std::abs(std::sin(a1));
      else if (orient1 == Qt::Vertical && orient2 == Qt::Vertical)
        a2 = std::abs(std::cos(a1));
      else
        return 1.0/3.0;
      return CMathUtil::map(a2, 0.0, 1.0, 0.5, 0.1);
    };

    //---

    auto f1 = calcControlFactor();

#if 0
    auto f2 = 1.0 - f1;

    Point p3, p4;

    // curve control point f1
    if (orient1 == Qt::Horizontal)
      p3 = Point(CMathUtil::lerp(f1, c1.p.x, c2.p.x), c1.p.y);
    else
      p3 = Point(c1.p.x, CMathUtil::lerp(f1, c1.p.y, c2.p.y));

    // curve control point f2
    if (orient2 == Qt::Horizontal)
      p4 = Point(CMathUtil::lerp(f2, c1.p.x, c2.p.x), c2.p.y);
    else
      p4 = Point(c2.p.x, CMathUtil::lerp(f2, c1.p.y, c2.p.y));
#else
    auto len = c1.p.distanceTo(c2.p);

    auto p3 = CQChartsGeom::movePointOnLine(c1.p, c1.angle.radians(), f1*len);
    auto p4 = CQChartsGeom::movePointOnLine(c2.p, c2.angle.radians(), f1*len);
#endif

    //---

    QPainterPath lpath;

    lpath.moveTo(c1.p.qpoint());
    lpath.cubicTo(p3.qpoint(), p4.qpoint(), c2.p.qpoint());

    path = strokerPath(lpath, Qt::RoundJoin);
  }
  else if (data.edgeType == EdgeType::RECTILINEAR) {
    if (data.route) {
      CQChartsRectiConnect::Router router;

      std::vector<Point> route;

      CQChartsRectiConnect::Rect rect1(c1.bbox);
      CQChartsRectiConnect::Rect rect2(c2.bbox);

      auto angleSide = [](const Angle &angle) {
        auto orient = angle.orient();
        if (orient == Qt::Horizontal) {
          if (angle.cos() >= 0) return CQChartsRectiConnect::Rect::Side::RIGHT;
          else                  return CQChartsRectiConnect::Rect::Side::LEFT;
        }
        else {
          if (angle.sin() >= 0) return CQChartsRectiConnect::Rect::Side::TOP;
          else                  return CQChartsRectiConnect::Rect::Side::BOTTOM;
        }
      };

      rect1.setConnectPoint(c1.p);
      rect2.setConnectPoint(c2.p);

      rect1.setSide(angleSide(c1.angle));
      rect2.setSide(angleSide(c2.angle));

      if (! router.calcRoute(rect1, rect2, route))
        return path;

      auto lpath = pointsToPath(route);

      path = strokerPath(lpath, Qt::MiterJoin);

      return path;
    }

    //---

    std::vector<Point> points;

    points.push_back(c1.p);

    //---

    double s1 { 1.0 }, s2 { 1.0 };

    if (c1.bbox.isValid() && c2.bbox.isValid()) {
      s1 = (orient1 == Qt::Vertical ? c1.bbox.getHeight() : c1.bbox.getWidth());
      s2 = (orient2 == Qt::Vertical ? c2.bbox.getHeight() : c2.bbox.getWidth());
    }

    // calc points offset from bbox edge
    auto p1 = CQChartsGeom::movePointOnLine(c1.p, c1.angle.radians(), c1.offset*s1);
    auto p2 = CQChartsGeom::movePointOnLine(c2.p, c2.angle.radians(), c2.offset*s2);

    double xr = CMathUtil::lerp(0.5, p1.x, p2.x);
    double yr = CMathUtil::lerp(0.5, p1.y, p2.y);

    //---

    if      (orient1 == Qt::Horizontal && orient2 == Qt::Horizontal) {
      if (c1.bbox.isValid() && c2.bbox.isValid()) {
        points.push_back(p1);

        auto pm1 = Point(p1.x, yr);
        auto pm2 = Point(p2.x, yr);

        points.push_back(pm1);
        points.push_back(pm2);

        points.push_back(p2);
      }
      else {
        double xr = CMathUtil::lerp(0.5, c1.p.x, c2.p.x);

        points.push_back(Point(xr, c1.p.y));
        points.push_back(Point(xr, c2.p.y));
      }
    }
    else if (orient1 == Qt::Vertical && orient2 == Qt::Vertical) {
      if (c1.bbox.isValid() && c2.bbox.isValid()) {
        points.push_back(p1);

        auto pm1 = Point(xr, p1.y);
        auto pm2 = Point(xr, p2.y);

        points.push_back(pm1);
        points.push_back(pm2);

        points.push_back(p2);
      }
      else {
        double yr = CMathUtil::lerp(0.5, c1.p.y, c2.p.y);

        points.push_back(Point(c1.p.x, yr));
        points.push_back(Point(c2.p.x, yr));
      }
    }
    else {
      if (c1.bbox.isValid() && c2.bbox.isValid()) {
        // add first point
        points.push_back(p1);

        // create mid point between connect points of each rect
        auto pm = Point::avg(p1, p2);

        // create point to connect points to mid point by connect direction
        Point pm1, pm2;

        auto updateConnectPoints = [&]() {
          pm1 = (orient1 == Qt::Vertical ? Point(p1.x, pm.y) : Point(pm.x, p1.y));
          pm2 = (orient2 == Qt::Vertical ? Point(p2.x, pm.y) : Point(pm.x, p2.y));
        };

        updateConnectPoints();

        points.push_back(pm1);
        points.push_back(pm);
        points.push_back(pm2);

        points.push_back(p2);
      }
      else {
        // create mid point between connect points
        auto pm = Point::avg(c1.p, c2.p);

        if (orient1 == Qt::Vertical)
          points.push_back(Point(c1.p.x, pm.y));
        else
          points.push_back(Point(pm.x, c1.p.y));

        points.push_back(pm);

        if (orient2 == Qt::Vertical)
          points.push_back(Point(c2.p.x, pm.y));
        else
          points.push_back(Point(pm.x, c2.p.y));
      }
    }

    points.push_back(c2.p);

    auto lpath = pointsToPath(points);

    path = strokerPath(lpath, Qt::MiterJoin);
  }
  else if (data.edgeType == EdgeType::ROUNDED_LINE) {
    roundedLinePath(path, c1.p, c2.p, data.lineWidth);
  }
  else if (data.edgeType == EdgeType::LINE) {
    QPainterPath lpath;

    lpath.moveTo(c1.p.qpoint());
    lpath.lineTo(c2.p.qpoint());

    path = strokerPath(lpath, Qt::RoundJoin);
  }

  return path;
}

// draw connecting edge, of line width, from rect to itself
void
selfEdgePath(QPainterPath &path, const BBox &bbox, double lw,
             const EdgeType & /*edgeType*/, const Angle &angle)
{
  auto orient = angle.orient();

  double xr = bbox.getWidth ()/2.0;
  double yr = bbox.getHeight()/2.0;

  if (orient == Qt::Horizontal) {
    // draw arc from 45 degrees left of vertical to 45 degrees right of vertical
    double a = M_PI/4.0;

    double c = std::cos(a);
    double s = std::sin(a);

    double xm = bbox.getXMid();
    double ym = bbox.getYMid();

    double yt  = bbox.getYMax() + yr/2.0;
    double yt1 = yt - lw/2.0;
    double yt2 = yt + lw/2.0;

    double x1 = xm - xr*c, y1 = ym + yr*s;
    double x2 = xm + xr*c, y2 = y1;

    double lw1 = std::sqrt(2)*lw/2.0;

    path.moveTo (QPointF(x1 - lw1, y1 - lw1));
    path.cubicTo(QPointF(x1 - lw1, yt2), QPointF(x2 + lw1, yt2), QPointF(x2 + lw1, y2 - lw1));
    path.lineTo (QPointF(x2 - lw1, y2 + lw1));
    path.cubicTo(QPointF(x2 - lw1, yt1), QPointF(x1 + lw1, yt1), QPointF(x1 + lw1, y1 + lw1));

    path.closeSubpath();
  }
  else {
    // draw arc from 45 degrees above horizontal to 45 degrees below horizontal
    double a = M_PI/4.0;

    double c = std::cos(a);
    double s = std::sin(a);

    double xm = bbox.getXMid();
    double ym = bbox.getYMid();

    double xt  = bbox.getXMax() + xr/2.0;
    double xt1 = xt - lw/2.0;
    double xt2 = xt + lw/2.0;

    double x1 = xm - xr*c, y1 = ym + yr*s;
    double x2 = xm + xr*c, y2 = y1;

    double lw1 = std::sqrt(2)*lw/2.0;

    path.moveTo (QPointF(x1 - lw1, y1 - lw1));
    path.cubicTo(QPointF(xt2     , y1 - lw1), QPointF(xt2, y2 + lw1), QPointF(x2 - lw1, y2 + lw1));
    path.lineTo (QPointF(x2 + lw1, y2 - lw1));
    path.cubicTo(QPointF(xt1     , y2 - lw1), QPointF(xt1, y1 + lw1), QPointF(x1 + lw1, y1 + lw1));

    path.closeSubpath();
  }
}

//---

// draw connecting line between two bounding boxes
void
drawCurvePath(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
              const EdgeType &edgeType, const Angle &angle)
{
  QPainterPath path;

  curvePath(path, ibbox, obbox, edgeType, angle);

  device->drawPath(path);
}

// draw connecting line between two points
void
drawCurvePath(PaintDevice *device, const Point &p1, const Point &p2,
              const EdgeType &edgeType, const Angle &angle1, const Angle &angle2)
{
  QPainterPath path;

  curvePath(path, p1, p2, edgeType, angle1, angle2);

  device->drawPath(path);
}

// calculate path connecting line between two bounding boxes
void
curvePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
          const EdgeType &edgeType, const Angle &angle)
{
  ConnectPos p1, p2;

  p1.angle = angle;
  p2.angle = angle;

  rectConnectionPoints(ibbox, obbox, p1, p2);

  ConnectPoint c1(p1.p, p1.angle);
  ConnectPoint c2(p2.p, p2.angle);

  ConnectData data(edgeType);

  path = curvePath(c1, c2, data);
}

// calculate path connecting line between two points
void
curvePath(QPainterPath &path, const Point &p1, const Point &p4,
          const EdgeType &edgeType, const Angle &angle1, const Angle &angle2,
          double /*startLength*/, double /*endLength*/)
{
  ConnectPoint c1(p1, angle1);
  ConnectPoint c2(p4, angle2);

  ConnectData data(edgeType);

  path = curvePath(c1, c2, data);
}

// calculate path connecting line between two points
QPainterPath
curvePath(const ConnectPoint &c1, const ConnectPoint &c2, const ConnectData &data)
{
  auto orient1 = c1.angle.orient();
  auto orient2 = c2.angle.orient();

  if      (orient1 == Qt::Horizontal && orient2 == Qt::Horizontal) {
    if (c1.p.x > c2.p.x)
      return curvePath(c2, c1, data);
  }
  else if (orient1 == Qt::Vertical && orient2 == Qt::Vertical) {
    if (c1.p.y > c2.p.y)
      return curvePath(c2, c1, data);
  }

  //---

  QPainterPath path;

  //---

  if      (data.edgeType == EdgeType::ARC) {
    auto calcControlFactor = [&]() {
      auto a1 = CQChartsGeom::pointAngle(c1.p, c2.p);
      double a2;
      if      (orient1 == Qt::Horizontal && orient2 == Qt::Horizontal)
        a2 = std::abs(std::sin(a1));
      else if (orient1 == Qt::Vertical && orient2 == Qt::Vertical)
        a2 = std::abs(std::cos(a1));
      else
        return 1.0/3.0;
      return CMathUtil::map(a2, 0.0, 1.0, 0.5, 0.1);
    };

    //---

    auto f1 = calcControlFactor();

#if 0
    auto f2 = 1.0 - f1;

    Point p3, p4;

    // curve control point f1
    if (orient1 == Qt::Horizontal)
      p3 = Point(CMathUtil::lerp(f1, c1.p.x, c2.p.x), c1.p.y);
    else
      p3 = Point(c1.p.x, CMathUtil::lerp(f1, c1.p.y, c2.p.y));

    // curve control point f2
    if (orient2 == Qt::Horizontal)
      p4 = Point(CMathUtil::lerp(f2, c1.p.x, c2.p.x), c2.p.y);
    else
      p4 = Point(c2.p.x, CMathUtil::lerp(f2, c1.p.y, c2.p.y));
#else
    auto len = c1.p.distanceTo(c2.p);

    auto p3 = CQChartsGeom::movePointOnLine(c1.p, c1.angle.radians(), f1*len);
    auto p4 = CQChartsGeom::movePointOnLine(c2.p, c2.angle.radians(), f1*len);
#endif

    //---

    path.moveTo(c1.p.qpoint());
    path.cubicTo(p3.qpoint(), p4.qpoint(), c2.p.qpoint());

    return path;
  }
  else if (data.edgeType == EdgeType::RECTILINEAR) {
    if (data.route) {
      CQChartsRectiConnect::Router router;

      std::vector<Point> route;

      CQChartsRectiConnect::Rect rect1(c1.bbox);
      CQChartsRectiConnect::Rect rect2(c2.bbox);

      auto angleSide = [](const Angle &angle) {
        auto orient = angle.orient();
        if (orient == Qt::Horizontal) {
          if (angle.cos() >= 0) return CQChartsRectiConnect::Rect::Side::RIGHT;
          else                  return CQChartsRectiConnect::Rect::Side::LEFT;
        }
        else {
          if (angle.sin() >= 0) return CQChartsRectiConnect::Rect::Side::TOP;
          else                  return CQChartsRectiConnect::Rect::Side::BOTTOM;
        }
      };

      rect1.setConnectPoint(c1.p);
      rect2.setConnectPoint(c2.p);

      rect1.setSide(angleSide(c1.angle));
      rect2.setSide(angleSide(c2.angle));

      if (! router.calcRoute(rect1, rect2, route))
        return path;

      auto path = pointsToPath(route);

      return path;
    }

    //---

    path.moveTo(c1.p.qpoint());

    if      (orient1 == Qt::Horizontal && orient2 == Qt::Horizontal) {
      if (c1.bbox.isValid() && c2.bbox.isValid()) {
        auto w1 = c1.bbox.getWidth();
        auto w2 = c2.bbox.getWidth();

        auto p1 = CQChartsGeom::movePointOnLine(c1.p, c1.angle.radians(), c1.offset*w1);
        auto p2 = CQChartsGeom::movePointOnLine(c2.p, c2.angle.radians(), c2.offset*w2);

        path.lineTo(p1.qpoint());

        double yr = CMathUtil::lerp(0.5, p1.y, p2.y);

        path.lineTo(QPointF(p1.x, yr));
        path.lineTo(QPointF(p2.x, yr));

        path.lineTo(p2.qpoint());
      }
      else {
        double x2 = CMathUtil::lerp(0.5, c1.p.x, c2.p.x);

        path.lineTo(QPointF(x2, c1.p.y));
        path.lineTo(QPointF(x2, c2.p.y));
      }
    }
    else if (orient1 == Qt::Vertical && orient2 == Qt::Vertical) {
      if (c1.bbox.isValid() && c2.bbox.isValid()) {
        auto h1 = c1.bbox.getHeight();
        auto h2 = c2.bbox.getHeight();

        auto p1 = CQChartsGeom::movePointOnLine(c1.p, c1.angle.radians(), c1.offset*h1);
        auto p2 = CQChartsGeom::movePointOnLine(c2.p, c2.angle.radians(), c2.offset*h2);

        path.lineTo(p1.qpoint());

        double xr = CMathUtil::lerp(0.5, p1.x, p2.x);

        path.lineTo(QPointF(xr, p1.y));
        path.lineTo(QPointF(xr, p2.y));

        path.lineTo(p2.qpoint());
      }
      else {
        double y2 = CMathUtil::lerp(0.5, c1.p.y, c2.p.y);

        path.lineTo(QPointF(c1.p.x, y2));
        path.lineTo(QPointF(c2.p.x, y2));
      }
    }
    else {
      if (c1.bbox.isValid() && c2.bbox.isValid()) {
        auto s1 = (orient1 == Qt::Vertical ? c1.bbox.getHeight() : c1.bbox.getWidth());
        auto s2 = (orient2 == Qt::Vertical ? c2.bbox.getHeight() : c2.bbox.getWidth());

        auto p1 = CQChartsGeom::movePointOnLine(c1.p, c1.angle.radians(), c1.offset*s1);
        auto p2 = CQChartsGeom::movePointOnLine(c2.p, c2.angle.radians(), c2.offset*s2);

        path.lineTo(p1.qpoint());

        auto pm = Point::avg(p1, p2);

        if (orient1 == Qt::Vertical)
          path.lineTo(QPointF(p1.x, pm.y));
        else
          path.lineTo(QPointF(pm.x, p1.y));

        path.lineTo(pm.qpoint());

        if (orient2 == Qt::Vertical)
          path.lineTo(QPointF(p2.x, pm.y));
        else
          path.lineTo(QPointF(pm.x, p2.y));

        path.lineTo(p2.qpoint());
      }
      else {
        auto pm = Point::avg(c1.p, c2.p);

        if (orient1 == Qt::Vertical)
          path.lineTo(QPointF(c1.p.x, pm.y));
        else
          path.lineTo(QPointF(pm.x, c1.p.y));

        path.lineTo(pm.qpoint());

        if (orient2 == Qt::Vertical)
          path.lineTo(QPointF(c2.p.x, pm.y));
        else
          path.lineTo(QPointF(pm.x, c2.p.y));
      }
    }

    path.lineTo(c2.p.qpoint());

    return path;
  }
  else {
    path.moveTo(c1.p.qpoint());
    path.lineTo(c2.p.qpoint());

    return path;
  }
}

// draw connecting line from rect to itself
void
selfCurvePath(QPainterPath &path, const BBox &bbox,
              const EdgeType &edgeType, const Angle &/*angle*/)
{
  path = QPainterPath();

  //---

  double xr = bbox.getWidth ()/2.0;
  double yr = bbox.getHeight()/2.0;

  // draw arc from 45 degrees left of vertical to 45 degrees right of vertical
  double a = M_PI/4.0;

  double c = std::cos(a);
  double s = std::sin(a);

  double xm = bbox.getXMid();
  double ym = bbox.getYMid();

  double yt = bbox.getYMax() + yr/2.0;

  double x1 = xm - xr*c, y1 = ym + yr*s;
  double x2 = xm + xr*c, y2 = y1;

  if (edgeType == EdgeType::ARC) {
    path.moveTo (QPointF(x1, y1));
    path.cubicTo(QPointF(x1, yt), QPointF(x2, yt), QPointF(x2, y2));
  }
  else {
    path.moveTo(QPointF(x1, y1));
    path.lineTo(QPointF(x1, yt));
    path.lineTo(QPointF(x2, yt));
    path.lineTo(QPointF(x2, y2));
  }
}

//---

void
cornerHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double w = 16;

  double sx = device->pixelToWindowWidth (w);
  double sy = device->pixelToWindowHeight(w);

  path.addEllipse(p.x - sx/2, p.y - sy/2, sx, sy);
}

void
resizeHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double w1 = 12;
  double w2 = 4;

  double msx1 = device->pixelToWindowWidth (w1);
  double msy1 = device->pixelToWindowHeight(w1);
  double msx2 = device->pixelToWindowWidth (w2);
  double msy2 = device->pixelToWindowHeight(w2);

  path.moveTo(p.x - msx1, p.y       );
  path.lineTo(p.x - msx2, p.y + msy2);
  path.lineTo(p.x       , p.y + msy1);
  path.lineTo(p.x + msx2, p.y + msy2);
  path.lineTo(p.x + msx1, p.y       );
  path.lineTo(p.x + msx2, p.y - msy2);
  path.lineTo(p.x       , p.y - msy1);
  path.lineTo(p.x - msx2, p.y - msy2);

  path.closeSubpath();
}

// square
void
extraHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double sx = device->pixelToWindowWidth (16);
  double sy = device->pixelToWindowHeight(16);

  path.moveTo(QPointF(p.x - sx/2, p.y - sy/2));
  path.lineTo(QPointF(p.x + sx/2, p.y - sy/2));
  path.lineTo(QPointF(p.x + sx/2, p.y + sy/2));
  path.lineTo(QPointF(p.x - sx/2, p.y + sy/2));

  path.closeSubpath();
}

// diamond
void
controlHandlePath(PaintDevice *device, QPainterPath &path, const Point &p)
{
  double sx = device->pixelToWindowWidth (16);
  double sy = device->pixelToWindowHeight(16);

  path.moveTo(QPointF(p.x - sx/2, p.y       ));
  path.lineTo(QPointF(p.x       , p.y - sy/2));
  path.lineTo(QPointF(p.x + sx/2, p.y       ));
  path.lineTo(QPointF(p.x       , p.y + sy/2));

  path.closeSubpath();
}

//---

QString
clipTextToLength(PaintDevice *device, const QString &text,
                 const Length &clipLength, const Qt::TextElideMode &clipElide)
{
  if (! clipLength.isValid())
    return text;

  double clipLengthPixels = device->lengthPixelWidth(clipLength);

  return clipTextToLength(text, device->font(), clipLengthPixels, clipElide);
}

QString
clipTextToLength(const QString &text, const QFont &font, double clipLength,
                 const Qt::TextElideMode &clipElide)
{
  if (clipLength <= 0.0)
    return text;

  if (clipElide != Qt::ElideLeft && clipElide != Qt::ElideRight)
    return text;

  //---

  QFontMetricsF fm(font);

  auto ellipseStr = QString(QChar(0x2026));
  //auto ellipseStr = "...";

  double ellipsisWidth = fm.horizontalAdvance(ellipseStr);

  if (ellipsisWidth > clipLength)
    return "";

//double clipLength1 = clipLength - ellipsisWidth;

  //---

  auto isClipped = [&](const QString &str) {
    double w = fm.horizontalAdvance(str);

    return (w > clipLength);
  };

  auto isLenClipped = [&](const QString &str, int len) {
    if      (clipElide == Qt::ElideLeft)
      return isClipped(str.right(len));
    else if (clipElide == Qt::ElideRight)
      return isClipped(str.left(len));
    else
      assert(false);
  };

  //---

  if (! isClipped(text))
    return text;

  //---

  using LenClipped = std::map<int, bool>;

  LenClipped lenClipped;

  int len = text.length();

  int len1 = 0;
  int len2 = len;

  int midLen = (len1 + len2)/2;

  while (midLen > 0) {
    auto pl = lenClipped.find(midLen);

    bool clipped;

    if (pl == lenClipped.end()) {
      clipped = isLenClipped(text, midLen);

      lenClipped[midLen] = clipped;
    }
    else {
      clipped = (*pl).second;

      if (! clipped && midLen == len1)
        break;
    }

    if (clipped)
      len2 = midLen;
    else
      len1 = midLen;

    midLen = (len1 + len2)/2;
  }

  QString text1;

  if      (clipElide == Qt::ElideLeft)
    text1 = ellipseStr + text.right(midLen);
  else if (clipElide == Qt::ElideRight)
    text1 = text.left(midLen) + ellipseStr;
  else
    assert(false);

  return text1;
}

//---

void
drawPointLabel(PaintDevice *device, const Point &point, const QString &text, bool above)
{
  auto dx = device->pixelToWindowWidth (4);
  auto dy = device->pixelToWindowHeight(4);

  // draw cross symbol
  QPen tpen;

  auto tc = Qt::black;

  CQChartsUtil::setPen(tpen, true, tc);

  device->setPen(tpen);

  Point p1(point.x - dx, point.y     );
  Point p2(point.x + dx, point.y     );
  Point p3(point.x     , point.y - dy);
  Point p4(point.x     , point.y + dy);

  device->drawLine(p1, p2);
  device->drawLine(p3, p4);

  //---

  auto pp = device->windowToPixel(point);

  QFontMetricsF fm(device->font());

  double fw = fm.horizontalAdvance(text);
  double fa = fm.ascent();
  double fd = fm.descent();

  Point ppt(pp.x - fw/2, pp.y + (above ? -(fd + 4) : fa + 4));

  drawContrastText(device, device->pixelToWindow(ppt), text, CQChartsAlpha(0.5));
}

}

//------

namespace CQChartsDrawPrivate {

CQChartsGeom::Size
calcHtmlTextSize(const QString &text, const QFont &font, int margin)
{
  CQHtmlTextPainter textPainter;

  textPainter.setText(text);
  textPainter.setMargin(margin);
  textPainter.setFont(font);

  auto s = textPainter.textSize();

  return Size(s);
}

//------

void
drawScaledHtmlText(PaintDevice *device, const BBox &tbbox, const QString &text,
                   const TextOptions &options, double adjustScale, bool rotateRect)
{
  assert(tbbox.isValid());

  // calc scale (if not specified)
  double s = options.scale;

  if (s <= 0.0) {
    auto ptbbox = device->windowToPixel(tbbox);

    double pw = ptbbox.getWidth ();
    double ph = ptbbox.getHeight();

    int iter = 0;

    auto currentFont = device->font();

    double bestS = currentFont.pointSizeF();

    while (iter < 20) {
      auto psize = calcHtmlTextSize(text, currentFont, options.margin);

      double pw1 = psize.width ();
      double ph1 = psize.height();
      if (pw1 <= 0.0 || ph1 <= 0.0) return;

      if (pw1 < pw && ph1 < ph)
        bestS = currentFont.pointSizeF();

      auto xs = pw/pw1;
      auto ys = ph/ph1;

      auto s1 = std::min(xs, ys);

      if (std::abs(s1 - 1.0) < 1E-5)
        break;

      auto ps = currentFont.pointSizeF();
      currentFont.setPointSizeF(s1*ps);

      ++iter;
    }

    currentFont.setPointSizeF(bestS);
    s = currentFont.pointSizeF()/device->font().pointSizeF();
  }

  s *= adjustScale;

  //---

  // scale font
  CQChartsDrawUtil::updateScaledFontSize(s);

  bool zoomFont = device->isZoomFont();
  device->setZoomFont(false);

  options.calcFontScale = s;

  device->setFont(CQChartsUtil::scaleFontSize(device->font(), s));

  device->setZoomFont(zoomFont);

  //---

  drawHtmlText(device, tbbox, text, options, 0.0, 0.0, rotateRect);
}

void
drawHtmlText(PaintDevice *device, const BBox &tbbox,
             const QString &text, const TextOptions &options, double pdx, double pdy,
             bool rotateRect)
{
  drawHtmlText(device, tbbox.getCenter(), tbbox, text, options, pdx, pdy, rotateRect);
}

void
drawHtmlText(PaintDevice *device, const Point &center, const BBox &tbbox,
             const QString &text, const TextOptions &options, double pdx, double pdy,
             bool rotateRect)
{
  assert(tbbox.isValid());

  auto ptbbox = device->windowToPixel(tbbox);

  //---

  auto psize = calcHtmlTextSize(text, device->font(), options.margin);

  double c = options.angle.cos();
  double s = options.angle.sin();

  double pdx1 = 0.0, pdy1 = 0.0;

  if (! options.formatted) {
    if      (options.align & Qt::AlignHCenter)
      pdx1 = (ptbbox.getWidth() - psize.width())/2.0;
    else if (options.align & Qt::AlignRight)
      pdx1 = ptbbox.getWidth() - psize.width();

    if      (options.align & Qt::AlignVCenter)
      pdy1 = (ptbbox.getHeight() - psize.height())/2.0;
    else if (options.align & Qt::AlignBottom)
      pdy1 = ptbbox.getHeight() - psize.height();
  }

  if (! options.angle.isZero()) {
    if (rotateRect) {
      double pdx2 = c*pdx1 - s*pdy1;
      double pdy2 = s*pdx1 + c*pdy1;

      pdx1 = pdx2;
      pdy1 = pdy2;
    }
  }

  pdx += pdx1;
  pdy += pdy1;

//auto ptbbox1 = ptbbox.translated(pdx, pdy); // inner text rect
  auto ppw = psize.width ();
  auto pph = psize.height();

  auto ptbbox1 = BBox(ptbbox.getXMin() + pdx      , ptbbox.getYMin() + pdy,
                      ptbbox.getXMin() + pdx + ppw, ptbbox.getYMin() + pdy + pph);

  //---

  QImage    image;
  QPainter *painter  = nullptr;
  QPainter *ipainter = nullptr;

  if (device->isInteractive()) {
    painter = dynamic_cast<CQChartsViewPlotPaintDevice *>(device)->painter();
  }
  else {
    image = CQChartsUtil::initImage(QSize(int(ptbbox.getWidth()), int(ptbbox.getHeight())));

    ipainter = new QPainter(&image);

    painter = ipainter;
  }

  //---

  painter->save();

  //painter->drawRect(ptbbox .qrect()); // DEBUG
  //painter->drawRect(ptbbox1.qrect()); // DEBUG

  auto pcenter = device->windowToPixel(center).qpoint();

  if (! options.angle.isZero()) {
  //auto pcenter = ptbbox1.getCenter().qpoint();
    painter->translate(pcenter);
    painter->rotate(-options.angle.value());
    painter->translate(-pcenter);
  }

  QTextDocument td;

  td.setDocumentMargin(options.margin);
  td.setHtml(text);
  td.setDefaultFont(device->font());

  //auto ptbbox2 = ptbbox1.translated(-ptbbox.getXMin(), -ptbbox.getYMin()); // move to origin

  double tx = ptbbox1.getXMin();
  double ty = ptbbox1.getYMin();

  if (device->isInteractive()) {
    painter->translate(tx, ty);
  }

  //if (options.angle.isZero())
  //  painter->setClipRect(ptbbox2.qrect(), Qt::IntersectClip);

  double pw = ptbbox.getWidth();  // psize.width() ?
  double ph = ptbbox.getHeight(); // psize.width() ?
  double pm = 8;

  td.setPageSize(QSizeF(pw + pm, ph + pm));

  //---

  // TODO: setting 'global' align screws up existing align in html text
  if (options.alignHtml) {
    QTextCursor cursor(&td);

    cursor.select(QTextCursor::Document);

    QTextBlockFormat f;

    f.setAlignment(options.align);

    cursor.setBlockFormat(f);
  }

  //---

  auto pc = device->pen().color();

  QAbstractTextDocumentLayout::PaintContext ctx;

  auto *layout = td.documentLayout();

  layout->setPaintDevice(painter->device());

  if (options.contrast) {
    auto ipc = CQChartsUtil::bwColor(pc);

    ctx.palette.setColor(QPalette::Text, ipc);

    for (int dy = -2; dy <= 2; ++dy) {
      for (int dx = -2; dx <= 2; ++dx) {
        if (dx != 0 || dy != 0) {
          painter->translate(dx, dy);

          layout->draw(painter, ctx);

          painter->translate(-dx, -dy);
        }
      }
    }
  }

  ctx.palette.setColor(QPalette::Text, pc);

  layout->draw(painter, ctx);

  if (device->isInteractive()) {
    painter->translate(-tx, -ty);
  }

  //---

  painter->restore();

  delete ipainter;
}

}

//---

namespace CQChartsDrawUtil {

void
drawCheckBox(PaintDevice *device, double px, double py, int bs, bool checked)
{
  auto img = CQChartsUtil::initImage(QSize(bs, bs));

  img.fill(Qt::transparent);

  BBox bbox(0, 0, bs, bs);

  auto *view = (device->plot() ? device->plot()->view() : device->view());

  QStylePainter spainter(&img, view);

  spainter.setPen(device->pen());

  QStyleOptionButton opt;

  opt.initFrom(view);

  opt.rect = bbox.qrect().toRect();

  opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

  spainter.drawControl(QStyle::CE_CheckBox, opt);

  device->drawImage(device->pixelToWindow(Point(px, py)), CQChartsImage(img));
}

void
drawPushButton(PaintDevice *device, const BBox &prect, const QString &textStr,
               const ButtonData &buttonData)
{
  int pw = int(prect.getWidth());
  int ph = int(prect.getHeight());

  auto img = CQChartsUtil::initImage(QSize(pw, ph));

  img.fill(Qt::transparent);

  auto *view = (device->plot() ? device->plot()->view() : device->view());

  QStylePainter spainter(&img, view);

  QStyleOptionButton opt;

  opt.initFrom(view);

  opt.rect = QRect(0, 0, pw, ph);
  opt.text = textStr;

  if (buttonData.pressed)
    opt.state |= QStyle::State_Sunken;
  else
    opt.state |= QStyle::State_Raised;

  opt.state |= QStyle::State_Active;

  if (buttonData.enabled)
    opt.state |= QStyle::State_Enabled;

  if (buttonData.checkable) {
    if (buttonData.checked)
      opt.state |= QStyle::State_On;
    else
      opt.state |= QStyle::State_Off;
  }

  opt.palette = view->palette();

  spainter.setFont(device->font());

  auto bg = opt.palette.color(QPalette::Button);
  auto fg = opt.palette.color(QPalette::ButtonText);

  auto c = fg;

  if      (! buttonData.enabled)
    c = CQChartsUtil::blendColors(bg, fg, 0.6);
  else if (buttonData.inside)
    c = Qt::blue;

  opt.palette.setColor(QPalette::ButtonText, c);

  spainter.drawControl(QStyle::CE_PushButton, opt);

  int px = int(prect.getXMin());
  int py = int(prect.getYMin());

  device->painter()->drawImage(px, py, img);
}

}

//---

namespace CQChartsDrawUtil {

void
rectConnectionPoints(const BBox &rect1, const BBox &rect2, ConnectPos &pos1, ConnectPos &pos2,
                     const RectConnectData &connectData)
{
  pos2.slot = -1;

  if (! rectConnectionPoint(rect1, rect2, pos1, connectData)) {
    pos2.p     = rect2.getCenter();
    pos2.angle = Angle::pointAngle(pos2.p, pos1.p);
    return;
  }

  double a2;
  pos2.p = CQChartsUtil::nearestRectPoint(rect2, pos1.p, a2, connectData.useCorners);

  pos2.angle = Angle(a2);
}

bool
rectConnectionPoint(const BBox &rect1, const BBox &rect2, ConnectPos &pos,
                    const RectConnectData &connectData)
{
  struct ConnectionPoint {
    Point pos;
    int   priority { -1 };

    ConnectionPoint() { }

    ConnectionPoint(const Point &pos_, int priority_) :
     pos(pos_), priority(priority_) {
    }
  };

  std::vector<ConnectionPoint> points;

  auto addPoint = [&](const Point &p, int priority) {
    points.push_back(ConnectionPoint(p, priority));
  };

  //---

  double dxr = rect2.getXMin() - rect1.getXMax();
  double dxl = rect1.getXMin() - rect2.getXMax();
  double dyt = rect2.getYMin() - rect1.getYMax();
  double dyb = rect1.getYMin() - rect2.getYMax();

  //---

  // top right
  if      (dxr > connectData.gap && dyt > connectData.gap) {
    if (connectData.useCorners)
      addPoint(rect1.getUR(), 1);

    addPoint(rect1.getMidR(), 3);
    addPoint(rect1.getMidT(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getUR() + rect1.getMidT())/2.0, 2);
      addPoint((rect1.getUR() + rect1.getMidR())/2.0, 2);
    }
  }
  // bottom right
  else if (dxr > connectData.gap && dyb > connectData.gap) {
    if (connectData.useCorners)
      addPoint(rect1.getLR(), 1);

    addPoint(rect1.getMidR(), 3);
    addPoint(rect1.getMidB(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getLR() + rect1.getMidR())/2.0, 2);
      addPoint((rect1.getLR() + rect1.getMidB())/2.0, 2);
    }
  }
  // mid right
  else if (dxr > connectData.gap) {
    if (connectData.useCorners) {
      addPoint(rect1.getLR(), 1);
      addPoint(rect1.getUR(), 1);
    }

    addPoint(rect1.getMidR(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getLR() + rect1.getMidR())/2.0, 2);
      addPoint((rect1.getUR() + rect1.getMidR())/2.0, 2);
    }
  }

  // top left
  if       (dxl > connectData.gap && dyt > connectData.gap) {
    if (connectData.useCorners)
      addPoint(rect1.getUL(), 1);

    addPoint(rect1.getMidL(), 3);
    addPoint(rect1.getMidT(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getUL() + rect1.getMidL())/2.0, 2);
      addPoint((rect1.getUL() + rect1.getMidT())/2.0, 2);
    }
  }
  // bottom left
  else if (dxl > connectData.gap && dyb > connectData.gap) {
    if (connectData.useCorners)
      addPoint(rect1.getLL(), 1);

    addPoint(rect1.getMidL(), 3);
    addPoint(rect1.getMidB(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getLL() + rect1.getMidL())/2.0, 2);
      addPoint((rect1.getLL() + rect1.getMidB())/2.0, 2);
    }
  }
  // mid left
  else if (dxl > connectData.gap) {
    if (connectData.useCorners) {
      addPoint(rect1.getLL(), 1);
      addPoint(rect1.getUL(), 1);
    }

    addPoint(rect1.getMidL(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getLL() + rect1.getMidL())/2.0, 2);
      addPoint((rect1.getUL() + rect1.getMidL())/2.0, 2);
    }
  }
  // top center
  else if (dyt > connectData.gap) {
    if (connectData.useCorners) {
      addPoint(rect1.getUL(), 1);
      addPoint(rect1.getUR(), 1);
    }

    addPoint(rect1.getMidT(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getUL() + rect1.getMidT())/2.0, 2);
      addPoint((rect1.getUR() + rect1.getMidT())/2.0, 2);
    }
  }
  // bottom center
  else if (dyb > connectData.gap) {
    if (connectData.useCorners) {
      addPoint(rect1.getLL(), 1);
      addPoint(rect1.getLR(), 1);
    }

    addPoint(rect1.getMidB(), 3);

    if (connectData.useMidCorners) {
      addPoint((rect1.getLL() + rect1.getMidB())/2.0, 2);
      addPoint((rect1.getLR() + rect1.getMidB())/2.0, 2);
    }
  }

  // center
  auto c2 = rect2.getCenter();

  addPoint(rect1.getCenter(), 0);

  //---

  double d        = 0;
  int    priority = -1;
  int    slot     = -1;

  for (uint i = 0; i < points.size(); ++i) {
    if (connectData.occupiedSlots.find(i) != connectData.occupiedSlots.end())
      continue;

    auto d1 = points[i].pos.distanceTo(c2);

    if (slot < 0 || points[i].priority > priority || d1 < d) {
      d        = d1;
      priority = points[i].priority;
      slot     = i;
    }
  }

  if (slot < 0) {
    pos.p     = rect1.getCenter();
    pos.angle = Angle::pointAngle(pos.p, c2);
    pos.slot  = int(points.size() + 1);

    return false;
  }

  pos.p     = points[slot].pos;
  pos.angle = Angle::pointAngle(pos.p, c2);
  pos.slot  = slot;

  return true;
}

void
circleConnectionPoints(const BBox &rect1, const BBox &rect2, ConnectPos &pos1, ConnectPos &pos2,
                       const CircleConnectData &connectData)
{
  circleConnectionPoint(rect1, rect2, pos1, connectData);
  circleConnectionPoint(rect2, rect1, pos2, connectData);
}

void
circleConnectionPoint(const BBox &rect1, const BBox &rect2, ConnectPos &pos,
                      const CircleConnectData &connectData)
{
  auto c1 = rect1.getCenter();
  auto c2 = rect2.getCenter();

  auto rx1 = rect1.getWidth ()/2;
  auto ry1 = rect1.getHeight()/2;
  auto rx2 = rect2.getWidth ()/2;
  auto ry2 = rect2.getHeight()/2;

  return circleConnectionPoint(c1, rx1, ry1, c2, rx2, ry2, pos, connectData);
}

void
circleConnectionPoint(const Point &c1, double rx1, double ry1,
                      const Point &c2, double /*rx2*/, double /*ry2*/,
                      ConnectPos &pos, const CircleConnectData &connectData)
{
  pos.angle = Angle::pointAngle(c1, c2);

  pos.p    = CQChartsGeom::ellipsePoint(c1, rx1, ry1, pos.angle.radians());
  pos.slot = -1;

  if (connectData.numSlots > 0) {
    auto sa = Angle(0);
    auto da = Angle(360)/connectData.numSlots;

    int    nearestSlot = -1;
    Point  nearestP;
    double nearestDist { 0 };
    Angle  nearestAngle;

    for (int i = 0; i < connectData.numSlots; ++i) {
      if (connectData.occupiedSlots.find(i) != connectData.occupiedSlots.end())
        continue;

      auto sa1 = sa + da*i;

      auto p1 = CQChartsGeom::ellipsePoint(c1, rx1, ry1, sa1.radians());
      auto d1 = pos.p.distanceTo(p1);

      if (nearestSlot < 0 || d1 < nearestDist) {
        nearestSlot  = i;
        nearestP     = p1;
        nearestDist  = d1;
        nearestAngle = sa1;
      }
    }

    if (nearestSlot >= 0) {
      pos.p     = nearestP;
      pos.angle = nearestAngle;
      pos.slot  = nearestSlot;
    }
  }
}

QPointF pathMidPoint(const QPainterPath &path)
{
  return path.pointAtPercent(0.5);
}

}

//---

namespace CQChartsDrawUtil {

double
lengthPixelWidth(PaintDevice *device, const Length &len)
{
  return device->lengthPixelWidth(len);
}

}

//---

namespace CQChartsDrawUtil {

void
visitPath(const QPainterPath &path, PathVisitor &visitor)
{
  visitor.path = &path;

  visitor.n = path.elementCount();

  visitor.init();

  for (visitor.i = 0; visitor.i < visitor.n; ++visitor.i) {
    const auto &e = path.elementAt(visitor.i);

    if      (e.isMoveTo()) {
      Point p(e.x, e.y);

      if (visitor.i < visitor.n - 1) {
        auto e1 = path.elementAt(visitor.i + 1);

        visitor.nextP = Point(e1.x, e1.y);
      }
      else
        visitor.nextP = p;

      visitor.moveTo(p);

      visitor.lastP = p;
    }
    else if (e.isLineTo()) {
      Point p(e.x, e.y);

      if (visitor.i < visitor.n - 1) {
        auto e1 = path.elementAt(visitor.i + 1);

        visitor.nextP = Point(e1.x, e1.y);
      }
      else
        visitor.nextP = p;

      visitor.lineTo(p);

      visitor.lastP = p;
    }
    else if (e.isCurveTo()) {
      Point p(e.x, e.y);

      Point p1, p2;

      QPainterPath::ElementType e1t { QPainterPath::MoveToElement };
      QPainterPath::ElementType e2t { QPainterPath::MoveToElement };

      if (visitor.i < visitor.n - 1) {
        auto e1 = path.elementAt(visitor.i + 1);

        e1t = e1.type;

        p1 = Point(e1.x, e1.y);
      }

      if (visitor.i < visitor.n - 2) {
        auto e2 = path.elementAt(visitor.i + 2);

        e2t = e2.type;

        p2 = Point(e2.x, e2.y);
      }

      if (e1t == QPainterPath::CurveToDataElement) {
        ++visitor.i;

        if (e2t == QPainterPath::CurveToDataElement) {
          ++visitor.i;

          if (visitor.i < visitor.n - 1) {
            auto e3 = path.elementAt(visitor.i + 1);

            visitor.nextP = Point(e3.x, e3.y);
          }
          else
            visitor.nextP = p;

          visitor.curveTo(p, p1, p2);

          visitor.lastP = p;
        }
        else {
          if (visitor.i < visitor.n - 1) {
            auto e3 = path.elementAt(visitor.i + 1);

            visitor.nextP = Point(e3.x, e3.y);
          }
          else
            visitor.nextP = p;

          visitor.quadTo(p, p1);

          visitor.lastP = p;
        }
      }
    }
    else
      assert(false);
  }

  visitor.term();
}

}

//---

namespace CQChartsDrawUtil {

void
drawBarChart(PaintDevice *device, const BBox &bbox, const std::vector<double> &values,
             const CQChartsOptReal &maxValue, const QString &paletteName, const PenBrush &penBrush)
{
  auto *device1 = dynamic_cast<CQChartsViewPlotPaintDevice *>(device);

  auto *plot = device1->plot();
  assert(plot);

  auto nx = values.size();
  if (nx == 0) return;

  // draw bar for each value
  CQChartsGeom::RMinMax range;

  range.add(0.0);

  for (size_t ix = 0; ix < nx; ++ix)
    range.add(values[ix]);

  if (maxValue.isSet())
    range.add(maxValue.real());

  double width = bbox.getWidth();

  double dx = (nx > 0 ? width/nx : 0.0);

  double x = bbox.getXMin();
  double y = bbox.getYMin();

  auto color = CQChartsColor::makePalette();

  color.setPaletteName(paletteName);

  for (size_t ix = 0; ix < nx; ++ix) {
    PenBrush penBrush1 = penBrush;

    auto c = plot->interpColor(color, CQChartsUtil::ColorInd(ix, nx));

    penBrush1.brush.setColor(c);

    CQChartsDrawUtil::setPenBrush(device, penBrush1);

    //---

    double y1 = CMathUtil::map(values[ix], range.min(), range.max(),
                               bbox.getYMin(), bbox.getYMax());

    BBox bbox1(x, y, x + dx, y1);

    device->drawRect(bbox1);

    x += dx;
  }
}

}
