#include <CQChartsColumnType.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsPolygonList.h>
#include <CQChartsConnectionList.h>
#include <CQChartsNamePair.h>
#include <CQChartsSymbol.h>
#include <CQChartsGradientPalette.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQCharts.h>
#include <CQChartsTypes.h>
#include <CQBaseModel.h>
#include <CQStrParse.h>

//------

namespace CQChartsColumnUtil {

QStringList splitNameValues(const QString &str) {
  QStringList words;

  //words = str.split(",", QString::SkipEmptyParts);

  CQStrParse parse(str);

  QString word;

  while (! parse.eof()) {
    if      (parse.isChar('\\')) {
      parse.skipChar();

      if (! parse.eof())
        word += parse.getChar();
    }
    else if (parse.isChar(',')) {
      parse.skipChar();

      if (word.length())
        words.push_back(word);

      word = "";
    }
    else
      word += parse.getChar();
  }

  if (word.length())
    words.push_back(word);

  return words;
}

bool decodeNameValues(const QString &str, CQChartsNameValues &nameValues) {
  if (! str.length())
    return true;

  QStringList strs = splitNameValues(str);

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

QString encodeNameValues(const CQChartsNameValues &nameValues) {
  QString str;

  if (nameValues.empty())
    return str;

  for (const auto &nameValue : nameValues) {
    if (str.length())
      str += ",";

    str += nameValue.first + '=';

    QString value = nameValue.second.toString();

    for (int i = 0; i < value.length(); ++i) {
      if (value[i] == ',')
        str += '\\';

      str += value[i];
    }
  }

  return str;
}

bool nameValueString(const CQChartsNameValues &nameValues, const QString &name, QString &value) {
  auto p = nameValues.find(name);

  if (p == nameValues.end())
    return false;

  value = (*p).second.toString();

  return true;
}

bool nameValueInteger(const CQChartsNameValues &nameValues, const QString &name, int &value) {
  auto p = nameValues.find(name);

  if (p == nameValues.end())
    return false;

  bool ok;

  value = CQChartsVariant::toInt((*p).second, ok);

  if (! ok)
    return false;

  return true;
}

bool nameValueReal(const CQChartsNameValues &nameValues, const QString &name, double &value) {
  auto p = nameValues.find(name);

  if (p == nameValues.end())
    return false;

  bool ok;

  value = CQChartsVariant::toReal((*p).second, ok);

  if (! ok)
    return false;

  return true;
}

bool nameValueBool(const CQChartsNameValues &nameValues, const QString &name, bool &value) {
  auto p = nameValues.find(name);

  if (p == nameValues.end())
    return false;

  bool ok;

  value = CQChartsVariant::toBool((*p).second, ok);

  if (! ok)
    return false;

  return true;
}

int varInteger(const QVariant &var, int def) {
  bool ok = false;

  int i = var.toInt(&ok);

  return (ok ? i : def);
}

double varReal(const QVariant &var, double def) {
  bool ok = false;

  double r = var.toDouble(&ok);

  return (ok ? r : def);
}

}

//------

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
typeNames(QStringList &names) const
{
  using IndString = std::map<int,QString>;

  IndString indString;

  for (auto &typeData : typeData_) {
    QString name = CQBaseModel::typeName(typeData.first);

    indString[typeData.second->ind()] = name;
  }

  assert(indString.size() == typeData_.size());

  for (const auto &p : indString) {
    names.push_back(p.second);
  }
}

void
CQChartsColumnTypeMgr::
addType(Type type, CQChartsColumnType *data)
{
  data->setInd(typeData_.size());

  typeData_[type] = data;
}

const CQChartsColumnType *
CQChartsColumnTypeMgr::
decodeTypeData(const QString &typeStr, CQChartsNameValues &nameValues) const
{
  QString baseTypeName;

  CQChartsColumnUtil::decodeType(typeStr, baseTypeName, nameValues);

  Type baseType = CQBaseModel::nameType(baseTypeName);

  const CQChartsColumnType *baseTypeData = getType(baseType);

  return baseTypeData;
}

QString
CQChartsColumnTypeMgr::
encodeTypeData(Type type, const CQChartsNameValues &nameValues) const
{
  QString lstr = CQBaseModel::typeName(type);
  QString rstr = CQChartsColumnUtil::encodeNameValues(nameValues);

  if (! rstr.length())
    return lstr;

  return lstr + ":" + rstr;
}

const CQChartsColumnType *
CQChartsColumnTypeMgr::
getType(Type type) const
{
  auto p = typeData_.find(type);

  if (p == typeData_.end())
    return nullptr;

  return (*p).second;
}

// convert variant into user data
QVariant
CQChartsColumnTypeMgr::
getUserData(const QAbstractItemModel *model, const CQChartsColumn &column,
            const QVariant &var, bool &converted) const
{
  converted = false;

  const TypeCacheData *typeCacheData = nullptr;

  if (! getModelColumnTypeData(model, column, typeCacheData))
    return var;

  QVariant var1 = typeCacheData->typeData->userData(charts_, model, column, var,
                                                    typeCacheData->nameValues, converted);

  return var1;
}

// convert variant into display data
QVariant
CQChartsColumnTypeMgr::
getDisplayData(const QAbstractItemModel *model, const CQChartsColumn &column,
               const QVariant &var, bool &converted) const
{
  converted = false;

  const TypeCacheData *typeCacheData = nullptr;

  if (! getModelColumnTypeData(model, column, typeCacheData))
    return var;

  QVariant var1 = typeCacheData->typeData->userData(charts_, model, column, var,
                                                    typeCacheData->nameValues, converted);

  if (! var1.isValid())
    return var;

  QVariant var2 = typeCacheData->typeData->dataName(charts_, model, column, var1,
                                                    typeCacheData->nameValues, converted);

  return var2;
}

bool
CQChartsColumnTypeMgr::
getModelColumnTypeData(const QAbstractItemModel *model, const CQChartsColumn &column,
                       const TypeCacheData* &typeCacheData) const
{
  const CacheData &cacheData = getModelCacheData(model);

  //---

  bool caching = (cacheData.depth > 0);

  if (caching) {
    // get cached column data
    auto pc = cacheData.columnTypeCache.find(column);

    if (pc == cacheData.columnTypeCache.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      CacheData &cacheData1 = const_cast<CacheData &>(cacheData);

      auto pc1 = cacheData1.columnTypeCache.find(column);

      if (pc1 == cacheData1.columnTypeCache.end()) {
        TypeCacheData typeCacheData1;

        if (getModelColumnType(model, column, typeCacheData1.type, typeCacheData1.baseType,
                               typeCacheData1.nameValues)) {
          typeCacheData1.typeData = getType(typeCacheData1.type);

          if (typeCacheData1.typeData)
            typeCacheData1.valid = true;
        }

        pc1 = cacheData1.columnTypeCache.insert(pc1,
          ColumnTypeCache::value_type(column, typeCacheData1));
      }

      pc = cacheData.columnTypeCache.find(column);
    }

    typeCacheData = &(*pc).second;
  }
  else {
    TypeCacheData &typeCacheData1 = const_cast<TypeCacheData &>(cacheData.typeCacheData);

    if (getModelColumnType(model, column, typeCacheData1.type, typeCacheData1.baseType,
                           typeCacheData1.nameValues)) {
      typeCacheData1.typeData = getType(typeCacheData1.type);

      if (typeCacheData1.typeData)
        typeCacheData1.valid = true;
    }

    typeCacheData = &typeCacheData1;
  }

  return typeCacheData->valid;
}

bool
CQChartsColumnTypeMgr::
getModelColumnType(const QAbstractItemModel *model, const CQChartsColumn &column,
                   Type &type, Type &baseType, CQChartsNameValues &nameValues) const
{
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX) {
    type     = Type::STRING;
    baseType = Type::STRING;
    return true;
  }

