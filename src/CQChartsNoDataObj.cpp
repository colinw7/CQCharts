#include <CQChartsNoDataObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

CQChartsNoDataObj::
CQChartsNoDataObj(CQChartsPlot *plot) :
 CQChartsPlotObj(plot, CQChartsGeom::BBox(0, 0, 1, 1))
{
}

void
CQChartsNoDataObj::
draw(CQChartsPaintDevice *device)
{
  auto view = plot()->view();

  // get plot inside bbox in view coords
  auto bbox = plot()->innerViewBBox();

  // get position of center
  double xm = bbox.getXMid();
  double ym = bbox.getYMid();

  CQChartsGeom::Point c(xm, ym);

  //--

  // draw no data text
  QString msg = "NO DATA";

  view->setPainterFont(device, font());

  CQChartsDrawUtil::drawCenteredText(device, c, msg);
}
