#ifndef CQChartsPaletteColor_H
#define CQChartsPaletteColor_H

#include <QColor>

class CQChartsPlot;

class CQChartsPaletteColor {
 public:
  enum class Type {
    PALETTE,
    PALETTE_VALUE,
    THEME_VALUE,
    COLOR
  };

 public:
  CQChartsPaletteColor() = default;

  CQChartsPaletteColor(Type type, double value=0.0) :
   type_(type), value_(value) {
  }

  CQChartsPaletteColor(const QColor &color) :
   type_(Type::COLOR), color_(color) {
  }

  void setColor(const QColor &color) {
    type_  = Type::COLOR;
    color_ = color;
  }

  void setValue(Type type, double value=0.0) {
    type_  = type;
    value_ = value;
  }

  QString colorStr() const;

  void setColorStr(const QString &str);

  QColor interpColor(const CQChartsPlot *plot, int i, int n) const;

  QColor interpColor(const CQChartsPlot *plot, double value) const;

 private:
  Type   type_  { Type::COLOR };
  double value_ { 0.0 };
  QColor color_ { 0, 0, 0 };
  bool   scale_ { false };
};

#endif
