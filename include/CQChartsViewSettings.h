#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsView;
class QTabWidget;
class CQPropertyTree;
class CQGradientPalette;
class CQGradientPaletteControl;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsView *view);

  CQPropertyTree *propertyTree() const { return propertyTree_; }

  CQGradientPalette *palettePlot() const { return palettePlot_; }

  CQGradientPaletteControl *paletteControl() const { return paletteControl_; }

 private:
  CQChartsView             *view_           { nullptr };
  QTabWidget               *tab_            { nullptr };
  CQPropertyTree           *propertyTree_   { nullptr };
  CQGradientPalette        *palettePlot_    { nullptr };
  CQGradientPaletteControl *paletteControl_ { nullptr };
};

#endif
