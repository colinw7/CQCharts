#include <CQChartsColumn.h>

CQChartsColumnTypeMgr::
CQChartsColumnTypeMgr()
{
}

CQChartsColumnTypeMgr::
~CQChartsColumnTypeMgr()
{
  for (auto &typeData : typeData_)
    delete typeData.second;
}

void
CQChartsColumnTypeMgr::
addType(CQBaseModel::Type type, CQChartsColumnType *data)
{
  typeData_[type] = data;
}

CQChartsColumnType *
CQChartsColumnTypeMgr::
decodeTypeData(const QString &typeStr, CQChartsNameValues &nameValues) const
{
  QString baseTypeName;

  CQChartsColumn::decodeType(typeStr, baseTypeName, nameValues);

  CQBaseModel::Type baseType = CQBaseModel::nameType(baseTypeName);

  CQChartsColumnType *baseTypeData = getType(baseType);

  return baseTypeData;
}

QString
CQChartsColumnTypeMgr::
encodeTypeData(CQBaseModel::Type type, const CQChartsNameValues &nameValues) const
{
  QString lstr = CQBaseModel::typeName(type);
  QString rstr = CQChartsColumn::encodeNameValues(nameValues);

  if (! rstr.length())
    return lstr;

  return lstr + ":" + rstr;
}

CQChartsColumnType *
CQChartsColumnTypeMgr::
getType(CQBaseModel::Type type) const
{
  auto p = typeData_.find(type);

  if (p == typeData_.end())
    return nullptr;

  return (*p).second;
}

QVariant
CQChartsColumnTypeMgr::
getUserData(const QAbstractItemModel *model, int column, const QVariant &var) const
{
  CQBaseModel::Type  type;
  CQChartsNameValues nameValues;

  if (! getModelColumnType(model, column, type, nameValues))
    return var;

  CQChartsColumnType *typeData = getType(type);

  if (! typeData)
    return var;

  return typeData->userData(var, nameValues);
}

QVariant
CQChartsColumnTypeMgr::
getDisplayData(const QAbstractItemModel *model, int column, const QVariant &var) const
{
  CQBaseModel::Type  type;
  CQChartsNameValues nameValues;

  if (! getModelColumnType(model, column, type, nameValues))
    return var;

  CQChartsColumnType *typeData = getType(type);

  if (! typeData)
    return var;

  QVariant var1 = typeData->userData(var, nameValues);

  if (! var1.isValid())
    return var;

  return typeData->dataName(var1, nameValues);
}

bool
CQChartsColumnTypeMgr::
getModelColumnType(const QAbstractItemModel *model, int column, CQBaseModel::Type &type,
                   CQChartsNameValues &nameValues) const
{
  QVariant var1 = model->headerData(column, Qt::Horizontal,
                    static_cast<int>(CQBaseModel::Role::Type));

  if (! var1.isValid())
    return false;

  // validate ?
  type = static_cast<CQBaseModel::Type>(var1.toInt());

  //---

  QVariant var2 = model->headerData(column, Qt::Horizontal,
                    static_cast<int>(CQBaseModel::Role::TypeValues));

  if (var2.isValid()) {
    QString str = var2.toString();

    (void) CQChartsColumn::decodeNameValues(str, nameValues);
  }

  return true;
}

bool
CQChartsColumnTypeMgr::
setModelColumnType(QAbstractItemModel *model, int column, CQBaseModel::Type type,
                   const CQChartsNameValues &nameValues)
{
  bool rc1 = model->setHeaderData(column, Qt::Horizontal, static_cast<int>(type),
              static_cast<int>(CQBaseModel::Role::Type));

  //---

  QString str = CQChartsColumn::encodeNameValues(nameValues);

  bool rc2 = model->setHeaderData(column, Qt::Horizontal, str,
              static_cast<int>(CQBaseModel::Role::TypeValues));

  return rc1 && rc2;
}

//------

bool
CQChartsColumn::
decodeType(QString &baseType, CQChartsNameValues &nameValues) const
{
  return decodeType(type_, baseType, nameValues);
}

bool
CQChartsColumn::
decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues)
{
  int pos = type.indexOf(":");

  if (pos < 0) {
    baseType = type;

    return true;
  }

  baseType = type.mid(0, pos);

  QString rhs = type.mid(pos + 1);

  return decodeNameValues(rhs, nameValues);
}

bool
CQChartsColumn::
decodeNameValues(const QString &str, CQChartsNameValues &nameValues)
{
  QStringList strs = str.split(",", QString::SkipEmptyParts);

  for (int i = 0; i < strs.length(); ++i) {
    int pos1 = strs[i].indexOf("=");

    if (pos1 < 1) {
      nameValues[strs[i]] = "1";
    }
    else {
      QString name  = strs[i].mid(0, pos1 ).simplified();
      QString value = strs[i].mid(pos1 + 1).simplified();

      nameValues[name] = value;
    }
  }

  return true;
}

QString
CQChartsColumn::
encodeNameValues(const CQChartsNameValues &nameValues)
{
  QString str;

  for (const auto &nameValue : nameValues) {
    if (str.length())
      str += ",";

    str += nameValue.first + '=' + nameValue.second;
  }

  return str;
}
