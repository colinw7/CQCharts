#include <CQChartsJsonFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQJsonModel.h>

CQChartsJsonFilterModel::
CQChartsJsonFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  jsonModel_ = new CQJsonModel;

  //exprModel_ = new CQChartsExprModel(charts_, jsonModel_);

  //setSourceModel(exprModel_);

  setSourceModel(jsonModel_);
}

CQChartsJsonFilterModel::
~CQChartsJsonFilterModel()
{
  delete jsonModel_;
}

QAbstractItemModel *
CQChartsJsonFilterModel::
baseModel() const
{
  return jsonModel_;
}

bool
CQChartsJsonFilterModel::
load(const QString &filename)
{
  return jsonModel_->load(filename);
}

bool
CQChartsJsonFilterModel::
isHierarchical() const
{
  return jsonModel_->isHierarchical();
}
