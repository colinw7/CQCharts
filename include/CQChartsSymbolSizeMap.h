#ifndef CQChartsSymbolSizeMap_H
#define CQChartsSymbolSizeMap_H

#include <CQChartsLength.h>
#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

/*!
 * \brief class to contain mapping from unique value to symbol size
 * \ingroup Charts
 */
class CQChartsSymbolSizeMap :
  public CQChartsComparatorBase<CQChartsSymbolSizeMap> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsSymbolSizeMap, metaTypeId)

 public:
  using Length = CQChartsLength;

  using LengthValue = std::map<Length, QVariant>;
  using ValueLength = std::map<QVariant, Length>;

 public:
  //! default constructor
  CQChartsSymbolSizeMap() = default;

  //! construct from string
  explicit CQChartsSymbolSizeMap(const QString &str) {
    fromString(str);
  }

  //---

  //! get/set values
  const ValueLength &valueLength() const { return valueLength_; }

  //---

  bool isValid() const { return ! valueLength_.empty(); }

  //---

  //! to/from string for QVariant
  QString toString() const;
  bool fromString(const QString &s);

  //---

  bool valueToLength(const QVariant &value, Length &length) const;
  bool lengthToValue(const Length &length, QVariant &value) const;

  //---

  int cmp(const CQChartsSymbolSizeMap &s) const;

  friend int cmp(const CQChartsSymbolSizeMap &s1, const CQChartsSymbolSizeMap &s2) {
    return s1.cmp(s2);
  }

  //---

 private:
  void updateLengthValue() const;

 private:
  ValueLength         valueLength_; //!< map of value to length
  mutable LengthValue lengthValue_; //!< map of length to value
};

//---

CQUTIL_DCL_META_TYPE(CQChartsSymbolSizeMap)

#endif
