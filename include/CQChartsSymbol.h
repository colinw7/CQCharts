#ifndef CQChartsSymbol_H
#define CQChartsSymbol_H

#include <CMathUtil.h>
#include <QString>
#include <QStringList>
#include <cassert>

/*!
 * \brief Symbol Data
 * \ingroup Charts
 */
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

 public:
  static QString typeToName(Type type);
  static Type nameToType(const QString &str);

  static QStringList typeNames();

  static int minOutlineValue() { return (int) Type::CROSS; }
  static int maxOutlineValue() { return (int) Type::IPENTAGON; }

  static int minFillValue() { return (int) Type::TRIANGLE; }
  static int maxFillValue() { return (int) Type::IPENTAGON; }

  static CQChartsSymbol interpOutline(double r) {
    return CQChartsSymbol((CQChartsSymbol::Type)
      int(CMathUtil::map(r, 0, 1, minOutlineValue(), maxOutlineValue())));
  }

  static CQChartsSymbol interpFilled(double r) {
    return CQChartsSymbol((CQChartsSymbol::Type)
      int(CMathUtil::map(r, 0, 1, minFillValue(), maxFillValue())));
  }

  static CQChartsSymbol outlineFromInt(int i) {
    int len = maxOutlineValue() - minOutlineValue() + 1;

    int n = (i - minOutlineValue())/len;

    int i1 = (i - n*len);

    assert(i1 >= minOutlineValue() && i1 <= maxOutlineValue());

    return (CQChartsSymbol::Type) i1;
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

  bool fromString(const QString &s);

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
