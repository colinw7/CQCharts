#include <CQChartsAngle.h>

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
  return QString("%1").arg(a_);
}

bool
CQChartsAngle::
fromString(const QString &str)
{
  bool ok;

  double a = str.toDouble(&ok);

  if (! ok)
    return false;

  a_ = a;

  return true;
}
