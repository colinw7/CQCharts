#include <CQChartsMargin.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsMargin, toString, fromString)

int CQChartsMargin::metaTypeId;

void
CQChartsMargin::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsMargin);

  CQPropertyViewMgrInst->setUserName("CQChartsMargin", "margin");
}

QString
CQChartsMargin::
toString() const
{
  QStringList strs;

  if (left() != top() || left() != right() || left() != bottom()) {
    strs << QString::number(left  ().value());
    strs << QString::number(top   ().value());
    strs << QString::number(right ().value());
    strs << QString::number(bottom().value());
  }
  else {
    strs << QString::number(left().value());
  }

  if (units() != Units::PERCENT)
    strs << CQChartsUnits::unitsString(units());

  return CQTcl::mergeList(strs);
}

bool
CQChartsMargin::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  auto units = Units::PERCENT;

  auto nstrs = strs.length();

  if (nstrs == 2 || nstrs == 5) {
    if (! CQChartsUnits::decodeUnits(strs[nstrs - 1], units, units))
      return false;

    --nstrs;
  }

  if      (nstrs == 1) {
    CQChartsLength length;

    if (! length.setValue(strs[0], units)) return false;

    set(length);
  }
  else if (nstrs == 4) {
    CQChartsLength left, top, right, bottom;

    if (! left  .setValue(strs[0], units)) return false;
    if (! top   .setValue(strs[1], units)) return false;
    if (! right .setValue(strs[2], units)) return false;
    if (! bottom.setValue(strs[3], units)) return false;

    set(left, top, right, bottom);
  }
  else
    return false;

  return true;
}
