#ifndef CQChartsNameValues_H
#define CQChartsNameValues_H

#include <CQModelNameValues.h>

class CQChartsColor;
class CQChartsFont;

/*!
 * \brief Name Values class
 * \ingroup Charts
 */
class CQChartsNameValues : public CQModelNameValues {
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

#endif
