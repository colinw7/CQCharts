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
  CQColorsTheme *obj = CQColorsMgrInst->getNamedTheme(name);
  if (! obj) return false;

  name_ = name;
  obj_  = obj;

  return true;
}
