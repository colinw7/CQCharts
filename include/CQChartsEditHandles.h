#ifndef CQChartsEditHandles_H
#define CQChartsEditHandles_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>

#include <QObject>
#include <QPointer>

class CQChartsView;
class CQChartsPlot;
class CQChartsResizeHandle;
class CQChartsPaintDevice;

/*!
 * \brief object edit handles
 * \ingroup Charts
 */
class CQChartsEditHandles : public QObject {
  Q_OBJECT

 public:
  enum class Mode {
    MOVE,
    RESIZE
  };

 public:
  using View         = CQChartsView;
  using Plot         = CQChartsPlot;
  using BBox         = CQChartsGeom::BBox;
  using Point        = CQChartsGeom::Point;
  using Handle       = CQChartsResizeHandle;
  using PaintDevice  = CQChartsPaintDevice;
  using ExtraHandles = std::vector<Handle *>;
  using ResizeSide   = CQChartsResizeSide;

  struct InsideData {
    ResizeSide resizeSide { ResizeSide::NONE };
    QVariant   data;
  };

 public:
  CQChartsEditHandles(const View *view, const Mode &mode=Mode::RESIZE);
  CQChartsEditHandles(const Plot *plot, const Mode &mode=Mode::RESIZE);

 ~CQChartsEditHandles();

  //---

  View *view() const;
  Plot *plot() const;

  //---

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &v) { bbox_ = v; }

  //---

  // move handle
  const Handle *moveHandle() const { return moveHandle_; }

  // corner handles
  const Handle *llHandle() const { return cornerHandle(ResizeSide::LL); }
  const Handle *lrHandle() const { return cornerHandle(ResizeSide::LR); }
  const Handle *ulHandle() const { return cornerHandle(ResizeSide::UL); }
  const Handle *urHandle() const { return cornerHandle(ResizeSide::UR); }

  const Handle *cornerHandle(const ResizeSide &side) const {
    auto p = cornerHandles_.find(side);
    assert(p != cornerHandles_.end());
    return (*p).second;
  }

  // extra handles
  const ExtraHandles &extraHandles() const { return extraHandles_; }

  void addExtraHandle(Handle *handle);
  void removeExtraHandle();

  void removeExtraHandles();

  //---

  void setDragData(const InsideData &insideData) {
    dragSide_ = insideData.resizeSide;
    dragData_ = insideData.data;
  }

  const Point &dragPos() const { return dragPos_; }
  void setDragPos(const Point &p) { dragPos_ = p; }

  const ResizeSide &dragSide() const { return dragSide_; }
  void setDragSide(const ResizeSide &s) { dragSide_ = s; }

  const QVariant &dragData() const { return dragData_; }
  void setDragData(const QVariant &v) { dragData_ = v; }

  //---

  bool selectInside(const Point &p);

  bool inside(const Point &p, InsideData &insideData) const;

  void updateBBox(double dx, double dy);

  //---

  void draw(PaintDevice *device) const;

 Q_SIGNALS:
  void extraHandleMoved(const QVariant &data, double dx, double dy);

 private:
  void init();

 private:
  using ViewP         = QPointer<View>;
  using PlotP         = QPointer<Plot>;
  using CornerHandles = std::map<ResizeSide, Handle *>;

  ViewP         view_;                            //!< parent view
  PlotP         plot_;                            //!< parent plot
  Mode          mode_       { Mode::RESIZE };     //!< handle mode
  BBox          bbox_       { 0, 0, 1, 1 };       //!< handle bbox
  Handle*       moveHandle_ { nullptr };          //!< move handle
  CornerHandles cornerHandles_;                   //!< corner resize handles
  ExtraHandles  extraHandles_;                    //!< extra (custom) handles
  Point         dragPos_    { 0, 0 };             //!< drag position
  ResizeSide    dragSide_   { ResizeSide::NONE }; //!< drag side
  QVariant      dragData_;                        //!< drag extra data
};

#endif
