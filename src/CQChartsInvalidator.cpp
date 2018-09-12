#include <CQChartsInvalidator.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>

void
CQChartsInvalidator::
invalidate(bool reload)
{
  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj_);

  if (plot) {
    if (reload)
      plot->updateRangeAndObjs();
    else
      plot->invalidateLayers();

    return;
  }

  CQChartsView *view = qobject_cast<CQChartsView *>(obj_);

  if (view) {
    view->update();

    return;
  }

  CQChartsAxis *axis = qobject_cast<CQChartsAxis *>(obj_);

  if (axis) {
    axis->redraw();

    return;
  }

  CQChartsKey *key = qobject_cast<CQChartsKey *>(obj_);

  if (key) {
    if      (key->plot()) {
      if (reload)
        plot->updateRangeAndObjs();
      else
        plot->invalidateLayers();
    }
    else if (key->view()) {
      view->update();
    }

    return;
  }

  assert(false);
}
