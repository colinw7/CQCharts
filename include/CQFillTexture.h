#ifndef CQFillTexture_H
#define CQFillTexture_H

#include <QObject>
#include <QColor>

#include <cmath>

class QPainter;
class QPainterPath;
class QPolygonF;
class QPointF;

class CQFillTexture : public QObject {
 public:
  enum class FillType {
    NONE,
    FILL,
    HATCH,
    CROSS_HATCH,
    ZIG_ZAG,
    DOTS,
    HEXAGONS,
    CROSSES,
    CAPS,
    WOVEN,
    WAVES,
    NYLON,
    SQUARES
  };

  enum class ShapeType {
    NONE     = int(FillType::NONE),
    DOTS     = int(FillType::DOTS),
    HEXAGONS = int(FillType::HEXAGONS),
    CROSSES  = int(FillType::CROSSES),
    CAPS     = int(FillType::CAPS),
    WOVEN    = int(FillType::WOVEN),
    WAVES    = int(FillType::WAVES),
    NYLON    = int(FillType::NYLON),
    SQUARES  = int(FillType::SQUARES)
  };

 public:
  CQFillTexture();

  //---

  const FillType &fillType() const { return fillData_.fillType; }
  void setFillType(const FillType &t) { fillData_.fillType = t; }

  double width() const { return fillData_.width; }
  void setWidth(double r) { fillData_.width = r; }

  double delta() const { return fillData_.delta; }
  void setDelta(double r) { fillData_.delta = r; }

  double angle() const { return fillData_.angle; }
  void setAngle(double r) { fillData_.angle = r; }

  double radius() const { return fillData_.radius; }
  void setRadius(double r) { fillData_.radius = r; }

  const QColor &bgColor() const { return fillData_.bgColor; }
  void setBgColor(const QColor &c) { fillData_.bgColor = c; }

  const QColor &fgColor() const { return fillData_.fgColor; }
  void setFgColor(const QColor &c) { fillData_.fgColor = c; }

  const QColor &altColor() const { return fillData_.altColor; }
  void setAltColor(const QColor &c) { fillData_.altColor = c; }

  bool isFilled() const { return fillData_.filled; }
  void setFilled(bool b) { fillData_.filled = b; }

  bool isStroked() const { return fillData_.stroked; }
  void setStroked(bool b) { fillData_.stroked = b; }

  bool isBgFilled() const { return fillData_.bgFilled; }
  void setBgFilled(bool b) { fillData_.bgFilled = b; }

  //---

  void fillPath(QPainter *painter, const QPainterPath &path);
  void fillRect(QPainter *painter, const QRectF &rect);
  void fillPolygon(QPainter *painter, const QPolygonF &poly);

  //---

  QString toString() const;

 private:
  struct FillData {
    FillType  fillType  { FillType::FILL };
    double    width     { 4.0 };
    double    delta     { 0.0 };
    double    angle     { 0.0 };
    double    radius    { 16 };
    bool      filled    { true };
    bool      stroked   { true };
    bool      bgFilled  { true };
    bool      connect   { false };
    ShapeType shapeType { ShapeType::NONE };
    QColor    bgColor   { Qt::white };
    QColor    fgColor   { Qt::black };
    QColor    altColor;
    int       lineNum   { 0 };
    int       cellNum   { 0 };
  };

 private:
  void hatchFillPolygon(QPainter *painter, const QPolygonF &poly, const FillData &fillData);

  void fillHexagons(QPainter *painter, const QPolygonF &poly, const FillData &fillData);

  void drawLine(QPainter *painter, const QPointF &p1, const QPointF &p2, double width);

  void fillHexagon(QPainter *painter, const QPointF &center, double r, const FillData &fillData);

  void drawWoven(QPainter *painter, const QPointF &center, double r, const FillData &fillData);
  void drawNylon(QPainter *painter, const QPointF &center, double r, const FillData &fillData);

  void moveFillShapePath(QPainter *painter, const QPainterPath &path,
                         const QPointF &center, const FillData &fillData);
  void moveDrawShapePath(QPainter *painter, const QPainterPath &path,
                         const QPointF &center, const FillData &fillData);
  void drawShapePath(QPainter *painter, const QPainterPath &path, double width);

  void circlePath(double r, QPainterPath &path) const;
  void crossPath (double r, double angle, QPainterPath &path) const;
  void capPath   (double r, double angle, QPainterPath &path) const;
  void wavePath  (double r, double angle, QPainterPath &path) const;
  void nylonPath (double r, double angle, QPainterPath &path) const;
  void wovenPath (double r, double angle, QPainterPath &path) const;
  void squarePath(double r, double angle, QPainterPath &path) const;

  QPainterPath movePath  (const QPainterPath &path, const QPointF &c) const;
  QPainterPath rotatePath(const QPainterPath &path, const QPointF &c, double angle) const;

 private:
  FillData fillData_;
};

#endif
