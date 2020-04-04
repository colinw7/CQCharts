#ifndef CQChartsOptString_H
#define CQChartsOptString_H

#include <CQChartsUtil.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional string
 * \ingroup Charts
 */
class CQChartsOptString {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using OptString = boost::optional<QString>;

  CQChartsOptString() { }

  CQChartsOptString(const OptString &value) :
   value_(value) {
  }

  explicit CQChartsOptString(const QString &s) :
   value_(s) {
  }

  //---

  const OptString &value() const { return value_; }
  void setValue(const OptString &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  const QString &defValue() const { return defValue_; }
  void setDefValue(const QString &s) { defValue_ = s; }

  //---

  QString string() const { return value_.value_or(defValue_); }
  void setString(const QString &s) { value_ = s; }

  QString stringOr(const QString &def) const { return value_.value_or(def); }

  //---

  QString toString() const { return string(); }

  bool fromString(const QString &s) { setString(s); return true; }

  //---

  friend bool operator==(const CQChartsOptString &lhs, const CQChartsOptString &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsOptString &lhs, const CQChartsOptString &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

 private:
  OptString value_;
  QString   defValue_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptString)

#endif
