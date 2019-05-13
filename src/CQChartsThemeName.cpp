#include <CQChartsThemeName.h>
#include <CQChartsTheme.h>
#include <CQPropertyView.h>

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
  CQChartsTheme *obj = CQChartsThemeMgrInst->getTheme(name);
  if (! obj) return false;

  name_ = name;
  obj_  = obj;

  return true;
}
