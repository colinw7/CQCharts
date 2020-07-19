#ifndef CQChartsPlotModelVisitor_H
#define CQChartsPlotModelVisitor_H

#include <CQChartsModelVisitor.h>

class CQChartsPlot;
class CQChartsModelExprMatch;

//! plot model visitor
class CQChartsPlotModelVisitor : public CQChartsModelVisitor {
 public:
  CQChartsPlotModelVisitor();

  virtual ~CQChartsPlotModelVisitor();

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(const CQChartsPlot *p) { plot_ = p; }

  void initVisit() override;
  void termVisit() override;

  State preVisit(const QAbstractItemModel *model, const VisitData &data) override;

 private:
  const CQChartsPlot*     plot_ { nullptr };
  int                     vrow_ { 0 };
  CQChartsModelExprMatch* expr_ { nullptr };
};

#endif