  //---

  int role = static_cast<int>(CQBaseModelRole::Type);

  bool ok1;

  QVariant var1 = CQChartsModelUtil::modelHeaderValue(model, column, role, ok1);

  if (! var1.isValid())
    return false;

  // validate ?
  type = static_cast<Type>(var1.toInt());

  if (type == Type::NONE) {
    baseType = Type::NONE;
    return false;
  }

  //---

  int brole = static_cast<int>(CQBaseModelRole::BaseType);

  bool ok2;

  QVariant var2 = CQChartsModelUtil::modelHeaderValue(model, column, brole, ok2);

  if (! var2.isValid())
    return false;

  // validate ?
  baseType = static_cast<Type>(var2.toInt());

  if (baseType == Type::NONE)
    baseType = type;

  //---

  int vrole = static_cast<int>(CQBaseModelRole::TypeValues);

  bool ok3;

  QVariant var3 = CQChartsModelUtil::modelHeaderValue(model, column, vrole, ok3);

  if (var3.isValid()) {
    QString str3;

    CQChartsVariant::toString(var3, str3);

    (void) CQChartsColumnUtil::decodeNameValues(str3, nameValues);
  }

  //---

  const CQChartsColumnType *typeData = getType(type);

  for (const auto &param : typeData->params()) {
    if (param.role() != vrole) {
      bool ok;

      QVariant var = CQChartsModelUtil::modelHeaderValue(model, column, param.role(), ok);

      nameValues[param.name()] = var;
    }
  }

