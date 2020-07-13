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

  QColor pc = strokeColor();
  QColor bc = fillColor();

  if (isSelected())
    bc = CQChartsUtil::invColor(bc);

  if      (plot()) {
    plot()->setPen  (penBrush, CQChartsPenData(true, pc, CQChartsAlpha()));
    plot()->setBrush(penBrush, CQChartsBrushData(true, bc, fillAlpha()));
  }
  else if (view()) {
    view()->setPen  (penBrush, CQChartsPenData(true, pc, CQChartsAlpha()));
    view()->setBrush(penBrush, CQChartsBrushData(true, bc, fillAlpha()));
  }

  painter->setPen  (penBrush.pen);
  painter->setBrush(penBrush.brush);

  //---

  double cs = 16;

  path_ = QPainterPath();

  if      (side() == CQChartsResizeSide::MOVE) {
    double ms1 = 12;
    double ms2 = 4;

    auto c = windowToPixel(bbox_.getCenter());

    path_.moveTo(c.x - ms1, c.y      );
    path_.lineTo(c.x - ms2, c.y + ms2);
    path_.lineTo(c.x      , c.y + ms1);
    path_.lineTo(c.x + ms2, c.y + ms2);
    path_.lineTo(c.x + ms1, c.y      );
    path_.lineTo(c.x + ms2, c.y - ms2);
    path_.lineTo(c.x      , c.y - ms1);
    path_.lineTo(c.x - ms2, c.y - ms2);
    path_.closeSubpath();
  }
  else if (side() == CQChartsResizeSide::LL) {
    auto ll = windowToPixel(bbox_.getLL());

    path_.addEllipse(ll.x - cs/2, ll.y - cs/2, cs, cs);
  }
  else if (side() == CQChartsResizeSide::LR) {
    auto lr = windowToPixel(bbox_.getLR());

    path_.addEllipse(lr.x - cs/2, lr.y - cs/2, cs, cs);
  }
  else if (side() == CQChartsResizeSide::UL) {
    auto ul = windowToPixel(bbox_.getUL());

    path_.addEllipse(ul.x - cs/2, ul.y - cs/2, cs, cs);
  }
  else if (side() == CQChartsResizeSide::UR) {
    auto ur = windowToPixel(bbox_.getUR());

    path_.addEllipse(ur.x - cs/2, ur.y - cs/2, cs, cs);
  }
  else {
    return;
  }

  painter->drawPath(path_);
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
