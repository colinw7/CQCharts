#ifndef CQChartsSymbol_H
#define CQChartsSymbol_H

#include <CMathUtil.h>
#include <QString>

class CQChartsSymbol {
 public:
  enum class Type {
    NONE,
    DOT,
    CROSS,
    PLUS,
    Y,
    TRIANGLE,
    ITRIANGLE,
    BOX,
    DIAMOND,
    STAR5,
    STAR6,
    CIRCLE,
    PENTAGON,
    IPENTAGON,
    HLINE,
    VLINE
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  static QString typeToName(Type type);
  static Type nameToType(const QString &str);

  static int minValue() { return (int) Type::DOT  ; }
  static int maxValue() { return (int) Type::VLINE; }

  static CQChartsSymbol interpOutline(double r) {
    return CQChartsSymbol((CQChartsSymbol::Type) int(CMathUtil::map(r, 0, 1, 2, 13)));
  }

  static CQChartsSymbol interpFilled(double r) {
    return CQChartsSymbol((CQChartsSymbol::Type) int(CMathUtil::map(r, 0, 1, 5, 13)));
  }

 public:
  CQChartsSymbol() = default;
  CQChartsSymbol(Type type) : type_(type) { }
  CQChartsSymbol(const QString &s);

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  //---

  QString toString() const;

  void fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsSymbol &lhs, const CQChartsSymbol &rhs) {
    return (lhs.type_ == rhs.type_);
  }

  friend bool operator!=(const CQChartsSymbol &lhs, const CQChartsSymbol &rhs) {
    return ! operator==(lhs, rhs);
  }

 private:
  Type type_ { Type::CIRCLE };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsSymbol)

#endif