  return true;
}

bool
CQChartsColumnTypeMgr::
setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                   Type type, const CQChartsNameValues &nameValues)
{
  bool changed = false;

  int role = static_cast<int>(CQBaseModelRole::Type);

  bool rc1 = CQChartsModelUtil::setModelHeaderValue(
               model, column, static_cast<int>(type), role);

  if (rc1)
    changed = true;

  //---

  int vrole = static_cast<int>(CQBaseModelRole::TypeValues);

  CQChartsNameValues nameValues1;

  const CQChartsColumnType *typeData = getType(type);

  for (const auto &param : typeData->params()) {
    auto p = nameValues.find(param.name());
    if (p == nameValues.end()) continue;

    if (param.role() == vrole) {
      nameValues1[param.name()] = (*p).second;
    }
    else {
      bool rc = CQChartsModelUtil::setModelHeaderValue(
                  model, column, (*p).second, param.role());

      if (rc)
        changed = true;
    }
  }

  if (! nameValues1.empty()) {
    QString str = CQChartsColumnUtil::encodeNameValues(nameValues1);

    bool rc2 = CQChartsModelUtil::setModelHeaderValue(model, column, str, vrole);

    if (rc2)
      changed = true;
  }

  //---

  if (changed) {
    CQChartsModelData *modelData = charts_->getModelData(model);

    if (modelData)
      charts_->emitModelTypeChanged(modelData->ind());
  }

  return changed;
}

void
CQChartsColumnTypeMgr::
startCache(const QAbstractItemModel *model)
{
  const CacheData &cacheData = getModelCacheData(model);

  std::unique_lock<std::mutex> lock(mutex_);

  CacheData &cacheData1 = const_cast<CacheData &>(cacheData);

  ++cacheData1.depth;
}

void
CQChartsColumnTypeMgr::
endCache(const QAbstractItemModel *model)
{
  const CacheData &cacheData = getModelCacheData(model);

  std::unique_lock<std::mutex> lock(mutex_);

  CacheData &cacheData1 = const_cast<CacheData &>(cacheData);

  assert(cacheData1.depth > 0);

  --cacheData1.depth;

  if (cacheData1.depth == 0)
    cacheData1.columnTypeCache.clear();
}

const CQChartsColumnTypeMgr::CacheData &
CQChartsColumnTypeMgr::
getModelCacheData(const QAbstractItemModel *model) const
{
  // get model data
  CQChartsModelData *modelData = charts_->getModelData(model);
  assert(modelData);

  //---

  // get cache data for model
  auto pm = modelCacheData_.find(modelData->ind());

  if (pm == modelCacheData_.end()) {
    std::unique_lock<std::mutex> lock(mutex_);

    CQChartsColumnTypeMgr *th = const_cast<CQChartsColumnTypeMgr *>(this);

    auto pm1 = th->modelCacheData_.find(modelData->ind());

    if (pm1 == modelCacheData_.end()) {
      pm1 = th->modelCacheData_.insert(pm1,
        ModelCacheData::value_type(modelData->ind(), CacheData()));
    }

    pm = modelCacheData_.find(modelData->ind());
  }

  const CacheData &cacheData = (*pm).second;

  return cacheData;
}

//------

CQChartsColumnType::
CQChartsColumnType(Type type) :
 type_(type)
{
  params_.emplace_back("key", Type::BOOLEAN, (int) CQBaseModelRole::Key, "Is Key", false);
}

QString
CQChartsColumnType::
name() const
{
  return CQBaseModel::typeName(type_);
}

bool
CQChartsColumnType::
hasParam(const QString &name) const
{
  for (const auto &param : params_)
    if (param.name() == name)
      return true;

  return false;
}

const CQChartsColumnTypeParam *
CQChartsColumnType::
getParam(const QString &name) const
{
  for (const auto &param : params_)
    if (param.name() == name)
      return &param;

  return nullptr;
}

CQChartsModelColumnDetails *
CQChartsColumnType::
columnDetails(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column) const
{
  CQChartsModelData *modelData = charts->getModelData(model);
  if (! modelData) return nullptr;

  CQChartsModelDetails *details = modelData->details();
  if (! details) return nullptr;

  return details->columnDetails(column);
}

//------

