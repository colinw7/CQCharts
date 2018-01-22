#include <CQChartsGnuData.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQGnuDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsGnuData::
CQChartsGnuData(CQCharts *charts) :
 CQChartsModelFilter(charts)
{
  dataModel_ = new CQGnuDataModel;

  exprModel_ = new CQExprModel(dataModel_);

  setSourceModel(exprModel_);
}

CQChartsGnuData::
~CQChartsGnuData()
{
  delete exprModel_;
  delete dataModel_;
}

void
CQChartsGnuData::
setCommentHeader(bool b)
{
  dataModel_->setCommentHeader(b);
}

void
CQChartsGnuData::
setFirstLineHeader(bool b)
{
  dataModel_->setFirstLineHeader(b);
}

bool
CQChartsGnuData::
load(const QString &filename)
{
  return dataModel_->load(filename);
}
