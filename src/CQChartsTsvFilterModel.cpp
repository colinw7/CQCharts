#include <CQChartsTsvFilterModel.h>
#include <CQCharts.h>
#include <CQTsvModel.h>
#include <CQExprModel.h>
#include <CQChartsModelFn.h>
#include <cassert>

CQChartsTsvFilterModel::
CQChartsTsvFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  tsvModel_ = new CQTsvModel;

  exprModel_ = new CQExprModel(tsvModel_);

#ifdef CQExprModel_USE_CEXPR
  exprModel_->addExprFunction("remap", new CQChartsModelRemapExprFn(charts, this, exprModel_));
#endif
#ifdef CQExprModel_USE_TCL
  exprModel_->addTclFunction ("remap", new CQChartsModelRemapTclFn (charts, this, exprModel_));
#endif

  setSourceModel(exprModel_);
}

CQChartsTsvFilterModel::
~CQChartsTsvFilterModel()
{
  delete exprModel_;
  delete tsvModel_;
}

void
CQChartsTsvFilterModel::
setCommentHeader(bool b)
{
  tsvModel_->setCommentHeader(b);
}

void
CQChartsTsvFilterModel::
setFirstLineHeader(bool b)
{
  tsvModel_->setFirstLineHeader(b);
}

void
CQChartsTsvFilterModel::
setFirstColumnHeader(bool b)
{
  tsvModel_->setFirstColumnHeader(b);
}

bool
CQChartsTsvFilterModel::
load(const QString &filename)
{
  return tsvModel_->load(filename);
}
