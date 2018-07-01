#include <CQChartsExprDataFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQCharts.h>
#include <CQDataModel.h>

CQChartsExprDataFilterModel::
CQChartsExprDataFilterModel(CQCharts *charts, int nc, int nr) :
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

  exprModel_ = new CQChartsExprModel(charts_, dataModel_);

  setSourceModel(exprModel_);
}

CQChartsExprDataFilterModel::
~CQChartsExprDataFilterModel()
{
  delete exprModel_;
  delete dataModel_;
}

QAbstractItemModel *
CQChartsExprDataFilterModel::
baseModel() const
{
  return dataModel_;
}
