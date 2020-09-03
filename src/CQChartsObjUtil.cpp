#include <CQChartsObjUtil.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsAxisRug.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsArrow.h>
#include <CQCharts.h>

namespace CQChartsObjUtil {

void
getObjPlotViewChart(QObject *obj, CQChartsPlot* &plot, CQChartsView* &view, CQCharts* &charts) {
  view   = nullptr;
  charts = nullptr;

  plot = qobject_cast<CQChartsPlot *>(obj);
  if (plot) return;

  view = qobject_cast<CQChartsView *>(obj);
  if (view) return;

  auto *axis = qobject_cast<CQChartsAxis *>(obj);

  if (axis) {
    plot = axis->plot();
    if (plot) return;

    charts = axis->charts();

    return;
  }

  auto *key = qobject_cast<CQChartsKey *>(obj);

  if (key) {
    plot = key->plot();
    if (plot) return;

    view = key->view();
    if (view) return;

    charts = key->charts();

    return;
  }

  auto *boxObj = qobject_cast<CQChartsBoxObj *>(obj);

  if (boxObj) {
    plot = boxObj->plot();
    if (plot) return;

    view = boxObj->view();
    if (view) return;

    charts = boxObj->charts();

    return;
  }

  auto *arrow = qobject_cast<CQChartsArrow *>(obj);

  if (arrow) {
    plot = arrow->plot();
    if (plot) return;

    view = arrow->view();
    if (view) return;

    charts = arrow->charts();

    return;
  }

  auto *axisRug = qobject_cast<CQChartsAxisRug *>(obj);

  if (axisRug) {
    plot = axisRug->plot();
    if (plot) return;

    return;
  }

  assert(false);
}

}
