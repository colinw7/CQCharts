#include <CQChartsValueInd.h>
#include <CQChartsVariant.h>

int
CQChartsValueInd::
calcId(const QVariant &val, CQBaseModelType type)
{
  bool ok;

  if      (type == CQBaseModelType::INTEGER) {
    auto i = CQChartsVariant::toInt(val, ok);
    if (! ok) return -1;

    return calcId(int(i));
  }
  else if (type == CQBaseModelType::REAL) {
    auto r = CQChartsVariant::toReal(val, ok);
    if (! ok) return -1;

    if (CMathUtil::isNaN(r))
      return -1;

    return calcId(r);
  }
  else {
    QString s;

    ok = CQChartsVariant::toString(val, s);
    if (! ok) return -1;

    return calcId(s);
  }
}
