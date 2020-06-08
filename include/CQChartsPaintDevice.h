#ifndef CQChartsPaintDevice_H
#define CQChartsPaintDevice_H

#include <CQChartsGeom.h>
#include <CQChartsAngle.h>
#include <CQChartsLength.h>
#include <QPainter>

class CQChartsView;
class CQChartsPlot;
class CQChartsImage;

class CQChartsPaintDevice {
 public:
  enum class Type {
    PIXEL,
    PLOT,
    VIEW,
    SCRIPT,
    SVG
  };

 public:
  using Point   = CQChartsGeom::Point;
  using BBox    = CQChartsGeom::BBox;
  using Size    = CQChartsGeom::Size;
  using Polygon = CQChartsGeom::Polygon;

 public:
  CQChartsPaintDevice(CQChartsView *view) : view_(view) { }
  CQChartsPaintDevice(CQChartsPlot *plot) : plot_(plot) { }
  CQChartsPaintDevice() { }

  const CQChartsView *view() const { return view_; }
  void setView(CQChartsView *p) { view_ = p; }

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p) { plot_ = p; }

  virtual Type type() const = 0;

  virtual bool isInteractive() const { return false; }

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

  virtual void fillPath  (const QPainterPath &, const QBrush &) { }
  virtual void strokePath(const QPainterPath &, const QPen &) { }
  virtual void drawPath  (const QPainterPath &) { }

  virtual void fillRect(const BBox &) { }
  virtual void drawRect(const BBox &) { }

  virtual void drawEllipse(const BBox &, const CQChartsAngle& =CQChartsAngle()) { }

//virtual void drawArc(const BBox &, const CQChartsAngle &, const CQChartsAngle &) { }

  virtual void drawPolygon (const Polygon &) { }
  virtual void drawPolyline(const Polygon &) { }

  virtual void drawLine(const Point &, const Point &) { }

  void drawRoundedLine(const Point &p1, const Point &p2, double w);

  virtual void drawPoint(const Point &) { }

  virtual void drawText(const Point &, const QString &) { }
  virtual void drawTransformedText(const Point &, const QString &) { }

  virtual void drawImage(const Point &, const QImage &) { }
  virtual void drawImageInRect(const BBox &, const CQChartsImage &, bool = true) { }

  virtual const QFont &font() const = 0;
  virtual void setFont(const QFont &f) = 0;

  virtual void setTransformRotate(const Point &p, double angle) = 0;

  virtual const QTransform &transform() const = 0;
  virtual void setTransform(const QTransform &t, bool combine=false) = 0;

  virtual void setRenderHints(QPainter::RenderHints, bool=true) { };

  virtual void setColorNames() { }
  virtual void setColorNames(const QString &, const QString &) { }

  virtual void resetColorNames() { }

  //---

  BBox windowToPixel(const BBox &r) const;
  BBox pixelToWindow(const BBox &r) const;

  Point windowToPixel(const Point &p) const;
  Point pixelToWindow(const Point &p) const;

  Polygon windowToPixel(const Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

  Size pixelToWindowSize(const Size &s) const;

  double lengthPixelWidth (const CQChartsLength &w) const;
  double lengthPixelHeight(const CQChartsLength &h) const;

  double lengthWindowWidth (const CQChartsLength &w) const;
  double lengthWindowHeight(const CQChartsLength &h) const;

  double pixelToWindowWidth (double pw) const;
  double pixelToWindowHeight(double ph) const;

  bool isInvertX() const;
  bool isInvertY() const;

  virtual bool invertY() const { return false; }

 protected:
  CQChartsView* view_ { nullptr };
  CQChartsPlot* plot_ { nullptr };
};

#endif
