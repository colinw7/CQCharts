#include <CQChartsEditHandles.h>
#include <CQChartsResizeHandle.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsEnv.h>
#include <CQChartsPaintDevice.h>

CQChartsEditHandles::
CQChartsEditHandles(const Plot *plot, const Mode &mode) :
 plot_(const_cast<Plot *>(plot)), mode_(mode)
{
  init();
}

CQChartsEditHandles::
CQChartsEditHandles(const View *view, const Mode &mode) :
 view_(const_cast<View *>(view)), mode_(mode)
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

CQChartsView *
CQChartsEditHandles::
view() const
{
  return view_.data();
}

CQChartsPlot *
CQChartsEditHandles::
plot() const
{
  return plot_.data();
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
removeExtraHandle()
{
  assert(! extraHandles_.empty());

  auto *handle = extraHandles_.back();

  delete handle;

  extraHandles_.pop_back();
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
    Q_EMIT extraHandleMoved(dragData(), dx, dy);
  }
  else {
    assert(false);
  }
}

//---

void
CQChartsEditHandles::
draw(PaintDevice *device) const
{
  QPen pen(Qt::black);

  pen.setStyle(Qt::DashLine);

  //---

  QPainterPath path;

  CQChartsDrawUtil::editHandlePath(device, path, bbox_);

  device->strokePath(path, pen);

  //---

  moveHandle_->setBBox(bbox_); moveHandle_->draw(device);

  if (mode() == Mode::RESIZE) {
    for (const auto &pc : cornerHandles_) {
      auto *cornerHandle = pc.second;

      cornerHandle->setBBox(bbox_);

      cornerHandle->draw(device);
    }
  }

  for (auto &handle : extraHandles_) {
    handle->draw(device);
  }

  if (CQChartsEnv::getBool("CQ_CHARTS_DRAW_EDIT_HANDLE_BOX", false)) {
    device->setPen(QColor(Qt::red));
    device->setBrush(Qt::NoBrush);

    device->drawRect(bbox_);
  }
}
