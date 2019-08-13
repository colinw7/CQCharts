#ifndef CQChartsOptReal_H
#define CQChartsOptReal_H

#include <CQChartsUtil.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional real
 * \ingroup Charts
 */
class CQChartsOptReal {
 public:
  static void registerMetaType();

  static int metaTypeId;

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
    (void) fromString(s);
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

    return QString("%1").arg(*value_);
  }

  bool fromString(const QString &s) {
    if (s.simplified().length() == 0)
      value_ = OptReal();
    else {
      bool ok;

      double r = CQChartsUtil::toReal(s, ok);

      if (! ok)
        return false;

      value_ = r;
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptReal &lhs, const CQChartsOptReal &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsOptReal &lhs, const CQChartsOptReal &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

 private:
  OptReal value_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptReal)

#endif
