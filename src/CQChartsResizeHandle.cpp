#include <CQChartsResizeHandle.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsPaintDevice.h>

CQChartsResizeHandle::
CQChartsResizeHandle(const View *view, CQChartsResizeSide side) :
 view_(const_cast<View *>(view)), side_(side)
{
}

CQChartsResizeHandle::
CQChartsResizeHandle(const Plot *plot, CQChartsResizeSide side) :
 plot_(const_cast<Plot *>(plot)), side_(side)
{
}

CQChartsView *
CQChartsResizeHandle::
view() const
{
  return view_.data();
}

CQChartsPlot *
CQChartsResizeHandle::
plot() const
{
  return plot_.data();
}

void
CQChartsResizeHandle::
draw(PaintDevice *device) const
{
  // set pen and brush
  CQChartsPenBrush penBrush;

  auto pc = strokeColor();
  auto bc = fillColor();

  auto a = fillAlpha();

  if (isSelected()) {
  //bc = QColor(Qt::red);
    bc = CQChartsUtil::invColor(bc);
    a  = CQChartsAlpha(1.0);
  }

  if      (plot()) {
    plot()->setPen  (penBrush, CQChartsPenData(true, pc));
    plot()->setBrush(penBrush, CQChartsBrushData(true, bc, a));
  }
  else if (view()) {
    view()->setPen  (penBrush, CQChartsPenData(true, pc));
    view()->setBrush(penBrush, CQChartsBrushData(true, bc, a));
  }

  device->setPen  (penBrush.pen);
  device->setBrush(penBrush.brush);

  //---

  path_ = calcPath(device);

  if (isSelected())
    device->fillPath(path_, penBrush.brush);

  device->drawPath(path_);
}

QPainterPath
CQChartsResizeHandle::
calcPath(PaintDevice *device) const
{
  QPainterPath path;

  if      (side() == CQChartsResizeSide::MOVE) {
    auto c = bbox_.getCenter();

    CQChartsDrawUtil::resizeHandlePath(device, path, c);
  }
  else if (side() == CQChartsResizeSide::LL) {
    auto ll = bbox_.getLL();

    CQChartsDrawUtil::cornerHandlePath(device, path, ll);
  }
  else if (side() == CQChartsResizeSide::LR) {
    auto lr = bbox_.getLR();

    CQChartsDrawUtil::cornerHandlePath(device, path, lr);
  }
  else if (side() == CQChartsResizeSide::UL) {
    auto ul = bbox_.getUL();

    CQChartsDrawUtil::cornerHandlePath(device, path, ul);
  }
  else if (side() == CQChartsResizeSide::UR) {
    auto ur = bbox_.getUR();

    CQChartsDrawUtil::cornerHandlePath(device, path, ur);
  }
  else if (side() == CQChartsResizeSide::EXTRA) {
    auto c = bbox_.getCenter();

    if (isControl())
      CQChartsDrawUtil::controlHandlePath(device, path, c);
    else
      CQChartsDrawUtil::extraHandlePath(device, path, c);
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
  return path_.contains(w.qpoint());
}
