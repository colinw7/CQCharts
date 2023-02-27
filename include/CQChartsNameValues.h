#ifndef CQChartsNameValues_H
#define CQChartsNameValues_H

#include <CQModelNameValues.h>
#include <CQUtilMeta.h>

class CQChartsColor;
class CQChartsFont;

/*!
 * \brief Name Values class
 * \ingroup Charts
 *
 * Comma separater list of name/value pairs (<name>=<value>)
 */
class CQChartsNameValues : public CQModelNameValues {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsNameValues, metaTypeId)

 public:
  CQChartsNameValues();

  explicit CQChartsNameValues(const QString &str);

  bool nameValueInteger(const QString &name, long &value, bool &ok) const override;
  bool nameValueReal   (const QString &name, double &value, bool &ok) const override;
  bool nameValueBool   (const QString &name, bool &value, bool &ok) const override;

  bool nameValueColor(const QString &name, QColor &color, bool &ok) const override {
    return CQModelNameValues::nameValueColor(name, color, ok); }
  bool nameValueColor(const QString &name, CQChartsColor &color, bool &ok) const;

  bool nameValueFont(const QString &name, QFont &font, bool &ok) const override {
    return CQModelNameValues::nameValueFont(name, font, ok); }
  bool nameValueFont(const QString &name, CQChartsFont &font, bool &ok) const;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsNameValues)

#endif
