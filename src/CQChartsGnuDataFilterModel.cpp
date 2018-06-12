#include <CQChartsGnuDataFilterModel.h>
#include <CQGnuDataModel.h>
#include <CQCharts.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsGnuDataFilterModel::
CQChartsGnuDataFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  dataModel_ = new CQGnuDataModel;

  exprModel_ = new CQExprModel(dataModel_);

  setSourceModel(exprModel_);
}

CQChartsGnuDataFilterModel::
~CQChartsGnuDataFilterModel()
{
  delete exprModel_;
  delete dataModel_;
}

void
CQChartsGnuDataFilterModel::
setCommentHeader(bool b)
{
  dataModel_->setCommentHeader(b);
}

void
CQChartsGnuDataFilterModel::
setFirstLineHeader(bool b)
{
  dataModel_->setFirstLineHeader(b);
}

bool
CQChartsGnuDataFilterModel::
load(const QString &filename)
{
  return dataModel_->load(filename);
}
