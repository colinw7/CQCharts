#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <CQChartsTextOptions.h>
#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsSides.h>
#include <CQChartsAlpha.h>
#include <CQChartsData.h>
#include <CQChartsGeom.h>

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QString>

#include <set>

class  CQChartsPaintDevice;
struct CQChartsPenBrush;
class  CQChartsBrushData;

//---

namespace CQChartsDrawUtil {

inline double *scaledFontSizeP() {
  static double scaledFontSize { -1 };

  return &scaledFontSize;
}

inline void resetScaledFontSize() { *scaledFontSizeP() = -1; }

inline double scaledFontSize() { return *scaledFontSizeP(); }

inline void updateScaledFontSize(double s) {
  if (*scaledFontSizeP() < 0)
    *scaledFontSizeP() = s;
  else
    *scaledFontSizeP() = std::min(*scaledFontSizeP(), s);
}

}

//---

// polygon
namespace CQChartsDrawUtil {

using Polygon = CQChartsGeom::Polygon;

inline QPainterPath polygonToPath(const double *x, const double *y, int n, bool closed=false) {
  QPainterPath path;

  for (int i = 0; i < n; ++i) {
    if (i == 0)
      path.moveTo(x[i], y[i]);
    else
      path.lineTo(x[i], y[i]);
  }

  if (closed)
    path.closeSubpath();

  return path;
}

inline QPainterPath polygonToPath(const Polygon &polygon, bool closed=false) {
  QPainterPath path;

  path.moveTo(polygon.qpoint(0));

  for (int i = 1; i < polygon.size(); ++i)
    path.lineTo(polygon.qpoint(i));

  if (closed)
    path.closeSubpath();

  return path;
}

inline QPainterPath rotatePath(const QPainterPath &path, const QPointF &c, double angle) {
  QTransform t;

  t.translate(c.x(), c.y());
  t.rotate(angle);
  t.translate(-c.x(), -c.y());

  return t.map(path);
}

inline QPainterPath rotatePath(const QPainterPath &path, double angle) {
  return rotatePath(path, path.boundingRect().center(), angle);
}

}

//---

// pen/brush
namespace CQChartsDrawUtil {

using PaintDevice = CQChartsPaintDevice;
using PenBrush    = CQChartsPenBrush;
using BrushData   = CQChartsBrushData;

void setPenBrush(PaintDevice *device, const PenBrush &penBrush);
void setPenBrush(QPainter *device, const PenBrush &penBrush);

void setBrush(QBrush &brush, const BrushData &data);

void updateBrushColor(QBrush &brush, const QColor &c);

void setPenGray  (QPen   &pen  , double alpha=1.0);
void setBrushGray(QBrush &brush, double alpha=1.0);

inline double brushAlpha(QBrush &brush) { return brush.color().alphaF(); }

void setBrushAlpha(QBrush &brush, double a);
void setPenAlpha(QPen &pen, double a);

}

//---

// misc drawing
namespace CQChartsDrawUtil {

using PaintDevice = CQChartsPaintDevice;
using Length      = CQChartsLength;
using Sides       = CQChartsSides;
using TextOptions = CQChartsTextOptions;
using Alpha       = CQChartsAlpha;
using Symbol      = CQChartsSymbol;
using Angle       = CQChartsAngle;
using BBox        = CQChartsGeom::BBox;
using Size        = CQChartsGeom::Size;
using Point       = CQChartsGeom::Point;
using EdgeType    = CQChartsEdgeType;

void drawShape(PaintDevice *device, const CQChartsShapeTypeData &shapeData, const BBox &rect);
void drawShapeSwatch(PaintDevice *device, const CQChartsShapeTypeData &shapeData,
                     const BBox &rect, double size=0.1);

void drawDotLine(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
                 const Length &lineWidth, bool horizontal, const Symbol &symbol,
                 const Length &symbolSize, const PenBrush &symbolPenBrush,
                 const Angle &angle=Angle());

void drawRoundedRect(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
                     const Length &size=Length(), const Sides &sides=Sides(Sides::Side::ALL),
                     const Angle &angle=Angle());
void drawRoundedRect(PaintDevice *device, const BBox &bbox, const Length &size=Length(),
                     const Sides &sides=Sides(Sides::Side::ALL),
                     const Angle &angle=Angle());
void drawRoundedRect(PaintDevice *device, const BBox &bbox, const Length &xsize,
                     const Length &ysize, const Sides &sides=Sides(Sides::Side::ALL),
                     const Angle &angle=Angle());

void drawRoundedPolygon(PaintDevice *device, const PenBrush &penBrush, const Polygon &poly,
                        const Length &size=Length(), const Angle &angle=Angle());
void drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &size=Length(),
                        const Angle &angle=Angle());
void drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &xsize,
                        const Length &ysize, const Angle &angle=Angle());

void drawAdjustedRoundedRect(PaintDevice *device, const BBox &bbox, double xsize, double ysize,
                             const CQChartsSides &sides, const Angle &angle=Angle());

void drawTextInCircle(PaintDevice *device, const BBox &rect, const QString &text,
                      const TextOptions &options);
void drawTextInBox(PaintDevice *device, const BBox &rect, const QString &text,
                   const TextOptions &options, double adjustScale=1.0);

void drawTextsInCircle(PaintDevice *device, const BBox &rect, const QStringList &strs,
                       const TextOptions &options);
void drawTextsInBox(PaintDevice *device, const BBox &rect, const QStringList &strs,
                    const TextOptions &options, double adjustScale=1.0);

void drawRotatedTextInBox(PaintDevice *device, const BBox &rect, const QString &text,
                          const QPen &pen, const TextOptions &options);

BBox calcTextAtPointRect(PaintDevice *device, const Point &point, const QString &text,
                         const TextOptions &options=TextOptions(), bool centered=false,
                         double dx=0.0, double dy=0.0);

void drawTextsAtPoint(PaintDevice *device, const Point &point, const QStringList &texts,
                      const TextOptions &options);

void drawTextAtPoint(PaintDevice *device, const Point &p, const QString &text,
                     const TextOptions &options=TextOptions(), bool centered=false,
                     double dx=0.0, double dy=0.0);

void drawAlignedText(PaintDevice *device, const Point &p, const QString &text,
                     Qt::Alignment align, double dx=0.0, double dy=0.0);

BBox calcAlignedTextRect(PaintDevice *device, const QFont &font, const Point &p,
                         const QString &text, Qt::Alignment align, double dx, double dy);

void drawContrastText(PaintDevice *device, const Point &p, const QString &text,
                      const Alpha &alpha);

Size calcTextSize(const QString &text, const QFont &font, const TextOptions &options);

void drawCenteredText(PaintDevice *device, const Point &pos, const QString &text);

void drawSimpleText(PaintDevice *device, const Point &pos, const QString &text);

//---

void drawSelectedOutline(PaintDevice *device, const BBox &rect,
                         double margin=0.2, double width=0.1);

//---

void drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
                const Point &c, const Length &size, bool scale=false);
void drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
                const Point &c, const Length &xsize, const Length &ysize, bool scale=false);

void drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c,
                const Length &size, bool scale=false);
void drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c,
                const Length &xsize, const Length &ysize, bool scale=false);

void drawSymbol(PaintDevice *device, const Symbol &symbol, const BBox &bbox, bool scale=false);

//---

void drawPolygonSides(PaintDevice *device, const BBox &bbox, int n, const Angle &angle);
bool polygonSidesPath(QPainterPath &path, const BBox &bbox, int n, const Angle &angle=Angle());

void drawDiamond(PaintDevice *device, const BBox &bbox);
void diamondPath(QPainterPath &path, const BBox &bbox);

void trianglePath(QPainterPath &path, const Point &p1, const Point &p2, const Point &p3);

void linePath(QPainterPath &path, const Point &p1, const Point &p2);

void editHandlePath(PaintDevice *device, QPainterPath &path, const BBox &bbox);

//---

void drawRoundedLine(PaintDevice *device, const Point &p1, const Point &p2, double w);
void fillRoundedLine(PaintDevice *device, const Point &p1, const Point &p2, double w);
void roundedLinePath(QPainterPath &path, const Point &p1, const Point &p2, double w);

#if 0

