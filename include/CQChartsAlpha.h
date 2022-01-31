#ifndef CQChartsAlpha_H
#define CQChartsAlpha_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QString>
#include <QVariant>

/*!
 * \brief class to contain charts alpha
 * \ingroup Charts
 *
 * TODO: support percent value e.g. 50%
 */
class CQChartsAlpha :
  public CQChartsEqBase<CQChartsAlpha>,
  public CQChartsToStringBase<CQChartsAlpha> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsAlpha, metaTypeId)

 public:
  //! default constructor
  CQChartsAlpha() = default;

  //! create from string
  explicit CQChartsAlpha(const QString &s);

  //! create from value
  explicit CQChartsAlpha(double a);

  //---

  //! get/set value (optionally clamped to valid range)
  double value(bool clamp=true) const;
  void setValue(double a);

  //! get value if set, or return default value if not set
  double valueOr(double defValue, bool clamp=true) const;

  //---

  // get is set
  bool isSet() const { return set_; }

  // get is valid value
  bool isValid() const { return a_ >= 0.0 && a_ <= 1.0; }

  //---

  //! operator ==
  friend bool operator==(const CQChartsAlpha &lhs, const CQChartsAlpha &rhs) {
    return std::abs(lhs.a_ - rhs.a_) < 1E-6;
  }

  //---

  //! convert to/from string
  QString toString() const;
  bool fromString(const QString &s);

 private:
  double a_   { 1.0 };   //!< alpha
  bool   set_ { false }; //!< is set
};

//---

CQUTIL_DCL_META_TYPE(CQChartsAlpha)

#endif
