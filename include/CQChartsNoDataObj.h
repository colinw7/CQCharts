#ifndef CQChartsNoDataObj_H
#define CQChartsNoDataObj_H

#include <CQChartsPlotObj.h>

class CQChartsNoDataObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsNoDataObj(CQChartsPlot *plot);

  QString calcId() const override { return "No Data"; }

  void getSelectIndices(Indices &) const override { }

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *, const CQChartsPlot::Layer &);
};

#endif
