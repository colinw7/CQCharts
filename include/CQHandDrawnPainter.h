#ifndef CQHandDrawnPainter_H
#define CQHandDrawnPainter_H

#include <QPolygonF>
#include <QPainterPath>
#include <QColor>

class QPainter;

class CQHandDrawnPainter {
 public:
  enum class FillType {
    NONE,
    FILL,
    HATCH,
    CROSS_HATCH,
    ZIG_ZAG,
    DOTS
  };

  struct FillData {
    double   delta    { 1.0 };
    FillType fillType { FillType::HATCH };
    double   angle    { 0.0 };
    bool     connect  { false };
    bool     dots     { false };
  };

 public:
  CQHandDrawnPainter(QPainter *painter=nullptr);

  //---

  const QPainter *painter() const { return painter_; }
  void setPainter(QPainter *p) { painter_ = p; }

  //---

  double roughness() const { return roughness_; }
  void setRoughness(double r) { roughness_ = r; }

  double fillDelta() const { return fillDelta_; }
  void setFillDelta(double r) { fillDelta_ = r; }

  const QColor &altColor() const { return altColor_; }
  void setAltColor(const QColor &c) { altColor_ = c; }

  double fillAngle() const { return fillAngle_; }
  void setFillAngle(double r) { fillAngle_ = r; }

  //--

  void setSize(int width, int height);

  //---

  void drawRects();
  void drawCircles();

  //---

  void fillPath  (const QPainterPath &path, const QBrush &brush);
  void strokePath(const QPainterPath &path, const QPen &pen);
  void drawPath  (const QPainterPath &path);

  void fillRect  (const QRectF &rect, const QBrush &brush);
  void strokeRect(const QRectF &rect);
  void drawRect  (const QRectF &rect);

  void drawCircle(const QPointF &c, double r);
  void fillCircle(const QPointF &c, double r);

  void drawLine(const QPointF &p1, const QPointF &p2);

  void drawPoint(const QPointF &p);

  void fillPolygon(const QPolygonF &poly);
  void drawPolygon(const QPolygonF &poly);

  void drawPolyline(const QPolygonF &poly);

  void drawText(const QPointF &p, const QString &text);

  void drawImage(const QPointF &p, const QImage &image);
  void drawImage(const QRectF &r, const QImage &image);

 private:
  QPainterPath rectPath(const QRectF &rect) const;

  void fillPatternPath(const QPainterPath &path);

  QPainterPath randomizePath(const QPainterPath &path) const;

  void fillDataPolygon(const QPolygonF &poly, const FillData &fillData);

  void hatchFillPolygon(const QPolygonF &poly, const FillData &fillData);

  void drawCircle1(const QPointF &c, double r);
  void fillCircle1(const QPointF &c, double r);

  QPainterPath circlePath(const QPointF &c, double r) const;

  void drawLine1(const QPointF &p1, const QPointF &p2);

  void drawDebugPoint(const QPointF &p);

  QPointF randomize(const QPointF &p1) const;

 private:
  QPainter*      painter_   { nullptr };
  double         roughness_ { 2.7 };
  double         fillDelta_ { 16 };
  mutable QColor altColor_;
  mutable double fillAngle_ { 45.0 };
  mutable double dampen_    { 1.0 };
  mutable int    width_     { 100 };
  mutable int    height_    { 100 };
  mutable int    maxSize_   { 100 };
};

#endif
