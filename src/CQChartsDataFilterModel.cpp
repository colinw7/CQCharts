#include <CQChartsDataFilterModel.h>
#include <CQChartsExprModel.h>
#include <CQCharts.h>
#include <CQDataModel.h>

CQChartsDataFilterModel::
CQChartsDataFilterModel(CQCharts *charts, int nc, int nr, bool fill) :
 CQChartsModelFilter(charts)
{
  QModelIndex parent; // OK ?

  dataModel_ = new CQDataModel(nc, nr);

  if (fill) {
    for (int r = 0; r < nr; ++r) {
      for (int c = 0; c < nc; ++c) {
        QModelIndex ind = dataModel_->index(r, c, parent);

        dataModel_->setData(ind, QVariant(r*nc + c));
      }
    }
  }

  exprModel_ = new CQChartsExprModel(charts_, dataModel_);

  setSourceModel(exprModel_);
}

CQChartsDataFilterModel::
~CQChartsDataFilterModel()
{
  delete dataModel_;
}

QAbstractItemModel *
CQChartsDataFilterModel::
baseModel() const
{
  return dataModel_;
}
