#ifndef CQChartsOptLength_H
#define CQChartsOptLength_H

#include <CQChartsLength.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional length
 * \ingroup Charts
 */
class CQChartsOptLength {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using OptLength = boost::optional<CQChartsLength>;

  CQChartsOptLength() { }

  CQChartsOptLength(const OptLength &value) :
   value_(value) {
  }

  explicit CQChartsOptLength(const CQChartsLength &l) :
   value_(l) {
  }

  explicit CQChartsOptLength(const QString &s) {
    (void) fromString(s);
  }

  const OptLength &value() const { return value_; }
  void setValue(const OptLength &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  const CQChartsLength &length() const { assert(isSet()); return value_.value(); }
  void setLength(const CQChartsLength &l) { value_ = l; }

  CQChartsLength lengthOr(const CQChartsLength &def) const { return value_.value_or(def); }

  //---

  QString toString() const {
    if (! value_) return "";

    return value_->toString();
  }

  bool fromString(const QString &s) {
    if (s.simplified().length() == 0)
      value_ = OptLength();
    else {
      CQChartsLength value;

      if (! value.fromString(s))
        return false;

      value_ = value;
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptLength &lhs, const CQChartsOptLength &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsOptLength &lhs, const CQChartsOptLength &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    if (! value_)
      os << "<unset>";
    else
      os << *value_;
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsOptLength &l) {
    l.print(os);

    return os;
  }

  //---

 private:
  OptLength value_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptLength)

#endif
