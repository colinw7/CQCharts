#ifndef CQChartsPaintDevice_H
#define CQChartsPaintDevice_H

#include <CQChartsGeom.h>
#include <CQChartsAngle.h>
#include <CQChartsLength.h>
#include <CQChartsFillPattern.h>

#include <QPainter>
#include <QPointer>

class CQChartsView;
class CQChartsPlot;
class CQChartsImage;
class CQChartsFont;
class CQCharts;

/*!
 * \brief Abstract Base Class for Painter
 * \ingroup Charts
 */
class CQChartsPaintDevice {
 public:
  enum class Type {
    PIXEL,
    PLOT,
    VIEW,
    SCRIPT,
    SVG,
    STATS
  };

 public:
  using View    = CQChartsView;
  using Plot    = CQChartsPlot;
  using Angle   = CQChartsAngle;
  using Image   = CQChartsImage;
  using Font    = CQChartsFont;
  using Length  = CQChartsLength;
  using Units   = CQChartsUnits::Type;
  using Point   = CQChartsGeom::Point;
  using BBox    = CQChartsGeom::BBox;
  using Size    = CQChartsGeom::Size;
  using Polygon = CQChartsGeom::Polygon;

 public:
  class SaveRestore {
   public:
    SaveRestore(CQChartsPaintDevice *device) :
     device_(device) {
      device_->save();
    }

   ~SaveRestore() {
     device_->restore();
    }

   private:
    CQChartsPaintDevice *device_ { nullptr };
  };

 public:
  CQChartsPaintDevice(View *view);
  CQChartsPaintDevice(Plot *plot);

  CQChartsPaintDevice(CQCharts *charts=nullptr) : charts_(charts) { }

  virtual ~CQChartsPaintDevice() = default;

  //---

  View *view() const;
  void setView(View *view);

  Plot *plot() const;
  void setPlot(Plot *plot);

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) { charts_ = charts; }

  virtual QPainter *painter() const { return nullptr; }

  //---

  CQCharts *calcCharts() const;

  //---

  Units parentUnits() const {
    if      (plot()) return Units::PLOT;
    else if (view()) return Units::VIEW;

    return Units::PIXEL;
  }

  //---

  virtual Type type() const = 0;

  virtual bool isInteractive() const { return false; }

  //---

  bool isZoomFont() const { return zoomFont_; }
  void setZoomFont(bool b) { zoomFont_ = b; }

  //---

  virtual void save() { }
  virtual void restore() { }

  virtual void setClipPath(const QPainterPath &, Qt::ClipOperation=Qt::ReplaceClip) { }
  virtual void setClipRect(const BBox &, Qt::ClipOperation=Qt::ReplaceClip) { }

  virtual BBox clipRect() const { return BBox(); }

  virtual QPen pen() const { return QPen(); }
  virtual void setPen(const QPen &) { }

  virtual QBrush brush() const { return QBrush(); }
  virtual void setBrush(const QBrush &) { }

  virtual void setAltColor(const QColor &) { }

  virtual void setAltAlpha(double) { }

  virtual void setFillAngle(double) { }
  virtual void setFillType(CQChartsFillPattern::Type) { }
  virtual void setFillRadius(double) { }
  virtual void setFillDelta(double) { }
  virtual void setFillWidth(double) { }

  virtual void fillPath  (const QPainterPath &, const QBrush &) { assert(false); }
  virtual void strokePath(const QPainterPath &, const QPen &) { assert(false); }
  virtual void drawPath  (const QPainterPath &) { assert(false); }

  virtual void fillRect(const BBox &) { assert(false); }
  virtual void drawRect(const BBox &) { assert(false); }

  virtual void drawEllipse(const BBox &, const Angle& =Angle()) { assert(false); }

//virtual void drawArc(const BBox &, const Angle &, const Angle &) { assert(false); }

  //---

  void drawDiamond(const BBox &bbox);

  //---

  virtual void drawPolygon (const Polygon &) { assert(false); }
  virtual void drawPolyline(const Polygon &) { assert(false); }

  virtual void drawLine(const Point &, const Point &) { assert(false); }

  virtual void drawPoint(const Point &) { assert(false); }

  virtual void drawText(const Point &, const QString &) { assert(false); }
  virtual void drawTransformedText(const Point &, const QString &) { assert(false); }

  virtual void drawImage(const Point &, const Image &) { assert(false); }
  virtual void drawImageInRect(const BBox &, const Image &, bool = true,
                               const Angle & = Angle()) { assert(false); }

  virtual const QFont &font() const = 0;
  virtual void setFont(const QFont &f, bool scale=true) = 0;

  virtual void setTransformRotate(const Point &p, double angle) = 0;

  virtual const QTransform &transform() const = 0;
  virtual void setTransform(const QTransform &t, bool combine=false) = 0;

  virtual void setRenderHints(QPainter::RenderHints, bool=true) { }

  virtual void setColorNames() { }
  virtual void setColorNames(const QString &, const QString &) { }

  virtual void resetColorNames() { }

  //---

  //! group data
  struct GroupData {
    bool    visible   { true };
    bool    onclick   { false };
    QString clickProc { "clickProc" };
    bool    hasTip    { false };
    QString tipStr;

    GroupData() { } // default not allowed ?
  };

  virtual void startGroup(const QString &, const GroupData& =GroupData()) { }
  virtual void endGroup() { }

  //---

  BBox windowToPixel(const BBox &r) const;
  BBox pixelToWindow(const BBox &r) const;

  Point windowToPixel(const Point &p) const;
  Point pixelToWindow(const Point &p) const;

  Polygon windowToPixel(const Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

  Size pixelToWindowSize(const Size &s) const;

  double lengthPixelWidth (const Length &w) const;
  double lengthPixelHeight(const Length &h) const;

  double lengthWindowWidth (const Length &w) const;
  double lengthWindowHeight(const Length &h) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  double pixelToSignedWindowWidth (double pw) const;
  double pixelToSignedWindowHeight(double ph) const;

  double windowToPixelWidth (double ww) const;
  double windowToPixelHeight(double wh) const;

  double windowToSignedPixelWidth (double ww) const;
  double windowToSignedPixelHeight(double wh) const;

  bool isInvertX() const;
  bool isInvertY() const;

  virtual bool invertY() const { return false; }

  //---

  virtual void setPainterFont(const Font &) { }

  //---

  bool isNull() const { return isNull_; }
  void setNull(bool b) { isNull_ = b; }

  //---

  const QColor &contrastColor() const { return contrastColor_; }
  void setContrastColor(const QColor &c) { contrastColor_ = c; }
  void resetContrastColor() { contrastColor_ = QColor(); }

 protected:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  CQCharts* charts_   { nullptr };
  ViewP     view_;
  PlotP     plot_;
  bool      zoomFont_ { false };
  bool      isNull_   { false };
  QColor    contrastColor_;
};

#endif
