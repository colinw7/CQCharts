#include <CQChartsPlotParameter.h>
#include <CQBaseModel.h>
#include <CQUtil.h>

CQChartsPlotParameter::
CQChartsPlotParameter(const QString &name, const QString &desc, const Type &type,
                      const QString &propName, const Attributes &attributes,
                      const QVariant &defValue) :
 name_(name), desc_(desc), type_(type), propName_(propName), attributes_(attributes),
 defValue_(defValue)
{
  addProperty("name"         , "name"           , "");
  addProperty("desc"         , "desc"           , "");
  addProperty("type"         , "type"           , "");
  addProperty("prop_name"    , "propName"       , "");
  addProperty("group_id"     , "groupId"        , "");
  addProperty("def_value"    , "defValue"       , "");
  addProperty("tip"          , "tip"            , "");
  addProperty("column"       , "isColumn"       , "");
  addProperty("hidden"       , "isHidden"       , "");
  addProperty("multiple"     , "isMultiple"     , "");
  addProperty("optional"     , "isOptional"     , "");
  addProperty("required"     , "isRequired"     , "");
  addProperty("discriminator", "isDiscriminator", "");
  addProperty("monotonic"    , "isMonotonic"    , "");
  addProperty("numeric"      , "isNumeric"      , "");
  addProperty("string"       , "isString"       , "");
  addProperty("bool"         , "isBool"         , "");
  addProperty("color"        , "isColor"        , "");
  addProperty("type_detail"  , "typeDetail"     , "");
  addProperty("groupable"    , "isGroupable"    , "");
  addProperty("mapped"       , "isMapped"       , "");
  addProperty("mapMin"       , "mapMin"         , "");
  addProperty("mapMax"       , "mapMax"         , "");
}

QString
CQChartsPlotParameter::
typeName() const
{
  return CQBaseModel::typeName(type_);
}

CQChartsPlotParameter &
CQChartsPlotParameter::
setTypeName(const QString &name)
{
  type_ = CQBaseModel::nameType(name);

  return *this;
}

void
CQChartsPlotParameter::
addProperty(const QString &name, const QString &propName, const QString &desc)
{
  properties_[name] = PropertyData(name, propName, desc);
}

void
CQChartsPlotParameter::
propertyNames(QStringList &names) const
{
  for (const auto &p : properties_)
    names.push_back(p.first);
}

bool
CQChartsPlotParameter::
hasProperty(const QString &name) const
{
  auto p = properties_.find(name);

  return (p != properties_.end());
}

QVariant
CQChartsPlotParameter::
getPropertyValue(const QString &name) const
{
  auto p = properties_.find(name);

  if (p == properties_.end())
    return QVariant();

  QVariant var;

  if (! CQUtil::getProperty(this, (*p).second.propName, var))
    return QVariant();

  return var;
}
