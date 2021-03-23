#ifndef CQChartsSymbol_H
#define CQChartsSymbol_H

#include <CQChartsTmpl.h>
#include <CMathUtil.h>
#include <QString>
#include <QStringList>
#include <cassert>

/*!
 * \brief Symbol Data
 * \ingroup Charts
 */
class CQChartsSymbol :
  public CQChartsEqBase<CQChartsSymbol> {
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
    VLINE,
    PAW,
    Z,
    HASH,
    CHAR
  };

  struct CharData {
    CharData() = default;

    explicit CharData(const QString &c, const QString &name="") :
     c(c), name(name) {
    }

    QString c;    //!< unicode character
    QString name; //!< display name
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
  static int maxOutlineValue() { return (int) Type::IPENTAGON; }


#if 0
  static CQChartsSymbol interpOutline(double r) {
    return CQChartsSymbol((CQChartsSymbol::Type)
      int(CMathUtil::map(r, 0, 1, minOutlineValue(), maxOutlineValue())));
  }
#endif

  static CQChartsSymbol interpOutline(int i) {
    int len = maxOutlineValue() - minOutlineValue() + 1;

    int n = (i - minOutlineValue())/len;

    int i1 = (i - n*len);

    assert(i1 >= minOutlineValue() && i1 <= maxOutlineValue());

    return CQChartsSymbol((CQChartsSymbol::Type) i1);
  }

  static int minFillValue() { return (int) Type::TRIANGLE; }
  static int maxFillValue() { return (int) Type::IPENTAGON; }

#if 0
  static CQChartsSymbol interpFilled(double r) {
    return CQChartsSymbol((CQChartsSymbol::Type)
      int(CMathUtil::map(r, 0, 1, minFillValue(), maxFillValue())));
  }
#endif

 public:
  CQChartsSymbol() = default;

  explicit CQChartsSymbol(Type type) : type_(type) { }

  explicit CQChartsSymbol(const CharData &charData) : type_(Type::CHAR), charData_(charData) { }

  explicit CQChartsSymbol(const QString &s);

  //---

  bool isValid() const { return type_ != Type::NONE; }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  //---

  const QString &charStr () { assert(type_ == Type::CHAR); return charData_.c; }
  const QString &charName() { assert(type_ == Type::CHAR); return charData_.name; }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsSymbol &lhs, const CQChartsSymbol &rhs) {
    return (lhs.type_ == rhs.type_);
  }

 private:
  Type     type_    { Type::NONE };
  CharData charData_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsSymbol)

#endif
