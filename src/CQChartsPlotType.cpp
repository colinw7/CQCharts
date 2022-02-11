#include <CQChartsPlotType.h>
#include <CQChartsPlot.h>
#include <CQUtil.h>

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
  CQCHARTS_QASSERT(nameTypes_.find(name) == nameTypes_.end(), "Type already exists: " + name);

  nameTypes_[name] = type;

  CQCHARTS_QASSERT(typeNameNames_.find(type->name()) == typeNameNames_.end(),
                   "Plot Type and name mismatch: " + name);

  typeNameNames_[type->name()] = name;

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
  CQCHARTS_QASSERT(p != nameTypes_.end(), "Invalid type: " + name);

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
  addProperty("xaxis"                , "hasXAxis"          , "");
  addProperty("yaxis"                , "hasYAxis"          , "");
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
  for (auto &ng : parameterGroups_)
    delete ng.second;

  for (auto &parameter : parameters_)
    delete parameter;
}

void
CQChartsPlotType::
addParameters()
{
  startParameterGroup("Common");

  addColumnParameter("id", "Id", "idColumn").
   setStringColumn().setTip("Row Unique Id Column");

  addColumnsParameter("tips", "Tips", "tipColumns").
   setStringColumn().setBasic().setTip("Row Tip Columns");

  addColumnsParameter("notips", "No Tips", "noTipColumns").
   setStringColumn().setBasic().setTip("Row No Tip Columns");

  addColumnParameter("visible", "Visible", "visibleColumn").
   setBoolColumn().setTip("Row Visible Column");

  addColumnParameter("color", "Color", "colorColumn").
   setTip("Row Color Column").setMapped();

  addColumnParameter("alpha", "Alpha", "alphaColumn").
   setTip("Row Alpha Column").setMapped();

  addColumnParameter("font", "Font", "fontColumn").
   setTip("Row Font Column");

  addColumnParameter("image", "Image", "imageColumn").
   setTip("Row Image Column").setMapped();

  addColumnsParameter("controls", "Controls", "controlColumns").
    setStringColumn().setBasic().setTip("Control Columns");

  if (hasKey()) {
    addBoolParameter("key", "Key", "keyVisible", true).setTip("Show Key");

    addBoolParameter("colorKey", "Use Color Column for Key", "colorKey").
      setTip("Use color column for key");
  }

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

const CQChartsPlotParameter *
CQChartsPlotType::
getParameter(const QString &name) const
{
  for (auto &parameter : parameters_)
    if (parameter->name() == name)
      return parameter;

  CQCHARTS_QASSERT(false, "Invalid parameter name: " + name);

  return nullptr;
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

CQChartsPlotType::ParameterGroups
CQChartsPlotType::
groupParameterGroups(int groupId) const
{
  ParameterGroups parameterGroups;

  for (auto &ig : parameterGroups_) {
    auto *parameterGroup = ig.second;

    if (parameterGroup->parentGroupId() == groupId)
      parameterGroups.push_back(parameterGroup);
  }

  return parameterGroups;
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

CQChartsPlotParameterGroup *
CQChartsPlotType::
startParameterGroup(const QString &name)
{
  auto parameterGroupId = int(parameterGroups_.size());

  auto *parameterGroup = new CQChartsPlotParameterGroup(name, parameterGroupId);

  parameterGroups_[parameterGroupId] = parameterGroup;

  if (! parameterGroupIds_.empty()) {
    int parentGroupId = parameterGroupIds_.back();

    parameterGroup->setParentGroupId(parentGroupId);
  }

  parameterGroupIds_.push_back(parameterGroupId);

  return parameterGroup;
}

void
CQChartsPlotType::
endParameterGroup()
{
  CQCHARTS_ASSERT(! parameterGroupIds_.empty(), "No parameter groups");

  parameterGroupIds_.pop_back();
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

  auto *parameter1 = parameters_.back();

  if (! parameterGroupIds_.empty()) {
    int parameterGroupId = parameterGroupIds_.back();

    parameter1->setGroupId(parameterGroupId);
  }

  return *parameter1;
}

CQChartsPlotParameter *
CQChartsPlotType::
getColumnParameter(const QString &name) const
{
  for (const auto &parameter : parameters()) {
    if (parameter->type() != CQChartsPlotParameter::Type::COLUMN &&
        parameter->type() != CQChartsPlotParameter::Type::COLUMN_LIST)
      continue;

    if (parameter->name() == name)
      return parameter;
  }

  return nullptr;
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

CQChartsPlot *
CQChartsPlotType::
createAndInit(View *view, const ModelP &model) const
{
  auto *plot = create(view, model);

  plot->init();

  return plot;
}
