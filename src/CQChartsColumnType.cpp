#include <CQChartsColumnType.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsPolygonList.h>
#include <CQChartsConnectionList.h>
#include <CQChartsNamePair.h>
#include <CQChartsNameValues.h>
#include <CQChartsSymbol.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CQCharts.h>
#include <CQChartsTypes.h>
#include <CQChartsHtml.h>

#include <CQPropertyView.h>
#include <CQTclUtil.h>
#include <CQBaseModel.h>
#include <CQModelUtil.h>
#include <CQColors.h>
#include <CQColorsPalette.h>
#include <CInterval.h>

//------

namespace CQChartsColumnUtil {

bool decodeType(const QString &type, QString &baseType, CQChartsNameValues &nameValues) {
  int pos = type.indexOf(":");

  if (pos < 0) {
    baseType = type;

    return true;
  }

  baseType = type.mid(0, pos);

  auto rhs = type.mid(pos + 1);

  if (! nameValues.fromString(rhs))
    return false;

  return true;
}

bool nameValueString(const CQChartsNameValues &nameValues, const QString &name, QString &value) {
  bool ok;
  return nameValues.nameValueString(name, value, ok) && ok;
}

bool nameValueInteger(const CQChartsNameValues &nameValues, const QString &name, long &value) {
  bool ok;
  return nameValues.nameValueInteger(name, value, ok) && ok;
}

bool nameValueReal(const CQChartsNameValues &nameValues, const QString &name, double &value) {
  bool ok;
  return nameValues.nameValueReal(name, value, ok) && ok;
}

bool nameValueBool(const CQChartsNameValues &nameValues, const QString &name, bool &value) {
  bool ok;
  return nameValues.nameValueBool(name, value, ok) && ok;
}

bool nameValueVariant(const CQChartsNameValues &nameValues, const QString &name,
                      CQBaseModelType type, QVariant &var) {
  if      (type == CQBaseModelType::BOOLEAN) {
    bool b;

    if (! nameValueBool(nameValues, name, b))
      return false;

    var = CQChartsVariant::fromBool(b);
  }
  else if (type == CQBaseModelType::STRING) {
    QString str;

    if (! nameValueString(nameValues, name, str))
      return false;

    var = CQChartsVariant::fromString(str);
  }
  else if (type == CQBaseModelType::INTEGER) {
    long i;

    if (! nameValueInteger(nameValues, name, i))
      return false;

    var = CQChartsVariant::fromInt(i);
  }
  else if (type == CQBaseModelType::REAL) {
    double r;

    if (! nameValueReal(nameValues, name, r))
      return false;

    var = CQChartsVariant::fromReal(r);
  }
  else {
    QString str;

    if (! nameValueString(nameValues, name, str))
      return false;

    var = CQChartsVariant::fromString(str);
  }

  return true;
}

long varInteger(const QVariant &var, long def) {
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

  auto A  = [](const QString &ref, const QString &str) {
    return CQChartsHtml::Str::a(CQChartsHtml::Str(ref), CQChartsHtml::Str(str)); };

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
typeNames(QStringList &names, bool hidden) const
{
  using IndString = std::map<int, QString>;

  IndString indString;

  for (auto &typeData : typeData_) {
    if (! hidden && typeData.second->isHidden())
      continue;

    auto name = CQBaseModel::typeName(typeData.first);

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
  data->setInd(int(typeData_.size()));

  typeData_[type] = data;
}

#if 0
const CQChartsColumnType *
CQChartsColumnTypeMgr::
decodeTypeData(const QString &typeStr, NameValues &nameValues) const
{
  QString baseTypeName;

  CQChartsColumnUtil::decodeType(typeStr, baseTypeName, nameValues);

  Type baseType = CQBaseModel::nameType(baseTypeName);

  const auto *baseTypeData = getType(baseType);

  return baseTypeData;
}

QString
CQChartsColumnTypeMgr::
encodeTypeData(Type type, const NameValues &nameValues) const
{
  auto lstr = CQBaseModel::typeName(type);
  auto rstr = nameValues.toString();

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
    auto name1 = CQBaseModel::typeName(typeData.first);

    if (name == name1)
      return typeData.second;
  }

  return nullptr;
}

// convert variant into user data
QVariant
CQChartsColumnTypeMgr::
getUserData(const QAbstractItemModel *model, const Column &column,
            const QVariant &var, bool &converted) const
{
  converted = false;

  const TypeCacheData *typeCacheData = nullptr;

  if (! getModelColumnTypeData(model, column, typeCacheData))
    return var;

  QVariant var1;

  if (typeCacheData->columnType) {
    var1 = typeCacheData->columnType->userData(charts_, model, column, var,
                                               typeCacheData->typeData, converted);

    var1 = typeCacheData->columnType->remapNamedValue(charts_, model, column, var1);
  }

  return var1;
}

// convert variant into display data
QVariant
CQChartsColumnTypeMgr::
getDisplayData(const QAbstractItemModel *model, const Column &column,
               const QVariant &var, bool &converted) const
{
  converted = false;

  const TypeCacheData *typeCacheData = nullptr;

  if (! getModelColumnTypeData(model, column, typeCacheData))
    return var;

  QVariant var1;

  if (typeCacheData->columnType) {
    var1 = typeCacheData->columnType->userData(charts_, model, column, var,
                                               typeCacheData->typeData, converted);

    var1 = typeCacheData->columnType->remapNamedValue(charts_, model, column, var1);
  }

  if (! var1.isValid())
    return var;

  QVariant var2;

  if (typeCacheData->columnType)
    var2 = typeCacheData->columnType->dataName(charts_, model, column, var1,
                                               typeCacheData->typeData, converted);

  return var2;
}

// convert variant into user data
QVariant
CQChartsColumnTypeMgr::
getHeaderUserData(const QAbstractItemModel *model, int section, const QVariant &var,
                  bool &converted) const
{
  const TypeCacheData *typeCacheData = nullptr;

  if (! getModelColumnTypeData(model, Column(section), typeCacheData))
    return var;

  QVariant var1;

  if (typeCacheData->headerColumnType) {
    ModelTypeData typeData;

    typeData.type       = typeCacheData->typeData.headerType;
    typeData.baseType   = typeData.type;
    typeData.nameValues = typeCacheData->typeData.headerNameValues;

    var1 = typeCacheData->headerColumnType->userData(charts_, model, Column(section), var,
                                                     typeData, converted);
  }

  return var1;
}

bool
CQChartsColumnTypeMgr::
getModelColumnTypeData(const QAbstractItemModel *model, const Column &column,
                       const TypeCacheData* &typeCacheData) const
{
  bool ok;

  const auto &cacheData = getModelCacheData(model, ok);

  //---

  //bool caching = (ok && cacheData.depth > 0);
  bool caching = ok;

  if (caching) {
    // get cached column data
    auto pc = cacheData.columnTypeCache.find(column);

    if (pc == cacheData.columnTypeCache.end()) {
      std::unique_lock<std::mutex> lock(mutex_);

      auto &cacheData1 = const_cast<CacheData &>(cacheData);

      auto pc1 = cacheData1.columnTypeCache.find(column);

      if (pc1 == cacheData1.columnTypeCache.end()) {
        TypeCacheData typeCacheData1;

        if (getModelColumnType(model, column, typeCacheData1.typeData)) {
          typeCacheData1.columnType = getType(typeCacheData1.typeData.type);

          if (typeCacheData1.columnType)
            typeCacheData1.valid = true;

          typeCacheData1.headerColumnType = getType(typeCacheData1.typeData.headerType);

          if (typeCacheData1.headerColumnType)
            typeCacheData1.headerValid = true;
        }

        pc1 = cacheData1.columnTypeCache.insert(pc1,
          ColumnTypeCache::value_type(column, typeCacheData1));
      }

      pc = cacheData.columnTypeCache.find(column);
    }

    typeCacheData = &(*pc).second;
  }
  else {
    auto &typeCacheData1 = const_cast<TypeCacheData &>(cacheData.typeCacheData);

    if (getModelColumnType(model, column, typeCacheData1.typeData)) {
      typeCacheData1.columnType = getType(typeCacheData1.typeData.type);

      if (typeCacheData1.columnType)
        typeCacheData1.valid = true;

      typeCacheData1.headerColumnType = getType(typeCacheData1.typeData.headerType);

      if (typeCacheData1.headerColumnType)
        typeCacheData1.headerValid = true;
    }

    typeCacheData = &typeCacheData1;
  }

  return typeCacheData->valid;
}

bool
CQChartsColumnTypeMgr::
getModelColumnType(const QAbstractItemModel *model, const Column &column,
                   ModelTypeData &typeData) const
{
  if (column.type() != Column::Type::DATA &&
      column.type() != Column::Type::DATA_INDEX) {
    typeData.type     = Type::STRING;
    typeData.baseType = Type::STRING;
    return true;
  }

  //---

  auto variantToModelType = [&](const QVariant &var, bool &ok) {
    ok = true;

    // validate ?
    Type type = static_cast<Type>(CQChartsVariant::toInt(var, ok));

    if (! ok || type == Type::NONE) {
      ok = false;
      return Type::NONE;
    }

    return type;
  };

  //---

  int role = CQModelUtil::roleCast(CQBaseModelRole::Type);

  bool ok1;

  auto var1 = CQChartsModelUtil::modelHeaderValue(model, column, role, ok1);

  if (! var1.isValid()) {
    bool ok;

    const auto &cacheData = getModelCacheData(model, ok);

    if (ok) {
      auto pc = cacheData.columnTypeCache.find(column);

      if (pc != cacheData.columnTypeCache.end()) {
        const auto &typeCacheData = (*pc).second;

        typeData = typeCacheData.typeData;

        return true;
      }
    }

    return false;
  }

  // validate ?
  typeData.type = variantToModelType(var1, ok1);

  if (! ok1) {
    typeData.baseType = Type::NONE;
    return false;
  }

  //---

  int brole = CQModelUtil::roleCast(CQBaseModelRole::BaseType);

  bool ok2;

  auto var2 = CQChartsModelUtil::modelHeaderValue(model, column, brole, ok2);

  if (ok2 && var2.isValid()) {
    typeData.baseType = variantToModelType(var2, ok2);

    if (! ok2)
      typeData.baseType = typeData.type;
  }
  else
    typeData.baseType = typeData.type;

  //---

  int vrole = CQModelUtil::roleCast(CQBaseModelRole::TypeValues);

  bool ok3;

  auto var3 = CQChartsModelUtil::modelHeaderValue(model, column, vrole, ok3);

  if (ok3 && var3.isValid()) {
    QString str3;

    CQChartsVariant::toString(var3, str3);

    typeData.nameValues = NameValues(str3);
  }

  //---

  const auto *columnType = getType(typeData.type);

  if (columnType) {
    for (const auto &param : columnType->params()) {
      if (param->role() != vrole) {
        bool ok;

        auto var = CQChartsModelUtil::modelHeaderValue(model, column, param->role(), ok);

        if (var.isValid())
          typeData.nameValues.setNameValue(param->name(), var);
      }
    }
  }

  //---

  int hrole = CQModelUtil::roleCast(CQBaseModelRole::HeaderType);

  bool ok4;

  auto var4 = CQChartsModelUtil::modelHeaderValue(model, column, hrole, ok4);

  if (ok4 && var4.isValid()) {
    typeData.headerType = variantToModelType(var4, ok4);

    if (! ok4)
      typeData.headerType = Type::STRING;
  }

  //---

  int hvrole = CQModelUtil::roleCast(CQBaseModelRole::HeaderTypeValues);

  bool ok5;

  auto var5 = CQChartsModelUtil::modelHeaderValue(model, column, hvrole, ok5);

  if (ok5 && var5.isValid()) {
    QString str5;

    CQChartsVariant::toString(var5, str5);

    typeData.headerNameValues = NameValues(str5);
  }

  return true;
}

bool
CQChartsColumnTypeMgr::
setModelColumnType(QAbstractItemModel *model, const Column &column,
                   Type type, const NameValues &nameValues)
{
  bool changed = false;

  auto *modelData = charts_->getModelData(model);
  if (! modelData) return false;

  modelData->startUpdate();

  //---

  // store role in model or cache if not supported
  int role = CQModelUtil::roleCast(CQBaseModelRole::Type);

  bool rc1 = CQChartsModelUtil::setModelHeaderValue(model, column, static_cast<int>(type), role);

  if (! rc1) {
    bool ok;

    const auto &cacheData = getModelCacheData(model, ok);

    // if in cache, update cache
    if (ok) {
      auto &cacheData1 = const_cast<CacheData &>(cacheData);

      auto pc = cacheData1.columnTypeCache.find(column);

      if (pc != cacheData1.columnTypeCache.end()) {
        auto &typeCacheData = (*pc).second;

        typeCacheData.typeData.type       = type;
        typeCacheData.typeData.nameValues = nameValues;
      }
    }
  }

  if (rc1)
    changed = true;

  //---

  // store parameter values in model (by parameter role)
  int vrole = CQModelUtil::roleCast(CQBaseModelRole::TypeValues);

  NameValues nameValues1;

  const auto *columnType = getType(type);

  if (columnType) {
    for (const auto &param : columnType->params()) {
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
    auto str = nameValues1.toString();

    bool rc2 = CQChartsModelUtil::setModelHeaderValue(model, column, str, vrole);

    if (rc2)
      changed = true;
  }

  //---

  modelData->endUpdate();

  if (changed) {
    if (modelData)
      charts_->emitModelTypeChanged(modelData->ind());
  }

  return changed;
}

bool
CQChartsColumnTypeMgr::
setModelHeaderType(QAbstractItemModel *model, const Column &column,
                   Type type, const NameValues &nameValues)
{
  bool changed = false;

  auto *modelData = charts_->getModelData(model);

  modelData->startUpdate();

  //---

  // store role in model or cache if not supported
  int role = CQModelUtil::roleCast(CQBaseModelRole::HeaderType);

  bool rc1 = CQChartsModelUtil::setModelHeaderValue(model, column, static_cast<int>(type), role);

  if (! rc1) {
    bool ok;

    const auto &cacheData = getModelCacheData(model, ok);

    // if in cache, update cache
    if (ok) {
      auto &cacheData1 = const_cast<CacheData &>(cacheData);

      auto pc = cacheData1.columnTypeCache.find(column);

      if (pc != cacheData1.columnTypeCache.end()) {
        auto &typeCacheData = (*pc).second;

        typeCacheData.typeData.headerType       = type;
        typeCacheData.typeData.headerNameValues = nameValues;
      }
    }
  }

  if (rc1)
    changed = true;

  //---

  // store parameter values in model (by parameter role)
  int vrole  = CQModelUtil::roleCast(CQBaseModelRole::TypeValues);
  int hvrole = CQModelUtil::roleCast(CQBaseModelRole::HeaderTypeValues);

  NameValues nameValues1;

  const auto *columnType = getType(type);

  if (columnType) {
    for (const auto &param : columnType->params()) {
      QVariant value;

      if (! nameValues.nameValue(param->name(), value))
        continue;

      if (param->role() == vrole) {
        if (value.isValid())
          nameValues1.setNameValue(param->name(), value);
      }
    }
  }

  // store name values in model (as encoded string)
  if (! nameValues1.empty()) {
    auto str = nameValues1.toString();

    bool rc2 = CQChartsModelUtil::setModelHeaderValue(model, column, str, hvrole);

    if (rc2)
      changed = true;
  }

  //---

  modelData->endUpdate();

  if (changed) {
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

  const auto &cacheData = getModelCacheData(model, ok);
  if (! ok) return;

  std::unique_lock<std::mutex> lock(mutex_);

  auto &cacheData1 = const_cast<CacheData &>(cacheData);

  ++cacheData1.depth;

  cacheDataStack_.push_back(cacheData1);
}

void
CQChartsColumnTypeMgr::
endCache(const QAbstractItemModel *model)
{
  bool ok;

  const auto &cacheData = getModelCacheData(model, ok);
  if (! ok) return;

  std::unique_lock<std::mutex> lock(mutex_);

  auto &cacheData1 = const_cast<CacheData &>(cacheData);

  assert(cacheData1.depth > 0);

  --cacheData1.depth;

  assert(! cacheDataStack_.empty());

  auto cacheData2 = cacheDataStack_.back();

  cacheDataStack_.pop_back();

  cacheData1 = cacheData2;

#if 0
  if (cacheData1.depth == 0)
    cacheData1.columnTypeCache.clear();
#endif
}

const CQChartsColumnTypeMgr::CacheData &
CQChartsColumnTypeMgr::
getModelCacheData(const QAbstractItemModel *model, bool &ok) const
{
  int modelInd = -1;

  // get model data
  auto *modelData = charts_->getModelData(model);

  if (! modelData) {
    if (! charts_->getModelInd(model, modelInd)) {
      auto *model1 = const_cast<QAbstractItemModel *>(model);

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
  std::unique_lock<std::mutex> lock(mutex_);

  auto pm = modelCacheData_.find(modelInd);

  if (pm == modelCacheData_.end()) {
    auto *th = const_cast<CQChartsColumnTypeMgr *>(this);

    pm = th->modelCacheData_.insert(pm, ModelCacheData::value_type(modelInd, CacheData()));
  }

  const auto &cacheData = (*pm).second;

  return cacheData;
}

//------

CQChartsColumnType::
CQChartsColumnType(Type type) :
 type_(type)
{
  addGenericParam("key", Type::BOOLEAN,
    CQModelUtil::roleCast(CQBaseModelRole::Key), "Is Key", false)->
    setDesc("Is Column a key for grouping");

  // preferred width
  addGenericParam("preferred_width", Type::INTEGER, "Preferred Width", "")->
    setDesc("Preferred column width");

  // null value
  addGenericParam("null_value", Type::STRING, "Null Value", "")->
    setDesc("Null value string").setNullValue(true);

  // extra values
  addGenericParam("extra_values", Type::STRING, "Extra Values", "")->
    setDesc("Extra values string");

  // draw color for table view
  addGenericParam("draw_color", Type::COLOR, "Table Draw Color", "")->
    setDesc("Base color for table value coloring");

  // draw type for table view
  addGenericParam("draw_type", Type::ENUM, "Table Draw Type", "")->
    setDesc("Table value draw type (heatmap or barchart)").
    addValue("heatmap").addValue("barchart");

  // draw stops for table view
  addGenericParam("draw_stops", Type::STRING, "Table Draw Stops", "")->
    setDesc("Table value draw stop values");

  // name to value map
  addGenericParam("named_values", Type::STRING, "Named Values", "")->
    setDesc("Named values");
  // name to color map
  addGenericParam("named_colors", Type::STRING, "Named Colors", "")->
    setDesc("Named colors");
  // name to image map
  addGenericParam("named_images", Type::STRING, "Named Images", "")->
    setDesc("Named images");
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
addGenericParam(const QString &name, Type type, int role, const QString &tip, const QVariant &def)
{
  auto *param = addParam(name, type, role, tip, def);

  param->setGeneric(true);

  return param;
}

CQChartsColumnTypeParam *
CQChartsColumnType::
addGenericParam(const QString &name, Type type, const QString &tip, const QVariant &def)
{
  auto *param = addParam(name, type, tip, def);

  param->setGeneric(true);

  return param;
}

CQChartsColumnTypeParam *
CQChartsColumnType::
addParam(const QString &name, Type type, int role, const QString &tip, const QVariant &def)
{
  auto *param = new CQChartsColumnTypeParam(name, type, role, tip, def);

  params_.push_back(param);

  return param;
}

CQChartsColumnTypeParam *
CQChartsColumnType::
addParam(const QString &name, Type type, const QString &tip, const QVariant &def)
{
  auto *param = new CQChartsColumnTypeParam(name, type, tip, def);

  params_.push_back(param);

  return param;
}

QStringList
CQChartsColumnType::
paramNames() const
{
  QStringList names;

  for (const auto &param : params_)
    names << param->name();

  return names;
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
columnDetails(CQCharts *charts, const QAbstractItemModel *model, const Column &column) const
{
  auto *modelData = charts->getModelData(model);
  if (! modelData) return nullptr;

  auto *details = modelData->details();
  if (! details) return nullptr;

  return details->columnDetails(column);
}

int
CQChartsColumnType::
preferredWidth(const NameValues &nameValues) const
{
  QString str;

  if (! nameValueString(nameValues, "preferred_width", str))
    str.clear();

  bool ok;

  long width = CQChartsUtil::toInt(str, ok);

  return (ok ? int(width) : -1);
}

QString
CQChartsColumnType::
nullValue(const NameValues &nameValues) const
{
  QString nullStr;

  if (! nameValueString(nameValues, "null_value", nullStr))
    nullStr.clear();

  return nullStr;
}

QVariantList
CQChartsColumnType::
extraValues(const NameValues &nameValues) const
{
  QVariantList extraValues;

  QString extraValuesStr;

  if (! nameValueString(nameValues, "extra_values", extraValuesStr))
    extraValuesStr.clear();

  QStringList strs;
  if (! CQTcl::splitList(extraValuesStr, strs))
    return extraValues;

  for (const auto &str : strs)
    extraValues.push_back(QVariant(str));

  return extraValues;
}

CQChartsColor
CQChartsColumnType::
drawColor(const NameValues &nameValues) const
{
  QString colorName;

  if (! nameValueString(nameValues, "draw_color", colorName))
    colorName.clear();

  CQChartsColor color;

  if (colorName.trimmed().length())
    color = CQChartsColor(colorName);

  return color;
}

CQChartsColumnType::DrawType
CQChartsColumnType::
drawType(const NameValues &nameValues) const
{
  QString typeName;

  if (! nameValueString(nameValues, "draw_type", typeName))
    typeName.clear();

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
drawStops(const NameValues &nameValues) const
{
  QString stopsStr;

  if (! nameValueString(nameValues, "draw_stops", stopsStr))
    stopsStr.clear();

  CQChartsColorStops stops;

  if (stopsStr.trimmed().length())
    stops = CQChartsColorStops(stopsStr);

  return stops;
}

QVariant
CQChartsColumnType::
remapNamedValue(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
                const QVariant &var) const
{
  std::unique_lock<std::mutex> lock(mutex_);

  if (! nameValueMapSet_) {
    auto *th = const_cast<CQChartsColumnType *>(this);

    auto *columnDetails = this->columnDetails(charts, model, column);

    th->nameValueMapSet_ = true;

    auto nv = namedValues(columnDetails->nameValues());

    th->nameValueMap_ = nv.nameValues();
  }

  if (nameValueMap_.empty())
    return var;

  auto pn = nameValueMap_.find(var.toString());

  if (pn != nameValueMap_.end())
    return (*pn).second;

  return var;
}

CQChartsNameValues
CQChartsColumnType::
namedValues(const NameValues &nameValues) const
{
  QString namedValuesStr;

  if (! nameValueString(nameValues, "named_values", namedValuesStr))
    namedValuesStr.clear();

  NameValues namedValues;

  QStringList strs;

  if (! CQTcl::splitList(namedValuesStr, strs))
    return namedValues;

  for (const auto &str : strs) {
    QStringList strs1;

    if (! CQTcl::splitList(str, strs1) || strs1.length() != 2)
      continue;

    namedValues.setNameValue(strs1[0], strs1[1]);
  }

  return namedValues;
}

CQChartsNameValues
CQChartsColumnType::
namedColors(const NameValues &nameValues) const
{
  QString namedColorsStr;

  if (! nameValueString(nameValues, "named_colors", namedColorsStr))
    namedColorsStr.clear();

  NameValues namedColors;

  QStringList strs;

  if (! CQTcl::splitList(namedColorsStr, strs))
    return namedColors;

  for (const auto &str : strs) {
    QStringList strs1;

    if (! CQTcl::splitList(str, strs1) || strs1.length() != 2)
      continue;

    namedColors.setNameValue(strs1[0], CQChartsUtil::stringToColor(strs1[1]));
  }

  return namedColors;
}

CQChartsNameValues
CQChartsColumnType::
namedImages(CQCharts *, const NameValues &nameValues) const
{
  QString namedImagesStr;

  if (! nameValueString(nameValues, "named_images", namedImagesStr))
    namedImagesStr.clear();

  NameValues namedImages;

  QStringList strs;

  if (! CQTcl::splitList(namedImagesStr, strs))
    return namedImages;

  for (const auto &str : strs) {
    QStringList strs1;

    if (! CQTcl::splitList(str, strs1) || strs1.length() != 2)
      continue;

    CQChartsImage image(strs1[1]);

    namedImages.setNameValue(strs1[0], CQChartsVariant::fromImage(image));
  }

  return namedImages;
}

bool
CQChartsColumnType::
getNameValueVariant(const NameValues &nameValues, const QString &name,
                    Type type, QVariant &value) const
{
  // TODO: check name is valid and has correct type ?

  // fails if name is not present in name values
  if (! CQChartsColumnUtil::nameValueVariant(nameValues, name, type, value))
    value = QVariant();

  return true;
}

bool
CQChartsColumnType::
setNameValueVariant(NameValues &nameValues, const QString &name,
                    Type /*type*/, const QVariant &value) const
{
  // TODO: check name is valid and has correct type ?

  nameValues.setNameValue(name, value);

  return true;
}

bool
CQChartsColumnType::
nameValueString(const NameValues &nameValues, const QString &name, QString &value) const
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isString(var))
    return var;

  converted = true;

  QString str;

  CQChartsVariant::toString(var, str);

  return CQChartsVariant::fromString(str);
}

QVariant
CQChartsColumnStringType::
dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  return userData(charts, model, column, var, typeData, converted);
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isBool(var))
    return var;

  converted = true;

  bool ok;
  bool b = CQChartsVariant::toBool(var, ok);

  return CQChartsVariant::fromBool(b);
}

QVariant
CQChartsColumnBooleanType::
dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  return userData(charts, model, column, var, typeData, converted);
}

//------

CQChartsColumnRealType::
CQChartsColumnRealType() :
 CQChartsColumnType(Type::REAL)
{
  addParam("format", Type::STRING, "Input/Output Format", "")->
    setDesc("Format string for input value conversion and output value display");
  addParam("iformat", Type::STRING, "Input Format", "")->
    setDesc("Format string for input value conversion");
  addParam("oformat", Type::STRING, "Output Format", "")->
    setDesc("Format string for output value display");
  addParam("format_scale", Type::REAL, "Format Scale Factor", 1.0)->
    setDesc("Scale factor; to apply to value before output");

  addParam("min", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Min), "Min Value", 0.0)->
    setDesc("Override min value for column");
  addParam("max", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Max), "Max Value", 1.0)->
    setDesc("Override max value for column");
  addParam("sum", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Sum), "Value Sum", 1.0)->
    setDesc("Override value sum for column");

  addParam("decreasing", Type::BOOLEAN, "Decreasing")->
    setDesc("Values are ordered decreasing");

  addParam("bad_value", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::BadValue),
           "Bad Value", CMathUtil::getNaN())->
    setDesc("Override bad value for column");
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isReal(var))
    return var;

  double r = 0.0;

  // use format string to convert model (input) string to time (double)
  // TODO: assert if no format ?
  auto fmt = getIFormat(typeData.nameValues);

  if (! fmt.length()) {
    bool ok;

    r = CQChartsVariant::toReal(var, ok);
    if (! ok) return var;
  }
  else {
    if (! CQChartsUtil::scanReal(fmt, var.toString(), r))
      return var;
  }

  converted = true;

  return CQChartsVariant::fromReal(r);
}