CQChartsColumnStringType::
CQChartsColumnStringType() :
 CQChartsColumnType(Type::STRING)
{
}

QVariant
CQChartsColumnStringType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::String)
    return var;

  converted = true;

  QString str;

  CQChartsVariant::toString(var, str);

  return QVariant::fromValue<QString>(str);
}

QVariant
CQChartsColumnStringType::
dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
         const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
{
  return userData(charts, model, column, var, nameValues, converted);
}

//------

CQChartsColumnBooleanType::
CQChartsColumnBooleanType() :
 CQChartsColumnType(Type::BOOLEAN)
{
}

QVariant
CQChartsColumnBooleanType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Bool)
    return var;

  converted = true;

  bool ok;

  bool b = CQChartsVariant::toBool(var, ok);

  if (! ok)
    b = var.toBool();

  return QVariant::fromValue<bool>(b);
}

QVariant
CQChartsColumnBooleanType::
dataName(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
         const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
{
  return userData(charts, model, column, var, nameValues, converted);
}

//------

CQChartsColumnRealType::
CQChartsColumnRealType() :
 CQChartsColumnType(Type::REAL)
{
  params_.emplace_back("format"      , Type::STRING, "Output Format", "");
  params_.emplace_back("format_scale", Type::REAL  , "Format Scale Factor", 1.0);

  params_.emplace_back("min", Type::REAL, (int) CQBaseModelRole::Min, "Min Value", 0.0);
  params_.emplace_back("max", Type::REAL, (int) CQBaseModelRole::Max, "Max Value", 1.0);
}

QVariant
CQChartsColumnRealType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Double)
    return var;

  bool ok;

  double r = CQChartsVariant::toReal(var, ok);

  if (! ok)
    return var;

  converted = true;

  return QVariant::fromValue<double>(r);
}

// data variant to output variant (string) for display
QVariant
CQChartsColumnRealType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get real value
  double r = 0.0;

  if (var.type() == QVariant::Double) {
    r = var.value<double>();
  }
  else {
    bool ok;

    r = CQChartsVariant::toReal(var, ok);

    if (! ok)
      return CQChartsVariant::toString(var, ok);

    converted = true;
  }

  //---

  // get optional format for real
  QString format;

  if (! CQChartsColumnUtil::nameValueString(nameValues, "format", format))
    return CQChartsUtil::toString(r);

  //---

  // get scale factor to support units suffix in format
  double scale;

  if (CQChartsColumnUtil::nameValueReal(nameValues, "format_scale", scale))
    r *= scale;

  //---

  // convert value using format
  return CQChartsUtil::toString(r, format);
}

QVariant
CQChartsColumnRealType::
minValue(const CQChartsNameValues &nameValues) const
{
  double r;

  if (! rmin(nameValues, r))
    return QVariant();

  return QVariant(r);
}

QVariant
CQChartsColumnRealType::
maxValue(const CQChartsNameValues &nameValues) const
{
  double r;

  if (! rmax(nameValues, r))
    return QVariant();

  return QVariant(r);
}

bool
CQChartsColumnRealType::
rmin(const CQChartsNameValues &nameValues, double &r) const
{
  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", r))
    return false;

  return true;
}

bool
CQChartsColumnRealType::
rmax(const CQChartsNameValues &nameValues, double &r) const
{
  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", r))
    return false;

  return true;
}

//------

CQChartsColumnIntegerType::
CQChartsColumnIntegerType() :
 CQChartsColumnType(Type::INTEGER)
{
  params_.emplace_back("format", Type::INTEGER, "Output Format", "");

  params_.emplace_back("min", Type::INTEGER, (int) CQBaseModelRole::Min, "Min Value",   0);
  params_.emplace_back("max", Type::INTEGER, (int) CQBaseModelRole::Max, "Max Value", 100);
}

QVariant
CQChartsColumnIntegerType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Int)
    return var;

  bool ok;

  long l = CQChartsVariant::toInt(var, ok);

  if (! ok)
    return var;

  converted = true;

  return QVariant::fromValue<int>(l);
}

// data variant to output variant (string) for display
QVariant
CQChartsColumnIntegerType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get integer value
  long l = 0;

  if (var.type() == QVariant::Int) {
    l = var.value<int>();
  }
  else {
    bool ok;

    l = CQChartsVariant::toInt(var, ok);

    if (! ok)
      return CQChartsVariant::toString(var, ok);

    converted = true;
  }

  converted = true;

  //---

  // get optional format for real
  QString format;

  if (! CQChartsColumnUtil::nameValueString(nameValues, "format", format))
    return CQChartsUtil::toString(l);

  //---

  // convert value using format
  return CQChartsUtil::toString(l, format);
}

