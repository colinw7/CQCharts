#include <CQChartsJson.h>
#include <CQJsonModel.h>

CQChartsJson::
CQChartsJson(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  jsonModel_ = new CQJsonModel;

  setSourceModel(jsonModel_);
}

CQChartsJson::
~CQChartsJson()
{
  delete jsonModel_;
}

bool
CQChartsJson::
load(const QString &filename)
{
  return jsonModel_->load(filename);
}

bool
CQChartsJson::
isHierarchical() const
{
  return jsonModel_->isHierarchical();
}
