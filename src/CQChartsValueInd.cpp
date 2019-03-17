#include <CQChartsValueInd.h>
#include <CQChartsVariant.h>

int
CQChartsValueInd::
calcId(const QVariant &val, CQBaseModelType type)
{
  bool ok;

  if      (type == CQBaseModelType::INTEGER) {
    int i = CQChartsVariant::toInt(val, ok);
    if (! ok) return -1;

    return calcId(i);
  }
  else if (type == CQBaseModelType::REAL) {
    double r = CQChartsVariant::toReal(val, ok);
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