// data variant to output variant (string) for display
QVariant
CQChartsColumnRealType::
dataName(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get real value
  bool ok;
  double r = CQChartsVariant::toConvertedReal(var, ok, converted);
  if (! ok) return CQChartsVariant::toString(var, ok);

  //---

  // get optional format for real
  auto fmt = getOFormat(typeData.nameValues);

  if (! fmt.length())
    return CQChartsUtil::formatReal(r);

  //---

  // get scale factor to support units suffix in format
  double scale = 1.0;
  bool scaleSet = CQChartsColumnUtil::nameValueReal(typeData.nameValues, "format_scale", scale);
  if (scale == 0.0)
    scaleSet = false;

  //---

  if (fmt == "%P") {
    int ndp = (scaleSet ? int(std::log10(std::abs(scale))) : 3);

    return CQChartsUtil::scaledNumberString(r, -ndp); // no pad
  }

  //---

  if (scaleSet)
    r *= scale;

  //---

  if (fmt == "%R") {
    auto *columnDetails = this->columnDetails(charts, model, column);

    if (columnDetails) {
      double min = 0.0;
      double max = 1.0;

      min = CQChartsColumnUtil::varReal(columnDetails->minValue(), min);
      max = CQChartsColumnUtil::varReal(columnDetails->maxValue(), max);

      CInterval interval(min, max, 10);

      auto min1 = interval.calcStart    ();
      auto max1 = interval.calcEnd      ();
      auto d1   = interval.calcIncrement();

      fmt = CQChartsUtil::rangeFormat(min1 - d1, max1 + d1);
    }
    else
      fmt = "%g";
  }

  //---

  // convert value using format
  return CQChartsUtil::formatVar(CQChartsVariant::fromReal(r), fmt);
}

