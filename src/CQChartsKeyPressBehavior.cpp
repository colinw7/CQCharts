#include <CQChartsKeyPressBehavior.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsKeyPressBehavior, toString, fromString)

int CQChartsKeyPressBehavior::metaTypeId;

void
CQChartsKeyPressBehavior::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsKeyPressBehavior);

  CQPropertyViewMgrInst->setUserName("CQChartsKeyPressBehavior", "key_press_behavior");
}

QString
CQChartsKeyPressBehavior::
toString() const
{
  switch (type_) {
    case Type::SHOW  : return "SHOW";
    case Type::SELECT: return "SELECT";
    default          : return "NONE";
  }
}

bool
CQChartsKeyPressBehavior::
fromString(const QString &str)
{
  Type type = Type::NONE;

  QString lstr = str.toLower();

  if      (lstr == "show"  ) type = Type::SHOW;
  else if (lstr == "select") type = Type::SELECT;
  else                       return false;

  type_ = type;

  return true;
}

QStringList
CQChartsKeyPressBehavior::
enumNames() const
{
  static QStringList names = QStringList() << "SHOW" << "SELECT";

  return names;
}