#if DEBUG_LABELS
QPainterPath pathAddRoundedEnds(PaintDevice *device, const QPainterPath &path, double w);
#else
QPainterPath pathAddRoundedEnds(const QPainterPath &path, double w);
#endif

#endif

bool roundedPolygonPath(QPainterPath &path, const Polygon &poly, double xsize, double ysize);

//---

void drawPieSlice(PaintDevice *device, const Point &c, double ri, double ro, const Angle &a1,
                  const Angle &a2, bool isInvertX=false, bool isInvertY=false);
void pieSlicePath(QPainterPath &path, const Point &c, double ri, double ro, const Angle &a1,
                  const Angle &a2, bool isInvertX=false, bool isInvertY=false);

//---

void drawEllipse(PaintDevice *device, const BBox &bbox);
void drawDoubleEllipse(PaintDevice *device, const BBox &bbox);
void ellipsePath(QPainterPath &path, const BBox &bbox);
void outlinePath(QPainterPath &path, const BBox &bbox1, const BBox &bbox2);

//---

void drawParetoGradient(PaintDevice *device, const Point &origin,
                        const BBox &bbox, const QColor &c1, const QColor &c2);

//--

void drawArc(PaintDevice *device, const BBox &bbox, const Angle &angle, const Angle &dangle);
void arcPath(QPainterPath &path, const BBox &bbox, const Angle &angle, const Angle &dangle);

//---

void drawArcSegment(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
                    const Angle &angle, const Angle &dangle);
void arcSegmentPath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
                    const Angle &angle, const Angle &dangle);

//---

void drawArcsConnector(PaintDevice *device, const BBox &ibbox, const Angle &a1, const Angle &da1,
                       const Angle &a2, const Angle &da2, bool isSelf=false);
void arcsConnectorPath(QPainterPath &path, const BBox &ibbox, const Angle &a1, const Angle &da1,
                       const Angle &a2, const Angle &da2, bool isSelf=false);

//---

void drawEdgePath(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
                  const EdgeType &edgeType=EdgeType::ARC,
                  const Angle &angle=Angle());
void edgePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
              const EdgeType &edgeType=EdgeType::ARC,
              const Angle &angle=Angle());

void drawEdgePath(PaintDevice *device, const Point &p1, const Point &p2, double lw,
                  const EdgeType &edgeType=EdgeType::ARC,
                  const Angle &angle1=Angle(), const Angle &angle2=Angle());
void edgePath(QPainterPath &path, const Point &p1, const Point &p2, double lw,
              const EdgeType &edgeType=EdgeType::ARC,
              const Angle &angle1=Angle(), const Angle &angle2=Angle());

void selfEdgePath(QPainterPath &path, const BBox &bbox, double lw,
                  const EdgeType &edgeType=EdgeType::ARC,
                  const Angle &angle=Angle());

//---

void drawCurvePath(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
                   const EdgeType &edgeType=EdgeType::ARC,
                   const Angle &angle=Angle());
void curvePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
               const EdgeType &edgeType=EdgeType::ARC,
               const Angle &angle=Angle());

void drawCurvePath(PaintDevice *device, const Point &p1, const Point &p2,
                   const EdgeType &edgeType=EdgeType::ARC,
                   const Angle &angle1=Angle(), const Angle &angle2=Angle());
void curvePath(QPainterPath &path, const Point &p1, const Point &p2,
               const EdgeType &edgeType=EdgeType::ARC,
               const Angle &angle1=Angle(), const Angle &angle2=Angle(),
               double startLength=0.0, double endLength=0.0);

void selfCurvePath(QPainterPath &path, const BBox &bbox, const EdgeType &edgeType=EdgeType::ARC,
                   const Angle &angle=Angle());

//---

void cornerHandlePath (PaintDevice *device, QPainterPath &path, const Point &p);
void resizeHandlePath (PaintDevice *device, QPainterPath &path, const Point &p);
void extraHandlePath  (PaintDevice *device, QPainterPath &path, const Point &p);
void controlHandlePath(PaintDevice *device, QPainterPath &path, const Point &p);

//---

// clip text
QString clipTextToLength(PaintDevice *device, const QString &text, const QFont &font,
                         const Length &clipLength, const Qt::TextElideMode &clipElide);

