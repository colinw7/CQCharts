#include <CQChartsCsvFilterModel.h>
#include <CQCharts.h>
#include <CQCsvModel.h>
#include <CQExprModel.h>
#include <CQChartsModelFn.h>
#include <cassert>

CQChartsCsvFilterModel::
CQChartsCsvFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  csvModel_ = new CQCsvModel;

  exprModel_ = new CQExprModel(csvModel_);

#ifdef CQExprModel_USE_CEXPR
  exprModel_->addExprFunction("remap", new CQChartsModelRemapExprFn(charts, this, exprModel_));
#endif
#ifdef CQExprModel_USE_TCL
  exprModel_->addTclFunction ("remap", new CQChartsModelRemapTclFn (charts, this, exprModel_));
#endif

  setSourceModel(exprModel_);
}

CQChartsCsvFilterModel::
~CQChartsCsvFilterModel()
{
  delete exprModel_;
  delete csvModel_;
}

void
CQChartsCsvFilterModel::
setCommentHeader(bool b)
{
  csvModel_->setCommentHeader(b);
}

void
CQChartsCsvFilterModel::
setFirstLineHeader(bool b)
{
  csvModel_->setFirstLineHeader(b);
}

void
CQChartsCsvFilterModel::
setFirstColumnHeader(bool b)
{
  csvModel_->setFirstColumnHeader(b);
}

void
CQChartsCsvFilterModel::
setSeparator(char c)
{
  csvModel_->setSeparator(c);
}

bool
CQChartsCsvFilterModel::
load(const QString &filename)
{
  return csvModel_->load(filename);
}
