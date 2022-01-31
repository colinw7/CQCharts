#ifndef CQChartsOptReal_H
#define CQChartsOptReal_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QVariant>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional real
 * \ingroup Charts
 */
class CQChartsOptReal :
  public CQChartsComparatorBase<CQChartsOptReal> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsOptReal, metaTypeId)

 public:
  using OptReal = boost::optional<double>;

  CQChartsOptReal() { }

  CQChartsOptReal(const OptReal &value) :
   value_(value) {
  }

  explicit CQChartsOptReal(double r) :
   value_(r) {
  }

  explicit CQChartsOptReal(const QString &s) {
    (void) setValue(s);
  }

  explicit CQChartsOptReal(const QVariant &value) {
    bool ok;
    double r = value.toDouble(&ok);
    if (ok) value_ = r;
  }

  const OptReal &value() const { return value_; }
  void setValue(const OptReal &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  double real() const { assert(isSet()); return value_.value(); }
  void setReal(double value) { value_ = value; }

  double realOr(double def) const { return value_.value_or(def); }

  //---

  QString toString() const {
    if (! value_) return "";

    return QString::number(*value_);
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  bool setValue(const QString &s);

  //---

  friend int cmp(const CQChartsOptReal &lhs, const CQChartsOptReal &rhs) {
    if (! lhs.isSet() && ! rhs.isSet()) return 0;
    if (! lhs.isSet()) return -1;
    if (! rhs.isSet()) return  1;

    if (lhs.value_ < rhs.value_) return -1;
    if (lhs.value_ > rhs.value_) return  1;

    return 0;
  }

  //---

 private:
  OptReal value_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsOptReal)

#endif
