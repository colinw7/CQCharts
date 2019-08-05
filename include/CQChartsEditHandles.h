#ifndef CQChartsEditHandles_H
#define CQChartsEditHandles_H

#include <CQChartsGeom.h>
#include <CQChartsTypes.h>
#include <QObject>
#include <QRectF>

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
  CQChartsEditHandles(const CQChartsView *view, const Mode &mode=Mode::RESIZE);
  CQChartsEditHandles(const CQChartsPlot *plot, const Mode &mode=Mode::RESIZE);

 ~CQChartsEditHandles();

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &v) { bbox_ = v; }

  const CQChartsResizeHandle *moveHandle() const { return moveHandle_; }

  const CQChartsResizeHandle *llHandle() const { return llHandle_; }
  const CQChartsResizeHandle *lrHandle() const { return lrHandle_; }
  const CQChartsResizeHandle *ulHandle() const { return ulHandle_; }
  const CQChartsResizeHandle *urHandle() const { return urHandle_; }

  const CQChartsGeom::Point &dragPos() const { return dragPos_; }
  void setDragPos(const CQChartsGeom::Point &p) { dragPos_ = p; }

  const CQChartsResizeSide &dragSide() const { return dragSide_; }
  void setDragSide(const CQChartsResizeSide &s) { dragSide_ = s; }

  bool selectInside(const CQChartsGeom::Point &p);

  CQChartsResizeSide inside(const CQChartsGeom::Point &p) const;

  void updateBBox(double dx, double dy);

  void draw(QPainter *painter) const;

 private:
  void init();

  CQChartsGeom::Point windowToPixel(const CQChartsGeom::Point &p) const;

 private:
  const CQChartsView*   view_       { nullptr };                  //!< parent view
  const CQChartsPlot*   plot_       { nullptr };                  //!< parent plot
  Mode                  mode_       { Mode::RESIZE };             //!< handle mode
  CQChartsGeom::BBox    bbox_       { 0, 0, 1, 1 };               //!< handle bbox
  CQChartsResizeHandle* moveHandle_ { nullptr };                  //!< move handle
  CQChartsResizeHandle* llHandle_   { nullptr };                  //!< lower left resize handle
  CQChartsResizeHandle* lrHandle_   { nullptr };                  //!< lower right resize handle
  CQChartsResizeHandle* ulHandle_   { nullptr };                  //!< upper left resize handle
  CQChartsResizeHandle* urHandle_   { nullptr };                  //!< upper right resize handle
  CQChartsGeom::Point   dragPos_    { 0, 0 };                     //!< drag position
  CQChartsResizeSide    dragSide_   { CQChartsResizeSide::NONE }; //!< drag side
};

#endif