//------

CQChartsColumnTimeType::
CQChartsColumnTimeType() :
 CQChartsColumnType(Type::TIME)
{
  params_.emplace_back("format" , Type::STRING, "Input/Output Format", "");
  params_.emplace_back("iformat", Type::STRING, "Input Format", "");
  params_.emplace_back("oformat", Type::STRING, "Output Format", "");
}

QVariant
CQChartsColumnTimeType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Double)
    return var;

  // use format string to convert model (input) string to time (double)
  // TODO: assert if no format ?
  QString fmt = getIFormat(nameValues);

  if (! fmt.length())
    return var;

  double t;

  if (! CQChartsUtil::stringToTime(fmt, var.toString(), t))
    return var;

  converted = true;

  return QVariant::fromValue<double>(t);
}

QVariant
CQChartsColumnTimeType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid())
    return var;

  // get time value (double)
  bool ok;

  double t = CQChartsVariant::toReal(var, ok);

  if (! ok)
    return var;

  //---

  converted = true;

  // use format string to convert time (double) to model (output) string
  // TODO: assert if no format ?
  QString fmt = getOFormat(nameValues);

  if (! fmt.length())
    return CQChartsUtil::toString(t);

  return CQChartsUtil::timeToString(fmt, t);
}

QString
CQChartsColumnTimeType::
getIFormat(const CQChartsNameValues &nameValues) const
{
  QString format;

  if (CQChartsColumnUtil::nameValueString(nameValues, "iformat", format))
    return format;

  if (CQChartsColumnUtil::nameValueString(nameValues, "format", format))
    return format;

  return "";
}

QString
CQChartsColumnTimeType::
getOFormat(const CQChartsNameValues &nameValues) const
{
  QString format;

  if (CQChartsColumnUtil::nameValueString(nameValues, "oformat", format))
    return format;

  if (CQChartsColumnUtil::nameValueString(nameValues, "format", format))
    return format;

  return "";
}

QVariant
CQChartsColumnTimeType::
indexVar(const QVariant &var, const QString &ind) const
{
  if (! var.isValid())
    return var;

  // get time value (double)
  bool ok;

  double t = CQChartsVariant::toReal(var, ok);

  if (! ok)
    return var;

  //---

  return CQChartsUtil::timeToString(ind, t);
}

CQChartsColumnTimeType::Type
CQChartsColumnTimeType::
indexType(const QString &) const
{
  return Type::STRING;
}

//------

CQChartsColumnRectType::
CQChartsColumnRectType() :
 CQChartsColumnType(Type::RECT)
{
}

QVariant
CQChartsColumnRectType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::RectF)
    return var;

  converted = true;

  QRectF r;

  if (var.type() == QVariant::Rect) {
    r = var.value<QRect>();

    return QVariant::fromValue<QRectF>(r);
  }

  QString str = var.toString();

  (void) CQChartsUtil::stringToRect(str, r);

  return QVariant::fromValue<QRectF>(r);
}

QVariant
CQChartsColumnRectType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.type() == QVariant::RectF) {
    QRectF r = var.value<QRectF>();

    return CQChartsUtil::rectToString(r);
  }

  return var; // TODO: other var formats
}

//------

CQChartsColumnPolygonType::
CQChartsColumnPolygonType() :
 CQChartsColumnType(Type::POLYGON)
{
}

QVariant
CQChartsColumnPolygonType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::PolygonF)
    return var;

  converted = true;

  QPolygonF poly;

  if (var.type() == QVariant::Polygon) {
    poly = var.value<QPolygon>();

    return QVariant::fromValue<QPolygonF>(poly);
  }

  QString str = var.toString();

  (void) CQChartsUtil::stringToPolygon(str, poly);

  return QVariant::fromValue<QPolygonF>(poly);
}

QVariant
CQChartsColumnPolygonType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.type() == QVariant::PolygonF) {
    QPolygonF poly = var.value<QPolygonF>();

    return CQChartsUtil::polygonToString(poly);
  }

  return var; // TODO: other var formats
}

//------

CQChartsColumnPolygonListType::
CQChartsColumnPolygonListType() :
 CQChartsColumnType(Type::POLYGON_LIST)
{
}

QVariant
CQChartsColumnPolygonListType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  if (isPolygonListVariant(var))
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsPolygonList polyList(str);

  return QVariant::fromValue<CQChartsPolygonList>(polyList);
}

