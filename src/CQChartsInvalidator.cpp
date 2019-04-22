#include <CQChartsInvalidator.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsBoxObj.h>
#include <CQChartsArrow.h>

void
CQChartsInvalidator::
invalidate(bool reload)
{
  CQChartsPlot *plot = qobject_cast<CQChartsPlot *>(obj_);

  if (plot) {
    if (reload)
      plot->updateRangeAndObjs();
    else
      plot->drawObjs();

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
        key->plot()->updateRangeAndObjs();
      else
        key->plot()->drawObjs();
    }
    else if (key->view()) {
      key->view()->update();
    }

    return;
  }

  CQChartsBoxObj *boxObj = qobject_cast<CQChartsBoxObj *>(obj_);

  if (boxObj) {
    if      (boxObj->plot()) {
      if (reload)
        boxObj->plot()->updateRangeAndObjs();
      else
        boxObj->plot()->drawObjs();
    }
    else if (boxObj->view()) {
      boxObj->view()->update();
    }

    return;
  }

  CQChartsArrow *arrow = qobject_cast<CQChartsArrow *>(obj_);

  if (arrow) {
    if      (arrow->plot()) {
      if (reload)
        arrow->plot()->updateRangeAndObjs();
      else
        arrow->plot()->drawObjs();
    }
    else if (arrow->view()) {
      arrow->view()->update();
    }

    return;
  }

  assert(false);
}
