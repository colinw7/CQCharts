#include <CQChartsNoDataObj.h>
#include <CQChartsView.h>
#include <CQChartsRenderer.h>

CQChartsNoDataObj::
CQChartsNoDataObj(CQChartsPlot *plot) :
 CQChartsPlotObj(plot, CQChartsGeom::BBox(0, 0, 1, 1))
{
}

void
CQChartsNoDataObj::
draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &)
{
  CQChartsView *view = plot_->view();

  QString msg = "NO DATA";

  QFontMetrics fm(view->font());

  int fw = fm.width(msg);

  int w = view->width ();
  int h = view->height();

  renderer->drawText(QPointF(w/2 - fw, h/2 - fm.ascent()), msg);
}
