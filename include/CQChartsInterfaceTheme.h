#ifndef CQChartsInterfaceTheme_H
#define CQChartsInterfaceTheme_H

#include <QColor>

class CQColorsPalette;

/*!
 * \brief interface theme
 * \ingroup Charts
 */
class CQChartsInterfaceTheme {
 public:
  CQChartsInterfaceTheme();

 ~CQChartsInterfaceTheme();

  CQColorsPalette *palette() const { return palette_; }

  bool isDark() const { return isDark_; }
  void setDark(bool b);

  QColor interpColor(double r, bool scale) const;

 private:
  CQColorsPalette* palette_      { nullptr };   //!< palette
  bool             isDark_       { false };     //!< is dark
  QColor           lightBgColor_ { "#ffffff" }; //!< light background color
  QColor           lightFgColor_ { "#000000" }; //!< light foreground color
  QColor           darkBgColor_  { "#222222" }; //!< dark background color
  QColor           darkFgColor_  { "#dddddd" }; //!< dark foreground color
};

#endif
