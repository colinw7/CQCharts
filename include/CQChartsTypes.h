#ifndef CQChartsTypes_H
#define CQChartsTypes_H

// selection modifier type
enum class CQChartsSelMod {
  REPLACE,
  ADD,
  REMOVE,
  TOGGLE
};

namespace CQChartsSymbolSize {
  inline double minValue() { return 5.0; }
  inline double maxValue() { return 64.0; }

  inline double maxPixelValue() { return 1000.0; }
}

namespace CQChartsFontSize {
  inline double minValue() { return 6.0; }
  inline double maxValue() { return 48.0; }

  inline double maxPixelValue() { return 1000.0; }
}

namespace CQChartsLineWidth {
  inline double maxPixelValue() { return 64.0; }
}

enum class CQChartsUnits {
  NONE,
  VIEW,
  PLOT,
  PIXEL,
  PERCENT
};

enum class CQChartsResizeSide {
  NONE,
  MOVE,
  LL,
  LR,
  UL,
  UR
};

#endif
