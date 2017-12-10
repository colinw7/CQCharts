#include <CQChartsGnuData.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQGnuDataModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsGnuData::
CQChartsGnuData(CQCharts *charts) :
 CQChartsModelFilter(), charts_(charts)
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

QVariant
CQChartsGnuData::
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
