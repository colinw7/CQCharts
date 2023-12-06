#include <CQChartsAlpha.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CMathUtil.h>

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

CQChartsAlpha::
CQChartsAlpha(double a) :
 a_(a), set_(true)
{
  assert(isValid());
}

double
CQChartsAlpha::
value(bool clamp) const
{
  assert(isSet());

  if (! clamp)
    return a_;

  return CMathUtil::clamp(a_, 0.0, 1.0);
}

void
CQChartsAlpha::
setValue(double a)
{
  a_   = a;
  set_ = true;

  assert(isValid());
}

double
CQChartsAlpha::
valueOr(double defValue, bool clamp) const
{
  if (isSet())
    return value(clamp);

  return defValue;
}

QString
CQChartsAlpha::
toString() const
{
  if (! isSet()) return "";

  return QString::number(a_);
}

bool
CQChartsAlpha::
fromString(const QString &str)
{
  if (str.trimmed() == "") {
    set_ = false;
    return true;
  }

  bool ok;
  double a = CQChartsUtil::toReal(str, ok);
  if (! ok) return false;

  a_   = a;
  set_ = true;

  assert(isValid());

  return true;
}
