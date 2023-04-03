#ifndef CQChartsOptInt_H
#define CQChartsOptInt_H

#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

#include <QString>

#include <optional>

/*!
 * \brief Optional integer
 * \ingroup Charts
 */
class CQChartsOptInt :
  public CQChartsEqBase<CQChartsOptInt> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsOptInt, metaTypeId)

 public:
  using OptInt = std::optional<long>;

  CQChartsOptInt() = default;

  CQChartsOptInt(const OptInt &value) :
   value_(value) {
  }

  explicit CQChartsOptInt(long i) :
   value_(i) {
  }

  explicit CQChartsOptInt(const QString &s) {
    (void) setValue(s);
  }

  const OptInt &value() const { return value_; }
  void setValue(const OptInt &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  long integer() const { return value_.value(); }
  void setInteger(long i) { value_ = i; }

  long integerOr(long def) const { return value_.value_or(def); }

  //---

  QString toString() const {
    if (! value_) return "";

    return QString::number(*value_);
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  bool setValue(const QString &s) {
    if (s.trimmed().length() == 0)
      value_ = OptInt();
    else {
      bool ok;
      long i = CQChartsUtil::toInt(s, ok);
      if (! ok) return false;

      value_ = static_cast<long>(i);
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptInt &lhs, const CQChartsOptInt &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  //---

 private:
  OptInt value_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsOptInt)

#endif
