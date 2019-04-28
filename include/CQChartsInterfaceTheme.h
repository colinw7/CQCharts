#ifndef CQChartsInterfaceTheme_H
#define CQChartsInterfaceTheme_H

#include <QColor>

class CQChartsGradientPalette;

//! \brief interface theme
class CQChartsInterfaceTheme {
 public:
  CQChartsInterfaceTheme();

 ~CQChartsInterfaceTheme();

  CQChartsGradientPalette *palette() const { return palette_; }

  bool isDark() const { return isDark_; }
  void setDark(bool b);

  QColor interpColor(double r, bool scale) const;

 private:
  CQChartsGradientPalette* palette_      { nullptr };   //!< palette
  bool                     isDark_       { false };     //!< is dark
  QColor                   lightBgColor_ { "#ffffff" }; //!< light bg color
  QColor                   lightFgColor_ { "#000000" }; //!< light fg color
  QColor                   darkBgColor_  { "#222222" }; //!< dark bg color
  QColor                   darkFgColor_  { "#dddddd" }; //!< dark fg color
};

#endif
