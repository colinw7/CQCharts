#include <CQChartsNoDataObj.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

CQChartsNoDataObj::
CQChartsNoDataObj(CQChartsPlot *plot) :
 CQChartsPlotObj(plot, BBox(0, 0, 1, 1))
{
}

void
CQChartsNoDataObj::
draw(PaintDevice *device) const
{
  auto *view = plot()->view();

  // get plot inside bbox in view coords
  auto bbox = plot()->innerViewBBox();

  // get position of center (in view coords)
  auto vc = bbox.getCenter();

  // get position of center (in plot coords)
  auto c = plot()->pixelToWindow(view->windowToPixel(vc));

  //--

  // draw no data text
  auto msg = QString("NO DATA");

  view->setPainterFont(device, font());

  CQChartsDrawUtil::drawCenteredText(device, c, msg);
}
