#include <CQChartsCsv.h>
#include <CQChartsColumn.h>
#include <CQCharts.h>
#include <CQCsvModel.h>
#include <CQExprModel.h>
#include <cassert>

CQChartsCsv::
CQChartsCsv(CQCharts *charts) :
 CQChartsModelFilter(), charts_(charts)
{
  csvModel_ = new CQCsvModel;

  exprModel_ = new CQExprModel(csvModel_);

  setSourceModel(exprModel_);
}

CQChartsCsv::
~CQChartsCsv()
{
  delete exprModel_;
  delete csvModel_;
}

void
CQChartsCsv::
setCommentHeader(bool b)
{
  csvModel_->setCommentHeader(b);
}

void
CQChartsCsv::
setFirstLineHeader(bool b)
{
  csvModel_->setFirstLineHeader(b);
}

bool
CQChartsCsv::
load(const QString &filename)
{
  return csvModel_->load(filename);
}

QVariant
CQChartsCsv::
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
