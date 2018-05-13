#include <CQChartsCsv.h>
#include <CQCharts.h>
#include <CQCsvModel.h>
#include <CQExprModel.h>
#include <CQChartsModelFn.h>
#include <cassert>

CQChartsCsv::
CQChartsCsv(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  csvModel_ = new CQCsvModel;

  exprModel_ = new CQExprModel(csvModel_);

  exprModel_->addFunction("remap", new CQChartsModelRemapFn(charts, this, exprModel_));

  setSourceModel(exprModel_);
}

CQChartsCsv::
~CQChartsCsv()
{
  delete exprModel_;
  delete csvModel_;
}

void
CQChartsCsv::
setCommentHeader(bool b)
{
  csvModel_->setCommentHeader(b);
}

void
CQChartsCsv::
setFirstLineHeader(bool b)
{
  csvModel_->setFirstLineHeader(b);
}

void
CQChartsCsv::
setFirstColumnHeader(bool b)
{
  csvModel_->setFirstColumnHeader(b);
}

bool
CQChartsCsv::
load(const QString &filename)
{
  return csvModel_->load(filename);
}
