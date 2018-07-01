#include <CQChartsGnuDataFilterModel.h>
#include <CQGnuDataModel.h>
#include <CQChartsExprModel.h>
#include <CQCharts.h>

CQChartsGnuDataFilterModel::
CQChartsGnuDataFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  dataModel_ = new CQGnuDataModel;

  exprModel_ = new CQChartsExprModel(charts_, dataModel_);

  setSourceModel(exprModel_);
}

CQChartsGnuDataFilterModel::
~CQChartsGnuDataFilterModel()
{
  delete exprModel_;
  delete dataModel_;
}

QAbstractItemModel *
CQChartsGnuDataFilterModel::
baseModel() const
{
  return dataModel_;
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