QVariant
CQChartsColumnRealType::
minValue(const NameValues &nameValues) const
{
  double r;

  if (! rmin(nameValues, r))
    return QVariant();

  return CQChartsVariant::fromReal(r);
}

QVariant
CQChartsColumnRealType::
maxValue(const NameValues &nameValues) const
{
  double r;

  if (! rmax(nameValues, r))
    return QVariant();

  return CQChartsVariant::fromReal(r);
}

QVariant
CQChartsColumnRealType::
sumValue(const NameValues &nameValues) const
{
  double r;

  if (! rsum(nameValues, r))
    return QVariant();

  return CQChartsVariant::fromReal(r);
}

QVariant
CQChartsColumnRealType::
decreasing(const NameValues &nameValues) const
{
  bool b;

  if (CQChartsColumnUtil::nameValueBool(nameValues, "decreasing", b))
    return CQChartsVariant::fromBool(b);

  return QVariant();
}

QVariant
CQChartsColumnRealType::
badValue(const NameValues &nameValues) const
{
  double r;

  if (CQChartsColumnUtil::nameValueReal(nameValues, "bad_value", r))
    return CQChartsVariant::fromReal(r);

  return QVariant();
}

bool
CQChartsColumnRealType::
rmin(const NameValues &nameValues, double &r) const
{
  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", r))
    return false;

  return true;
}

