#ifndef CQChartsEditHandles_H
#define CQChartsEditHandles_H

#include <CQChartsResizeHandle.h>
#include <QRectF>

class CQChartsEditHandles : public QObject {
  Q_OBJECT

 public:
  enum class Mode {
    MOVE,
    RESIZE
  };

 public:
  CQChartsEditHandles(CQChartsView *view, const Mode &mode=Mode::RESIZE);
  CQChartsEditHandles(CQChartsPlot *plot, const Mode &mode=Mode::RESIZE);

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &v) { bbox_ = v; }

  const CQChartsResizeHandle &moveHandle() const { return moveHandle_; }

  const CQChartsResizeHandle &llHandle() const { return llHandle_; }
  const CQChartsResizeHandle &lrHandle() const { return lrHandle_; }
  const CQChartsResizeHandle &ulHandle() const { return ulHandle_; }
  const CQChartsResizeHandle &urHandle() const { return urHandle_; }

  const CQChartsGeom::Point &dragPos() const { return dragPos_; }
  void setDragPos(const CQChartsGeom::Point &v) { dragPos_ = v; }

  const CQChartsResizeHandle::Side &dragSide() const { return dragSide_; }
  void setDragSide(const CQChartsResizeHandle::Side &v) { dragSide_ = v; }

  bool selectInside(const CQChartsGeom::Point &p);

  CQChartsResizeHandle::Side inside(const CQChartsGeom::Point &p) const;

  void updateBBox(double dx, double dy);

  void draw(QPainter *painter);

 private:
  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &p) const;

 private:
  CQChartsView*              view_       { nullptr };
  CQChartsPlot*              plot_       { nullptr };
  Mode                       mode_       { Mode::RESIZE };
  CQChartsGeom::BBox         bbox_       { 0, 0, 1, 1 };
  CQChartsResizeHandle       moveHandle_;
  CQChartsResizeHandle       llHandle_;
  CQChartsResizeHandle       lrHandle_;
  CQChartsResizeHandle       ulHandle_;
  CQChartsResizeHandle       urHandle_;
  CQChartsGeom::Point        dragPos_    { 0, 0 };
  CQChartsResizeHandle::Side dragSide_   { CQChartsResizeHandle::Side::NONE };
};

#endif
