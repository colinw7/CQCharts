#ifndef CQChartsDrawUtil_H
#define CQChartsDrawUtil_H

#include <CQChartsTextOptions.h>
#include <CQChartsSymbol.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>
#include <CQChartsFillPattern.h>
#include <CQChartsSides.h>
#include <CQChartsAlpha.h>
#include <CQChartsGeom.h>

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QString>

class CQChartsPaintDevice;

struct CQChartsPenBrush {
  QPen   pen;
  QBrush brush;
  QColor altColor;

  CQChartsPenBrush() = default;

  CQChartsPenBrush(const QPen &pen, const QBrush &brush) :
   pen(pen), brush(brush) {
  }
};

//---

class CQChartsPenData {
 public:
  using Alpha    = CQChartsAlpha;
  using Length   = CQChartsLength;
  using LineDash = CQChartsLineDash;

 public:
  CQChartsPenData() = default;

  explicit CQChartsPenData(bool visible, const QColor &color=QColor(), const Alpha &alpha=Alpha(),
                           const Length &width=Length(), const LineDash &dash=LineDash()) :
   visible_(visible), color_(color), alpha_(alpha), width_(width), dash_(dash) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  const Length &width() const { return width_; }
  void setWidth(const Length &v) { width_ = v; }

  const LineDash &dash() const { return dash_; }
  void setDash(const LineDash &v) { dash_ = v; }

 private:
  bool     visible_ { true };  //!< visible
  QColor   color_;             //!< pen color
  Alpha    alpha_;             //!< pen alpha
  Length   width_   { "0px" }; //!< pen width
  LineDash dash_    { };       //!< pen dash
};

//---

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

  explicit CQChartsBrushData(bool visible, const QColor &color, const QColor &altColor,
                             const Alpha &alpha=Alpha(), const FillPattern &pattern=FillPattern()) :
   visible_(visible), color_(color), altColor_(altColor), alpha_(alpha), pattern_(pattern) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const QColor &altColor() const { return altColor_; }
  void setAltColor(const QColor &v) { altColor_ = v; }

  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  const FillPattern &pattern() const { return pattern_; }
  void setPattern(const FillPattern &v) { pattern_ = v; }

  //---

 private:
  bool        visible_ { true };                             //!< visible
  QColor      color_;                                        //!< fill color
  QColor      altColor_;                                     //!< alt fill color
  Alpha       alpha_;                                        //!< fill alpha
  FillPattern pattern_ { CQChartsFillPattern::Type::SOLID }; //!< fill pattern
};

//---

namespace CQChartsDrawUtil {

inline QPainterPath polygonToPath(const CQChartsGeom::Polygon &polygon, bool closed=false) {
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

namespace CQChartsDrawUtil {

void setPenBrush(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush);
void setPenBrush(QPainter *device, const CQChartsPenBrush &penBrush);

void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                        const CQChartsLength &size=CQChartsLength(),
                        const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));
void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
                        const CQChartsLength &xsize, const CQChartsLength &ysize,
                        const CQChartsSides &sides=CQChartsSides(CQChartsSides::Side::ALL));

void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly,
                        const CQChartsLength &size=CQChartsLength());
void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsGeom::Polygon &poly,
                        const CQChartsLength &xsize, const CQChartsLength &ysize);

void drawTextInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                   const QString &text, const CQChartsTextOptions &options);

void drawStringsInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                       const QStringList &strs, const CQChartsTextOptions &options);

void drawRotatedTextInBox(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect,
                          const QString &text, const QPen &pen, const CQChartsTextOptions &options);

void drawTextAtPoint(CQChartsPaintDevice *device, const CQChartsGeom::Point &p, const QString &text,
                     const CQChartsTextOptions &options=CQChartsTextOptions(),
                     bool centered=false, double dx=0.0, double dy=0.0);

void drawAlignedText(CQChartsPaintDevice *device, const CQChartsGeom::Point &p,
                     const QString &text, Qt::Alignment align, double dx=0.0, double dy=0.0);

CQChartsGeom::BBox calcAlignedTextRect(CQChartsPaintDevice *device, const QFont &font,
                                       const CQChartsGeom::Point &p, const QString &text,
                                       Qt::Alignment align, double dx, double dy);

void drawContrastText(CQChartsPaintDevice *device, const CQChartsGeom::Point &p,
                      const QString &text, const CQChartsAlpha &alpha);

CQChartsGeom::Size calcTextSize(const QString &text, const QFont &font,
                                const CQChartsTextOptions &options);

void drawCenteredText(CQChartsPaintDevice *device, const CQChartsGeom::Point &pos,
                      const QString &text);

void drawSimpleText(CQChartsPaintDevice *device, const CQChartsGeom::Point &pos,
                    const QString &text);

void drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
                const CQChartsGeom::Point &c, const CQChartsLength &size);
void drawSymbol(CQChartsPaintDevice *device, const CQChartsSymbol &symbol,
                const CQChartsGeom::BBox &bbox);

void drawPieSlice(CQChartsPaintDevice *device, const CQChartsGeom::Point &c,
                  double ri, double ro, const CQChartsAngle &a1, const CQChartsAngle &a2,
                  bool isInvertX=false, bool isInvertY=false);

void drawArc(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox,
             const CQChartsAngle &angle, const CQChartsAngle &dangle);

void drawArcSegment(CQChartsPaintDevice *device, const CQChartsGeom::BBox &ibbox,
                    const CQChartsGeom::BBox &obbox, const CQChartsAngle &angle,
                    const CQChartsAngle &dangle);

void drawArcsConnector(CQChartsPaintDevice *device, const CQChartsGeom::BBox &ibbox,
                       const CQChartsAngle &a1, const CQChartsAngle &da1,
                       const CQChartsAngle &a2, const CQChartsAngle &da2, bool isSelf);

void arcsConnectorPath(QPainterPath &path, const CQChartsGeom::BBox &ibbox,
                       const CQChartsAngle &a1, const CQChartsAngle &da1,
                       const CQChartsAngle &a2, const CQChartsAngle &da2, bool isSelf);

}

//---

namespace CQChartsDrawPrivate {

// private
CQChartsGeom::Size calcHtmlTextSize(const QString &text, const QFont &font, int margin=0);

void drawScaledHtmlText(CQChartsPaintDevice *device, const CQChartsGeom::BBox &tbbox,
                        const QString &text, const CQChartsTextOptions &options);

void drawHtmlText(CQChartsPaintDevice *device, const CQChartsGeom::BBox &tbbox,
                  const QString &text, const CQChartsTextOptions &options);

}

#endif
