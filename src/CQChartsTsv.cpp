#include <CQChartsTsv.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQTsvModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsTsv::
CQChartsTsv(CQCharts *charts) :
 CQChartsModelFilter(), charts_(charts)
{
  tsvModel_ = new CQTsvModel;

  exprModel_ = new CQExprModel(tsvModel_);

  setSourceModel(exprModel_);
}

CQChartsTsv::
~CQChartsTsv()
{
  delete exprModel_;
  delete tsvModel_;
}

void
CQChartsTsv::
setCommentHeader(bool b)
{
  tsvModel_->setCommentHeader(b);
}

void
CQChartsTsv::
setFirstLineHeader(bool b)
{
  tsvModel_->setFirstLineHeader(b);
}

bool
CQChartsTsv::
load(const QString &filename)
{
  return tsvModel_->load(filename);
}

QVariant
CQChartsTsv::
data(const QModelIndex &index, int role) const
{
  QVariant var = CQChartsModelFilter::data(index, role);

  if (role == Qt::EditRole && ! var.isValid())
    var = CQChartsModelFilter::data(index, Qt::DisplayRole);

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
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
