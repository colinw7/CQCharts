#include <CQChartsResizeHandle.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <QPainter>

CQChartsResizeHandle::
CQChartsResizeHandle(const CQChartsView *view, CQChartsResizeSide side) :
 view_(view), side_(side)
{
}

CQChartsResizeHandle::
CQChartsResizeHandle(const CQChartsPlot *plot, CQChartsResizeSide side) :
 plot_(plot), side_(side)
{
}

void
CQChartsResizeHandle::
draw(QPainter *painter) const
{
  // set pen and brush
  CQChartsPenBrush penBrush;

  auto pc = strokeColor();
  auto bc = fillColor();

  if (isSelected())
    bc = CQChartsUtil::invColor(bc);

  if      (plot()) {
    plot()->setPen  (penBrush, CQChartsPenData(true, pc));
    plot()->setBrush(penBrush, CQChartsBrushData(true, bc, fillAlpha()));
  }
  else if (view()) {
    view()->setPen  (penBrush, CQChartsPenData(true, pc));
    view()->setBrush(penBrush, CQChartsBrushData(true, bc, fillAlpha()));
  }

  painter->setPen  (penBrush.pen);
  painter->setBrush(penBrush.brush);

  //---

  path_ = calcPath();

  painter->drawPath(path_);
}

QPainterPath
CQChartsResizeHandle::
calcPath() const
{
  QPainterPath path;

  if      (side() == CQChartsResizeSide::MOVE) {
    auto c = windowToPixel(bbox_.getCenter());

    CQChartsDrawUtil::resizeHandlePath(path, c);
  }
  else if (side() == CQChartsResizeSide::LL) {
    auto ll = windowToPixel(bbox_.getLL());

    CQChartsDrawUtil::cornerHandlePath(path, ll);
  }
  else if (side() == CQChartsResizeSide::LR) {
    auto lr = windowToPixel(bbox_.getLR());

    CQChartsDrawUtil::cornerHandlePath(path, lr);
  }
  else if (side() == CQChartsResizeSide::UL) {
    auto ul = windowToPixel(bbox_.getUL());

    CQChartsDrawUtil::cornerHandlePath(path, ul);
  }
  else if (side() == CQChartsResizeSide::UR) {
    auto ur = windowToPixel(bbox_.getUR());

    CQChartsDrawUtil::cornerHandlePath(path, ur);
  }
  else if (side() == CQChartsResizeSide::EXTRA) {
    auto c = windowToPixel(bbox_.getCenter());

    CQChartsDrawUtil::extraHandlePath(path, c);
  }
  else {
    assert(false);
  }

  return path;
}

bool
CQChartsResizeHandle::
selectInside(const Point &p)
{
  bool selected = inside(p);

  if (selected == isSelected())
    return false;

  setSelected(selected);

  return true;
}

bool
CQChartsResizeHandle::
inside(const Point &w) const
{
  auto p = windowToPixel(w);

  return path_.contains(p.qpoint());
}

CQChartsGeom::Point
CQChartsResizeHandle::
windowToPixel(const Point &p) const
{
  if (view_) return view_->windowToPixel(p);
  if (plot_) return plot_->windowToPixel(p);
  return p;
}
