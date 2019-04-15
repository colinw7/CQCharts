#ifndef CQChartsPaletteColorData_H
#define CQChartsPaletteColorData_H

#include <boost/optional.hpp>

//! \brief Charts Defined Color
struct CQChartsDefinedColor {
  double v { -1.0 };
  QColor c;

  CQChartsDefinedColor(double v, const QColor &c) :
   v(v), c(c) {
  }
};

//! \brief Charts Palette Color Data
struct CQChartsPaletteColorData {
  using OptReal       = boost::optional<double>;
  using OptInt        = boost::optional<int>;
  using OptBool       = boost::optional<bool>;
  using DefinedColors = std::vector<CQChartsDefinedColor>;

  QString       colorTypeStr;
  QString       colorModelStr;
  OptInt        redModel;
  OptInt        greenModel;
  OptInt        blueModel;
  OptBool       negateRed;
  OptBool       negateGreen;
  OptBool       negateBlue;
  OptReal       redMin;
  OptReal       redMax;
  OptReal       greenMin;
  OptReal       greenMax;
  OptReal       blueMin;
  OptReal       blueMax;
  DefinedColors definedColors;
  bool          getColorScale { false };
  bool          getColorFlag { false };
  double        getColorValue { 0.0 };

  CQChartsPaletteColorData() {
    redModel    = boost::make_optional(false, 0);
    greenModel  = boost::make_optional(false, 0);
    blueModel   = boost::make_optional(false, 0);
    negateRed   = boost::make_optional(false, false);
    negateGreen = boost::make_optional(false, false);
    negateBlue  = boost::make_optional(false, false);
    redMin      = boost::make_optional(false, 0.0);
    redMax      = boost::make_optional(false, 0.0);
    greenMin    = boost::make_optional(false, 0.0);
    greenMax    = boost::make_optional(false, 0.0);
    blueMin     = boost::make_optional(false, 0.0);
    blueMax     = boost::make_optional(false, 0.0);
  }
};

#endif
