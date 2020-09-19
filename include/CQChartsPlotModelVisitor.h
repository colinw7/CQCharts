#ifndef CQChartsPlotModelVisitor_H
#define CQChartsPlotModelVisitor_H

#include <CQChartsModelVisitor.h>

class CQChartsPlot;
class CQChartsModelExprMatch;

//! plot model visitor
class CQChartsPlotModelVisitor : public CQChartsModelVisitor {
 public:
  using Plot = CQChartsPlot;

 public:
  CQChartsPlotModelVisitor();

  virtual ~CQChartsPlotModelVisitor();

  const Plot *plot() const { return plot_; }
  void setPlot(const Plot *p) { plot_ = p; }

  void initVisit() override;
  void termVisit() override;

  State preVisit(const QAbstractItemModel *model, const VisitData &data) override;

 private:
  const Plot*             plot_ { nullptr };
  int                     vrow_ { 0 };
  CQChartsModelExprMatch* expr_ { nullptr };
};

#endif
