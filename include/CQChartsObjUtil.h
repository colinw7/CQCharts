#ifndef CQChartsObjUtil_H
#define CQChartsObjUtil_H

class CQCharts;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;

class QObject;

namespace CQChartsObjUtil {

struct ObjData {
  CQCharts*           charts     = nullptr;
  CQChartsView*       view       = nullptr;
  CQChartsPlot*       plot       = nullptr;
  CQChartsAnnotation* annotation = nullptr;

  void clear() {
    charts     = nullptr;
    view       = nullptr;
    plot       = nullptr;
    annotation = nullptr;
  }
};

void getObjData(QObject *obj, ObjData &objData);

}

#endif
