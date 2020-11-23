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

class CQChartsPaintDevice;

/*!
 * \brief Pen/Brush Data
 * \ingroup Charts
 */
struct CQChartsPenBrush {
  QPen   pen;
  QBrush brush;
  QColor altColor;
  double fillAngle { 45 };

  CQChartsPenBrush() = default;

  CQChartsPenBrush(const QPen &pen, const QBrush &brush) :
   pen(pen), brush(brush) {
  }
};

//---

/*!
 * \brief Pen Data
 *
 * visible, color, alpha, width, dash
 */
class CQChartsPenData {
 public:
  using Alpha    = CQChartsAlpha;
  using Length   = CQChartsLength;
  using LineDash = CQChartsLineDash;
  using LineCap  = CQChartsLineCap;

 public:
  CQChartsPenData() = default;

  explicit CQChartsPenData(bool visible, const QColor &color=QColor(), const Alpha &alpha=Alpha(),
                           const Length &width=Length(), const LineDash &dash=LineDash(),
                           const LineCap &lineCap=LineCap()) :
   visible_(visible), color_(color), alpha_(alpha), width_(width),
   dash_(dash), lineCap_(lineCap) {
  }

  CQChartsPenData(bool visible, const QColor &color, const CQChartsStrokeData &strokeData) :
   visible_(visible), color_(color), alpha_(strokeData.alpha()), width_(strokeData.width()),
   dash_(strokeData.dash()) {
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set color
  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  //! get/set alpha
  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  //! get/set width
  const Length &width() const { return width_; }
  void setWidth(const Length &v) { width_ = v; }

  //! get/set line dash
  const LineDash &dash() const { return dash_; }
  void setDash(const LineDash &v) { dash_ = v; }

  //! get/set line cap
  const LineCap &lineCap() const { return lineCap_; }
  void setLineCap(const LineCap &c) { lineCap_ = c; }

 private:
  bool     visible_  { true };  //!< visible
  QColor   color_;              //!< pen color
  Alpha    alpha_;              //!< pen alpha
  Length   width_    { "0px" }; //!< pen width
  LineDash dash_;               //!< pen line dash
  LineCap  lineCap_;            //!< pen line cap
};

//---

/*!
 * \brief Brush Data
 *
 * visible, color, alpha, pattern
 */
class CQChartsBrushData {
 public:
  using Alpha       = CQChartsAlpha;
  using FillPattern = CQChartsFillPattern;

 public:
  CQChartsBrushData() = default;

  explicit CQChartsBrushData(bool visible, const QColor &color=QColor(),
                             const Alpha &alpha=Alpha(), const FillPattern &pattern=FillPattern()) :
   visible_(visible), color_(color), alpha_(alpha), pattern_(pattern) {
  }

  CQChartsBrushData(bool visible, const QColor &color, const CQChartsFillData &fillData) :
   visible_(visible), color_(color), alpha_(fillData.alpha()), pattern_(fillData.pattern()) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  const FillPattern &pattern() const { return pattern_; }
  void setPattern(const FillPattern &v) { pattern_ = v; }

  //---

 private:
  bool        visible_ { true };                     //!< visible
  QColor      color_;                                //!< fill color
  Alpha       alpha_;                                //!< fill alpha
  FillPattern pattern_ { FillPattern::Type::SOLID }; //!< fill pattern
};

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

}

//---

