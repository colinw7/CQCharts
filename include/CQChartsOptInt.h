#ifndef CQChartsOptInt_H
#define CQChartsOptInt_H

#include <CQChartsUtil.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional integer
 * \ingroup Charts
 */
class CQChartsOptInt {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using OptInt = boost::optional<int>;

  CQChartsOptInt() { }

  CQChartsOptInt(const OptInt &value) :
   value_(value) {
  }

  explicit CQChartsOptInt(int i) :
   value_(i) {
  }

  explicit CQChartsOptInt(const QString &s) {
    (void) fromString(s);
  }

  const OptInt &value() const { return value_; }
  void setValue(const OptInt &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  int integer() const { return value_.value(); }
  void setInteger(int i) { value_ = i; }

  int integerOr(int def) const { return value_.value_or(def); }

  //---

  QString toString() const {
    if (! value_) return "";

    return QString("%1").arg(*value_);
  }

  bool fromString(const QString &s) {
    if (s.simplified().length() == 0)
      value_ = OptInt();
    else {
      bool ok;

      int r = CQChartsUtil::toInt(s, ok);

      if (! ok)
        return false;

      value_ = r;
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptInt &lhs, const CQChartsOptInt &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsOptInt &lhs, const CQChartsOptInt &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

 private:
  OptInt value_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptInt)

#endif
