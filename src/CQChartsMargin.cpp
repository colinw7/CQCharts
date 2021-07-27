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

  strs << left  ().toString();
  strs << top   ().toString();
  strs << right ().toString();
  strs << bottom().toString();

  if (strs[0] == strs[1] && strs[0] == strs[2] && strs[0] == strs[3])
    return strs[0];

  return CQTcl::mergeList(strs);
}

bool
CQChartsMargin::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if      (strs.length() == 1) {
    CQChartsLength length;

    if (! length.setValue(strs[0], Units::PERCENT)) return false;

    set(length);
  }
  else if (strs.length() == 4) {
    CQChartsLength left, top, right, bottom;

    if (! left  .setValue(strs[0], Units::PERCENT)) return false;
    if (! top   .setValue(strs[1], Units::PERCENT)) return false;
    if (! right .setValue(strs[2], Units::PERCENT)) return false;
    if (! bottom.setValue(strs[3], Units::PERCENT)) return false;

    set(left, top, right, bottom);
  }
  else
    return false;

  return true;
}
