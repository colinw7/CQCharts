#include <CQChartsColumnType.h>
#include <CQChartsModelData.h>
#include <CQChartsVariant.h>
#include <CQChartsPolygonList.h>
#include <CQChartsConnectionList.h>
#include <CQChartsNamePair.h>
#include <CQCharts.h>
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

CQChartsColumnType *
CQChartsColumnTypeMgr::
decodeTypeData(const QString &typeStr, CQChartsNameValues &nameValues) const
{
  QString baseTypeName;

  CQChartsColumnUtil::decodeType(typeStr, baseTypeName, nameValues);

  Type baseType = CQBaseModel::nameType(baseTypeName);

  CQChartsColumnType *baseTypeData = getType(baseType);

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

CQChartsColumnType *
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
getUserData(QAbstractItemModel *model, const CQChartsColumn &column,
            const QVariant &var, bool &converted) const
{
  converted = false;

  Type               type;
  CQChartsNameValues nameValues;

  if (! getModelColumnType(model, column, type, nameValues))
    return var;

  CQChartsColumnType *typeData = getType(type);

  if (! typeData)
    return var;

  QVariant var1 = typeData->userData(var, nameValues, converted);

  return var1;
}

// convert variant into display data
QVariant
CQChartsColumnTypeMgr::
getDisplayData(QAbstractItemModel *model, const CQChartsColumn &column,
               const QVariant &var, bool &converted) const
{
  converted = false;

  Type               type;
  CQChartsNameValues nameValues;

  if (! getModelColumnType(model, column, type, nameValues))
    return var;

  CQChartsColumnType *typeData = getType(type);

  if (! typeData)
    return var;

  QVariant var1 = typeData->userData(var, nameValues, converted);

  if (! var1.isValid())
    return var;

  QVariant var2 = typeData->dataName(var1, nameValues, converted);

  return var2;
}

bool
CQChartsColumnTypeMgr::
getModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                   Type &type, CQChartsNameValues &nameValues) const
{
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX) {
    type = Type::STRING;
    return true;
  }

  int role = static_cast<int>(CQBaseModel::Role::Type);

  bool ok;

  QVariant var1 = CQChartsUtil::modelHeaderValue(model, column, role, ok);

  if (! var1.isValid())
    return false;

  // validate ?
  type = static_cast<Type>(var1.toInt());

  if (type == Type::NONE)
    return false;

  //---

  int vrole = static_cast<int>(CQBaseModel::Role::TypeValues);

  QVariant var2 = CQChartsUtil::modelHeaderValue(model, column, vrole, ok);

  if (var2.isValid()) {
    QString str2;

    CQChartsVariant::toString(var2, str2);

    (void) CQChartsColumnUtil::decodeNameValues(str2, nameValues);
  }

  //---

  CQChartsColumnType *typeData = getType(type);

  for (const auto &param : typeData->params()) {
    if (param.role() != vrole) {
      bool ok;

      QVariant var = CQChartsUtil::modelHeaderValue(model, column, param.role(), ok);

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

  int role = static_cast<int>(CQBaseModel::Role::Type);

  bool rc1 = CQChartsUtil::setModelHeaderValue(model, column, static_cast<int>(type), role);

  if (rc1)
    changed = true;

  //---

  int vrole = static_cast<int>(CQBaseModel::Role::TypeValues);

  CQChartsNameValues nameValues1;

  CQChartsColumnType *typeData = getType(type);

  for (const auto &param : typeData->params()) {
    auto p = nameValues.find(param.name());
    if (p == nameValues.end()) continue;

    if (param.role() == vrole) {
      nameValues1[param.name()] = (*p).second;
    }
    else {
      bool rc = CQChartsUtil::setModelHeaderValue(model, column, (*p).second, param.role());

      if (rc)
        changed = true;
    }
  }

  if (! nameValues1.empty()) {
    QString str = CQChartsColumnUtil::encodeNameValues(nameValues1);

    bool rc2 = CQChartsUtil::setModelHeaderValue(model, column, str, vrole);

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

//------

CQChartsColumnType::
CQChartsColumnType(Type type) :
 type_(type)
{
  params_.emplace_back("key", Type::BOOLEAN, (int) CQBaseModel::Role::Key, "Is Key");
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

//------

QVariant
CQChartsColumnStringType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::String)
    return var;

  converted = true;

  QString str;

  CQChartsVariant::toString(var, str);

  return QVariant::fromValue<QString>(str);
}

//------

QVariant
CQChartsColumnBooleanType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Bool)
    return var;

  converted = true;

  bool b;

  CQChartsVariant::toBool(var, b);

  return QVariant::fromValue<bool>(b);
}

//------

CQChartsColumnRealType::
CQChartsColumnRealType() :
 CQChartsColumnType(Type::REAL)
{
  params_.emplace_back("format"      , Type::STRING, "Output Format");
  params_.emplace_back("format_scale", Type::REAL  , "Format Scale Factor");

  params_.emplace_back("min", Type::REAL, (int) CQBaseModel::Role::Min, "Min Value");
  params_.emplace_back("max", Type::REAL, (int) CQBaseModel::Role::Max, "Max Value");
}

