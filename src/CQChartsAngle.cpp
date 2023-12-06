#include <CQChartsAngle.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsAngle, toString, fromString)

int CQChartsAngle::metaTypeId;

void
CQChartsAngle::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsAngle);

  CQPropertyViewMgrInst->setUserName("CQChartsAngle", "angle");
}

CQChartsAngle::
CQChartsAngle(const QString &s)
{
  fromString(s);
}

QString
CQChartsAngle::
toString() const
{
  return QString::number(a_);
}

bool
CQChartsAngle::
fromString(const QString &str)
{
  if (str.trimmed() == "") {
    a_ = 0.0;
    return true;
  }

  bool ok;
  double a = CQChartsUtil::toReal(str, ok);
  if (! ok) return false;

  a_ = a;

  return true;
}
