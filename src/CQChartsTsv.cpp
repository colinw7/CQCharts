#include <CQChartsTsv.h>
#include <CQCharts.h>
#include <CQTsvModel.h>

CQChartsTsv::
CQChartsTsv(CQCharts *charts) :
 QSortFilterProxyModel(), CQChartsModelColumn(charts), charts_(charts)
{
  tsvModel_ = new CQTsvModel;

  setSourceModel(tsvModel_);
}

CQChartsTsv::
~CQChartsTsv()
{
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

int
CQChartsTsv::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsTsv::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

bool
CQChartsTsv::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (role == CQCharts::Role::ColumnType) {
    setColumnType(section, value.toString());
    return true;
  }

  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsTsv::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return QVariant(columnType(section));

  return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant
CQChartsTsv::
data(const QModelIndex &index, int role) const
{
  QVariant var = QSortFilterProxyModel::data(index, role);

  if (role == Qt::UserRole && ! var.isValid())
    var = QSortFilterProxyModel::data(index, Qt::DisplayRole);

  if (role == Qt::DisplayRole || role == Qt::UserRole) {
    if (! index.isValid())
      return QVariant();

    assert(index.model() == this);

    QModelIndex index1 = QSortFilterProxyModel::mapToSource(index);

    assert(index.column() == index1.column());

    if (role == Qt::DisplayRole)
      return columnDisplayData(index1.column(), var);
    else
      return columnUserData(index1.column(), var);
  }

  return var;
}

QModelIndex
CQChartsTsv::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsTsv::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
