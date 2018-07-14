#include <CQChartsNoDataObj.h>
#include <CQChartsView.h>
#include <QPainter>

CQChartsNoDataObj::
CQChartsNoDataObj(CQChartsPlot *plot) :
 CQChartsPlotObj(plot, CQChartsGeom::BBox(0, 0, 1, 1))
{
}

void
CQChartsNoDataObj::
draw(QPainter *painter)
{
  CQChartsView *view = plot_->view();

  QString msg = "NO DATA";

  QFontMetricsF fm(view->font());

  double fw = fm.width(msg);

  double w = view->width ();
  double h = view->height();

  painter->setFont(view->font());

  painter->drawText(QPointF(w/2 - fw, h/2 - fm.ascent()), msg);
}
