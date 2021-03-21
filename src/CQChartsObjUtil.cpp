#include <CQChartsObjUtil.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsAxisRug.h>
#include <CQChartsKey.h>
#include <CQChartsMapKey.h>
#include <CQChartsAnnotation.h>
#include <CQChartsBoxObj.h>
#include <CQChartsArrow.h>
#include <CQCharts.h>

namespace CQChartsObjUtil {

void
getObjPlotViewChart(QObject *obj, CQChartsPlot* &plot, CQChartsView* &view, CQCharts* &charts) {
  view   = nullptr;
  charts = nullptr;

  plot = qobject_cast<CQChartsPlot *>(obj);

  if (plot) {
    charts = plot->charts();
    return;
  }

  view = qobject_cast<CQChartsView *>(obj);

  if (view) {
    charts = view->charts();
    return;
  }

  auto *axis = qobject_cast<CQChartsAxis *>(obj);

  if (axis) {
    plot   = axis->plot();
    charts = axis->charts();

    return;
  }

  auto *key = qobject_cast<CQChartsKey *>(obj);

  if (key) {
    plot   = key->plot();
    charts = key->charts();

    if (! plot)
      view = key->view();

    return;
  }

  auto *annotation = qobject_cast<CQChartsAnnotation *>(obj);

  if (annotation) {
    plot   = annotation->plot();
    charts = annotation->charts();

    if (! plot)
      view = annotation->view();

    return;
  }

  auto *mapKey = qobject_cast<CQChartsMapKey *>(obj);

  if (mapKey) {
    plot = mapKey->plot();

    if (plot) {
      charts = plot->charts();
      return;
    }
  }

  auto *boxObj = qobject_cast<CQChartsBoxObj *>(obj);

  if (boxObj) {
    plot   = boxObj->plot();
    charts = boxObj->charts();

    if (plot)
      return;

    view = boxObj->view();

    return;
  }

  auto *arrow = qobject_cast<CQChartsArrow *>(obj);

  if (arrow) {
    plot   = arrow->plot();
    charts = arrow->charts();

    if (! plot)
      view = arrow->view();

    return;
  }

  auto *axisRug = qobject_cast<CQChartsAxisRug *>(obj);

  if (axisRug) {
    plot = axisRug->plot();

    if (plot) {
      charts = plot->charts();
      return;
    }
  }

  assert(false);
}

}
