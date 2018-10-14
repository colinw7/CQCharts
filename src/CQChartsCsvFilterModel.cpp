#include <CQChartsCsvFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQCharts.h>
#include <CQCsvModel.h>

CQChartsCsvFilterModel::
CQChartsCsvFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  setObjectName("csvFilterModel");

  csvModel_ = new CQCsvModel;

  exprModel_ = new CQChartsExprModel(charts_, this, csvModel_);

  setSourceModel(exprModel_);
}

CQChartsCsvFilterModel::
~CQChartsCsvFilterModel()
{
  delete exprModel_;
  delete csvModel_;
}

QAbstractItemModel *
CQChartsCsvFilterModel::
baseModel() const
{
  return csvModel_;
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
