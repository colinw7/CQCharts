#ifndef CQChartsColorSet_H
#define CQChartsColorSet_H

#include <CQChartsValueSet.h>

class CQChartsPlot;
class CQChartsColor;

class CQChartsColorSet : public CQChartsValueSet {
 public:
  CQChartsColorSet(CQChartsPlot *plot);

  bool icolor(int i, CQChartsColor &color);
};

#endif
