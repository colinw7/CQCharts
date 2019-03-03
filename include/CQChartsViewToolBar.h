#ifndef CQChartsViewToolBar_H
#define CQChartsViewToolBar_H

#include <QFrame>

class CQChartsWindow;
class CQChartsManageModelsDlg;
class CQChartsCreatePlotDlg;
class CQIconCombo;
class QToolButton;
class QRadioButton;
class QCheckBox;
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

  void selectInsideSlot(int state);

  void manageModelsSlot();

  void addPlotSlot();

  void zoomFullSlot();

  void panResetSlot();

  void autoFitSlot();

  void leftSlot();
  void rightSlot();

 private:
  CQChartsWindow*          window_                { nullptr }; //! parent window
  CQIconCombo*             modeCombo_             { nullptr }; //! plot mode
  QRadioButton*            selectPointButton_     { nullptr }; //! select point radio
  QRadioButton*            selectRectButton_      { nullptr }; //! select rect radio
  QCheckBox*               selectInsideCheck_     { nullptr }; //! selecy index checkbox
  QToolButton*             manageModelsDlgButton_ { nullptr }; //! load model dlg button
  QToolButton*             createPlotDlgButton_   { nullptr }; //! create plot dlg button
  QToolButton*             autoFitButton_         { nullptr }; //! auto fit button
  QToolButton*             leftButton_            { nullptr }; //! scroll plots left button
  QToolButton*             rightButton_           { nullptr }; //! scroll plots right button
  QStackedWidget*          controlsStack_         { nullptr }; //! mode controls stack
  CQChartsManageModelsDlg* manageModelsDlg_       { nullptr }; //! manage model dialog
  CQChartsCreatePlotDlg*   createPlotDlg_         { nullptr }; //! create plot dialog
};

#endif
