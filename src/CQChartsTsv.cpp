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

  exprModel_->addFunction("remap", new CQChartsModelRemapFn(charts, this, exprModel_));

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
