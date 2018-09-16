#include <CQChartsPlotMargin.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>

CQChartsGeom::BBox
CQChartsPlotMargin::
adjustRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox, bool inside) const
{
  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  double l = 0.0, t = 0.0, r = 0.0, b = 0.0;

  if      (left_.units() == CQChartsLength::Units::PIXEL)
    l = plot->pixelToWindowWidth(left_.value());
  else if (left_.units() == CQChartsLength::Units::PLOT)
    l = left_.value();
  else if (left_.units() == CQChartsLength::Units::VIEW)
    l = plot->pixelToWindowWidth(plot->view()->windowToPixelWidth(left_.value()));
  else if (left_.units() == CQChartsLength::Units::PERCENT)
    l = w*left_.value()/100.0;

  if      (top_.units() == CQChartsLength::Units::PIXEL)
    t = plot->pixelToWindowHeight(top_.value());
  else if (top_.units() == CQChartsLength::Units::PLOT)
    t = top_.value();
  else if (top_.units() == CQChartsLength::Units::VIEW)
    t = plot->pixelToWindowHeight(plot->view()->windowToPixelHeight(top_.value()));
  else if (top_.units() == CQChartsLength::Units::PERCENT)
    t = h*top_.value()/100.0;

  if      (right_.units() == CQChartsLength::Units::PIXEL)
    r = plot->pixelToWindowWidth(right_.value());
  else if (right_.units() == CQChartsLength::Units::PLOT)
    r = right_.value();
  else if (right_.units() == CQChartsLength::Units::VIEW)
    r = plot->pixelToWindowWidth(plot->view()->windowToPixelWidth(right_.value()));
  else if (right_.units() == CQChartsLength::Units::PERCENT)
    r = w*right_.value()/100.0;

  if      (bottom_.units() == CQChartsLength::Units::PIXEL)
    b = plot->pixelToWindowHeight(bottom_.value());
  else if (bottom_.units() == CQChartsLength::Units::PLOT)
    b = bottom_.value();
  else if (bottom_.units() == CQChartsLength::Units::VIEW)
    b = plot->pixelToWindowHeight(plot->view()->windowToPixelHeight(bottom_.value()));
  else if (bottom_.units() == CQChartsLength::Units::PERCENT)
    b = h*bottom_.value()/100.0;

  CQChartsGeom::BBox bbox1 = bbox;

  if (inside) {
    bbox1.setLeft  (bbox1.getLeft  () - l);
    bbox1.setRight (bbox1.getRight () + r);
    bbox1.setBottom(bbox1.getBottom() - b);
    bbox1.setTop   (bbox1.getTop   () + t);
  }
  else {
    bbox1.setLeft  (bbox1.getLeft  () + l);
    bbox1.setRight (bbox1.getRight () - r);
    bbox1.setBottom(bbox1.getBottom() + b);
    bbox1.setTop   (bbox1.getTop   () - t);
  }

  return bbox1;
}
