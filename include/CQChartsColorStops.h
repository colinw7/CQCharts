#ifndef CQChartsColorStops_H
#define CQChartsColorStops_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>
#include <QStringList>
#include <vector>

/*!
 * \brief class to contain color stop values
 * \ingroup Charts
 */
class CQChartsColorStops :
  public CQChartsComparatorBase<CQChartsColorStops> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsColorStops, metaTypeId)

 public:
  enum class Units {
    ABSOLUTE,
    PERCENT
  };

  struct ValuePercent {
    double value   { 0.0 };
    double percent { -1.0 };

    ValuePercent() { }

    ValuePercent(double value1, double percent1) :
     value(value1), percent(percent1) {
    }

    int cmp(const ValuePercent &rhs) const {
      if (value   < rhs.value  ) return -1;
      if (value   > rhs.value  ) return  1;
      if (percent < rhs.percent) return -1;
      if (percent > rhs.percent) return  1;
      return 0;
    }
  };

  using Values = std::vector<ValuePercent>;

 public:
  //! default constructor
  CQChartsColorStops() = default;

  //! construct from values
  explicit CQChartsColorStops(const Values &values) :
   values_(values) {
  }

  //! color from string
  explicit CQChartsColorStops(const QString &str) {
    fromString(str);
  }

  //---

  //! get/set values
  const Values &values() const { return values_; }
  void setValues(const Values &values) { values_ = values; }

  //! get.set units
  const Units &units() const { return units_; }
  void setUnits(const Units &v) { units_ = v; }

  bool isPercent() const { return (units() == Units::PERCENT); }

  //---

  int size() const { return int(values_.size()); }

  //---

  bool isValid() const { return ! values_.empty(); }

  bool isDiscreet() const { return isDiscreet_; }

  //---

  //! get discreet value index
  int ind(double v) const;

  //! get mapped value
  double interp(double v) const;

  //---

  //! color to/from string for QVariant
  QString toString() const;
  bool fromString(const QString &s);

  //---

  int cmp(const CQChartsColorStops &s) const;

  friend int cmp(const CQChartsColorStops &s1, const CQChartsColorStops &s2) {
    return s1.cmp(s2);
  }

  //---

 private:
  Units  units_      { Units::ABSOLUTE }; //!< units
  Values values_;                         //!< stop values
  bool   isDiscreet_ { true };            //!< is discreet
};

//---

CQUTIL_DCL_META_TYPE(CQChartsColorStops)

#endif