bool
CQChartsColumnRealType::
rmax(const NameValues &nameValues, double &r) const
{
  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", r))
    return false;

  return true;
}

bool
CQChartsColumnRealType::
rsum(const NameValues &nameValues, double &r) const
{
  if (! CQChartsColumnUtil::nameValueReal(nameValues, "sum", r))
    return false;

  return true;
}

QString
CQChartsColumnRealType::
getIFormat(const NameValues &nameValues) const
{
  QString format;

  if (nameValueString(nameValues, "iformat", format))
    return format;

  if (nameValueString(nameValues, "format", format))
    return format;

  return "";
}

QString
CQChartsColumnRealType::
getOFormat(const NameValues &nameValues) const
{
  QString format;

  if (nameValueString(nameValues, "oformat", format))
    return format;

  if (nameValueString(nameValues, "format", format))
    return format;

  return "";
}

//------

CQChartsColumnIntegerType::
CQChartsColumnIntegerType() :
 CQChartsColumnType(Type::INTEGER)
{
  addParam("format", Type::STRING, "Input/Output Format", "")->
    setDesc("Format string for input value conversion and output value display");
  addParam("iformat", Type::STRING, "Input Format", "")->
    setDesc("Format string for input value conversion");
  addParam("oformat", Type::STRING, "Output Format", "")->
    setDesc("Format string for output value display");

  addParam("min", Type::INTEGER, CQModelUtil::roleCast(CQBaseModelRole::Min), "Min Value",   0)->
    setDesc("Override min value for column");
  addParam("max", Type::INTEGER, CQModelUtil::roleCast(CQBaseModelRole::Max), "Max Value", 100)->
    setDesc("Override max value for column");
  addParam("sum", Type::INTEGER, CQModelUtil::roleCast(CQBaseModelRole::Sum), "Value Sum", 100)->
    setDesc("Override value sum for column");
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isInt(var))
    return var;

  long l = 0;

  // use format string to convert model (input) string to time (double)
  // TODO: assert if no format ?
  auto fmt = getIFormat(typeData.nameValues);

  if (! fmt.length()) {
    bool ok;

    l = CQChartsVariant::toInt(var, ok);
    if (! ok) return var;
  }
  else {
    if (! CQChartsUtil::scanInteger(fmt, var.toString(), l))
      return var;
  }

  converted = true;

  return CQChartsVariant::fromInt(l);
}

