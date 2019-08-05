#ifndef CQChartsResizeHandle_H
#define CQChartsResizeHandle_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <QPainterPath>
#include <QColor>
#include <QPointF>

class CQChartsView;
class CQChartsPlot;
class QPainter;

/*!
 * \brief edit object resize handle
 * \ingroup Charts
 */
class CQChartsResizeHandle : QObject {
  Q_OBJECT

  Q_PROPERTY(QColor fillColor   READ fillColor   WRITE setFillColor  )
  Q_PROPERTY(double fillAlpha   READ fillAlpha   WRITE setFillAlpha  )
  Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor)

 public:
  CQChartsResizeHandle() = default;

  CQChartsResizeHandle(const CQChartsView *view, CQChartsResizeSide side);
  CQChartsResizeHandle(const CQChartsPlot *plot, CQChartsResizeSide side);

  const CQChartsView* view() const { return view_; }
  const CQChartsPlot* plot() const { return plot_; }

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &v) { bbox_ = v; }

  const CQChartsResizeSide &side() const { return side_; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &v) { fillColor_ = v; }

  double fillAlpha() const { return fillAlpha_; }
  void setFillAlpha(double r) { fillAlpha_ = r; }

  const QColor &strokeColor() const { return strokeColor_; }
  void setStrokeColor(const QColor &v) { strokeColor_ = v; }

  void draw(QPainter *painter) const;

  bool selectInside(const CQChartsGeom::Point &p);

  bool inside(const CQChartsGeom::Point &p) const;

 private:
  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &p) const;

 private:
  const CQChartsView*  view_        { nullptr };
  const CQChartsPlot*  plot_        { nullptr };
  CQChartsGeom::BBox   bbox_;
  CQChartsResizeSide   side_        { CQChartsResizeSide::NONE };
  bool                 selected_    { false };
  QColor               fillColor_   { "#4444aa" };
  double               fillAlpha_   { 0.5 };
  QColor               strokeColor_ { Qt::black };
  QPointF              pos_         { 0, 0 };
  mutable QPainterPath path_;
};

#endif
