#ifndef CQChartsAngle_H
#define CQChartsAngle_H

#include <QString>

/*!
 * \brief class to contain charts angle
 * \ingroup Charts
 */
class CQChartsAngle {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  //! default constructor
  CQChartsAngle() = default;

  explicit CQChartsAngle(const QString &str);

  explicit CQChartsAngle(double a) :
   a_(a) {
  }

  double value() const { return a_; }
  void setValue(double a) { a_ = a; }

  //---

  bool isValid() const { return true; }

  //---

  friend CQChartsAngle operator+(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return CQChartsAngle(lhs.a_ + rhs.a_);
  }

  CQChartsAngle &operator+=(const CQChartsAngle &rhs) { a_ += rhs.a_; return *this; }

  //---

  //! operator ==
  friend bool operator==(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return (lhs.a_ == rhs.a_);
  }

  //! operator !=
  friend bool operator!=(const CQChartsAngle &lhs, const CQChartsAngle &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

 private:
  double a_ { 0.0 }; //!< angle
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsAngle)

#endif
