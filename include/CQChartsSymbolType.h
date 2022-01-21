#ifndef CQChartsSymbolType_H
#define CQChartsSymbolType_H

#include <CQChartsTmpl.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>
#include <CMathUtil.h>
#include <QString>
#include <QStringList>
#include <cassert>

/*!
 * \brief Symbol Data
 * \ingroup Charts
 */
class CQChartsSymbolType :
  public CQChartsComparatorBase<CQChartsSymbolType>,
  public CQChartsToStringBase<CQChartsSymbolType> {
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
    HEXAGON,
    OCTAGON,
    HLINE,
    VLINE,
    PAW,
    Z,
    HASH,
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  static QString typeToName(Type type);
  static Type nameToType(const QString &str);

  static QStringList typeNames();

  //---

  static int minOutlineValue() { return (int) Type::CROSS; }
  static int maxOutlineValue() { return (int) Type::OCTAGON; }

  static int minFillValue() { return (int) Type::TRIANGLE; }
  static int maxFillValue() { return (int) Type::OCTAGON; }

  //---

  static bool isValidType(Type type) { return (type > Type::NONE && type <= Type::HASH); }

 public:
  CQChartsSymbolType() = default;

  explicit CQChartsSymbolType(Type type);

  explicit CQChartsSymbolType(const QString &s);

  //---

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; assert(isValidType(type_)); }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsSymbolType &lhs, const CQChartsSymbolType &rhs) {
    if (lhs.type_ > rhs.type_) return  1;
    if (lhs.type_ < rhs.type_) return -1;
    return 0;
  }

 private:
  Type type_ { Type::NONE };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsSymbolType)

#endif
