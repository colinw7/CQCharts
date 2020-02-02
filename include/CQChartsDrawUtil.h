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
};

//---

class CQChartsPenData {
 public:
  CQChartsPenData() = default;

  explicit CQChartsPenData(bool visible, const QColor &color=QColor(),
                           const CQChartsAlpha &alpha=CQChartsAlpha(),
                           const CQChartsLength &width=CQChartsLength(),
                           const CQChartsLineDash &dash=CQChartsLineDash()) :
   visible_(visible), color_(color), alpha_(alpha), width_(width), dash_(dash) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const CQChartsAlpha &alpha() const { return alpha_; }
  void setAlpha(const CQChartsAlpha &a) { alpha_ = a; }

  const CQChartsLength &width() const { return width_; }
  void setWidth(const CQChartsLength &v) { width_ = v; }

  const CQChartsLineDash &dash() const { return dash_; }
  void setDash(const CQChartsLineDash &v) { dash_ = v; }

 private:
  bool             visible_ { true };  //!< visible
  QColor           color_;             //!< pen color
  CQChartsAlpha    alpha_;             //!< pen alpha
  CQChartsLength   width_   { "0px" }; //!< pen width
  CQChartsLineDash dash_    { };       //!< pen dash
};

//---

class CQChartsBrushData {
 public:
  CQChartsBrushData() = default;

  explicit CQChartsBrushData(bool visible, const QColor &color=QColor(),
                             const CQChartsAlpha &alpha=CQChartsAlpha(),
                             const CQChartsFillPattern &pattern=CQChartsFillPattern()) :
   visible_(visible), color_(color), alpha_(alpha), pattern_(pattern) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const CQChartsAlpha &alpha() const { return alpha_; }
  void setAlpha(const CQChartsAlpha &a) { alpha_ = a; }

  const CQChartsFillPattern &pattern() const { return pattern_; }
  void setPattern(const CQChartsFillPattern &v) { pattern_ = v; }

  //---

 private:
  bool                visible_ { true };                             //!< visible
  QColor              color_;                                        //!< fill color
  CQChartsAlpha       alpha_;                                        //!< fill alpha
  CQChartsFillPattern pattern_ { CQChartsFillPattern::Type::SOLID }; //!< fill pattern
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