// data variant to output variant (string) for display
QVariant
CQChartsColumnIntegerType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get integer value
  long l = 0;
  bool ok;

  if (CQChartsVariant::isInt(var))
    l = CQChartsVariant::toInt(var, ok);
  else {
    l = CQChartsVariant::toInt(var, ok);

    if (! ok)
      return CQChartsVariant::toString(var, ok);

    converted = true;
  }

  //---

  // get optional format for real
  auto fmt = getOFormat(typeData.nameValues);

  if (! fmt.length())
    return CQChartsUtil::formatInteger(l);

  //---

  if (fmt == "%R") {
    fmt = "%d";
  }

  //---

  // convert value using format
  return CQChartsUtil::formatVar(var, fmt);
}

QVariant
CQChartsColumnIntegerType::
minValue(const NameValues &nameValues) const
{
  long i;

  if (! imin(nameValues, i))
    return QVariant();

  return CQChartsVariant::fromInt(i);
}

QVariant
CQChartsColumnIntegerType::
maxValue(const NameValues &nameValues) const
{
  long i;

  if (! imax(nameValues, i))
    return QVariant();

  return CQChartsVariant::fromInt(i);
}

QVariant
CQChartsColumnIntegerType::
sumValue(const NameValues &nameValues) const
{
  long i;

  if (! isum(nameValues, i))
    return QVariant();

  return CQChartsVariant::fromInt(i);
}

bool
CQChartsColumnIntegerType::
imin(const NameValues &nameValues, long &i) const
{
  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "min", i))
    return false;

  return true;
}

bool
CQChartsColumnIntegerType::
imax(const NameValues &nameValues, long &i) const
{
  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "max", i))
    return false;

  return true;
}

bool
CQChartsColumnIntegerType::
isum(const NameValues &nameValues, long &i) const
{
  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "sum", i))
    return false;

  return true;
}

QString
CQChartsColumnIntegerType::
getIFormat(const NameValues &nameValues) const
{
  QString format;

  if (nameValueString(nameValues, "iformat", format))
    return format;

  if (nameValueString(nameValues, "format", format))
    return format;

  return "";
}

QString
CQChartsColumnIntegerType::
getOFormat(const NameValues &nameValues) const
{
  QString format;

  if (nameValueString(nameValues, "oformat", format))
    return format;

  if (nameValueString(nameValues, "format", format))
    return format;

  return "";
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isReal(var))
    return var;

  // use format string to convert model (input) string to time (double)
  // TODO: assert if no format ?
  auto fmt = getIFormat(typeData.nameValues);

  if (! fmt.length())
    return var;

  double t;

  if (! CQChartsUtil::stringToTime(fmt, var.toString(), t))
    return var;

  converted = true;

  return CQChartsVariant::fromReal(t);
}

QVariant
CQChartsColumnTimeType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &typeData, bool &converted) const
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
  auto fmt = getOFormat(typeData.nameValues);

  if (! fmt.length())
    return CQChartsUtil::formatReal(t);

  return CQChartsUtil::timeToString(fmt, t);
}

QString
CQChartsColumnTimeType::
getIFormat(const NameValues &nameValues) const
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
getOFormat(const NameValues &nameValues) const
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
    return QVariant();

  // get time value (double)
  bool ok;

  double t = CQChartsVariant::toReal(var, ok);

  if (! ok)
    return QVariant();

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

CQChartsColumnPointType::
CQChartsColumnPointType() :
 CQChartsColumnType(Type::POINT)
{
}

QString
CQChartsColumnPointType::
desc() const
{
  return CQChartsHtml().
    h2("Pointangle").
     p("Specifies that the column values are point values.");
}

QVariant
CQChartsColumnPointType::
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isPointF(var))
    return var;

  converted = true;

  bool ok;

  auto p = CQChartsVariant::toPoint(var, ok);
  if (! ok) return var;

  return CQChartsVariant::fromPoint(p);
}

QVariant
CQChartsColumnPointType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  bool ok;

  auto p = CQChartsVariant::toPoint(var, ok);
  if (! ok) return var;

  converted = true;

  return p.toString();
}

