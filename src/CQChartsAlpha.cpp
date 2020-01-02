#include <CQChartsAlpha.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsAlpha, toString, fromString)

int CQChartsAlpha::metaTypeId;

void
CQChartsAlpha::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsAlpha);

  CQPropertyViewMgrInst->setUserName("CQChartsAlpha", "alpha");
}

CQChartsAlpha::
CQChartsAlpha(const QString &s)
{
  fromString(s);
}

QString
CQChartsAlpha::
toString() const
{
  return QString("%1").arg(a_);
}

bool
CQChartsAlpha::
fromString(const QString &str)
{
  bool ok;

  double a = str.toDouble(&ok);

  if (! ok)
    return false;

  a_ = a;

  return true;
}
