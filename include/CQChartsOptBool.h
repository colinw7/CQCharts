#ifndef CQChartsOptBool_H
#define CQChartsOptBool_H

#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional boolean
 * \ingroup Charts
 */
class CQChartsOptBool :
  public CQChartsEqBase<CQChartsOptBool> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsOptBool, metaTypeId)

 public:
  using OptBool = boost::optional<bool>;

  CQChartsOptBool() { }

  CQChartsOptBool(const OptBool &value) :
   value_(value) {
  }

  explicit CQChartsOptBool(bool b) :
   value_(b) {
  }

  explicit CQChartsOptBool(const QString &s) {
    (void) setValue(s);
  }

  const OptBool &value() const { return value_; }
  void setValue(const OptBool &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  bool isBool() const { return value_.value(); }
  void setBool(bool b) { value_ = b; }

  bool boolOr(bool def) const { return value_.value_or(def); }

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
      value_ = OptBool();
    else {
      bool ok;
      bool b = CQChartsUtil::stringToBool(s, &ok);
      if (! ok) return false;

      value_ = static_cast<bool>(b);
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptBool &lhs, const CQChartsOptBool &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  //---

 private:
  OptBool value_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsOptBool)

#endif
