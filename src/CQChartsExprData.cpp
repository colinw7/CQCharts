#include <CQChartsExprData.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsExprData::
CQChartsExprData(CQCharts *charts, int nc, int nr) :
 CQChartsModelFilter(), charts_(charts)
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

QVariant
CQChartsExprData::
data(const QModelIndex &index, int role) const
{
  QVariant var = CQChartsModelFilter::data(index, role);

  if (role == Qt::UserRole && ! var.isValid())
    var = CQChartsModelFilter::data(index, Qt::DisplayRole);

  if (role == Qt::DisplayRole || role == Qt::UserRole) {
    if (! index.isValid())
      return QVariant();

    assert(index.model() == this);

    QModelIndex index1 = CQChartsModelFilter::mapToSource(index);

    assert(index.column() == index1.column());

    CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

    if (role == Qt::DisplayRole)
      return columnTypeMgr->getDisplayData(this, index1.column(), var);
    else
      return columnTypeMgr->getUserData(this, index1.column(), var);
  }

  return var;
}
