#include <CQChartsExprModel.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsExprModel::
CQChartsExprModel(CQCharts *charts, int nc, int nr) :
 CQChartsModelFilter(charts)
{
  QModelIndex parent; // OK ?

  dataModel_ = new CQDataModel(nc, nr);

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = dataModel_->index(r, c, parent);

      dataModel_->setData(ind, QVariant(r*nc + c));
    }
  }

  exprModel_ = new CQExprModel(dataModel_);

  setSourceModel(exprModel_);
}

CQChartsExprModel::
~CQChartsExprModel()
{
  delete exprModel_;
  delete dataModel_;
}