// pen/brush
namespace CQChartsDrawUtil {

using PenBrush  = CQChartsPenBrush;
using BrushData = CQChartsBrushData;

void setPenBrush(CQChartsPaintDevice *device, const PenBrush &penBrush);
void setPenBrush(QPainter *device, const PenBrush &penBrush);

void setBrush(QBrush &brush, const BrushData &data);

inline void setBrushAlpha(QBrush &brush, double a) {
  auto c = brush.color();

  c.setAlphaF(a);

  brush.setColor(c);
}

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

void drawDotLine(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
                 const Length &lineWidth, bool horizontal,
                 const Symbol &symbolType, const Length &symbolSize);

void drawRoundedPolygon(PaintDevice *device, const PenBrush &penBrush, const BBox &bbox,
                        const Length &size=Length(), const Sides &sides=Sides(Sides::Side::ALL));
void drawRoundedPolygon(PaintDevice *device, const BBox &bbox, const Length &size=Length(),
                        const Sides &sides=Sides(Sides::Side::ALL));
void drawRoundedPolygon(PaintDevice *device, const BBox &bbox, const Length &xsize,
                        const Length &ysize, const Sides &sides=Sides(Sides::Side::ALL));

void drawRoundedPolygon(PaintDevice *device, const PenBrush &penBrush, const Polygon &poly,
                        const Length &size=Length());
void drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &size=Length());
void drawRoundedPolygon(PaintDevice *device, const Polygon &poly, const Length &xsize,
                        const Length &ysize);

void drawTextInBox(PaintDevice *device, const BBox &rect, const QString &text,
                   const TextOptions &options);

void drawStringsInBox(PaintDevice *device, const BBox &rect, const QStringList &strs,
                      const TextOptions &options);

void drawRotatedTextInBox(PaintDevice *device, const BBox &rect, const QString &text,
                          const QPen &pen, const TextOptions &options);

BBox calcTextAtPointRect(PaintDevice *device, const Point &point, const QString &text,
                         const TextOptions &options=TextOptions(), bool centered=false,
                         double dx=0.0, double dy=0.0);

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

void drawSymbol(PaintDevice *device, const PenBrush &penBrush, const Symbol &symbol,
                const Point &c, const Length &size);
void drawSymbol(PaintDevice *device, const Symbol &symbol, const Point &c, const Length &size);
void drawSymbol(PaintDevice *device, const Symbol &symbol, const BBox &bbox);

//---

void drawPieSlice(PaintDevice *device, const Point &c, double ri, double ro, const Angle &a1,
                  const Angle &a2, bool isInvertX=false, bool isInvertY=false);

void pieSlicePath(QPainterPath &path, const Point &c, double ri, double ro, const Angle &a1,
                  const Angle &a2, bool isInvertX, bool isInvertY);

//---

void drawEllipse(PaintDevice *device, const BBox &bbox);

void ellipsePath(QPainterPath &path, const BBox &bbox);

//---

void drawArc(PaintDevice *device, const BBox &bbox, const Angle &angle, const Angle &dangle);

void arcPath(QPainterPath &path, const BBox &bbox, const Angle &angle, const Angle &dangle);

//---

void drawArcSegment(PaintDevice *device, const BBox &ibbox, const BBox &obbox,
                    const Angle &angle, const Angle &dangle);

void arcSegmentPath(QPainterPath &path, const BBox &ibbox, const BBox &obbox,
                    const Angle &angle, const Angle &dangle);

//---

void drawArcsConnector(PaintDevice *device, const BBox &ibbox, const Angle &a1, const Angle &da1,
                       const Angle &a2, const Angle &da2, bool isSelf);

void arcsConnectorPath(QPainterPath &path, const BBox &ibbox, const Angle &a1, const Angle &da1,
                       const Angle &a2, const Angle &da2, bool isSelf);

//---

void edgePath(QPainterPath &path, const BBox &ibbox, const BBox &obbox, bool isLine=false);

//---

QString clipTextToLength(PaintDevice *device, const QString &text, const QFont &font,
                         const Length &clipLength, const Qt::TextElideMode &clipElide);

QString clipTextToLength(const QString &text, const QFont &font, double clipLength,
                         const Qt::TextElideMode &clipElide);

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
                        const TextOptions &options);

void drawHtmlText(PaintDevice *device, const Point &center, const BBox &tbbox,
                  const QString &text, const TextOptions &options,
                  double pdx=0.0, double pdy=0.0);

}

#endif
