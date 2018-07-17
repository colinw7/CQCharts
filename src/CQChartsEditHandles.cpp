#include <CQChartsEditHandles.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <QPainter>

CQChartsEditHandles::
CQChartsEditHandles(CQChartsPlot *plot, const Mode &mode) :
 plot_(plot), mode_(mode),
 moveHandle_(plot, CQChartsResizeHandle::Side::MOVE),
 llHandle_  (plot, CQChartsResizeHandle::Side::LL),
 lrHandle_  (plot, CQChartsResizeHandle::Side::LR),
 ulHandle_  (plot, CQChartsResizeHandle::Side::UL),
 urHandle_  (plot, CQChartsResizeHandle::Side::UR)
{
}

CQChartsEditHandles::
CQChartsEditHandles(CQChartsView *view, const Mode &mode) :
 view_(view), mode_(mode),
 moveHandle_(view, CQChartsResizeHandle::Side::MOVE),
 llHandle_  (view, CQChartsResizeHandle::Side::LL),
 lrHandle_  (view, CQChartsResizeHandle::Side::LR),
 ulHandle_  (view, CQChartsResizeHandle::Side::UL),
 urHandle_  (view, CQChartsResizeHandle::Side::UR)
{
}

bool
CQChartsEditHandles::
selectInside(const CQChartsGeom::Point &p)
{
  int changed = 0;

  if (moveHandle_.selectInside(p)) ++changed;

  if (mode() == Mode::RESIZE) {
    if (llHandle_.selectInside(p)) ++changed;
    if (lrHandle_.selectInside(p)) ++changed;
    if (ulHandle_.selectInside(p)) ++changed;
    if (urHandle_.selectInside(p)) ++changed;
  }

  return changed;
}

CQChartsResizeHandle::Side
CQChartsEditHandles::
inside(const CQChartsGeom::Point &p) const
{
  if (moveHandle().inside(p)) return CQChartsResizeHandle::Side::MOVE;

  if (mode() == Mode::RESIZE) {
    if (llHandle().inside(p)) return CQChartsResizeHandle::Side::LL;
    if (lrHandle().inside(p)) return CQChartsResizeHandle::Side::LR;
    if (ulHandle().inside(p)) return CQChartsResizeHandle::Side::UL;
    if (urHandle().inside(p)) return CQChartsResizeHandle::Side::UR;
  }

  return CQChartsResizeHandle::Side::NONE;
}

void
CQChartsEditHandles::
updateBBox(double dx, double dy)
{
  if      (dragSide() == CQChartsResizeHandle::Side::MOVE) {
    bbox_.setLL(bbox_.getLL() + CQChartsGeom::Point(dx, dy));
    bbox_.setUR(bbox_.getUR() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeHandle::Side::LL) {
    bbox_.setLL(bbox_.getLL() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeHandle::Side::LR) {
    bbox_.setLR(bbox_.getLR() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeHandle::Side::UL) {
    bbox_.setUL(bbox_.getUL() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeHandle::Side::UR) {
    bbox_.setUR(bbox_.getUR() + CQChartsGeom::Point(dx, dy));
  }
}

void
CQChartsEditHandles::
draw(QPainter *painter)
{
  QPointF ll = CQChartsUtil::toQPoint(windowToPixel(bbox_.getLL()));
  QPointF ur = CQChartsUtil::toQPoint(windowToPixel(bbox_.getUR()));

  QPainterPath path;

  path.moveTo(ll.x(), ll.y());
  path.lineTo(ur.x(), ll.y());
  path.lineTo(ur.x(), ur.y());
  path.lineTo(ll.x(), ur.y());
  path.closeSubpath();

  QPen pen(Qt::black);

  pen.setStyle(Qt::DashLine);

  painter->strokePath(path, pen);

  //---

  moveHandle_.setBBox(bbox_);

  moveHandle_.draw(painter);

  if (mode() == Mode::RESIZE) {
    llHandle_.setBBox(bbox_);
    lrHandle_.setBBox(bbox_);
    ulHandle_.setBBox(bbox_);
    urHandle_.setBBox(bbox_);

    llHandle_.draw(painter);
    lrHandle_.draw(painter);
    ulHandle_.draw(painter);
    urHandle_.draw(painter);
  }
}

CQChartsGeom::Point
CQChartsEditHandles::
windowToPixel(const CQChartsGeom::Point &p) const
{
  return (view_ ? view_->windowToPixel(p) : plot_->windowToPixel(p));
}
