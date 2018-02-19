#ifndef CQChartsViewSettings_H
#define CQChartsViewSettings_H

#include <QFrame>

class CQChartsWindow;
class CQChartsFilterEdit;
class CQPropertyViewTree;
class CQChartsGradientPaletteCanvas;
class CQChartsGradientPaletteControl;

class QTabWidget;
class QComboBox;
class QSpinBox;
class QLabel;

class CQChartsViewSettings : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewSettings(CQChartsWindow *window);
 ~CQChartsViewSettings();

  CQPropertyViewTree *propertyTree() const { return propertyTree_; }

  CQChartsGradientPaletteCanvas *palettePlot() const { return palettePlot_; }

  CQChartsGradientPaletteControl *paletteControl() const { return paletteControl_; }

 signals:
  void propertyItemSelected(QObject *obj, const QString &path);

 private slots:
  void gradientComboSlot(int ind);
  void paletteIndexSlot(int ind);

  void loadPaletteNameSlot();

  void replaceFilterSlot(const QString &text);
  void addFilterSlot(const QString &text);

  void replaceSearchSlot(const QString &text);
  void addSearchSlot(const QString &text);

 private:
  void updateGradientPalette();

 private:
  CQChartsWindow*                 window_         { nullptr };
  QTabWidget*                     tab_            { nullptr };
  CQChartsFilterEdit*             filterEdit_     { nullptr };
  CQPropertyViewTree*             propertyTree_   { nullptr };
  CQChartsGradientPaletteCanvas*  palettePlot_    { nullptr };
  CQChartsGradientPaletteControl* paletteControl_ { nullptr };
  QComboBox*                      gradientCombo_  { nullptr };
  QSpinBox*                       paletteSpin_    { nullptr };
  QComboBox*                      paletteCombo_   { nullptr };
};

#endif