QVariant
CQChartsColumnRealType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
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
  auto p1 = nameValues.find("format");

  if (p1 == nameValues.end())
    return CQChartsUtil::toString(r);

  QString format = (*p1).second.toString();

  //---

  // get scale factor to support units suffix in format
  auto p2 = nameValues.find("format_scale");

  if (p2 != nameValues.end()) {
    bool ok1;

    double scale = CQChartsVariant::toReal((*p2).second, ok1);

    if (ok1)
      r *= scale;
  }

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
  auto p = nameValues.find("min");
  if (p == nameValues.end()) return false;

  bool ok;

  r = CQChartsVariant::toReal((*p).second, ok);
  if (! ok) return false;

  return true;
}

bool
CQChartsColumnRealType::
rmax(const CQChartsNameValues &nameValues, double &r) const
{
  auto p = nameValues.find("max");
  if (p == nameValues.end()) return false;

  bool ok;

  r = CQChartsVariant::toReal((*p).second, ok);
  if (! ok) return false;

  return true;
}

//------

CQChartsColumnIntegerType::
CQChartsColumnIntegerType() :
 CQChartsColumnType(Type::INTEGER)
{
  params_.emplace_back("format", Type::INTEGER, "Output Format");

  params_.emplace_back("min", Type::REAL, (int) CQBaseModel::Role::Min, "Min Value");
  params_.emplace_back("max", Type::REAL, (int) CQBaseModel::Role::Max, "Max Value");
}

QVariant
CQChartsColumnIntegerType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
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
  auto p1 = nameValues.find("format");

  if (p1 == nameValues.end())
    return CQChartsUtil::toString(l);

  QString format = (*p1).second.toString();

  //---

  // convert value using format
  return CQChartsUtil::toString(l, format);
}

//------

CQChartsColumnTimeType::
CQChartsColumnTimeType() :
 CQChartsColumnType(Type::TIME)
{
  params_.emplace_back("format" , Type::STRING, "Input/Output Format");
  params_.emplace_back("iformat", Type::STRING, "Input Format");
  params_.emplace_back("oformat", Type::STRING, "Output Format");
}

QVariant
CQChartsColumnTimeType::
userData(const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &nameValues, bool &converted) const
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
  auto p1 = nameValues.find("iformat");

  if (p1 != nameValues.end())
    return (*p1).second.toString();

  auto p2 = nameValues.find("format");

  if (p2 != nameValues.end())
    return (*p2).second.toString();

  return "";
}

QString
CQChartsColumnTimeType::
getOFormat(const CQChartsNameValues &nameValues) const
{
  auto p1 = nameValues.find("oformat");

  if (p1 != nameValues.end())
    return (*p1).second.toString();

  auto p2 = nameValues.find("format");

  if (p2 != nameValues.end())
    return (*p2).second.toString();

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

QVariant
CQChartsColumnRectType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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

QVariant
CQChartsColumnPolygonType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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

QVariant
CQChartsColumnPolygonListType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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

QVariant
CQChartsColumnConnectionListType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  if (isConnectionListVariant(var))
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsConnectionList connectionList(str);

  return QVariant::fromValue<CQChartsConnectionList>(connectionList);
}

QVariant
CQChartsColumnConnectionListType::
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (isConnectionListVariant(var)) {
    CQChartsConnectionList connectionList = var.value<CQChartsConnectionList>();

    return connectionList.toString();
  }

  return var; // TODO: other var formats
}

bool
CQChartsColumnConnectionListType::
isConnectionListVariant(const QVariant &var) const
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

QVariant
CQChartsColumnNamePairType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
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

QVariant
CQChartsColumnPathType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.userType() == CQChartsPath::metaType())
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsPath path;

  (void) CQChartsUtil::stringToPath(str, path);

  return QVariant::fromValue<CQChartsPath>(path);
}

QVariant
CQChartsColumnPathType::
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.userType() == CQChartsPath::metaType()) {
    CQChartsPath path = var.value<CQChartsPath>();

    return CQChartsUtil::pathToString(path);
  }

  return var; // TODO: other var formats
}

//------

QVariant
CQChartsColumnStyleType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.userType() == CQChartsStyle::metaType())
    return var;

  converted = true;

  QString str = var.toString();

  CQChartsStyle style;

  (void) CQChartsUtil::stringToStyle(str, style);

  return QVariant::fromValue<CQChartsStyle>(style);
}

QVariant
CQChartsColumnStyleType::
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.userType() == CQChartsStyle::metaType()) {
    CQChartsStyle style = var.value<CQChartsStyle>();

    return CQChartsUtil::styleToString(style);
  }

  return var; // TODO: other var formats
}

//------

QVariant
CQChartsColumnColorType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Color)
    return var;

  converted = true;

  QString str = var.toString();

  QColor c(str);

  return c;
}

QVariant
CQChartsColumnColorType::
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  converted = true;

  if (var.type() == QVariant::Color) {
    QColor c = var.value<QColor>();

    return c.name();
  }

  return var; // TODO: other var formats
}
