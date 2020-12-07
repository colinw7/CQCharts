#ifndef CQChartsFillPattern_H
#define CQChartsFillPattern_H

#include <CQChartsEnum.h>
#include <CQChartsPaletteName.h>
#include <CQChartsImage.h>
#include <CQChartsColor.h>
#include <CQChartsAlpha.h>
#include <CQChartsAngle.h>

/*!
 * \brief fill pattern
 * \ingroup Charts
 *
 * Fill Pattern is applied to fill operation (color/alpha).
 *
 * Supports:
 *  Solid Color
 *   uses fill color/alpha
 *  Pattern (Hatch, Dense, Hozizontal, Vertical, Diagonal (Forward/Back)
 *   uses fill color/alpha, scale
 *  Gradient (Linear, Radial, Palette)
 *   uses fill color/alpha, optional pattern alt color and pattern angle
 *  Image, Texture, MASK
 *   uses image (texture is gray scale colored by fill color, mask is b/w with dark fg)
 */
class CQChartsFillPattern {
 public:
  enum class Type {
    NONE,
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG,
    LGRADIENT,
    RGRADIENT,
    PALETTE,
    IMAGE,
    TEXTURE,
    MASK
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using PaletteName = CQChartsPaletteName;
  using Image       = CQChartsImage;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Angle       = CQChartsAngle;

 public:
  CQChartsFillPattern(Type type=Type::SOLID) :
   type_(type) {
  }

  explicit CQChartsFillPattern(const QString &str) {
    (void) setValue(str);
  }

  //---

  //! get/set type
  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  //---

  bool isValid() const { return type() != Type::NONE; }

  //---

  //! get/set scale factor (for pattern)
  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  //---

  //! get/set palette name
  const PaletteName &palette() const { return palette_; }
  void setPalette(const PaletteName &p) { palette_ = p; }

  //---

  //! get/set image
  const Image &image() const { return image_; }
  void setImage(const Image &i);

  //---

  //! get/set palette angle
  const Angle &angle() const { return angle_; }
  void setAngle(const Angle &a) { angle_ = a; }

  //---

  //! get/set alt color (for pattern)
  const Color &altColor() const { return altColor_; }
  void setAltColor(const Color &c) { altColor_ = c; }

  //! get/set alt alpha (for pattern)
  const Alpha &altAlpha() const { return altAlpha_; }
  void setAltAlpha(const Alpha &a) { altAlpha_ = a; }

  //---

  QString toString() const;
  bool fromString(const QString &s);

  //---

  QStringList enumNames() const;

  //---

  static Type stringToType(const QString &str);
  static QString typeToString(const Type &type);

  static Type styleToType(const Qt::BrushStyle &style);
  static Qt::BrushStyle typeToStyle(const Type &type);

  //---

  Qt::BrushStyle style() const { return typeToStyle(type_); }

  //---

  void reset() {
    type_     = Type::NONE;
    scale_    = 1.0;
    palette_  = PaletteName();
    image_    = Image();
    angle_    = Angle();
    altColor_ = Color();
    altAlpha_ = Alpha();
  }

  //---

  friend bool operator==(const CQChartsFillPattern &lhs, const CQChartsFillPattern &rhs) {
    if (lhs.type() != rhs.type()) return false;

    // TODO: only compare values for type
    return (lhs.scale   () == rhs.scale   () &&
            lhs.palette () == rhs.palette () &&
            lhs.image   () == rhs.image   () &&
            lhs.angle   () == rhs.angle   () &&
            lhs.altColor() == rhs.altColor() &&
            lhs.altAlpha() == rhs.altAlpha());
  }

  friend bool operator!=(const CQChartsFillPattern &lhs, const CQChartsFillPattern &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const CQChartsFillPattern &t) {
    t.print(os);
    return os;
  }

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

 private:
  bool setValue(const QString &str);

 private:
  Type        type_     { Type::NONE }; //! pattern type
  double      scale_    { 1.0 };        //! scale for pattern (ldiag, ...)
  PaletteName palette_;                 //! palette name for gradients
  Image       image_;                   //! image
  Angle       angle_;                   //! angle for gradient
  Color       altColor_;                //! alt color for gradient/image background
  Alpha       altAlpha_;                //! alt alpha for gradient/image background
};

//---

CQUTIL_DCL_META_TYPE(CQChartsFillPattern)

#endif
