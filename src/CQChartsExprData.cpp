#include <CQChartsExprData.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsExprData::
CQChartsExprData(CQCharts *charts, int nc, int nr) :
 CQChartsModelFilter(charts)
{
  dataModel_ = new CQDataModel(nc, nr);

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = dataModel_->index(r, c);

      dataModel_->setData(ind, QVariant(r*nc + c));
    }
  }

  exprModel_ = new CQExprModel(dataModel_);

  setSourceModel(exprModel_);
}

CQChartsExprData::
~CQChartsExprData()
{
  delete exprModel_;
  delete dataModel_;
}
