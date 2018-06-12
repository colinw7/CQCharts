#include <CQChartsJsonFilterModel.h>
#include <CQJsonModel.h>

CQChartsJsonFilterModel::
CQChartsJsonFilterModel(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  jsonModel_ = new CQJsonModel;

  setSourceModel(jsonModel_);
}

CQChartsJsonFilterModel::
~CQChartsJsonFilterModel()
{
  delete jsonModel_;
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
