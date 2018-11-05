#include <CQChartsPlotMargin.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>

CQChartsGeom::BBox
CQChartsPlotMargin::
adjustPlotRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox, bool inside) const
{
  auto plotViewWidth = [&](const CQChartsLength &len) -> double {
    if      (len.units() == CQChartsLength::Units::PIXEL)
      return plot->pixelToWindowWidth(len.value());
    else if (len.units() == CQChartsLength::Units::PLOT)
      return len.value();
    else if (len.units() == CQChartsLength::Units::VIEW)
      return plot->pixelToWindowWidth(plot->view()->windowToPixelWidth(len.value()));
    else if (len.units() == CQChartsLength::Units::PERCENT)
      return bbox.getWidth()*len.value()/100.0;
    else
      return len.value();
  };

  auto plotViewHeight = [&](const CQChartsLength &len) -> double {
    if      (len.units() == CQChartsLength::Units::PIXEL)
      return plot->pixelToWindowHeight(len.value());
    else if (len.units() == CQChartsLength::Units::PLOT)
      return len.value();
    else if (len.units() == CQChartsLength::Units::VIEW)
      return plot->pixelToWindowHeight(plot->view()->windowToPixelHeight(len.value()));
    else if (len.units() == CQChartsLength::Units::PERCENT)
      return bbox.getHeight()*len.value()/100.0;
    else
      return len.value();
  };

  double l = plotViewWidth (left_  );
  double t = plotViewHeight(top_   );
  double r = plotViewWidth (right_ );
  double b = plotViewHeight(bottom_);

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

CQChartsGeom::BBox
CQChartsPlotMargin::
adjustViewRange(const CQChartsPlot *plot, const CQChartsGeom::BBox &bbox, bool inside) const
{
  auto lengthViewWidth = [&](const CQChartsLength &len) -> double {
    if      (len.units() == CQChartsLength::Units::PIXEL)
      return plot->view()->pixelToWindowWidth(len.value());
    else if (len.units() == CQChartsLength::Units::PLOT)
      return plot->view()->pixelToWindowWidth(plot->windowToPixelWidth(len.value()));
    else if (len.units() == CQChartsLength::Units::VIEW)
      return len.value();
    else if (len.units() == CQChartsLength::Units::PERCENT)
      return bbox.getWidth()*len.value()/100.0;
    else
      return len.value();
  };

  auto lengthViewHeight = [&](const CQChartsLength &len) -> double {
    if      (len.units() == CQChartsLength::Units::PIXEL)
      return plot->view()->pixelToWindowHeight(len.value());
    else if (len.units() == CQChartsLength::Units::PLOT)
      return plot->view()->pixelToWindowHeight(plot->windowToPixelHeight(len.value()));
    else if (len.units() == CQChartsLength::Units::VIEW)
      return len.value();
    else if (len.units() == CQChartsLength::Units::PERCENT)
      return bbox.getHeight()*len.value()/100.0;
    else
      return len.value();
  };

  double l = lengthViewWidth (left_  );
  double t = lengthViewHeight(top_   );
  double r = lengthViewWidth (right_ );
  double b = lengthViewHeight(bottom_);

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
