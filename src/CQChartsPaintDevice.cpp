#include <CQChartsPaintDevice.h>
#include <CQChartsPlot.h>
#include <QBuffer>

CQChartsPaintDevice::
CQChartsPaintDevice(View *view) :
 view_(view)
{
}

CQChartsPaintDevice::
CQChartsPaintDevice(Plot *plot) :
 plot_(plot)
{
}

CQChartsView *
CQChartsPaintDevice::
view() const
{
  return view_.data();
}

void
CQChartsPaintDevice::
setView(View *view)
{
  view_ = view;
}

CQChartsPlot *
CQChartsPaintDevice::
plot() const
{
  return plot_.data();
}

void
CQChartsPaintDevice::
setPlot(Plot *plot)
{
  plot_ = plot;
}

CQCharts *
CQChartsPaintDevice::
calcCharts() const
{
  if (plot()) return plot()->charts();
  if (view()) return view()->charts();
  return charts_;
}

CQChartsGeom::BBox
CQChartsPaintDevice::
windowToPixel(const BBox &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->windowToPixel(r)) : view_->windowToPixel(r));
}

CQChartsGeom::BBox
CQChartsPaintDevice::
pixelToWindow(const BBox &r) const
{
  return (! view_ ? (! plot_ ? r : plot_->pixelToWindow(r)) : view_->pixelToWindow(r));
}

CQChartsGeom::Point
CQChartsPaintDevice::
windowToPixel(const Point &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

CQChartsGeom::Point
CQChartsPaintDevice::
pixelToWindow(const Point &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->pixelToWindow(p)) : view_->pixelToWindow(p));
}

CQChartsGeom::Polygon
CQChartsPaintDevice::
windowToPixel(const Polygon &p) const
{
  return (! view_ ? (! plot_ ? p : plot_->windowToPixel(p)) : view_->windowToPixel(p));
}

CQChartsGeom::Size
CQChartsPaintDevice::
pixelToWindowSize(const Size &s) const
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
  return (! view_ ? (! plot_ ? std::abs(pw) :
    plot_->pixelToWindowWidth(pw)) : view_->pixelToWindowWidth(pw));
}

double
CQChartsPaintDevice::
pixelToWindowHeight(double ph) const
{
  return (! view_ ? (! plot_ ? std::abs(ph) :
    plot_->pixelToWindowHeight(ph)) : view_->pixelToWindowHeight(ph));
}

double
CQChartsPaintDevice::
pixelToSignedWindowWidth(double pw) const
{
  return (! view_ ? (! plot_ ? pw :
    plot_->pixelToSignedWindowWidth(pw)) : view_->pixelToSignedWindowWidth(pw));
}

double
CQChartsPaintDevice::
pixelToSignedWindowHeight(double ph) const
{
  return (! view_ ? (! plot_ ? ph :
    plot_->pixelToSignedWindowHeight(ph)) : view_->pixelToSignedWindowHeight(ph));
}

double
CQChartsPaintDevice::
windowToPixelWidth(double ww) const
{
  return (! view_ ? (! plot_ ? ww :
    plot_->windowToPixelWidth(ww)) : view_->windowToPixelWidth(ww));
}

double
CQChartsPaintDevice::
windowToPixelHeight(double wh) const
{
  return (! view_ ? (! plot_ ? wh :
    plot_->windowToPixelHeight(wh)) : view_->windowToPixelHeight(wh));
}

double
CQChartsPaintDevice::
windowToSignedPixelWidth(double ww) const
{
  return (! view_ ? (! plot_ ? ww :
    plot_->windowToSignedPixelWidth(ww)) : view_->windowToSignedPixelWidth(ww));
}

double
CQChartsPaintDevice::
windowToSignedPixelHeight(double wh) const
{
  return (! view_ ? (! plot_ ? wh :
    plot_->windowToSignedPixelHeight(wh)) : view_->windowToSignedPixelHeight(wh));
}

QPainterPath
CQChartsPaintDevice::
windowToPixel(const QPainterPath &path) const
{
  auto path1 = (! view_ ? (! plot_ ? path :
    plot_->windowToPixel(path)) : view_->windowToPixel(path));

  path1.setFillRule(path.fillRule());

  return path1;
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
