#include <CQChartsModelColumn.h>
#include <CQCharts.h>

CQChartsModelColumn::
CQChartsModelColumn(CQCharts *charts) :
 charts_(charts)
{
}

CQChartsModelColumn::
~CQChartsModelColumn()
{
}

QVariant
CQChartsModelColumn::
columnUserData(int column, const QVariant &var) const
{
  QString type = columnType(column);

  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = charts_->columnTypeMgr()->decodeTypeData(type, nameValues);

  if (! typeData)
    return var;

  return typeData->userData(var, nameValues);
}

QVariant
CQChartsModelColumn::
columnDisplayData(int column, const QVariant &var) const
{
  QString type = columnType(column);

  CQChartsNameValues nameValues;

  CQChartsColumnType *typeData = charts_->columnTypeMgr()->decodeTypeData(type, nameValues);

  if (! typeData)
    return var;

  QVariant var1 = typeData->userData(var, nameValues);

  return typeData->dataName(var1, nameValues);
}

QString
CQChartsModelColumn::
columnType(int col) const
{
  if (col < 0)
    return QString();

  if (col >= int(columns_.size())) {
    CQChartsModelColumn *th = const_cast<CQChartsModelColumn *>(this);

    while (col >= int(th->columns_.size()))
      th->columns_.emplace_back("");
  }

  return columns_[col].type();
}

bool
CQChartsModelColumn::
setColumnType(int col, const QString &type)
{
  if (col < 0)
    return false;

  QString            baseType;
  CQChartsNameValues nameValues;

  CQChartsColumn::decodeType(type, baseType, nameValues);

  CQChartsColumnType *typeData = charts_->columnTypeMgr()->getType(baseType);

  if (! typeData)
    return false;

  while (col >= int(columns_.size()))
    columns_.emplace_back("");

  columns_[col].setType(type);

  return true;
}
