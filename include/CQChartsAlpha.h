#ifndef CQChartsAlpha_H
#define CQChartsAlpha_H

#include <QString>

/*!
 * \brief class to contain charts alpha
 * \ingroup Charts
 */
class CQChartsAlpha {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  //! default constructor
  CQChartsAlpha() = default;

  explicit CQChartsAlpha(const QString &s);

  explicit CQChartsAlpha(double a) :
   a_(a) {
  }

  double value() const { return a_; }
  void setValue(double a) { a_ = a; }

  //---

  bool isValid() const { return a_ >= 0.0 && a_ <= 1.0; }

  //---

  //! operator ==
  friend bool operator==(const CQChartsAlpha &lhs, const CQChartsAlpha &rhs) {
    return (lhs.a_ == rhs.a_);
  }

  //! operator !=
  friend bool operator!=(const CQChartsAlpha &lhs, const CQChartsAlpha &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

 private:
  double a_ { 1.0 }; //!< alpha
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsAlpha)

#endif