QVariant
CQChartsColumnPointType::
indexVar(const QVariant &var, const QString &ind) const
{
  bool ok;

  auto p = CQChartsVariant::toPoint(var, ok);
  if (! ok) return var;

  //---

  if      (ind == "x") return p.x;
  else if (ind == "y") return p.y;
  else return var;
}

CQChartsColumnTimeType::Type
CQChartsColumnPointType::
indexType(const QString &) const
{
  return Type::REAL;
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isRectF(var))
    return var;

  converted = true;

  bool ok;

  auto bbox = CQChartsVariant::toBBox(var, ok);
  if (! ok) return var;

  return CQChartsVariant::fromBBox(bbox);
}

QVariant
CQChartsColumnRectType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  bool ok;

  auto bbox = CQChartsVariant::toBBox(var, ok);
  if (! ok) return var;

  converted = true;

  return bbox.toString();
}

QVariant
CQChartsColumnRectType::
indexVar(const QVariant &var, const QString &ind) const
{
  bool ok;

  auto bbox = CQChartsVariant::toBBox(var, ok);
  if (! ok) return var;

  //---

  if      (ind == "xmin") return bbox.getXMin();
  else if (ind == "ymin") return bbox.getYMin();
  else if (ind == "xmax") return bbox.getXMax();
  else if (ind == "ymax") return bbox.getYMax();
  else return var;
}

CQChartsColumnTimeType::Type
CQChartsColumnRectType::
indexType(const QString &) const
{
  return Type::REAL;
}

//------

CQChartsColumnLengthType::
CQChartsColumnLengthType() :
 CQChartsColumnType(Type::LENGTH)
{
}

QString
CQChartsColumnLengthType::
desc() const
{
  return CQChartsHtml().
    h2("Length").
     p("Specifies that the column values are length values.");
}

QVariant
CQChartsColumnLengthType::
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isLength(var))
    return var;

  converted = true;

  bool ok;

  auto len = CQChartsVariant::toLength(var, ok);
  if (! ok) return var;

  return CQChartsVariant::fromLength(len);
}

QVariant
CQChartsColumnLengthType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  bool ok;

  auto len = CQChartsVariant::toLength(var, ok);
  if (! ok) return var;

  converted = true;

  return len.toString();
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isPolygonF(var))
    return var;

  converted = true;

  if (CQChartsVariant::isPolygon(var)) {
    bool ok;
    auto poly = CQChartsVariant::toQPolygon(var, ok);

    return CQChartsVariant::fromQPolygon(poly);
  }

  auto str = var.toString();

  CQChartsGeom::Polygon poly;

  (void) CQChartsUtil::stringToPolygon(str, poly);

  return CQChartsVariant::fromQPolygon(poly.qpoly());
}

QVariant
CQChartsColumnPolygonType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  QPolygonF poly;

  if      (CQChartsVariant::isPolygon(var)) {
    bool ok;
    poly = CQChartsVariant::toQPolygon(var, ok);
  }
  else
    return QVariant();

  return CQChartsUtil::polygonToString(CQChartsGeom::Polygon(poly));
}

QVariant
CQChartsColumnPolygonType::
indexVar(const QVariant &var, const QString &ind) const
{
  bool ok;
  auto poly = CQChartsVariant::toQPolygon(var, ok);
  if (! ok) return QVariant();

  //---

  if (ind.size() < 2) return false;

  int xy;
  if      (ind[0] == 'x') xy = 0;
  else if (ind[1] == 'y') xy = 1;
  else return QVariant();

  auto ni = ind.mid(1).toInt(&ok);
  if (! ok) return QVariant();

  auto n = poly.size();
  if (ni < 0 || ni >= n) return QVariant();

  if (xy == 0) return poly[ni].x();
  else         return poly[ni].y();
}

CQChartsColumnTimeType::Type
CQChartsColumnPolygonType::
indexType(const QString &) const
{
  return Type::REAL;
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || isPolygonListVariant(var))
    return var;

  converted = true;

  auto str = var.toString();

  CQChartsPolygonList polyList(str);

  return CQChartsVariant::fromPolygonList(polyList);
}

QVariant
CQChartsColumnPolygonListType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isPolygonListVariant(var)) {
    auto polyList = CQChartsPolygonList::fromVariant(var);

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

  if (CQChartsPolygonList::isVariantType(var))
    return true;

  return false;
}

// TODO: index polygon points

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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || isVariantType(var))
    return var;

  converted = true;

  auto str = var.toString();

  CQChartsConnectionList connectionList(str);

  return CQChartsConnectionList::toVariant(connectionList);
}

QVariant
CQChartsColumnConnectionListType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isVariantType(var)) {
    auto connectionList = CQChartsConnectionList::fromVariant(var);

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

  if (CQChartsConnectionList::isVariantType(var))
    return true;

  return false;
}

// TODO: index connection points

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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || isNamePairVariant(var))
    return var;

  converted = true;

  auto str = var.toString();

  CQChartsNamePair namePair(str);

  if (! namePair.isValid())
    return QVariant();

  return CQChartsNamePair::toVariant(namePair);
}

QVariant
CQChartsColumnNamePairType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isNamePairVariant(var)) {
    auto namePair = CQChartsNamePair::fromVariant(var);

    if (! namePair.isValid())
      return var;

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

  if (CQChartsNamePair::isVariantType(var))
    return true;

  return false;
}

// TODO: pair first/second

//------

CQChartsColumnNameValuesType::
CQChartsColumnNameValuesType() :
 CQChartsColumnType(Type::NAME_VALUES)
{
}

QString
CQChartsColumnNameValuesType::
desc() const
{
  return CQChartsHtml().
    h2("Name Pair").
     p("Specifies that the column values are name value pairs.");
}

QVariant
CQChartsColumnNameValuesType::
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || isNameValuesVariant(var))
    return var;

  converted = true;

  auto str = var.toString();

  CQChartsNameValues nv;

  if (! nv.fromString(str))
    return QVariant();

  return CQChartsNameValues::toVariant(nv);
}

QVariant
CQChartsColumnNameValuesType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isNameValuesVariant(var)) {
    auto nv = CQChartsNameValues::fromVariant(var);
    //if (! nv.isValid()) return var;

    return nv.toString();
  }

  return var; // TODO: other var formats
}

QVariant
CQChartsColumnNameValuesType::
indexVar(const QVariant &var, const QString &ind) const
{
  if (! var.isValid())
    return var;

  if (isNameValuesVariant(var)) {
    auto nv = CQChartsNameValues::fromVariant(var);
    //if (! nv.isValid()) return var;

    QVariant value;

    if (! nv.nameValue(ind, value))
      return QVariant();

    return QVariant(value);
  }

  return var; // TODO: other var formats
}

CQChartsColumnTimeType::Type
CQChartsColumnNameValuesType::
indexType(const QString &) const
{
  return Type::STRING;
}

bool
CQChartsColumnNameValuesType::
isNameValuesVariant(const QVariant &var) const
{
  if (! var.isValid())
    return false;

  if (CQChartsNameValues::isVariantType(var))
    return true;

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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsPath::isVariantType(var))
    return var;

  converted = true;

  auto str = var.toString();

  CQChartsPath path;

  (void) CQChartsUtil::stringToPath(str, path);

  return CQChartsVariant::fromPath(path);
}

