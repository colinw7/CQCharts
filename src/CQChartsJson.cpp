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
  return QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
}

QVariant
CQChartsJson::
headerData(int section, Qt::Orientation orientation, int role) const
{
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

    CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

    QString typeStr = columnType(index1.column());

    CQChartsNameValues nameValues;

    CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

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
  return QSortFilterProxyModel::flags(index);
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
setColumnType(int col, const QString &typeStr)
{
  int n = columnCount();

  if (col < 0 || col >= n)
    return false;

  QString            baseTypeStr;
  CQChartsNameValues nameValues;

  CQChartsColumn::decodeType(typeStr, baseTypeStr, nameValues);

  CQBaseModel::Type baseType = CQBaseModel::nameType(baseTypeStr);

  CQChartsColumnType *typeData = charts_->columnTypeMgr()->getType(baseType);

  if (! typeData)
    return false;

  while (n > int(columns_.size()))
    columns_.emplace_back("");

  columns_[col].setType(typeStr);

  return true;
}
