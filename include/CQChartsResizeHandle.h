#ifndef CQChartsResizeHandle_H
#define CQChartsResizeHandle_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <CQChartsAlpha.h>

#include <QPainterPath>
#include <QColor>
#include <QPointer>

class CQChartsView;
class CQChartsPlot;
class CQChartsPaintDevice;

/*!
 * \brief edit object resize handle
 * \ingroup Charts
 */
class CQChartsResizeHandle : QObject {
  Q_OBJECT

  Q_PROPERTY(QColor        fillColor   READ fillColor   WRITE setFillColor  )
  Q_PROPERTY(CQChartsAlpha fillAlpha   READ fillAlpha   WRITE setFillAlpha  )
  Q_PROPERTY(QColor        strokeColor READ strokeColor WRITE setStrokeColor)

 public:
  using View        = CQChartsView;
  using Plot        = CQChartsPlot;
  using ResizeSide  = CQChartsResizeSide;
  using Alpha       = CQChartsAlpha;
  using PaintDevice = CQChartsPaintDevice;
  using Point       = CQChartsGeom::Point;
  using BBox        = CQChartsGeom::BBox;

 public:
  CQChartsResizeHandle() = default;

  CQChartsResizeHandle(const View *view, ResizeSide side);
  CQChartsResizeHandle(const Plot *plot, ResizeSide side);

  virtual ~CQChartsResizeHandle() { }

  //---

  View* view() const;
  Plot* plot() const;

  //---

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &v) { bbox_ = v; }

  const ResizeSide &side() const { return side_; }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; }

  const QVariant &data() const { return data_; }
  void setData(const QVariant &v) { data_ = v; }

  //---

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &v) { fillColor_ = v; }

  const Alpha &fillAlpha() const { return fillAlpha_; }
  void setFillAlpha(const Alpha &a) { fillAlpha_ = a; }

  const QColor &strokeColor() const { return strokeColor_; }
  void setStrokeColor(const QColor &v) { strokeColor_ = v; }

  //---

  virtual void draw(PaintDevice *device) const;

  virtual QPainterPath calcPath(PaintDevice *device) const;

  //---

  bool selectInside(const Point &p);

  bool inside(const Point &p) const;

 private:
  Point windowToPixel(const Point &p) const;

 private:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  ViewP                view_;
  PlotP                plot_;
  BBox                 bbox_;
  ResizeSide           side_        { ResizeSide::NONE };
  bool                 selected_    { false };
  QVariant             data_;
  QColor               fillColor_   { "#4444aa" };
  Alpha                fillAlpha_   { 0.8 };
  QColor               strokeColor_ { Qt::black };
  mutable QPainterPath path_;
};

#endif
