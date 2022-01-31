#ifndef CQChartsUnits_H
#define CQChartsUnits_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QString>
#include <QVariant>

#include <iostream>

/*!
 * \brief Data units
 * \ingroup Charts
 */
class CQChartsUnits :
  public CQChartsEqBase<CQChartsUnits>,
  public CQChartsToStringBase<CQChartsUnits> {
 public:
  enum class Type {
    NONE,
    VIEW,
    PLOT,
    PIXEL,
    PERCENT,
    EM,
    EX
  };

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsUnits, metaTypeId)

 public:
  CQChartsUnits() = default;

  explicit CQChartsUnits(Type type) :
   type_(type) {
  }

  explicit CQChartsUnits(const QString &s) {
    setValue(s);
  }

  const Type &type() const { return type_; }

  bool setValue(const QString &str) {
    Type type;

    if (! decodeUnits(str, type, Type::NONE))
      return false;

    type_ = type;

    return true;
  }

  //---

  bool isSet() const { return type_ != Type::NONE; }

  //---

  QString toString() const { return unitsString(type_); }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  friend bool operator==(const CQChartsUnits &lhs, const CQChartsUnits &rhs) {
    if (lhs.type_ != rhs.type_) return false;

    return true;
  }

  //---

 public:
  static QString unitsString(const Type &type);

  static bool decodeUnits(const QString &str, Type &type, const Type &defType=Type::PLOT);

  static QStringList unitNames(bool includeNone=false);

  static QStringList unitTipNames(bool includeNone=false);

 private:
  Type type_ { Type::NONE };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsUnits)

#endif
