#include <CQChartsGnuData.h>
#include <CQCharts.h>
#include <CQGnuDataModel.h>

CQChartsGnuData::
CQChartsGnuData(CQCharts *charts) :
 QSortFilterProxyModel(), CQChartsModelColumn(charts), charts_(charts)
{
  dataModel_ = new CQGnuDataModel;

  setSourceModel(dataModel_);
}

CQChartsGnuData::
~CQChartsGnuData()
{
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

int
CQChartsGnuData::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsGnuData::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

bool
CQChartsGnuData::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (role == CQCharts::Role::ColumnType) {
    setColumnType(section, value.toString());
    return true;
  }

  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsGnuData::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return QVariant(columnType(section));

  return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant
CQChartsGnuData::
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
CQChartsGnuData::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsGnuData::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
