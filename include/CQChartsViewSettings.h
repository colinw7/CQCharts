#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsView;
class CQPropertyView;
class CQGradientPalette;
class CQGradientPaletteControl;
class QTabWidget;
class QLineEdit;
class QComboBox;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsView *view);

  CQPropertyView *propertyView() const { return propertyView_; }

  CQGradientPalette *palettePlot() const { return palettePlot_; }

  CQGradientPaletteControl *paletteControl() const { return paletteControl_; }

 private slots:
  void filterSlot();

 private:
  CQChartsView*             view_           { nullptr };
  QTabWidget*               tab_            { nullptr };
  QLineEdit*                filterEdit_     { nullptr };
  QComboBox*                filterCombo_    { nullptr };
  CQPropertyView*           propertyView_   { nullptr };
  CQGradientPalette*        palettePlot_    { nullptr };
  CQGradientPaletteControl* paletteControl_ { nullptr };
};

#endif
