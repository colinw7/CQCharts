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

  for (auto &pc : cornerHandles_)
    delete pc.second;

  removeExtraHandles();
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

  std::vector<ResizeSide> resizeSides = {
    ResizeSide::LL, ResizeSide::LR, ResizeSide::UL, ResizeSide::UR };

  for (const auto &resizeSide : resizeSides)
    cornerHandles_[resizeSide] = createHandle(resizeSide);
}

//---

void
CQChartsEditHandles::
addExtraHandle(Handle *handle)
{
  extraHandles_.push_back(handle);
}

void
CQChartsEditHandles::
removeExtraHandles()
{
  for (auto &handle : extraHandles_)
    delete handle;

  extraHandles_.clear();
}

//---

bool
CQChartsEditHandles::
selectInside(const Point &p)
{
  int changed = 0;

  if (moveHandle_->selectInside(p)) ++changed;

  if (mode() == Mode::RESIZE) {
    for (const auto &pc : cornerHandles_) {
      auto *cornerHandle = pc.second;

      if (cornerHandle->selectInside(p))
        ++changed;
    }
  }

  for (auto &handle : extraHandles_) {
    if (handle->selectInside(p))
      ++changed;
  }

  return changed;
}

bool
CQChartsEditHandles::
inside(const Point &p, InsideData &insideData) const
{
  if (moveHandle()->inside(p)) {
    insideData.resizeSide = ResizeSide::MOVE;
    insideData.data       = QVariant();

    return true;
  }

  if (mode() == Mode::RESIZE) {
    for (const auto &pc : cornerHandles_) {
      auto *cornerHandle = pc.second;

      if (cornerHandle->inside(p)) {
        insideData.resizeSide = pc.first;
        insideData.data       = QVariant();

        return true;
      }
    }
  }

  for (auto &handle : extraHandles_) {
    if (handle->inside(p)) {
      insideData.resizeSide = ResizeSide::EXTRA;
      insideData.data       = handle->data();

      return true;
    }
  }

  return false;
}

//---

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
  else if (dragSide() == ResizeSide::EXTRA) {
    emit extraHandleMoved(dragData(), dx, dy);
  }
  else {
    assert(false);
  }
}

//---

void
CQChartsEditHandles::
draw(QPainter *painter) const
{
  QPen pen(Qt::black);

  pen.setStyle(Qt::DashLine);

  //---

  auto windowToPixel =[&](const BBox &bbox) {
    return (view_ ? view_->windowToPixel(bbox) : plot_->windowToPixel(bbox));
  };

  QPainterPath path;

  CQChartsDrawUtil::editHandlePath(path, windowToPixel(bbox_));

  painter->strokePath(path, pen);

  //---

  moveHandle_->setBBox(bbox_); moveHandle_->draw(painter);

  if (mode() == Mode::RESIZE) {
    for (const auto &pc : cornerHandles_) {
      auto *cornerHandle = pc.second;

      cornerHandle->setBBox(bbox_);

      cornerHandle->draw(painter);
    }
  }

  for (auto &handle : extraHandles_) {
    handle->draw(painter);
  }
}
