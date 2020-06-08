#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>
#include <QBuffer>

void
CQChartsPaintDevice::
drawRoundedLine(const Point &p1, const Point &p2, double w)
{
  double w2 = w/2.0;

  double a = atan2(p2.y - p1.y, p2.x - p1.x);

  double c = cos(a);
  double s = sin(a);

  QPainterPath path;

  QPointF pl1(p1.x + w2*s, p1.y - w2*c);
  QPointF pl2(p2.x + w2*s, p2.y - w2*c);
  QPointF pl6(p2.x - w2*s, p2.y + w2*c);
  QPointF pl7(p1.x - w2*s, p1.y + w2*c);

  QPointF pl4(p2.x + w2*c, p2.y + w2*s);
  QPointF pl9(p1.x - w2*c, p1.y - w2*s);

  QPointF pl3(pl2.x() + w2*c, pl2.y() + w2*s);
  QPointF pl5(pl6.x() + w2*c, pl6.y() + w2*s);
  QPointF pl8(pl7.x() - w2*c, pl7.y() - w2*s);
  QPointF pl0(pl1.x() - w2*c, pl1.y() - w2*s);

  QPointF pl23((pl2.x() + pl3.x())/2.0, (pl2.y() + pl3.y())/2.0);
  QPointF pl34((pl3.x() + pl4.x())/2.0, (pl3.y() + pl4.y())/2.0);
  QPointF pl45((pl4.x() + pl5.x())/2.0, (pl4.y() + pl5.y())/2.0);
  QPointF pl56((pl5.x() + pl6.x())/2.0, (pl5.y() + pl6.y())/2.0);

  QPointF pl78((pl7.x() + pl8.x())/2.0, (pl7.y() + pl8.y())/2.0);
  QPointF pl89((pl8.x() + pl9.x())/2.0, (pl8.y() + pl9.y())/2.0);
  QPointF pl90((pl9.x() + pl0.x())/2.0, (pl9.y() + pl0.y())/2.0);
  QPointF pl01((pl0.x() + pl1.x())/2.0, (pl0.y() + pl1.y())/2.0);

  path.moveTo (pl1);
  path.lineTo (pl2);
  path.cubicTo(pl23, pl34, pl4);
  path.cubicTo(pl45, pl56, pl6);
  path.lineTo (pl7);
  path.cubicTo(pl78, pl89, pl9);
  path.cubicTo(pl90, pl01, pl1);

  path.closeSubpath();

  fillPath(path, QBrush(pen().color()));
}

CQChartsGeom::BBox
CQChartsPaintDevice::
windowToPixel(const CQChartsGeom::BBox &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->windowToPixel(r)) : view_->windowToPixel(r));
}

CQChartsGeom::BBox
CQChartsPaintDevice::
pixelToWindow(const CQChartsGeom::BBox &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->pixelToWindow(r)) : view_->pixelToWindow(r));
}

CQChartsGeom::Point
CQChartsPaintDevice::
windowToPixel(const CQChartsGeom::Point &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

CQChartsGeom::Point
CQChartsPaintDevice::
pixelToWindow(const CQChartsGeom::Point &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->pixelToWindow(p)) : view_->pixelToWindow(p));
}

CQChartsGeom::Polygon
CQChartsPaintDevice::
windowToPixel(const CQChartsGeom::Polygon &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

CQChartsGeom::Size
CQChartsPaintDevice::
pixelToWindowSize(const CQChartsGeom::Size &s) const
{
  return (! view_ ? (! plot_ ? s : plot_->pixelToWindowSize(s)) : view_->pixelToWindowSize(s));
}

double
CQChartsPaintDevice::
lengthPixelWidth(const CQChartsLength &w) const
{
  return (! view_ ? (! plot_ ? w.value() :
    plot_->lengthPixelWidth(w)) : view_->lengthPixelWidth(w));
}

double
CQChartsPaintDevice::
lengthPixelHeight(const CQChartsLength &h) const
{
  return (! view_ ? (! plot_ ? h.value() :
    plot_->lengthPixelHeight(h)) : view_->lengthPixelHeight(h));
}

double
CQChartsPaintDevice::
lengthWindowWidth(const CQChartsLength &w) const
{
  return (! view_ ? (! plot_ ? w.value() :
    plot_->lengthPlotWidth(w)) : view_->lengthViewWidth(w));
}

double
CQChartsPaintDevice::
lengthWindowHeight(const CQChartsLength &h) const
{
  return (! view_ ? (! plot_ ? h.value() :
    plot_->lengthPlotHeight(h)) : view_->lengthViewHeight(h));
}

double
CQChartsPaintDevice::
pixelToWindowWidth(double pw) const
{
  return (! view_ ? (! plot_ ? 0.0 :
    plot_->pixelToWindowWidth(pw)) : view_->pixelToWindowWidth(pw));
}

double
CQChartsPaintDevice::
pixelToWindowHeight(double ph) const
{
  return (! view_ ? (! plot_ ? 0.0 :
    plot_->pixelToWindowHeight(ph)) : view_->pixelToWindowHeight(ph));
}

QPainterPath
CQChartsPaintDevice::
windowToPixel(const QPainterPath &path) const
{
  return (! view_ ? (! plot_ ? path : plot_->windowToPixel(path)) : view_->windowToPixel(path));
}

bool
CQChartsPaintDevice::
isInvertX() const
{
  return (plot_ ? plot_->isInvertX() : false);
}

bool
CQChartsPaintDevice::
isInvertY() const
{
  return (plot_ ? plot_->isInvertY() : false);
}
