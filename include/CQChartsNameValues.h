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
  CQChartsNameValues(const QString &str);

  bool nameValueInteger(const QString &name, int &value) const override;
  bool nameValueReal   (const QString &name, double &value) const override;
  bool nameValueBool   (const QString &name, bool &value) const override;

  bool nameValueColor(const QString &name, QColor &color) const override {
    return CQModelNameValues::nameValueColor(name, color); }
  bool nameValueColor(const QString &name, CQChartsColor &color) const;

  bool nameValueFont(const QString &name, QFont &font) const override {
    return CQModelNameValues::nameValueFont(name, font); }
  bool nameValueFont(const QString &name, CQChartsFont &font) const;
};

#endif
