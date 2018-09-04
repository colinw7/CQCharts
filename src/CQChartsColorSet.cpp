#include <CQChartsColorSet.h>
#include <CQChartsColor.h>
#include <CQChartsVariant.h>

CQChartsColorSet::
CQChartsColorSet(CQChartsPlot *plot) :
 CQChartsValueSet(plot)
{
  setMapped(true);
}

bool
CQChartsColorSet::
icolor(int i, CQChartsColor &color)
{
  if (empty())
    return false;

  if (! hasInd(i))
    return false;

  // if column is color already always used that (ignore mapping)
  if      (type() == Type::COLOR) {
    bool ok;

    CQChartsColor c = CQChartsVariant::toColor(value(i), ok);

    if (c.isValid()) {
      color = c;

      return true;
    }

    return false;
  }
  // color can be actual color value (string) or value used to map into palette
  // (map enabled or disabled)
  else if (type() == Type::STRING) {
    QVariant colorVar = value(i);

    // only use string as potential color name if mapped
    // TODO: force all color columns with names to use color type
    if (! isMapped()) {
      QColor c(colorVar.toString());

      if (c.isValid()) {
        color = c;

        return true;
      }
    }

    // interped color must have at least 2 unique string values
    if (snum() <= 1)
      return false;
  }

  //---

  double value = imap(i);

  CQChartsColor c = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, value);

  color = c;

  return true;
}
