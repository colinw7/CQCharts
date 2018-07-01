#include <CQChartsPlotType.h>
#include <QStringList>
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
    names.push_back(type.second->name());
    descs.push_back(type.second->desc());
  }
}

//------

CQChartsPlotType::
CQChartsPlotType()
{
}

void
CQChartsPlotType::
addParameters()
{
  startParameterGroup("Common");

  addColumnParameter("id", "Id", "idColumn").setTip("Unique row id");

  if (hasKey())
    addBoolParameter("key", "Key", "keyVisible").setTip("Show Key");

  endParameterGroup();
}

CQChartsPlotType::Parameters
CQChartsPlotType::
groupParameters(int groupId) const
{
  Parameters parameters;

  for (auto &parameter : parameters_) {
    if (parameter.groupId() == groupId)
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
    if (parameter.groupId() == -1)
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
  return addParameter(CQChartsColumnParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                    const QString &defValue)
{
  return addParameter(CQChartsColumnsParameter(name, desc, propName, ParameterAttributes(),
                                               defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addColumnsParameter(const QString &name, const QString &desc, const QString &propName,
                    const ParameterAttributes &attributes, const QString &defValue)
{
  return addParameter(CQChartsColumnsParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addStringParameter(const QString &name, const QString &desc, const QString &propName,
                   const QString &defValue)
{
  return addParameter(CQChartsStringParameter(name, desc, propName, ParameterAttributes(),
                                              defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addStringParameter(const QString &name, const QString &desc, const QString &propName,
                   const ParameterAttributes &attributes, const QString &defValue)
{
  return addParameter(CQChartsStringParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addRealParameter(const QString &name, const QString &desc, const QString &propName,
                 double defValue)
{
  return addParameter(CQChartsRealParameter(name, desc, propName, ParameterAttributes(),
                                            defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addRealParameter(const QString &name, const QString &desc, const QString &propName,
                 const ParameterAttributes &attributes, double defValue)
{
  return addParameter(CQChartsRealParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addIntParameter(const QString &name, const QString &desc, const QString &propName,
                int defValue)
{
  return addParameter(CQChartsIntParameter(name, desc, propName, ParameterAttributes(),
                                           defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addIntParameter(const QString &name, const QString &desc, const QString &propName,
                const ParameterAttributes &attributes, int defValue)
{
  return addParameter(CQChartsIntParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addBoolParameter(const QString &name, const QString &desc, const QString &propName, bool defValue)
{
  return addParameter(CQChartsBoolParameter(name, desc, propName, ParameterAttributes(),
                                            defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addBoolParameter(const QString &name, const QString &desc, const QString &propName,
                 const ParameterAttributes &attributes, bool defValue)
{
  return addParameter(CQChartsBoolParameter(name, desc, propName, attributes, defValue));
}

CQChartsPlotParameter &
CQChartsPlotType::
addParameter(const CQChartsPlotParameter &parameter)
{
  parameters_.push_back(parameter);

  CQChartsPlotParameter &parameter1 = parameters_.back();

  if (parameterGroupId_ >= 0)
    parameter1.setGroupId(parameterGroupId_);

  return parameter1;
}
