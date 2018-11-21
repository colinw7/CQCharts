#ifndef CQChartsFillPattern_H
#define CQChartsFillPattern_H

#include <QString>

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
    BDIAG
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsFillPattern(Type type=Type::SOLID) :
   type_(type) {
  }

  explicit CQChartsFillPattern(const QString &str) {
    type_ = stringToType(str);
  }

  bool isValid() const { return type_ != Type::NONE; }

  Type type() const { return type_; }
  void setType(Type type) { type_  = type; }

  //---

  Qt::BrushStyle style() const { return typeToStyle(type_); }

  //---

  QString toString() const { return typeToString(type_); }

  void fromString(const QString &s) { type_ = stringToType(s); }

  //---

  static Type stringToType(const QString &str) {
    if (str == "SOLID"     ) return Type::SOLID;
    if (str == "HATCH"     ) return Type::HATCH;
    if (str == "DENSE"     ) return Type::DENSE;
    if (str == "HORIZ"     ) return Type::HORIZ;
    if (str == "HORIZONTAL") return Type::HORIZ;
    if (str == "VERT"      ) return Type::VERT;
    if (str == "VERTICAL"  ) return Type::VERT;
    if (str == "FDIAG"     ) return Type::FDIAG;
    if (str == "BDIAG"     ) return Type::BDIAG;

    return Type::NONE;
  }

  static QString typeToString(const Type &type) {
    switch (type) {
      case Type::SOLID: return "SOLID";
      case Type::HATCH: return "HATCH";
      case Type::DENSE: return "DENSE";
      case Type::HORIZ: return "HORIZONTAL";
      case Type::VERT : return "VERTICAL";
      case Type::FDIAG: return "FDIAG";
      case Type::BDIAG: return "BDIAG";
      default         : return "NONE";
    }
  }

  static Type styleToType(const Qt::BrushStyle &style) {
    switch (style) {
      case Qt::SolidPattern : return Type::SOLID;
      case Qt::CrossPattern : return Type::HATCH;
      case Qt::Dense5Pattern: return Type::DENSE;
      case Qt::HorPattern   : return Type::HORIZ;
      case Qt::VerPattern   : return Type::VERT;
      case Qt::FDiagPattern : return Type::FDIAG;
      case Qt::BDiagPattern : return Type::BDIAG;
      default               : return Type::NONE;
    }
  }

  static Qt::BrushStyle typeToStyle(const Type &type) {
    switch (type) {
      case Type::SOLID: return Qt::SolidPattern;
      case Type::HATCH: return Qt::CrossPattern;
      case Type::DENSE: return Qt::Dense5Pattern;
      case Type::HORIZ: return Qt::HorPattern;
      case Type::VERT : return Qt::VerPattern;
      case Type::FDIAG: return Qt::FDiagPattern;
      case Type::BDIAG: return Qt::BDiagPattern;
      default         : return Qt::SolidPattern;
    }
  }

  friend bool operator==(const CQChartsFillPattern &lhs, const CQChartsFillPattern &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsFillPattern &lhs, const CQChartsFillPattern &rhs) {
    return (lhs.type_ != rhs.type_);
  }

 private:
  Type type_ { Type::NONE };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsFillPattern)

#endif
