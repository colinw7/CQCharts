#include <CQChartsPlotMargin.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>

CQChartsPlotMargin::
CQChartsPlotMargin(const QString &str)
{
  setValue(str);
}

CQChartsPlotMargin::
CQChartsPlotMargin(const Length &l, const Length &t, const Length &r, const Length &b) :
  CQChartsMargin(l, t, r, b)
{
}

CQChartsPlotMargin::
CQChartsPlotMargin(const Length &l) :
 CQChartsMargin(l, l, l, l)
{
}

void
CQChartsPlotMargin::
set(const Length &l, const Length &t, const Length &r, const Length &b)
{
  CQChartsMargin::set(l, t, r, b);
}

void
CQChartsPlotMargin::
set(const Length &l)
{
  CQChartsMargin::set(l);
}

bool
CQChartsPlotMargin::
setValue(const QString &str)
{
  return CQChartsMargin::setValue(str);
}

// adjust plot bbox using margin (inside or outside)
CQChartsGeom::BBox
CQChartsPlotMargin::
adjustPlotRange(const CQChartsPlot *plot, const BBox &bbox, bool inside) const
{
  auto plotViewWidth = [&](const Length &len) -> double {
    if      (len.units() == Units::PIXEL)
      return plot->pixelToWindowWidth(len.value());
    else if (len.units() == Units::PLOT)
      return len.value();
    else if (len.units() == Units::VIEW)
      return plot->pixelToWindowWidth(plot->view()->windowToPixelWidth(len.value()));
    else if (len.units() == Units::PERCENT)
      return bbox.getWidth()*len.value()/100.0;
    else if (len.units() == Units::EM)
      return plot->pixelToWindowWidth(len.value()*plot->view()->fontEm());
    else if (len.units() == Units::EX)
      return plot->pixelToWindowWidth(len.value()*plot->view()->fontEx());
    else
      return len.value();
  };

  auto plotViewHeight = [&](const Length &len) -> double {
    if      (len.units() == Units::PIXEL)
      return plot->pixelToWindowHeight(len.value());
    else if (len.units() == Units::PLOT)
      return len.value();
    else if (len.units() == Units::VIEW)
      return plot->pixelToWindowHeight(plot->view()->windowToPixelHeight(len.value()));
    else if (len.units() == Units::PERCENT)
      return bbox.getHeight()*len.value()/100.0;
    else if (len.units() == Units::EM)
      return plot->pixelToWindowHeight(len.value()*plot->view()->fontEm());
    else if (len.units() == Units::EX)
      return plot->pixelToWindowHeight(len.value()*plot->view()->fontEx());
    else
      return len.value();
  };

  double l = plotViewWidth (left_  );
  double t = plotViewHeight(top_   );
  double r = plotViewWidth (right_ );
  double b = plotViewHeight(bottom_);

  auto bbox1 = bbox;

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

// adjust view bbox using margin (inside or outside)
CQChartsGeom::BBox
CQChartsPlotMargin::
adjustViewRange(const CQChartsPlot *plot, const BBox &bbox, bool inside) const
{
  auto lengthViewWidth = [&](const Length &len) -> double {
    if      (len.units() == Units::PIXEL)
      return plot->view()->pixelToWindowWidth(len.value());
    else if (len.units() == Units::PLOT)
      return plot->view()->pixelToWindowWidth(plot->windowToPixelWidth(len.value()));
    else if (len.units() == Units::VIEW)
      return len.value();
    else if (len.units() == Units::PERCENT)
      return bbox.getWidth()*len.value()/100.0;
    else if (len.units() == Units::EM)
      return plot->view()->pixelToWindowWidth(len.value()*plot->view()->fontEm());
    else if (len.units() == Units::EX)
      return plot->view()->pixelToWindowWidth(len.value()*plot->view()->fontEx());
    else if (len.isValid())
      return len.value();
    else
      return 0.0;
  };

  auto lengthViewHeight = [&](const Length &len) -> double {
    if      (len.units() == Units::PIXEL)
      return plot->view()->pixelToWindowHeight(len.value());
    else if (len.units() == Units::PLOT)
      return plot->view()->pixelToWindowHeight(plot->windowToPixelHeight(len.value()));
    else if (len.units() == Units::VIEW)
      return len.value();
    else if (len.units() == Units::PERCENT)
      return bbox.getHeight()*len.value()/100.0;
    else if (len.units() == Units::EM)
      return plot->view()->pixelToWindowHeight(len.value()*plot->view()->fontEm());
    else if (len.units() == Units::EX)
      return plot->view()->pixelToWindowHeight(len.value()*plot->view()->fontEx());
    else if (len.isValid())
      return len.value();
    else
      return 0.0;
  };

  double l = lengthViewWidth (left_  );
  double t = lengthViewHeight(top_   );
  double r = lengthViewWidth (right_ );
  double b = lengthViewHeight(bottom_);

  auto bbox1 = bbox;

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
