#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsWindow;
class CQPropertyViewTree;
class CGradientPalette;
class CQGradientControlPlot;
class CQGradientControlIFace;
class CQIconCombo;
class QTabWidget;
class QLineEdit;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

  CQPropertyViewTree *propertyTree() const { return propertyTree_; }

  CQGradientControlPlot *palettePlot() const { return palettePlot_; }

  CQGradientControlIFace *paletteControl() const { return paletteControl_; }

 private slots:
  void filterSlot();

 private:
  CQChartsWindow*         window_         { nullptr };
  QTabWidget*             tab_            { nullptr };
  QLineEdit*              filterEdit_     { nullptr };
  CQIconCombo*            filterCombo_    { nullptr };
  CQPropertyViewTree*     propertyTree_   { nullptr };
  CQGradientControlPlot*  palettePlot_    { nullptr };
  CQGradientControlIFace* paletteControl_ { nullptr };
};

#endif
