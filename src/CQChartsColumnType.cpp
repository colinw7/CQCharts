#include <CQChartsColumnType.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>

CQChartsColumnTypeMgr::
CQChartsColumnTypeMgr(CQCharts *charts) :
 charts_(charts)
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

  CQChartsColumnUtil::decodeType(typeStr, baseTypeName, nameValues);

  CQBaseModel::Type baseType = CQBaseModel::nameType(baseTypeName);

  CQChartsColumnType *baseTypeData = getType(baseType);

  return baseTypeData;
}

QString
CQChartsColumnTypeMgr::
encodeTypeData(CQBaseModel::Type type, const CQChartsNameValues &nameValues) const
{
  QString lstr = CQBaseModel::typeName(type);
  QString rstr = CQChartsColumnUtil::encodeNameValues(nameValues);

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

// convert variant into user data
QVariant
CQChartsColumnTypeMgr::
getUserData(QAbstractItemModel *model, const CQChartsColumn &column, const QVariant &var) const
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

// convert variant into display data
QVariant
CQChartsColumnTypeMgr::
getDisplayData(QAbstractItemModel *model, const CQChartsColumn &column, const QVariant &var) const
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
getModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                   CQBaseModel::Type &type, CQChartsNameValues &nameValues) const
{
  if (column.type() != CQChartsColumn::Type::DATA) {
    type = CQBaseModel::Type::STRING;
    return true;
  }

  int role = static_cast<int>(CQBaseModel::Role::Type);

  bool ok;

  QVariant var1 = CQChartsUtil::modelHeaderValue(model, column, role, ok);

  if (! var1.isValid())
    return false;

  // validate ?
  type = static_cast<CQBaseModel::Type>(var1.toInt());

  if (type == CQBaseModel::Type::NONE)
    return false;

  //---

  int vrole = static_cast<int>(CQBaseModel::Role::TypeValues);

  QVariant var2 = CQChartsUtil::modelHeaderValue(model, column, vrole, ok);

  if (var2.isValid()) {
    QString str2;

    CQChartsUtil::variantToString(var2, str2);

    (void) CQChartsColumnUtil::decodeNameValues(str2, nameValues);
  }

  return true;
}

bool
CQChartsColumnTypeMgr::
setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                   CQBaseModel::Type type, const CQChartsNameValues &nameValues)
{
  int role = static_cast<int>(CQBaseModel::Role::Type);

  bool rc1 = CQChartsUtil::setModelHeaderValue(model, column, static_cast<int>(type), role);

  //---

  int vrole = static_cast<int>(CQBaseModel::Role::TypeValues);

  QString str = CQChartsColumnUtil::encodeNameValues(nameValues);

  bool rc2 = CQChartsUtil::setModelHeaderValue(model, column, str, vrole);

  //---

  if (rc1 || rc2) {
    CQChartsModelData *modelData = charts_->getModelData(model);

    if (modelData)
      charts_->emitModelTypeChanged(modelData->ind());
  }

  return rc1 && rc2;
}

//------

#if 0
bool
CQChartsColumn::
decodeType(QString &baseType, CQChartsNameValues &nameValues) const
{
  return CQChartsColumnUtil::decodeType(type_, baseType, nameValues);
}
#endif

//------

namespace CQChartsColumnUtil {

bool decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues) {
  int pos = type.indexOf(":");

  if (pos < 0) {
    baseType = type;

    return true;
  }

  baseType = type.mid(0, pos);

  QString rhs = type.mid(pos + 1);

  return decodeNameValues(rhs, nameValues);
}

bool decodeNameValues(const QString &str, CQChartsNameValues &nameValues) {
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

QString encodeNameValues(const CQChartsNameValues &nameValues) {
  QString str;

  for (const auto &nameValue : nameValues) {
    if (str.length())
      str += ",";

    str += nameValue.first + '=' + nameValue.second;
  }

  return str;
}

}
