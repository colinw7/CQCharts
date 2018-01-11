#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsWindow;
class CQChartsFilterEdit;
class CQPropertyViewTree;
class CQGradientControlPlot;
class CQGradientControlIFace;
class QTabWidget;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

  CQPropertyViewTree *propertyTree() const { return propertyTree_; }

  CQGradientControlPlot *palettePlot() const { return palettePlot_; }

  CQGradientControlIFace *paletteControl() const { return paletteControl_; }

 private slots:
  void gradientComboSlot(int ind);

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

 private:
  CQChartsWindow*         window_         { nullptr };
  QTabWidget*             tab_            { nullptr };
  CQChartsFilterEdit*     filterEdit_     { nullptr };
  CQPropertyViewTree*     propertyTree_   { nullptr };
  CQGradientControlPlot*  palettePlot_    { nullptr };
  CQGradientControlIFace* paletteControl_ { nullptr };
};

#endif