QVariant
CQChartsColumnPolygonListType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isPolygonListVariant(var)) {
    CQChartsPolygonList polyList = var.value<CQChartsPolygonList>();

    return polyList.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnPolygonListType::
isPolygonListVariant(const QVariant &var) const
{
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsPolygonList::metaTypeId)
      return true;
  }

  return false;
}

//------

CQChartsColumnConnectionListType::
CQChartsColumnConnectionListType() :
 CQChartsColumnType(Type::CONNECTION_LIST)
{
}

QVariant
CQChartsColumnConnectionListType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  if (isVariantType(var))
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsConnectionList connectionList(str);

  return QVariant::fromValue<CQChartsConnectionList>(connectionList);
}

QVariant
CQChartsColumnConnectionListType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isVariantType(var)) {
    CQChartsConnectionList connectionList = var.value<CQChartsConnectionList>();

    return connectionList.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnConnectionListType::
isVariantType(const QVariant &var) const
{
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsConnectionList::metaTypeId)
      return true;
  }

  return false;
}

//------

CQChartsColumnNamePairType::
CQChartsColumnNamePairType() :
 CQChartsColumnType(Type::NAME_PAIR)
{
}

QVariant
CQChartsColumnNamePairType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  if (isNamePairVariant(var))
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsNamePair namePair(str);

  if (! namePair.isValid())
    return QVariant();

  return QVariant::fromValue<CQChartsNamePair>(namePair);
}

QVariant
CQChartsColumnNamePairType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isNamePairVariant(var)) {
    CQChartsNamePair namePair = var.value<CQChartsNamePair>();

    if (! namePair.isValid())
      return "";

    return namePair.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnNamePairType::
isNamePairVariant(const QVariant &var) const
{
  if (! var.isValid())
    return false;

  if (var.type() == QVariant::UserType) {
    if (var.userType() == CQChartsNamePair::metaTypeId)
      return true;
  }

  return false;
}

//------

CQChartsColumnPathType::
CQChartsColumnPathType() :
 CQChartsColumnType(Type::PATH)
{
}

QVariant
CQChartsColumnPathType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.userType() == CQChartsPath::metaTypeId)
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsPath path;

  (void) CQChartsUtil::stringToPath(str, path);

  return QVariant::fromValue<CQChartsPath>(path);
}

QVariant
CQChartsColumnPathType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.userType() == CQChartsPath::metaTypeId) {
    CQChartsPath path = var.value<CQChartsPath>();

    return CQChartsUtil::pathToString(path);
  }

  return var; // TODO: other var formats
}

//------

CQChartsColumnStyleType::
CQChartsColumnStyleType() :
 CQChartsColumnType(Type::STYLE)
{
}

QVariant
CQChartsColumnStyleType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.userType() == CQChartsStyle::metaTypeId)
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsStyle style;

  (void) CQChartsUtil::stringToStyle(str, style);

  return QVariant::fromValue<CQChartsStyle>(style);
}

QVariant
CQChartsColumnStyleType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.userType() == CQChartsStyle::metaTypeId) {
    CQChartsStyle style = var.value<CQChartsStyle>();

    return CQChartsUtil::styleToString(style);
  }

  return var; // TODO: other var formats
}

//------

CQChartsColumnColorType::
CQChartsColumnColorType() :
 CQChartsColumnType(Type::COLOR)
{
  // map from model value to 0.0 -> 1.0
  params_.emplace_back("mapped", Type::BOOLEAN, "Value Mapped", false);

  params_.emplace_back("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0);
  params_.emplace_back("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0);

  // get color from named palette
  params_.emplace_back("palette", Type::STRING, "Palette", "");
}

QVariant
CQChartsColumnColorType::
userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
         const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  bool    mapped = false;
  double  min    = 0.0, max = 1.0;
  QString palette;

  getMapData(charts, model, column, nameValues, mapped, min, max, palette);

  if (mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      bool ok;

      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, min, max, 0, 1);

      if (r1 < 0.0 || r1 > 1.0)
        return var;

      CQChartsColor color;

      if (palette != "")
        color = CQChartsThemeMgrInst->getNamedPalette(palette)->getColor(r1);
      else
        color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r1);

      return QVariant::fromValue<CQChartsColor>(color);
    }
    else {
      if (CQChartsVariant::isColor(var)) {
        return var;
      }
      else {
        CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

        if (! columnDetails)
          return var;

        // use value index/count of original values
        int n = columnDetails->numValues();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        CQChartsColor color;

        if (palette != "")
          color = CQChartsThemeMgrInst->getNamedPalette(palette)->getColor(r);
        else
          color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r);

        return QVariant::fromValue<CQChartsColor>(color);
      }
    }
  }
  else {
    if (CQChartsVariant::isColor(var)) {
      return var;
    }
    else {
      QString str = var.toString();

      CQChartsColor color(str);

      if (! color.isValid())
        return var;

      return QVariant::fromValue<CQChartsColor>(color);
    }
  }
}

