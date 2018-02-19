#ifndef CQChartsColor_H
#define CQChartsColor_H

#include <QColor>
#include <iostream>

class CQChartsPlot;

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

  //---

  QString toString() const { return colorStr(); }

  void fromString(const QString &s) { setColorStr(s); }

  //---

  friend bool operator==(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    if (lhs.type_  != rhs.type_ ) return false;
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
  Type   type_  { Type::NONE };
  double value_ { 0.0 };
  QColor color_ { 0, 0, 0 };
  bool   scale_ { false };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColor)

#endif
