#ifndef CQChartsOptPosition_H
#define CQChartsOptPosition_H

#include <CQChartsPosition.h>
#include <QString>

#include <boost/optional.hpp>

/*!
 * \brief Optional position
 * \ingroup Charts
 */
class CQChartsOptPosition :
  public CQChartsEqBase<CQChartsOptPosition> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using OptPosition = boost::optional<CQChartsPosition>;

  CQChartsOptPosition() { }

  CQChartsOptPosition(const OptPosition &value) :
   value_(value) {
  }

  explicit CQChartsOptPosition(const CQChartsPosition &l) :
   value_(l) {
  }

  explicit CQChartsOptPosition(const QString &s) {
    (void) setValue(s);
  }

  const OptPosition &value() const { return value_; }
  void setValue(const OptPosition &v) { value_ = v; }

  bool isSet() const { return bool(value_); }

  //---

  const CQChartsPosition &position() const { return value_.value(); }
  void setPosition(const CQChartsPosition &l) { value_ = l; }

  CQChartsPosition positionOr(const CQChartsPosition &def) const { return value_.value_or(def); }

  //---

  QString toString() const {
    if (! value_) return "";

    return value_->toString();
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  bool setValue(const QString &s) {
    if (s.trimmed().length() == 0)
      value_ = OptPosition();
    else {
      CQChartsPosition value;

      if (! value.fromString(s))
        return false;

      value_ = value;
    }

    return true;
  }

  //---

  friend bool operator==(const CQChartsOptPosition &lhs, const CQChartsOptPosition &rhs) {
    if (lhs.value_ != rhs.value_) return false;

    return true;
  }

  //---

 private:
  OptPosition value_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsOptPosition)

#endif
