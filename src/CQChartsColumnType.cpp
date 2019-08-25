#include <CQChartsColumnType.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsPolygonList.h>
#include <CQChartsConnectionList.h>
#include <CQChartsNamePair.h>
#include <CQChartsSymbol.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQCharts.h>
#include <CQChartsTypes.h>
#include <CQChartsHtml.h>

#include <CQBaseModel.h>
#include <CQColors.h>
#include <CQColorsPalette.h>

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

  if (! nameValues.fromString(rhs))
    return false;

  return true;
}

bool nameValueString(const CQChartsNameValues &nameValues, const QString &name, QString &value) {
  return nameValues.nameValueString(name, value);
}

bool nameValueInteger(const CQChartsNameValues &nameValues, const QString &name, int &value) {
  return nameValues.nameValueInteger(name, value);
}

bool nameValueReal(const CQChartsNameValues &nameValues, const QString &name, double &value) {
  return nameValues.nameValueReal(name, value);
}

bool nameValueBool(const CQChartsNameValues &nameValues, const QString &name, bool &value) {
  return nameValues.nameValueBool(name, value);
}

int varInteger(const QVariant &var, int def) {
  bool ok = false;

  long i = CQChartsVariant::toInt(var, ok);

  return (ok ? i : def);
}

double varReal(const QVariant &var, double def) {
  bool ok = false;

  double r = CQChartsVariant::toReal(var, ok);

  return (ok ? r : def);
}

}

//------

QString
CQChartsColumnTypeMgr::
description()
{
  auto LI = [](const QString &str) { return CQChartsHtml::Str(str); };
  auto A  = [](const QString &ref, const QString &str) { return CQChartsHtml::Str::a(ref, str); };

  return CQChartsHtml().
   h2("Column Types").
    p("The following column types are supported:").
    ul({LI(A("charts://column_type/string"          , "string"          )),
        LI(A("charts://column_type/boolean"         , "boolean"         )),
        LI(A("charts://column_type/real"            , "real"            )),
        LI(A("charts://column_type/integer"         , "integer"         )),
        LI(A("charts://column_type/time"            , "time"            )),
        LI(A("charts://column_type/rect"            , "rectangle"       )),
        LI(A("charts://column_type/polygon"         , "polygon"         )),
        LI(A("charts://column_type/polygon list"    , "polygon list"    )),
        LI(A("charts://column_type/connections list", "connections list")),
        LI(A("charts://column_type/name pair"       , "name pair"       )),
        LI(A("charts://column_type/path"            , "path"            )),
        LI(A("charts://column_type/style"           , "style"           )),
        LI(A("charts://column_type/color"           , "color"           )),
        LI(A("charts://column_type/image"           , "image"           )),
        LI(A("charts://column_type/symbol type"     , "symbol type"     )),
        LI(A("charts://column_type/symbol size"     , "symbol size"     )),
        LI(A("charts://column_type/font size"       , "font size"       )) }).
    p("Values can be converted in the model (edit role) or in the plot (display role).").
    p("For example time values are usually stored as date strings in the input data so "
      "to store the actual UNIX time value in the edit role of the model they need to "
      "be converted using the column 'iformat' name value. To display the time in the "
      "plot (e.g. on axis tick labels) this converted time needs to be converted back "
      "to a string using the 'oformat' name value.").
    p("Color values can either be stored as color names in the input data or can "
      "be mapped into a palette from numeric values or discrete strings. Usually "
      "mapping takes place on data that can be used for other parts of the plot "
      "so it is better to convert the value in the plot rather than the model so "
      "the original data can still be accessed.");
}

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

#if 0
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
  QString rstr = nameValues.toString();

  if (! rstr.length())
    return lstr;

  return lstr + ":" + rstr;
}
#endif

const CQChartsColumnType *
CQChartsColumnTypeMgr::
getType(Type type) const
{
  auto p = typeData_.find(type);

  if (p == typeData_.end())
    return nullptr;

  return (*p).second;
}

