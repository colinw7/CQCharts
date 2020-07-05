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
  auto createHandle = [&](ResizeSide side) {
    if (plot_) return new CQChartsResizeHandle(plot_, side);
    else       return new CQChartsResizeHandle(view_, side);
  };

  moveHandle_ = createHandle(ResizeSide::MOVE);
  llHandle_   = createHandle(ResizeSide::LL  );
  lrHandle_   = createHandle(ResizeSide::LR  );
  ulHandle_   = createHandle(ResizeSide::UL  );
  urHandle_   = createHandle(ResizeSide::UR  );
}

bool
CQChartsEditHandles::
selectInside(const Point &p)
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
inside(const Point &p) const
{
  if (moveHandle()->inside(p)) return ResizeSide::MOVE;

  if (mode() == Mode::RESIZE) {
    if (llHandle()->inside(p)) return ResizeSide::LL;
    if (lrHandle()->inside(p)) return ResizeSide::LR;
    if (ulHandle()->inside(p)) return ResizeSide::UL;
    if (urHandle()->inside(p)) return ResizeSide::UR;
  }

  return ResizeSide::NONE;
}

void
CQChartsEditHandles::
updateBBox(double dx, double dy)
{
  if      (dragSide() == ResizeSide::MOVE) {
    bbox_.moveBy(Point(dx, dy));

    //bbox_.setLL(bbox_.getLL() + Point(dx, dy));
    //bbox_.setUR(bbox_.getUR() + Point(dx, dy));
  }
  else if (dragSide() == ResizeSide::LL) {
    bbox_.setLL(bbox_.getLL() + Point(dx, dy));
  }
  else if (dragSide() == ResizeSide::LR) {
    bbox_.setLR(bbox_.getLR() + Point(dx, dy));
  }
  else if (dragSide() == ResizeSide::UL) {
    bbox_.setUL(bbox_.getUL() + Point(dx, dy));
  }
  else if (dragSide() == ResizeSide::UR) {
    bbox_.setUR(bbox_.getUR() + Point(dx, dy));
  }
}

void
CQChartsEditHandles::
draw(QPainter *painter) const
{
  auto ll = windowToPixel(bbox_.getLL());
  auto ur = windowToPixel(bbox_.getUR());

  QPainterPath path;

  path.moveTo(ll.x, ll.y);
  path.lineTo(ur.x, ll.y);
  path.lineTo(ur.x, ur.y);
  path.lineTo(ll.x, ur.y);
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
windowToPixel(const Point &p) const
{
  return (view_ ? view_->windowToPixel(p) : plot_->windowToPixel(p));
}