QVariant
CQChartsColumnPathType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  CQChartsPath path;

  if (! CQChartsVariant::toPath(var, path))
    return QVariant();

  return CQChartsUtil::pathToString(path);
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsStyle::isVariantType(var))
    return var;

  converted = true;

  auto str = var.toString();

  CQChartsStyle style;

  (void) CQChartsUtil::stringToStyle(str, style);

  return CQChartsStyle::toVariant(style);
}

QVariant
CQChartsColumnStyleType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (CQChartsStyle::isVariantType(var)) {
    auto style = CQChartsStyle::fromVariant(var);

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

  addParam("min", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Min), "Map Min", 0.0)->
    setDesc("Override min value for column");
  addParam("max", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Max), "Map Max", 1.0)->
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
userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  bool        mapped = false;
  double      min    = 0.0, max = 1.0;
  PaletteName paletteName;

  getMapData(charts, model, column, typeData.nameValues, mapped, min, max, paletteName);

  if (mapped) {
    if (CQChartsVariant::isNumeric(var)) {
      bool ok;

      double r = CQChartsVariant::toReal(var, ok);
      if (! ok) return false;

      double r1 = CMathUtil::map(r, min, max, 0, 1);

      if (r1 < 0.0 || r1 > 1.0)
        return var;

      CQChartsColor color;

      if (paletteName.isValid()) {
        auto *palette = paletteName.palette();

        if (palette)
          color = Color(palette->getColor(r1));
        else
          color = Color::makePaletteValue(r1);
      }
      else
        color = Color::makePaletteValue(r1);

      return CQChartsVariant::fromColor(color);
    }
    else {
      if (CQChartsVariant::isColor(var)) {
        return var;
      }
      else {
        auto *columnDetails = this->columnDetails(charts, model, column);
        if (! columnDetails) return var;

        // use value index/count of original values
        int n = columnDetails->numValues();
        int i = columnDetails->valueInd(var);

        double r = (n > 1 ? double(i)/(n - 1) : 0.0);

        CQChartsColor color;

        if (paletteName.isValid()) {
          auto *palette = paletteName.palette();

          if (palette)
            color = Color(palette->getColor(r));
          else
            color = Color::makePaletteValue(r);
        }
        else
          color = Color::makePaletteValue(r);

        return CQChartsVariant::fromColor(color);
      }
    }
  }
  else {
    if (CQChartsVariant::isColor(var)) {
      return var;
    }
    else {
      auto str = var.toString();

      CQChartsColor color(str);

      if (! color.isValid())
        return var;

      return CQChartsVariant::fromColor(color);
    }
  }
}

QVariant
CQChartsColumnColorType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  converted = true;

  if (CQChartsVariant::isColor(var)) {
    bool ok;

    auto c = CQChartsVariant::toColor(var, ok);

    if (ok)
      return c.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnColorType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, ColorColumnData &colorColumnData) const
{
  auto mapped  = colorColumnData.isMapped();
  auto mapMin  = colorColumnData.mapMin();
  auto mapMax  = colorColumnData.mapMax();
  auto palette = colorColumnData.palette();

  if (! getMapData(charts, model, column, nameValues, mapped, mapMin, mapMax, palette))
    return false;

  colorColumnData.setMapped  (mapped);
  colorColumnData.setMapRange(mapMin, mapMax);
  colorColumnData.setPalette (palette);

  return true;
}

bool
CQChartsColumnColorType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, bool &mapped,
           double &mapMin, double &mapMax, PaletteName &paletteName) const
{
  if (! CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped))
    mapped = false;

  QString paletteName1;

  if (nameValueString(nameValues, "palette", paletteName1))
    paletteName = PaletteName(paletteName1);
  else
    paletteName = PaletteName();

  mapMin = 0.0;
  mapMax = 1.0;

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", mapMin)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMin = CQChartsColumnUtil::varReal(columnDetails->minValue(), mapMin);
    }
  }

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", mapMax)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMax = CQChartsColumnUtil::varReal(columnDetails->maxValue(), mapMax);
    }
  }

  return true;
}

//------

CQChartsColumnFontType::
CQChartsColumnFontType() :
 CQChartsColumnType(Type::FONT)
{
}

QString
CQChartsColumnFontType::
desc() const
{
  return CQChartsHtml().
    h2("Font").
     p("Specifies that the column values are font names.");
}

QVariant
CQChartsColumnFontType::
userData(CQCharts *, const QAbstractItemModel *, const Column &,
         const QVariant &var, const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (CQChartsVariant::isFont(var)) {
    return var;
  }
  else {
    auto str = var.toString();

    CQChartsFont font(str);

    if (! font.isValid())
      return var;

    return CQChartsVariant::fromFont(font);
  }
}

QVariant
CQChartsColumnFontType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  converted = true;

  if (CQChartsVariant::isFont(var)) {
    bool ok;

    auto c = CQChartsVariant::toFont(var, ok);

    if (ok)
      return c.toString();
  }

  return var; // TODO: other var formats
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
userData(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isImage(var))
    return var;

  converted = true;

  bool ok;

  auto image = CQChartsVariant::toImage(var, ok);

  if (image.isValid())
    return var;

  return CQChartsVariant::fromImage(image);
}

QVariant
CQChartsColumnImageType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  converted = true;

  if (CQChartsVariant::isImage(var)) {
    bool ok;

    auto image = CQChartsVariant::toImage(var, ok);

    if (ok)
      return image.toString();
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

  addParam("min", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Min), "Map Min", 0.0)->
    setDesc("Override min value for column");
  addParam("max", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Max), "Map Max", 1.0)->
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
userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isSymbol(var))
    return var;

  converted = true;

  bool mapped  = false;
  long min     = 0, max = 1;
  long sizeMin = CQChartsSymbolType::minFillValue();
  long sizeMax = CQChartsSymbolType::maxFillValue();

  getMapData(charts, model, column, typeData.nameValues, mapped, min, max, sizeMin, sizeMax);

  if (mapped) {
    bool ok;

    long i = CQChartsVariant::toInt(var, ok);
    if (! ok) return QVariant();

    long i1 = long(CMathUtil::map(double(i), double(min), double(max),
                                  double(sizeMin), double(sizeMax)));

    auto symbolTypeInd = static_cast<CQChartsSymbolType::Type>(i1);

    if (! CQChartsSymbolType::isValidType(symbolTypeInd))
      return QVariant();

    CQChartsSymbolType symbolType(symbolTypeInd);

    auto symbol = CQChartsSymbol(symbolType);

    return CQChartsVariant::fromSymbol(symbol);
  }
  else {
    auto str = var.toString();

    CQChartsSymbol symbol(str);

    return CQChartsVariant::fromSymbol(symbol);
  }
}

QVariant
CQChartsColumnSymbolTypeType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  converted = true;

  if (CQChartsVariant::isSymbol(var)) {
    bool ok;

    auto symbol = CQChartsVariant::toSymbol(var, ok);

    if (ok)
      return symbol.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnSymbolTypeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, SymbolTypeData &symbolTypeData) const
{
  auto mapped  = symbolTypeData.isMapped();
  auto mapMin  = symbolTypeData.mapMin();
  auto mapMax  = symbolTypeData.mapMax();
  auto dataMin = symbolTypeData.dataMin();
  auto dataMax = symbolTypeData.dataMax();

  if (! getMapData(charts, model, column, nameValues, mapped, mapMin, mapMax, dataMin, dataMax))
    return false;

  symbolTypeData.setMapped   (mapped);
  symbolTypeData.setMapRange (mapMin, mapMax);
  symbolTypeData.setDataRange(dataMin, dataMax);

  return true;
}

