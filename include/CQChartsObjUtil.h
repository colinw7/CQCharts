#ifndef CQChartsObjUtil_H
#define CQChartsObjUtil_H

class CQChartsPlot;
class CQChartsView;
class CQCharts;

class QObject;

namespace CQChartsObjUtil {

void getObjPlotViewChart(QObject *obj, CQChartsPlot* &plot,
                         CQChartsView* &view, CQCharts* &charts);

}

#endif
