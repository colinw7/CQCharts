#ifndef CQChartsViewPlotPaintDevice_H
#define CQChartsViewPlotPaintDevice_H

#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>

class CQHandDrawnPainter;

class CQChartsViewPlotPaintDevice : public CQChartsPaintDevice {
 public:
  CQChartsViewPlotPaintDevice(CQChartsView *view, QPainter *painter);
  CQChartsViewPlotPaintDevice(CQChartsPlot *plot, QPainter *painter);
  CQChartsViewPlotPaintDevice(QPainter *painter);

 ~CQChartsViewPlotPaintDevice();

  bool isInteractive() const override { return true; }

  bool isHandDrawn() const { return handDrawn_; }
  void setHandDrawn(bool b);

  double handRoughness() const { return handRoughness_; }
  void setHandRoughness(double r);

  double handFillDelta() const { return handFillDelta_; }
  void setHandFillDelta(double r);

  QPainter *painter() const { return painter_; }

  void save   () override;
  void restore() override;

  void setClipPath(const QPainterPath &path, Qt::ClipOperation operation) override;
  void setClipRect(const BBox &bbox, Qt::ClipOperation operation) override;

  BBox clipRect() const override;

  QPen pen() const override;
  void setPen(const QPen &pen) override;

  QBrush brush() const override;
  void setBrush(const QBrush &brush) override;

  void setAltColor(const QColor &c) override;

  void fillPath  (const QPainterPath &path, const QBrush &brush) override;
  void strokePath(const QPainterPath &path, const QPen &pen) override;
  void drawPath  (const QPainterPath &path) override;

  void fillRect(const BBox &bbox) override;
  void drawRect(const BBox &bbox) override;

  void drawEllipse(const BBox &bbox, const CQChartsAngle &a=CQChartsAngle()) override;

//void drawArc(const BBox &rect, const CQChartsAngle &a1, CQChartsAngle &a2) override;

  void drawPolygon (const Polygon &poly) override;
  void drawPolyline(const Polygon &poly) override;

  void drawLine(const Point &p1, const Point &p2) override;

  void drawPoint(const Point &p) override;

  void drawText(const Point &p, const QString &text) override;
  void drawTransformedText(const Point &p, const QString &text) override;

  void drawImage(const Point &, const QImage &) override;
  void drawImageInRect(const BBox &bbox, const CQChartsImage &image, bool stretch=true) override;

  const QFont &font() const override;
  void setFont(const QFont &f) override;

  void setTransformRotate(const Point &p, double angle) override;

  const QTransform &transform() const override;
  void setTransform(const QTransform &t, bool combine=false) override;

  void setRenderHints(QPainter::RenderHints hints, bool on) override;

 private:
  QPainter*           painter_       { nullptr };
  BBox                clipRect_;
  QPainterPath        clipPath_;
  bool                handDrawn_     { false };
  double              handRoughness_ { 1.0 };
  double              handFillDelta_ { 16.0 };
  CQHandDrawnPainter* hdPainter_     { nullptr };
};

//---

class CQChartsPixelPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  CQChartsPixelPaintDevice(QPainter *painter);

  Type type() const override { return Type::PIXEL; }

  bool invertY() const override { return true; }
};

//---

class CQChartsViewPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  CQChartsViewPaintDevice(CQChartsView *view, QPainter *painter);

  Type type() const override { return Type::VIEW; }
};

//---

class CQChartsPlotPaintDevice : public CQChartsViewPlotPaintDevice {
 public:
  CQChartsPlotPaintDevice(CQChartsPlot *plot, QPainter *painter);

  Type type() const override { return Type::PLOT; }
};

#endif
