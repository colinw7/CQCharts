#ifndef CQChartsColor_H
#define CQChartsColor_H

#include <cassert>
#include <QColor>
#include <iostream>

class QObject;

/*!
 * \brief class to contain charts color.
 * \ingroup Charts
 *
 * A charts color can be:
 *  . An interpolated palette color ("palette" "palette#<ind>")
 *  . An explicit palette color ("palette <value>" "palette#<ind> <value>")
 *  . An indexed palette color ("ind_palette" "ind_palette#<ind>")
 *  . An explicit indexed palette color ("ind_palette <value>" "ind_palette#<ind> <value>")
 *  . An interpolated interface color ("interface")
 *  . An explicit interface color ("interface <value>")
 *  . A model color ("model <r> <g> <b>")
 *  . An explicit model color ("model <r> <g> <b> <value>")
 *  . An explicit rgb color ("<name>")
 */
class CQChartsColor {
 public:
  enum class Type {
    NONE,
    PALETTE,
    PALETTE_VALUE,
    INDEXED,
    INDEXED_VALUE,
    INTERFACE,
    INTERFACE_VALUE,
    MODEL,
    MODEL_VALUE,
    COLOR
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  //! default constructor
  CQChartsColor() = default;

  //! color from type and value
  explicit CQChartsColor(Type type, double value=0.0) :
   type_(type), value_(value) {
  }

  //! color from QColor
  CQChartsColor(const QColor &color) :
   type_(Type::COLOR), color_(color) {
  }

  //! color from string
  explicit CQChartsColor(const QString &str) {
    setColorStr(str);
  }

  //! is color valid
  bool isValid() const {
    if      (type_ == Type::NONE)
      return false;
    else if (type_ == Type::COLOR)
      return color_.isValid();
    else
      return true;
  }

  //! get/set type
  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  //! get/set palette index (< 0 is unset)
  int ind() const { return ind_; }
  void setInd(int ind) { ind_ = (ind >= 0 ? ind : -1); }

  //! get/set color
  const QColor &color() const { return color_; }

  void setColor(const QColor &color) {
    type_  = Type::COLOR;
    color_ = color;
  }

  //! get/set palette value
  double value() const { return value_; }

  //! set type and value
  void setValue(Type type, double value) {
    type_  = type;
    value_ = value;
  }

  //! set type, index and value
  void setIndValue(Type type, int ind, double value) {
    type_  = type;
    ind_   = ind;
    value_ = value;
  }

  //! set type, value and scale
  void setScaleValue(Type type, double value, bool scale) {
    type_  = type;
    value_ = value;
    scale_ = scale;
  }

  //! set type, index, value and scale
  void setIndScaleValue(Type type, int ind, double value, bool scale) {
    type_  = type;
    ind_   = ind;
    value_ = value;
    scale_ = scale;
  }

  //! get/set scale
  bool isScale() const { return scale_; }
  void setScale(bool scale) { scale_ = scale; }

  //---

  //! can color be calculated directly (with no additional state)
  bool isDirect() const {
    return (type_ == Type::PALETTE_VALUE   ||
            type_ == Type::INDEXED_VALUE   ||
            type_ == Type::INTERFACE_VALUE ||
            type_ == Type::MODEL_VALUE ||
            type_ == Type::COLOR);
  }

  //---

  bool hasPaletteIndex() const {
    assert(type_ == Type::PALETTE || type_ == Type::PALETTE_VALUE ||
           type_ == Type::INDEXED || type_ == Type::INDEXED_VALUE);

    return (ind() >= 0);
  }

  bool hasPaletteName() const {
    assert(type_ == Type::PALETTE || type_ == Type::PALETTE_VALUE ||
           type_ == Type::INDEXED || type_ == Type::INDEXED_VALUE);

    return (ind() <= -2);
  }

  bool getPaletteName(QString &name) const;
  bool setPaletteName(const QString &name);

  static bool paletteNameInd(const QString &name, long &ind);

  //---

  void getModelRGB(int &r, int &g, int &b) const {
    assert(type_ == Type::MODEL || type_ == Type::MODEL_VALUE);

    decodeModelRGB(ind_, r, g, b);
  }

  void setModelRGB(int r, int g, int b) {
    assert(type_ == Type::MODEL || type_ == Type::MODEL_VALUE);

    ind_ = encodeModelRGB(r, g, b);
  }

  //---

  //! get/set color string
  QString colorStr() const;
  bool setColorStr(const QString &str);

  //---

  //! color to/from string for QVariant
  QString toString() const { return colorStr(); }

  bool fromString(const QString &s) { return setColorStr(s); }

  //---

  //! compare two QColors
  static int cmp(const QColor &lhs, const QColor &rhs) {
    if (lhs.red  () < rhs.red  ()) return -1;
    if (lhs.red  () > rhs.red  ()) return  1;
    if (lhs.green() < rhs.green()) return -1;
    if (lhs.green() > rhs.green()) return  1;
    if (lhs.blue () < rhs.blue ()) return -1;
    if (lhs.blue () > rhs.blue ()) return  1;

    return 0;
  }

  //! compare two colors
  static int cmp(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    if (lhs.type_  != rhs.type_ ) return (lhs.type_  < rhs.type_  ? -1 : 1);
    if (lhs.ind_   != rhs.ind_  ) return (lhs.ind_   < rhs.ind_   ? -1 : 1);
    if (lhs.scale_ != rhs.scale_) return (lhs.scale_ < rhs.scale_ ? -1 : 1);

    if (lhs.type_ == Type::COLOR) {
      return cmp(lhs.color_, rhs.color_);
    }
    else {
      if (lhs.value_ != rhs.value_) return (lhs.value_ < rhs.value_ ? -1 : 1);
    }

    return 0;
  }

  //! operator ==
  friend bool operator==(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    return (cmp(lhs, rhs) == 0);
  }

  //! operator !=
  friend bool operator!=(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    return (cmp(lhs, rhs) != 0);
  }

  //! operator <
  friend bool operator<(const CQChartsColor &lhs, const CQChartsColor &rhs) {
    return (cmp(lhs, rhs) < 0);
  }

  //---

  //! print color (debug)
  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  //! ostream operator <<
  friend std::ostream &operator<<(std::ostream &os, const CQChartsColor &c) {
    c.print(os);

    return os;
  }

  //---

 private:
  // model number is 0-37 and supports negative
  // To encode we map from -127 -> 127 to 0-255
  static int encodeModelRGB(int r, int g, int b) {
    return ( mapModelNum(r) & 0xff       ) |
           ((mapModelNum(g) & 0xff) <<  8) |
           ((mapModelNum(b) & 0xff) << 16);
  }

  static void decodeModelRGB(int rgb, int &r, int &g, int &b) {
    r = unmapModelNum( rgb        & 0xff);
    g = unmapModelNum((rgb >>  8) & 0xff);
    b = unmapModelNum((rgb >> 16) & 0xff);
  }

  static int mapModelNum(int n) {
    if (n < -127) n = -127;
    if (n >  127) n =  127;
    return (n + 127);
  }

  static int unmapModelNum(int n) {
    if (n < 0  ) n = 0;
    if (n > 254) n = 254;
    return n - 127;
  }

 private:
  Type   type_  { Type::NONE }; //!< color type (palette, theme or color)
  int    ind_   { -1 };         //!< -1 unset, >=0 palette index, <-2 color mgr index - 2
  double value_ { 0.0 };        //!< specific palette or theme value
  QColor color_ { 0, 0, 0 };    //!< specific color
  bool   scale_ { false };      //!< color scaled to palette defined color values (pseudo index)
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsColor)

#endif
