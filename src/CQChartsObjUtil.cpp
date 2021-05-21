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
getObjData(QObject *obj, ObjData &objData)
{
  objData.clear();

  objData.plot = qobject_cast<CQChartsPlot *>(obj);

  if (objData.plot) {
    objData.view   = objData.plot->view();
    objData.charts = objData.plot->charts();
    return;
  }

  //---

  objData.view = qobject_cast<CQChartsView *>(obj);

  if (objData.view) {
    objData.charts = objData.view->charts();
    return;
  }

  //---

  auto *axis = qobject_cast<CQChartsAxis *>(obj);

  if (axis) {
    objData.plot   = axis->plot();
    objData.charts = axis->charts();
    return;
  }

  //---

  auto *key = qobject_cast<CQChartsKey *>(obj);

  if (key) {
    objData.plot   = key->plot();
    objData.charts = key->charts();

    if (! objData.plot)
      objData.view = key->view();

    return;
  }

  //---

  objData.annotation = qobject_cast<CQChartsAnnotation *>(obj);

  if (objData.annotation) {
    objData.plot   = objData.annotation->plot();
    objData.view   = objData.annotation->view();
    objData.charts = objData.annotation->charts();

    if (! objData.plot)
      objData.view = objData.annotation->view();

    return;
  }

  //---

  auto *mapKey = qobject_cast<CQChartsMapKey *>(obj);

  if (mapKey) {
    objData.plot = mapKey->plot();

    if (objData.plot)
      objData.charts = objData.plot->charts();

    return;
  }

  //---

  auto *boxObj = qobject_cast<CQChartsBoxObj *>(obj);

  if (boxObj) {
    objData.plot   = boxObj->plot();
    objData.charts = boxObj->charts();

    if (! objData.plot)
      objData.view = boxObj->view();

    return;
  }

  //---

  auto *arrow = qobject_cast<CQChartsArrow *>(obj);

  if (arrow) {
    objData.plot   = arrow->plot();
    objData.charts = arrow->charts();

    if (! objData.plot)
      objData.view = arrow->view();

    return;
  }

  //---

  auto *axisRug = qobject_cast<CQChartsAxisRug *>(obj);

  if (axisRug) {
    objData.plot = axisRug->plot();

    if (objData.plot)
      objData.charts = objData.plot->charts();

    return;
  }

  assert(false);
}

}
