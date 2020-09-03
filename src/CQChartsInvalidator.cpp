#include <CQChartsInvalidator.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsObjUtil.h>
#include <CQCharts.h>

void
CQChartsInvalidator::
invalidate(bool reload)
{
  auto *axis = qobject_cast<CQChartsAxis *>(obj_);

  if (axis) {
    if (axis->isUpdatesEnabled())
      axis->redraw();

    return;
  }

  //---

  CQChartsPlot *plot   = nullptr;
  CQChartsView *view   = nullptr;
  CQCharts     *charts = nullptr;

  CQChartsObjUtil::getObjPlotViewChart(obj_, plot, view, charts);

  if      (plot) {
    if (plot->isUpdatesEnabled()) {
      if (reload)
        plot->updateRangeAndObjs();
      else
        plot->drawObjs();
    }

    auto *key = qobject_cast<CQChartsPlotKey *>(obj_);

    if (key)
      key->updatePositionAndLayout();
  }
  else if (view) {
    view->invalidateObjects();

    view->update();
  }
}

//---

#if 0
QColor
CQChartsInterpolator::
interpColor(const CQChartsColor &c, int i, int n) const
{
  CQChartsUtil::ColorInd ic(i, n);

  return interpColor(c, ic);
}

QColor
CQChartsInterpolator::
interpColor(const CQChartsColor &c, double r) const
{
  CQChartsUtil::ColorInd ic(r);

  return interpColor(c, ic);
}
#endif

QColor
CQChartsInterpolator::
interpColor(const CQChartsColor &c, const CQChartsUtil::ColorInd &ind) const
{
  CQChartsPlot *plot   = nullptr;
  CQChartsView *view   = nullptr;
  CQCharts     *charts = nullptr;

  CQChartsObjUtil::getObjPlotViewChart(obj_, plot, view, charts);

  if      (plot  ) return plot  ->interpColor(c, ind);
  else if (view  ) return view  ->interpColor(c, ind);
  else if (charts) return charts->interpColor(c, ind);

  return QColor();
}
