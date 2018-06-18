#include <CQChartsTsvFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQCharts.h>
#include <CQTsvModel.h>
#include <cassert>

CQChartsTsvFilterModel::
CQChartsTsvFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  tsvModel_ = new CQTsvModel;

  exprModel_ = new CQChartsExprModel(charts_, tsvModel_);

  setSourceModel(exprModel_);
}

CQChartsTsvFilterModel::
~CQChartsTsvFilterModel()
{
  delete exprModel_;
  delete tsvModel_;
}

QAbstractItemModel *
CQChartsTsvFilterModel::
baseModel() const
{
  return tsvModel_;
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
