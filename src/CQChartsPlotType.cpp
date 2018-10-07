#include <CQChartsPlotType.h>
#include <CQUtil.h>
#include <cassert>

CQChartsPlotTypeMgr::
CQChartsPlotTypeMgr()
{
}

CQChartsPlotTypeMgr::
~CQChartsPlotTypeMgr()
{
  for (auto &type : nameTypes_)
    delete type.second;
}

void
CQChartsPlotTypeMgr::
addType(const QString &name, CQChartsPlotType *type)
{
  nameTypes_[name] = type;

  type->addParameters();
}

bool
CQChartsPlotTypeMgr::
isType(const QString &name) const
{
  auto p = nameTypes_.find(name);

  return (p != nameTypes_.end());
}

CQChartsPlotType *
CQChartsPlotTypeMgr::
type(const QString &name) const
{
  auto p = nameTypes_.find(name);
  assert(p != nameTypes_.end());

  return (*p).second;
}

void
CQChartsPlotTypeMgr::
getTypes(Types &types) const
{
  for (const auto &type : nameTypes_)
    types.push_back(type.second);
}

void
CQChartsPlotTypeMgr::
getTypeNames(QStringList &names, QStringList &descs) const
{
  for (const auto &type : nameTypes_) {
    names.push_back(type.first);
    descs.push_back(type.second->desc());
  }
}

//------

CQChartsPlotType::
CQChartsPlotType()
{
  addProperty("name"                 , "name"              , "");
  addProperty("desc"                 , "desc"              , "");
  addProperty("html_desc"            , "htmlDesc"          , "");
  addProperty("dimension"            , "dimension"         , "");
  addProperty("x_column"             , "xColumnName"       , "");
  addProperty("y_column"             , "yColumnName"       , "");
  addProperty("custom_x_range"       , "customXRange"      , "");
  addProperty("custom_y_range"       , "customXRange"      , "");
  addProperty("axes"                 , "hasAxes"           , "");
  addProperty("key"                  , "hasKey"            , "");
  addProperty("title"                , "hasTitle"          , "");
  addProperty("allow_x_axis_integral", "allowXAxisIntegral", "");
  addProperty("allow_y_axis_integral", "allowYAxisIntegral", "");
  addProperty("allow_x_log"          , "allowXLog"         , "");
  addProperty("allow_y_log"          , "allowYLog"         , "");
  addProperty("is_group"             , "isGroupType"       , "");
  addProperty("hierarchical"         , "isHierarchical"    , "");
}

CQChartsPlotType::
~CQChartsPlotType()
{
  for (auto &parameter : parameters_)
    delete parameter;
}

void
CQChartsPlotType::
addParameters()
{
  startParameterGroup("Common");

  addColumnParameter("id", "Id", "idColumn").
    setString().setTip("Unique row id");

  addColumnParameter("tip", "Tip", "tipColumn").
    setString().setTip("Tip Column");

  if (hasKey())
    addBoolParameter("key", "Key", "keyVisible").setTip("Show Key");

  endParameterGroup();
}

bool
CQChartsPlotType::
hasParameter(const QString &name) const
{
  for (auto &parameter : parameters_)
    if (parameter->name() == name)
      return true;

  return false;
}

const CQChartsPlotParameter &
CQChartsPlotType::
getParameter(const QString &name) const
{
  for (auto &parameter : parameters_)
    if (parameter->name() == name)
      return *parameter;

  assert(false);

  return *parameters_[0];
}

CQChartsPlotType::Parameters
CQChartsPlotType::
groupParameters(int groupId) const
{
  Parameters parameters;

  for (auto &parameter : parameters_) {
    if (parameter->groupId() == groupId)
      parameters.push_back(parameter);
  }

  return parameters;
}

CQChartsPlotType::Parameters
CQChartsPlotType::
nonGroupParameters() const
{
  Parameters parameters;

  for (auto &parameter : parameters_) {
    if (parameter->groupId() == -1)
      parameters.push_back(parameter);
  }

  return parameters;
}

void
CQChartsPlotType::
startParameterGroup(const QString &name)
{
  parameterGroupId_ = parameterGroups_.size();

  parameterGroups_[parameterGroupId_] = CQChartsPlotParameterGroup(name, parameterGroupId_);
}

void
CQChartsPlotType::
endParameterGroup()
{
  parameterGroupId_ = -1;
}

CQChartsPlotParameter &
CQChartsPlotType::
addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                   int defValue)
{
  return addColumnParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsPlotParameter &
CQChartsPlotType::
addColumnParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, int defValue)
{
  return addParameter(new CQChartsColumnParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                    const QString &defValue)
{
  return addColumnsParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsPlotParameter &
CQChartsPlotType::
addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                    const ParameterAttributes &attributes, const QString &defValue)
{
  return addParameter(new CQChartsColumnsParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addStringParameter(const QString &name, const QString &desc, const QString &propName,
                   const QString &defValue)
{
  return addStringParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsPlotParameter &
CQChartsPlotType::
addStringParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, const QString &defValue)
{
  return addParameter(new CQChartsStringParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addRealParameter(const QString &name, const QString &desc, const QString &propName,
                 double defValue)
{
  return addRealParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsPlotParameter &
CQChartsPlotType::
addRealParameter(const QString &name, const QString &desc, const QString &propName,
                 const ParameterAttributes &attributes, double defValue)
{
  return addParameter(new CQChartsRealParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addIntParameter(const QString &name, const QString &desc, const QString &propName,
                int defValue)
{
  return addIntParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsPlotParameter &
CQChartsPlotType::
addIntParameter(const QString &name, const QString &desc, const QString &propName,
                const ParameterAttributes &attributes, int defValue)
{
  return addParameter(new CQChartsIntParameter(name, desc, propName, attributes, defValue));
}

CQChartsEnumParameter &
CQChartsPlotType::
addEnumParameter(const QString &name, const QString &desc, const QString &propName, bool defValue)
{
  return addEnumParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsEnumParameter &
CQChartsPlotType::
addEnumParameter(const QString &name, const QString &desc, const QString &propName,
                 const ParameterAttributes &attributes, bool defValue)
{
  return static_cast<CQChartsEnumParameter &>
    (addParameter(new CQChartsEnumParameter(name, desc, propName, attributes, defValue)));
}

CQChartsPlotParameter &
CQChartsPlotType::
addBoolParameter(const QString &name, const QString &desc, const QString &propName, bool defValue)
{
  return addBoolParameter(name, desc, propName, ParameterAttributes(), defValue);
}

CQChartsPlotParameter &
CQChartsPlotType::
addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                 const ParameterAttributes &attributes, bool defValue)
{
  return addParameter(new CQChartsBoolParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addParameter(CQChartsPlotParameter *parameter)
{
  parameters_.push_back(parameter);

  CQChartsPlotParameter *parameter1 = parameters_.back();

  if (parameterGroupId_ >= 0)
    parameter1->setGroupId(parameterGroupId_);

  return *parameter1;
}

void
CQChartsPlotType::
addProperty(const QString &name, const QString &propName, const QString &desc)
{
  properties_[name] = PropertyData(name, propName, desc);
}

void
CQChartsPlotType::
propertyNames(QStringList &names) const
{
  for (const auto &p : properties_)
    names.push_back(p.first);
}

bool
CQChartsPlotType::
hasProperty(const QString &name) const
{
  auto p = properties_.find(name);

  return (p != properties_.end());
}

QVariant
CQChartsPlotType::
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
