#ifndef CQChartsColorSet_H
#define CQChartsColorSet_H

#include <CQChartsValueSet.h>
#include <CQChartsColor.h>

class CQChartsColorSet : public CQChartsValueSet {
 public:
  using OptColor = boost::optional<CQChartsColor>;

 public:
  CQChartsColorSet(CQChartsPlot *plot) :
   CQChartsValueSet(plot) {
  }

  bool icolor(int i, OptColor &color) {
    if (empty())
      return false;

    if (! hasInd(i))
      return false;

    // color can be actual color value (string) or value used to map into palette
    // (map enabled or disabled)
    if      (type() == Type::STRING) {
      QVariant colorVar = value(i);

      // TODO: force all color columns with names to use color type
      QColor c(colorVar.toString());

      if (c.isValid()) {
        color = c;

        return true;
      }

      // interped color must have at least 2 unique string values
      if (snum() <= 1)
        return false;
    }
    else if (type() == Type::COLOR) {
      color = value(i).value<QColor>();

      return true;
    }

    double value = imap(i);

    color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, value);

    return true;
  }
};

#endif
