#ifndef CQChartsTypes_H
#define CQChartsTypes_H

// selection modifier type
enum class CQChartsSelMod {
  REPLACE,
  ADD,
  REMOVE,
  TOGGLE
};

// click modifier type
enum CQChartsClickMod {
  TOGGLE     = int(CQChartsSelMod::REPLACE),
  SELECT     = int(CQChartsSelMod::ADD),
  UNSELECT   = int(CQChartsSelMod::REMOVE),
  TOGGLE_ALL = int(CQChartsSelMod::TOGGLE)
};

namespace CQChartsSymbolSize {
  inline double minValue() { return 3.0; }
  inline double maxValue() { return 64.0; }
}

namespace CQChartsFontSize {
  inline double minValue() { return 6.0; }
  inline double maxValue() { return 48.0; }
}

//! resize side
enum class CQChartsResizeSide {
  NONE,
  MOVE,
  LL,
  LR,
  UL,
  UR,
  EXTRA
};

//! object draw type
enum class CQChartsObjDrawType {
  LINE,
  BOX,
  SYMBOL,
  TEXT,
  IMAGE
};

//! color value type
enum class CQChartsColorType {
  AUTO,
  SET,
  GROUP,
  INDEX,
  X_VALUE,
  Y_VALUE
};

enum class CQChartsFilterModelType {
  EXPRESSION,
  REGEXP,
  WILDCARD,
  SIMPLE,
  SELECTED
};

enum class CQChartsLabelPosition {
  TOP_INSIDE,
  TOP_OUTSIDE,
  CENTER,
  BOTTOM_INSIDE,
  BOTTOM_OUTSIDE,
  LEFT_INSIDE,
  LEFT_OUTSIDE,
  RIGHT_INSIDE,
  RIGHT_OUTSIDE
};

enum class CQChartsEdgeType {
  NONE,
  ARC,
  RECTILINEAR,
  ROUNDED_LINE,
  LINE
};

#endif
