#ifndef CQChartsColor_H
#define CQChartsColor_H

#include <QColor>
#include <iostream>
#include <cassert>

class CQChartsPlot;
class CQChartsView;

class CQChartsColor {
 public:
  enum class Type {
    NONE,
    PALETTE,
    PALETTE_VALUE,
    THEME_VALUE,
    COLOR
  };

 public:
  static void registerMetaType();

 public:
  CQChartsColor() = default;

  CQChartsColor(Type type, double value=0.0) :
   type_(type), value_(value) {
  }

  CQChartsColor(const QColor &color) :
   type_(Type::COLOR), color_(color) {
  }

  explicit CQChartsColor(const QString &str) {
    setColorStr(str);
  }

  bool isValid() const { return type_ != Type::NONE; }

  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  int ind() const { return ind_; }
  void setInd(int ind) { assert(ind_ >= 0); ind_ = ind; }

  const QColor &color() const { return color_; }

  double value() const { return value_; }

  void setColor(const QColor &color) {
    type_  = Type::COLOR;
    color_ = color;
  }

  void setValue(Type type, double value) {
    type_  = type;
    value_ = value;
  }

  void setIndValue(Type type, int ind, double value) {
    type_  = type;
    ind_   = ind;
    value_ = value;
  }

  void setScaleValue(Type type, double value, bool scale) {
    type_  = type;
    value_ = value;
    scale_ = scale;
  }

  void setIndScaleValue(Type type, int ind, double value, bool scale) {
    type_  = type;
    ind_   = ind;
    value_ = value;
    scale_ = scale;
  }

  bool isScale() const { return scale_; }
  void setScale(bool scale) { scale_ = scale; }

  //---

  QString colorStr() const;
  void setColorStr(const QString &str);

  //---

  QColor interpColor(const CQChartsPlot *plot, int i, int n) const;
  QColor interpColor(const CQChartsPlot *plot, double value) const;

  QColor interpColor(const CQChartsView *view, int i, int n) const;
  QColor interpColor(const CQChartsView *view, double value) const;

  //---

  QString toString() const { return colorStr(); }

  void fromString(const QString &s) { setColorStr(s); }

  //---

  friend bool operator==(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    if (lhs.type_  != rhs.type_ ) return false;
    if (lhs.ind_   != rhs.ind_  ) return false;
    if (lhs.scale_ != rhs.scale_) return false;

    if (lhs.type_ == Type::COLOR) {
      if (lhs.color_ != rhs.color_)
        return false;
    }
    else {
      if (lhs.value_ != rhs.value_)
        return false;
    }

    return true;
  }

  friend bool operator!=(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsColor &c) {
    c.print(os);

    return os;
  }

  //---

 private:
  Type   type_  { Type::NONE }; // color type (palette, theme or color)
  int    ind_   { 0 };          // palette index
  double value_ { 0.0 };        // specific palette or theme value
  QColor color_ { 0, 0, 0 };    // specific color
  bool   scale_ { false };      // color scaled to palette defined color values (psuedo index)
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColor)

#endif