const CQChartsColumnType *
CQChartsColumnTypeMgr::
getNamedType(const QString &name) const
{
  for (const auto &typeData : typeData_) {
    QString name1 = CQBaseModel::typeName(typeData.first);

    if (name == name1)
      return typeData.second;
  }

  return nullptr;
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

  QVariant var1;

  if (typeCacheData->typeData)
    var1 = typeCacheData->typeData->userData(charts_, model, column, var,
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

  QVariant var1;

  if (typeCacheData->typeData)
    var1 = typeCacheData->typeData->userData(charts_, model, column, var,
                                             typeCacheData->nameValues, converted);

  if (! var1.isValid())
    return var;

  QVariant var2;

  if (typeCacheData->typeData)
    var2 = typeCacheData->typeData->dataName(charts_, model, column, var1,
                                             typeCacheData->nameValues, converted);

  return var2;
}

bool
CQChartsColumnTypeMgr::
getModelColumnTypeData(const QAbstractItemModel *model, const CQChartsColumn &column,
                       const TypeCacheData* &typeCacheData) const
{
  bool ok;

  const CacheData &cacheData = getModelCacheData(model, ok);

  //---

  //bool caching = (ok && cacheData.depth > 0);
  bool caching = ok;

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

  if (! var1.isValid()) {
    bool ok;

    const CacheData &cacheData = getModelCacheData(model, ok);

    if (ok) {
      auto pc = cacheData.columnTypeCache.find(column);

      if (pc != cacheData.columnTypeCache.end()) {
        const TypeCacheData &typeCacheData = (*pc).second;

        type       = typeCacheData.type;
        baseType   = typeCacheData.baseType;
        nameValues = typeCacheData.nameValues;

        return true;
      }
    }

    return false;
  }

  // validate ?
  bool ok2;

  type = static_cast<Type>(CQChartsVariant::toInt(var1, ok2));

  if (! ok2 || type == Type::NONE) {
    baseType = Type::NONE;
    return false;
  }

  //---

  int brole = static_cast<int>(CQBaseModelRole::BaseType);

  bool ok3;

  QVariant var2 = CQChartsModelUtil::modelHeaderValue(model, column, brole, ok3);

  if (! ok3 || ! var2.isValid())
    return false;

  // validate ?
  bool ok4;

  baseType = static_cast<Type>(CQChartsVariant::toInt(var2, ok4));

  if (! ok4 || baseType == Type::NONE)
    baseType = type;

  //---

  int vrole = static_cast<int>(CQBaseModelRole::TypeValues);

  bool ok5;

  QVariant var3 = CQChartsModelUtil::modelHeaderValue(model, column, vrole, ok5);

  if (ok5 && var3.isValid()) {
    QString str3;

    CQChartsVariant::toString(var3, str3);

    nameValues = CQChartsNameValues(str3);
  }

  //---

  const CQChartsColumnType *typeData = getType(type);

  if (typeData) {
    for (const auto &param : typeData->params()) {
      if (param->role() != vrole) {
        bool ok;

        QVariant var = CQChartsModelUtil::modelHeaderValue(model, column, param->role(), ok);

        if (var.isValid())
          nameValues.setNameValue(param->name(), var);
      }
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

  // store role in model or cache if not supported
  int role = static_cast<int>(CQBaseModelRole::Type);

  bool rc1 = CQChartsModelUtil::setModelHeaderValue(model, column, static_cast<int>(type), role);

  if (! rc1) {
    bool ok;

    const CacheData &cacheData = getModelCacheData(model, ok);

    // if in cache, update cache
    if (ok) {
      CacheData &cacheData1 = const_cast<CacheData &>(cacheData);

      auto pc = cacheData1.columnTypeCache.find(column);

      if (pc != cacheData1.columnTypeCache.end()) {
        TypeCacheData &typeCacheData = (*pc).second;

        typeCacheData.type       = type;
        typeCacheData.nameValues = nameValues;
      }
    }
  }

  if (rc1)
    changed = true;

  //---

  // store parameter values in model (by parameter role)
  int vrole = static_cast<int>(CQBaseModelRole::TypeValues);

  CQChartsNameValues nameValues1;

  const CQChartsColumnType *typeData = getType(type);

  if (typeData) {
    for (const auto &param : typeData->params()) {
      QVariant value;

      if (! nameValues.nameValue(param->name(), value))
        continue;

      if (param->role() == vrole) {
        if (value.isValid())
          nameValues1.setNameValue(param->name(), value);
      }
      else {
        bool rc = CQChartsModelUtil::setModelHeaderValue(model, column, value, param->role());

        if (rc)
          changed = true;
      }
    }
  }

  // store name values in model (as encoded string)
  if (! nameValues1.empty()) {
    QString str = nameValues1.toString();

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
  bool ok;

  const CacheData &cacheData = getModelCacheData(model, ok);
  if (! ok) return;

  std::unique_lock<std::mutex> lock(mutex_);

  CacheData &cacheData1 = const_cast<CacheData &>(cacheData);

  ++cacheData1.depth;
}

void
CQChartsColumnTypeMgr::
endCache(const QAbstractItemModel *model)
{
  bool ok;

  const CacheData &cacheData = getModelCacheData(model, ok);
  if (! ok) return;

  std::unique_lock<std::mutex> lock(mutex_);

  CacheData &cacheData1 = const_cast<CacheData &>(cacheData);

  assert(cacheData1.depth > 0);

  --cacheData1.depth;

  if (cacheData1.depth == 0)
    cacheData1.columnTypeCache.clear();
}

const CQChartsColumnTypeMgr::CacheData &
CQChartsColumnTypeMgr::
getModelCacheData(const QAbstractItemModel *model, bool &ok) const
{
  int modelInd = -1;

  // get model data
  CQChartsModelData *modelData = charts_->getModelData(model);

  if (! modelData) {
    if (! charts_->getModelInd(model, modelInd)) {
      QAbstractItemModel *model1 = const_cast<QAbstractItemModel *>(model);

      if (! charts_->assignModelInd(model1, modelInd)) {
        static CQChartsColumnTypeMgr::CacheData dummyCacheData;

        ok = false;

        return dummyCacheData;
      }
    }
  }
  else {
    modelInd = modelData->ind();
  }

  ok = true;

  //---

  // get cache data for model
  auto pm = modelCacheData_.find(modelInd);

  if (pm == modelCacheData_.end()) {
    std::unique_lock<std::mutex> lock(mutex_);

    CQChartsColumnTypeMgr *th = const_cast<CQChartsColumnTypeMgr *>(this);

    auto pm1 = th->modelCacheData_.find(modelInd);

    if (pm1 == modelCacheData_.end()) {
      pm1 = th->modelCacheData_.insert(pm1,
        ModelCacheData::value_type(modelInd, CacheData()));
    }

    pm = modelCacheData_.find(modelInd);
  }

  const CacheData &cacheData = (*pm).second;

  return cacheData;
}

//------

CQChartsColumnType::
CQChartsColumnType(Type type) :
 type_(type)
{
  addParam("key", Type::BOOLEAN, (int) CQBaseModelRole::Key, "Is Key", false)->
   setDesc("Is Column a key for grouping");

  // draw color for table view
  addParam("draw_color", Type::COLOR, "Table Draw Color", "")->
   setDesc("Base color for table value coloring");

  // draw type for table view
  addParam("draw_type", Type::ENUM, "Table Draw Type", "")->
   setDesc("Table value draw type (heatmap or barchart)").
   addValue("heatmap").addValue("barchart");

  // draw stops for table view
  addParam("draw_stops", Type::STRING, "Table Draw Stops", "")->
   setDesc("Table value draw stop values");
}

CQChartsColumnType::
~CQChartsColumnType()
{
  for (auto &param : params_)
    delete param;
}

QString
CQChartsColumnType::
name() const
{
  return CQBaseModel::typeName(type_);
}

CQChartsColumnTypeParam *
CQChartsColumnType::
addParam(const QString &name, Type type, int role, const QString &tip, const QVariant &def)
{
  CQChartsColumnTypeParam *param = new CQChartsColumnTypeParam(name, type, role, tip, def);

  params_.push_back(param);

  return param;
}

CQChartsColumnTypeParam *
CQChartsColumnType::
addParam(const QString &name, Type type, const QString &tip, const QVariant &def)
{
  CQChartsColumnTypeParam *param = new CQChartsColumnTypeParam(name, type, tip, def);

  params_.push_back(param);

  return param;
}

bool
CQChartsColumnType::
hasParam(const QString &name) const
{
  for (const auto &param : params_)
    if (param->name() == name)
      return true;

  return false;
}

const CQChartsColumnTypeParam *
CQChartsColumnType::
getParam(const QString &name) const
{
  for (const auto &param : params_)
    if (param->name() == name)
      return param;

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

CQChartsColor
CQChartsColumnType::
drawColor(const CQChartsNameValues &nameValues) const
{
  QString colorName;

  if (! nameValueString(nameValues, "draw_color", colorName))
    colorName = "";

  CQChartsColor color;

  if (colorName.simplified().length())
    color = CQChartsColor(colorName);

  return color;
}

CQChartsColumnType::DrawType
CQChartsColumnType::
drawType(const CQChartsNameValues &nameValues) const
{
  QString typeName;

  if (! nameValueString(nameValues, "draw_type", typeName))
    typeName = "";

  typeName = typeName.toLower();

  if      (typeName == "normal")
    return CQChartsColumnType::DrawType::NORMAL;
  else if (typeName == "barchart")
    return CQChartsColumnType::DrawType::BARCHART;
  else if (typeName == "heatmap")
    return CQChartsColumnType::DrawType::HEATMAP;
  else
    return CQChartsColumnType::DrawType::NORMAL;
}

CQChartsColorStops
CQChartsColumnType::
drawStops(const CQChartsNameValues &nameValues) const
{
  QString stopsStr;

  if (! nameValueString(nameValues, "draw_stops", stopsStr))
    stopsStr = "";

  CQChartsColorStops stops;

  if (stopsStr.simplified().length())
    stops = CQChartsColorStops(stopsStr);

  return stops;
}

bool
CQChartsColumnType::
nameValueString(const CQChartsNameValues &nameValues, const QString &name, QString &value) const
{
  return CQChartsColumnUtil::nameValueString(nameValues, name, value);
}

//------

CQChartsColumnStringType::
CQChartsColumnStringType() :
 CQChartsColumnType(Type::STRING)
{
}

QString
CQChartsColumnStringType::
desc() const
{
  return CQChartsHtml().
   h2("String").
    p("Specifies that the column values are strings.");
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

QString
CQChartsColumnBooleanType::
desc() const
{
  return CQChartsHtml().
   h2("Boolean").
    p("Specifies that the column values are booleans.");
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
  addParam("format", Type::STRING, "Output Format", "")->
   setDesc("Format string for output value display");
  addParam("format_scale", Type::REAL, "Format Scale Factor", 1.0)->
   setDesc("Scale factor; to apply to value before output");

  addParam("min", Type::REAL, (int) CQBaseModelRole::Min, "Min Value", 0.0)->
   setDesc("Override min value for column");
  addParam("max", Type::REAL, (int) CQBaseModelRole::Max, "Max Value", 1.0)->
   setDesc("Override max value for column");
}

QString
CQChartsColumnRealType::
desc() const
{
  return CQChartsHtml().
   h2("Real").
    p("Specifies that the column values are real (double precision) numbers.");
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

  if (! nameValueString(nameValues, "format", format))
    return CQChartsUtil::formatReal(r);

  //---

  // get scale factor to support units suffix in format
  double scale;

  if (CQChartsColumnUtil::nameValueReal(nameValues, "format_scale", scale))
    r *= scale;

  //---

  // convert value using format
  return CQChartsUtil::formatVar(QVariant(r), format);
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
  addParam("format", Type::INTEGER, "Output Format", "")->
   setDesc("Format string for output value display");

  addParam("min", Type::INTEGER, (int) CQBaseModelRole::Min, "Min Value",   0)->
   setDesc("Override min value for column");
  addParam("max", Type::INTEGER, (int) CQBaseModelRole::Max, "Max Value", 100)->
   setDesc("Override max value for column");
}

QString
CQChartsColumnIntegerType::
desc() const
{
  return CQChartsHtml().
   h2("Real").
    p("Specifies that the column values are integer numbers.");
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

  if (! nameValueString(nameValues, "format", format))
    return CQChartsUtil::formatInteger(l);

  //---

  // convert value using format
  return CQChartsUtil::formatVar(var, format);
}

QVariant
CQChartsColumnIntegerType::
minValue(const CQChartsNameValues &nameValues) const
{
  int i;

  if (! imin(nameValues, i))
    return QVariant();

  return QVariant(i);
}

QVariant
CQChartsColumnIntegerType::
maxValue(const CQChartsNameValues &nameValues) const
{
  int i;

  if (! imax(nameValues, i))
    return QVariant();

  return QVariant(i);
}

bool
CQChartsColumnIntegerType::
imin(const CQChartsNameValues &nameValues, int &i) const
{
  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "min", i))
    return false;

  return true;
}

bool
CQChartsColumnIntegerType::
imax(const CQChartsNameValues &nameValues, int &i) const
{
  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "max", i))
    return false;

  return true;
}

//------

CQChartsColumnTimeType::
CQChartsColumnTimeType() :
 CQChartsColumnType(Type::TIME)
{
  addParam("format", Type::STRING, "Input/Output Format", "")->
   setDesc("Format string for input value conversion and output value display");
  addParam("iformat", Type::STRING, "Input Format", "")->
   setDesc("Format string for input value conversion");
  addParam("oformat", Type::STRING, "Output Format", "")->
   setDesc("Format string for output value display");
}

QString
CQChartsColumnTimeType::
desc() const
{
  return CQChartsHtml().
   h2("Time").
    p("Specifies that the column values are time values.");
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
    return CQChartsUtil::formatReal(t);

  return CQChartsUtil::timeToString(fmt, t);
}

QString
CQChartsColumnTimeType::
getIFormat(const CQChartsNameValues &nameValues) const
{
  QString format;

  if (nameValueString(nameValues, "iformat", format))
    return format;

  if (nameValueString(nameValues, "format", format))
    return format;

  return "";
}

QString
CQChartsColumnTimeType::
getOFormat(const CQChartsNameValues &nameValues) const
{
  QString format;

  if (nameValueString(nameValues, "oformat", format))
    return format;

  if (nameValueString(nameValues, "format", format))
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

QString
CQChartsColumnRectType::
desc() const
{
  return CQChartsHtml().
   h2("Rectangle").
    p("Specifies that the column values are rectangle values.");
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

QString
CQChartsColumnPolygonType::
desc() const
{
  return CQChartsHtml().
   h2("Polygon").
    p("Specifies that the column values are polygon values.");
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

QString
CQChartsColumnPolygonListType::
desc() const
{
  return CQChartsHtml().
   h2("Polygon List").
    p("Specifies that the column values are lists of polygon values.");
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

QString
CQChartsColumnConnectionListType::
desc() const
{
  return CQChartsHtml().
   h2("Connection List").
    p("Specifies that the column values are connection lists.");
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

QString
CQChartsColumnNamePairType::
desc() const
{
  return CQChartsHtml().
   h2("Name Pair").
    p("Specifies that the column values are name value pairs.");
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

QString
CQChartsColumnPathType::
desc() const
{
  return CQChartsHtml().
   h2("Path").
    p("Specifies that the column values are path definitions.");
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

QString
CQChartsColumnStyleType::
desc() const
{
  return CQChartsHtml().
   h2("Style").
    p("Specifies that the column values are style definitions.");
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
  // specific palette to get color from
  addParam("palette", Type::STRING, "Color Palette", "")->
   setDesc("Color Palette Name");

  // map from model value to 0.0 -> 1.0
  addParam("mapped", Type::BOOLEAN, "Value Mapped", false)->
   setDesc("Does value need to be remapped to 0.0->1.0");

  addParam("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0)->
   setDesc("Override min value for column");
  addParam("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0)->
   setDesc("Override max value for column");
}

QString
CQChartsColumnColorType::
desc() const
{
  return CQChartsHtml().
   h2("Color").
    p("Specifies that the column values are color names.");
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
  QString paletteName;

  getMapData(charts, model, column, nameValues, mapped, min, max, paletteName);

  if (mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      bool ok;

      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, min, max, 0, 1);

      if (r1 < 0.0 || r1 > 1.0)
        return var;

      CQChartsColor color;

      if (paletteName.simplified().length()) {
        CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(paletteName);

        if (palette)
          color = palette->getColor(r1);
        else
          color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r1);
      }
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

        if (paletteName.simplified().length()) {
          CQColorsPalette *palette = CQColorsMgrInst->getNamedPalette(paletteName);

          if (palette)
            color = palette->getColor(r);
          else
            color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r);
        }
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
           double &map_min, double &map_max, QString &paletteName) const
{
  if (! CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped))
    mapped = false;

  if (! nameValueString(nameValues, "palette", paletteName))
    paletteName = "";

  map_min = 0.0;
  map_max = 1.0;

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

QString
CQChartsColumnImageType::
desc() const
{
  return CQChartsHtml().
   h2("Image").
    p("Specifies that the column values are image names.");
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
  addParam("mapped", Type::BOOLEAN, "Value Mapped", false)->
   setDesc("Does value need to be remapped to 0.0->1.0");

  addParam("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0)->
   setDesc("Override min value for column");
  addParam("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0)->
   setDesc("Override max value for column");
}

QString
CQChartsColumnSymbolTypeType::
desc() const
{
  return CQChartsHtml().
   h2("Symbol Type").
    p("Specifies that the column values are symbol types.");
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
  addParam("mapped", Type::BOOLEAN, "Value Mapped", false)->
   setDesc("Does value need to be remapped to 0.0->1.0");

  addParam("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0)->
   setDesc("Override min value for column");
  addParam("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0)->
   setDesc("Override max value for column");

  addParam("size_min", Type::REAL, "Symbol Size Min", 0.0)->
   setDesc("Min size for mapped size value");
  addParam("size_max", Type::REAL, "Symbol Size Max", 1.0)->
   setDesc("Max size for mapped size value");
}

QString
CQChartsColumnSymbolSizeType::
desc() const
{
  return CQChartsHtml().
   h2("Symbol Size").
    p("Specifies that the column values are symbol sizes.");
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

  return CQChartsUtil::formatReal(r);
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
  addParam("mapped", Type::BOOLEAN, "Value Mapped", false)->
   setDesc("Does value need to be remapped to 0.0->1.0");

  addParam("min", Type::REAL, (int) CQBaseModelRole::Min, "Map Min", 0.0)->
   setDesc("Override min value for column");
  addParam("max", Type::REAL, (int) CQBaseModelRole::Max, "Map Max", 1.0)->
   setDesc("Override max value for column");

  addParam("size_min", Type::REAL, "Font Size Min", 0.0)->
   setDesc("Min size for mapped size value");
  addParam("size_max", Type::REAL, "Font Size Max", 1.0)->
   setDesc("Max size for mapped size value");
}

QString
CQChartsColumnFontSizeType::
desc() const
{
  return CQChartsHtml().
   h2("Font Size").
    p("Specifies that the column values are font sizes.");
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

  return CQChartsUtil::formatReal(r);
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