bool
CQChartsColumnSymbolTypeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, bool &mapped, long &mapMin, long &mapMax,
           long &dataMin, long &dataMax) const
{
  mapped  = false;
  mapMin  = 0;
  mapMax  = 1;
  dataMin = 0;
  dataMax = 1;

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "min", mapMin)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMin = CQChartsColumnUtil::varInteger(columnDetails->minValue(), mapMin);
    }
  }

  if (! CQChartsColumnUtil::nameValueInteger(nameValues, "max", mapMax)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMax = CQChartsColumnUtil::varInteger(columnDetails->maxValue(), mapMax);
    }
  }

  (void) CQChartsColumnUtil::nameValueInteger(nameValues, "size_min", dataMin);
  (void) CQChartsColumnUtil::nameValueInteger(nameValues, "size_max", dataMax);

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

  addParam("min", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Min), "Map Min", 0.0)->
    setDesc("Override min value for column");
  addParam("max", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Max), "Map Max", 1.0)->
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
userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isReal(var))
    return var;

  bool ok;

  double r = CQChartsVariant::toReal(var, ok);
  if (! ok) return var;

  converted = true;

  bool   mapped  = false;
  double min     = 0.0, max = 1.0;
  double sizeMin = CQChartsSymbolSize::minValue();
  double sizeMax = CQChartsSymbolSize::maxValue();

  getMapData(charts, model, column, typeData.nameValues, mapped, min, max, sizeMin, sizeMax);

  if (mapped) {
    double r1 = CMathUtil::map(r, min, max, sizeMin, sizeMax);

    return CQChartsVariant::fromReal(r1);
  }
  else {
    return CQChartsVariant::fromReal(r);
  }
}

QVariant
CQChartsColumnSymbolSizeType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get real value
  bool   ok;
  double r = 0.0;

  if (CQChartsVariant::isReal(var)) {
    r = CQChartsVariant::toReal(var, ok);
  }
  else {
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
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, SymbolSizeData &symbolSizeData) const
{
  auto mapped  = symbolSizeData.isMapped();
  auto mapMin  = symbolSizeData.mapMin();
  auto mapMax  = symbolSizeData.mapMax();
  auto dataMin = symbolSizeData.dataMin();
  auto dataMax = symbolSizeData.dataMax();

  if (! getMapData(charts, model, column, nameValues, mapped, mapMin, mapMax, dataMin, dataMax))
    return false;

  symbolSizeData.setMapped   (mapped);
  symbolSizeData.setMapRange (mapMin, mapMax);
  symbolSizeData.setDataRange(dataMin, dataMax);

  return true;
}

bool
CQChartsColumnSymbolSizeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, bool &mapped, double &mapMin, double &mapMax,
           double &dataMin, double &dataMax) const
{
  mapped  = false;
  mapMin  = 0.0;
  mapMax  = 1.0;
  dataMin = 0.0;
  dataMax = 1.0;

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", mapMin)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMin = CQChartsColumnUtil::varReal(columnDetails->minValue(), mapMin);
    }
  }

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", mapMax)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMax = CQChartsColumnUtil::varReal(columnDetails->maxValue(), mapMax);
    }
  }

  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_min", dataMin);
  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_max", dataMax);

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

  addParam("min", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Min), "Map Min", 0.0)->
    setDesc("Override min value for column");
  addParam("max", Type::REAL, CQModelUtil::roleCast(CQBaseModelRole::Max), "Map Max", 1.0)->
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
userData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
         const QVariant &var, const ModelTypeData &typeData, bool &converted) const
{
  if (! var.isValid() || CQChartsVariant::isReal(var))
    return var;

  bool ok;

  double r = CQChartsVariant::toReal(var, ok);
  if (! ok) return var;

  converted = true;

  bool   mapped  = false;
  double min     = 0.0, max = 1.0;
  double sizeMin = CQChartsFontSize::minValue();
  double sizeMax = CQChartsFontSize::maxValue();

  getMapData(charts, model, column, typeData.nameValues, mapped, min, max, sizeMin, sizeMax);

  if (mapped) {
    double r1 = CMathUtil::map(r, min, max, sizeMin, sizeMax);

    return CQChartsVariant::fromReal(r1);
  }
  else {
    return CQChartsVariant::fromReal(r);
  }
}

QVariant
CQChartsColumnFontSizeType::
dataName(CQCharts *, const QAbstractItemModel *, const Column &, const QVariant &var,
         const ModelTypeData &, bool &converted) const
{
  if (! var.isValid())
    return var;

  //---

  // get real value
  bool   ok;
  double r = 0.0;

  if (CQChartsVariant::isReal(var)) {
    r = CQChartsVariant::toReal(var, ok);
  }
  else {
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
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, FontSizeData &fontSizeData) const
{
  auto mapped  = fontSizeData.isMapped();
  auto mapMin  = fontSizeData.mapMin();
  auto mapMax  = fontSizeData.mapMax();
  auto dataMin = fontSizeData.dataMin();
  auto dataMax = fontSizeData.dataMax();

  if (! getMapData(charts, model, column, nameValues, mapped, mapMin, mapMax, dataMin, dataMax))
    return false;

  fontSizeData.setMapped   (mapped);
  fontSizeData.setMapRange (mapMin, mapMax);
  fontSizeData.setDataRange(dataMin, dataMax);

  return true;
}

bool
CQChartsColumnFontSizeType::
getMapData(CQCharts *charts, const QAbstractItemModel *model, const Column &column,
           const NameValues &nameValues, bool &mapped, double &mapMin, double &mapMax,
           double &dataMin, double &dataMax) const
{
  mapped  = false;
  mapMin  = 0.0;
  mapMax  = 1.0;
  dataMin = 0.0;
  dataMax = 1.0;

  (void) CQChartsColumnUtil::nameValueBool(nameValues, "mapped", mapped);

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "min", mapMin)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMin = CQChartsColumnUtil::varReal(columnDetails->minValue(), mapMin);
    }
  }

  if (! CQChartsColumnUtil::nameValueReal(nameValues, "max", mapMax)) {
    if (mapped) {
      auto *columnDetails = this->columnDetails(charts, model, column);

      if (columnDetails)
        mapMax = CQChartsColumnUtil::varReal(columnDetails->maxValue(), mapMax);
    }
  }

  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_min", dataMin);
  (void) CQChartsColumnUtil::nameValueReal(nameValues, "size_max", dataMax);

  return true;
}

//---

CQUTIL_DEF_META_TYPE(CQChartsColumnTypeId, toString, fromString)

int CQChartsColumnTypeId::metaTypeId;

void
CQChartsColumnTypeId::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColumnTypeId);

  CQPropertyViewMgrInst->setUserName("CQChartsColumnTypeId", "columnTypeId");
}

CQChartsColumnTypeId::
CQChartsColumnTypeId(CQBaseModelType type) :
 type_(type)
{
}

CQChartsColumnTypeId::
CQChartsColumnTypeId(const QString &s)
{
  if (! fromString(s))
    type_ = CQBaseModelType::STRING;
}

//---

bool
CQChartsColumnTypeId::
fromString(const QString &str)
{
  if (str.trimmed() == "") {
    type_ = CQBaseModelType::STRING;
    return true;
  }

  type_ = CQBaseModel::nameType(str);

  return true;
}

QString
CQChartsColumnTypeId::
toString() const
{
  return CQBaseModel::typeName(type_);
}
