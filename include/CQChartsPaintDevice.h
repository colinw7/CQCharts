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
  virtual void setClipRect(const CQChartsGeom::BBox &, Qt::ClipOperation=Qt::ReplaceClip) { }

  virtual CQChartsGeom::BBox clipRect() const { return CQChartsGeom::BBox(); }

  virtual QPen pen() const { return QPen(); }
  virtual void setPen(const QPen &) { }

  virtual QBrush brush() const { return QBrush(); }
  virtual void setBrush(const QBrush &) { }

  virtual void fillPath  (const QPainterPath &, const QBrush &) { }
  virtual void strokePath(const QPainterPath &, const QPen &) { }
  virtual void drawPath  (const QPainterPath &) { }

  virtual void fillRect(const CQChartsGeom::BBox &, const QBrush &) { }
  virtual void drawRect(const CQChartsGeom::BBox &) { }

  virtual void drawEllipse(const CQChartsGeom::BBox &, const CQChartsAngle& =CQChartsAngle()) { }

//virtual void drawArc(const CQChartsGeom::BBox &, const CQChartsAngle &,
//                     const CQChartsAngle &) { }

  virtual void drawPolygon (const CQChartsGeom::Polygon &) { }
  virtual void drawPolyline(const CQChartsGeom::Polygon &) { }

  virtual void drawLine(const CQChartsGeom::Point &, const CQChartsGeom::Point &) { }

  virtual void drawPoint(const CQChartsGeom::Point &) { }

  virtual void drawText(const CQChartsGeom::Point &, const QString &) { }
  virtual void drawTransformedText(const CQChartsGeom::Point &, const QString &) { }

  virtual void drawImage(const CQChartsGeom::Point &, const QImage &) { }
  virtual void drawImageInRect(const CQChartsGeom::BBox &, const CQChartsImage &, bool = true) { }

  virtual const QFont &font() const = 0;
  virtual void setFont(const QFont &f) = 0;

  virtual void setTransformRotate(const CQChartsGeom::Point &p, double angle) = 0;

  virtual const QTransform &transform() const = 0;
  virtual void setTransform(const QTransform &t, bool combine=false) = 0;

  virtual void setRenderHints(QPainter::RenderHints, bool=true) { };

  virtual void setColorNames() { }
  virtual void setColorNames(const QString &, const QString &) { }

  virtual void resetColorNames() { }

  //---

  CQChartsGeom::BBox windowToPixel(const CQChartsGeom::BBox &r) const;
  CQChartsGeom::BBox pixelToWindow(const CQChartsGeom::BBox &r) const;

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &p) const;
  CQChartsGeom::Point pixelToWindow(const CQChartsGeom::Point &p) const;

  CQChartsGeom::Polygon windowToPixel(const CQChartsGeom::Polygon &p) const;

  QPainterPath windowToPixel(const QPainterPath &p) const;

  CQChartsGeom::Size pixelToWindowSize(const CQChartsGeom::Size &s) const;

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
