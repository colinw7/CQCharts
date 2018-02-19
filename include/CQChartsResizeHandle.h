#ifndef CQChartsResizeHandle_H
#define CQChartsResizeHandle_H

#include <CQChartsGeom.h>
#include <QPainterPath>
#include <QColor>
#include <QPointF>

class CQChartsView;
class CQChartsPlot;
class QPainter;

class CQChartsResizeHandle : QObject {
  Q_OBJECT

  Q_PROPERTY(QColor fillColor   READ fillColor   WRITE setFillColor  )
  Q_PROPERTY(double fillAlpha   READ fillAlpha   WRITE setFillAlpha  )
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)

 public:
  enum class Side {
    NONE,
    MOVE,
    LL,
    LR,
    UL,
    UR
  };

 public:
  CQChartsResizeHandle() = default;

  CQChartsResizeHandle(CQChartsView *view, Side side);
  CQChartsResizeHandle(CQChartsPlot *plot, Side side);

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &v) { bbox_ = v; }

  const Side &side() const { return side_; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &v) { fillColor_ = v; }

  double fillAlpha() const { return fillAlpha_; }
  void setFillAlpha(double r) { fillAlpha_ = r; }

  const QColor &borderColor() const { return borderColor_; }
  void setBorderColor(const QColor &v) { borderColor_ = v; }

  void draw(QPainter *painter);

  bool selectInside(const CQChartsGeom::Point &p);

  bool inside(const CQChartsGeom::Point &p) const;

 private:
  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &p) const;

 private:
  CQChartsView*      view_        { nullptr };
  CQChartsPlot*      plot_        { nullptr };
  CQChartsGeom::BBox bbox_;
  Side               side_        { Side::NONE };
  bool               selected_    { false };
  QColor             fillColor_   { "#4444aa" };
  double             fillAlpha_   { 0.5 };
  QColor             borderColor_ { Qt::black };
  QPointF            pos_         { 0, 0 };
  QPainterPath       path_;
};

#endif
