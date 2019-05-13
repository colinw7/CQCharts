#include <CQChartsAxisValueType.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsAxisValueType, toString, fromString)

int CQChartsAxisValueType::metaTypeId;

void
CQChartsAxisValueType::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsAxisValueType);

  CQPropertyViewMgrInst->setUserName("CQChartsAxisValueType", "axis_value_type");
}

QString
CQChartsAxisValueType::
toString() const
{
  switch (type_) {
    case Type::REAL   : return "REAL";
    case Type::INTEGER: return "INTEGER";
    case Type::DATE   : return "DATE";
    case Type::LOG    : return "LOG";
    default           : return "NONE";
  }
}

bool
CQChartsAxisValueType::
fromString(const QString &str)
{
  Type type = Type::NONE;

  if      (str == "REAL"   ) type = Type::REAL;
  else if (str == "INTEGER") type = Type::INTEGER;
  else if (str == "DATE"   ) type = Type::DATE;
  else if (str == "LOG"    ) type = Type::LOG;
  else                       return false;

  type_ = type;

  return true;
}

QStringList
CQChartsAxisValueType::
enumNames() const
{
  static QStringList names = QStringList() << "REAL" << "INTEGER" << "DATE" << "LOG";

  return names;
}
