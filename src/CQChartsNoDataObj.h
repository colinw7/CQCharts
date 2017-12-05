#ifndef CQChartsNoDataObj_H
#define CQChartsNoDataObj_H

#include <CQChartsPlotObj.h>

class CQChartsNoDataObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsNoDataObj(CQChartsPlot *plot);

  QString calcId() const { return "No Data"; }

  void draw(CQChartsRenderer *, const CQChartsPlot::Layer &);
};

#endif
