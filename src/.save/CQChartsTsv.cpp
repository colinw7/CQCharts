#include <CQChartsTsv.h>
#include <CQCharts.h>
#include <CQTsvModel.h>
#include <CQExprModel.h>
#include <CQChartsModelFn.h>
#include <cassert>

CQChartsTsv::
CQChartsTsv(CQCharts *charts) :
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

CQChartsTsv::
~CQChartsTsv()
{
  delete exprModel_;
  delete tsvModel_;
}

void
CQChartsTsv::
setCommentHeader(bool b)
{
  tsvModel_->setCommentHeader(b);
}

void
CQChartsTsv::
setFirstLineHeader(bool b)
{
  tsvModel_->setFirstLineHeader(b);
}

void
CQChartsTsv::
setFirstColumnHeader(bool b)
{
  tsvModel_->setFirstColumnHeader(b);
}

bool
CQChartsTsv::
load(const QString &filename)
{
  return tsvModel_->load(filename);
}
