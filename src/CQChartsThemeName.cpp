#include <CQChartsThemeName.h>
#include <CQPropertyView.h>
#include <CQColors.h>

CQUTIL_DEF_META_TYPE(CQChartsThemeName, toString, fromString)

int CQChartsThemeName::metaTypeId;

void
CQChartsThemeName::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsThemeName);

  CQPropertyViewMgrInst->setUserName("CQChartsThemeName", "theme_name");
}

bool
CQChartsThemeName::
setName(const QString &name)
{
  auto *obj = CQColorsMgrInst->getNamedTheme(name);
  if (! obj) return false;

  name_ = name;
  obj_  = obj;

  return true;
}

QString
CQChartsThemeName::
toString() const
{
  return name();
}

bool
CQChartsThemeName::
fromString(const QString &s)
{
  if (s.trimmed() == "") {
    *this = CQChartsThemeName();
    return true;
  }

  return setName(s);
}