QString clipTextToLength(const QString &text, const QFont &font, double clipLength,
                         const Qt::TextElideMode &clipElide);

//---

void drawPointLabel(PaintDevice *device, const Point &point, const QString &text, bool above);

}

//---

// html text
namespace CQChartsDrawPrivate {

using PaintDevice = CQChartsPaintDevice;
using TextOptions = CQChartsTextOptions;
using BBox        = CQChartsGeom::BBox;
using Size        = CQChartsGeom::Size;
using Point       = CQChartsGeom::Point;

// private
Size calcHtmlTextSize(const QString &text, const QFont &font, int margin=0);

void drawScaledHtmlText(PaintDevice *device, const BBox &tbbox, const QString &text,
                        const TextOptions &options, double adjustScale=1.0);

void drawHtmlText(PaintDevice *device, const BBox &tbbox,
                  const QString &text, const TextOptions &options,
                  double pdx=0.0, double pdy=0.0);
void drawHtmlText(PaintDevice *device, const Point &center, const BBox &tbbox,
                  const QString &text, const TextOptions &options,
                  double pdx=0.0, double pdy=0.0);

}

//---

namespace CQChartsDrawUtil {
  using PaintDevice = CQChartsPaintDevice;

  struct ButtonData {
    bool  pressed   { false };
    bool  enabled   { true };
    bool  checkable { false };
    bool  checked   { false };
    bool  inside    { false };
  };

  void drawCheckBox(PaintDevice *device, double px, double py, int bs, bool checked);

  void drawPushButton(PaintDevice *device, const BBox &prect, const QString &textStr,
                      const ButtonData &buttonData);
}

//---

namespace CQChartsDrawUtil {
  using PaintDevice = CQChartsPaintDevice;

  double lengthPixelWidth(PaintDevice *device, const Length &len);
}

//---

namespace CQChartsDrawUtil {

// path visitor
class PathVisitor {
 public:
  PathVisitor() = default;

  virtual ~PathVisitor() = default;

  virtual void init() { }
  virtual void term() { }

  virtual void moveTo (const Point &p) = 0;
  virtual void lineTo (const Point &p) = 0;
  virtual void quadTo (const Point &p1, const Point &p2) = 0;
  virtual void curveTo(const Point &p1, const Point &p2, const Point &p3) = 0;

 public:
  const QPainterPath *path { nullptr };

  int   i { -1 };
  int   n { 0 };
  Point lastP;
  Point nextP;
};

void visitPath(const QPainterPath &path, PathVisitor &visitor);

}

//---

namespace CQChartsDrawUtil {

struct ConnectPos {
  Point p;
  Angle angle;
  int   slot { -1 };
};

struct RectConnectData {
  bool          useCorners { false };
  double        gap { 0.0 };
  std::set<int> occupiedSlots;
};

struct CircleConnectData {
  int           numSlots { -1 };
  double        gap { 0.0 };
  std::set<int> occupiedSlots;
};

void rectConnectionPoints(const BBox &rect1, const BBox &rect2,
                          ConnectPos &pos1, ConnectPos &pos2,
                          const RectConnectData &connectData=RectConnectData());
bool rectConnectionPoint(const BBox &rect1, const BBox &rect2, ConnectPos &pos,
                         const RectConnectData &connectData=RectConnectData());

void circleConnectionPoints(const BBox &rect1, const BBox &rect2,
                            ConnectPos &pos1, ConnectPos &pos2,
                            const CircleConnectData &connectData=CircleConnectData());
void circleConnectionPoint(const BBox &rect1, const BBox &rect2, ConnectPos &pos,
                           const CircleConnectData &connectData=CircleConnectData());
void circleConnectionPoint(const Point &c1, double r1, const Point &c2, double r2, ConnectPos &pos,
                           const CircleConnectData &connectData=CircleConnectData());

QPointF pathMidPoint(const QPainterPath &path);

}

//---

namespace CQChartsDrawUtil {

inline QColor setColorAlpha(QColor &c, const CQChartsAlpha &a) {
  c.setAlphaF(a.valueOr(1.0));
  return c;
}

}

#endif
