#include <CQChartsPlotModelVisitor.h>
#include <CQChartsPlot.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

CQChartsPlotModelVisitor::
CQChartsPlotModelVisitor()
{
}

CQChartsPlotModelVisitor::
~CQChartsPlotModelVisitor()
{
}

void
CQChartsPlotModelVisitor::
initVisit()
{
  assert(plot_);

  expr_ = new CQChartsModelExprMatch;

  expr_->setModel(plot_->model().data());

  expr_->initColumns();

  if (plot_->filterStr().length())
    expr_->initMatch(plot_->filterStr());

  plot_->charts()->setCurrentExpr(expr_->qtcl());
}

void
CQChartsPlotModelVisitor::
termVisit()
{
  plot_->charts()->setCurrentExpr(nullptr);

  delete expr_;

  expr_ = nullptr;
}

CQChartsPlotModelVisitor::State
CQChartsPlotModelVisitor::
preVisit(const QAbstractItemModel *model, const VisitData &data)
{
  if (plot_->isInterrupt())
    return State::TERMINATE;

  //---

  int vrow = vrow_++;

  //---

  if (plot_->filterStr().length()) {
    bool ok;

    auto ind = model->index(data.row, 0, data.parent);

    if (! expr_->match(ind, ok))
      return State::SKIP;
  }

  //---

  if (plot_->isEveryEnabled()) {
    int start = plot_->everyStart();
    int end   = plot_->everyEnd();

    if (vrow < start || vrow > end)
      return State::SKIP;

    int step = plot_->everyStep();

    if (step > 1) {
      int n = (vrow - start) % step;

      if (n != 0)
        return State::SKIP;
    }
  }

  //---

  using ModelIndex = CQChartsModelIndex;

  if (plot_->visibleColumn().isValid()) {
    ModelIndex visibleColumnInd(const_cast<CQChartsPlot *>(plot_),
                                data.row, plot_->visibleColumn(), data.parent);

    bool ok;

    auto value = plot_->modelValue(visibleColumnInd, ok);

    if (! ok || ! CQChartsVariant::toBool(value, ok))
      return State::SKIP;
  }

  return State::OK;
}
