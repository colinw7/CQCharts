#include <CQChartsJson.h>
#include <CQCharts.h>
#include <CQJsonModel.h>

CQChartsJson::
CQChartsJson(CQCharts *charts) :
 QSortFilterProxyModel(), charts_(charts)
{
  jsonModel_ = new CQJsonModel;

  setSourceModel(jsonModel_);
}

CQChartsJson::
~CQChartsJson()
{
  delete jsonModel_;
}

bool
CQChartsJson::
load(const QString &filename)
{
  return jsonModel_->load(filename);
}

bool
CQChartsJson::
isHierarchical() const
{
  return jsonModel_->isHierarchical();
}

int
CQChartsJson::
columnCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::columnCount(parent);
}

int
CQChartsJson::
rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

bool
CQChartsJson::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (role == CQCharts::Role::ColumnType) {
    setColumnType(section, value.toString());
    return true;
  }

  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsJson::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == CQCharts::Role::ColumnType)
    return QVariant(columnType(section));

  return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant
CQChartsJson::
data(const QModelIndex &index, int role) const
{
  QVariant var = QSortFilterProxyModel::data(index, role);

  if (role == Qt::DisplayRole) {
    if (! index.isValid())
      return QVariant();

    assert(index.model() == this);

    QModelIndex index1 = QSortFilterProxyModel::mapToSource(index);

    assert(index.column() == index1.column());

    QString type = columnType(index1.column());

    CQChartsNameValues nameValues;

    CQChartsColumnType *typeData = charts_->columnTypeMgr()->decodeTypeData(type, nameValues);

    if (typeData)
      return typeData->userData(var, nameValues);
  }

  return var;
}

QModelIndex
CQChartsJson::
parent(const QModelIndex &index) const
{
  return QSortFilterProxyModel::parent(index);
}

Qt::ItemFlags
CQChartsJson::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//---

QString
CQChartsJson::
columnType(int col) const
{
  int n = columnCount();

  if (col < 0 || col >= n)
    return QString();

  if (n > int(columns_.size())) {
    CQChartsJson *th = const_cast<CQChartsJson *>(this);

    while (n > int(th->columns_.size()))
      th->columns_.emplace_back("");
  }

  return columns_[col].type();
}

bool
CQChartsJson::
setColumnType(int col, const QString &type)
{
  int n = columnCount();

  if (col < 0 || col >= n)
    return false;

  QString            baseType;
  CQChartsNameValues nameValues;

  CQChartsColumn::decodeType(type, baseType, nameValues);

  CQChartsColumnType *typeData = charts_->columnTypeMgr()->getType(baseType);

  if (! typeData)
    return false;

  while (n > int(columns_.size()))
    columns_.emplace_back("");

  columns_[col].setType(type);

  return true;
}