QVariant
CQChartsColumnColorType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  converted = true;

  if (CQChartsVariant::isColor(var)) {
    bool ok;

    CQChartsColor c = CQChartsVariant::toColor(var, ok);

    if (ok)
      return c.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnColorType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
           const CQChartsNameValues &nameValues, bool &mapped,
           double &map_min, double &map_max, QString &palette) const
{
  mapped  = false;
  map_min = 0.0;
  map_max = 1.0;
  palette = "";

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  (void) CQChartsColumnUtil::nameValueString(nameValues, "palette", palette);

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", map_min)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_min = CQChartsColumnUtil::varReal(columnDetails->minValue(), map_min);
    }
  }

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", map_max)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_max = CQChartsColumnUtil::varReal(columnDetails->maxValue(), map_max);
    }
  }

  return true;
}

//------

CQChartsColumnImageType::
CQChartsColumnImageType() :
 CQChartsColumnType(Type::IMAGE)
{
}

QVariant
CQChartsColumnImageType::
userData(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Image)
    return var;

  converted = true;

  QString str = var.toString();

  QImage image(str);

  image.setText("filename", str);

  if (image.isNull())
    return var;

  return image;
}

QVariant
CQChartsColumnImageType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.type() == QVariant::Image) {
    QImage image = var.value<QImage>();

    return image.text("filename");
  }

  return var; // TODO: other var formats
}

//------

CQChartsColumnSymbolTypeType::
CQChartsColumnSymbolTypeType() :
 CQChartsColumnType(Type::SYMBOL)
{
  // map from model value to fixed symbol type range
  params_.emplace_back("mapped", Type::BOOLEAN, "Value Mapped", false);

  params_.emplace_back("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0);
  params_.emplace_back("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0);
}

QVariant
CQChartsColumnSymbolTypeType::
userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
         const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid())
    return var;

  if (CQChartsVariant::isSymbol(var))
    return var;

  converted = true;

  bool mapped   = false;
  int  min      = 0, max = 1;
  int  size_min = CQChartsSymbol::minFillValue(), size_max = CQChartsSymbol::maxFillValue();

  getMapData(charts, model, column, nameValues, mapped, min, max, size_min, size_max);

  if (mapped) {
    bool ok;

    int i = CQChartsVariant::toInt(var, ok);
    if (! ok) return QVariant();

    int i1 = CMathUtil::map(i, min, max, size_min, size_max);

    CQChartsSymbol symbol = CQChartsSymbol((CQChartsSymbol::Type) i1);

    return QVariant::fromValue<CQChartsSymbol>(symbol);
  }
  else {
    QString str = var.toString();

    CQChartsSymbol symbol(str);

    return QVariant::fromValue<CQChartsSymbol>(symbol);
  }
}

QVariant
CQChartsColumnSymbolTypeType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  converted = true;

  if (CQChartsVariant::isSymbol(var)) {
    bool ok;

    CQChartsSymbol symbol = CQChartsVariant::toSymbol(var, ok);

    if (ok)
      return symbol.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnSymbolTypeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
           const CQChartsNameValues &nameValues, bool &mapped, int &map_min, int &map_max,
           int &data_min, int &data_max) const
{
  mapped   = false;
  map_min  = 0.0;
  map_max  = 1.0;
  data_min = 0.0;
  data_max = 1.0;

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "min", map_min)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_min = CQChartsColumnUtil::varInteger(columnDetails->minValue(), map_min);
    }
  }

  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "max", map_max)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_max = CQChartsColumnUtil::varInteger(columnDetails->maxValue(), map_max);
    }
  }

  (void) CQChartsColumnUtil::nameValueInteger(nameValues, "size_min", data_min);
  (void) CQChartsColumnUtil::nameValueInteger(nameValues, "size_max", data_max);

  return true;
}

//------

