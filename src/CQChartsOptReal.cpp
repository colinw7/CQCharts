#include <CQChartsOptReal.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsOptReal, toString, fromString)

int CQChartsOptReal::metaTypeId;

bool
CQChartsOptReal::
setValue(const QString &s)
{
  if (s.simplified().length() == 0)
    value_ = OptReal();
  else {
    bool ok;

    double r = CQChartsUtil::toReal(s, ok);

    if (! ok)
      return false;

    value_ = r;
  }

  return true;
}

void
CQChartsOptReal::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsOptReal);

  CQPropertyViewMgrInst->setUserName("CQChartsOptReal", "optional_real");
}
