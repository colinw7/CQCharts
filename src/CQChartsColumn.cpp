#include <CQChartsColumn.h>

CQChartsColumnTypeMgr::
CQChartsColumnTypeMgr()
{
}

CQChartsColumnTypeMgr::
~CQChartsColumnTypeMgr()
{
  for (auto &nameType : nameType_)
    delete nameType.second;
}

void
CQChartsColumnTypeMgr::
addType(const QString &name, CQChartsColumnType *type)
{
  nameType_[name] = type;
}

CQChartsColumnType *
CQChartsColumnTypeMgr::
decodeTypeData(const QString &type, CQChartsNameValues &nameValues) const
{
  QString baseType;

  CQChartsColumn::decodeType(type, baseType, nameValues);

  CQChartsColumnType *typeData = getType(baseType);

  return typeData;
}

CQChartsColumnType *
CQChartsColumnTypeMgr::
getType(const QString &name) const
{
  auto p = nameType_.find(name);

  if (p == nameType_.end())
    return nullptr;

  return (*p).second;
}
