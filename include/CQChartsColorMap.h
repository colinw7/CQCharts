#ifndef CQChartsColorMap_H
#define CQChartsColorMap_H

#include <CQChartsColor.h>
#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

/*!
 * \brief class to contain mapping from unique value to color
 * \ingroup Charts
 */
class CQChartsColorMap :
  public CQChartsComparatorBase<CQChartsColorMap> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsColorMap, metaTypeId)

 public:
  using Color = CQChartsColor;

  using ColorValue = std::map<Color, QVariant>;
  using ValueColor = std::map<QVariant, Color>;

 public:
  //! default constructor
  CQChartsColorMap() = default;

  //! construct from string
  explicit CQChartsColorMap(const QString &str) {
    fromString(str);
  }

  //---

  //! get/set values
  const ValueColor &valueColor() const { return valueColor_; }

  //---

  bool isValid() const { return ! valueColor_.empty(); }

  //---

  //! to/from string for QVariant
  QString toString() const;
  bool fromString(const QString &s);

  //---

  bool valueToColor(const QVariant &value, Color &color) const;
  bool colorToValue(const Color &color, QVariant &value) const;

  //---

  int cmp(const CQChartsColorMap &s) const;

  friend int cmp(const CQChartsColorMap &s1, const CQChartsColorMap &s2) {
    return s1.cmp(s2);
  }

  //---

 private:
  void updateColorValue() const;

 private:
  ValueColor         valueColor_; //!< map of value to color
  mutable ColorValue colorValue_; //!< map of color to value
};

//---

CQUTIL_DCL_META_TYPE(CQChartsColorMap)

#endif
