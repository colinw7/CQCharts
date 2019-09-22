#include <CQChartsEditHandles.h>
#include <CQChartsResizeHandle.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <QPainter>

CQChartsEditHandles::
CQChartsEditHandles(const CQChartsPlot *plot, const Mode &mode) :
 plot_(plot), mode_(mode)
{
  init();
}

CQChartsEditHandles::
CQChartsEditHandles(const CQChartsView *view, const Mode &mode) :
 view_(view), mode_(mode)
{
  init();
}

CQChartsEditHandles::
~CQChartsEditHandles()
{
  delete moveHandle_;
  delete llHandle_;
  delete lrHandle_;
  delete ulHandle_;
  delete urHandle_;
}

void
CQChartsEditHandles::
init()
{
  auto createHandle = [&](CQChartsResizeSide side) -> CQChartsResizeHandle * {
    if (plot_)
      return new CQChartsResizeHandle(plot_, side);
    else
      return new CQChartsResizeHandle(view_, side);
  };

  moveHandle_ = createHandle(CQChartsResizeSide::MOVE);
  llHandle_   = createHandle(CQChartsResizeSide::LL  );
  lrHandle_   = createHandle(CQChartsResizeSide::LR  );
  ulHandle_   = createHandle(CQChartsResizeSide::UL  );
  urHandle_   = createHandle(CQChartsResizeSide::UR  );
}

bool
CQChartsEditHandles::
selectInside(const CQChartsGeom::Point &p)
{
  int changed = 0;

  if (moveHandle_->selectInside(p)) ++changed;

  if (mode() == Mode::RESIZE) {
    if (llHandle_->selectInside(p)) ++changed;
    if (lrHandle_->selectInside(p)) ++changed;
    if (ulHandle_->selectInside(p)) ++changed;
    if (urHandle_->selectInside(p)) ++changed;
  }

  return changed;
}

CQChartsResizeSide
CQChartsEditHandles::
inside(const CQChartsGeom::Point &p) const
{
  if (moveHandle()->inside(p)) return CQChartsResizeSide::MOVE;

  if (mode() == Mode::RESIZE) {
    if (llHandle()->inside(p)) return CQChartsResizeSide::LL;
    if (lrHandle()->inside(p)) return CQChartsResizeSide::LR;
    if (ulHandle()->inside(p)) return CQChartsResizeSide::UL;
    if (urHandle()->inside(p)) return CQChartsResizeSide::UR;
  }

  return CQChartsResizeSide::NONE;
}

void
CQChartsEditHandles::
updateBBox(double dx, double dy)
{
  if      (dragSide() == CQChartsResizeSide::MOVE) {
    bbox_.moveBy(CQChartsGeom::Point(dx, dy));

    //bbox_.setLL(bbox_.getLL() + CQChartsGeom::Point(dx, dy));
    //bbox_.setUR(bbox_.getUR() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeSide::LL) {
    bbox_.setLL(bbox_.getLL() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeSide::LR) {
    bbox_.setLR(bbox_.getLR() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeSide::UL) {
    bbox_.setUL(bbox_.getUL() + CQChartsGeom::Point(dx, dy));
  }
  else if (dragSide() == CQChartsResizeSide::UR) {
    bbox_.setUR(bbox_.getUR() + CQChartsGeom::Point(dx, dy));
  }
}

void
CQChartsEditHandles::
draw(QPainter *painter) const
{
  QPointF ll = windowToPixel(bbox_.getLL()).qpoint();
  QPointF ur = windowToPixel(bbox_.getUR()).qpoint();

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

  moveHandle_->setBBox(bbox_); moveHandle_->draw(painter);

  if (mode() == Mode::RESIZE) {
    llHandle_->setBBox(bbox_); llHandle_->draw(painter);
    lrHandle_->setBBox(bbox_); lrHandle_->draw(painter);
    ulHandle_->setBBox(bbox_); ulHandle_->draw(painter);
    urHandle_->setBBox(bbox_); urHandle_->draw(painter);
  }
}

CQChartsGeom::Point
CQChartsEditHandles::
windowToPixel(const CQChartsGeom::Point &p) const
{
  return (view_ ? view_->windowToPixel(p) : plot_->windowToPixel(p));
}
