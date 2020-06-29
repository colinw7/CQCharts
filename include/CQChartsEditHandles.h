#ifndef CQChartsEditHandles_H
#define CQChartsEditHandles_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <QObject>

class CQChartsView;
class CQChartsPlot;
class CQChartsResizeHandle;

class QPainter;

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
  using BBox   = CQChartsGeom::BBox;
  using Point  = CQChartsGeom::Point;
  using Handle = CQChartsResizeHandle;

 public:
  CQChartsEditHandles(const CQChartsView *view, const Mode &mode=Mode::RESIZE);
  CQChartsEditHandles(const CQChartsPlot *plot, const Mode &mode=Mode::RESIZE);

 ~CQChartsEditHandles();

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &v) { bbox_ = v; }

  const Handle *moveHandle() const { return moveHandle_; }

  const Handle *llHandle() const { return llHandle_; }
  const Handle *lrHandle() const { return lrHandle_; }
  const Handle *ulHandle() const { return ulHandle_; }
  const Handle *urHandle() const { return urHandle_; }

  const Point &dragPos() const { return dragPos_; }
  void setDragPos(const Point &p) { dragPos_ = p; }

  const CQChartsResizeSide &dragSide() const { return dragSide_; }
  void setDragSide(const CQChartsResizeSide &s) { dragSide_ = s; }

  bool selectInside(const Point &p);

  CQChartsResizeSide inside(const Point &p) const;

  void updateBBox(double dx, double dy);

  void draw(QPainter *painter) const;

 private:
  void init();

  Point windowToPixel(const Point &p) const;

 private:
  const CQChartsView* view_       { nullptr };                  //!< parent view
  const CQChartsPlot* plot_       { nullptr };                  //!< parent plot
  Mode                mode_       { Mode::RESIZE };             //!< handle mode
  BBox                bbox_       { 0, 0, 1, 1 };               //!< handle bbox
  Handle*             moveHandle_ { nullptr };                  //!< move handle
  Handle*             llHandle_   { nullptr };                  //!< lower left resize handle
  Handle*             lrHandle_   { nullptr };                  //!< lower right resize handle
  Handle*             ulHandle_   { nullptr };                  //!< upper left resize handle
  Handle*             urHandle_   { nullptr };                  //!< upper right resize handle
  Point               dragPos_    { 0, 0 };                     //!< drag position
  CQChartsResizeSide  dragSide_   { CQChartsResizeSide::NONE }; //!< drag side
};

#endif
