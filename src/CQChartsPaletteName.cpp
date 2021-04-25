#include <CQChartsPaletteName.h>
#include <CQPropertyView.h>
#include <CQColors.h>

CQUTIL_DEF_META_TYPE(CQChartsPaletteName, toString, fromString)

int CQChartsPaletteName::metaTypeId;

CQChartsPaletteName::
CQChartsPaletteName(const QString &str)
{
  fromString(str);
}

bool
CQChartsPaletteName::
isValid() const
{
  return (name_ != "");
}

bool
CQChartsPaletteName::
setName(const QString &name)
{
  if (name.trimmed() != "") {
    auto *palette = CQColorsMgrInst->getNamedPalette(name);
    if (! palette) return false;

    name_ = name;
  }
  else
    name_ = "";

  return true;
}

CQColorsPalette *
CQChartsPaletteName::
palette() const
{
  auto *palette = CQColorsMgrInst->getNamedPalette(name());

  return palette;
}

QString
CQChartsPaletteName::
toString() const
{
  auto str = name();

  if (str != "") {
    if (min_ != 0.0 || max_ != 1.0)
      str += QString("[%1,%2]").arg(min_).arg(max_);
  }

  return str;
}

bool
CQChartsPaletteName::
fromString(const QString &name)
{
  if (name.trimmed() != "") {
    auto lhs = name;

    QString rhs;

    double min = 0.0;
    double max = 1.0;

    auto pos = lhs.indexOf("[");

    if (pos >= 0) {
      rhs = lhs.mid(pos + 1);
      lhs = lhs.mid(0, pos);

      auto pos1 = rhs.indexOf(",");
      if (pos1 < 0) return false;

      auto minStr = rhs.mid(0, pos1);

      rhs = rhs.mid(pos1 + 1);

      auto pos2 = rhs.indexOf("]");
      if (pos2 < 0) return false;

      auto maxStr = rhs.mid(0, pos2);

      bool ok;
      min = minStr.toDouble(&ok); if (! ok) return false;
      max = maxStr.toDouble(&ok); if (! ok) return false;
    }

    if (! setName(lhs))
      return false;

    min_ = min;
    max_ = max;
  }
  else {
    name_ = "";
    min_  = 0.0;
    max_  = 1.0;
  }

  return true;
}

void
CQChartsPaletteName::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPaletteName);

  CQPropertyViewMgrInst->setUserName("CQChartsPaletteName", "paletteName");
}
