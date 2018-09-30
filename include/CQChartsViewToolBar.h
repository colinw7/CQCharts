#ifndef CQChartsViewToolBar_H
#define CQChartsViewToolBar_H

#include <QFrame>

class CQChartsWindow;
class CQChartsModelDlg;
class CQChartsPlotDlg;
class CQIconCombo;
class QToolButton;
class QRadioButton;
class QStackedWidget;

class CQChartsViewToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewToolBar(CQChartsWindow *window);

  void updateMode();

  QSize sizeHint() const override;

 private slots:
  void modeSlot(int ind);

  void selectButtonClicked(int);

  void manageModelsSlot();

  void addPlotSlot();

  void zoomFullSlot();

  void panResetSlot();

  void autoFitSlot();

  void leftSlot();
  void rightSlot();

 private:
  CQChartsWindow*   window_            { nullptr };
  CQIconCombo*      modeCombo_         { nullptr };
  QRadioButton*     selectPointButton_ { nullptr };
  QRadioButton*     selectRectButton_  { nullptr };
  QToolButton*      modelDlgButton_    { nullptr };
  QToolButton*      plotDlgButton_     { nullptr };
  QToolButton*      autoFitButton_     { nullptr };
  QToolButton*      leftButton_        { nullptr };
  QToolButton*      rightButton_       { nullptr };
  QStackedWidget*   controlsStack_     { nullptr };
  CQChartsModelDlg* modelDlg_          { nullptr };
  CQChartsPlotDlg*  plotDlg_           { nullptr };
};

#endif
