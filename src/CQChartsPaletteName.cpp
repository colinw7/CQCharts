#include <CQChartsPaletteName.h>
#include <CQPropertyView.h>
#include <CQColors.h>

CQUTIL_DEF_META_TYPE(CQChartsPaletteName, toString, fromString)

int CQChartsPaletteName::metaTypeId;

CQChartsPaletteName::
CQChartsPaletteName(const QString &name) :
 name_(name)
{
}

bool
CQChartsPaletteName::
isValid() const
{
  return (name_.simplified() != "");
}

bool
CQChartsPaletteName::
setName(const QString &name)
{
  auto *palette = CQColorsMgrInst->getNamedPalette(name);
  if (! palette) return false;

  name_ = name;

  return true;
}

CQColorsPalette *
CQChartsPaletteName::
palette() const
{
  auto *palette = CQColorsMgrInst->getNamedPalette(name());

  return palette;
}

void
CQChartsPaletteName::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPaletteName);

  CQPropertyViewMgrInst->setUserName("CQChartsPaletteName", "paletteName");
}
