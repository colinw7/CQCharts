#ifndef CQChartsViewToolBar_H
#define CQChartsViewToolBar_H

#include <QFrame>

class CQChartsWindow;
class CQChartsView;
class CQChartsManageModelsDlg;
class CQIconCombo;
class QToolButton;
class QRadioButton;
class QCheckBox;
class QStackedWidget;

/*!
 * \brief Charts View ToolBar
 * \ingroup Charts
 */
class CQChartsViewToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewToolBar(CQChartsWindow *window);

  CQChartsWindow *window() const{ return window_; }

  CQChartsView *view() const;

  void updateMode();

  QSize sizeHint() const override;

 private slots:
  void updateState();

  void modeSlot(int ind);

  void selectButtonClicked(int);

  void selectInsideSlot(int state);

  void viewSettingsSlot(bool b);

  void viewTableSlot(bool b);

  void manageModelsSlot();

  void addPlotSlot();

  void zoomFullSlot();

  void panResetSlot();

  void autoFitSlot();

  void leftSlot();
  void rightSlot();

  void helpSlot();

 private:
  CQChartsWindow*          window_                { nullptr }; //!< parent window
  CQIconCombo*             modeCombo_             { nullptr }; //!< plot mode
  QRadioButton*            selectPointButton_     { nullptr }; //!< select point radio
  QRadioButton*            selectRectButton_      { nullptr }; //!< select rect radio
  QCheckBox*               selectInsideCheck_     { nullptr }; //!< select index checkbox
  QToolButton*             viewSettingsButton_    { nullptr }; //!< show/hide view settings button
  QToolButton*             viewTableButton_       { nullptr }; //!< show/hide view table button
  QToolButton*             manageModelsDlgButton_ { nullptr }; //!< load model dlg button
  QToolButton*             createPlotDlgButton_   { nullptr }; //!< create plot dlg button
  QToolButton*             autoFitButton_         { nullptr }; //!< auto fit button
  QToolButton*             leftButton_            { nullptr }; //!< scroll plots left button
  QToolButton*             rightButton_           { nullptr }; //!< scroll plots right button
  QStackedWidget*          controlsStack_         { nullptr }; //!< mode controls stack
  CQChartsManageModelsDlg* manageModelsDlg_       { nullptr }; //!< manage model dialog
};

#endif
