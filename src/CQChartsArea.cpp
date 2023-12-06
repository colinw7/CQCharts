#include <CQChartsArea.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsArea, toString, fromString)

int CQChartsArea::metaTypeId;

void
CQChartsArea::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsArea);

  CQPropertyViewMgrInst->setUserName("CQChartsArea", "area");
}

bool
CQChartsArea::
setValue(const QString &str, const Units &defUnits)
{
  Units  units;
  double value;

  if (! decodeString(str, units, value, defUnits))
    return false;

  units_ = units;
  value_ = value;

  return true;
}

QString
CQChartsArea::
toString() const
{
  auto ustr = CQChartsUnits::unitsString(units_);

  return QString("%1%2").arg(value_).arg(ustr);
}

bool
CQChartsArea::
fromString(const QString &s)
{
  if (s.trimmed() == "") {
    *this = CQChartsArea();
    return true;
  }

  return setValue(s);
}

bool
CQChartsArea::
decodeString(const QString &str, Units &units, double &value, const Units &defUnits)
{
  // format is <area> [<units>]

  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if (strs.length() < 1)
    return false;

  //---

  if (strs.length() == 1) {
    auto str0 = strs[0];

    for (int i = 0; i < str0.length(); ++i) {
      if (! str0[i].isLetter() && str0[i] != '%') continue;

      Units units1;

      if (CQChartsUnits::decodeUnits(str0.mid(i), units1, defUnits)) {
        auto strl = str0.mid(0, i);

        double value1;

        if (CQChartsUtil::toReal(strl, value1)) {
          value = value1;
          units = units1;
          return true;
        }
      }
    }
  }

  //---

  if (! CQChartsUtil::toReal(strs[0], value))
    return false;

  units = defUnits;

  if (strs.length() > 1) {
    if (! CQChartsUnits::decodeUnits(strs[1], units, defUnits))
      return false;
  }

  return true;
}
