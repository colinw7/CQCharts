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
addType(Type type, CQChartsColumnType *data)
{
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

    CQChartsUtil::variantToString(var2, str2);

    (void) CQChartsColumnUtil::decodeNameValues(str2, nameValues);
  }

  return true;
}

bool
CQChartsColumnTypeMgr::
setModelColumnType(QAbstractItemModel *model, const CQChartsColumn &column,
                   Type type, const CQChartsNameValues &nameValues)
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

//------

QVariant
CQChartsColumnStringType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::String)
    return var;

  converted = true;

  QString str;

  CQChartsUtil::variantToString(var, str);

  return QVariant::fromValue<QString>(str);
}

//------

QVariant
CQChartsColumnRealType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Double)
    return var;

  bool ok;

  double r = CQChartsUtil::toReal(var, ok);

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

  if (var.type() == QVariant::Double)
    return CQChartsUtil::toString(var.value<double>());

  bool ok;

  double r = CQChartsUtil::toReal(var, ok);

  if (! ok)
    return CQChartsUtil::varToString(var, ok);

  converted = true;

  // optional format for real
  auto p1 = nameValues.find("format");

  if (p1 == nameValues.end())
    return CQChartsUtil::toString(r);

  // support units suffix with scale factor and format
  auto p2 = nameValues.find("format_scale");

  if (p2 != nameValues.end()) {
    bool ok1;

    double scale = CQChartsUtil::toReal((*p2).second, ok1);

    if (ok1)
      r *= scale;
  }

  // convert value using format
  return CQChartsUtil::toString(r, (*p1).second);
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

  r = CQChartsUtil::toReal((*p).second, ok);
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

  r = CQChartsUtil::toReal((*p).second, ok);
  if (! ok) return false;

  return true;
}

//------

QVariant
CQChartsColumnIntegerType::
userData(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid() || var.type() == QVariant::Int)
    return var;

  bool ok;

  long l = CQChartsUtil::toInt(var, ok);

  if (! ok)
    return var;

  converted = true;

  return QVariant::fromValue<int>(l);
}

QVariant
CQChartsColumnIntegerType::
dataName(const QVariant &var, const CQChartsNameValues &, bool &converted) const
{
  if (! var.isValid())
    return var;

  if (var.type() == QVariant::Int) {
    bool ok;

    long l = CQChartsUtil::toInt(var, ok);

    return CQChartsUtil::toString(l);
  }

  bool ok;

  long l = CQChartsUtil::toInt(var, ok);

  if (! ok)
    return CQChartsUtil::varToString(var, ok);

  converted = true;

  return CQChartsUtil::toString(l);
}

//------

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

  double t = CQChartsUtil::toReal(var, ok);

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
    return (*p1).second;

  auto p2 = nameValues.find("format");

  if (p2 != nameValues.end())
    return (*p2).second;

  return "";
}

QString
CQChartsColumnTimeType::
getOFormat(const CQChartsNameValues &nameValues) const
{
  auto p1 = nameValues.find("oformat");

  if (p1 != nameValues.end())
    return (*p1).second;

  auto p2 = nameValues.find("format");

  if (p2 != nameValues.end())
    return (*p2).second;

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

  double t = CQChartsUtil::toReal(var, ok);

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
