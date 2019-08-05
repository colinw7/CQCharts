#ifndef CQChartsOptRect_H
#define CQChartsOptRect_H

#include <CQChartsRect.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional rectangle
 * \ingroup Charts
 */
class CQChartsOptRect {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using OptRect = boost::optional<CQChartsRect>;

  CQChartsOptRect() { }

  CQChartsOptRect(const OptRect &value) :
   value_(value) {
  }

  explicit CQChartsOptRect(const CQChartsRect &l) :
   value_(l) {
  }

  explicit CQChartsOptRect(const QString &s) {
    (void) fromString(s);
  }

  const OptRect &value() const { return value_; }
  void setValue(const OptRect &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  const CQChartsRect &rect() const { return value_.value(); }
  void setRect(const CQChartsRect &l) { value_ = l; }

  CQChartsRect rectOr(const CQChartsRect &def) const { return value_.value_or(def); }

  //---

  QString toString() const {
    if (! value_) return "";

    return value_->toString();
  }

  bool fromString(const QString &s) {
    if (s.simplified().length() == 0)
      value_ = OptRect();
    else {
      CQChartsRect value;

      if (! value.fromString(s))
        return false;

      value_ = value;
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptRect &lhs, const CQChartsOptRect &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  friend bool operator!=(const CQChartsOptRect &lhs, const CQChartsOptRect &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

 private:
  OptRect value_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptRect)

#endif
