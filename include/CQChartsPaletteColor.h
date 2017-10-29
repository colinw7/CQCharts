#ifndef CQChartsCQChartsPaletteColor_H
#define CQChartsCQChartsPaletteColor_H

#include <QColor>

struct CQChartsPaletteColor {
  bool   palette { true };
  QColor color   { 0, 0, 0 };

  CQChartsPaletteColor() :
   palette(true) {
  }

  CQChartsPaletteColor(const QColor &color, bool palette) :
   palette(palette), color(color) {
  }

  QString colorStr() const {
    if (palette)
      return "palette";

    return color.name();
  }

  void setColorStr(const QString &str) {
    if (str == "palette") {
      palette = true;
    }
    else {
      palette = false;
      color   = QColor(str);
    }
  }
};

#endif
