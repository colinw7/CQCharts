#include <CQChartsInvalidator.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>

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

  CQChartsAxis *axis = qobject_cast<CQChartsAxis *>(obj_);

  if (axis) {
    axis->redraw();

    return;
  }

  assert(false);
}
