#ifndef CQChartsOptString_H
#define CQChartsOptString_H

#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

#include <QString>

#include <optional>

/*!
 * \brief Optional string
 * \ingroup Charts
 */
class CQChartsOptString :
  public CQChartsEqBase<CQChartsOptString> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsOptString, metaTypeId)

 public:
  using OptString = std::optional<QString>;

  CQChartsOptString() = default;

  CQChartsOptString(const OptString &value) :
   value_(value) {
  }

  explicit CQChartsOptString(const QString &s) :
   value_(s) {
  }

  CQChartsOptString(const CQChartsOptString &rhs) :
   value_(rhs.value_), defValue_(rhs.defValue_) {
    // TODO: keep def value if rhs hasn't got one ?
  }

  CQChartsOptString &operator=(const CQChartsOptString &rhs) {
    // TODO: keep def value if rhs hasn't got one ?
    value_    = rhs.value_;
    defValue_ = rhs.defValue_;
    return *this;
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

  QString stringOr(const QString &def="") const { return value_.value_or(def); }

  //---

  QString toString() const { return string(); }

  bool fromString(const QString &s) { setString(s); return true; }

  //---

  friend bool operator==(const CQChartsOptString &lhs, const CQChartsOptString &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  //---

 private:
  OptString value_;
  QString   defValue_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsOptString)

#endif
