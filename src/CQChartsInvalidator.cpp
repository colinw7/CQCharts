#include <CQChartsInvalidator.h>
#include <CQChartsPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsObjUtil.h>
#include <CQChartsAnnotation.h>
#include <CQCharts.h>

void
CQChartsInvalidator::
invalidate(bool reload)
{
  auto *axis = qobject_cast<CQChartsAxis *>(obj_);

  if (axis) {
    axis->optRedraw();
    return;
  }

  //---

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj_, objData);

  if      (objData.plot) {
    // plot automatically defers updates if updates disabled
    if (reload)
      objData.plot->updateRangeAndObjs();
    else
      objData.plot->drawObjs();

    auto *key = qobject_cast<CQChartsPlotKey *>(obj_);

    if (key) {
      if (objData.plot->isUpdatesEnabled())
        key->updatePositionAndLayout();
    }
  }
  else if (objData.view) {
    if (objData.view->isUpdatesEnabled()) {
      if (reload)
        objData.view->invalidateObjects();

      objData.view->doUpdate();
    }
  }
}

//---

QColor
CQChartsInterpolator::
interpColor(const CQChartsColor &c, const CQChartsUtil::ColorInd &ind) const
{
  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj_, objData);

  if      (objData.annotation) return objData.annotation->interpColor(c, ind);
  if      (objData.plot      ) return objData.plot      ->interpColor(c, ind);
  else if (objData.view      ) return objData.view      ->interpColor(c, ind);
  else if (objData.charts    ) return objData.charts    ->interpColor(c, ind);

  return QColor();
}
