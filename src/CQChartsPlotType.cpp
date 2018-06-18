#include <CQChartsPlotType.h>
#include <cassert>

CQChartsPlotTypeMgr::
CQChartsPlotTypeMgr()
{
}

CQChartsPlotTypeMgr::
~CQChartsPlotTypeMgr()
{
  for (auto &type : types_)
    delete type.second;
}

void
CQChartsPlotTypeMgr::
addType(const QString &name, CQChartsPlotType *type)
{
  types_[name] = type;

  type->addParameters();
}

bool
CQChartsPlotTypeMgr::
isType(const QString &name) const
{
  auto p = types_.find(name);

  return (p != types_.end());
}

CQChartsPlotType *
CQChartsPlotTypeMgr::
type(const QString &name) const
{
  auto p = types_.find(name);
  assert(p != types_.end());

  return (*p).second;
}

void
CQChartsPlotTypeMgr::
getTypeNames(QStringList &names, QStringList &descs) const
{
  for (const auto &type : types_) {
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
  addColumnParameter("id", "Id", "idColumn").setTip("Unique row id");

  if (hasKey())
    addBoolParameter("key", "Key", "keyVisible").setTip("Show Key");
}