CQChartsColumnSymbolSizeType::
CQChartsColumnSymbolSizeType() :
 CQChartsColumnType(Type::SYMBOL_SIZE)
{
  // map from model value to symbol size min/max
  params_.emplace_back("mapped", Type::BOOLEAN, "Value Mapped", false);

  params_.emplace_back("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0);
  params_.emplace_back("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0);

  params_.emplace_back("size_min", Type::REAL, "Symbol Size Min", 0.0);
  params_.emplace_back("size_max", Type::REAL, "Symbol Size Max", 1.0);
}

QVariant
CQChartsColumnSymbolSizeType::
userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
         const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Double)
    return var;

  bool ok;

  double r = CQChartsVariant::toReal(var, ok);

  if (! ok)
    return var;

  converted = true;

  bool   mapped   = false;
  double min      = 0.0, max = 1.0;
  double size_min = CQChartsSymbolSize::minValue(), size_max = CQChartsSymbolSize::maxValue();

  getMapData(charts, model, column, nameValues, mapped, min, max, size_min, size_max);

  if (mapped) {
    bool ok;

    double r = CQChartsVariant::toReal(var, ok);
    if (! ok) return QVariant();

    double r1 = CMathUtil::map(r, min, max, size_min, size_max);

    return QVariant::fromValue<double>(r1);
  }
  else {
    return QVariant::fromValue<double>(r);
  }
}

QVariant
CQChartsColumnSymbolSizeType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get real value
  double r = 0.0;

  if (var.type() == QVariant::Double) {
    r = var.value<double>();
  }
  else {
    bool ok;

    r = CQChartsVariant::toReal(var, ok);

    if (! ok)
      return CQChartsVariant::toString(var, ok);

    converted = true;
  }

  //---

  return CQChartsUtil::toString(r);
}

bool
CQChartsColumnSymbolSizeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
           const CQChartsNameValues &nameValues, bool &mapped, double &map_min, double &map_max,
           double &data_min, double &data_max) const
{
  mapped   = false;
  map_min  = 0.0;
  map_max  = 1.0;
  data_min = 0.0;
  data_max = 1.0;

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", map_min)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_min = CQChartsColumnUtil::varReal(columnDetails->minValue(), map_min);
    }
  }

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", map_max)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_max = CQChartsColumnUtil::varReal(columnDetails->maxValue(), map_max);
    }
  }

  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_min", data_min);
  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_max", data_max);

  return true;
}

//------

CQChartsColumnFontSizeType::
CQChartsColumnFontSizeType() :
 CQChartsColumnType(Type::FONT_SIZE)
{
  // map from model value to font size min/max
  params_.emplace_back("mapped", Type::BOOLEAN, "Value Mapped", false);

  params_.emplace_back("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0);
  params_.emplace_back("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0);

  params_.emplace_back("size_min", Type::REAL, "Font Size Min", 0.0);
  params_.emplace_back("size_max", Type::REAL, "Font Size Max", 1.0);
}

QVariant
CQChartsColumnFontSizeType::
userData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
         const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Double)
    return var;

  bool ok;

  double r = CQChartsVariant::toReal(var, ok);

  if (! ok)
    return var;

  converted = true;

  bool   mapped   = false;
  double min      = 0.0, max = 1.0;
  double size_min = CQChartsFontSize::minValue(), size_max = CQChartsFontSize::maxValue();

  getMapData(charts, model, column, nameValues, mapped, min, max, size_min, size_max);

  if (mapped) {
    bool ok;

    double r = CQChartsVariant::toReal(var, ok);
    if (! ok) return QVariant();

    double r1 = CMathUtil::map(r, min, max, size_min, size_max);

    return QVariant::fromValue<double>(r1);
  }
  else {
    return QVariant::fromValue<double>(r);
  }
}

QVariant
CQChartsColumnFontSizeType::
dataName(CQCharts *, const QAbstractItemModel *, const CQChartsColumn &, const QVariant &var,
         const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get real value
  double r = 0.0;

  if (var.type() == QVariant::Double) {
    r = var.value<double>();
  }
  else {
    bool ok;

    r = CQChartsVariant::toReal(var, ok);

    if (! ok)
      return CQChartsVariant::toString(var, ok);

    converted = true;
  }

  //---

  return CQChartsUtil::toString(r);
}

bool
CQChartsColumnFontSizeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
           const CQChartsNameValues &nameValues, bool &mapped, double &map_min, double &map_max,
           double &data_min, double &data_max) const
{
  mapped   = false;
  map_min  = 0.0;
  map_max  = 1.0;
  data_min = 0.0;
  data_max = 1.0;

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", map_min)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_min = CQChartsColumnUtil::varReal(columnDetails->minValue(), map_min);
    }
  }

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", map_max)) {
    if (mapped) {
      CQChartsModelColumnDetails *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        map_max = CQChartsColumnUtil::varReal(columnDetails->maxValue(), map_max);
    }
  }

  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_min", data_min);
  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_max", data_max);

  return true;
}
