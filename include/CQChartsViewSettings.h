#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsView;
class CQPropertyViewTree;
class CQGradientControlPlot;
class CQGradientControlIFace;
class CQIconCombo;
class QTabWidget;
class QLineEdit;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsView *view);
 ~CQChartsViewSettings();

  CQPropertyViewTree *propertyView() const { return propertyView_; }

  CQGradientControlPlot *palettePlot() const { return palettePlot_; }

  CQGradientControlIFace *paletteControl() const { return paletteControl_; }

 private slots:
  void filterSlot();

 private:
  CQChartsView*           view_           { nullptr };
  QTabWidget*             tab_            { nullptr };
  QLineEdit*              filterEdit_     { nullptr };
  CQIconCombo*            filterCombo_    { nullptr };
  CQPropertyViewTree*     propertyView_   { nullptr };
  CQGradientControlPlot*  palettePlot_    { nullptr };
  CQGradientControlIFace* paletteControl_ { nullptr };
};

#endif
