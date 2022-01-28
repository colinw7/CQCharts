#ifndef CQChartsOptInt_H
#define CQChartsOptInt_H

#include <CQChartsUtil.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional integer
 * \ingroup Charts
 */
class CQChartsOptInt :
  public CQChartsEqBase<CQChartsOptInt> {
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
    (void) setValue(s);
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

      if (! ok)
        return false;

      value_ = static_cast<int>(i);
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

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptInt)

#endif
